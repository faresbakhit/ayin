#include <filesystem>
#include <algorithm>

#include <ImageFilter.hpp>
#include <Application.hpp>
#include <MaterialIcons.hpp>
#include <MaterialIconsFont.hpp>
#include <OpenSansFont.hpp>
#include <Win32Theme.hpp>
#include <portable-file-dialogs.hpp>

using namespace bps;

Photo::Photo() = default;

void Photo::reset()
{
    bool newDataSize = image->width != origImage->width || image->height != origImage->height;
    if (newDataSize) {
        newDataSize = true;
        delete image;
        image = new Image(origImage->width, origImage->height, origImage->channels);
    }
    memcpy(image->data, origImage->data, origImage->width * origImage->height * origImage->channels);
    if (newDataSize) {
        image->load_texture();
    } else {
        image->update_texture();
    }
    m_undoPos = 0;
    m_undoStack.clear();
}

void Photo::soft_reset() {
    m_undoPos = 0;
    m_undoStack.clear();
}

void Photo::push_change(Commands::Info info) {
    if (m_undoPos != 0) {
        m_undoStack.resize(m_undoStack.size() - m_undoPos);
        m_undoPos = 0;
    }
    m_undoStack.push_back(info);
}

static void doCommand(Image& image, Commands::Info cmd)
{
    switch (cmd.ty)
    {
    case Commands::Type_Grayscale:
        ImageFilter::Grayscale(image);
        break;
    case Commands::Type_BlackAndWhite:
        ImageFilter::BlackAndWhite(image);
        break;
    case Commands::Type_Invert:
        ImageFilter::Invert(image);
        break;
    case Commands::Type_Merge: {
        Image* image2 = new Image();
        image2->load(cmd.merge_image);
        ImageFilter::Merge(image, *image2);
        delete image2;
        break;
    }
    case Commands::Type_FlipHorizontally:
        ImageFilter::FlipHorizontally(image);
        break;
    case Commands::Type_FlipVertically:
        ImageFilter::FlipVertically(image);
        break;
    case Commands::Type_Rotate:
        ImageFilter::Rotate(image);
        break;
    case Commands::Type_DarkenAndLighten:
        ImageFilter::ChangeBrightness(image, cmd.darkenlighten_factor);
        break;
    case Commands::Type_Crop:
        ImageFilter::Crop(image, cmd.crop_x, cmd.crop_y, cmd.crop_width, cmd.crop_height);
        break;
    case Commands::Type_Frame:
        ImageFilter::Frame(image, cmd.frame_fanciness, cmd.frame_color);
        break;
    case Commands::Type_DetectEdges:
        ImageFilter::DetectEdges(image);
        break;
    case Commands::Type_Resize:
        ImageFilter::Resize(image, cmd.resize_width, cmd.resize_height);
        break;
    case Commands::Type_Blur:
        ImageFilter::Blur(image, cmd.blur_level);
        break;
    case Commands::Type_Sunlight:
        ImageFilter::Sunlight(image);
        break;
    case Commands::Type_OilPaint:
        ImageFilter::OilPaint(image);
        break;
    case Commands::Type_Purple:
        ImageFilter::Purple(image);
        break;
    case Commands::Type_Infrared:
        ImageFilter::Infrared(image);
        break;
    case Commands::Type_Skew:
        ImageFilter::Skew(image, cmd.skew_angle);
        break;
    case Commands::Type_Glasses3D:
        ImageFilter::Glasses3D(image, cmd.darkenlighten_factor);
        break;
    case Commands::Type_MotionBlur:
        ImageFilter::MotionBlur(image, cmd.blur_level);
        break;
    case Commands::Type_Emboss:
        ImageFilter::Emboss(image);
        break;
    }
}

void Photo::undo_change()
{
    ++m_undoPos;

    bool newDataSize = image->width != origImage->width || image->height != origImage->height;
    if (newDataSize) {
        newDataSize = true;
        delete image;
        image = new Image(origImage->width, origImage->height, origImage->channels);
    }

    memcpy(image->data, origImage->data, origImage->width * origImage->height * origImage->channels);

    for (size_t i = 0; i < m_undoStack.size() - m_undoPos; ++i) {
        doCommand(*image, m_undoStack[i]);
    }

    if (newDataSize) {
        image->load_texture();
    } else {
        image->update_texture();
    }
}

bool Photo::can_undo_change() {
    return m_undoPos <= (int)m_undoStack.size() - 1;
}

void Photo::redo_change() {
    --m_undoPos;
    int width = image->width, height = image->height;
    doCommand(*image, m_undoStack[m_undoStack.size() - m_undoPos - 1]);
    if (width == image->width && height == image->height) {
        image->update_texture();
    } else {
        image->load_texture();
    }
}

bool Photo::can_redo_change() {
    return m_undoPos != 0;
}

Application::Application() = default;

bool Application::init(const char* title)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        return false;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    sdl_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL
            | SDL_WINDOW_RESIZABLE
            | SDL_WINDOW_MAXIMIZED
            | SDL_WINDOW_ALLOW_HIGHDPI);

    if (sdl_window == nullptr) {
        return false;
    }

    gl_context = SDL_GL_CreateContext(sdl_window);
    if (gl_context == NULL) {
        return false;
    }
    if (SDL_GL_MakeCurrent(sdl_window, gl_context) < 0) {
        return false;
    };
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->IniFilename = nullptr;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(sdl_window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    const float font_size = 18.0f;
    io->Fonts->AddFontFromMemoryCompressedTTF(
        OpenSansFont_compressed_data,
        OpenSansFont_compressed_size,
        font_size);
    static const ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = font_size * 2.0f / 3.0f; // font size (px)
    io->Fonts->AddFontFromMemoryCompressedTTF(
        MaterialIconsFont_compressed_data,
        MaterialIconsFont_compressed_size,
        icons_config.GlyphMinAdvanceX, // font size (px)
        &icons_config,
        icons_ranges);

#ifdef _WIN32
    windows_theme_timer_id = SDL_AddTimer(0, Win32Theme::TimerCallback, sdl_window);
#else
    ImGui::StyleColorsDark(style);
#endif

    return true;
}

Application::~Application()
{
    for (auto &&ph : photos)
    {
        delete ph->image;
        delete ph->origImage;
        delete ph;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(NULL);

#ifdef _WIN32
    SDL_RemoveTimer(windows_theme_timer_id);
#endif
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}

void Application::new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Application::open_file_dialog()
{
    auto selection = pfd::OpenFile("Open", "", pfdImageFile, pfd::Option::multiselect).result();
    for (auto it = selection.begin(); it != selection.end(); ++it) {
        Image* image = new Image();
        std::string name = std::filesystem::path(*it).filename().string();
        std::ostringstream name_suffix {};
        int name_suffix_i = 1;
        image->load(it->c_str());
        image->load_texture();

        auto pred = [&](Photo* photo) { return photo->filename == name + name_suffix.str(); };

        while (std::find_if(photos.begin(), photos.end(), pred) != photos.end()) {
            name_suffix.str("");
            name_suffix << " (" << name_suffix_i++ << ")";
        }

        Image* origImage = new Image(image->width, image->height, image->channels);
        memcpy(origImage->data, image->data, image->width * image->height * image->channels);

        Photo* photo = new Photo;
        photo->image = image;
        photo->origImage = origImage;
        photo->filename = name + name_suffix.str();
        photo->filepath = *it;
        photos.push_back(photo);
    }
}

void Application::save_file_dialog(Photo& photo)
{
    auto selection = pfd::SaveFile("Save As...", "", pfdImageFile, pfd::Option::none).result();
    if (selection.empty()) {
        return;
    }
    if (photo.image->save(selection.c_str())) {
        pfd::Notify("Error: Save", "An error happened", pfd::Icon::error);
    }
}

InputRequest Application::handle_input()
{
    static bool zoomin = false;
    static bool zoomout = false;
    SDL_Event event;
    Uint32 window_flags = SDL_GetWindowFlags(sdl_window);
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            done = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_F11) {
                if ((window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP) {
                    SDL_SetWindowFullscreen(sdl_window, 0);
                } else {
                    SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
            } else if (event.key.keysym.mod & KMOD_CTRL) {
                if (event.key.keysym.sym == SDLK_o) {
                    open_file_dialog();
                } else if (event.key.keysym.mod & KMOD_SHIFT  && (event.key.keysym.sym == SDLK_s)) {
                    return InputRequest(InputRequest_SaveAs);
                } else if (event.key.keysym.sym == SDLK_s) {
                    return InputRequest(InputRequest_Save);
                } else if (event.key.keysym.sym == SDLK_q) {
                    done = true;
                } else if (event.key.keysym.sym == SDLK_z) {
                    return InputRequest(InputRequest_Undo);
                } else if (event.key.keysym.sym == SDLK_y) {
                    return InputRequest(InputRequest_Redo);
                } else if (event.key.keysym.sym == SDLK_EQUALS) {
                    zoomin = true;
                } else if (event.key.keysym.sym == SDLK_MINUS) {
                    zoomout = true;
                } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                    return InputRequest(InputRequest_SwitchTab, event.key.keysym.sym - SDLK_0);
                }
            }
            break;
        case SDL_KEYUP:
            if (event.key.keysym.mod & SDLK_LCTRL) {
                if (event.key.keysym.sym == SDLK_EQUALS) {
                    zoomin = false;
                } else if (event.key.keysym.sym == SDLK_MINUS) {
                    zoomout = false;
                }
            }
            break;
        default:
            break;
        }
    }

    if (zoomin) {
        return InputRequest(InputRequest_ZoomIn);
    }

    if (zoomout) {
        return InputRequest(InputRequest_ZoomOut);
    }

    return InputRequest(InputRequest_None);
}

void Application::render()
{
    ImGui::Render();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    ImVec4 clear_color = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdl_window);
}

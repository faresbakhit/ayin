#include "Application.hpp"
#include "ImageFilter.hpp"
#include "fonts/MaterialIcons.hpp"
#include "fonts/MaterialIconsFont.hpp"
#include "fonts/OpenSansFont.hpp"
#include "utils/win32.hpp"

#include <algorithm>
#include <filesystem>

#include <SDL_syswm.h>
#include <portable-file-dialogs.hpp>

using namespace ayin;

#ifdef _WIN32
static Uint32 sdl_timer_callback(Uint32, void *sdl_window) {
	static int is_light = -1;
	switch (is_light) {
	case -1: // init
		is_light = utils::win32::use_light_theme();
		goto change;
	case 0: // false
		if (utils::win32::use_light_theme()) {
			is_light = true;
			goto change;
		}
		break;
	case 1: // true
		if (!utils::win32::use_light_theme()) {
			is_light = false;
			goto change;
		}
		break;
	}
	return 5000;
change:
	is_light ? ImGui::StyleColorsLight() : ImGui::StyleColorsDark();
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo((SDL_Window *)sdl_window, &wmInfo);
	BOOL USE_DARK_MODE = !is_light;
	DwmSetWindowAttribute((HWND)wmInfo.info.win.window, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
						  &USE_DARK_MODE, sizeof(USE_DARK_MODE));
	return 5000;
}
#endif

bool Application::init(const char *title) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		return false;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char *glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char *glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
						SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
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
	sdl_window =
		SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
						 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_ALLOW_HIGHDPI);

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
	io->Fonts->AddFontFromMemoryCompressedTTF(OpenSansFont_compressed_data, OpenSansFont_compressed_size, font_size);
	static const ImWchar icons_ranges[] = {ICON_MIN_MD, ICON_MAX_16_MD, 0};
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = font_size * 2.0f / 3.0f; // font size (px)
	io->Fonts->AddFontFromMemoryCompressedTTF(MaterialIconsFont_compressed_data, MaterialIconsFont_compressed_size,
											  icons_config.GlyphMinAdvanceX, // font size (px)
											  &icons_config, icons_ranges);

#ifdef _WIN32
	windows_theme_timer_id = SDL_AddTimer(0, sdl_timer_callback, sdl_window);
#else
	ImGui::StyleColorsDark(style);
#endif

	return true;
}

Application::~Application() {
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

void Application::new_frame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void Application::open_file_dialog() {
	auto selection = pfd::OpenFile("Open", "", pfdImageFile, pfd::Option::multiselect).result();
	for (auto it = selection.begin(); it != selection.end(); ++it) {
		Image *image = new Image();
		std::string name = std::filesystem::path(*it).filename().string();
		std::ostringstream name_suffix{};
		int name_suffix_i = 1;
		image->load(it->c_str());
		image->load_texture();

		auto pred = [&](Photo *photo) { return photo->filename == name + name_suffix.str(); };

		while (std::find_if(photos.begin(), photos.end(), pred) != photos.end()) {
			name_suffix.str("");
			name_suffix << " (" << name_suffix_i++ << ")";
		}

		Image *origImage = new Image(image->width, image->height, image->channels);
		memcpy(origImage->data, image->data, image->width * image->height * image->channels);

		std::unique_ptr<Photo> photo = std::make_unique<Photo>();
		photo->image = image;
		photo->origImage = origImage;
		photo->filename = name + name_suffix.str();
		photo->filepath = *it;
		photos.push_back(std::move(photo));
	}
}

void Application::save_file_dialog(Photo &photo) {
	auto selection = pfd::SaveFile("Save As...", "", pfdImageFile, pfd::Option::none).result();
	if (selection.empty()) {
		return;
	}
	if (photo.image->save(selection.c_str())) {
		pfd::Notify("Error: Save", "An error happened", pfd::Icon::error);
	}
}

InputRequest Application::handle_input() {
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
				} else if (event.key.keysym.sym == SDLK_w) {
					return InputRequest(InputRequest_CloseTab);
				} else if (event.key.keysym.mod & KMOD_SHIFT && (event.key.keysym.sym == SDLK_s)) {
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

void Application::render() {
	ImGui::Render();
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	ImVec4 clear_color = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
				 clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(sdl_window);
}

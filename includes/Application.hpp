#pragma once

#include <Commands.hpp>
#include <Image.hpp>
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>
#include <imgui_impl_sdl2.h>
#include <string>
#include <vector>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include <SDL_opengl.h>

namespace bps {

const std::vector<std::string> pfdImageFile = {
    "All Picture Files (*.bmp;*.jpg;*.jpeg;*.png;*.psd)",
    "*.bmp *.dib *.jpg *.jpeg *.jpe *.jfif *.gif *.png *.psd"
};

enum InputRequest_ {
    InputRequest_ZoomIn,
    InputRequest_ZoomOut,
    InputRequest_SwitchTab,
    InputRequest_Save,
    InputRequest_SaveAs,
    InputRequest_Undo,
    InputRequest_Redo,
    InputRequest_None,
};

struct InputRequest {
    InputRequest_ ty;
    union {
        int tab_number;
    };
    InputRequest(InputRequest_ ty)
        : ty(ty)
    {
    }
    InputRequest(InputRequest_ ty, int tab_number)
        : ty(ty)
        , tab_number(tab_number)
    {
    }
};

class Photo
{
public:
    Image* image = nullptr;
    Image* origImage = nullptr;
    std::string filename {};
    std::string filepath {};
    float x = 0.0f,
          y = 0.0f,
          zoom = 1.0f;

    Photo();
    void reset();
    void soft_reset();
    void push_change(Commands::Info info);
    void undo_change();
    bool can_undo_change();
    void redo_change();
    bool can_redo_change();

private:
    std::vector<Commands::Info> m_undoStack {};
    int m_undoPos = 0;
};

class Application {
public:
    ImGuiIO* io = nullptr;
    std::vector<Photo*> photos;
    bool done = false;
    Application();
    ~Application();
    bool init(const char* title);
    void new_frame();
    void open_file_dialog();
    void save_file_dialog(Photo& photo);
    void render();
    InputRequest handle_input();

private:
    SDL_GLContext gl_context = nullptr;
    SDL_Window* sdl_window = nullptr;
#ifdef _WIN32
    SDL_TimerID windows_theme_timer_id = 0;
#endif
};
} // namespace

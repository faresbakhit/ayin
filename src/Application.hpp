#pragma once

#include "Commands.hpp"
#include "Image.hpp"
#include "Photo.hpp"

#include <string>
#include <vector>
#include <memory>

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>
#include <imgui_impl_sdl2.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include <SDL_opengl.h>

namespace ayin {

const std::vector<std::string> pfdImageFile = {"All Picture Files (*.bmp;*.jpg;*.jpeg;*.png;*.psd)",
											   "*.bmp *.dib *.jpg *.jpeg *.jpe *.jfif *.gif *.png *.psd"};

enum InputRequest_ {
	InputRequest_ZoomIn,
	InputRequest_ZoomOut,
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
	InputRequest(InputRequest_ ty) : ty(ty) {}
	InputRequest(InputRequest_ ty, int tab_number) : ty(ty), tab_number(tab_number) {}
};

class Application {
  public:
	std::vector<std::unique_ptr<Photo>> photos;

	ImGuiIO *io = nullptr;
	bool done = false;
	Application() = default;
	~Application();
	bool init(const char *title);
	void new_frame();
	Photo* get_selected_photo();
	void set_selected_photo(size_t index);
	void open_file_dialog();
	void save_file_dialog(Photo &photo);
	void render();
	InputRequest handle_input();

  private:
	SDL_GLContext gl_context = nullptr;
	SDL_Window *sdl_window = nullptr;
#ifdef _WIN32
	SDL_TimerID windows_theme_timer_id = 0;
#endif
	size_t m_selectedPhotoIndex = 0;
};
} // namespace ayin

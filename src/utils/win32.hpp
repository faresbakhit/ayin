#pragma once

#include <dwmapi.h>
#ifndef __MINGW32__
#include <Windows.h>
#else
#include <windows.h>
#endif // __MINGW32_

#include <SDL2/SDL_syswm.h>
#include <imgui.h>

#include <vector>

#ifndef AYIN_UTILS_WIN32_AUTO_IMGUI_STYLE_COLORS_SDL_TIMER_CALLBACK_TIMEOUT
#define AYIN_UTILS_WIN32_AUTO_IMGUI_STYLE_COLORS_SDL_TIMER_CALLBACK_TIMEOUT 5000
#endif

namespace ayin::utils::win32 {

static bool AppsUseLightTheme() {
	auto buffer = std::vector<char>(4);
	auto cbData = static_cast<DWORD>(buffer.size() * sizeof(char));
	auto res = RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
							L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, buffer.data(), &cbData);
	if (res != ERROR_SUCCESS)
		return -1;
	auto i = int(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
	return i == 1;
}

static Uint32 AutoImGuiStyleColorsSDLTimerCallback(Uint32, void *sdl_window) {
	static int is_light = -1;
	switch (is_light) {
	case -1: // init
		is_light = AppsUseLightTheme();
		goto change;
	case 0: // false
		if (AppsUseLightTheme()) {
			is_light = true;
			goto change;
		}
		break;
	case 1: // true
		if (!AppsUseLightTheme()) {
			is_light = false;
			goto change;
		}
		break;
	}
	return AYIN_UTILS_WIN32_AUTO_IMGUI_STYLE_COLORS_SDL_TIMER_CALLBACK_TIMEOUT;
change:
	is_light ? ImGui::StyleColorsLight() : ImGui::StyleColorsDark();
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo((SDL_Window *)sdl_window, &wmInfo);
	BOOL USE_DARK_MODE = !is_light;
	DwmSetWindowAttribute((HWND)wmInfo.info.win.window, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
						  &USE_DARK_MODE, sizeof(USE_DARK_MODE));
	return AYIN_UTILS_WIN32_AUTO_IMGUI_STYLE_COLORS_SDL_TIMER_CALLBACK_TIMEOUT;
}
} // namespace ayin::utils::win32

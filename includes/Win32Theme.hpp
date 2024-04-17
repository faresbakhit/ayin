#pragma once

#ifdef _WIN32
#ifndef __MINGW32__
#include <Windows.h>
#else
#include <windows.h>
#endif // __MINGW32_
#include <vector>
#include <dwmapi.h>
#include <SDL_syswm.h>
#include <imgui.h>

#ifndef BPS_WIN32THEME_CHECK_INTERVAL
#define BPS_WIN32THEME_CHECK_INTERVAL 5000
#endif

namespace bps::Win32Theme
{
    static bool UseLight() {
        auto buffer = std::vector<char>(4);
        auto cbData = static_cast<DWORD>(buffer.size() * sizeof(char));
        auto res = RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            RRF_RT_REG_DWORD,
            nullptr,
            buffer.data(),
            &cbData);
        if (res != ERROR_SUCCESS)
            return -1;
        auto i = int(buffer[3] << 24 |
            buffer[2] << 16 |
            buffer[1] << 8 |
            buffer[0]);
        return i == 1;
    }

    static Uint32 TimerCallback(Uint32, void *sdl_window) {
        static int is_light = -1;
        switch (is_light) {
        case -1: // init
            is_light = UseLight();
            goto change;
        case 0: // false
            if (UseLight()) {
                is_light = true;
                goto change;
            }
            break;
        case 1: // true
            if (!UseLight()) {
                is_light = false;
                goto change;
            }
            break;
        }
        return BPS_WIN32THEME_CHECK_INTERVAL;
    change:
        is_light ? ImGui::StyleColorsLight() : ImGui::StyleColorsDark();
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo((SDL_Window*)sdl_window, &wmInfo);
        BOOL USE_DARK_MODE = !is_light;
        DwmSetWindowAttribute(
            (HWND)wmInfo.info.win.window, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
            &USE_DARK_MODE, sizeof(USE_DARK_MODE));
        return BPS_WIN32THEME_CHECK_INTERVAL;
    }
}
#endif // _WIN32

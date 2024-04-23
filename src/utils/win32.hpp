#pragma once

#ifdef _WIN32

#include <dwmapi.h>
#include <vector>
#ifndef __MINGW32__
#include <Windows.h>
#else
#include <windows.h>
#endif // __MINGW32_

namespace ayin::utils::win32 {
static bool use_light_theme() {
	auto buffer = std::vector<char>(4);
	auto cbData = static_cast<DWORD>(buffer.size() * sizeof(char));
	auto res = RegGetValueW(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, buffer.data(),
		&cbData);
	if (res != ERROR_SUCCESS)
		return -1;
	auto i =
		int(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
	return i == 1;
}
} // namespace ayin::utils::win32
#endif // _WIN32

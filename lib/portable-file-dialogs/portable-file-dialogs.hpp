//
//  Portable File Dialogs
//
//  Copyright © 2018–2022 Sam Hocevar <sam@hocevar.net>
//
//  This library is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

#if _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h> // IFileDialog
#include <shellapi.h>
#include <strsafe.h>
#include <future>     // std::async
#include <userenv.h>  // GetUserProfileDirectory()

#elif __EMSCRIPTEN__
#include <emscripten.h>

#else
#ifndef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 2 // for popen()
#endif
#ifdef __APPLE__
#   ifndef _DARWIN_C_SOURCE
#       define _DARWIN_C_SOURCE
#   endif
#endif
#include <cstdio>     // popen()
#include <cstdlib>    // std::getenv()
#include <fcntl.h>    // fcntl()
#include <unistd.h>   // read(), pipe(), dup2(), getuid()
#include <csignal>    // ::kill, std::signal
#include <sys/stat.h> // stat()
#include <sys/wait.h> // waitpid()
#include <pwd.h>      // getpwnam()
#endif

#include <cstdint>  // std::uint8_t
#include <string>   // std::string
#include <memory>   // std::shared_ptr
#include <iostream> // std::ostream
#include <map>      // std::map
#include <set>      // std::set
#include <regex>    // std::regex
#include <thread>   // std::mutex, std::this_thread
#include <chrono>   // std::chrono

// Versions of mingw64 g++ up to 9.3.0 do not have a complete IFileDialog
#ifndef PFD_HAS_IFILEDIALOG
#   define PFD_HAS_IFILEDIALOG 1
#   if (defined __MINGW64__ || defined __MINGW32__) && defined __GXX_ABI_VERSION
#       if __GXX_ABI_VERSION <= 1013
#           undef PFD_HAS_IFILEDIALOG
#           define PFD_HAS_IFILEDIALOG 0
#       endif
#   endif
#endif

namespace pfd
{

enum class button
{
    cancel = -1,
    ok,
    yes,
    no,
    abort,
    retry,
    ignore,
};

enum class choice
{
    ok = 0,
    ok_cancel,
    yes_no,
    yes_no_cancel,
    retry_cancel,
    abort_retry_ignore,
};

enum class icon
{
    info = 0,
    warning,
    error,
    question,
};

// Additional option flags for various dialog constructors
enum class opt : std::uint8_t
{
    none = 0,
    // For file open, allow multiselect.
    multiselect     = 0x1,
    // For file save, force overwrite and disable the confirmation dialog.
    force_overwrite = 0x2,
    // For folder select, force path to be the provided argument instead
    // of the last opened directory, which is the Microsoft-recommended,
    // user-friendly behaviour.
    force_path      = 0x4,
};

inline opt operator |(opt a, opt b) { return opt(std::uint8_t(a) | std::uint8_t(b)); }
inline bool operator &(opt a, opt b) { return bool(std::uint8_t(a) & std::uint8_t(b)); }

// The settings class, only exposing to the user a way to set verbose mode
// and to force a rescan of installed desktop helpers (zenity, kdialog…).
class settings
{
public:
    static bool available();

    static void verbose(bool value);
    static void rescan();

protected:
    explicit settings(bool resync = false);

    bool check_program(std::string const &program);

    inline bool is_osascript() const;
    inline bool is_zenity() const;
    inline bool is_kdialog() const;

    enum class flag
    {
        is_scanned = 0,
        is_verbose,

        has_zenity,
        has_matedialog,
        has_qarma,
        has_kdialog,
        is_vista,

        max_flag,
    };

    // Static array of flags for internal state
    bool const &flags(flag in_flag) const;

    // Non-const getter for the static array of flags
    bool &flags(flag in_flag);
};

// Internal classes, not to be used by client applications
namespace internal
{

// Process wait timeout, in milliseconds
static int const default_wait_timeout = 20;

class executor
{
    friend class dialog;

public:
    // High level function to get the result of a command
    std::string result(int *exit_code = nullptr);

    // High level function to abort
    bool kill();

#if _WIN32
    void start_func(std::function<std::string(int *)> const &fun);
    static BOOL CALLBACK enum_windows_callback(HWND hwnd, LPARAM lParam);
#elif __EMSCRIPTEN__
    void start(int exit_code);
#else
    void start_process(std::vector<std::string> const &command);
#endif

    ~executor();

protected:
    bool ready(int timeout = default_wait_timeout);
    void stop();

private:
    bool m_running = false;
    std::string m_stdout;
    int m_exit_code = -1;
#if _WIN32
    std::future<std::string> m_future;
    std::set<HWND> m_windows;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    DWORD m_tid;
#elif __EMSCRIPTEN__ || __NX__
    // FIXME: do something
#else
    pid_t m_pid = 0;
    int m_fd = -1;
#endif
};

class platform
{
protected:
#if _WIN32
    // Helper class around LoadLibraryA() and GetProcAddress() with some safety
    class dll
    {
    public:
        dll(std::string const &name);
        ~dll();

        template<typename T> class proc
        {
        public:
            proc(dll const &lib, std::string const &sym)
              : m_proc(reinterpret_cast<T *>((void *)::GetProcAddress(lib.handle, sym.c_str())))
            {}

            operator bool() const { return m_proc != nullptr; }
            operator T *() const { return m_proc; }

        private:
            T *m_proc;
        };

    private:
        HMODULE handle;
    };

    // Helper class around CoInitialize() and CoUnInitialize()
    class ole32_dll : public dll
    {
    public:
        ole32_dll();
        ~ole32_dll();
        bool is_initialized();

    private:
        HRESULT m_state;
    };

    // Helper class around CreateActCtx() and ActivateActCtx()
    class new_style_context
    {
    public:
        new_style_context();
        ~new_style_context();

    private:
        HANDLE create();
        ULONG_PTR m_cookie = 0;
    };
#endif
};

class dialog : protected settings, protected platform
{
public:
    bool ready(int timeout = default_wait_timeout) const;
    bool kill() const;

protected:
    explicit dialog();

    std::vector<std::string> desktop_helper() const;
    static std::string buttons_to_name(choice _choice);
    static std::string get_icon_name(icon _icon);

    std::string powershell_quote(std::string const &str) const;
    std::string osascript_quote(std::string const &str) const;
    std::string shell_quote(std::string const &str) const;

    // Keep handle to executing command
    std::shared_ptr<executor> m_async;
};

class file_dialog : public dialog
{
protected:
    enum type
    {
        open,
        save,
        folder,
    };

    file_dialog(type in_type,
                std::string const &title,
                std::string const &default_path = "",
                std::vector<std::string> const &filters = {},
                opt options = opt::none);

protected:
    std::string string_result();
    std::vector<std::string> vector_result();

#if _WIN32
    static int CALLBACK bffcallback(HWND hwnd, UINT uMsg, LPARAM, LPARAM pData);
#if PFD_HAS_IFILEDIALOG
    std::string select_folder_vista(IFileDialog *ifd, bool force_path);
#endif

    std::wstring m_wtitle;
    std::wstring m_wdefault_path;

    std::vector<std::string> m_vector_result;
#endif
};

} // namespace internal

//
// The path class provides some platform-specific path constants
//

class path : protected internal::platform
{
public:
    static std::string home();
    static std::string separator();
};

//
// The notify widget
//

class notify : public internal::dialog
{
public:
    notify(std::string const &title,
           std::string const &message,
           icon _icon = icon::info);
};

//
// The message widget
//

class message : public internal::dialog
{
public:
    message(std::string const &title,
            std::string const &text,
            choice _choice = choice::ok_cancel,
            icon _icon = icon::info);

    button result();

private:
    // Some extra logic to map the exit code to button number
    std::map<int, button> m_mappings;
};

//
// The open_file, save_file, and open_folder widgets
//

class open_file : public internal::file_dialog
{
public:
    open_file(std::string const &title,
              std::string const &default_path = "",
              std::vector<std::string> const &filters = { "All Files", "*" },
              opt options = opt::none);

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
    // Backwards compatibility
    [[deprecated("Use pfd::opt::multiselect instead of allow_multiselect")]]
#endif
#endif
    open_file(std::string const &title,
              std::string const &default_path,
              std::vector<std::string> const &filters,
              bool allow_multiselect);

    std::vector<std::string> result();
};

class save_file : public internal::file_dialog
{
public:
    save_file(std::string const &title,
              std::string const &default_path = "",
              std::vector<std::string> const &filters = { "All Files", "*" },
              opt options = opt::none);

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
    // Backwards compatibility
    [[deprecated("Use pfd::opt::force_overwrite instead of confirm_overwrite")]]
#endif
#endif
    save_file(std::string const &title,
              std::string const &default_path,
              std::vector<std::string> const &filters,
              bool confirm_overwrite);

    std::string result();
};

class select_folder : public internal::file_dialog
{
public:
    select_folder(std::string const &title,
                  std::string const &default_path = "",
                  opt options = opt::none);

    std::string result();
};

} // namespace pfd

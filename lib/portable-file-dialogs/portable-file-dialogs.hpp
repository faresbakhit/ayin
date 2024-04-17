#pragma once

#include <string>
#include <vector>
#include <memory>

namespace pfd
{
    class notify;
    class message;
    class open_file;
    class save_file;
    class select_folder;

    enum class Button
    {
        cancel = -1,
        ok,
        yes,
        no,
        abort,
        retry,
        ignore,
    };

    enum class Choice
    {
        ok = 0,
        ok_cancel,
        yes_no,
        yes_no_cancel,
        retry_cancel,
        abort_retry_ignore,
    };

    enum class Icon
    {
        info = 0,
        warning,
        error,
        question,
    };

    enum class Option : uint8_t
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

    inline Option operator |(Option a, Option b) { return Option(uint8_t(a) | uint8_t(b)); }
    inline bool operator &(Option a, Option b) { return bool(uint8_t(a) & uint8_t(b)); }


    class Settings
    {
    public:
        static int const default_wait_timeout = 20;
        static bool available();
        static void verbose(bool value);
        static void rescan();
    };

    class Notify
    {
    public:
        Notify(std::string const &title, std::string const &message, Icon icon = Icon::info);
        ~Notify();
        bool ready(int timeout = Settings::default_wait_timeout) const;
        bool kill() const;
    private:
        notify* impl;
    };

    class Message
    {
    public:
        Message(std::string const &title, std::string const &text, Choice choice = Choice::ok_cancel, Icon icon = Icon::info);
        ~Message();
        bool ready(int timeout = Settings::default_wait_timeout) const;
        bool kill() const;
        Button result() const;
    private:
        message* impl;
    };

    class OpenFile
    {
    public:
        OpenFile(std::string const &title,
                std::string const &default_path = "",
                std::vector<std::string> const &filters = { "All Files", "*" },
                Option options = Option::none);
        ~OpenFile();
        bool ready(int timeout = Settings::default_wait_timeout) const;
        bool kill() const;
        std::vector<std::string> result() const;
    private:
        open_file* impl;
    };

    class SaveFile
    {
    public:
        SaveFile(std::string const &title,
                std::string const &default_path = "",
                std::vector<std::string> const &filters = { "All Files", "*" },
                Option options = Option::none);
        ~SaveFile();
        bool ready(int timeout = Settings::default_wait_timeout) const;
        bool kill() const;
        std::string result() const;
    private:
        save_file* impl;
    };

    class SelectFolder
    {
    public:
        SelectFolder(std::string const &title,
                    std::string const &default_path = "",
                    Option options = Option::none);
        ~SelectFolder();
        bool ready(int timeout = Settings::default_wait_timeout) const;
        bool kill() const;
        std::string result() const;
    private:
        select_folder* impl;
    };
}
#include "portable-file-dialogs.hpp"
#include "portable-file-dialogs-impl.h"

#include <cstdint>
#include <memory>

bool pfd::Settings::available()                     { return settings::available(); }
void pfd::Settings::verbose(bool value)             { settings::verbose(value); }
void pfd::Settings::rescan()                        { settings::rescan(); }


pfd::Notify::Notify(
    std::string const &title,
    std::string const &message,
    Icon t_icon)
    : impl{new pfd::notify{title, message, icon(int(t_icon))}} {}

pfd::Notify::~Notify() { delete impl; }

bool pfd::Notify::ready(int timeout) const          { return impl->ready(timeout); }
bool pfd::Notify::kill() const                      { return impl->kill(); }


pfd::Message::Message(
    std::string const &title, 
    std::string const &text, 
    Choice t_choice, 
    Icon t_icon)
    : impl{new pfd::message{title, text, choice(int(t_choice)), icon(int(t_icon))}} {}

pfd::Message::~Message() { delete impl; }

bool pfd::Message::ready(int timeout) const         { return impl->ready(timeout); }
bool pfd::Message::kill() const                     { return impl->kill(); }
pfd::Button pfd::Message::result() const            { return Button(int(impl->result())); }


pfd::OpenFile::OpenFile(std::string const &title,
    std::string const &default_path,
    std::vector<std::string> const &filters,
    Option options)
    : impl{new open_file{title, default_path, filters, opt(uint8_t(options))}} {}

pfd::OpenFile::~OpenFile() { delete impl; }

bool pfd::OpenFile::ready(int timeout) const            { return impl->ready(timeout); }
bool pfd::OpenFile::kill() const                        { return impl->kill(); }
std::vector<std::string> pfd::OpenFile::result() const  { return impl->result(); }


pfd::SaveFile::SaveFile(
    std::string const &title,
    std::string const &default_path,
    std::vector<std::string> const &filters,
    Option options)
    : impl{new save_file{title, default_path, filters, opt(uint8_t(options))}} {}

pfd::SaveFile::~SaveFile() { delete impl; }

bool pfd::SaveFile::ready(int timeout) const        { return impl->ready(timeout); }
bool pfd::SaveFile::kill() const                    { return impl->kill(); }
std::string pfd::SaveFile::result() const           { return impl->result(); }


pfd::SelectFolder::SelectFolder(
    std::string const &title,
    std::string const &default_path,
    Option options)
    : impl{new select_folder{title, default_path, opt(uint8_t(options))}} {}

pfd::SelectFolder::~SelectFolder() { delete impl; }

bool pfd::SelectFolder::ready(int timeout) const    { return impl->ready(timeout); }
bool pfd::SelectFolder::kill() const                { return impl->kill(); }
std::string pfd::SelectFolder::result() const       { return impl->result(); }

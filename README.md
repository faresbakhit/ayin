# Ayin Photo Editor

Ayin (عَيْن, [ʕajn]), Arabic for "eye", is a free and open-source photo editing software.

## Building

Ayin is written in C++23 with [modules](https://en.cppreference.com/w/cpp/language/modules.html), so you'll need to have a "recent" version of your compiler of choice, specifically:

- MSVC toolset 14.34 and newer (provided with Visual Studio 17.4 and newer)
- LLVM/Clang 16.0 and newer
- GCC 14 (for the in-development branch, after 2023-09-20) and newer

> from https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#compiler-support

The minimum required CMake version is 3.28.

Dependencies are automatically managed with [vcpkg](https://github.com/microsoft/vcpkg), but you can also use your distribution's package manager to install them. They will be typically listed under `*-devel`. Here are they:

- [FLTK](https://www.fltk.org) 1.3.11

Set up vcpkg (not needed if you've installed the dependencies with your package manager):

```sh
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg && ./bootstrap-vcpkg.sh
```

Clone Ayin, and build it in release mode:

```sh
git clone http://github.com/faresbakhit/ayin
cd ayin
cmake --preset release
cmake --build --preset release
```

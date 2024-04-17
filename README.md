# Ayin

![](./.github/ayin.png)

## Features

- [x] Multiple image tabs
- [x] Undo, and redo stack
- [x] Follow device theme (Works on Win32)

## Build on Windows/MinGW-w64

1. Install [MSYS2](https://www.msys2.org/)
2. Open `MSYS2 UCRT64`
3. Install [gcc](https://gcc.gnu.org/) a C/C++ Compiler, [SDL2](https://www.libsdl.org/) the window handling library, and [FeeType](https://freetype.org/) the font renderer:
4. Build (debug build by default)
```
$ pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-freetype
```

### Configure MSYS2 UCRT64 for VSCode

1. Ctrl+Shift+P
2. Preferences: Open User Settings (JSON)
3. Copy this inside of your `settings.json`
```json
    "terminal.integrated.profiles.windows": {
        "MSYS2 UCRT": {
            "path": "cmd.exe",
            "args": [
                "/c",
                "C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64"
            ]
        }
    }
```

# Ayin – Photo Editing Software

![Ayin interface showcase](./misc/readme/ayin.png)

Ayin (عَيْن, [\[ʕajn\]](https://en.wikipedia.org/wiki/Help:IPA/Arabic)), Arabic for "eye", is a free and open-source photo editing software I developed in part of a contest in my first year at FCAI-CU and got 1st place in the solo competition. Ayin is very simplistic compared to big corporate software™ but it has some cool [features](#features) and gets the job done in terms of functionality implemented. A pre-compiled version for Windows is available in the [releases](https://github.com/faresbakhit/ayin/releases/) page, but I urge you to compile it yourself and hack your own version of Ayin with your own patches.

![Photo of a framer with the text 'it ain't much' on top, and the text 'but it's honest work' on the bottom.](./misc/readme/honest-work.jpg)

## Features

- [x] 21+ Filters
- [x] Multiple image tabs
- [x] Undo and redo stack
- [x] Follow device theme
 
## Build on Windows/MinGW-w64

1. Install [MSYS2](https://www.msys2.org/)
2. Open `MSYS2 UCRT64`
3. Install dependencies:
```
$ pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-freetype
```
5. Build
```
$ cd /path/to/ayin/
$ make mode=release
$ ./target/release/ayin.exe
```

### Configure MSYS2 UCRT64 for VSCode

1. Ctrl+Shift+P
2. Preferences: Open User Settings (JSON)
3. Copy this to of your `settings.json`
```jsonc
{
    // ...
    "terminal.integrated.profiles.windows": {
        "MSYS2 UCRT": {
            "path": "cmd.exe",
            "args": [
                "/c",
                "C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64"
            ]
        }
    }
    // ...
}
```

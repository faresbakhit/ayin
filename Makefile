mode = debug

CXXFLAGS = -Ilib/imgui -Ilib/imgui/backends -Ilib/imgui/misc/freetype -Ilib/portable-file-dialogs -Ilib/stb
CXXFLAGS += -std=c++17 -Wall -Wextra -Wno-missing-field-initializers -Wno-missing-braces

ifeq ($(mode),debug)
 BUILDDIR = target/debug
 CXXFLAGS += -g -O0
else
 ifeq ($(mode),release)
  BUILDDIR = target/release
  CXXFLAGS += -O3 -DNDEBUG
 else
  ifeq ($(mode),dev-release)
   BUILDDIR = target/dev-release
   CXXFLAGS += -g -O2
  else
   $(error unknown mode: $(mode))
  endif
 endif
endif

exe := $(BUILDDIR)/ayin

AYIN_SOURCES = src/Application.cpp src/Commands.cpp src/Image.cpp src/ImageFilter.cpp src/Photo.cpp src/Main.cpp
# for `make format`
AYIN_HEADERS = src/Application.hpp src/Commands.hpp src/Image.hpp src/ImageFilter.hpp src/Photo.hpp src/utils/win32.hpp

EXTERN_SOURCES = lib/imgui/imgui.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp # ImGui
EXTERN_SOURCES += lib/imgui/misc/freetype/imgui_freetype.cpp # ImGui FreeType
EXTERN_SOURCES += lib/imgui/backends/imgui_impl_sdl2.cpp lib/imgui/backends/imgui_impl_opengl3.cpp # ImGui (SDL2 + OpenGL3) Backend
EXTERN_SOURCES += lib/portable-file-dialogs/portable-file-dialogs.cpp # Portable File Dialogs

AYIN_OBJECTS = $(addprefix $(BUILDDIR)/ayin/, $(addsuffix .o, $(basename $(notdir $(AYIN_SOURCES)))))
EXTERN_OBJECTS = $(addprefix $(BUILDDIR)/extern/, $(addsuffix .o, $(basename $(notdir $(EXTERN_SOURCES)))))

ifeq ($(OS),Windows_NT)
 exe := $(exe).exe
 LDFLAGS += -mwindows -lopengl32 -luuid -ldwmapi
 ifeq ($(mode),debug)
  LDFLAGS += -mconsole `pkg-config --libs sdl2 freetype2`
 else
  LDFLAGS += -static `pkg-config --static --libs sdl2 freetype2`
 endif
 CXXFLAGS += `pkg-config --cflags sdl2 freetype2`
 AYIN_OBJECTS += $(BUILDDIR)/ayin/icon.o
else
 $(error unsupported platform: $(mode))
endif

$(exe): $(BUILDDIR) $(AYIN_OBJECTS) $(EXTERN_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/ayin $(BUILDDIR)/extern

$(BUILDDIR)/ayin/%.o:src/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

ifeq ($(OS),Windows_NT)
$(BUILDDIR)/ayin/icon.o:misc/icon/icon.rc
	windres $^ $@
endif

$(BUILDDIR)/extern/%.o:lib/portable-file-dialogs/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/extern/%.o:lib/imgui/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/extern/%.o:lib/imgui/backends/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/extern/%.o:lib/imgui/misc/freetype/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: all format clean clean-ayin clean-extern

all: $(BUILDDIR) $(exe)

format:
	clang-format -i $(AYIN_SOURCES) $(AYIN_HEADERS)

clean: clean-ayin clean-extern

clean-ayin:
	$(RM) $(exe) $(AYIN_OBJECTS)

clean-extern:
	$(RM) $(exe) $(EXTERN_OBJECTS)

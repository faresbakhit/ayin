mode = debug

CXXFLAGS = -Ilib/imgui -Ilib/imgui/backends -Ilib/imgui/misc/freetype -Ilib/portable-file-dialogs -Ilib/stb
CXXFLAGS += -std=c++17 -Wall -Wextra -Wno-missing-field-initializers -Wno-missing-braces

ifeq ($(mode),debug)
 BUILDDIR = target/debug
 CXXFLAGS += -O1
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

INTERNAL_SOURCES = src/Application.cpp src/Commands.cpp src/Image.cpp src/ImageFilter.cpp src/Photo.cpp src/Main.cpp
# for `make format`
INTERNAL_HEADERS = src/Application.hpp src/Commands.hpp src/Image.hpp src/ImageFilter.hpp src/Photo.hpp src/utils/win32.hpp

EXTERNAL_SOURCES = lib/imgui/imgui.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp # ImGui
EXTERNAL_SOURCES += lib/imgui/misc/freetype/imgui_freetype.cpp # ImGui FreeType
EXTERNAL_SOURCES += lib/imgui/backends/imgui_impl_sdl2.cpp lib/imgui/backends/imgui_impl_opengl3.cpp # ImGui (SDL2 + OpenGL3) Backend
EXTERNAL_SOURCES += lib/portable-file-dialogs/portable-file-dialogs.cpp # Portable File Dialogs

INTERNAL_OBJECTS = $(addprefix $(BUILDDIR)/internal/, $(addsuffix .o, $(basename $(notdir $(INTERNAL_SOURCES)))))
EXTERNAL_OBJECTS = $(addprefix $(BUILDDIR)/external/, $(addsuffix .o, $(basename $(notdir $(EXTERNAL_SOURCES)))))

ifeq ($(OS),Windows_NT)
 exe := $(exe).exe
 LDFLAGS += -mwindows -lopengl32 -luuid -ldwmapi
 ifeq ($(mode),debug)
  LDFLAGS += -mconsole `pkg-config --libs sdl2 freetype2`
 else
  LDFLAGS += -static `pkg-config --static --libs sdl2 freetype2`
 endif
 CXXFLAGS += `pkg-config --cflags sdl2 freetype2`
 INTERNAL_OBJECTS += $(BUILDDIR)/internal/icon.o
else
 ifeq ($(shell uname),Linux)
  LDFLAGS += -lGL `pkg-config --libs sdl2 freetype2`
  CXXFLAGS += `pkg-config --cflags sdl2 freetype2`
 else
  $(error unsupported platform: $(mode))
 endif
endif

$(exe): $(INTERNAL_OBJECTS) $(EXTERNAL_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/internal $(BUILDDIR)/external

$(BUILDDIR)/internal/%.o:src/%.cpp|$(BUILDDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

ifeq ($(OS),Windows_NT)
$(BUILDDIR)/internal/icon.o:misc/icon/icon.rc
	windres $^ $@
endif

$(BUILDDIR)/external/%.o:lib/portable-file-dialogs/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/external/%.o:lib/imgui/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/external/%.o:lib/imgui/backends/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/external/%.o:lib/imgui/misc/freetype/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: all format clean clean-ayin clean-extern

all: $(BUILDDIR) $(exe)

format:
	clang-format -i $(INTERNAL_SOURCES) $(INTERNAL_HEADERS)

clean: clean-ayin clean-extern

clean-ayin:
	$(RM) $(exe) $(INTERNAL_OBJECTS)

clean-extern:
	$(RM) $(exe) $(EXTERNAL_OBJECTS)

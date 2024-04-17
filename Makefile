SOURCES = src/Main.cpp src/Commands.cpp src/Image.cpp src/ImageFilter.cpp src/Application.cpp
SOURCES += lib/imgui/imgui.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp # ImGui
SOURCES += lib/imgui/misc/freetype/imgui_freetype.cpp # ImGui FreeType
SOURCES += lib/imgui/backends/imgui_impl_sdl2.cpp lib/imgui/backends/imgui_impl_opengl3.cpp # ImGui (SDL2 + OpenGL3) Backend
SOURCES += lib/portable-file-dialogs/portable-file-dialogs.cpp # Portable File Dialogs

CXXFLAGS = -Iincludes -Ilib/imgui -Ilib/imgui/backends -Ilib/imgui/misc/freetype -Ilib/portable-file-dialogs -Ilib/stb
CXXFLAGS += -std=c++17 -Wall -Wextra -Wno-missing-field-initializers -Wno-missing-braces

mode = debug

ifeq ($(mode),debug)
 BUILDDIR := ./target/debug
 CXXFLAGS += -g -O0
else
 ifeq ($(mode),release)
  BUILDDIR := ./target/release
  CXXFLAGS += -O3 -DNDEBUG
 else
  ifeq ($(mode),dev-release)
   BUILDDIR := ./target/dev-release
   CXXFLAGS += -g -O2
  else
   $(error unknown mode: $(mode))
  endif
 endif
endif

EXECUTABLE = $(BUILDDIR)/ayin
OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

ifeq ($(OS),Windows_NT)
 LDFLAGS += -mwindows -lopengl32 -luuid -ldwmapi
 ifeq ($(mode),debug)
  LDFLAGS += -mconsole `pkg-config --libs sdl2 freetype2`
 else
  LDFLAGS += -static `pkg-config --static --libs sdl2 freetype2`
 endif
 CXXFLAGS += `pkg-config --cflags sdl2 freetype2`
 OBJECTS += $(BUILDDIR)/icon.o
else
 $(error unsupported platform: $(mode))
endif

.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o:src/%.cpp|$(BUILDDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/%.o:lib/portable-file-dialogs/%.cpp|$(BUILDDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/%.o:lib/imgui/%.cpp|$(BUILDDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/%.o:lib/imgui/backends/%.cpp|$(BUILDDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR)/%.o:lib/imgui/misc/freetype/%.cpp|$(BUILDDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILDDIR):
	mkdir -p $@

ifeq ($(OS),Windows_NT)
$(BUILDDIR)/icon.o: misc/icon/icon.rc
	windres $^ $@
endif

.PHONY: clean
clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)

.PHONY: clean-my-files
clean-my-files:
	$(RM) $(EXECUTABLE) $(BUILDDIR)/Application.o $(BUILDDIR)/Commands.o $(BUILDDIR)/Image.o $(BUILDDIR)/ImageFilter.o $(BUILDDIR)/Main.o

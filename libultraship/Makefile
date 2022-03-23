# Only used for standalone compilation, usually inherits these from the main makefile

CXX := g++
CC := gcc
AR	:= ar
FORMAT := clang-format-11

ASAN ?= 0
DEBUG ?= 1
OPTFLAGS ?= -O0
LTO ?= 0

WARN := -Wall -Wextra -Werror \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-Wno-parentheses \
	-Wno-narrowing \
	-Wno-missing-field-initializers

CXXFLAGS := $(WARN) -std=c++20 -D_GNU_SOURCE -DENABLE_OPENGL -m32
CFLAGS := $(WARN) -std=c99 -D_GNU_SOURCE -DENABLE_OPENGL -m32
CPPFLAGS := -MMD

ifneq ($(DEBUG),0)
	CXXFLAGS += -g -D_DEBUG
	CFLAGS += -g -D_DEBUG
endif

ifneq ($(ASAN),0)
	CXXFLAGS += -fsanitize=address
	CFLAGS += -fsanitize=address
endif

ifneq ($(LTO),0)
	CXXFLAGS += -flto
	CFLAGS += -flto
endif

SRC_DIRS  := $(shell find -type d -not -path "*build*")

CXX_FILES := \
	libultraship/Blob.cpp \
	libultraship/Cvar.cpp \
	libultraship/Environment.cpp \
	libultraship/GameSettings.cpp \
	libultraship/luslog.cpp \
	libultraship/ModManager.cpp \
	libultraship/SDLAudioPlayer.cpp \
	libultraship/SohConsole.cpp \
	libultraship/Factories/AnimationFactory.cpp \
	libultraship/Factories/ArrayFactory.cpp \
	libultraship/Factories/BlobFactory.cpp \
	libultraship/Factories/CutsceneFactory.cpp \
	libultraship/Factories/MtxFactory.cpp \
	libultraship/Factories/PathFactory.cpp \
	libultraship/Animation.cpp \
	libultraship/Factories/TextFactory.cpp \
	libultraship/Factories/TextureFactory.cpp \
	libultraship/Factories/VtxFactory.cpp \
	libultraship/Array.cpp \
	libultraship/ConfigFile.cpp \
	libultraship/Controller.cpp \
	libultraship/SohHooks.cpp \
	libultraship/SohImGuiImpl.cpp \
	libultraship/KeyboardController.cpp \
	libultraship/Factories/CollisionHeaderFactory.cpp \
	libultraship/Factories/DisplayListFactory.cpp \
	libultraship/Factories/MaterialFactory.cpp \
	libultraship/Factories/PlayerAnimationFactory.cpp \
	libultraship/Factories/ResourceLoader.cpp \
	libultraship/Factories/SceneFactory.cpp \
	libultraship/Factories/SkeletonFactory.cpp \
	libultraship/Factories/SkeletonLimbFactory.cpp \
	libultraship/Lib/Fast3D/gfx_cc.cpp \
	libultraship/Lib/Fast3D/gfx_direct3d11.cpp \
	libultraship/Lib/Fast3D/gfx_direct3d12.cpp \
	libultraship/Lib/Fast3D/gfx_direct3d_common.cpp \
	libultraship/Lib/Fast3D/gfx_dxgi.cpp \
	libultraship/Lib/Fast3D/gfx_glx.cpp \
	libultraship/Lib/Fast3D/gfx_opengl.cpp \
	libultraship/Lib/Fast3D/gfx_pc.cpp \
	libultraship/Lib/Fast3D/gfx_sdl2.cpp \
	libultraship/Lib/ImGui/backends/imgui_impl_opengl3.cpp \
	libultraship/Lib/ImGui/backends/imgui_impl_sdl.cpp \
	libultraship/Lib/ImGui/imgui.cpp \
	libultraship/Lib/ImGui/imgui_demo.cpp \
	libultraship/Lib/ImGui/imgui_draw.cpp \
	libultraship/Lib/ImGui/imgui_tables.cpp \
	libultraship/Lib/ImGui/imgui_widgets.cpp \
	libultraship/Lib/StrHash64.cpp \
	libultraship/Lib/tinyxml2/tinyxml2.cpp \
	libultraship/Archive.cpp \
	libultraship/CollisionHeader.cpp \
	libultraship/ControllerAttachment.cpp \
	libultraship/Cutscene.cpp \
	libultraship/DisplayList.cpp \
	libultraship/GlobalCtx2.cpp \
	libultraship/File.cpp \
	libultraship/Material.cpp \
	libultraship/Matrix.cpp \
	libultraship/MemoryPack.cpp \
	libultraship/Model.cpp \
	libultraship/Path.cpp \
	libultraship/PlayerAnimation.cpp \
	libultraship/Resource.cpp \
	libultraship/ResourceMgr.cpp \
	libultraship/RumblePack.cpp \
	libultraship/Scene.cpp \
	libultraship/Skeleton.cpp \
	libultraship/SkeletonLimb.cpp \
	libultraship/Text.cpp \
	libultraship/Texture.cpp \
	libultraship/TextureMod.cpp \
	libultraship/Utils.cpp \
	libultraship/Vertex.cpp \
	libultraship/Window.cpp \
	libultraship/WindowShim.cpp \
	libultraship/stox.cpp \
	libultraship/SDLController.cpp

C_FILES := \
	libultraship/mixer.c \
	libultraship/Lib/stb/stb_impl.c

FMT_FILES := $(shell find libultraship/ -type f \( -name *.cpp -o -name *.h \) -a -not -path "libultraship/Lib/*")

O_FILES   := \
	$(CXX_FILES:%.cpp=build/%.o) \
	$(C_FILES:%.c=build/%.o)

D_FILES   := $(O_FILES:%.o=%.d)

LIB       := libultraship.a

INC_DIRS := $(addprefix -I, \
	../ZAPDTR/ZAPDUtils \
	libultraship/Lib/Fast3D/U64 \
	libultraship/Lib/spdlog \
	libultraship/Lib/spdlog/include \
	libultraship \
)

# create build directories
$(shell mkdir -p $(SRC_DIRS:%=build/%))

all: $(LIB)

clean:
	rm -rf build $(LIB)

format:
	$(FORMAT) -i $(FMT_FILES)

.PHONY: all clean format

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OPTFLAGS) $(INC_DIRS) -c $< -o $@

build/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPTFLAGS) $(INC_DIRS) -c $< -o $@

$(LIB): $(O_FILES)
	$(AR) rcs $@ $^

-include $(D_FILES)
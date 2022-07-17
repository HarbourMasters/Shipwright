# Only used for standalone compilation, usually inherits these from the main makefile

CXX ?= g++
CC ?= gcc
AR := ar
FORMAT := clang-format-11
UNAME := $(shell uname)

ASAN ?= 0
DEBUG ?= 1
OPTFLAGS ?= -O0
LTO ?= 0

# flag to save whether the compiler being used is clang or gcc by checking CXX --version
CXX_IS_CLANG ?= $(shell $(CXX) --version | grep -c clang)
ifeq ($(CXX_IS_CLANG),1)
	MXX := $(CXX)
else
	MXX ?= clang++
endif


WARN := -Wall -Wextra -Werror \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-Wno-parentheses \
	-Wno-narrowing \
	-Wno-missing-field-initializers \
	-Wno-error=multichar \
	-Wno-unused-command-line-argument \
	-Wno-delete-non-abstract-non-virtual-dtor \
	-Wno-unused-private-field \
	-Wno-deprecated-copy-with-user-provided-copy \
	-Wno-deprecated-declarations \
	-Wno-unknown-warning-option

CWARN :=
CXXWARN := -Wno-deprecated-enum-enum-conversion -Wno-deprecated-copy

ifneq ($(CXX_IS_CLANG),1)
	WARN += -Wno-error=stringop-overflow
	CXXWARN += -Wno-error=maybe-uninitialized
endif

CXXFLAGS := $(WARN) $(CXXWARN) -std=c++20 -D_GNU_SOURCE -DENABLE_OPENGL -DSPDLOG_ACTIVE_LEVEL=0
CFLAGS := $(WARN) $(CWARN) -std=c99 -D_GNU_SOURCE -DENABLE_OPENGL -DSPDLOG_ACTIVE_LEVEL=0
CPPFLAGS := -MMD

MMFLAGS := -Wno-deprecated-declarations -ObjC++ -fobjc-weak -fobjc-arc

# if not using clang, ask clang to use gcc standard library
ifneq ($(CXX_IS_CLANG),1)
	STD_ISYSTEM=$(shell ${CXX} -xc++ -E -v - < /dev/null 2>&1  | grep "> search starts here" -A2 | tail -n 2 | head -n 1)
	CXX_ISYSTEM=$(shell ${CXX} -xc++ -E -v - < /dev/null 2>&1  | grep "> search starts here" -A2 | tail -n 2 | tail -n 1)
	MMFLAGS += -stdlib++-isystem ${STD_ISYSTEM} -cxx-isystem ${CXX_ISYSTEM}
endif

ifeq ($(UNAME), Darwin) #APPLE
	CPPFLAGS += $(shell pkg-config --cflags sdl2 glew) -framework OpenGL -framework Foundation
endif

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

SRC_DIRS  := $(shell find . -type d -not -path "*build*")

CXX_FILES := \
	$(shell find libultraship/Factories -name "*.cpp") \
	$(shell find libultraship/Lib/Fast3D -name "*.cpp") \
	$(shell find libultraship -maxdepth 1 -name "*.cpp") \
	$(shell find libultraship/Lib/ImGui -maxdepth 1 -name "*.cpp") \
	$(shell find libultraship/Lib/Mercury -maxdepth 1 -name "*.cpp") \
	libultraship/Lib/ImGui/backends/imgui_impl_opengl3.cpp \
	libultraship/Lib/ImGui/backends/imgui_impl_sdl.cpp \
	libultraship/Lib/StrHash64.cpp \
	libultraship/Lib/tinyxml2/tinyxml2.cpp

C_FILES := \
	libultraship/mixer.c \
	libultraship/Lib/stb/stb_impl.c

MM_FILES := \
	libultraship/OSXFolderManager.mm

FMT_FILES := $(shell find libultraship/ -type f \( -name "*.cpp" -o -name "*.h" \) -a -not -path "libultraship/Lib/*")

O_FILES   := \
	$(CXX_FILES:%.cpp=build/%.o) \
	$(C_FILES:%.c=build/%.o)

ifeq ($(UNAME), Darwin) #APPLE
	O_FILES += $(MM_FILES:%.mm=build/%.o)
endif

D_FILES   := $(O_FILES:%.o=%.d)

LIB       := libultraship.a

INC_DIRS := $(addprefix -I, \
	../ZAPDTR/ZAPDUtils \
	libultraship/Lib/Fast3D/U64 \
	libultraship/Lib/spdlog \
	libultraship/Lib/spdlog/include \
	libultraship/Lib/ImGui \
	libultraship/Lib/Mercury \
	libultraship \
	../StormLib/src \
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

build/%.o: %.mm
	$(MXX) $(MMFLAGS) $(CXXFLAGS) $(OPTFLAGS) $(INC_DIRS) -c $< -o $@

$(LIB): $(O_FILES)
	$(AR) rcs $@ $^

-include $(D_FILES)

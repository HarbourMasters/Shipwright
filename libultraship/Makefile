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

CXXFLAGS := $(WARN) -std=c++20 -D_GNU_SOURCE -DENABLE_OPENGL -DSPDLOG_ACTIVE_LEVEL=0 -m32
CFLAGS := $(WARN) -std=c99 -D_GNU_SOURCE -DENABLE_OPENGL -DSPDLOG_ACTIVE_LEVEL=0 -m32
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
	$(shell find libultraship/Factories -name *.cpp) \
	$(shell find libultraship/Lib/Fast3D -name *.cpp) \
	$(shell find libultraship -maxdepth 1 -name *.cpp) \
	$(shell find libultraship/Lib/ImGui -maxdepth 1 -name *.cpp) \
	libultraship/Lib/ImGui/backends/imgui_impl_opengl3.cpp \
	libultraship/Lib/ImGui/backends/imgui_impl_sdl.cpp \
	libultraship/Lib/StrHash64.cpp \
	libultraship/Lib/tinyxml2/tinyxml2.cpp

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
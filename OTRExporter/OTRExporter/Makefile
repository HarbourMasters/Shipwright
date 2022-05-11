# Only used for standalone compilation, usually inherits these from the main makefile

CXX := g++
AR	:= ar
FORMAT := clang-format-11

ASAN ?= 0
DEBUG ?= 1
OPTFLAGS ?= -O0
LTO ?= 0

WARN := -Wall -Wextra -Werror \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-Wno-unused-variable


CXXFLAGS := $(WARN) -std=c++17 
CPPFLAGS := -MMD

ifneq ($(DEBUG),0)
	CXXFLAGS += -g
endif

ifneq ($(ASAN),0)
	CXXFLAGS += -fsanitize=address
endif

ifneq ($(LTO),0)
	CXXFLAGS += -flto
endif

SRC_DIRS  := $(shell find -type d -not -path "*build*")
CXX_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
H_FILES   := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.h))

O_FILES   := $(CXX_FILES:%.cpp=build/%.o)
D_FILES   := $(O_FILES:%.o=%.d)
LIB       := OTRExporter.a

INC_DIRS := $(addprefix -I, \
	../../ZAPDTR/ZAPD \
	../../ZAPDTR/lib/tinyxml2 \
	../../ZAPDTR/lib/libgfxd \
	../../ZAPDTR/ZAPDUtils \
	../../libultraship/libultraship \
	../../libultraship/libultraship/Lib/spdlog/include \
	../../libultraship/libultraship/Lib/Fast3D/U64 \
)

# create build directories
$(shell mkdir -p $(SRC_DIRS:%=build/%))

all: $(LIB)

clean:
	rm -rf build $(LIB)

format:
	$(FORMAT) -i $(CXX_FILES) $(H_FILES)

.PHONY: all clean format

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OPTFLAGS) $(INC_DIRS) -c $< -o $@

$(LIB): $(O_FILES)
	$(AR) rcs $@ $^

-include $(D_FILES)
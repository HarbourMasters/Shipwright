# Only used for standalone compilation, usually inherits these from the main makefile
CXXFLAGS ?= -Wall -Wextra -O2 -g -std=c++17

SRC_DIRS  := $(shell find . -type d -not -path "*build*")
CPP_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
H_FILES   := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.h))

O_FILES   := $(foreach f,$(CPP_FILES:.cpp=.o),build/$f)
LIB       := ExporterTest.a

# create build directories
$(shell mkdir -p $(foreach dir,$(SRC_DIRS),build/$(dir)))

all: $(LIB)

clean:
	rm -rf build $(LIB)

format:
	clang-format-11 -i $(CPP_FILES) $(H_FILES)

.PHONY: all clean format

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -I ./ -I ../ZAPD -I ../ZAPDUtils -I ../lib/tinyxml2 -c $(OUTPUT_OPTION) $<

$(LIB): $(O_FILES)
	$(AR) rcs $@ $^

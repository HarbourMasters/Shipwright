# use variables in submakes
export
OPTIMIZATION_ON ?= 1
ASAN ?= 0
DEPRECATION_ON ?= 1
DEBUG ?= 0
COPYCHECK_ARGS ?=
LLD ?= 0
WERROR ?= 0

# Use clang++ if available, else use g++
ifeq ($(shell command -v clang++ >/dev/null 2>&1; echo $$?),0)
  CXX := clang++
else
  CXX := g++
endif

INC := -I ZAPD -I lib/libgfxd -I lib/tinyxml2 -I ZAPDUtils
CXXFLAGS := -fpic -std=c++17 -Wall -Wextra -fno-omit-frame-pointer
OPTFLAGS :=

ifneq ($(DEBUG),0)
  OPTIMIZATION_ON = 0
  CXXFLAGS += -g3 -DDEVELOPMENT -D_DEBUG
  COPYCHECK_ARGS += --devel
  DEPRECATION_ON = 0
endif

ifneq ($(WERROR),0)
  CXXFLAGS += -Werror
endif

ifeq ($(OPTIMIZATION_ON),0)
  OPTFLAGS := -O0
else
  OPTFLAGS := -O2
endif

ifneq ($(ASAN),0)
  CXXFLAGS += -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined
endif
ifneq ($(DEPRECATION_ON),0)
  CXXFLAGS += -DDEPRECATION_ON
endif
# CXXFLAGS += -DTEXTURE_DEBUG

LDFLAGS := -lm -ldl -lpng

# Use LLD if available. Set LLD=0 to not use it
ifeq ($(shell command -v ld.lld >/dev/null 2>&1; echo $$?),0)
  LLD := 1
endif

ifneq ($(LLD),0)
  LDFLAGS += -fuse-ld=lld
endif

UNAME := $(shell uname)
UNAMEM := $(shell uname -m)
ifneq ($(UNAME), Darwin)
  LDFLAGS += -Wl,-export-dynamic -lstdc++fs
  EXPORTERS := -Wl,--whole-archive ExporterTest/ExporterTest.a -Wl,--no-whole-archive
else
  EXPORTERS := -Wl,-force_load ExporterTest/ExporterTest.a
  ifeq ($(UNAMEM),arm64)
    ifeq ($(shell brew list libpng > /dev/null 2>&1; echo $$?),0)
      LDFLAGS += -L $(shell brew --prefix)/lib
      INC += -I $(shell brew --prefix)/include
      else
      $(error Please install libpng via Homebrew)
    endif
  endif
endif


ZAPD_SRC_DIRS := $(shell find ZAPD -type d)
SRC_DIRS = $(ZAPD_SRC_DIRS) lib/tinyxml2

ZAPD_CPP_FILES := $(foreach dir,$(ZAPD_SRC_DIRS),$(wildcard $(dir)/*.cpp))
ZAPD_H_FILES   := $(foreach dir,$(ZAPD_SRC_DIRS),$(wildcard $(dir)/*.h))

CPP_FILES += $(ZAPD_CPP_FILES) lib/tinyxml2/tinyxml2.cpp
O_FILES   := $(foreach f,$(CPP_FILES:.cpp=.o),build/$f)
O_FILES   += build/ZAPD/BuildInfo.o

# create build directories
$(shell mkdir -p $(foreach dir,$(SRC_DIRS),build/$(dir)))


# Main targets
all: ZAPD.out copycheck

build/ZAPD/BuildInfo.o:
	python3 ZAPD/genbuildinfo.py $(COPYCHECK_ARGS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INC) -c $(OUTPUT_OPTION) build/ZAPD/BuildInfo.cpp

copycheck: ZAPD.out
	python3 copycheck.py

clean:
	rm -rf build ZAPD.out
	$(MAKE) -C lib/libgfxd clean
	$(MAKE) -C ZAPDUtils clean
	$(MAKE) -C ExporterTest clean

rebuild: clean all

format:
	clang-format-11 -i $(ZAPD_CPP_FILES) $(ZAPD_H_FILES)
	$(MAKE) -C ZAPDUtils format
	$(MAKE) -C ExporterTest format

.PHONY: all build/ZAPD/BuildInfo.o copycheck clean rebuild format

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INC) -c $(OUTPUT_OPTION) $<


# Submakes
lib/libgfxd/libgfxd.a:
	$(MAKE) -C lib/libgfxd

.PHONY: ExporterTest
ExporterTest:
	$(MAKE) -C ExporterTest

.PHONY: ZAPDUtils
ZAPDUtils:
	$(MAKE) -C ZAPDUtils


# Linking
ZAPD.out: $(O_FILES) lib/libgfxd/libgfxd.a ExporterTest ZAPDUtils
	$(CXX) $(CXXFLAGS) $(O_FILES) lib/libgfxd/libgfxd.a ZAPDUtils/ZAPDUtils.a $(EXPORTERS) $(LDFLAGS) $(OUTPUT_OPTION)

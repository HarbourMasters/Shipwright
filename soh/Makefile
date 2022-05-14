CXX := g++
CC := gcc
LD := lld
AR	:= ar
FORMAT := clang-format-11
ZAPD := ../ZAPDTR/ZAPD.out

LIBULTRASHIP := ../libultraship/libultraship.a
ZAPDUTILS := ../ZAPDTR/ZAPDUtils/ZAPDUtils.a

ASAN ?= 0
DEBUG ?= 1
OPTFLAGS ?= -O0
LTO ?= 0

WARN := \
	-Wno-return-type \
	-funsigned-char \
	-m32 -mhard-float -fno-stack-protector -fno-common -fno-zero-initialized-in-bss -fno-strict-aliasing -fno-inline-functions -fno-inline-small-functions -fno-toplevel-reorder -ffreestanding -fwrapv \

CXXFLAGS := $(WARN) -std=c++20 -D_GNU_SOURCE -fpermissive -no-pie -nostdlib -march=i386
CFLAGS := $(WARN) -std=c99 -D_GNU_SOURCE -no-pie -nostdlib -march=i386
LDFLAGS := -m32
CPPFLAGS := -MMD

ifneq ($(DEBUG),0)
	CXXFLAGS += -g
	CFLAGS += -g
endif

ifneq ($(ASAN),0)
	CXXFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

ifneq ($(LTO),0)
	CXXFLAGS += -flto
	LDFLAGS += -flto
endif

TARGET := soh.elf

INC_DIRS := $(addprefix -I, \
	. \
	assets \
	build \
	include \
	src \
	../ZAPDTR/ZAPDUtils \
	../libultraship/libultraship \
	../libultraship/libultraship/Lib/spdlog/include \
	../libultraship/libultraship/Lib/Fast3D/U64 \
	../libultraship/libultraship/Lib/Fast3D/U64/PR \
)

LDDIRS := $(addprefix -L, \
	../external \
	../libultraship/ \
)

LDLIBS := \
	$(ZAPDUTILS) \
	$(addprefix -l, \
	X11 \
	dl \
	bz2 \
	z \
	pthread \
	atomic \
	SDL2 \
	GL \
	GLEW \
	storm \
	pulse\
	ultraship \
) \

ASSET_BIN_DIRS := $(shell find assets/* -type d -not -path "assets/xml*")
ASSET_FILES_XML := $(foreach dir,$(ASSET_BIN_DIRS),$(wildcard $(dir)/*.xml))
ASSET_FILES_BIN := $(foreach dir,$(ASSET_BIN_DIRS),$(wildcard $(dir)/*.bin))
ASSET_FILES_OUT := $(foreach f,$(ASSET_FILES_XML:.xml=.c),$f) \
				   $(foreach f,$(ASSET_FILES_BIN:.bin=.bin.inc.c),build/$f)

TEXTURE_FILES_PNG := $(foreach dir,$(ASSET_BIN_DIRS),$(wildcard $(dir)/*.png))
TEXTURE_FILES_JPG := $(foreach dir,$(ASSET_BIN_DIRS),$(wildcard $(dir)/*.jpg))
TEXTURE_FILES_OUT := $(foreach f,$(TEXTURE_FILES_PNG:.png=.inc.c),build/$f) \
					 $(foreach f,$(TEXTURE_FILES_JPG:.jpg=.jpg.inc.c),build/$f) \

CXX_FILES := \
	$(shell find soh -type f -name *.cpp)

C_FILES := \
	$(shell find soh -type f -name *.c) \
	$(shell find src/boot -type f -name *.c) \
	$(shell find src/buffers -type f -name *.c) \
	$(shell find src/code -type f -name *.c) \
	$(shell find src/overlays -type f -name *.c) \
	src/libultra/gu/coss.c \
	src/libultra/gu/guLookAt.c \
	src/libultra/gu/guLookAtHilite.c \
	src/libultra/gu/guPerspectiveF.c \
	src/libultra/gu/guPosition.c \
	src/libultra/gu/guS2DInitBg.c \
	src/libultra/gu/ortho.c \
	src/libultra/gu/rotate.c \
	src/libultra/gu/sins.c \
	src/libultra/gu/sintable.c \
	src/libultra/libc/sprintf.c

O_FILES := \
	$(C_FILES:%.c=build/%.o) \
	$(CXX_FILES:%.cpp=build/%.o)
D_FILES := $(O_FILES:%.o=%.d)

# create build directory
SRC_DIRS := $(shell find . -type d -a -not -path "*build*")
$(shell mkdir -p $(SRC_DIRS:%=build/%))

all:
	$(MAKE) -C ../libultraship
	$(MAKE) $(TARGET)

setup:
	cd ../OTRExporter && python3 extract_baserom.py
	$(MAKE) mpq

mpq:
	$(MAKE) -C ../libultraship
	$(MAKE) -C ../OTRExporter/OTRExporter
	$(MAKE) -C ../ZAPDTR
	rm -rf ../OTRExporter/oot.otr
	cd ../OTRExporter && python3 extract_assets.py
	cp ../OTRExporter/oot.otr .

distclean: clean
	$(RM) -r baserom/
	$(MAKE) clean -C ../libultraship
	$(MAKE) clean -C ../OTRExporter/OTRExporter
	$(MAKE) clean -C ../ZAPDTR

clean:
	rm -rf build $(TARGET)

.PHONY: all clean distclean setup mpq

build/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(OPTFLAGS) $(INC_DIRS) $< -o $@

build/%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(OPTFLAGS) $(INC_DIRS) $< -o $@

# make soh depend on libultraship
$(TARGET): $(LIBULTRASHIP)

$(TARGET): $(O_FILES)
	$(CXX) $^ -o $@ $(LDFLAGS) -fuse-ld=$(LD) $(LDDIRS) $(LDLIBS)

-include $(D_FILES)
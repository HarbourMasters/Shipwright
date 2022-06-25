CXX ?= g++
CC ?= gcc
LD := lld
AR	:= ar
FORMAT := clang-format-11
ZAPD := ../ZAPDTR/ZAPD.out
UNAME := $(shell uname)
UNAMEM := $(shell uname -m)

LIBULTRASHIP := ../libultraship/libultraship.a
ZAPDUTILS := ../ZAPDTR/ZAPDUtils/ZAPDUtils.a
LIBSTORM := ../StormLib/build/libstorm.a

ASAN ?= 0
DEBUG ?= 1
OPTFLAGS ?= -O0
LTO ?= 0

WARN := \
	-Wno-return-type \
	-funsigned-char \
	-fno-stack-protector -fno-common -fno-zero-initialized-in-bss -fno-strict-aliasing -fno-inline-functions -fno-inline-small-functions -fno-toplevel-reorder -ffreestanding -fwrapv \

CXXFLAGS := $(WARN) -std=c++20 -D_GNU_SOURCE -fpermissive -no-pie -nostdlib
CFLAGS := $(WARN) -std=c99 -D_GNU_SOURCE -no-pie -nostdlib
LDFLAGS :=

ifeq ($(UNAME), Linux) #LINUX
	CXXFLAGS += -mhard-float -msse2 -mfpmath=sse
	CFLAGS += -mhard-float -msse2 -mfpmath=sse
endif

ifeq ($(UNAME), Darwin) #APPLE
	CXXFLAGS += $(shell pkg-config --cflags sdl2) $(shell sdl2-config --cflags) $(shell pkg-config --cflags glew) -framework OpenGL
	CFLAGS += $(shell pkg-config --cflags sdl2) $(shell sdl2-config --cflags) $(shell pkg-config --cflags glew) -framework OpenGL
endif

CPPFLAGS := -MMD

ifneq ($(DEBUG),0)
	CXXFLAGS += -g
	CFLAGS += -g
endif

ifneq ($(ASAN),0)
	CXXFLAGS += -fsanitize=address
	CFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

ifneq ($(LTO),0)
	CXXFLAGS += -flto
	CFLAGS += -flto
	LDFLAGS += -flto
endif

ifeq ($(UNAME), Linux) #LINUX
TARGET := soh.elf
endif

ifeq ($(UNAME), Darwin) #APPLE
TARGET := soh-$(UNAMEM)
endif

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

ifeq ($(UNAME), Linux) #LINUX
	INC_DIRS += $(addprefix -I, \
		/opt/X11/include \
)
endif

LDDIRS := $(addprefix -L, \
	../libultraship/ \
)

ifeq ($(UNAME), Linux) #LINUX
	LDDIRS += $(addprefix -L, \
		/opt/X11/lib \
)
endif

LDLIBS := \
	$(ZAPDUTILS) \
	$(LIBSTORM) \
	$(addprefix -l, \
	dl \
	bz2 \
	z \
	pthread \
	atomic \
	ultraship \
)

ifeq ($(UNAME), Linux) #LINUX
LDLIBS += \
	$(addprefix -l, \
	X11 \
	SDL2 \
	GL \
	GLEW \
	pulse \
)
endif

ifeq ($(UNAME), Darwin) #APPLE
LDLIBS += \
	$(addprefix -framework, \
	OpenGL \
) \
	$(shell sdl2-config --libs) $(shell pkg-config --libs glew)
endif

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
	$(shell find soh -type f -name "*.cpp")

C_FILES := \
	$(shell find soh -type f -name "*.c") \
	$(shell find src/boot -type f -name "*.c") \
	$(shell find src/buffers -type f -name "*.c") \
	$(shell find src/code -type f -name "*.c") \
	$(shell find src/overlays -type f -name "*.c") \
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

# Apple App Bundle
APPNAME=soh
APPBUNDLE=$(APPNAME).app
APPBUNDLECONTENTS=$(APPBUNDLE)/Contents
APPBUNDLEEXE=$(APPBUNDLECONTENTS)/MacOS
APPBUNDLERESOURCES=$(APPBUNDLECONTENTS)/Resources
APPBUNDLEICON=$(APPBUNDLECONTENTS)/Resources

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

appbundle: macosx/$(APPNAME).icns
	rm -rf $(APPBUNDLE)
	mkdir $(APPBUNDLE)
	mkdir $(APPBUNDLE)/Contents
	mkdir $(APPBUNDLE)/Contents/MacOS
	mkdir $(APPBUNDLE)/Contents/Resources
	cp macosx/Info.plist $(APPBUNDLECONTENTS)/
	cp macosx/PkgInfo $(APPBUNDLECONTENTS)/
	cp macosx/$(APPNAME).icns $(APPBUNDLEICON)/
	cp $(TARGET) $(APPBUNDLEEXE)/soh
	cp macosx/launcher.sh $(APPBUNDLEEXE)/launcher.sh
	clang -ObjC macosx/appsupport.m -arch arm64 -arch x86_64 -framework Foundation -o macosx/appsupport
	cp macosx/appsupport $(APPBUNDLEEXE)/appsupport
	otool -l $(TARGET) | grep -A 2 LC_RPATH  | tail -n 1 | awk '{print $2}' | dylibbundler -od -b -x  $(APPBUNDLEEXE)/soh -d $(APPBUNDLECONTENTS)/libs

macosx/$(APPNAME).icns: macosx/$(APPNAME)Icon.png
	rm -rf macosx/$(APPNAME).iconset
	mkdir macosx/$(APPNAME).iconset
	sips -z 16 16     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16.png
	sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16@2x.png
	sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32.png
	sips -z 64 64     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32@2x.png
	sips -z 128 128   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128.png
	sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128@2x.png
	sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256.png
	sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256@2x.png
	sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_512x512.png
	cp macosx/$(APPNAME)Icon.png macosx/$(APPNAME).iconset/icon_512x512@2x.png
	iconutil -c icns -o macosx/$(APPNAME).icns macosx/$(APPNAME).iconset
	rm -r macosx/$(APPNAME).iconset

filledappbundle: appbundle
	cp ./oot.otr $(APPBUNDLEEXE)/oot.otr


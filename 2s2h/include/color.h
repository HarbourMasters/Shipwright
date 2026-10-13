#ifndef _COLOR_H_
#define _COLOR_H_

#include <ship/utils/binarytools/endianness.h>
#include "PR/ultratypes.h"
#include <libultraship/color.h>
// For checking the alpha bit in an RGBA16 pixel
#define RGBA16_PIXEL_OPAQUE 1
#if 0
typedef struct {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
} Color_RGB8; // size = 0x3

typedef struct {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
} Color_RGBA8; // size = 0x4



// only use when necessary for alignment purposes
typedef union Color_RGBA8_u32 {
    struct {
        u8 r, g, b, a;
    };
    u32 rgba;
} Color_RGBA8_u32;

typedef struct {
    u32 r, g, b, a;
} Color_RGBAu32;

typedef union {
    struct {
        u16 r : 5;
        u16 g : 5;
        u16 b : 5;
        u16 a : 1;
    };
    u16 rgba;
} Color_RGBA16;


typedef struct {
    f32 r, g, b, a;
} Color_RGBAf;

#endif

typedef struct {
    /* 0x0 */ s16 r;
    /* 0x2 */ s16 g;
    /* 0x4 */ s16 b;
} Color_RGB16; // size = 0x6

typedef struct {
    u32 r, g, b, a;
} Color_RGBAu32;

typedef union {
    struct {
        u32 r : 5;
        u32 g : 5;
        u32 b : 5;
        u32 a : 1;
    };
    u16 rgba;
} Color_RGBA16_2;

typedef union {
    struct {
        u32 r : 3;
        u32 g : 3;
        u32 b : 3;
        u32 a : 5;
    };
    u16 rgba;
} Color_RGBA14;
#define RGBA8(r, g, b, a) ((((r)&0xFF) << 24) | (((g)&0xFF) << 16) | (((b)&0xFF) << 8) | (((a)&0xFF) << 0))

#define RGBA16_GET_R(pixel) (((pixel) >> 11) & 0x1F)
#define RGBA16_GET_G(pixel) (((pixel) >> 6) & 0x1F)
#define RGBA16_GET_B(pixel) (((pixel) >> 1) & 0x1F)

#endif

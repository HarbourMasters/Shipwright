#ifndef COLOR_H
#define COLOR_H

#include "endianness.h"

typedef struct {
    u8 r, g, b;
} Color_RGB8;

typedef struct {
    u8 r, g, b, a;
} Color_RGBA8;

// only use when necessary for alignment purposes
typedef union {
    struct {
#ifdef IS_BIGENDIAN
        u8 r, g, b, a;
#else
        u8 a, b, g, r;
#endif
    };
    u32 rgba;
} Color_RGBA8_u32;

typedef struct {
    f32 r, g, b, a;
} Color_RGBAf;

typedef union {
    struct {
        u16 r : 5;
        u16 g : 5;
        u16 b : 5;
        u16 a : 1;
    };
    u16 rgba;
} Color_RGBA16;

#endif

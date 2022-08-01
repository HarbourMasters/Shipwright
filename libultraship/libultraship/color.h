#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include "endianness.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    uint8_t r, g, b;
} Color_RGB8;

typedef struct {
    uint8_t r, g, b, a;
} Color_RGBA8;

// only use when necessary for alignment purposes
typedef union {
    struct {
#ifdef IS_BIGENDIAN
        uint8_t r, g, b, a;
#else
        uint8_t a, b, g, r;
#endif
    };
    uint32_t rgba;
} Color_RGBA8_u32;

typedef struct {
    float r, g, b, a;
} Color_RGBAf;

typedef union {
    struct {
        uint16_t r : 5;
        uint16_t g : 5;
        uint16_t b : 5;
        uint16_t a : 1;
    };
    uint16_t rgba;
} Color_RGBA16;

#ifdef __cplusplus
};
#endif

#endif

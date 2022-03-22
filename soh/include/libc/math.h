#ifndef MATH_H
#define MATH_H

#include "ultra64/types.h"

#define M_PI 3.14159265358979323846f
#define M_SQRT2 1.41421356237309504880f
#define FLT_MAX 340282346638528859811704183484516925440.0f
#define SHT_MAX 32767.0f
#define SHT_MINV (1.0f / SHT_MAX)
#define DEGTORAD(x) (x * M_PI / 180.0f)

typedef union {
    struct {
        u32 hi;
        u32 lo;
    } word;

    f64 d;
} du;

typedef union {
    u32 i;
    f32 f;
} fu;

extern f32 __libm_qnan_f;

#endif

#ifndef PR_GUINT_H
#define PR_GUINT_H

#include "ultratypes.h"

typedef union {
    f64 d;
    struct {
        u32 hi;
        u32 lo;
    } word;
} du;

typedef union {
    u32 i;
    f32 f;
} fu;

//#define ROUND(x) (s32)(((x) >= 0.0) ? ((x) + 0.5) : ((x) - 0.5))

extern f32 __libm_qnan_f;

#endif

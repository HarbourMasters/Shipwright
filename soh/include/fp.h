#ifndef FP_H
#define FP_H
#include "ultra64.h"

extern f32 qNaN0x3FFFFF;
extern f32 qNaN0x10000;
extern f32 sNaN0x3FFFFF;

f32 floorf(f32 x);
f64 floor(f64 x);
s32 lfloorf(f32 x);
s32 lfloor(f64 x);

f32 ceilf(f32 x);
f64 ceil(f64 x);
s32 lceilf(f32 x);
s32 lceil(f64 x);

f32 truncf(f32 x);
f64 trunc(f64 x);
s32 ltruncf(f32 x);
s32 ltrunc(f64 x);

f32 nearbyintf(f32 x);
f64 nearbyint(f64 x);
s32 lnearbyintf(f32 x);
s32 lnearbyint(f64 x);

f32 roundf(f32 x);
f64 round(f64 x);
s32 lroundf(f32 x);
s32 lround(f64 x);

#endif

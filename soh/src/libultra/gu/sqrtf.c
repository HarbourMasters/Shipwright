// clang-format off

#include "global.h"

#ifndef __GNUC__
#define __builtin_sqrtf sqrtf
#endif

f32 sqrtf(f32 f) {
    return __builtin_sqrtf(f);
}

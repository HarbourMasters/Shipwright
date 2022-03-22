#include "global.h"

#ifndef __GNUC__
#define __builtin_sqrt sqrt
#endif

f64 sqrt(f64 f) {
    return __builtin_sqrt(f);
}

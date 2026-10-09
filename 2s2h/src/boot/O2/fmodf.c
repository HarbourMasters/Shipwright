#include "global.h"
#if 0
f32 fmodf(f32 dividend, f32 divisor) {
    s32 quotient;

    if (divisor == 0.0f) {
        return 0.0f;
    }
    quotient = dividend / divisor;

    return dividend - quotient * divisor;
}
#endif
#include "global.h"

void osViSetYScale(f32 scale) {
    register u32 prevInt = __osDisableInt();

    __osViNext->y.factor = scale;
    __osViNext->state |= 4;

    __osRestoreInt(prevInt);
}

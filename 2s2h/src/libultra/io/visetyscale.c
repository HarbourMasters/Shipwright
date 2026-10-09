#include "ultra64.h"

void osViSetYScale(f32 value) {
    register u32 saveMask = __osDisableInt();

    __osViNext->y.factor = value;

    __osViNext->state |= VI_STATE_YSCALE_UPDATED;

    __osRestoreInt(saveMask);
}

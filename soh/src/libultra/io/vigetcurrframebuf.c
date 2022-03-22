#include "global.h"

u32* osViGetCurrentFramebuffer(void) {
    register u32 prevInt = __osDisableInt();
    u32* var1 = __osViCurr->buffer;

    __osRestoreInt(prevInt);

    return var1;
}

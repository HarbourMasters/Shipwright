#include "ultra64.h"

void* osViGetCurrentFramebuffer(void) {
    register u32 prevInt = __osDisableInt();
    void* curBuf = __osViCurr->buffer;

    __osRestoreInt(prevInt);

    return curBuf;
}

#include "ultra64.h"

void osViSwapBuffer(void* frameBufPtr) {

    u32 saveMask = __osDisableInt();

    __osViNext->buffer = frameBufPtr;
    __osViNext->state |= VI_STATE_BUFFER_UPDATED;

    __osRestoreInt(saveMask);
}

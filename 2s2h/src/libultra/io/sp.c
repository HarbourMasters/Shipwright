#include "ultra64.h"
#include "libc/stdbool.h"

s32 __osSpDeviceBusy(void) {
    register u32 status = IO_READ(SP_STATUS_REG);

    if (status & (SP_STATUS_DMA_BUSY | SP_STATUS_DMA_FULL | SP_STATUS_IO_FULL)) {
        return true;
    } else {
        return false;
    }
}

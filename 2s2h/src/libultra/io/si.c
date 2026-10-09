#include "ultra64.h"
#include "libc/stdbool.h"

s32 __osSiDeviceBusy() {
    register u32 status = IO_READ(SI_STATUS_REG);

    if (status & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_READ_BUSY)) {
        return true;
    } else {
        return false;
    }
}

#include "global.h"

s32 __osSiDeviceBusy(void) {
    register u32 status = HW_REG(SI_STATUS_REG, u32);

    if (status & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_READ_BUSY)) {
        return true;
    } else {
        return false;
    }
}

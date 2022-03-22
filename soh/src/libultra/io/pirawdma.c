#include "global.h"

s32 __osPiRawStartDma(s32 dir, u32 cartAddr, void* dramAddr, size_t size) {
    register s32 status = HW_REG(PI_STATUS_REG, u32);

    while (status & (PI_STATUS_BUSY | PI_STATUS_IOBUSY)) {
        status = HW_REG(PI_STATUS_REG, u32);
    }

    HW_REG(PI_DRAM_ADDR_REG, void*) = (void*)osVirtualToPhysical(dramAddr);

    HW_REG(PI_CART_ADDR_REG, void*) = (void*)((osRomBase | cartAddr) & 0x1FFFFFFF);

    switch (dir) {
        case OS_READ:
            HW_REG(PI_WR_LEN_REG, u32) = size - 1;
            break;
        case OS_WRITE:
            HW_REG(PI_RD_LEN_REG, u32) = size - 1;
            break;
        default:
            return -1;
            break;
    }
    return 0;
}

#include "global.h"

s32 __osSiRawStartDma(s32 dir, void* addr) {
    if (HW_REG(SI_STATUS_REG, u32) & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_READ_BUSY)) {
        return -1;
    }
    if (dir == OS_WRITE) {
        osWritebackDCache(addr, 0x40);
    }
    HW_REG(SI_DRAM_ADDR_REG, void*) = (void*)osVirtualToPhysical(addr);
    if (dir == OS_READ) {
        HW_REG(SI_PIF_ADDR_RD64B_REG, void*) = (void*)PIF_RAM_START;
    } else {
        HW_REG(SI_PIF_ADDR_WR64B_REG, void*) = (void*)PIF_RAM_START;
    }
    if (dir == OS_READ) {
        osInvalDCache(addr, 0x40);
    }
    return 0;
}

#include "ultra64.h"
#include "alignment.h"

s32 __osSiRawStartDma(s32 direction, void* dramAddr) {
    if (IO_READ(SI_STATUS_REG) & (SI_STATUS_DMA_BUSY | SI_STATUS_IO_READ_BUSY)) {
        return -1;
    }

    if (direction == OS_WRITE) {
        osWritebackDCache(dramAddr, ALIGN16(PIF_RAM_END - PIF_RAM_START));
    }

    IO_WRITE(SI_DRAM_ADDR_REG, osVirtualToPhysical(dramAddr));

    if (direction == OS_READ) {
        IO_WRITE(SI_PIF_ADDR_RD64B_REG, PIF_RAM_START);
    } else {
        IO_WRITE(SI_PIF_ADDR_WR64B_REG, PIF_RAM_START);
    }

    if (direction == OS_READ) {
        osInvalDCache(dramAddr, ALIGN16(PIF_RAM_END - PIF_RAM_START));
    }
    return 0;
}

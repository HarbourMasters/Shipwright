#include "ultra64.h"
#include "libc/stdbool.h"
#include "alignment.h"

OSPiHandle __CartRomHandle ALIGNED(8);

OSPiHandle* osCartRomInit(void) {
    register u32 initialConfig;
    register s32 status;
    register u32 prevInt;
    register u32 lastLatency;
    register u32 lastPageSize;
    register u32 lastRelDuration;
    register u32 lastPulse;

    static s32 sCartRomNeedsInit = true;

    __osPiGetAccess();

    if (!sCartRomNeedsInit) {
        __osPiRelAccess();
        return &__CartRomHandle;
    }

    sCartRomNeedsInit = false;
    __CartRomHandle.type = DEVICE_TYPE_CART;
    __CartRomHandle.baseAddress = PHYS_TO_K1(PI_DOM1_ADDR2);
    __CartRomHandle.domain = PI_DOMAIN1;
    __CartRomHandle.speed = 0;
    bzero(&__CartRomHandle.transferInfo, sizeof(__OSTranxInfo));

    /* Uses `status & PI_STATUS_ERROR` in OoT */
    while (status = IO_READ(PI_STATUS_REG), status & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY)) {
        ;
    }

    lastLatency = IO_READ(PI_BSD_DOM1_LAT_REG);
    lastPageSize = IO_READ(PI_BSD_DOM1_PGS_REG);
    lastRelDuration = IO_READ(PI_BSD_DOM1_RLS_REG);
    lastPulse = IO_READ(PI_BSD_DOM1_PWD_REG);

    IO_WRITE(PI_BSD_DOM1_LAT_REG, 0xFF);
    IO_WRITE(PI_BSD_DOM1_PGS_REG, 0);
    IO_WRITE(PI_BSD_DOM1_RLS_REG, 3);
    IO_WRITE(PI_BSD_DOM1_PWD_REG, 0xFF);

    initialConfig = IO_READ(__CartRomHandle.baseAddress);
    __CartRomHandle.latency = initialConfig & 0xFF;
    __CartRomHandle.pageSize = (initialConfig >> 0x10) & 0xF;
    __CartRomHandle.relDuration = (initialConfig >> 0x14) & 0xF;
    __CartRomHandle.pulse = (initialConfig >> 8) & 0xFF;

    IO_WRITE(PI_BSD_DOM1_LAT_REG, lastLatency);
    IO_WRITE(PI_BSD_DOM1_PGS_REG, lastPageSize);
    IO_WRITE(PI_BSD_DOM1_RLS_REG, lastRelDuration);
    IO_WRITE(PI_BSD_DOM1_PWD_REG, lastPulse);

    prevInt = __osDisableInt();
    __CartRomHandle.next = __osPiTable;
    __osPiTable = &__CartRomHandle;
    __osRestoreInt(prevInt);
    __osPiRelAccess();

    return &__CartRomHandle;
}

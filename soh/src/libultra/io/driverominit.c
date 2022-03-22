#include "global.h"

OSPiHandle __DriveRomHandle;

OSPiHandle* osDriveRomInit(void) {
    register s32 status;
    register u32 a;
    register u32 prevInt;
    static u32 D_8000AC70 = 1;

    __osPiGetAccess();

    if (!D_8000AC70) {
        __osPiRelAccess();
        return &__DriveRomHandle;
    }

    D_8000AC70 = 0;
    __DriveRomHandle.type = DEVICE_TYPE_BULK;
    __DriveRomHandle.baseAddress = 0xA6000000;
    __DriveRomHandle.domain = PI_DOMAIN1;
    __DriveRomHandle.speed = 0;
    bzero(&__DriveRomHandle.transferInfo, sizeof(__OSTranxInfo));

    while (status = HW_REG(PI_STATUS_REG, u32), status & (PI_STATUS_BUSY | PI_STATUS_IOBUSY)) {
        ;
    }

    HW_REG(PI_BSD_DOM1_LAT_REG, u32) = 0xFF;
    HW_REG(PI_BSD_DOM1_PGS_REG, u32) = 0;
    HW_REG(PI_BSD_DOM1_RLS_REG, u32) = 3;
    HW_REG(PI_BSD_DOM1_PWD_REG, u32) = 0xFF;

    a = HW_REG(__DriveRomHandle.baseAddress, u32);
    __DriveRomHandle.latency = a & 0xFF;
    __DriveRomHandle.pulse = (a >> 8) & 0xFF;
    __DriveRomHandle.pageSize = (a >> 0x10) & 0xF;
    __DriveRomHandle.relDuration = (a >> 0x14) & 0xF;

    HW_REG(PI_BSD_DOM1_LAT_REG, u32) = (u8)a;
    HW_REG(PI_BSD_DOM1_PGS_REG, u32) = __DriveRomHandle.pageSize;
    HW_REG(PI_BSD_DOM1_RLS_REG, u32) = __DriveRomHandle.relDuration;
    HW_REG(PI_BSD_DOM1_PWD_REG, u32) = __DriveRomHandle.pulse;

    __osCurrentHandle[__DriveRomHandle.domain]->type = __DriveRomHandle.type;
    __osCurrentHandle[__DriveRomHandle.domain]->latency = __DriveRomHandle.latency;
    __osCurrentHandle[__DriveRomHandle.domain]->pageSize = __DriveRomHandle.pageSize;
    __osCurrentHandle[__DriveRomHandle.domain]->relDuration = __DriveRomHandle.relDuration;
    __osCurrentHandle[__DriveRomHandle.domain]->pulse = __DriveRomHandle.pulse;

    prevInt = __osDisableInt();
    __DriveRomHandle.next = __osPiTable;
    __osPiTable = &__DriveRomHandle;
    __osRestoreInt(prevInt);
    __osPiRelAccess();

    return &__DriveRomHandle;
}

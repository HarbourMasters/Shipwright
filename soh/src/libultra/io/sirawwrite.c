#include "global.h"

s32 __osSiRawWriteIo(void* devAddr, u32 val) {
    if (__osSiDeviceBusy()) {
        return -1;
    }
    HW_REG((u32)devAddr, u32) = val;
    return 0;
}

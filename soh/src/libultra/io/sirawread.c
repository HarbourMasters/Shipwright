#include "global.h"

s32 __osSiRawReadIo(void* devAddr, u32* dst) {
    if (__osSiDeviceBusy()) {
        return -1;
    }
    *dst = HW_REG((uintptr_t)devAddr, u32);
    return 0;
}

#include "ultra64.h"

s32 __osSiRawReadIo(uintptr_t devAddr, u32* data) {
    if (__osSiDeviceBusy()) {
        return -1;
    }
    *data = IO_READ(devAddr);
    return 0;
}

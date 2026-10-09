#include "ultra64.h"

s32 __osSiRawWriteIo(uintptr_t devAddr, u32 data) {
    if (__osSiDeviceBusy()) {
        return -1;
    }

    IO_WRITE(devAddr, data);

    return 0;
}

#include "ultra64.h"

s32 osEPiWriteIo(OSPiHandle* handle, uintptr_t devAddr, u32 data) {
    register s32 ret;

    __osPiGetAccess();
    ret = __osEPiRawWriteIo(handle, devAddr, data);
    __osPiRelAccess();

    return ret;
}

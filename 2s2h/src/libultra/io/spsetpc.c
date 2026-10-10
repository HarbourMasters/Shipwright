#include "ultra64.h"

s32 __osSpSetPc(u32 pc) {
    register u32 spStatus = IO_READ(SP_STATUS_REG);

    if (!(spStatus & SP_STATUS_HALT)) {
        return -1;
    }
    IO_WRITE(SP_PC_REG, pc);

    return 0;
}

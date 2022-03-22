#include "global.h"

s32 __osSpSetPc(void* pc) {
    register u32 spStatus = HW_REG(SP_STATUS_REG, u32);

    if (!(spStatus & SP_STATUS_HALT)) {
        return -1;
    } else {
        HW_REG(SP_PC_REG, void*) = pc;
    }

    return 0;
}

#include "global.h"

//! Note that this is not the same as the original libultra
//! osAiSetNextBuffer, see comments in the function

s32 osAiSetNextBuffer(void* buf, size_t size) {
    static u8 D_80130500 = false;
    uintptr_t bufAdjusted = (uintptr_t)buf;
    s32 status;

    if (D_80130500) {
        bufAdjusted = (uintptr_t)buf - 0x2000;
    }
    if ((((uintptr_t)buf + size) & 0x1FFF) == 0) {
        D_80130500 = true;
    } else {
        D_80130500 = false;
    }

    // Originally a call to __osAiDeviceBusy
    status = HW_REG(AI_STATUS_REG, s32);
    if (status & AI_STATUS_AI_FULL) {
        return -1;
    }

    // OS_K0_TO_PHYSICAL replaces osVirtualToPhysical, this replacement
    // assumes that only KSEG0 addresses are given
    HW_REG(AI_DRAM_ADDR_REG, u32) = OS_K0_TO_PHYSICAL(bufAdjusted);
    HW_REG(AI_LEN_REG, u32) = size;
    return 0;
}

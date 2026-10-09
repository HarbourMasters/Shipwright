#include "global.h"

//! Note that this is not the same as the original libultra
//! osAiSetNextBuffer, see comments in the function

#if 0

s32 osAiSetNextBuffer(void* buf, u32 size) {
    static u8 D_801D6010 = false;
    uintptr_t bufAdjusted = (uintptr_t)buf;
    s32 status;

    if (D_801D6010) {
        bufAdjusted = (uintptr_t)buf - 0x2000;
    }
    if ((((uintptr_t)buf + size) & 0x1FFF) == 0) {
        D_801D6010 = true;
    } else {
        D_801D6010 = false;
    }

    // Originally a call to __osAiDeviceBusy
    status = IO_READ(AI_STATUS_REG);
    if (status & AI_STATUS_FIFO_FULL) {
        return -1;
    }

    // OS_K0_TO_PHYSICAL replaces osVirtualToPhysical, this replacement
    // assumes that only KSEG0 addresses are given
    IO_WRITE(AI_DRAM_ADDR_REG, OS_K0_TO_PHYSICAL(bufAdjusted));
    IO_WRITE(AI_LEN_REG, size);
    return 0;
}

#endif // 0

//! Note that the remaining data and functions are unused and not part of the
//! original libultra osAiSetNextBuffer

s16 D_801D6014[] = {
    19720, 18360, 17680, 16320, 14960, 13600, 12240, 10880, 9520, 0,
};
s8 D_801D6028[7][16] = {
    { 0, 1, 2, 2, 1, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3 }, { 0, 1, 2, 3, 3, 2, 1, 4, 5, 6, 7, 8, 7, 6, 5, 4 },
    { 0, 1, 2, 3, 4, 4, 3, 2, 1, 5, 6, 7, 8, 7, 6, 5 }, { 0, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 6, 7, 8, 7, 6 },
    { 0, 1, 2, 3, 4, 5, 6, 6, 5, 4, 3, 2, 1, 7, 8, 7 }, { 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 8 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1 },
};
s16 D_801D6098[7][17] = {
    { 0, 268, 560, 834, 1126, 1418, 1692, 1966, 2240, 2514, 2718, 2922, 3196, 3470, 3744, 4018, 4292 },
    { 0, 268, 560, 834, 1108, 1382, 1674, 1966, 2240, 2514, 2788, 2992, 3196, 3470, 3744, 4018, 4292 },
    { 0, 268, 560, 834, 1108, 1382, 1656, 1930, 2222, 2514, 2788, 3062, 3266, 3470, 3744, 4018, 4292 },
    { 0, 268, 560, 834, 1108, 1382, 1656, 1930, 2204, 2478, 2770, 3062, 3336, 3540, 3744, 4018, 4292 },
    { 0, 268, 560, 834, 1108, 1382, 1656, 1930, 2222, 2514, 2752, 3026, 3318, 3610, 3814, 4018, 4292 },
    { 0, 268, 560, 834, 1108, 1382, 1656, 1930, 2134, 2408, 2682, 2956, 3230, 3504, 3796, 4088, 4292 },
    { 0, 268, 560, 834, 1108, 1382, 1656, 1930, 2134, 2338, 2612, 2886, 3163, 3434, 3708, 4000, 4292 },
};
s16* D_801D6188 = D_801D6098[0];
s8* D_801D618C = D_801D6028[0];

// Unused
void func_80194804(s32 index) {
    D_801D6188 = D_801D6098[index];
    D_801D618C = D_801D6028[index];
}

// Only used below
s16 func_80194840(s32 arg0) {
    s32 i;
    s32 j = 0;

    for (i = 1; i < 16; i++) {
        if (arg0 < D_801D6188[i]) {
            break;
        }
        j++;
    }

    if (i == 16) {
        return 0;
    } else {
        j = D_801D618C[j];
        return D_801D6014[j];
    }
}

// Unused
s32 func_801948B0(s32* arg0, s32* arg1) {
    s32 temp_v0;
    s32 phi_s0 = *arg0;
    s32 phi_s1 = *arg1;

    while (true) {
        temp_v0 = func_80194840(phi_s0);
        if (temp_v0 == 0) {
            return -1;
        }

        if (phi_s1 < temp_v0) {
            break;
        }

        phi_s1 -= temp_v0;
        phi_s0++;
    }

    *arg0 = phi_s0;
    *arg1 = phi_s1;

    return 0;
}

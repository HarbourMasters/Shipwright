#include "global.h"

void MtxConv_F2L(Mtx* m1, MtxF* m2) {
    s32 i;
    s32 j;

    LogUtils_CheckNullPointer("m1", m1, "../mtxuty-cvt.c", 31);
    LogUtils_CheckNullPointer("m2", m2, "../mtxuty-cvt.c", 32);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            s32 value = (m2->mf[i][j] * 0x10000);

            m1->intPart[i][j] = value >> 16;
            m1->fracPart[i][j] = value;
        }
    }
}

void MtxConv_L2F(MtxF* m1, Mtx* m2) {
    LogUtils_CheckNullPointer("m1", m1, "../mtxuty-cvt.c", 55);
    LogUtils_CheckNullPointer("m2", m2, "../mtxuty-cvt.c", 56);
    guMtxL2F(m1, m2);
}

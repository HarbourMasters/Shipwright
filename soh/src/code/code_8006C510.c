#include "global.h"

f32 func_8006C510(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    char pad[0x1C];
    f32 sq = SQ(arg0);
    f32 cube = sq * arg0;

    return (((cube + cube) - sq * 3.0f) + 1.0f) * arg2 + (sq * 3.0f - (cube + cube)) * arg3 +
           ((cube - (sq + sq)) + arg0) * arg4 * arg1 + (cube - sq) * arg5 * arg1;
}

f32 func_8006C5A8(f32 target, TransformData* transData, s32 refIdx) {
    s32 i;
    s32 j;

    if (target <= transData->unk_02) {
        return transData->unk_08;
    }
    if (target >= transData[refIdx - 1].unk_02) {
        return transData[refIdx - 1].unk_08;
    }

    for (i = 0;; i++) {
        j = i + 1;
        if (transData[j].unk_02 > target) {
            if (transData[i].unk_00 & 1) {
                return transData[i].unk_08;
            } else if (transData[i].unk_00 & 2) {
                return transData[i].unk_08 +
                       ((target - (f32)transData[i].unk_02) / ((f32)transData[j].unk_02 - (f32)transData[i].unk_02)) *
                           (transData[j].unk_08 - transData[i].unk_08);
            } else {
                f32 diff = (f32)transData[j].unk_02 - (f32)transData[i].unk_02;
                return func_8006C510((target - transData[i].unk_02) / ((f32)transData[j].unk_02 - transData[i].unk_02),
                                     diff * (1.0f / 30.0f), transData[i].unk_08, transData[j].unk_08,
                                     transData[i].unk_06, transData[j].unk_04);
            }
        }
    }
}

#include "global.h"

u8 D_8012D200[] = {
    0, 1, 2, 3, 4, 5, 6,
};

void func_800C3C20(void) {
    s32 i;

    for (i = 0; (i < ARRAY_COUNT(D_8012D200)) & 0xFFFFFFFF; i++) {
        // The port leaves the file select before the confirm chime ends; the console never did.
        if (D_8012D200[i] == BANK_SYSTEM && Audio_IsSfxPlaying(NA_SE_SY_FSEL_DECIDE_L)) {
            continue;
        }
        Audio_StopSfxByBank(D_8012D200[i]);
    }
}

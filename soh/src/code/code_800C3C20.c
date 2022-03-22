#include "global.h"

u8 D_8012D200[] = {
    0, 1, 2, 3, 4, 5, 6,
};

void func_800C3C20(void) {
    s32 i;

    for (i = 0; (i < ARRAY_COUNT(D_8012D200)) & 0xFFFFFFFF; i++) {
        Audio_StopSfxByBank(D_8012D200[i]);
    }
}

#include "global.h"

void Flags_UnsetAllEnv(GlobalContext* globalCtx) {
    u8 i;

    for (i = 0; i < 20; i++) {
        globalCtx->envFlags[i] = 0;
    }
}

void Flags_SetEnv(GlobalContext* globalCtx, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = 1 << bit;

    globalCtx->envFlags[index] |= mask;
}

void Flags_UnsetEnv(GlobalContext* globalCtx, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = (1 << bit) ^ 0xFFFF;

    globalCtx->envFlags[index] &= mask;
}

s32 Flags_GetEnv(GlobalContext* globalCtx, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = 1 << bit;

    return globalCtx->envFlags[index] & mask;
}

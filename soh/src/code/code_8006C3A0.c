#include "global.h"

void Flags_UnsetAllEnv(PlayState* play) {
    u8 i;

    for (i = 0; i < 20; i++) {
        play->envFlags[i] = 0;
    }
}

void Flags_SetEnv(PlayState* play, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = 1 << bit;

    play->envFlags[index] |= mask;
}

void Flags_UnsetEnv(PlayState* play, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = (1 << bit) ^ 0xFFFF;

    play->envFlags[index] &= mask;
}

s32 Flags_GetEnv(PlayState* play, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = 1 << bit;

    return play->envFlags[index] & mask;
}

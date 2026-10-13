#include "global.h"

void CutsceneFlags_UnsetAll(PlayState* play) {
    u8 i;

    for (i = 0; i < ARRAY_COUNT(play->cutsceneFlags); i++) {
        play->cutsceneFlags[i] = 0;
    }
}

void CutsceneFlags_Set(PlayState* play, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = 1 << bit;

    play->cutsceneFlags[index] |= mask;
}

void CutsceneFlags_Unset(PlayState* play, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = (1 << bit) ^ 0xFFFF;

    play->cutsceneFlags[index] &= mask;
}

s32 CutsceneFlags_Get(PlayState* play, s16 flag) {
    s16 index = flag / 16;
    s16 bit = flag % 16;
    s16 mask = 1 << bit;

    return play->cutsceneFlags[index] & mask;
}

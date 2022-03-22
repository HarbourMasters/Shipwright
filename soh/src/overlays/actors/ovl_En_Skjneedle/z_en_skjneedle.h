#ifndef Z_EN_SKJNEEDLE_H
#define Z_EN_SKJNEEDLE_H

#include "ultra64.h"
#include "global.h"

struct EnSkjneedle;

typedef struct EnSkjneedle {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[0x48];
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ s16 unusedTimer1;
    /* 0x01E2 */ s16 killTimer;
    /* 0x01E4 */ char unk_1E4[4];
} EnSkjneedle; // size = 0x01E8

#endif

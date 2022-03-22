#ifndef Z_EFF_SS_EN_FIRE_H
#define Z_EFF_SS_EN_FIRE_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Actor* actor;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ s16 scale;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s16 flags;
    /* 0x16 */ s16 bodyPart;
} EffectSsEnFireInitParams; // size = 0x18

#endif

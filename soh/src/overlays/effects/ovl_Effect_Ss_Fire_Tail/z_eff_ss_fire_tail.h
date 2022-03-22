#ifndef Z_EFF_SS_FIRE_TAIL_H
#define Z_EFF_SS_FIRE_TAIL_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Actor* actor;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ f32 scale;
    /* 0x14 */ Vec3f unk_14;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ Color_RGBA8 primColor; 
    /* 0x26 */ Color_RGBA8 envColor;
    /* 0x2A */ s16 type;
    /* 0x2C */ s16 bodyPart;
    /* 0x30 */ s32 life;
} EffectSsFireTailInitParams; // size = 0x

#endif

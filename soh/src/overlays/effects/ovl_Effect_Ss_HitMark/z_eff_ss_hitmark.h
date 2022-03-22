#ifndef Z_EFF_SS_HITMARK_H
#define Z_EFF_SS_HITMARK_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ s32 type;
    /* 0x04 */ s16 scale;
    /* 0x08 */ Vec3f pos;
} EffectSsHitMarkInitParams; // size = 0x14

typedef enum {
    EFFECT_HITMARK_WHITE,
    EFFECT_HITMARK_DUST,
    EFFECT_HITMARK_RED,
    EFFECT_HITMARK_METAL
} EffectSsHitmarkType;

#endif

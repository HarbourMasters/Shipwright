#ifndef Z_EFF_SS_ICE_PIECE_H
#define Z_EFF_SS_ICE_PIECE_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 scale;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ s32 life;
} EffectSsIcePieceInitParams; // size = 0xC

#endif

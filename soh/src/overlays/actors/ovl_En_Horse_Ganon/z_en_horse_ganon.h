#ifndef Z_EN_HORSE_GANON_H
#define Z_EN_HORSE_GANON_H

#include "ultra64.h"
#include "global.h"

struct EnHorseGanon;

typedef void (*EnHorseGanonActionFunc)(struct EnHorseGanon*, PlayState*);

typedef struct EnHorseGanon {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 action;
    /* 0x0150 */ s32 currentAnimation;
    /* 0x0154 */ Skin skin;
    /* 0x01E4 */ u8 unk_1E4[0x04];
    /* 0x01E8 */ s32 soundCount;
    /* 0x01EC */ s32 unk_1EC;
    /* 0x01F0 */ u8 unk_1F0[0x04];
    /* 0x01F4 */ f32 unk_1F4;
    /* 0x01F8 */ u8 unk_1F8[0x04];
    /* 0x01FC */ ColliderCylinder colliderBody;
    /* 0x0248 */ ColliderJntSph colliderHead;
    /* 0x0268 */ ColliderJntSphElement headElements[1];
} EnHorseGanon; // size = 0x02A8

#endif

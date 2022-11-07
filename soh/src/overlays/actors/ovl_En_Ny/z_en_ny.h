#ifndef Z_EN_NY_H
#define Z_EN_NY_H

#include "ultra64.h"
#include "global.h"

struct EnNy;

typedef void (*EnNyActionFunc)(struct EnNy*, PlayState*);

typedef struct EnNy {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnNyActionFunc actionFunc;
    /* 0x0150 */ ColliderJntSph collider;
    /* 0x0170 */ ColliderJntSphElement elements[1];
    /* 0x01B0 */ char unk_1B0[0x18];
    /* 0x01C8 */ s16 timer;
    /* 0x01CA */ s16 unk_1CA;
    /* 0x01CC */ s16 hitPlayer;
    /* 0x01CE */ u16 unk_1CE;
    /* 0x01D0 */ u8 unk_1D0;
    /* 0x01D1 */ s8 unk_1D1;
    /* 0x01D4 */ s32 unk_1D4;
    /* 0x01D8 */ s32 unk_1D8;
    /* 0x01DC */ s32 stoneTimer; // Delay for when to attempt to change forms
    /* 0x01E0 */ f32 unk_1E0;
    /* 0x01E4 */ f32 unk_1E4; // This, unk_1E8, and unk_1EC have to do with movement speed
    /* 0x01E8 */ f32 unk_1E8;
    /* 0x01EC */ f32 unk_1EC;
    /* 0x01F0 */ f32 unk_1F0;
    /* 0x01F4 */ f32 unk_1F4;
    /* 0x01F8 */ Vec3f unk_1F8[16];
    u32 epoch;
} EnNy; // size = 0x02B8

#endif

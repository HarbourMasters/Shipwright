#ifndef Z_EN_ANUBICE_FIRE_H
#define Z_EN_ANUBICE_FIRE_H

#include "ultra64.h"
#include "global.h"

struct EnAnubiceFire;

typedef void (*EnAnubiceFireActionFunc)(struct EnAnubiceFire*, PlayState*);

typedef struct EnAnubiceFire {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnAnubiceFireActionFunc actionFunc;
    /* 0x0150 */ f32 scale;
    /* 0x0154 */ f32 unk_154;
    /* 0x0158 */ s16 unused;
    /* 0x015A */ s16 unk_15A;
    /* 0x015C */ s16 unk_15C;
    /* 0x015E */ s16 unk_15E;
    /* 0x0178 */ Vec3f unk_160[6];
    /* 0x01A8 */ ColliderCylinder cylinder;
    /* 0x01F4 */ u32 epoch;
} EnAnubiceFire; // size = 0x01F8

#endif

#ifndef Z_BG_FIRE_WALL_H
#define Z_BG_FIRE_WALL_H

#include "global.h"

struct BgFireWall;

typedef void (*BgFireWallActionFunc)(struct BgFireWall*, PlayState*);

typedef enum {
    /* 0 */ BGFIREWALL_PARAM_0,
    /* 1 */ BGFIREWALL_PARAM_1
} BgFireWallParam;

typedef struct BgFireWall {
    /* 0x000 */ Actor actor;
    /* 0x144 */ BgFireWallActionFunc actionFunc;
    /* 0x148 */ s16 texIndex;
    /* 0x14A */ s16 timer;
    /* 0x14C */ s16 unk_14C;
    /* 0x150 */ f32 step;
    /* 0x154 */ f32 unk_154;
    /* 0x158 */ f32 unk_158;
    /* 0x15C */ f32 unk_15C;
    /* 0x160 */ f32 unk_160;
    /* 0x164 */ ColliderCylinder collider;
} BgFireWall; // size = 0x1B0

#endif // Z_BG_FIRE_WALL_H

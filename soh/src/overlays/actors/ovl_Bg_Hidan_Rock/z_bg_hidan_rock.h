#ifndef Z_BG_HIDAN_ROCK_H
#define Z_BG_HIDAN_ROCK_H

#include "ultra64.h"
#include "global.h"

struct BgHidanRock;

typedef void (*BgHidanRockActionFunc)(struct BgHidanRock*, PlayState*);

typedef struct BgHidanRock {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHidanRockActionFunc actionFunc;
    /* 0x0168 */ u8 type;
    /* 0x0169 */ u8 unk_169;
    /* 0x016A */ s16 timer;
    /* 0x016C */ f32 unk_16C;
    /* 0x0170 */ Vec3f unk_170;
    /* 0x017C */ ColliderCylinder collider;
} BgHidanRock; // size = 0x01C8

#endif

#ifndef Z_BG_MIZU_BWALL_H
#define Z_BG_MIZU_BWALL_H

#include "ultra64.h"
#include "global.h"

struct BgMizuBwall;

typedef void (*BgMizuBwallActionFunc)(struct BgMizuBwall*, PlayState*);

typedef struct BgMizuBwall {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ColliderTris collider;
    /* 0x0184 */ ColliderTrisElement elements[3]; // only ever uses 2
    /* 0x0298 */ BgMizuBwallActionFunc actionFunc;
    /* 0x029C */ f32 yRot;
    /* 0x02A0 */ s32 scrollAlpha1;
    /* 0x02A4 */ s32 scrollAlpha2;
    /* 0x02A8 */ s32 scrollAlpha3;
    /* 0x02AC */ s32 scrollAlpha4;
    /* 0x02B0 */ s32 breakTimer;
    /* 0x02B4 */ char unk_2B4[4];
    /* 0x02B8 */ Gfx* dList;
} BgMizuBwall; // size = 0x02BC

typedef enum {
    MIZUBWALL_FLOOR,
    MIZUBWALL_RUTO_ROOM,
    MIZUBWALL_UNUSED,
    MIZUBWALL_STINGER_ROOM_1,
    MIZUBWALL_STINGER_ROOM_2
} BgMizuBwallType;

#endif

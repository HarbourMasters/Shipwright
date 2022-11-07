#ifndef Z_BG_BOMBWALL_H
#define Z_BG_BOMBWALL_H

#include "ultra64.h"
#include "global.h"

struct BgBombwall;

typedef void (*BgBombwallActionFunc)(struct BgBombwall*, PlayState*);

typedef struct BgBombwall {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ColliderTris collider;
    /* 0x0184 */ ColliderTrisElement colliderItems[3];
    /* 0x0298 */ BgBombwallActionFunc actionFunc;
    /* 0x029C */ Gfx* dList;
    /* 0x02A0 */ s16 unk_2A0;
    /* 0x02A2 */ u8 unk_2A2;
    /* 0x02A3 */ u8 unk_2A3;
} BgBombwall; // size = 0x02A4

#endif

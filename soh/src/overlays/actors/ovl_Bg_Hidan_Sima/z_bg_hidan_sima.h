#ifndef Z_BG_HIDAN_SIMA_H
#define Z_BG_HIDAN_SIMA_H

#include "ultra64.h"
#include "global.h"

struct BgHidanSima;

typedef void (*BgHidanSimaActionFunc)(struct BgHidanSima*, PlayState*);

typedef struct BgHidanSima {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHidanSimaActionFunc actionFunc;
    /* 0x0168 */ s16 timer;
    /* 0x016C */ ColliderJntSph collider;
    /* 0x018C */ ColliderJntSphElement elements[2];
} BgHidanSima; // size = 0x020C

#endif

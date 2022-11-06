#ifndef Z_BG_HIDAN_DALM_H
#define Z_BG_HIDAN_DALM_H

#include "ultra64.h"
#include "global.h"

struct BgHidanDalm;

typedef void (*BgHidanDalmActionFunc)(struct BgHidanDalm*, PlayState*);

typedef struct BgHidanDalm {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHidanDalmActionFunc actionFunc;
    /* 0x0168 */ u8 switchFlag;
    /* 0x016C */ ColliderTris collider;
    /* 0x018C */ ColliderTrisElement colliderItems[4];
} BgHidanDalm; // size = 0x02FC

#endif

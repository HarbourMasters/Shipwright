#ifndef Z_BG_MIZU_UZU_H
#define Z_BG_MIZU_UZU_H

#include "ultra64.h"
#include "global.h"

struct BgMizuUzu;

typedef void (*BgMizuUzuActionFunc)(struct BgMizuUzu*, PlayState*);

typedef struct BgMizuUzu {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgMizuUzuActionFunc actionFunc;
} BgMizuUzu; // size = 0x0168

#endif

#ifndef Z_BG_MORI_RAKKATENJO_H
#define Z_BG_MORI_RAKKATENJO_H

#include "ultra64.h"
#include "global.h"

struct BgMoriRakkatenjo;

typedef void (*BgMoriRakkatenjoActionFunction)(struct BgMoriRakkatenjo*, PlayState*);

typedef struct BgMoriRakkatenjo {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgMoriRakkatenjoActionFunction actionFunc;
    /* 0x0168 */ s32 timer;
    /* 0x016C */ s32 bounceCount;
    /* 0x0170 */ s32 fallCount;
    /* 0x0174 */ s8 moriTexObjIndex;
} BgMoriRakkatenjo; // size = 0x0178

#endif

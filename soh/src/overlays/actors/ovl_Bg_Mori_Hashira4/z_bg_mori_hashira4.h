#ifndef Z_BG_MORI_HASHIRA4_H
#define Z_BG_MORI_HASHIRA4_H

#include "ultra64.h"
#include "global.h"

struct BgMoriHashira4;

typedef void (*BgMoriHashira4ActionFunc)(struct BgMoriHashira4*, PlayState*);

typedef struct BgMoriHashira4 {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgMoriHashira4ActionFunc actionFunc;
    /* 0x0168 */ s8 moriTexObjIndex;
    /* 0x0169 */ s8 switchFlag;
    /* 0x016A */ s16 gateTimer;
} BgMoriHashira4; // size = 0x016C

#endif

#ifndef Z_BG_HAKA_TOMB_H
#define Z_BG_HAKA_TOMB_H

#include "global.h"

struct BgHakaTomb;

typedef void (*BgHakaTombActionFunc)(struct BgHakaTomb*, PlayState*);

typedef struct BgHakaTomb {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgHakaTombActionFunc actionFunc;
    /* 0x160 */ s16 csIdList[1];
} BgHakaTomb; // size = 0x164

#endif // Z_BG_HAKA_TOMB_H

#ifndef Z_BG_ICEFLOE_H
#define Z_BG_ICEFLOE_H

#include "global.h"

struct BgIcefloe;

typedef void (*BgIcefloeActionFunc)(struct BgIcefloe*, PlayState*);

typedef struct BgIcefloe {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgIcefloeActionFunc actionFunc;
    /* 0x160 */ s32 timer;
} BgIcefloe; // size = 0x164

#endif // Z_BG_ICEFLOE_H

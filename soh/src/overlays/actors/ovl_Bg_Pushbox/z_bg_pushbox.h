#ifndef Z_BG_PUSHBOX_H
#define Z_BG_PUSHBOX_H

#include "ultra64.h"
#include "global.h"

struct BgPushbox;

typedef void (*BgPushboxActionFunc)(struct BgPushbox*, PlayState*);

typedef struct BgPushbox {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgPushboxActionFunc actionFunc;
} BgPushbox; // size = 0x0168

#endif

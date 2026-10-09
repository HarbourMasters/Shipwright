#ifndef Z_BG_F40_FLIFT_H
#define Z_BG_F40_FLIFT_H

#include "global.h"

struct BgF40Flift;

typedef void (*BgF40FliftActionFunc)(struct BgF40Flift*, PlayState*);

typedef struct BgF40Flift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgF40FliftActionFunc actionFunc;
    /* 0x160 */ s32 timer;
} BgF40Flift; // size = 0x164

#endif // Z_BG_F40_FLIFT_H

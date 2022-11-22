#ifndef Z_BG_MORI_IDOMIZU_H
#define Z_BG_MORI_IDOMIZU_H

#include "ultra64.h"
#include "global.h"

struct BgMoriIdomizu;

typedef void (*BgMoriIdomizuActionFunc)(struct BgMoriIdomizu*, PlayState*);

typedef struct BgMoriIdomizu {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgMoriIdomizuActionFunc actionFunc;
    /* 0x0150 */ f32 targetWaterLevel;
    /* 0x0154 */ s32 prevSwitchFlagSet;
    /* 0x0158 */ s16 isLoaded;
    /* 0x015A */ s16 drainTimer;
    /* 0x015C */ s8 moriTexObjIndex;
} BgMoriIdomizu; // size = 0x0160

#endif

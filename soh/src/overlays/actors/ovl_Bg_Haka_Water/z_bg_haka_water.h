#ifndef Z_BG_HAKA_WATER_H
#define Z_BG_HAKA_WATER_H

#include "ultra64.h"
#include "global.h"

struct BgHakaWater;

typedef void (*BgHakaWaterActionFunc)(struct BgHakaWater*, PlayState*);

typedef struct BgHakaWater {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgHakaWaterActionFunc actionFunc;
    /* 0x0150 */ u8 isLowered;
} BgHakaWater; // size = 0x0154

#endif

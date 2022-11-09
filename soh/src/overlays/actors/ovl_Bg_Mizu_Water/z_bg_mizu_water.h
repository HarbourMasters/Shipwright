#ifndef Z_BG_MIZU_WATER_H
#define Z_BG_MIZU_WATER_H

#include "ultra64.h"
#include "global.h"

struct BgMizuWater;

typedef void (*BgMizuWaterActionFunc)(struct BgMizuWater*, PlayState*);

typedef struct BgMizuWater {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgMizuWaterActionFunc actionFunc;
    /* 0x0150 */ s32 type;
    /* 0x0154 */ f32 targetY;
    /* 0x0158 */ f32 baseY;
    /* 0x015C */ s32 switchFlag; // only used for types 2-4
} BgMizuWater; // size = 0x0160

#define WATER_TEMPLE_WATER_F3_Y 765.0f
#define WATER_TEMPLE_WATER_F2_Y 445.0f
#define WATER_TEMPLE_WATER_F1_Y -15.0f
#define WATER_TEMPLE_WATER_B1_Y -835.0f

#define WATER_TEMPLE_WATER_F3_FLAG 0x1E
#define WATER_TEMPLE_WATER_F2_FLAG 0x1D
#define WATER_TEMPLE_WATER_F1_FLAG 0x1C

#endif

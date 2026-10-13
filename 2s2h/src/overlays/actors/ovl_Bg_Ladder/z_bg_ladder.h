#ifndef Z_BG_LADDER_H
#define Z_BG_LADDER_H

#include "global.h"

#define BGLADDER_GET_SIZE(thisx) ((thisx)->params & 0xFF)
#define BGLADDER_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0xFF)

struct BgLadder;

typedef void (*BgLadderActionFn)(struct BgLadder*, PlayState*);

typedef struct BgLadder {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgLadderActionFn action;
    /* 0x160 */ s16 alpha;
    /* 0x162 */ s16 switchFlag;
} BgLadder; // size = 0x164

typedef enum {
    /* 0 */ LADDER_SIZE_12RUNG,
    /* 1 */ LADDER_SIZE_16RUNG,
    /* 2 */ LADDER_SIZE_20RUNG,
    /* 3 */ LADDER_SIZE_24RUNG
} BgLadderSize;

#endif // Z_BG_LADDER_H

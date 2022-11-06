#ifndef Z_BG_SPOT03_TAKI_H
#define Z_BG_SPOT03_TAKI_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    WATERFALL_CLOSED,
    WATERFALL_OPENING_IDLE,
    WATERFALL_OPENING_ANIMATED,
    WATERFALL_OPENED,
    WATERFALL_CLOSING
} BgSpot03TakiState;

struct BgSpot03Taki;

typedef void (*BgSpot03TakiActionFunc)(struct BgSpot03Taki*, PlayState*);

typedef struct BgSpot03Taki {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot03TakiActionFunc actionFunc;
    /* 0x0168 */ s16 timer;
    /* 0x016A */ u8 state;
    /* 0x016C */ u16 switchFlag;
    /* 0x0170 */ f32 openingAlpha;
    /* 0x0174 */ u8 bufferIndex;
} BgSpot03Taki; // size = 0x0178

#endif

#ifndef Z_BG_IKANA_DHARMA_H
#define Z_BG_IKANA_DHARMA_H

#include "global.h"

#define BGIKANADHARMA_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)
#define BGIKANADHARMA_IS_CHILD(thisx) (((thisx)->params >> 5) & 1)
#define BGIKANADHARMA_NUM_SEGMENTS(thisx) ((thisx)->params & 0xF)
#define BGIKANADHARMA_PARAMS(switchFlag, isChild, numSegments) (((switchFlag & 0x7F) << 8) | ((isChild & 1) << 5) | (numSegments & 0xF))

struct BgIkanaDharma;

typedef void (*BgIkanaDharmaActionFunc)(struct BgIkanaDharma*, PlayState*);

typedef struct BgIkanaDharma {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ BgIkanaDharmaActionFunc actionFunc;
    /* 0x1AC */ s8 cutsceneFramesRemaining;
} BgIkanaDharma; // size = 0x1B0

#endif // Z_BG_IKANA_DHARMA_H

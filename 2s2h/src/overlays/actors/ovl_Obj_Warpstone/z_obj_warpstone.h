#ifndef Z_OBJ_WARPSTONE_H
#define Z_OBJ_WARPSTONE_H

#include "global.h"

struct ObjWarpstone;

// 2S2H [Port] Unnamed parameters so this header can be included from C++
typedef s32 (*ObjWarpstoneActionFunc)(struct ObjWarpstone*, PlayState*);

typedef enum {
    /* 0 */ SEK_MODEL_CLOSED,
    /* 1 */ SEK_MODEL_OPENED
} ObjectSekModels;

typedef enum {
    /* 25 */ OBJ_WARPSTONE_TIMER_OPEN_THRESHOLD = 25,
    /* 66 */ OBJ_WARPSTONE_TIMER_ACTIVATE_THRESHOLD = 66
} ObjWarpstoneTimerCutoffs;

typedef struct ObjWarpstone {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider; 
    /* 0x1A8 */ u8 isTalking;
    /* 0x1A9 */ u8 openingCSTimer;
    /* 0x1AA */ u8 modelIndex;
    /* 0x1AC */ ObjWarpstoneActionFunc actionFunc;
} ObjWarpstone; // size = 0x1B0

#define OBJ_WARPSTONE_GET_OWL_WARP_ID(thisx) ((u16)((thisx)->params & 0xF))

#endif // Z_OBJ_WARPSTONE_H

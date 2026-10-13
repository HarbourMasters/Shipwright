#ifndef Z_BG_ASTR_BOMBWALL_H
#define Z_BG_ASTR_BOMBWALL_H

#include "global.h"

struct BgAstrBombwall;

#define BGASTRBOMBWALL_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

typedef void (*BgAstrBombwallActionFunc)(struct BgAstrBombwall*, PlayState*);
typedef struct BgAstrBombwall {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgAstrBombwallActionFunc actionFunc;
    /* 0x160 */ ColliderTris collider;
    /* 0x180 */ ColliderTrisElement colliderElements[2];
    /* 0x238 */ s16 csIdList[1];
} BgAstrBombwall;
#endif // Z_BG_ASTR_BOMBWALL_H

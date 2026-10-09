#ifndef Z_BG_IKANA_BOMBWALL_H
#define Z_BG_IKANA_BOMBWALL_H

#include "global.h"

struct BgIkanaBombwall;

typedef void (*BgIkanaBombwallActionFunc)(struct BgIkanaBombwall*, PlayState*);

#define BGIKANABOMBWALL_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define BGIKANABOMBWALL_GET_100(thisx) (((thisx)->params >> 8) & 1)

typedef struct BgIkanaBombwall {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ BgIkanaBombwallActionFunc actionFunc;
    /* 0x1AC */ s8 unk_1AC;
} BgIkanaBombwall; // size = 0x1B0

#endif // Z_BG_IKANA_BOMBWALL_H

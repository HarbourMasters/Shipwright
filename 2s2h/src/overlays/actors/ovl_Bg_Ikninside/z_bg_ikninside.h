#ifndef Z_BG_IKNINSIDE_H
#define Z_BG_IKNINSIDE_H

#include "global.h"

struct BgIkninside;

#define DMIKNINSIDE_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)

typedef void (*BgIkninsideActionFunc)(struct BgIkninside*, PlayState*);

typedef struct BgIkninside {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ UNK_TYPE1 pad_1A8[2];
    /* 0x1AA */ s16 timer;
    /* 0x1AC */ BgIkninsideActionFunc actionFunc;
} BgIkninside; // size = 0x1B0

#endif // Z_BG_IKNINSIDE_H

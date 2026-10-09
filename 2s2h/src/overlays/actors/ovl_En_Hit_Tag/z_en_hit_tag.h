#ifndef Z_EN_HIT_TAG_H
#define Z_EN_HIT_TAG_H

#include "global.h"

struct EnHitTag;

typedef void (*EnHitTagActionFunc)(struct EnHitTag*, PlayState*);

#define ENHITTAG_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)

typedef struct EnHitTag {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ EnHitTagActionFunc actionFunc;
} EnHitTag; // size = 0x194

#endif // Z_EN_HIT_TAG_H

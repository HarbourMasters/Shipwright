#ifndef Z_EN_HAKUROCK_H
#define Z_EN_HAKUROCK_H

#include "global.h"

struct EnHakurock;

#define EN_HAKUROCK_GET_TYPE(thisx) ((thisx)->params)

typedef void (*EnHakurockActionFunc)(struct EnHakurock*, PlayState*);

typedef enum {
    /* 0x0 */ EN_HAKUROCK_TYPE_NONE,
    /* 0x1 */ EN_HAKUROCK_TYPE_BOULDER,
    /* 0x2 */ EN_HAKUROCK_TYPE_FALLING_STALACTITE,
    /* 0x3 */ EN_HAKUROCK_TYPE_STUCK_STALACTITE,
    /* 0x4 */ EN_HAKUROCK_TYPE_LARGE_STALACTITE
} EnHakurockType;

typedef struct EnHakurock {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnHakurockActionFunc actionFunc;
    /* 0x148 */ s32 timer;
    /* 0x14C */ ColliderCylinder collider;
} EnHakurock; // size = 0x198

#endif // Z_EN_HAKUROCK_H

#ifndef Z_EN_MS_H
#define Z_EN_MS_H

#include "ultra64.h"
#include "global.h"

struct EnMs;

typedef void (*EnMsActionFunc)(struct EnMs*, PlayState*);

typedef struct EnMs {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[9];
    /* 0x01C6 */ Vec3s morphTable[9];
    /* 0x01FC */ EnMsActionFunc actionFunc;
    /* 0x0200 */ ColliderCylinder collider;
    /* 0x024C */ s16 activeTimer;
} EnMs; // size = 0x0250

#endif

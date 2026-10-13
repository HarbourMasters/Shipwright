#ifndef Z_EN_MM_H
#define Z_EN_MM_H

#include "global.h"

struct EnMm;

typedef void (*EnMmActionFunc)(struct EnMm*, PlayState*);

typedef struct EnMm {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ s16 unk_190;
    /* 0x194 */ EnMmActionFunc actionFunc;
} EnMm; // size = 0x198

#endif // Z_EN_MM_H

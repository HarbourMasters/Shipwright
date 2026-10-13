#ifndef Z_EN_MS_H
#define Z_EN_MS_H

#include "global.h"
#include "objects/object_ms/object_ms.h"

struct EnMs;

typedef void (*EnMsActionFunc)(struct EnMs*, PlayState*);

typedef struct EnMs {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BEAN_SALESMAN_LIMB_MAX];
    /* 0x1BE */ Vec3s morphTable[BEAN_SALESMAN_LIMB_MAX];
    /* 0x1F4 */ EnMsActionFunc actionFunc;
    /* 0x1F8 */ ColliderCylinder collider;
} EnMs; // size = 0x244

#endif // Z_EN_MS_H

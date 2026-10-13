#ifndef Z_EN_DRS_H
#define Z_EN_DRS_H

#include "global.h"
#include "objects/object_drs/object_drs.h"
#include "objects/object_msmo/object_msmo.h"

struct EnDrs;

typedef void (*EnDrsActionFunc)(struct EnDrs*, PlayState*);

typedef struct EnDrs {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnDrsActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ Vec3s jointTable[WEDDING_DRESS_MANNEQUIN_LIMB_MAX];
    /* 0x1F0 */ Vec3s morphTable[WEDDING_DRESS_MANNEQUIN_LIMB_MAX];
    /* 0x208 */ s8 moonMaskObjectSlot;
} EnDrs; // size = 0x20C */

#endif // Z_EN_DRS_H

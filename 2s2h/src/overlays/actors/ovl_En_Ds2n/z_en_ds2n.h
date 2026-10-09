#ifndef Z_EN_DS2N_H
#define Z_EN_DS2N_H

#include "global.h"
#include "objects/object_ds2n/object_ds2n.h"

struct EnDs2n;

typedef void (*EnDs2nActionFunc)(struct EnDs2n*, PlayState*);

typedef struct EnDs2n {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider; // unused
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnDs2nActionFunc actionFunc;
    /* 0x1D8 */ s16 fidgetTableY[DS2N_LIMB_MAX];
    /* 0x1FE */ s16 fidgetTableZ[DS2N_LIMB_MAX];
    /* 0x224 */ Vec3s headRot;
    /* 0x22A */ Vec3s torsoRot; // set by function, but not applied in limbdraw
    /* 0x230 */ s16 blinkState;
    /* 0x232 */ s16 blinkTimer;

} EnDs2n; // size = 0x234

#endif // Z_EN_DS2N_H

#ifndef Z_EN_WDHAND_H
#define Z_EN_WDHAND_H

#include "global.h"
#include "objects/object_wdhand/object_wdhand.h"

struct EnWdhand;

typedef void (*EnWdhandActionFunc)(struct EnWdhand*, PlayState*);

#define EN_WDHAND_NUM_SEGMENTS 3
#define EN_WDHAND_NUM_COLLIDER_ELEMENTS (2 * EN_WDHAND_NUM_SEGMENTS + 1)

#define EN_WDHAND_CUT_POINT  0
#define EN_WDHAND_HAND_POINT 1

#define EN_WDHAND_INIT_VELOCITY_MAX 0x7F
#define EN_WDHAND_GET_Y_INIT_VELOCITY(thisx) (((thisx)->params >> 0) & 0x7F)
#define EN_WDHAND_GET_Z_INIT_VELOCITY(thisx) (((thisx)->params >> 7) & 0x7F)

typedef struct EnWdhand {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[DEXIHAND_LIMB_MAX];
    /* 0x1B8 */ Vec3s morphTable[DEXIHAND_LIMB_MAX];
    /* 0x1E8 */ EnWdhandActionFunc actionFunc;
    /* 0x1EC */ s16 limbIndexBeforeCut;
    /* 0x1EE */ s16 limbIndexAfterCut;
    /* 0x1F0 */ s16 timer;
    /* 0x1F2 */ Vec3s limbRotations[EN_WDHAND_NUM_SEGMENTS + 1];
    /* 0x20C */ f32 globalLimbScaleFactor;
    /* 0x20C */ f32 limbScaleFactors[EN_WDHAND_NUM_SEGMENTS];
    /* 0x21C */ MtxF relativeToWorldTransform;
    /* 0x25C */ Vec3f endPoints[2];
    /* 0x274 */ ColliderJntSph collider;
    /* 0x294 */ ColliderJntSphElement colliderElements[EN_WDHAND_NUM_COLLIDER_ELEMENTS];
} EnWdhand; // size = 0x454

#endif // Z_EN_WDHAND_H

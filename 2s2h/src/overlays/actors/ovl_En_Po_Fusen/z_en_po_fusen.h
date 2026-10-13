#ifndef Z_EN_PO_FUSEN_H
#define Z_EN_PO_FUSEN_H

#include "global.h"
#include "objects/object_po_fusen/object_po_fusen.h"

struct EnPoFusen;

// The version used in the credits, where we see romani shoot three in a row, are on timer, this is the fuse length.
#define POE_BALLOON_GET_FUSE_LEN(thisx) ((thisx)->params & 0x3FF)
#define POE_BALLOON_IS_FUSE_TYPE(thisx) ((thisx)->params & 0x8000)

typedef void (*EnPoFusenActionFunc)(struct EnPoFusen*, PlayState*);

typedef struct EnPoFusen {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime anime;
    /* 0x188 */ EnPoFusenActionFunc actionFunc;
    /* 0x18C */ ColliderSphere collider;
    /* 0x1E4 */ Vec3s jointTable[POE_BALLOON_LIMB_MAX];
    /* 0x220 */ Vec3s morphTable[POE_BALLOON_LIMB_MAX];
    /* 0x25C */ s16 randScaleChange;
    /* 0x25E */ s16 randXZRotChange;
    /* 0x260 */ s16 randYRotChange;
    /* 0x262 */ s16 avgBaseRotation;
    /* 0x264 */ s16 limbRotYRightUpperArm;
    /* 0x266 */ s16 limb46Rot;
    /* 0x268 */ s16 limb57Rot;
    /* 0x26A */ s16 limbRotLeftHand;
    /* 0x26C */ s16 limbRotChainAndLantern;
    /* 0x26E */ s16 randBaseRotChange;
    /* 0x270 */ s16 fuse;
} EnPoFusen; // size = 0x274

#endif // Z_EN_PO_FUSEN_H

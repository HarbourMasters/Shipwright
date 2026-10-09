#ifndef Z_EN_MA4_H
#define Z_EN_MA4_H

#include "global.h"
#include "objects/object_ma1/object_ma1.h"

struct EnMa4;

typedef void (*EnMa4ActionFunc)(struct EnMa4*, PlayState*);

#define ENMA_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFF00) >> 8)

typedef struct EnMa4 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnMa4ActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ NpcInteractInfo interactInfo;
    /* 0x200 */ Vec3s* pathPoints;
    /* 0x204 */ Vec3s jointTable[ROMANI_LIMB_MAX];
    /* 0x28E */ UNK_TYPE1 unk28E[0x6];
    /* 0x294 */ Vec3s morphTable[ROMANI_LIMB_MAX];
    /* 0x31E */ UNK_TYPE1 unk31E[0x6];
    /* 0x324 */ s16 waypointIndex;
    /* 0x326 */ s16 pathPointsCount;
    /* 0x328 */ UNK_TYPE1 unk_328[0x2];
    /* 0x32A */ s16 blinkTimer;
    /* 0x33C */ s16 overrideEyeTexIndex;
    /* 0x32E */ s16 eyeTexIndex;
    /* 0x330 */ s16 mouthTexIndex;
    /* 0x332 */ s16 type;
    /* 0x334 */ u16 unk_334; // set but not used
    /* 0x336 */ s16 state;
    /* 0x338 */ s16 poppedBalloonCounter; // modified by EnPoFusen
    /* 0x33A */ s16 hasBow;
    /* 0x33C */ s16 animTimer;
    /* 0x33E */ u16 textId;
} EnMa4; // size = 0x340

#endif // Z_EN_MA4_H

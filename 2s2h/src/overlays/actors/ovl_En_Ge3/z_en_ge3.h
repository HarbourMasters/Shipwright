#ifndef Z_EN_GE3_H
#define Z_EN_GE3_H

#include "global.h"
#include "z64snap.h"
#include "objects/object_geldb/object_geldb.h"

struct EnGe3;

typedef void (*EnGe3ActionFunc)(struct EnGe3*, PlayState*);

#define GERUDO_AVEIL_GET_PATH(thisx) (((thisx)->params & 0xFC00) >> 10)
#define GERUDO_AVEIL_GET_EXIT(thisx) (((thisx)->params & 0x1F0) >> 4)
#define GERUDO_AVEIL_GET_TYPE(thisx) ((thisx)->params & 1)

#define GERUDO_AVEIL_PATH_INDEX_NONE 0x3F

typedef enum GerudoAveilType {
    /* 0 */ GERUDO_AVEIL_TYPE_0, // Unused, just walks forwards
    /* 1 */ GERUDO_AVEIL_TYPE_AVEILS_CHAMBER
} GerudoAveilType;

typedef enum GerudoAveilCueId {
    /* -1 */ GERUDO_AVEIL_CUEID_NONE = -1,
    /*  0 */ GERUDO_AVEIL_CUEID_0, // Unused
    /*  1 */ GERUDO_AVEIL_CUEID_WAIT,
    /*  2 */ GERUDO_AVEIL_CUEID_PAUSE,
    /*  3 */ GERUDO_AVEIL_CUEID_INSTRUCT,
    /*  4 */ GERUDO_AVEIL_CUEID_DEMAND,
    /*  5 */ GERUDO_AVEIL_CUEID_DISMISS,
    /*  6 */ GERUDO_AVEIL_CUEID_BEG,
    /*  7 */ GERUDO_AVEIL_CUEID_BEEHIVE_RUN_AWAY,
    /*  8 */ GERUDO_AVEIL_CUEID_BEEHIVE_VANISH
} GerudoAveilCueId;

typedef struct EnGe3 {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ SkelAnime skelAnime;
    /* 0x1D8 */ Vec3s jointTable[GERUDO_RED_LIMB_MAX];
    /* 0x268 */ Vec3s morphTable[GERUDO_RED_LIMB_MAX];
    /* 0x2F8 */ s16 eyeIndex;
    /* 0x2FA */ s16 blinkTimer;
    /* 0x2FC */ Path* path;
    /* 0x300 */ s32 curPointIndex;
    /* 0x304 */ Vec3s headRot;
    /* 0x30A */ Vec3s torsoRot;
    /* 0x310 */ s16 stateFlags; // Set and not used, named based on OoT use
    /* 0x312 */ s16 animIndex;
    /* 0x314 */ s16 cueId;
    /* 0x316 */ s16 actionTimer;
    /* 0x318 */ s16 screamTimer;
    /* 0x31C */ EnGe3ActionFunc actionFunc;
} EnGe3; // size = 0x320

#endif // Z_EN_GE3_H

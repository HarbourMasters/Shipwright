#ifndef Z_EN_GE1_H
#define Z_EN_GE1_H

#include "global.h"
#include "z64snap.h"
#include "objects/object_ge1/object_ge1.h"

#define GERUDO_WHITE_GET_TYPE(thisx) (((thisx)->params) & 0xF)
#define GERUDO_WHITE_GET_PATH_INDEX(thisx) ((((thisx)->params) & 0xFC00) >> 10)

#define GERUDO_WHITE_PATH_INDEX_NONE 0x3F

//! Only the first type is used
typedef enum {
    /* 0 */ GERUDO_WHITE_TYPE_CUTSCENE,
    /* 1 */ GERUDO_WHITE_TYPE_HAIR_STRAIGHT,
    /* 2 */ GERUDO_WHITE_TYPE_HAIR_SPIKEY
} GerudoWhiteType;

struct EnGe1;

typedef void (*EnGe1ActionFunc)(struct EnGe1*, PlayState*);

typedef struct EnGe1 {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ SkelAnime skelAnime;
    /* 0x1D8 */ Vec3f waistPos;
    /* 0x1E4 */ Vec3s jointTable[GERUDO_WHITE_LIMB_MAX];
    /* 0x244 */ Vec3s morphTable[GERUDO_WHITE_LIMB_MAX];
    /* 0x2A4 */ Vec3s headRot;
    /* 0x2AA */ Vec3s torsoRot;
    /* 0x2B0 */ Path* path;
    /* 0x2B4 */ s32 curPointIndex;
    /* 0x2B8 */ s16 eyeIndex;
    /* 0x2BA */ s16 blinkTimer;
    /* 0x2BC */ u16 stateFlags;
    /* 0x2BE */ s16 animIndex;
    /* 0x2C0 */ s16 cueId;
    /* 0x2C2 */ s16 screamTimer;
    /* 0x2C4 */ u8 hairstyle;
    /* 0x2C8 */ EnGe1ActionFunc actionFunc;
} EnGe1; // size = 0x2CC

#endif // Z_EN_GE1_H

#ifndef Z_EN_ZO_H
#define Z_EN_ZO_H

#include "global.h"
#include "objects/object_zo/object_zo.h"

struct EnZo;

typedef void (*EnZoActionFunc)(struct EnZo*, PlayState*);

#define ENZO_GET_PATH_INDEX(thisx) (((thisx)->params & 0x7E00) >> 9)

#define ENZO_PATH_INDEX_NONE 0x3F

typedef enum EnZoBodyPart {
    /*  0 */ ZORA_BODYPART_0,
    /*  1 */ ZORA_BODYPART_1,
    /*  2 */ ZORA_BODYPART_2,
    /*  3 */ ZORA_BODYPART_3,
    /*  4 */ ZORA_BODYPART_4,
    /*  5 */ ZORA_BODYPART_5,
    /*  6 */ ZORA_BODYPART_6,
    /*  7 */ ZORA_BODYPART_7,
    /*  8 */ ZORA_BODYPART_8,
    /*  9 */ ZORA_BODYPART_9,
    /* 10 */ ZORA_BODYPART_10,
    /* 11 */ ZORA_BODYPART_11,
    /* 12 */ ZORA_BODYPART_12,
    /* 13 */ ZORA_BODYPART_13,
    /* 14 */ ZORA_BODYPART_14,
    /* 15 */ ZORA_BODYPART_MAX
} EnZoBodyPart;

typedef struct EnZo {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnZoActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x194 */ ColliderCylinder collider;
    /* 0x1D8 */ UNK_TYPE1 unk_1D8[0x4];
    /* 0x1DC */ Path* path;
    /* 0x1E0 */ s16 waypoint;
    /* 0x1E4 */ Vec3f leftFootPos;
    /* 0x1F0 */ Vec3f rightFootPos;
    /* 0x1FC */ u8 isLeftFootGrounded;
    /* 0x1FD */ u8 isRightFootGrounded;
    /* 0x1FE */ Vec3s jointTable[ZORA_LIMB_MAX];
    /* 0x276 */ Vec3s morphTable[ZORA_LIMB_MAX];
    /* 0x2EE */ Vec3s trackTarget;
    /* 0x2F4 */ Vec3s headRot;
    /* 0x2FA */ Vec3s upperBodyRot;
    /* 0x300 */ UNK_TYPE1 unk_300[0x12];
    /* 0x312 */ s16 fidgetTableY[ZORA_LIMB_MAX];
    /* 0x33A */ s16 fidgetTableZ[ZORA_LIMB_MAX];
    /* 0x364 */ Vec3f bodyPartsPos[ZORA_BODYPART_MAX];
    /* 0x41A */ UNK_TYPE1 unk_41A[0x6];
    /* 0x41E */ s16 eyeIndex;
    /* 0x420 */ s16 blinkTimer;
} EnZo; // size = 0x424

#endif // Z_EN_ZO_H

#ifndef Z_EN_BIGOKUTA_H
#define Z_EN_BIGOKUTA_H

#include "global.h"
#include "z64snap.h"
#include "assets/objects/object_bigokuta/object_bigokuta.h"

struct EnBigokuta;

typedef void (*EnBigokutaActionFunc)(struct EnBigokuta*, PlayState*);

#define EN_BIGOKUTA_GET_SWITCH_FLAG(thisx) ((thisx)->params)

typedef enum BigokutaBodyPart {
    /*  0 */ BIGOKUTA_BODYPART_0,
    /*  1 */ BIGOKUTA_BODYPART_1,
    /*  2 */ BIGOKUTA_BODYPART_2,
    /*  3 */ BIGOKUTA_BODYPART_3,
    /*  4 */ BIGOKUTA_BODYPART_4,
    /*  5 */ BIGOKUTA_BODYPART_5,
    /*  6 */ BIGOKUTA_BODYPART_6,
    /*  7 */ BIGOKUTA_BODYPART_7,
    /*  8 */ BIGOKUTA_BODYPART_8,
    /*  9 */ BIGOKUTA_BODYPART_9,
    /* 10 */ BIGOKUTA_BODYPART_10,
    /* 11 */ BIGOKUTA_BODYPART_11,
    /* 12 */ BIGOKUTA_BODYPART_12,
    /* 13 */ BIGOKUTA_BODYPART_MAX
} BigokutaBodyPart;

typedef struct EnBigokuta {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ EnBigokutaActionFunc actionFunc;
    /* 0x190 */ u8 drawDmgEffType;
    /* 0x192 */ s16 timer;
    /* 0x194 */ s16 subCamId;
    /* 0x196 */ s16 csId;
    /* 0x198 */ Vec3s jointTable[BIGOKUTA_LIMB_MAX];
    /* 0x210 */ Vec3s morphTable[BIGOKUTA_LIMB_MAX];
    /* 0x288 */ f32 drawDmgEffAlpha;
    /* 0x28C */ f32 drawDmgEffScale;
    /* 0x290 */ f32 drawDmgEffFrozenSteamScale; // only affects DRAWEFFECT_ICE
    /* 0x294 */ Vec3f playerPos;
    /* 0x2A0 */ Vec3f playerHoldPos;
    /* 0x2AC */ Vec3f subCamAt;
    /* 0x2B8 */ Vec3f subCamEye;
    /* 0x2C4 */ Vec3f bodyPartsPos[BIGOKUTA_BODYPART_MAX];
    /* 0x360 */ ColliderCylinder shellCollider;
    /* 0x3AC */ ColliderCylinder bodyCollider;
} EnBigokuta; // size = 0x3F8

#endif // Z_EN_BIGOKUTA_H

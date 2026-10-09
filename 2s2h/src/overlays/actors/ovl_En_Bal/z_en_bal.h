#ifndef Z_EN_BAL_H
#define Z_EN_BAL_H

#include "global.h"
#include "z64snap.h"
#include "objects/object_bal/object_bal.h"

#define TINGLE_GET_MAP_ID(thisx) ((thisx)->params & 0xFF)

struct EnBal;

typedef void (*EnBalActionFunc)(struct EnBal*, PlayState*);

typedef struct EnBal {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ EnBalActionFunc actionFunc;
    /* 0x190 */ ColliderJntSph collider;
    /* 0x1B0 */ ColliderJntSphElement colliderElements[1];
    /* 0x1F0 */ Vec3f balloonScale;
    /* 0x1FC */ s32 eyeTexIndex;
    /* 0x200 */ Vec3s jointTable[TINGLE_LIMB_MAX];
    /* 0x2C6 */ Vec3s morphTable[TINGLE_LIMB_MAX];
    /* 0x38C */ Vec3s headRot;
    /* 0x392 */ Vec3s torsoRot;
    /* 0x398 */ s16 eyeTimer;
    /* 0x39A */ s16 forceEyesShut;
    /* 0x39C */ s16 watchTarget;
    /* 0x39E */ s16 skyFloatPhase;
    /* 0x3A0 */ s16 textId;
    /* 0x3A2 */ UNK_TYPE1 unk3A2[0x2];
    /* 0x3A4 */ s16 balloonAction;
    /* 0x3A6 */ s16 idleAnimStage;
    /* 0x3A8 */ s16 timer;
    /* 0x3AA */ UNK_TYPE1 unk3AA[0x2]; 
    /* 0x3AC */ s16 purchaseMapId;
    /* 0x3AE */ s16 locationMapId;    
    /* 0x3B0 */ s16 inflateEarly;
    /* 0x3B2 */ u8 isTalking;
} EnBal; /* size = 0x3B4 */

#endif // Z_EN_BAL_H

#ifndef Z_EN_BAISEN_H
#define Z_EN_BAISEN_H

#include "global.h"
#include "objects/object_bai/object_bai.h"

struct EnBaisen;

typedef void (*EnBaisenActionFunc)(struct EnBaisen*, PlayState*);

typedef struct EnBaisen {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_BAI_LIMB_MAX];
    /* 0x200 */ Vec3s morphTable[OBJECT_BAI_LIMB_MAX];
    /* 0x278 */ EnBaisenActionFunc actionFunc;
    /* 0x27C */ s16 headRotY;
    /* 0x27E */ s16 headRotX;
    /* 0x280 */ s16 headRotZ;
    /* 0x282 */ s16 headRotYTarget;
    /* 0x284 */ s16 headRotXTarget;
    /* 0x288 */ s32 animIndex;
    /* 0x28C */ s32 paramCopy;
    /* 0x290 */ s32 unk290;
    /* 0x294 */ s16 unusedCounter;
    /* 0x298 */ f32 animEndFrame;
    /* 0x29C */ s16 unk29C;
    /* 0x29E */ s16 unk29E;
    /* 0x2A0 */ s16 textIdIndex;
    /* 0x2A4 */ Actor* targetActor;
    /* 0x2A8 */ Actor* heishiPointer;
    /* 0x2AC */ s32 cutsceneState;
    /* 0x2B0 */ ColliderCylinder collider;
} EnBaisen; // size = 0x2FC

#endif // Z_EN_BAISEN_H

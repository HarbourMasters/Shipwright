#ifndef Z_EN_MM3_H
#define Z_EN_MM3_H

#include "global.h"
#include "objects/object_mm/object_mm.h"

struct EnMm3;

typedef void (*EnMm3ActionFunc)(struct EnMm3*, PlayState*);

typedef enum {
    /* 0 */ POSTMAN_MINIGAME_BUNNY_HOOD_OFF,
    /* 2 */ POSTMAN_MINIGAME_BUNNY_HOOD_ON = 2
} PostmanMinigameBunnyHoodState;

typedef struct EnMm3 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ ColliderCylinder collider;
    /* 0x1D4 */ EnMm3ActionFunc actionFunc;
    /* 0x1D8 */ UNK_TYPE1 unk1D8[4];
    /* 0x1DC */ s32 unk_1DC;
    /* 0x1E0 */ Vec3s jointTable[OBJECT_MM_LIMB_MAX];
    /* 0x240 */ Vec3s morphTable[OBJECT_MM_LIMB_MAX];
    /* 0x2A0 */ Vec3s headRot;
    /* 0x2A6 */ Vec3s torsoRot;
    /* 0x2AC */ s16 unk_2AC;
    /* 0x2AE */ s16 unk_2AE;
    /* 0x2B0 */ u16 unk_2B0;
    /* 0x2B2 */ u16 unk_2B2;
    /* 0x2B4 */ u16 unk_2B4;
} EnMm3; // size = 0x2B8

#endif // Z_EN_MM3_H

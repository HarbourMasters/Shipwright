#ifndef Z_EN_BOMBF_H
#define Z_EN_BOMBF_H

#include "ultra64.h"
#include "global.h"

struct EnBombf;

typedef void (*EnBombfActionFunc)(struct EnBombf*, struct PlayState*);

typedef struct EnBombf {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder bombCollider;
    /* 0x0198 */ ColliderJntSph explosionCollider;
    /* 0x01B8 */ ColliderJntSphElement explosionColliderItems[1];
    /* 0x01F8 */ s16 timer;
    /* 0x01FC */ EnBombfActionFunc actionFunc;
    /* 0x0200 */ s32 unk_200;
    /* 0x0204 */ u8 bumpOn;
    /* 0x0206 */ s16 flashSpeedScale;
    /* 0x0208 */ f32 flashIntensity;
    /* 0x020C */ f32 flowerBombScale;
} EnBombf; // size = 0x0210

typedef enum {
    /* 0xFFFF */ BOMBFLOWER_FLOWER = -1,
    /* 0x0000 */ BOMBFLOWER_BODY,
    /* 0x0001 */ BOMBFLOWER_EXPLOSION
} EnBombfType;

#endif

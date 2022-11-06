#ifndef Z_EN_BOM_CHU_H
#define Z_EN_BOM_CHU_H

#include "ultra64.h"
#include "global.h"

struct EnBomChu;

typedef void (*EnBomChuActionFunc)(struct EnBomChu*, PlayState*);

typedef struct EnBomChu {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnBomChuActionFunc actionFunc;
    /* 0x0150 */ s16 timer;
    /* 0x0154 */ Vec3f axisForwards;
    /* 0x0160 */ Vec3f axisUp;
    /* 0x016C */ Vec3f axisLeft;
    /* 0x0178 */ f32 visualJitter;
    /* 0x017C */ s32 blure1Index;
    /* 0x0180 */ s32 blure2Index;
    /* 0x0184 */ ColliderJntSph collider;
    /* 0x01A4 */ ColliderJntSphElement colliderElements[1];
} EnBomChu; // size = 0x01E4

#endif

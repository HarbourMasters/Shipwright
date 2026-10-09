#ifndef Z_EN_BOM_CHU_H
#define Z_EN_BOM_CHU_H

#include "global.h"

struct EnBomChu;

typedef void (*EnBomChuActionFunc)(struct EnBomChu*, PlayState*);

typedef struct EnBomChu {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnBomChuActionFunc actionFunc;
    /* 0x148 */ u8 shouldTimerCountDown; // the actor always sets this to true
    /* 0x149 */ u8 isMoving;
    /* 0x14A */ s16 timer;
    /* 0x14C */ Vec3f axisForwards;
    /* 0x158 */ Vec3f axisUp;
    /* 0x164 */ Vec3f axisLeft;
    /* 0x170 */ f32 visualJitter;
    /* 0x174 */ f32 unk_174; // set but never used
    /* 0x178 */ UNK_TYPE1 unk_178[0x4];
    /* 0x17C */ f32 movingSpeed;
    /* 0x180 */ s32 blure1Index;
    /* 0x184 */ s32 blure2Index;
    /* 0x188 */ ColliderSphere collider;
} EnBomChu; // size = 0x1E0

#endif // Z_EN_BOM_CHU_H

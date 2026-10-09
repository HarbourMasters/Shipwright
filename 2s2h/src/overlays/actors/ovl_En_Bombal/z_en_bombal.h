#ifndef Z_EN_BOMBAL_H
#define Z_EN_BOMBAL_H

#include "global.h"

struct EnBombal;

typedef void (*EnBombalActionFunc)(struct EnBombal*, PlayState*);

typedef struct EnBombalEffect {
    /* 0x00 */ u8 isEnabled;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ UNK_TYPE1 unk10[0x4];
    /* 0x14 */ s16 alpha;
    /* 0x16 */ s16 alphaFadeDelay;
    /* 0x18 */ Vec3f velocity;
    /* 0x24 */ Vec3f accel;
    /* 0x30 */ f32 scale;
} EnBombalEffect;

typedef struct EnBombal {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnBombalActionFunc actionFunc;
    /* 0x148 */ s16 timer;
    /* 0x14A */ s16 isPopped;
    /* 0x14C */ s16 csId;
    /* 0x150 */ f32 scale;
    /* 0x154 */ f32 oscillationAngle;
    /* 0x158 */ ColliderCylinder collider;
    /* 0x1A4 */ EnBombalEffect effects[200];
} EnBombal; // size = 0x2A44

#endif // Z_EN_BOMBAL_H

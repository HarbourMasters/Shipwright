#ifndef Z_EN_BA_H
#define Z_EN_BA_H

#include "ultra64.h"
#include "global.h"

struct EnBa;

typedef void (*EnBaActionFunc)(struct EnBa*, PlayState*);

typedef enum {
    /* 0x00 */ EN_BA_TENTACLE_RED,
    /* 0x01 */ EN_BA_TENTACLE_GREEN,
    /* 0x02 */ EN_BA_TENTACLE_GRAY,
    /* 0x03 */ EN_BA_DEAD_BLOB
} EnBaType;

typedef struct EnBa {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 unk_14C;
    /* 0x0150 */ EnBaActionFunc actionFunc;
    /* 0x0154 */ s16 upperParams;
    /* 0x0156 */ s16 unk_156;
    /* 0x0158 */ Vec3f unk_158[14];
    /* 0x0200 */ Vec3f unk_200[14];
    /* 0x02A8 */ Vec3s unk_2A8[14];
    /* 0x02FC */ Vec3f unk_2FC;
    /* 0x0308 */ Vec3f unk_308;
    /* 0x0314 */ f32 unk_314;
    /* 0x0318 */ s16 unk_318;
    /* 0x031A */ s16 unk_31A;
    /* 0x031C */ s16 unk_31C;
    /* 0x0320 */ ColliderJntSph collider;
    /* 0x0340 */ ColliderJntSphElement colliderItems[2];
    /* 0x03C0 */ u32 epoch;
} EnBa; // size = 0x03C4

#endif

#ifndef Z_EN_BA_H
#define Z_EN_BA_H

#include "ultra64.h"
#include "global.h"

struct EnBa;

typedef void (*EnBaActionFunc)(struct EnBa*, GlobalContext*);

typedef enum {
    /* 0x00 */ EN_BA_TENTACLE_RED,
    /* 0x01 */ EN_BA_TENTACLE_GREEN,
    /* 0x02 */ EN_BA_TENTACLE_GRAY,
    /* 0x03 */ EN_BA_DEAD_BLOB
} EnBaType;

typedef struct EnBa {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 unk14C; 
    /* 0x0150 */ EnBaActionFunc actionFunc; 
    /* 0x0154 */ s16 upperParams; 
    /* 0x0156 */ s16 unk156;
    /* 0x0158 */ Vec3f unk158[14];
    /* 0x0200 */ Vec3f unk200[14];
    /* 0x02A8 */ Vec3s unk2A8[14];
    /* 0x02FC */ Vec3f unk2FC;
    /* 0x0308 */ Vec3f unk308;
    /* 0x0314 */ f32 unk314;
    /* 0x0318 */ s16 unk318;
    /* 0x031A */ s16 unk31A;
    /* 0x031C */ s16 unk31C;
    /* 0x0320 */ ColliderJntSph collider;
    /* 0x0340 */ ColliderJntSphElement colliderItems[2];
} EnBa; // size = 0x03C0

#endif

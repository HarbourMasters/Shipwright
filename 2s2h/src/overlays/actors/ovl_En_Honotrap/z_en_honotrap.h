#ifndef Z_EN_HONOTRAP_H
#define Z_EN_HONOTRAP_H

#include "global.h"

struct EnHonotrap;

typedef void (*EnHonotrapActionFunc)(struct EnHonotrap*, PlayState*);

typedef union {
    struct {
        /* 0x00 */ ColliderTris tris;
        /* 0x20 */ ColliderTrisElement elements[2];
    };
    /* 0x00 */ ColliderCylinder cyl;
} EnHonotrapCollider; // size = 0xD8

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32  unkC;
    /* 0x10 */ s16 flameScroll;
    /* 0x12 */ u8 isDrawn;
} EnHonotrapFlameElement; // size = 0x14

typedef struct {
    /* 0x0 */ f32 unk0;
    /* 0x4 */ f32 unk4;
    /* 0x8 */ f32 unk8;
    /* 0xC */ EnHonotrapFlameElement flameList[6];
} EnHonotrapFlameGroup; // size = 0x84;

typedef struct EnHonotrap {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnHonotrapActionFunc actionFunc;
    /* 0x148 */ EnHonotrapCollider collider;
    /* 0x220 */ s16 timer;
    /* 0x222 */ s16 eyeState;
    /* 0x224 */ s16 unk224;
    /* 0x228 */ Vec3f targetPos;
    /* 0x234 */ f32 speedMod;
    /* 0x238 */ s16 bobPhase;
    /* 0x23A */ s16 flameScroll;
    /* 0x23C */ EnHonotrapFlameGroup flameGroup;
    /* 0x2C0 */ u8 colChkFlags;
} EnHonotrap; // size = 0x2C4

typedef enum {
    /* 0 */ HONOTRAP_TYPE_EYE,
    /* 1 */ HONOTRAP_TYPE_FLAME_MOVE,
    /* 2 */ HONOTRAP_TYPE_FLAME_DROP,
    /* 3 */ HONOTRAP_TYPE_EYE_MUTI_FLAME,
    /* 4 */ HONOTRAP_TYPE_FLAME_GROUP
} EnHonotrapType;

#endif // Z_EN_HONOTRAP_H

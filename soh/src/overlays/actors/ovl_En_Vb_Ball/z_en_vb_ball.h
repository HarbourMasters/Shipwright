#ifndef Z_EN_VB_BALL_H
#define Z_EN_VB_BALL_H

#include "ultra64.h"
#include "global.h"

struct EnVbBall;

typedef struct EnVbBall {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[4];
    /* 0x0150 */ s16 unkTimer1; // These count up and down, but nothing uses them.
    /* 0x0152 */ s16 unkTimer2;
    /* 0x0154 */ char unk_154[4];
    /* 0x0158 */ f32 xRotVel;
    /* 0x015C */ f32 yRotVel;
    /* 0x0160 */ f32 shadowSize;
    /* 0x0164 */ f32 shadowOpacity;
    /* 0x0168 */ ColliderCylinder collider;
} EnVbBall; // size = 0x01B4

#endif

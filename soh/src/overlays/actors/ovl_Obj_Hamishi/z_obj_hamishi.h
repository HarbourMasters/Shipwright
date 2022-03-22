#ifndef Z_OBJ_HAMISHI_H
#define Z_OBJ_HAMISHI_H

#include "ultra64.h"
#include "global.h"

struct ObjHamishi;

typedef struct ObjHamishi {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ f32 shakePosSize;
    /* 0x019C */ f32 shakeRotSize;
    /* 0x01A0 */ s16 shakeFrames;
    /* 0x01A2 */ s16 shakePosPhase;
    /* 0x01A4 */ s16 shakeRotPhase;
    /* 0x01A6 */ s16 hitCount;
} ObjHamishi; // size = 0x01A8

#endif

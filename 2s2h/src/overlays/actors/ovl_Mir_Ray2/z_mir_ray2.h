#ifndef Z_MIR_RAY2_H
#define Z_MIR_RAY2_H

#include "global.h"

struct MirRay2;

#define MIRRAY2_GET_F(thisx) ((thisx)->params & 0xF)
#define MIRRAY2_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)

typedef struct MirRay2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider;
    /* 0x164 */ ColliderJntSphElement elements;
    /* 0x1A4 */ u16 unk1A4;
    /* 0x1A6 */ s16 radius;
    /* 0x1A8 */ f32 range;
    /* 0x1AC */ LightNode* light;
    /* 0x1B0 */ LightInfo lightInfo;
} MirRay2; // size = 0x1C0

#endif // Z_MIR_RAY2_H

#ifndef Z_MIR_RAY3_H
#define Z_MIR_RAY3_H

#include "global.h"

struct MirRay3;

#define MIRRAY3_GET_F(thisx) ((thisx)->params & 0xF)

#define MIRRAY3_F_1 1

typedef struct MirRay3 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderQuad colliderQuad;
    /* 0x1C4 */ ColliderCylinder colliderCylinder;
    /* 0x210 */ u16 unk_210;
    /* 0x214 */ f32 unk_214;
    /* 0x218 */ Vec3f unk_218[6];
    /* 0x260 */ f32 unk_260;
} MirRay3; // size = 0x264

#endif // Z_MIR_RAY3_H

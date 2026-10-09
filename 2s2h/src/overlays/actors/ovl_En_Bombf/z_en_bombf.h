#ifndef Z_EN_BOMBF_H
#define Z_EN_BOMBF_H

#include "global.h"

struct EnBombf;

typedef void (*EnBombfActionFunc)(struct EnBombf*, PlayState*);

#define ENBOMBF_GET(thisx) ((thisx)->params)

#define ENBOMBF_0 0
#define ENBOMBF_1 1

typedef struct EnBombf {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder colliderCylinder;
    /* 0x190 */ ColliderJntSph colliderJntSph;
    /* 0x1B0 */ ColliderJntSphElement colliderJntSphElements[1];
    /* 0x1F0 */ s16 timer;
    /* 0x1F4 */ EnBombfActionFunc actionFunc;
    /* 0x1F8 */ s32 unk_1F8;
    /* 0x1FC */ u8 unk_1FC;
    /* 0x1FE */ s16 unk_1FE;
    /* 0x200 */ f32 unk_200;
    /* 0x204 */ f32 unk_204;
} EnBombf; // size = 0x208

#endif // Z_EN_BOMBF_H

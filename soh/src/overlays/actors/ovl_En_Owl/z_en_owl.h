#ifndef Z_EN_OWL_H
#define Z_EN_OWL_H

#include "ultra64.h"
#include "global.h"

struct EnOwl;

typedef void (*EnOwlActionFunc)(struct EnOwl*, PlayState*);
typedef void (*OwlFunc)(struct EnOwl*);

typedef struct EnOwl {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ SkelAnime skelAnime;
    /* 0x01DC */ Vec3s jointTable[21];
    /* 0x025A */ Vec3s morphTable[21];
    /* 0x02D8 */ SkelAnime skelAnime2;
    /* 0x031C */ Vec3s jointTable2[16];
    /* 0x037C */ Vec3s morphTable2[16];
    /* 0x03DC */ SkelAnime* curSkelAnime;
    /* 0x03E0 */ Vec3f eye;
    /* 0x03EC */ s16 unk_3EC;
    /* 0x03EE */ s16 unk_3EE;
    /* 0x03F0 */ s16 unk_3F0;
    /* 0x03F2 */ s16 unk_3F2;
    /* 0x03F4 */ s16 eyeTexIndex;
    /* 0x03F6 */ s16 blinkTimer;
    /* 0x03F8 */ f32 unk_3F8;
    /* 0x03FC */ u16 actionFlags;
    /* 0x03FE */ u16 unk_3FE;
    /* 0x0400 */ s16 unk_400;
    /* 0x0402 */ s16 cameraIdx;
    /* 0x0404 */ u8 unk_404;
    /* 0x0405 */ u8 unk_405;
    /* 0x0406 */ u8 unk_406;
    /* 0x0407 */ u8 unk_407;
    /* 0x0408 */ u8 unk_408;
    /* 0x0409 */ u8 unk_409;
    /* 0x040A */ u8 unk_40A;
    /* 0x040B */ u8 unk_40B;
    /* 0x040C */ EnOwlActionFunc actionFunc;
    /* 0x0410 */ OwlFunc unk_410;
} EnOwl; // size = 0x0414

#endif

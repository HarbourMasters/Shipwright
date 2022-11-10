#ifndef Z_EN_MM2_H
#define Z_EN_MM2_H

#include "ultra64.h"
#include "global.h"

struct EnMm2;

typedef void (*EnMm2ActionFunc)(struct EnMm2*, PlayState*);

typedef struct EnMm2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnMm2ActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ s32 mouthTexIndex;
    /* 0x01E4 */ char unk_1E4[0x4];
    /* 0x01E8 */ Vec3s unk_1E8;
    /* 0x01EE */ Vec3s unk_1EE;
    /* 0x01F4 */ u16 unk_1F4;
    /* 0x01F6 */ s16 unk_1F6;
    /* 0x01F8 */ s32 previousAnimation;
    /* 0x01FC */ Vec3s jointTable[16];
    /* 0x025C */ Vec3s morphTable[16];
} EnMm2; // size = 0x02BC

#endif

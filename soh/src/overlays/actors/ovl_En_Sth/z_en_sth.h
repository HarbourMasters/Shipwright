#ifndef Z_EN_STH_H
#define Z_EN_STH_H

#include "ultra64.h"
#include "global.h"

struct EnSth;

typedef void (*EnSthActionFunc)(struct EnSth*, PlayState*);

typedef struct EnSth {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ SkelAnime skelAnime;
    /* 0x01DC */ Vec3s jointTable[16];
    /* 0x023C */ Vec3s morphTable[16];
    /* 0x029C */ u16 eventFlag;
    /* 0x02A0 */ ActorFunc drawFunc;
    /* 0x02A4 */ u8 objectBankIdx;
    /* 0x02A6 */ Vec3s headRot;
    /* 0x02AC */ Vec3s unk_2AC;
    /* 0x02B2 */ u16 unk_2B2;
    /* 0x02B4 */ s16 unk_2B4;
    /* 0x02B6 */ s16 unk_2B6;
    /* 0x02B8 */ EnSthActionFunc actionFunc;
} EnSth; // size = 0x02BC

#endif

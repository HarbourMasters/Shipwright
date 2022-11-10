#ifndef Z_EN_DU_H
#define Z_EN_DU_H

#include "ultra64.h"
#include "global.h"

struct EnDu;

typedef void (*EnDuActionFunc)(struct EnDu*, PlayState*);

typedef struct EnDu {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnDuActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ char unk_1E0[0x2];
    /* 0x01E2 */ s16 unk_1E2;
    /* 0x01E4 */ char unk_1E4[0x2];
    /* 0x01E6 */ s16 unk_1E6;
    /* 0x01E8 */ s16 unk_1E8;
    /* 0x01EA */ s16 unk_1EA;
    /* 0x01EC */ u8 unk_1EC;
    /* 0x01ED */ u8 unk_1ED;
    /* 0x01EE */ u8 unk_1EE;
    /* 0x01EF */ u8 eyeTexIndex;
    /* 0x01F0 */ u8 mouthTexIndex;
    /* 0x01F1 */ u8 noseTexIndex;
    /* 0x01F2 */ s16 blinkTimer;
    /* 0x01F4 */ struct_80034A14_arg1 unk_1F4;
} EnDu; // size = 0x021C

#endif

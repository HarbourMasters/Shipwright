#ifndef Z_EN_MA1_H
#define Z_EN_MA1_H

#include "ultra64.h"
#include "global.h"

struct EnMa1;

typedef void (*EnMa1ActionFunc)(struct EnMa1*, PlayState*);

typedef struct EnMa1 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnMa1ActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ s16 unk_1E0;
    /* 0x01E2 */ s16 blinkTimer;
    /* 0x01E4 */ s16 eyeIndex;
    /* 0x01E6 */ s16 mouthIndex;
    /* 0x01E8 */ struct_80034A14_arg1 unk_1E8;
} EnMa1; // size = 0x0210

#endif

#ifndef Z_EN_ZL1_H
#define Z_EN_ZL1_H

#include "ultra64.h"
#include "global.h"

struct EnZl1;

typedef void (*EnZl1ActionFunc)(struct EnZl1*, PlayState*);

typedef struct EnZl1 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnZl1ActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ char unk_1E0[0x2];
    /* 0x01E2 */ s16 unk_1E2;
    /* 0x01E4 */ s16 unk_1E4;
    /* 0x01E6 */ s16 unk_1E6;
    /* 0x01E8 */ s16 unk_1E8;
    /* 0x01EA */ char unk_1EA[0x2];
    /* 0x01EC */ void* unk_1EC;
    /* 0x01F0 */ char unk_1F0[0x2];
    /* 0x01F2 */ s16 unk_1F2;
    /* 0x01F4 */ void* unk_1F4;
    /* 0x01F8 */ void* unk_1F8;
    /* 0x01FC */ s16 unk_1FC;
    /* 0x01FE */ s16 unk_1FE;
    /* 0x0200 */ Vec3s unk_200;
    /* 0x0206 */ Vec3s unk_206;
} EnZl1; // size = 0x020C

#endif

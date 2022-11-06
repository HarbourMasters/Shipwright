#ifndef Z_EN_KAKASI3_H
#define Z_EN_KAKASI3_H

#include "ultra64.h"
#include "global.h"

struct EnKakasi3;

typedef void (*EnKakasi3ActionFunc)(struct EnKakasi3*, PlayState*);

typedef struct EnKakasi3 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnKakasi3ActionFunc actionFunc;
    /* 0x0150 */ SkelAnime skelAnime;
    /* 0x0194 */ u8 unk_194;
    /* 0x0195 */ u8 unk_195;
    /* 0x0196 */ s16 dialogState;
    /* 0x0198 */ s16 unk_198;
    /* 0x019A */ s16 unk_19A;
    /* 0x019C */ s16 unk_19C[4];
    /* 0x01A4 */ s16 unk_1A4;
    /* 0x01A6 */ s16 unk_1A6;
    /* 0x01A8 */ s16 unk_1A8;
    /* 0x01AA */ s16 unk_1AA;
    /* 0x01AC */ char unk_1AC[2];
    /* 0x01AE */ s16 unk_1AE;
    /* 0x01B0 */ Vec3s rot;
    /* 0x01B8 */ f32 unk_1B8;
    /* 0x01BC */ ColliderCylinder collider;
    /* 0x0208 */ s16 camId;
} EnKakasi3; // size = 0x020C

#endif

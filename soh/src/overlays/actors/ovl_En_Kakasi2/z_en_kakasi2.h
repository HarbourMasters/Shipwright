#ifndef Z_EN_KAKASI2_H
#define Z_EN_KAKASI2_H

#include "ultra64.h"
#include "global.h"

struct EnKakasi2;

typedef void (*EnKakasi2ActionFunc)(struct EnKakasi2*, PlayState*);

typedef struct EnKakasi2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnKakasi2ActionFunc actionFunc;
    /* 0x0150 */ SkelAnime skelAnime;
    /* 0x0194 */ s16 unk_194;
    /* 0x0196 */ s16 switchFlag;
    /* 0x0198 */ s16 unk_198;
    /* 0x019C */ Vec3f maxSpawnDistance;
    /* 0x01A8 */ f32 height;
    /* 0x01AC */ ColliderCylinder collider;
} EnKakasi2; // size = 0x01F8

#endif

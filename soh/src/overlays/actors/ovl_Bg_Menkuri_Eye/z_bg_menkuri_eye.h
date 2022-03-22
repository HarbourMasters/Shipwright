#ifndef Z_BG_MENKURI_EYE_H
#define Z_BG_MENKURI_EYE_H

#include "ultra64.h"
#include "global.h"

struct BgMenkuriEye;

typedef struct BgMenkuriEye {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 framesUntilDisable;
    /* 0x0150 */ ColliderJntSph collider;
    /* 0x0170 */ ColliderJntSphElement colliderItems[1];
} BgMenkuriEye; // size = 0x01B0

#endif

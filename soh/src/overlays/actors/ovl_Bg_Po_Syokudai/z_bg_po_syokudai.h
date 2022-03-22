#ifndef Z_BG_PO_SYOKUDAI_H
#define Z_BG_PO_SYOKUDAI_H

#include "ultra64.h"
#include "global.h"

struct BgPoSyokudai;

typedef struct BgPoSyokudai {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ u8 flameColor;
    /* 0x014E */ s16 flameTextureScroll;
    /* 0x0150 */ LightNode* lightNode;
    /* 0x0154 */ LightInfo lightInfo;
    /* 0x0164 */ ColliderCylinder collider;
} BgPoSyokudai; // size = 0x01B0

#endif

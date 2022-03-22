#ifndef Z_EN_LIGHT_H
#define Z_EN_LIGHT_H

#include "ultra64.h"
#include "global.h"

struct EnLight;

typedef struct EnLight {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ u8 timer;
    /* 0x0150 */ LightNode* lightNode;
    /* 0x0154 */ LightInfo lightInfo;
} EnLight; // size = 0x0164

#endif

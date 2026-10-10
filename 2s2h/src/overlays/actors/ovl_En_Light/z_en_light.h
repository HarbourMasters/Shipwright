#ifndef Z_EN_LIGHT_H
#define Z_EN_LIGHT_H

#include "global.h"

struct EnLight;

#define ENLIGHT_GET_1(thisx) ((thisx)->params & 1)
#define ENLIGHT_GET_F(thisx) ((thisx)->params & 0xF)
#define ENLIGHT_GET_800(thisx) ((thisx)->params & 0x800)
#define ENLIGHT_SWITCH_FLAG(thisx) (((thisx)->params & 0x7F0) >> 4)
#define ENLIGHT_GET_1000(thisx) ((thisx)->params & 0x1000)
#define ENLIGHT_GET_2000(thisx) ((thisx)->params & 0x2000)
#define ENLIGHT_GET_4000(thisx) ((thisx)->params & 0x4000)

typedef struct EnLight {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u8 unk_144;
    /* 0x148 */ LightNode* lightNode;
    /* 0x14C */ LightInfo lightInfo;
} EnLight; // size = 0x15C

#endif // Z_EN_LIGHT_H

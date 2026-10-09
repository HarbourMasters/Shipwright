#ifndef Z_EN_ELFBUB_H
#define Z_EN_ELFBUB_H

#include "global.h"

#define ENELFBUB_GET_SWITCH_FLAG(thisx) ((((thisx)->params) & 0xFE00) >> 9)

struct EnElfbub;

typedef void (*EnElfbubActionFunc)(struct EnElfbub*, PlayState*);

typedef struct EnElfbub {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ s16 zRot;
    /* 0x192 */ s16 zRotDelta;
    /* 0x194 */ s16 oscillationAngle;
    /* 0x196 */ s16 popTimer;
    /* 0x198 */ f32 xScale;
    /* 0x19C */ f32 xyScale;
    /* 0x1A0 */ EnElfbubActionFunc actionFunc;
} EnElfbub; // size = 0x1A4

#endif // Z_EN_ELFBUB_H

#ifndef Z_EN_OKUTA_H
#define Z_EN_OKUTA_H

#include "ultra64.h"
#include "global.h"

struct EnOkuta;

typedef void (*EnOkutaActionFunc)(struct EnOkuta*, PlayState*);

typedef struct EnOkuta {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnOkutaActionFunc actionFunc;
    /* 0x0194 */ s16 timer;
    /* 0x0196 */ s16 numShots;
    /* 0x0198 */ Vec3s jointTable[38];
    /* 0x027C */ Vec3s morphTable[38];
    /* 0x0360 */ f32 jumpHeight;
    /* 0x0364 */ Vec3f headScale;
    /* 0x0370 */ ColliderCylinder collider;
} EnOkuta; // size = 0x03BC

#endif

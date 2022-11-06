#ifndef Z_EN_KAREBABA_H
#define Z_EN_KAREBABA_H

#include "ultra64.h"
#include "global.h"

struct EnKarebaba;

typedef void (*EnKarebabaActionFunc)(struct EnKarebaba*, PlayState*);

typedef struct EnKarebaba {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnKarebabaActionFunc actionFunc;
    /* 0x0194 */ Vec3s jointTable[8];
    /* 0x01C4 */ Vec3s morphTable[8];
    /* 0x01F4 */ CollisionPoly* boundFloor;
    /* 0x01F8 */ ColliderCylinder headCollider;
    /* 0x0244 */ ColliderCylinder bodyCollider;
} EnKarebaba; // size = 0x0290

#endif

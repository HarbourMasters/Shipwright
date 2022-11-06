#ifndef Z_EN_DOG_H
#define Z_EN_DOG_H

#include "ultra64.h"
#include "global.h"

struct EnDog;

typedef void (*EnDogActionFunc)(struct EnDog*, PlayState*);

typedef struct EnDog {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnDogActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ Path* path;
    /* 0x01E4 */ u8 reverse;
    /* 0x01E6 */ s16 waypoint;
    /* 0x01E8 */ s16 unusedAngle;
    /* 0x01EA */ s16 pad1;
    /* 0x01EC */ s16 behaviorTimer;
    /* 0x01EE */ s16 pad2;
    /* 0x01F0 */ s16 nextBehavior; 
    /* 0x01F2 */ s16 behavior;
    /* 0x01F4 */ Vec3s jointTable[13];
    /* 0x0242 */ Vec3s morphTable[13];
} EnDog; // size = 0x0290

#endif

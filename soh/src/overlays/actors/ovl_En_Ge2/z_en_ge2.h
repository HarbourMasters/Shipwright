#ifndef Z_EN_GE2_H
#define Z_EN_GE2_H

#include "ultra64.h"
#include "global.h"

struct EnGe2;

typedef void (*EnGe2ActionFunc)(struct EnGe2*, PlayState*);

typedef struct EnGe2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ SkelAnime skelAnime;
    /* 0x01DC */ Vec3s jointTable[22];
    /* 0x0260 */ Vec3s morphTable[22];
    /* 0x02E4 */ s16 eyeIndex;
    /* 0x02E6 */ s16 blinkTimer;
    /* 0x02E8 */ Vec3s headRot;
    /* 0x02EE */ Vec3s unk_2EE;
    /* 0x02F4 */ u16 stateFlags;
    /* 0x02F6 */ s16 walkDirection;
    /* 0x02F8 */ s16 yawTowardsPlayer;
    /* 0x02FC */ f32 yDetectRange;
    /* 0x0300 */ u16 walkDuration;
    /* 0x0302 */ u16 walkTimer;
    /* 0x0304 */ u8 unk_304;
    /* 0x0305 */ u8 timer;
    /* 0x0306 */ u8 playerSpottedParam;
    /* 0x0308 */ EnGe2ActionFunc actionFunc;
} EnGe2; // size = 0x030C

#endif

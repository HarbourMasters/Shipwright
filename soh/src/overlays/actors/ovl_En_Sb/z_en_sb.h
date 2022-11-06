#ifndef Z_EN_SB_H
#define Z_EN_SB_H

#include "ultra64.h"
#include "global.h"

struct EnSb;

typedef void (*EnSbActionFunc)(struct EnSb*, PlayState*);

typedef struct EnSb {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnSbActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ BodyBreak bodyBreak;
    /* 0x01F8 */ s16 fire;
    /* 0x01FA */ s16 behavior;
    /* 0x01FC */ s16 isDead;
    /* 0x01FE */ s16 timer;
    /* 0x0200 */ s16 attackYaw;
    /* 0x0202 */ s16 bouncesLeft; // amount of bounces left in the attack before going back to wait
    /* 0x0204 */ u8 hitByWindArrow;
} EnSb; // size = 0x0208

#endif

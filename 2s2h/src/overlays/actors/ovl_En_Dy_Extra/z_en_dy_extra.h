#ifndef Z_EN_DY_EXTRA_H
#define Z_EN_DY_EXTRA_H

#include "global.h"

struct EnDyExtra;

typedef void (*EnDyExtraActionFunc)(struct EnDyExtra*, PlayState*);

typedef struct EnDyExtra {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnDyExtraActionFunc actionFunc;
    /* 0x148 */ s16 type;
    /* 0x14A */ s16 trigger;
    /* 0x14C */ s16 timer;
    /* 0x150 */ f32 alphaScale;
    /* 0x154 */ Vec3f scale; // not used, leftover from OoT
    /* 0x160 */ Vec3f initPos; // set and not used
} EnDyExtra; // size = 0x16C

#endif // Z_EN_DY_EXTRA_H

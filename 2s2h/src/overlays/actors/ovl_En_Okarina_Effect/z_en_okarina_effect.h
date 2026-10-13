#ifndef Z_EN_OKARINA_EFFECT_H
#define Z_EN_OKARINA_EFFECT_H

#include "global.h"

struct EnOkarinaEffect;

typedef void (*EnOkarinaEffectActionFunc)(struct EnOkarinaEffect*, PlayState*);

typedef struct EnOkarinaEffect {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u16 timer;
    /* 0x148 */ EnOkarinaEffectActionFunc actionFunc;
} EnOkarinaEffect; // size = 0x14C

#endif // Z_EN_OKARINA_EFFECT_H

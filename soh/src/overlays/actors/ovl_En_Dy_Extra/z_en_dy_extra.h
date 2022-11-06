#ifndef Z_EN_DY_EXTRA_H
#define Z_EN_DY_EXTRA_H

#include "ultra64.h"
#include "global.h"

struct EnDyExtra;

typedef void (*EnDyExtraActionFunc)(struct EnDyExtra*, PlayState*);

typedef struct EnDyExtra {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnDyExtraActionFunc actionFunc;
    /* 0x0150 */ s16 type;
    /* 0x0152 */ s16 trigger;
    /* 0x0154 */ s16 timer;
    /* 0x0158 */ f32 unk_158;
    /* 0x015C */ Vec3f scale;
    /* 0x0168 */ Vec3f unk_168; // Set and not used
} EnDyExtra; // size = 0x0174

#endif

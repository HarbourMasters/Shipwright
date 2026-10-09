#ifndef _Z64_EFFECT_SS_SOLDER_SRCH_BALL_H_
#define _Z64_EFFECT_SS_SOLDER_SRCH_BALL_H_

#include "global.h"

#define SOLDERSRCHBALL_INVISIBLE (1 << 0)
#define SOLDERSRCHBALL_SMALL_DETECT_RADIUS (1 << 1)

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x28 */ s16* playerDetected;
    /* 0x2C */ s16 flags;
} EffectSsSolderSrchBallInitParams; // size = 0x30

#endif

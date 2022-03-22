#ifndef Z_MAGIC_FIRE_H
#define Z_MAGIC_FIRE_H

#include "ultra64.h"
#include "global.h"

struct MagicFire;

typedef struct MagicFire {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ f32 alphaMultiplier;
    /* 0x019C */ f32 screenTintIntensity;
    /* 0x01A0 */ f32 scalingSpeed;
    /* 0x01A4 */ s16 action;
    /* 0x01A6 */ s16 screenTintBehaviour;
    /* 0x01A8 */ s16 actionTimer;
    /* 0x01AA */ s16 screenTintBehaviourTimer;
} MagicFire; // size = 0x01AC

#endif

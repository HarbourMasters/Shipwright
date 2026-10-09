#ifndef Z_EFF_DUST_H
#define Z_EFF_DUST_H

#include "global.h"

struct EffDust;

typedef void (*EffDustActionFunc)(struct EffDust*, PlayState*);

typedef struct EffDust {
    /* 0x000 */ Actor actor;
    /* 0x144 */ f32 distanceTraveled[64]; // For each particle. Normalized. From 0.0f to 1.0f
    /* 0x244 */ Vec3f initialPositions[64]; // Array of position for each dust particle.
    /* 0x544 */ u8 index;
    /* 0x545 */ u8 life; // Only considered if actor.params is 2, 3 or 4.
    /* 0x548 */ f32 dx; // Normalized. 0.0f to 1.0f
    /* 0x54C */ f32 dy; // Normalized. 0.0f to 1.0f
    /* 0x550 */ f32 dz; // Normalized. 0.0f to 1.0f
    /* 0x554 */ f32 scalingFactor;
    /* 0x558 */ EffDustActionFunc actionFunc;
    /* 0x55C */ ActorFunc drawFunc;
} EffDust; // size = 0x560

typedef enum {
    /* 0 */ EFF_DUST_TYPE_0,
    /* 1 */ EFF_DUST_TYPE_1,
    /* 2 */ EFF_DUST_TYPE_SPIN_ATTACK_CHARGE,
    /* 3 */ EFF_DUST_TYPE_3,
    /* 4 */ EFF_DUST_TYPE_4,
    /* 5 */ EFF_DUST_TYPE_5
} EffDustType;

#endif // Z_EFF_DUST_H

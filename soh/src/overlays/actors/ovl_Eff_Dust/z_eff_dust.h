#ifndef Z_EFF_DUST_H
#define Z_EFF_DUST_H

#include "ultra64.h"
#include "global.h"

struct EffDust;

typedef void (*EffDustActionFunc)(struct EffDust*, PlayState*);
typedef void (*EffDustDrawFunc)(Actor*, PlayState*);

typedef struct EffDust {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ f32 distanceTraveled[64]; // For each particle. Normalized. From 0.0f to 1.0f
    /* 0x024C */ Vec3f initialPositions[64]; // Array of position for each dust particle.
    /* 0x054C */ u8 index;
    /* 0x054D */ u8 life; // Only considered if actor.params is 2, 3 or 4.
    /* 0x0550 */ f32 dx; // Normalized. 0.0f to 1.0f
    /* 0x0554 */ f32 dy; // Normalized. 0.0f to 1.0f
    /* 0x0558 */ f32 dz; // Normalized. 0.0f to 1.0f
    /* 0x055C */ f32 scalingFactor;
    /* 0x0560 */ EffDustActionFunc actionFunc;
    /* 0x0564 */ EffDustDrawFunc drawFunc;
} EffDust; // size = 0x0568

typedef enum {
    /* 0x00 */ EFF_DUST_TYPE_0,
    /* 0x01 */ EFF_DUST_TYPE_1,
    /* 0x02 */ EFF_DUST_TYPE_2,
    /* 0x03 */ EFF_DUST_TYPE_3,
    /* 0x04 */ EFF_DUST_TYPE_4
} EffDustType;

#endif

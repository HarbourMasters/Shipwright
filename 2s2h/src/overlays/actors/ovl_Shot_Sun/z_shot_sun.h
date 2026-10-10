#ifndef Z_SHOT_SUN_H
#define Z_SHOT_SUN_H

#include "global.h"

#define SHOTSUN_GET_TYPE(thisx) ((thisx)->params & 0xFF)
#define SHOTSUN_FAIRY_SPAWNER_SUNS 0x40
#define SHOTSUN_FAIRY_SPAWNER_STORMS 0x41

struct ShotSun;

typedef void (*ShotSunActionFunc)(struct ShotSun*, PlayState*);

typedef struct ShotSun {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ ShotSunActionFunc actionFunc;
    /* 0x194 */ Vec3s hitboxPos;
    /* 0x19A */ s16 timer;
    /* 0x19C */ s8 fairySpawnerState; // leftover from OOT secret ocarina occurrence function.
} ShotSun; // size = 0x1A0

// Left over from OOT, unused in MM
typedef enum {
    /* 0 */ SPAWNER_OUT_OF_RANGE,
    /* 1 */ SPAWNER_OCARINA_START,
    /* 2 */ SPAWNER_OCARINA_PLAYING
} FairySpawnerState;

#endif // Z_SHOT_SUN_H

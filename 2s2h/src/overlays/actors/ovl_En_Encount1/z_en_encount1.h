#ifndef Z_EN_ENCOUNT1_H
#define Z_EN_ENCOUNT1_H

#include "global.h"

#define ENENCOUNT1_SPAWNS_TOTAL_MAX_INFINITE 63 // aka 0x3F, All bits set

#define ENENCOUNT1_GET_TYPE(thisx) (((thisx)->params >> 11) & 0x1F)
#define ENENCOUNT1_GET_SPAWN_ACTIVE_MAX(thisx) (((thisx)->params >> 6) & 0x1F)
#define ENENCOUNT1_GET_SPAWN_TOTAL_MAX(thisx) ((thisx)->params & 0x3F)
#define ENENCOUNT1_GET_PATH_INDEX(thisx) ((thisx)->params & 0x3F) // Used only by EN_ENCOUNT1_SKULLFISH_2 which doesn't use SpawnTotalMax

#define ENENCOUNT1_PATH_INDEX_NONE 0x3F

#define ENENCOUNT1_GET_SPAWN_TIME_MIN(thisx) ((thisx)->world.rot.x) // Time to wait between spawning
#define ENENCOUNT1_GET_SPAWN_UNUSED_PROP(thisx) ((thisx)->world.rot.y) // Unused spawn property
#define ENENCOUNT1_GET_SPAWN_DISTANCE_MAX(thisx) ((thisx)->world.rot.z) // Negative means infinite distance

typedef enum EnEncount1Enemy {
    /* 0 */ EN_ENCOUNT1_GRASSHOPPER,
    /* 1 */ EN_ENCOUNT1_WALLMASTER,
    /* 2 */ EN_ENCOUNT1_SKULLFISH,
    /* 3 */ EN_ENCOUNT1_SKULLFISH_2
} EnEncount1Enemy;

struct EnEncount1;

typedef void (*EnEncount1ActionFunc)(struct EnEncount1*, PlayState*);

typedef struct EnEncount1 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnEncount1ActionFunc actionFunc;
    /* 0x148 */ Path* path;
    /* 0x14C */ s16 spawnActiveMax;
    /* 0x14E */ s16 spawnActiveCount;
    /* 0x150 */ s16 type;
    /* 0x152 */ s16 spawnTotalCount;
    /* 0x154 */ s16 spawnTotalMax;
    /* 0x156 */ s16 timer;
    /* 0x158 */ s16 spawnTimeMin;
    /* 0x15A */ s16 pathIndex;
    /* 0x15C */ s32 spawnUnusedProp;
    /* 0x160 */ f32 spawnDistanceMax;
} EnEncount1; // size = 0x164

#endif // Z_EN_ENCOUNT1_H

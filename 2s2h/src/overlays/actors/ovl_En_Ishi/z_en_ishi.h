#ifndef Z_EN_ISHI_H
#define Z_EN_ISHI_H

#include "global.h"

struct EnIshi;

typedef void (*EnIshiActionFunc)(struct EnIshi*, PlayState*);
typedef void (*EnIshiDrawFunc)(struct EnIshi*, PlayState*);
typedef void (*EnIshiSpawnFunc)(Actor*, PlayState*);

// silver boulder instead of small rock
#define ENISHI_GET_SIZE_FLAG(thisx) ((thisx)->params & 1)
#define ENISHI_GET_SPAWN_BUGS_FLAG(thisx) (((thisx)->params >> 1) & 1)
// for sitting still on rock/dirt cliffs in TF/Ikana Graveyard
#define ENISHI_GET_LEVITATE_FLAG(thisx) (((thisx)->params >> 2) & 1)
// dual objects: this actor can use field_keep or object_ishi
// the only place its used in the US version is the swamp spiderhouse, which uses dangeon_keep
#define ENISHI_GET_USE_OBJECT(thisx) (((thisx)->params >> 3) & 1)
// index of a collectable item list in .data
#define ENISHI_GET_COLLECTABLE_ID(thisx) (((thisx)->params >> 4) & 7)
#define ENISHI_GET_DROP_TABLE(thisx) (((thisx)->params >> 4) & 0xF)
#define ENISHI_GET_IGNORE_DROP_TABLE_FLAG(thisx) (((thisx)->params >> 8) & 1)
// these two are the same, but are used by different subtypes
#define ENISHI_GET_COLLECTABLE_FLAG(thisx) (((thisx)->params >> 9) & 0x7F)
#define ENISHI_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 9) & 0x7F)

#define ISHI_FLAG_UNDERWATER    (1 << 0)
#define ISHI_FLAG_CUTSCENE_ROCK (1 << 1)

typedef enum IshiSize {
  /* 0 */ ISHI_SIZE_SMALL_ROCK,
  /* 1 */ ISHI_SIZE_SILVER_BOULDER
} IshiSize;

// if yRot is zero, y rotation gets randomized
#define ISHI_RANDOM_ROTATION_FLAG 0

typedef struct EnIshi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ EnIshiActionFunc actionFunc;
    /* 0x194 */ s8 thrownTimer;
    /* 0x195 */ s8 hitTimer;
    /* 0x196 */ s8 objectSlot;
    /* 0x197 */ u8 flags;
} EnIshi; // size = 0x198

#endif // Z_EN_ISHI_H

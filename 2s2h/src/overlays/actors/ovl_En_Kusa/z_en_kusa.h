#ifndef Z_EN_KUSA_H
#define Z_EN_KUSA_H

#include "global.h"

struct EnKusa;

typedef void (*EnKusaActionFunc)(struct EnKusa*, PlayState*);

#define KUSA_GET_TYPE(thisx)((thisx)->params & 0x3)

// `ENKUSA_TYPE_BUSH` and `ENKUSA_TYPE_GRASS`
#define KUSA_GET_RAND_COLLECTIBLE_ID(thisx) (((thisx)->params >> 8) & 0xF)
#define KUSA_GET_PARAM_0C(thisx) (((thisx)->params >> 12) & 1)

// `ENKUSA_TYPE_REGROWING_GRASS`, `ENKUSA_TYPE_BUSH` and `ENKUSA_TYPE_GRASS`
#define KUSA_SHOULD_SPAWN_BUGS(thisx) (((thisx)->params >> 4) & 1)

// `ENKUSA_TYPE_GRASS_2`
#define KUSA_GET_COLLECTIBLE_ID(thisx) (((thisx)->params >> 8) & 0x7F)
#define KUSA_GET_PARAM_FC(thisx) (((thisx)->params >> 2) & 0x3F)

typedef enum {
    /* 0 */ ENKUSA_TYPE_BUSH,
    /* 1 */ ENKUSA_TYPE_REGROWING_GRASS,
    /* 2 */ ENKUSA_TYPE_GRASS,
    /* 3 */ ENKUSA_TYPE_GRASS_2
} EnKusaType;

#define KUSA_BUSH_PARAMS(spawnBugs, randCollectibleId, param0C) \
    ((ENKUSA_TYPE_BUSH & 0x3) | (((spawnBugs) & 1) << 4) | (((randCollectibleId) & 0xF) << 8) | (((param0C) & 1) << 12))

#define KUSA_REGROWING_GRASS_PARAMS(spawnBugs) \
    ((ENKUSA_TYPE_REGROWING_GRASS & 0x3) | (((spawnBugs) & 1) << 4))

#define KUSA_GRASS_PARAMS(spawnBugs, randCollectibleId, param0C) \
    ((ENKUSA_TYPE_GRASS & 0x3) | (((spawnBugs) & 1) << 4) | (((randCollectibleId) & 0xF) << 8) | (((param0C) & 1) << 12))

#define KUSA_GRASS_2_PARAMS(paramFC, collectibleId) \
    ((ENKUSA_TYPE_GRASS_2 & 0x3) | (((paramFC) & 0x3F) << 2) | (((collectibleId) & 0x7F) << 8))


typedef struct EnKusa {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnKusaActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ u8 timer;
    /* 0x195 */ s8 objectSlot;
    /* 0x196 */ s8 kusaMtxIdx;
    /* 0x197 */ u8 isCut;
    /* 0x198 */ u8 isInWater;
} EnKusa; // size = 0x19C

#endif // Z_EN_KUSA_H

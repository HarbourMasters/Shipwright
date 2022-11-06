#ifndef Z_EN_ENCOUNT1_H
#define Z_EN_ENCOUNT1_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_En_Reeba/z_en_reeba.h"

#define SPAWNER_PARAMS(type, number, total) ((type << 0xB) | (number << 0x6) | total)

struct EnEncount1;

typedef void (*EnEncount1UpdateFunc)(struct EnEncount1*, PlayState*);

typedef struct EnEncount1 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnEncount1UpdateFunc updateFunc;
    /* 0x0150 */ s16 maxCurSpawns;
    /* 0x0152 */ s16 curNumSpawn;
    /* 0x0154 */ s16 spawnType;
    /* 0x0156 */ s16 maxTotalSpawns;
    /* 0x0158 */ s16 totalNumSpawn;
    /* 0x015A */ s16 outOfRangeTimer;
    /* 0x015C */ s16 fieldSpawnTimer;
    /* 0x015E */ s16 killCount;
    /* 0x0160 */ s16 numLeeverSpawns;
    /* 0x0162 */ s16 leeverIndex;
    /* 0x0164 */ s16 timer;
    /* 0x0166 */ u8 reduceLeevers;
    /* 0x0168 */ f32 spawnRange;
    /* 0x016C */ EnReeba* bigLeever;
} EnEncount1; // size = 0x0170

typedef enum {
    /* 0 */ SPAWNER_LEEVER,
    /* 1 */ SPAWNER_TEKTITE,
    /* 2 */ SPAWNER_STALCHILDREN,
    /* 3 */ SPAWNER_WOLFOS
} EnEncount1type;

#endif

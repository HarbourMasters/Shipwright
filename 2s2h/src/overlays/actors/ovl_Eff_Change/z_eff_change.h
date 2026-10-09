#ifndef Z_EFF_CHANGE_H
#define Z_EFF_CHANGE_H

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define EFFCHANGE_GET_COLORS(thisx) ((thisx)->params & 7)

struct EffChange;

typedef void (*EffChangeActionFunc)(struct EffChange*, PlayState*);

typedef struct EffChange {
    /* 0x000 */ Actor actor;
    /* 0x144 */ KFSkelAnimeFlex kfSkelAnime;
    /* 0x174 */ Vec3s jointTable[PINK_DEKU_FLOWER_LIMB_BACK_RIGHT_PETAL];
    /* 0x198 */ Vec3s morphTable[PINK_DEKU_FLOWER_LIMB_BACK_RIGHT_PETAL];
    /* 0x1BC */ s16 step;
    /* 0x1BE */ u8 primColors[4];
    /* 0x1C2 */ u8 envColors[4];
    /* 0x1C8 */ EffChangeActionFunc actionFunc;
} EffChange; // size = 0x1CC

#endif // Z_EFF_CHANGE_H

#ifndef Z_BG_BOWL_WALL_H
#define Z_BG_BOWL_WALL_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"

struct BgBowlWall;

typedef void (*BgBowlWallActionFunc)(struct BgBowlWall*, PlayState*);

typedef struct BgBowlWall {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgBowlWallActionFunc actionFunc;
    /* 0x0168 */ Vec3f initPos;
    /* 0x0174 */ Vec3f bullseyeCenter;
    /* 0x0180 */ s16 isHit;
    /* 0x0182 */ s16 timer;
    /* 0x0184 */ EnBomBowlMan* chuGirl;
} BgBowlWall; // size = 0x0188

#endif

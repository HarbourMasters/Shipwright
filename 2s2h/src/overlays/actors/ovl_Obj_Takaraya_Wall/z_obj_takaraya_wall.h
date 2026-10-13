#ifndef Z_OBJ_TAKARAYA_WALL_H
#define Z_OBJ_TAKARAYA_WALL_H

#include "global.h"

struct ObjTakarayaWall;

#define TAKARAYA_WALL_GET_SWTICH_FLAG(thisx) ((thisx)->shape.rot.z)

typedef void (*ObjTakarayaWallActionFunc)(struct ObjTakarayaWall*, PlayState*);

typedef struct ObjTakarayaWall {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjTakarayaWallActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder columnCollider;
    /* 0x194 */ ColliderCylinder rowCollider;
} ObjTakarayaWall; // size = 0x1E0

#endif // Z_OBJ_TAKARAYA_WALL_H

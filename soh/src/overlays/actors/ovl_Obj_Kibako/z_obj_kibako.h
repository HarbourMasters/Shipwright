#ifndef Z_OBJ_KIBAKO_H
#define Z_OBJ_KIBAKO_H

#include "ultra64.h"
#include "global.h"

struct ObjKibako;

typedef void (*ObjKibakoActionFunc)(struct ObjKibako*, PlayState*);

typedef struct ObjKibako {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ObjKibakoActionFunc actionFunc;
    /* 0x0150 */ ColliderCylinder collider;
} ObjKibako; // size = 0x019C

#endif

#ifndef Z_OBJ_TREE_H
#define Z_OBJ_TREE_H

#include "global.h"

struct ObjTree;

typedef void (*ObjTreeActionFunc)(struct ObjTree*, PlayState*);

#define OBJTREE_ISLARGE(thisx) ((thisx)->params & 0x8000)

typedef struct ObjTree {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ ObjTreeActionFunc actionFunc;
    /* 0x1AC */ f32 swayAmplitude;
    /* 0x1B0 */ s16 swayAngle;
    /* 0x1B2 */ s16 swayVelocity;
    /* 0x1B4 */ s16 timer;
} ObjTree; // size = 0x1B8

#endif // Z_OBJ_TREE_H

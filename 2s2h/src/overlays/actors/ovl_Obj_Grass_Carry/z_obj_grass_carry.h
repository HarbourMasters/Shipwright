#ifndef Z_OBJ_GRASS_CARRY_H
#define Z_OBJ_GRASS_CARRY_H

#include "global.h"

struct ObjGrassCarry;
struct ObjGrass;
struct ObjGrassElement;

typedef void (*ObjGrassCarryActionFunc)(struct ObjGrassCarry*, PlayState*);

typedef struct ObjGrassCarry {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ struct ObjGrass* grassManager;
    /* 0x194 */ struct ObjGrassElement* grassElem;
    /* 0x198 */ s16 dropTable;
    /* 0x19A */ s16 fallTimer;
    /* 0x19C */ ObjGrassCarryActionFunc actionFunc;
} ObjGrassCarry; // size = 0x1A0

#endif // Z_OBJ_GRASS_CARRY_H

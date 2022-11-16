#ifndef Z_DOOR_ANA_H
#define Z_DOOR_ANA_H

#include "ultra64.h"
#include "global.h"

struct DoorAna;

typedef void (*DoorAnaActionFunc)(struct DoorAna*, PlayState*);

typedef struct DoorAna {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ DoorAnaActionFunc actionFunc;
} DoorAna; // size = 0x019C

#endif

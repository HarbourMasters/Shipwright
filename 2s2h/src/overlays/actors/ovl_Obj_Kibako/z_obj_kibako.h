#ifndef Z_OBJ_KIBAKO_H
#define Z_OBJ_KIBAKO_H

#include "global.h"

#define KIBAKO_COLLECTIBLE_ID(thisx) ((thisx)->params & 0x3F)
#define KIBAKO_COLLECTIBLE_FLAG(thisx) (((thisx)->params >> 0x8) & 0x7F)
#define KIBAKO_BANK_INDEX(thisx) (((thisx)->params >> 0xF) & 1)
#define KIBAKO_BOMBER_CAN_HIDE_IN_BOX(thisx) (((thisx)->params >> 7) & 1)

struct ObjKibako;

typedef void (*ObjKibakoActionFunc)(struct ObjKibako*, PlayState*);

typedef struct ObjKibako {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjKibakoActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ s16 timer;
    /* 0x196 */ s16 objectSlot;
    /* 0x198 */ s8 isDropCollected;
    /* 0x199 */ s8 unk199; /* room? */
} ObjKibako; // size = 0x19C

#endif // Z_OBJ_KIBAKO_H

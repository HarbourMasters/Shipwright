#ifndef Z_OBJ_LIGHTBLOCK_H
#define Z_OBJ_LIGHTBLOCK_H

#include "global.h"

#define LIGHTBLOCK_TYPE(thisx) ((thisx)->params & 1)
#define LIGHTBLOCK_GET_DESTROYED_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)

struct ObjLightblock;

typedef void (*ObjLightblockActionFunc)(struct ObjLightblock*, PlayState*);

typedef struct ObjLightblock {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ ObjLightblockActionFunc actionFunc;
    /* 0x1AC */ s8 timer;
    /* 0x1AD */ u8 alpha;
    /* 0x1AE */ s8 collisionCounter;
} ObjLightblock; // size = 0x1B0

#endif // Z_OBJ_LIGHTBLOCK_H

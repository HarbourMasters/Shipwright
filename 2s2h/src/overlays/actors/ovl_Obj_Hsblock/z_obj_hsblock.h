#ifndef Z_OBJ_HSBLOCK_H
#define Z_OBJ_HSBLOCK_H

#include "global.h"

#define OBJHSBLOCK_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)
#define OBJHSBLOCK_GET_3(thisx) ((thisx)->params & 3)
#define OBJHSBLOCK_GET_5(thisx) (((thisx)->params >> 5) & 1)
#define OBJHSBLOCK_GET_6(thisx) (((thisx)->params >> 6) & 3)

struct ObjHsblock;

typedef void (*ObjHsblockActionFunc)(struct ObjHsblock*, PlayState*);

typedef struct ObjHsblock {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjHsblockActionFunc actionFunc;
} ObjHsblock; // size = 0x160

#endif // Z_OBJ_HSBLOCK_H

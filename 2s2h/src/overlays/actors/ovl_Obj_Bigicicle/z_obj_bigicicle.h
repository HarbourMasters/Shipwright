#ifndef Z_OBJ_BIGICICLE_H
#define Z_OBJ_BIGICICLE_H

#include "global.h"

struct ObjBigicicle;

typedef void (*ObjBigicicleActionFunc)(struct ObjBigicicle*, PlayState*);

#define OBJBIGICLE_GET_SWITCH_FLAG_MASK(thisx) ((thisx)->params & 0xFF)
#define OBJBIGICLE_GET_SWITCH_FLAG(thisx) ((thisx)->params)
#define OBJBIGICLE_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

typedef struct ObjBigicicle {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjBigicicleActionFunc actionFunc;
    /* 0x148 */ u8 unk_148;
    /* 0x149 */ u8 unk_149;
    /* 0x14A */ s16 unk_14A;
    /* 0x14C */ ColliderCylinder collider1;
    /* 0x198 */ ColliderCylinder collider2;
} ObjBigicicle; // size = 0x1E4

#endif // Z_OBJ_BIGICICLE_H

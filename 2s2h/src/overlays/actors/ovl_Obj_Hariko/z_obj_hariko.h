#ifndef Z_OBJ_HARIKO_H
#define Z_OBJ_HARIKO_H

#include "global.h"

struct ObjHariko;

typedef void (*ObjHarikoActionFunc)(struct ObjHariko*, PlayState*);

typedef struct ObjHariko {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjHarikoActionFunc actionFunc;
    /* 0x148 */ f32 bobbleStep;
    /* 0x14C */ Vec3s headRot;
    /* 0x152 */ s16 headOffset;
    /* 0x154 */ s16 unk154; // Set but not used
} ObjHariko; // size = 0x158

#endif // Z_OBJ_HARIKO_H

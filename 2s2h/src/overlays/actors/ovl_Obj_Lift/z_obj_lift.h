#ifndef Z_OBJ_LIFT_H
#define Z_OBJ_LIFT_H

#include "global.h"

#define OBJLIFT_GET_1(thisx) ((thisx)->params & 1)
#define OBJLIFT_GET_7(thisx) (((thisx)->params >> 8) & 7)
#define OBJLIFT_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 1) & 0x7F)

struct ObjLift;

typedef void (*ObjLiftActionFunc)(struct ObjLift*, PlayState*);

typedef struct ObjLift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjLiftActionFunc actionFunc;
    /* 0x160 */ s16 unk_160;
    /* 0x162 */ s16 unk_162;
    /* 0x164 */ s16 unk_164;
    /* 0x166 */ s16 timer;
    /* 0x168 */ Vec3f unk_168;
    /* 0x174 */ s16 unk_174;
    /* 0x176 */ s16 unk_176;
    /* 0x178 */ s16 unk_178;
} ObjLift; // size = 0x17C

#endif // Z_OBJ_LIFT_H

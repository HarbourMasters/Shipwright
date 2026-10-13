#ifndef Z_OBJ_LUPYGAMELIFT_H
#define Z_OBJ_LUPYGAMELIFT_H

#include "global.h"

#define OBJLUPYGAMELIFT_GET_PATH_INDEX(thisx) ((thisx)->params & 0x7F)
#define OBJLUPYGAMELIFT_GET_7(thisx) (((thisx)->params >> 7) & 0x1F)
#define OBJLUPYGAMELIFT_GET_C(thisx) (((thisx)->params >> 0xC) & 1)

struct ObjLupygamelift;

typedef void (*ObjLupygameliftActionFunc)(struct ObjLupygamelift*, PlayState*);

typedef struct ObjLupygamelift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjLupygameliftActionFunc actionFunc;
    /* 0x160 */ f32 targetSpeedXZ;
    /* 0x164 */ s32 count;
    /* 0x168 */ s32 pointIndex;
    /* 0x16C */ Vec3s* pathPoints;
    /* 0x170 */ s16 timer;
} ObjLupygamelift; /* size = 0x174 */

#endif // Z_OBJ_LUPYGAMELIFT_H

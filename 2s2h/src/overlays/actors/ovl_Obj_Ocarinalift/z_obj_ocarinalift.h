#ifndef Z_OBJ_OCARINALIFT_H
#define Z_OBJ_OCARINALIFT_H

#include "global.h"

#define OBJOCARINALIFT_GET_C(thisx) (((thisx)->params >> 0xC) & 3)
#define OBJOCARINALIFT_GET_PATH_INDEX(thisx) ((thisx)->params & 0x7F)
#define OBJOCARINALIFT_GET_1F(thisx) (((thisx)->params >> 7) & 0x1F)
#define OBJOCARINALIFT_GET_SWITCH_FLAG(thisx) ((thisx)->home.rot.x & 0x7F)

struct ObjOcarinalift;

typedef void (*ObjOcarinaliftActionFunc)(struct ObjOcarinalift*, PlayState*);

typedef struct ObjOcarinalift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjOcarinaliftActionFunc actionFunc;
    /* 0x160 */ f32 unk160;
    /* 0x164 */ s32 unk164;
    /* 0x168 */ s32 unk168;
    /* 0x16C */ s32 unk16C;
    /* 0x170 */ Vec3s* pathPoints;
    /* 0x174 */ s16 timer;
    /* 0x176 */ s16 cutsceneTimer;
} ObjOcarinalift; // size = 0x178

typedef enum ObjOcarinaliftParam {
    /* 0 */ OBJOCARINALIFT_PARAM_0,
    /* 1 */ OBJOCARINALIFT_PARAM_1
} ObjOcarinaliftParam;

#endif // Z_OBJ_OCARINALIFT_H

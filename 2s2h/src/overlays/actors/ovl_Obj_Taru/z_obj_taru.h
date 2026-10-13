#ifndef Z_OBJ_TARU_H
#define Z_OBJ_TARU_H

#include "global.h"

#define OBJ_TARU_GET_100(thisx) ((thisx)->params & 0x100)
#define OBJ_TARU_GET_1F(thisx) ((thisx)->params & 0x1F)
#define OBJ_TARU_GET_3F(thisx) ((thisx)->params & 0x3F)
#define OBJ_TARU_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define OBJ_TARU_GET_7F00(thisx) (((thisx)->params >> 8) & 0x7F)
#define OBJ_TARU_GET_80(thisx) ((thisx)->params & 0x80)
#define OBJ_TARU_GET_8000(thisx) (((thisx)->params >> 0xF) & 1)

struct ObjTaru;

typedef void (*ObjTaruActionFunc)(struct ObjTaru*, PlayState*);

typedef struct ObjTaru {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ ObjTaruActionFunc actionFunc;
    /* 0x1AC */ s8 unk_1AC;
    /* 0x1AD */ s8 unk_1AD;
} ObjTaru; // size = 0x1B0

#endif // Z_OBJ_TARU_H

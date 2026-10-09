#ifndef Z_OBJ_HUGEBOMBIWA_H
#define Z_OBJ_HUGEBOMBIWA_H

#include "global.h"

struct ObjHugebombiwa;

typedef void (*ObjHugebombiwaActionFunc)(struct ObjHugebombiwa*, PlayState*);

#define ENHUGEBOMBIWA_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define ENHUGEBOMBIWA_GET_100(thisx) (((thisx)->params >> 8) & 1)

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ Vec3s unk_1C;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ s16 unk_24;
} EnHugebombiwaStruct; // size = 0x28

typedef struct ObjHugebombiwa {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ EnHugebombiwaStruct unk_190[20];
    /* 0x4B0 */ s16 unk_4B0;
    /* 0x4B2 */ s8 unk_4B2;
    /* 0x4B3 */ s8 unk_4B3;
    /* 0x4B4 */ ObjHugebombiwaActionFunc actionFunc;
} ObjHugebombiwa; // size = 0x4B8

#endif // Z_OBJ_HUGEBOMBIWA_H

#ifndef Z_OBJ_BOMBIWA_H
#define Z_OBJ_BOMBIWA_H

#include "global.h"

struct ObjBombiwa;

typedef void (*ObjBombiwaActionFunc)(struct ObjBombiwa*, PlayState*);

#define OBJBOMBIWA_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define OBJBOMBIWA_GET_100(thisx) (((thisx)->params >> 8) & 1)
#define OBJBOMBIWA_GET_8000(thisx) (((thisx)->params >> 0xF) & 1)

#define OBJBOMBIWA_100_0 0

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ Vec3s unk_14;
    /* 0x1A */ s16 unk_1A;
} ObjBombiwaStruct; // size = 0x1C

typedef struct ObjBombiwa {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ ObjBombiwaStruct unk_190[4];
    /* 0x200 */ s8 unk_200;
    /* 0x201 */ s8 unk_201;
    /* 0x202 */ s8 unk_202;
    /* 0x203 */ u8 unk_203;
    /* 0x204 */ ObjBombiwaActionFunc actionFunc;
} ObjBombiwa; // size = 0x208

#endif // Z_OBJ_BOMBIWA_H

#ifndef Z_OBJ_DHOUSE_H
#define Z_OBJ_DHOUSE_H

#include "global.h"

struct ObjDhouse;

typedef void (*ObjDhouseActionFunc)(struct ObjDhouse*, PlayState*);

#define OBJDHOUSE_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3s unk_18;
    /* 0x1E */ s8 unk_1E;
} ObjDhouseStruct1; // size = 0x20

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ Vec3s unk_1C;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ s16 unk_24;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ s8 unk_28;
    /* 0x29 */ u8 unk_29;
} ObjDhouseStruct2; // size = 0x2C

typedef struct {
    /* 0x00 */ Gfx* unk_00;
    /* 0x04 */ Gfx* unk_04;
    /* 0x08 */ Vec3f unk_08;
    /* 0x14 */ Vec3f unk_14;
    /* 0x20 */ Vec3s unk_20;
    /* 0x26 */ s8 unk_26;
} ObjDhouseStruct3; // size = 0x28

typedef struct ObjDhouse {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x015C */ ObjDhouseActionFunc actionFunc;
    /* 0x0160 */ ObjDhouseStruct1 unk_160[7];
    /* 0x0240 */ ObjDhouseStruct2 unk_240[100];
    /* 0x1370 */ s16 unk_1370;
} ObjDhouse; // size = 0x1374

#endif // Z_OBJ_DHOUSE_H

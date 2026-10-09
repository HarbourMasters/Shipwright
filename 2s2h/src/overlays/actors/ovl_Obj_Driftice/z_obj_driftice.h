#ifndef Z_OBJ_DRIFTICE_H
#define Z_OBJ_DRIFTICE_H

#include "global.h"

struct ObjDriftice;

typedef void (*ObjDrifticeActionFunc)(struct ObjDriftice*, PlayState*);

#define OBJDRIFTICE_GET_3(thisx) ((thisx)->params & 3)
#define OBJDRIFTICE_GET_PATH_INDEX(thisx) (((thisx)->params >> 2) & 0x7F)
#define OBJDRIFTICE_GET_E00(thisx) (((thisx)->params >> 9) & 7)
#define OBJDRIFTICE_GET_1000(thisx) (((thisx)->params >> 0xC) & 1)
#define OBJDRIFTICE_GET_ROT(thisx) ((thisx)->home.rot.x & 3)

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
} ObjDrifticeStruct3; // size = 0x14

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ ObjDrifticeStruct3 unk_0C[2];
} ObjDrifticeStruct2; // size = 0x34

typedef struct {
    /* 0x00 */ s16 unk_00[6];
    /* 0x0C */ ObjDrifticeStruct3 unk_0C[3];
} ObjDrifticeStruct4; // size = 0x48

typedef struct {
    /* 0x00 */ ObjDrifticeStruct3 unk_00;
    /* 0x14 */ ObjDrifticeStruct3 unk_14[3];
    /* 0x50 */ ObjDrifticeStruct2 unk_50;
    /* 0x84 */ ObjDrifticeStruct4 unk_84;
} ObjDrifticeStruct; // size = 0xCC

typedef struct ObjDriftice {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjDrifticeActionFunc actionFunc;
    /* 0x160 */ s32 unk_160;
    /* 0x164 */ s32 unk_164;
    /* 0x168 */ s32 unk_168;
    /* 0x16C */ Vec3s* pathPoints;
    /* 0x170 */ ObjDrifticeStruct unk_170;
    /* 0x23C */ f32 unk_23C;
    /* 0x240 */ f32 unk_240;
    /* 0x244 */ s16 unk_244;
    /* 0x248 */ s32 unk_248;
    /* 0x24C */ s32 unk_24C;
} ObjDriftice; // size = 0x250

#endif // Z_OBJ_DRIFTICE_H

#ifndef Z_OBJ_CHIKUWA_H
#define Z_OBJ_CHIKUWA_H

#include "global.h"

struct ObjChikuwa;

typedef struct {
    /* 0x0 */ Vec3f unk_00;
    /* 0xC */ s16 unk_0C;
    /* 0xE */ u8 unk_0E;
} ObjChikuwaStruct; // size = 0x10

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ Vec3s unk_28;
    /* 0x2E */ Vec3s unk_2E;
    /* 0x34 */ s16 unk_34;
} ObjChikuwaStruct2; // size = 0x38

typedef struct ObjChikuwa {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjChikuwaStruct unk_15C[20];
    /* 0x29C */ s16 unk_29C;
    /* 0x29E */ s16 unk_29E;
    /* 0x2A0 */ s16 unk_2A0;
    /* 0x2A4 */ ObjChikuwaStruct2 unk_2A4[32];
    /* 0x9A4 */ s16 unk_9A4;
    /* 0x9A6 */ u8 unk_9A6;
} ObjChikuwa; // size = 0x9A8

#endif // Z_OBJ_CHIKUWA_H

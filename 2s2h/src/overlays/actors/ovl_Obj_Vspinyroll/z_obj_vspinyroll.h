#ifndef Z_OBJ_VSPINYROLL_H
#define Z_OBJ_VSPINYROLL_H

#include "global.h"

struct ObjVspinyroll;

typedef void (*ObjVspinyrollActionFunc)(struct ObjVspinyroll*, PlayState*);

#define OBJVSPINYROLL_GET_PATH_INDEX(thisx) ((thisx)->params & 0x7F)
#define OBJVSPINYROLL_GET_380(thisx) (((thisx)->params >> 7) & 7)
#define OBJVSPINYROLL_GET_1C00(thisx) (((thisx)->params >> 0xA) & 7)
#define OBJVSPINYROLL_GET_4000(thisx) (((thisx)->params >> 0xE) & 1)

#define OBJVSPINYROLL_PATH_INDEX_NONE 0x7F
#define OBJVSPINYROLL_4000_1 1

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ CollisionPoly* collisionPoly;
    /* 0x10 */ s32 bgId;
} ObjVspinyrollStruct2; // size = 0x14

typedef struct {
    /* 0x000 */ ObjVspinyrollStruct2 unk_000[24];
    /* 0x1E0 */ ObjVspinyrollStruct2* unk_1E0;
    /* 0x1E4 */ s32 unk_1E4;
} ObjVspinyrollStruct3; // size = 0x1E8

typedef struct ObjVspinyroll {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ ObjVspinyrollStruct3 unk_1A8;
    /* 0x390 */ ObjVspinyrollActionFunc actionFunc;
    /* 0x394 */ f32 unk_394;
    /* 0x398 */ s32 unk_398;
    /* 0x39C */ Vec3f unk_39C[2];
    /* 0x3B4 */ Vec3f unk_3B4;
    /* 0x3C0 */ s16 unk_3C0;
    /* 0x3C2 */ s16 unk_3C2;
    /* 0x3C4 */ s16 unk_3C4;
    /* 0x3C6 */ s16 unk_3C6;
    /* 0x3C8 */ s16 unk_3C8;
} ObjVspinyroll; // size = 0x3CC

#endif // Z_OBJ_VSPINYROLL_H

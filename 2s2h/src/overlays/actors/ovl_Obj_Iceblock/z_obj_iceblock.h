#ifndef Z_OBJ_ICEBLOCK_H
#define Z_OBJ_ICEBLOCK_H

#include "global.h"

struct ObjIceblock;

typedef void (*ObjIceblockActionFunc)(struct ObjIceblock*, PlayState*);
typedef void (*ObjIceblockExtendedDrawFunc)(struct ObjIceblock*, PlayState*);

#define ICEBLOCK_GET_SNAP_ROT(thisx) (((thisx)->params >> 1) & 1)
#define ICEBLOCK_GET_ICEBERG(thisx) ((thisx)->params & 1)

typedef struct {
    /* 0x0 */ s16 unk_00;
    /* 0x2 */ s16 unk_02;
    /* 0x4 */ f32 unk_04;
    /* 0x8 */ f32 unk_08;
    /* 0xC */ f32 unk_0C;
} ObjIceBlockUnkStruct; // size = 0x10

typedef struct {
    /* 0x0 */ CollisionPoly* unk_00;
    /* 0x4 */ f32 unk_04;
    /* 0x8 */ s32 unk_08;
    /* 0xC */ f32 unk_0C;
} ObjIceBlockUnkStruct2; // size = 0x10

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ f32 unk_04;
} ObjIceBlockUnkStruct3; // size = 0x8

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s16 unk_14;
    /* 0x14 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
} ObjIceBlockUnkStruct4; // size = 0x24

typedef struct ObjIceblock {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ ObjIceblockActionFunc actionFunc;
    /* 0x1AC */ ObjIceblockExtendedDrawFunc extendedDrawFunc;
    /* 0x1B0 */ s32 unk_1B0;
    /* 0x1B4 */ ObjIceBlockUnkStruct unk_1B4[4];
    /* 0x1F4 */ ObjIceBlockUnkStruct2 unk_1F4[5];
    /* 0x244 */ f32 unk_244;
    /* 0x248 */ Vec3f unk_248;
    /* 0x254 */ Vec3s unk_254;
    /* 0x25C */ f32 unk_25C;
    /* 0x260 */ f32 unk_260;
    /* 0x264 */ f32* unk_264;
    /* 0x268 */ f32 unk_268;
    /* 0x26C */ s16 unk_26C;
    /* 0x26E */ s16 unk_26E[4];
    /* 0x276 */ s16 unk_276;
    /* 0x278 */ s16 unk_278;
    /* 0x27C */ ObjIceBlockUnkStruct4 unk_27C;
    /* 0x2A0 */ s16 stateTimer; // re-used per-actionFunc
    /* 0x2A2 */ s16 unk_2A2;
    /* 0x2A4 */ f32 unk_2A4;
    /* 0x2A8 */ f32 unk_2A8;
    /* 0x2AC */ s16 unk_2AC;
    /* 0x2AE */ s16 meltTimer; // starts at 450 frames = 22 seconds
    /* 0x2B0 */ s8 unk_2B0;
    /* 0x2B1 */ s8 spawnCutsceneTimer;
    /* 0x2B4 */ f32 unk_2B4;
} ObjIceblock; // size = 0x2B8

#endif // Z_OBJ_ICEBLOCK_H

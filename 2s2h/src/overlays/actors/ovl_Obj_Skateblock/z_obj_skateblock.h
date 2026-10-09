#ifndef Z_OBJ_SKATEBLOCK_H
#define Z_OBJ_SKATEBLOCK_H

#include "global.h"

struct ObjSkateblock;

typedef void (*ObjSkateblockActionFunc)(struct ObjSkateblock*, PlayState*);

#define OBJSKAEBLOCK_GET_F(thisx) ((thisx)->params & 0xF)

typedef struct {
    /* 0x0 */ CollisionPoly* poly;
    /* 0x4 */ f32 height;
    /* 0x8 */ s32 bgId;
} ObjSkateblockFloor; // size = 0xC

typedef struct ObjSkateblock {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjSkateblockActionFunc actionFunc;
    /* 0x160 */ f32 unk_160;
    /* 0x164 */ f32 unk_164;
    /* 0x168 */ f32 unk_168;
    /* 0x16C */ f32* unk_16C;
    /* 0x170 */ s16 unk_170;
    /* 0x172 */ s16 unk_172[4];
    /* 0x17A */ s16 unk_17A;
    /* 0x17C */ s16 unk_17C;
    /* 0x180 */ f32 unk_180;
    /* 0x184 */ ObjSkateblockFloor unk_184[5];
    /* 0x1C0 */ u8 unk_1C0;
    /* 0x1C1 */ u8 unk_1C1;
} ObjSkateblock; // size = 0x1C4

#endif // Z_OBJ_SKATEBLOCK_H

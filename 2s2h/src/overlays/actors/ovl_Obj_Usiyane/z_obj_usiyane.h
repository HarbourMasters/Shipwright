#ifndef Z_OBJ_USIYANE_H
#define Z_OBJ_USIYANE_H

#include "global.h"

struct ObjUsiyane;

typedef void (*ObjUsiyaneActionFunc)(struct ObjUsiyane*, PlayState*);

#define OBJUSIYANE_GET_F(thisx) ((thisx)->params & 0xF)

typedef enum {
    /* 0 */ OBJUSIYANE_F_0,
    /* 1 */ OBJUSIYANE_F_1
} ObjUsiyaneParam;

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3s unk_18;
    /* 0x1E */ Vec3s unk_1E;
} ObjUsiyaneStruct; // size = 0x24

typedef struct ObjUsiyane {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjUsiyaneActionFunc actionFunc;
    /* 0x160 */ s32 cueChannel;
    /* 0x164 */ s32 cueId;
    /* 0x168 */ ObjUsiyaneStruct unk_168[10][4];
    /* 0x708 */ f32 unk_708;
    /* 0x70C */ f32 unk_70C;
    /* 0x710 */ Vec3f unk_710[4];
    /* 0x740 */ s32 unk_740;
    /* 0x744 */ s32 unk_744;
} ObjUsiyane; // size = 0x748

#endif // Z_OBJ_USIYANE_H

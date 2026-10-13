#ifndef Z_OBJ_SNOWBALL2_H
#define Z_OBJ_SNOWBALL2_H

#include "global.h"

struct ObjSnowball2;

typedef void (*ObjSnowball2ActionFunc)(struct ObjSnowball2*, PlayState*);

#define ENOBJSNOWBALL2_GET_3F(thisx) ((thisx)->params & 0x3F)
#define ENOBJSNOWBALL2_GET_7F00(thisx) (((thisx)->params >> 8) & 0x7F)

typedef struct ObjSnowball2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider;
    /* 0x164 */ ColliderJntSphElement colliderElements[1];
    /* 0x1A4 */ ObjSnowball2ActionFunc actionFunc;
    /* 0x1A8 */ s16 unk_1A8;
    /* 0x1AA */ s16 unk_1AA;
    /* 0x1AC */ s8 unk_1AC;
    /* 0x1AD */ u8 unk_1AD;
    /* 0x1AE */ s8 unk_1AE;
    /* 0x1AF */ s8 unk_1AF;
    /* 0x1B0 */ UNK_TYPE1 unk1B0[4];
} ObjSnowball2; // size = 0x1B4

#endif // Z_OBJ_SNOWBALL2_H

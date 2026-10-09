#ifndef Z_EN_ZOW_H
#define Z_EN_ZOW_H

#include "global.h"
#include "assets/objects/object_zo/object_zo.h"

struct EnZow;

typedef void (*EnZowActionFunc)(struct EnZow*, PlayState*);

#define ENZOW_GET_F(thisx) ((thisx)->params & 0xF)

#define ENZOW_F_1 1

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ UNK_TYPE1 unk_0C[0x3];
    /* 0x0F */ u8 unk_0F;
    /* 0x10 */ UNK_TYPE1 unk_10[0x4];
    /* 0x14 */ Vec3f unk_14;
    /* 0x20 */ Vec3f unk_20;
    /* 0x2C */ Vec3f unk_2C;
} EnZowStruct; // size = 0x38

typedef struct EnZow {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ ColliderCylinder collider;
    /* 0x1D4 */ Vec3s jointTable[ZORA_LIMB_MAX];
    /* 0x24C */ Vec3s morphTable[ZORA_LIMB_MAX];
    /* 0x2C4 */ s16 unk_2C4;
    /* 0x2C6 */ s16 unk_2C6;
    /* 0x2C8 */ s16 animIndex;
    /* 0x2CA */ u16 unk_2CA;
    /* 0x2CC */ s16 unk_2CC;
    /* 0x2CE */ s16 unk_2CE;
    /* 0x2D0 */ EnZowStruct unk_2D0[15];
    /* 0x618 */ EnZowActionFunc actionFunc;
} EnZow; // size = 0x61C

#endif // Z_EN_ZOW_H

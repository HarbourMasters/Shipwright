#ifndef Z_EN_TANRON1_H
#define Z_EN_TANRON1_H

#include "global.h"

struct EnTanron1;

#define ENTANRON1_GET_100(thisx) ((thisx)->params & 0x100)

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ UNK_TYPE1 unk_22[0x2];
    /* 0x24 */ u8 unk_24;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ s16 unk_28;
    /* 0x2A */ s16 unk_2A;
    /* 0x2C */ f32 unk_2C;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ f32 unk_3C;
} EnTanron1Struct; // size = 0x40

typedef struct EnTanron1 {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ u8 unk_144;
    /* 0x0145 */ UNK_TYPE1 pad145[3];
    /* 0x0148 */ s16 unk_148;
    /* 0x014C */ Vec3f unk_14C;
    /* 0x0158 */ s16 unk_158;
    /* 0x015C */ f32 unk_15C;
    /* 0x0160 */ EnTanron1Struct unk_160[200];
    /* 0x3360 */ Vec3f unk_3360;
} EnTanron1; // size = 0x336C

#endif // Z_EN_TANRON1_H

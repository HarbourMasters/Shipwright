#ifndef Z_EN_DNB_H
#define Z_EN_DNB_H

#include "global.h"

struct EnDnb;

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3s unk_18;
    /* 0x1E */ Vec3s unk_1E;
    /* 0x24 */ s16 unk_24;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ UNK_TYPE1 unk_28[0x4];
    /* 0x2C */ f32 unk_2C;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
} EnDnbEffect; // size = 0x38

typedef struct {
    /* 0x00 */ u8 isEnabled;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ f32 unk_24;
} EnDnbUnkStruct; // size = 0x28

#define EN_DNB_EFFECT_COUNT 54

typedef struct EnDnb {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x015C */ UNK_TYPE1 unk_015C[0x4];
    /* 0x0160 */ EnDnbEffect effects[EN_DNB_EFFECT_COUNT];
    /* 0x0D30 */ s16 unk_0D30;
    /* 0x0D32 */ s16 unk_0D32;
    /* 0x0D34 */ s16 unk_0D34;
    /* 0x0D38 */ EnDnbUnkStruct unk_0D38[256];
} EnDnb; // size = 0x3538

#endif // Z_EN_DNB_H

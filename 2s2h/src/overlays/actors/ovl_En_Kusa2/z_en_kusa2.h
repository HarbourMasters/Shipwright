#ifndef Z_EN_KUSA2_H
#define Z_EN_KUSA2_H

#include "global.h"

struct EnKusa2;
struct EnKusa2UnkBssSubStruct;

typedef void (*EnKusa2ActionFunc)(struct EnKusa2*, PlayState*);
typedef void (*EnKusa2BssFunc)(struct EnKusa2UnkBssSubStruct*);

#define ENKUSA2_GET_1(thisx) ((thisx)->params & 1)
#define ENKUSA2_GET_7F00(thisx) (u8)(((thisx)->params >> 8) & 0x7F)

typedef struct EnKusa2UnkBssSubStruct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ s16 unk_24;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ EnKusa2BssFunc unk_28;
    /* 0x2C */ f32 unk_2C;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ f32 unk_3C;
    /* 0x40 */ f32 unk_40;
    /* 0x44 */ f32 unk_44;
    /* 0x48 */ f32 unk_48;
    /* 0x4C */ f32 unk_4C;
    /* 0x50 */ f32 unk_50;
    /* 0x54 */ f32 unk_54;
    /* 0x58 */ f32 unk_58;
    /* 0x5C */ f32 unk_5C;
    /* 0x60 */ f32 unk_60;
    /* 0x64 */ f32 unk_64;
    /* 0x68 */ f32 unk_68;
    /* 0x6C */ f32 unk_6C;
    /* 0x70 */ f32 unk_70;
    /* 0x74 */ f32 unk_74;
    /* 0x78 */ f32 unk_78;
    /* 0x7C */ f32 unk_7C;
} EnKusa2UnkBssSubStruct; // size = 0x80

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ Vec3s unk_20;
    /* 0x26 */ Vec3s unk_26;
    /* 0x2C */ s16 unk_2C;
    /* 0x2C */ UNK_TYPE1 unk2E[0x2];
} EnKusa2UnkBssSubStruct2; // size = 0x30

typedef struct {
    /* 0x0000 */ EnKusa2UnkBssSubStruct unk_0000[9];
    /* 0x0480 */ EnKusa2UnkBssSubStruct2 unk_0480[100];
} EnKusa2UnkBssStruct; // size = 0x1740

typedef struct EnKusa2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ EnKusa2ActionFunc actionFunc;
    /* 0x194 */ struct EnKusa2* unk_194[9];
    /* 0x1B8 */ EnKusa2UnkBssSubStruct* unk_1B8;
    /* 0x1BC */ s16 unk_1BC;
    /* 0x1BE */ s8 unk_1BE;
    /* 0x1C0 */ struct EnKusa2* unk_1C0;
    /* 0x1C4 */ s16 unk_1C4;
    /* 0x1C6 */ UNK_TYPE1 unk1C6[0x2];
    /* 0x1C8 */ s16 unk_1C8;
    /* 0x1CA */ s16 unk_1CA;
    /* 0x1CC */ s16 unk_1CC;
    /* 0x1CE */ s8 unk_1CE;
    /* 0x1CF */ u8 unk_1CF;
    /* 0x1D0 */ s8 unk_1D0;
    /* 0x1D1 */ s8 unk_1D1;
} EnKusa2; // size = 0x1D4

#endif // Z_EN_KUSA2_H

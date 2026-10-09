#ifndef Z_EN_HANABI_H
#define Z_EN_HANABI_H

#include "global.h"

struct EnHanabi;

typedef void (*EnHanabiActionFunc)(struct EnHanabi*, PlayState*);

#define ENHANABI_GET_1F(thisx) ((thisx)->params & 0x1F)

#define ENHANABI_1F_1 1

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ Vec3f unk_08;
    /* 0x14 */ Vec3f unk_14;
    /* 0x20 */ Vec3f unk_20;
} EnHanabiStruct; // size = 0x2C

typedef struct EnHanabi {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ s16 unk_144;
    /* 0x0148 */ EnHanabiStruct unk_148[400];
    /* 0x4608 */ LightInfo unk_4608[3];
    /* 0x4634 */ LightNode* unk_4634[3];
    /* 0x4640 */ s16 unk_4640[3];
    /* 0x4648 */ s32 unk_4648[3];
    /* 0x4654 */ EnHanabiActionFunc actionFunc;
} EnHanabi; // size = 0x4658

#endif // Z_EN_HANABI_H

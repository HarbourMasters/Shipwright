#ifndef Z_EN_FZ_H
#define Z_EN_FZ_H

#include "global.h"

struct EnFz;

typedef void (*EnFzActionFunc)(struct EnFz*, PlayState*);
typedef void (*EnFzUnkFunc)(struct EnFz*);

#define ENFZ_GET_F(thisx) ((thisx)->params & 0xF)
#define ENFZ_GET_3000(thisx) (((thisx)->params & 0x3000) >> 0xC)
#define ENFZ_GET_4000(thisx) ((thisx)->params & 0x4000)
#define ENFZ_GET_8000(thisx) ((thisx)->params & 0x8000)

typedef enum {
    /* 0 */ ENFZ_F_0,
    /* 1 */ ENFZ_F_1,
    /* 2 */ ENFZ_F_2,
    /* 3 */ ENFZ_F_3
} EnFzParam;

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ Vec3f unk_1C;
    /* 0x28 */ UNK_TYPE1 unk_28[0x4];
    /* 0x2C */ s16 unk_2C;
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ u8 unk_38;
} EnFzStruct; // size = 0x3C

typedef struct EnFz {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnFzActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider1;
    /* 0x194 */ ColliderCylinder collider2;
    /* 0x1E0 */ ColliderCylinder collider3;
    /* 0x22C */ Vec3f unk_22C;
    /* 0x238 */ f32 unk_238;
    /* 0x23C */ EnFzStruct unk_23C[40];
    /* 0xB9C */ f32 drawDmgEffAlpha;
    /* 0xBA0 */ f32 drawDmgEffScale;
    /* 0xBA4 */ s16 drawDmgEffTimer;
    /* 0xBA8 */ f32 unk_BA8;
    /* 0xBAC */ f32 unk_BAC;
    /* 0xBB0 */ f32 unk_BB0;
    /* 0xBB4 */ f32 unk_BB4;
    /* 0xBB8 */ f32 unk_BB8;
    /* 0xBBC */ f32 unk_BBC;
    /* 0xBC0 */ u32 unk_BC0;
    /* 0xBC4 */ s16 unk_BC4;
    /* 0xBC6 */ s16 unk_BC6;
    /* 0xBC8 */ s16 unk_BC8;
    /* 0xBCA */ s16 unk_BCA;
    /* 0xBCC */ u8 unk_BCC;
    /* 0xBCD */ u8 unk_BCD;
    /* 0xBCE */ u8 unk_BCE;
    /* 0xBCF */ u8 unk_BCF;
    /* 0xBD0 */ s16 unk_BD0;
    /* 0xBD2 */ s16 unk_BD2;
    /* 0xBD4 */ UNK_TYPE1 unkBD4[2];
    /* 0xBD6 */ u8 unk_BD6;
    /* 0xBD7 */ u8 unk_BD7;
    /* 0xBD8 */ u8 unk_BD8;
    /* 0xBD9 */ u8 unk_BD9;
} EnFz; /* size = 0xBDC */

#endif // Z_EN_FZ_H

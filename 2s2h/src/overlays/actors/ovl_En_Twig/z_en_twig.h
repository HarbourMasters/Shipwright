#ifndef Z_EN_TWIG_H
#define Z_EN_TWIG_H

#include "global.h"

struct EnTwig;

typedef void (*EnTwigActionFunc)(struct EnTwig*, PlayState*);

#define RACERING_GET_PARAM_F(thisx) ((thisx)->params & 0xF)
#define RACERING_GET_PARAM_1F0(thisx) (((thisx)->params >> 4) & 0x1F)
#define RACERING_GET_PARAM_FE0(thisx) (((thisx)->params >> 9) & 0x7F)

typedef struct EnTwig {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ EnTwigActionFunc actionFunc;
    /* 0x160 */ s32 unk_160;
    /* 0x160 */ UNK_TYPE1 unk164[8];
    /* 0x16C */ s32 unk_16C;
    /* 0x170 */ f32 unk_170;
    /* 0x174 */ f32 unk_174;
    /* 0x178 */ s16 unk_178;
    /* 0x17A */ u16 unk_17A;
    /* 0x17C */ UNK_TYPE1 unk17C[4];
    /* 0x180 */ Vec3f unk_180;
} EnTwig; // size = 0x18C

#endif // Z_EN_TWIG_H

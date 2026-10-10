#ifndef Z_EN_TEST2_H
#define Z_EN_TEST2_H

#include "global.h"

struct EnTest2;

#define ENTEST2_GET_TYPE(thisx) ((thisx)->params)

typedef enum EnTest2Type {
    /* 0x0 */ EN_TEST2_TYPE_0,
    /* 0x1 */ EN_TEST2_TYPE_1,
    /* 0x2 */ EN_TEST2_TYPE_2,
    /* 0x3 */ EN_TEST2_TYPE_3,
    /* 0x4 */ EN_TEST2_TYPE_4,
    /* 0x5 */ EN_TEST2_TYPE_5,
    /* 0x6 */ EN_TEST2_TYPE_6,
    /* 0x7 */ EN_TEST2_TYPE_7,
    /* 0x8 */ EN_TEST2_TYPE_8,
    /* 0x9 */ EN_TEST2_TYPE_9,
    /* 0xA */ EN_TEST2_TYPE_10,
    /* 0xB */ EN_TEST2_TYPE_11,
    /* 0xC */ EN_TEST2_TYPE_12,
    /* 0xD */ EN_TEST2_TYPE_MAX
} EnTest2Type;

typedef struct EnTest2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ AnimatedMaterial* animMat;
} EnTest2; // size = 0x148

#endif // Z_EN_TEST2_H

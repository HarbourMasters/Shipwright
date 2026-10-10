#ifndef Z_EN_ZL1_H
#define Z_EN_ZL1_H

#include "global.h"

struct EnZl1;

typedef struct EnZl1 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ char unk_144[0xC0];
} EnZl1; // size = 0x204

#endif // Z_EN_ZL1_H

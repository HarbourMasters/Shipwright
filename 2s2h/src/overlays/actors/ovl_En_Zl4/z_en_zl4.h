#ifndef Z_EN_ZL4_H
#define Z_EN_ZL4_H

#include "global.h"

struct EnZl4;

typedef void (*EnZl4ActionFunc)(struct EnZl4*, PlayState*);

typedef struct EnZl4 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ UNK_TYPE1 unk_188[0x108];
    /* 0x290 */ EnZl4ActionFunc actionFunc;
    /* 0x294 */ UNK_TYPE1 unk_294[0x4C];
    /* 0x2E0 */ u16 unk_2E0;
    /* 0x2E2 */ UNK_TYPE1 unk_2E2[0xE];
    /* 0x2F0 */ u32 alpha;
} EnZl4; // size = 0x2F4

#endif // Z_EN_ZL4_H

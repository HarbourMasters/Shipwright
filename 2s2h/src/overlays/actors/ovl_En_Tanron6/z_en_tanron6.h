#ifndef Z_EN_TANRON6_H
#define Z_EN_TANRON6_H

#include "global.h"

struct EnTanron6;

typedef void (*EnTanron6ActionFunc)(struct EnTanron6*, PlayState*);

typedef struct EnTanron6 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0xBC];
    /* 0x200 */ EnTanron6ActionFunc actionFunc;
} EnTanron6; // size = 0x204

#endif // Z_EN_TANRON6_H

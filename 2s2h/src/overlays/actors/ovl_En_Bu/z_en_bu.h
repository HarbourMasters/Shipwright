#ifndef Z_EN_BU_H
#define Z_EN_BU_H

#include "global.h"

struct EnBu;

typedef void (*EnBuActionFunc)(struct EnBu*, PlayState*);

typedef struct EnBu {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnBuActionFunc actionFunc;
    /* 0x148 */ UNK_TYPE1 unk148[0x44];
    /* 0x18C */ Gfx* displayListPtr;
} EnBu; // size = 0x190

#endif // Z_EN_BU_H

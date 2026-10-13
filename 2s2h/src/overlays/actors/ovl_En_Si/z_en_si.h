#ifndef Z_EN_SI_H
#define Z_EN_SI_H

#include "global.h"

#define ENSI_GET_CHEST_FLAG(thisx) (((thisx)->params & 0xFC) >> 2)

#define SPIDER_HOUSE_TOKENS_REQUIRED 30

struct EnSi;

typedef void (*EnSiActionFunc)(struct EnSi*, PlayState*);

typedef struct EnSi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnSiActionFunc actionFunc;
    /* 0x148 */ ColliderSphere collider;
} EnSi; // size = 0x1A0

#endif // Z_EN_SI_H

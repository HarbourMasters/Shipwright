#ifndef Z_EN_KUJIYA_H
#define Z_EN_KUJIYA_H

#include "global.h"

struct EnKujiya;

typedef void (*EnKujiyaActionFunc)(struct EnKujiya*, PlayState*);

typedef struct EnKujiya {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnKujiyaActionFunc actionFunc;
    /* 0x148 */ s16 timer;
    /* 0x14A */ s16 textId;
} EnKujiya; // size = 0x14C

#endif // Z_EN_KUJIYA_H

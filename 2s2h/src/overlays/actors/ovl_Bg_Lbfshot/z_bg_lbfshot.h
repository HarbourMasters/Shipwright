#ifndef Z_BG_LBFSHOT_H
#define Z_BG_LBFSHOT_H

#include "global.h"

struct BgLbfshot;

typedef struct BgLbfshot {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ UNK_TYPE1 pad15C[0x4];
} BgLbfshot; // size = 0x160

#endif // Z_BG_LBFSHOT_H

#ifndef Z_BG_HAKA_CURTAIN_H
#define Z_BG_HAKA_CURTAIN_H

#include "global.h"

struct BgHakaCurtain;

typedef void (*BgHakaCurtainActionFunc)(struct BgHakaCurtain*, PlayState*);

typedef struct BgHakaCurtain {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgHakaCurtainActionFunc actionFunc;
} BgHakaCurtain; // size = 0x160

#endif // Z_BG_HAKA_CURTAIN_H

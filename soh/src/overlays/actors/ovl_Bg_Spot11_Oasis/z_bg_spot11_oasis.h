#ifndef Z_BG_SPOT11_OASIS_H
#define Z_BG_SPOT11_OASIS_H

#include "ultra64.h"
#include "global.h"

struct BgSpot11Oasis;

typedef void (*BgSpot11OasisActionFunc)(struct BgSpot11Oasis*, PlayState*);

typedef struct BgSpot11Oasis {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgSpot11OasisActionFunc actionFunc;
    /* 0x0150 */ u8 unk_150;
    /* 0x0151 */ u8 unk_151;
} BgSpot11Oasis; // size = 0x0154

#endif

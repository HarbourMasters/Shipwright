#ifndef Z_BG_JYA_ZURERUKABE_H
#define Z_BG_JYA_ZURERUKABE_H

#include "ultra64.h"
#include "global.h"

struct BgJyaZurerukabe;

typedef void (*BgJyaZurerukabeActionFunc)(struct BgJyaZurerukabe*, PlayState*);

typedef struct BgJyaZurerukabe {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgJyaZurerukabeActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168;
    /* 0x016A */ s16 unk_16A;
    /* 0x016C */ s16 unk_16C;
    /* 0x016E */ s16 unk_16E;
} BgJyaZurerukabe; // size = 0x0170

#endif

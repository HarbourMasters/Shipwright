#ifndef Z_BG_JYA_KANAAMI_H
#define Z_BG_JYA_KANAAMI_H

#include "ultra64.h"
#include "global.h"

struct BgJyaKanaami;

typedef void (*BgJyaKanaamiActionFunc)(struct BgJyaKanaami*, PlayState*);

typedef struct BgJyaKanaami {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgJyaKanaamiActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168;
    /* 0x016A */ s16 unk_16A;
} BgJyaKanaami; // size = 0x016C

#endif

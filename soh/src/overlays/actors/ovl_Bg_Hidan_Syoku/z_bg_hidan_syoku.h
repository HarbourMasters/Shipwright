#ifndef Z_BG_HIDAN_SYOKU_H
#define Z_BG_HIDAN_SYOKU_H

#include "ultra64.h"
#include "global.h"

struct BgHidanSyoku;

typedef void (*BgHidanSyokuActionFunc)(struct BgHidanSyoku*, PlayState*);

typedef struct BgHidanSyoku {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHidanSyokuActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168;
    /* 0x016A */ s16 timer;
} BgHidanSyoku; // size = 0x016C

#endif

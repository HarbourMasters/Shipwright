#ifndef Z_BG_HIDAN_KOUSI_H
#define Z_BG_HIDAN_KOUSI_H

#include <libultraship/libultra.h>
#include "global.h"

struct BgHidanKousi;

typedef void (*BgHidanKousiActionFunc)(struct BgHidanKousi*, PlayState*);

typedef struct BgHidanKousi {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHidanKousiActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168;
} BgHidanKousi; // size = 0x016C

void BgHidanKousi_SetupAction(BgHidanKousi* bgHidanKousi, BgHidanKousiActionFunc actionFunc);
void func_80889C18(BgHidanKousi* bgHidanKousi, PlayState* play);

#endif

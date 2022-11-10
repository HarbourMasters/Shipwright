#ifndef Z_BG_HIDAN_HROCK_H
#define Z_BG_HIDAN_HROCK_H

#include "ultra64.h"
#include "global.h"

struct BgHidanHrock;

typedef void (*BgHidanHrockActionFunc)(struct BgHidanHrock*, PlayState*);

typedef struct BgHidanHrock {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHidanHrockActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168;
    /* 0x016A */ u8 unk_16A;
    /* 0x016C */ ColliderTris collider;
    /* 0x018C */ ColliderTrisElement colliderItems[2];
} BgHidanHrock; // size = 0x0244

#endif

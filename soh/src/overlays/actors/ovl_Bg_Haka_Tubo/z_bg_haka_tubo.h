#ifndef Z_BG_HAKA_TUBO_H
#define Z_BG_HAKA_TUBO_H

#include "ultra64.h"
#include "global.h"

struct BgHakaTubo;

typedef void (*BgHakaTuboActionFunc)(struct BgHakaTubo*, PlayState*);

typedef struct BgHakaTubo {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHakaTuboActionFunc actionFunc;
    /* 0x0168 */ s16 dropTimer;
    /* 0x016A */ s16 fireScroll;
    /* 0x016C */ ColliderCylinder potCollider;
    /* 0x01B8 */ ColliderCylinder flamesCollider;
} BgHakaTubo; // size = 0x0204

#endif

#ifndef Z_BG_HIDAN_RSEKIZOU_H
#define Z_BG_HIDAN_RSEKIZOU_H

#include "ultra64.h"
#include "global.h"

struct BgHidanRsekizou;

typedef struct BgHidanRsekizou {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ s16 bendFrame;
    /* 0x0166 */ s16 burnFrame;
    /* 0x0168 */ ColliderJntSph collider;
    /* 0x0188 */ ColliderJntSphElement colliderItems[6];
} BgHidanRsekizou; // size = 0x0308

#endif

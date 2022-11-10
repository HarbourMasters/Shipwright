#ifndef Z_BG_HIDAN_SEKIZOU_H
#define Z_BG_HIDAN_SEKIZOU_H

#include "ultra64.h"
#include "global.h"

struct BgHidanSekizou;

typedef void (*BgHidanSekizouUpdateFunc)(struct BgHidanSekizou*, PlayState*);

typedef struct BgHidanSekizou {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHidanSekizouUpdateFunc updateFunc;
    /* 0x0168 */ s16 unk_168[4];
    /* 0x0170 */ s16 unk_170;
    /* 0x0174 */ ColliderJntSph collider;
    /* 0x0194 */ ColliderJntSphElement elements[6];
} BgHidanSekizou; // size = 0x0314

#endif

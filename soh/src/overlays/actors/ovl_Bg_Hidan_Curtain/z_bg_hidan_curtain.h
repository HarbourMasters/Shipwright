#ifndef Z_BG_HIDAN_CURTAIN_H
#define Z_BG_HIDAN_CURTAIN_H

#include "ultra64.h"
#include "global.h"

struct BgHidanCurtain;

typedef void (*BgHidanCurtainActionFunc)(struct BgHidanCurtain*, PlayState*);

typedef struct BgHidanCurtain {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgHidanCurtainActionFunc actionFunc;
    /* 0x0150 */ u8 type;
    /* 0x0151 */ u8 treasureFlag;
    /* 0x0152 */ u8 size;
    /* 0x0153 */ u8 alpha;
    /* 0x0154 */ s16 timer;
    /* 0x0156 */ s16 texScroll;
    /* 0x0158 */ ColliderCylinder collider;
} BgHidanCurtain; // size = 0x01A4

#endif

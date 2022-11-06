#ifndef Z_BG_HIDAN_FWBIG_H
#define Z_BG_HIDAN_FWBIG_H

#include "ultra64.h"
#include "global.h"

struct BgHidanFwbig;

typedef void (*BgHidanFwbigActionFunc)(struct BgHidanFwbig*, PlayState*);

typedef struct BgHidanFwbig {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgHidanFwbigActionFunc actionFunc;
    /* 0x0150 */ s8 direction;
    /* 0x0151 */ u8 moveState;
    /* 0x0152 */ s16 timer;
    /* 0x0154 */ ColliderCylinder collider;
} BgHidanFwbig; // size = 0x01A0

#endif

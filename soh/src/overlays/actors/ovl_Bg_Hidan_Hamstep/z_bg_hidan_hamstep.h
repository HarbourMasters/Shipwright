#ifndef Z_BG_HIDAN_HAMSTEP_H
#define Z_BG_HIDAN_HAMSTEP_H

#include "ultra64.h"
#include "global.h"

struct BgHidanHamstep;

typedef void (*BgHidanHamstepActionFunc)(struct BgHidanHamstep*, PlayState*);

typedef struct BgHidanHamstep {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ColliderTris collider;
    /* 0x0184 */ ColliderTrisElement colliderItems[2];
    /* 0x023C */ BgHidanHamstepActionFunc actionFunc;
    /* 0x0240 */ s32 action;
    /* 0x0244 */ s32 unk_244;
} BgHidanHamstep; // size = 0x0248

#endif

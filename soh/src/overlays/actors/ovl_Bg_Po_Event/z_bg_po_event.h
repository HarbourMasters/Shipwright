#ifndef Z_BG_PO_EVENT_H
#define Z_BG_PO_EVENT_H

#include "ultra64.h"
#include "global.h"

struct BgPoEvent;

typedef void (*BgPoEventActionFunc)(struct BgPoEvent*, PlayState*);

typedef struct BgPoEvent {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgPoEventActionFunc actionFunc;
    /* 0x0168 */ u8 type;
    /* 0x0169 */ u8 index;
    /* 0x016A */ s8 direction;
    /* 0x016C */ s16 timer;
    /* 0x0170 */ ColliderTris collider;
    /* 0x0190 */ ColliderTrisElement colliderItems[2];
} BgPoEvent; // size = 0x0248

#endif

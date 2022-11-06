#ifndef Z_BG_SPOT15_RRBOX_H
#define Z_BG_SPOT15_RRBOX_H

#include "ultra64.h"
#include "global.h"

struct BgSpot15Rrbox;

typedef void (*BgSpot15RrboxActionFunc)(struct BgSpot15Rrbox*, PlayState*);

typedef struct BgSpot15Rrbox {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot15RrboxActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168; 
    /* 0x016C */ f32 unk_16C; 
    /* 0x0170 */ f32 unk_170;
    /* 0x0174 */ f32 unk_174;
    /* 0x0178 */ f32 unk_178;
    /* 0x017C */ f32 unk_17C;
    /* 0x0180 */ s32 bgId; // Id of BgActor beneath the box
} BgSpot15Rrbox; // size = 0x0184

#endif

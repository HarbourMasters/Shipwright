#ifndef Z_BG_SPOT01_FUSYA_H
#define Z_BG_SPOT01_FUSYA_H

#include "ultra64.h"
#include "global.h"

struct BgSpot01Fusya;

typedef void (*BgSpot01FusyaActionFunc)(struct BgSpot01Fusya*, PlayState*);

typedef struct BgSpot01Fusya {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgSpot01FusyaActionFunc actionFunc;
    /* 0x0150 */ char unk_150[0x4];
    /* 0x0154 */ f32 unk_154;
    /* 0x0158 */ f32 unk_158;
    /* 0x015C */ f32 unk_15C;
} BgSpot01Fusya; // size = 0x0160

#endif

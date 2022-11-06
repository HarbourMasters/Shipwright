#ifndef Z_DEMO_6K_H
#define Z_DEMO_6K_H

#include "ultra64.h"
#include "global.h"

struct Demo6K;

typedef void (*Demo6KActionFunc)(struct Demo6K*, PlayState*);

typedef struct Demo6K {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ Demo6KActionFunc initActionFunc;
    /* 0x0150 */ LightInfo lightInfo;
    /* 0x0160 */ LightNode* lightNode;
    /* 0x0164 */ f32 unk_164;
    /* 0x0168 */ f32 unk_168;
    /* 0x016C */ f32 unk_16C;
    /* 0x0170 */ f32 unk_170;
    /* 0x0174 */ f32 unk_174[16];
    /* 0x01B4 */ f32 unk_1B4[16];
    /* 0x01F4 */ f32 unk_1F4[16];
    /* 0x0234 */ f32 unk_234[16];
    /* 0x0274 */ u8 unk_274[16];
    /* 0x0284 */ ActorFunc drawFunc;
    /* 0x0288 */ Demo6KActionFunc actionFunc;
    /* 0x028C */ u16 flags;
    /* 0x028E */ u16 timer1;
    /* 0x0290 */ u16 timer2;
    /* 0x0292 */ u8 objBankIndex;
    /* 0x0293 */ u8 unk_293;
} Demo6K; // size = 0x0294

#endif

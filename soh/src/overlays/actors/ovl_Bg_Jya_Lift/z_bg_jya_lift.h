#ifndef Z_BG_JYA_LIFT_H
#define Z_BG_JYA_LIFT_H

#include "ultra64.h"
#include "global.h"

struct BgJyaLift;
typedef void (*BgJyaLiftActionFunc)(struct BgJyaLift*, PlayState*);

typedef struct BgJyaLift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x164 */ BgJyaLiftActionFunc actionFunc;
    /* 0x168 */ s16 moveDelay;
    /* 0x16A */ u8 isSpawned;
    /* 0x16B */ u8 unk_16B;
} BgJyaLift; // size = 0x016C

#endif

#ifndef Z_BG_HAKA_SHIP_H
#define Z_BG_HAKA_SHIP_H

#include "ultra64.h"
#include "global.h"

struct BgHakaShip;

typedef void (*BgHakaShipActionFunc)(struct BgHakaShip*, PlayState*);

typedef struct BgHakaShip {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHakaShipActionFunc actionFunc;
    /* 0x0168 */ u8 counter;
    /* 0x0169 */ u8 switchFlag;
    /* 0x016A */ s16 yOffset;
    /* 0x016C */ Vec3f bellSoundPos;
} BgHakaShip; // size = 0x0178

#endif

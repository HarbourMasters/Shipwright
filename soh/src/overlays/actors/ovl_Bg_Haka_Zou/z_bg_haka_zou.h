#ifndef Z_BG_HAKA_ZOU_H
#define Z_BG_HAKA_ZOU_H

#include "ultra64.h"
#include "global.h"

struct BgHakaZou;

typedef void (*BgHakaZouActionFunc)(struct BgHakaZou*, PlayState*);

typedef struct BgHakaZou {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHakaZouActionFunc actionFunc;
    /* 0x0168 */ s8 requiredObjBankIndex;
    /* 0x0169 */ u8 switchFlag;
    /* 0x016A */ s16 timer;
    /* 0x016C */ ColliderCylinder collider;
} BgHakaZou; // size = 0x01B8

#endif

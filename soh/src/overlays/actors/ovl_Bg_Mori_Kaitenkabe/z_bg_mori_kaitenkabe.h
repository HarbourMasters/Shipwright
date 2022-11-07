#ifndef Z_BG_MORI_KAITENKABE_H
#define Z_BG_MORI_KAITENKABE_H

#include "ultra64.h"
#include "global.h"

struct BgMoriKaitenkabe;

typedef void (*BgMoriKaitenkabeActionFunc)(struct BgMoriKaitenkabe*, PlayState*);

typedef struct BgMoriKaitenkabe {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgMoriKaitenkabeActionFunc actionFunc;
    /* 0x0168 */ s32 timer;
    /* 0x016C */ f32 rotDirection;
    /* 0x0170 */ f32 rotSpeed;
    /* 0x0174 */ f32 rotYdeg;
    /* 0x0178 */ Vec3f lockedPlayerPos;
    /* 0x0184 */ s8 moriTexObjIndex;
} BgMoriKaitenkabe; // size = 0x0188

#endif

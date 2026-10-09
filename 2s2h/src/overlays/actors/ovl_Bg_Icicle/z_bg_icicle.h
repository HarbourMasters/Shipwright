#ifndef Z_BG_ICICLE_H
#define Z_BG_ICICLE_H

#include "global.h"

struct BgIcicle;

typedef void (*BgIcicleActionFunc)(struct BgIcicle*, PlayState*);

typedef enum {
    /* 0 */ ICICLE_STALAGMITE_RANDOM_DROP,
    /* 1 */ ICICLE_STALACTITE,
    /* 2 */ ICICLE_STALACTITE_REGROW,
    /* 3 */ ICICLE_STALAGMITE_FIXED_DROP
} BgIcicleMainType;

typedef struct BgIcicle {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgIcicleActionFunc actionFunc;
    /* 0x160 */ u8 unk_160;
    /* 0x161 */ u8 unk_161;
    /* 0x162 */ s16 shiverTimer;
    /* 0x164 */ ColliderCylinder collider;
} BgIcicle; // size = 0x1B0

#endif // Z_BG_ICICLE_H

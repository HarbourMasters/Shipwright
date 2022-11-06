#ifndef Z_BG_HEAVY_BLOCK_H
#define Z_BG_HEAVY_BLOCK_H

#include "ultra64.h"
#include "global.h"

struct BgHeavyBlock;

typedef void (*BgHeavyBlockActionFunc)(struct BgHeavyBlock*, PlayState*);

typedef struct BgHeavyBlock {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ Vec3f unk_164;
    /* 0x0170 */ s16 timer;
    /* 0x0172 */ u16 pieceFlags;
    /* 0x0174 */ BgHeavyBlockActionFunc actionFunc;
} BgHeavyBlock; // size = 0x0178

typedef enum {
    /* 0x00 */ HEAVYBLOCK_UNBREAKABLE,
    /* 0x01 */ HEAVYBLOCK_BREAKABLE,
    /* 0x02 */ HEAVYBLOCK_BIG_PIECE,
    /* 0x03 */ HEAVYBLOCK_SMALL_PIECE,
    /* 0x04 */ HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE
} HeavyBlockType;

#endif

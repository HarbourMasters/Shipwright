#ifndef Z_BG_BOM_GUARD_H
#define Z_BG_BOM_GUARD_H

#include "ultra64.h"
#include "global.h"

struct BgBomGuard;

typedef void (*BgBomGuardActionFunc)(struct BgBomGuard*, PlayState*);

typedef struct BgBomGuard {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgBomGuardActionFunc actionFunc;
    /* 0x0168 */ u8 unk_168;
    /* 0x016C */ Vec3f unk_16C;
} BgBomGuard; // size = 0x0178

#endif

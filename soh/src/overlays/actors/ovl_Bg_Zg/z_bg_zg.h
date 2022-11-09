#ifndef Z_BG_ZG_H
#define Z_BG_ZG_H

#include "ultra64.h"
#include "global.h"

struct BgZg;

typedef void (*BgZgActionFunc)(struct BgZg*, PlayState*);
typedef void (*BgZgDrawFunc)(struct BgZg*, PlayState*);

typedef struct BgZg {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ s32 action;
    /* 0x0168 */ s32 drawConfig;
} BgZg; // size = 0x016C

#endif

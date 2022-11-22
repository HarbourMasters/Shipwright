#ifndef Z_BG_SPOT01_IDOHASHIRA_H
#define Z_BG_SPOT01_IDOHASHIRA_H

#include "ultra64.h"
#include "global.h"

struct BgSpot01Idohashira;

typedef void (*BgSpot01IdohashiraActionFunc)(struct BgSpot01Idohashira*, PlayState*);
typedef void (*BgSpot01IdohashiraDrawFunc)(struct BgSpot01Idohashira*, PlayState*);

typedef struct BgSpot01Idohashira {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ s32 action;
    /* 0x0168 */ s32 drawConfig;
    /* 0x016C */ u32 npcAction;
    /* 0x0170 */ s32 unk_170;
} BgSpot01Idohashira; // size = 0x0174

#endif

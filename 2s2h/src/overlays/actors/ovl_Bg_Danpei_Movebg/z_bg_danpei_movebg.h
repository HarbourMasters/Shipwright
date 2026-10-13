#ifndef Z_BG_DANPEI_MOVEBG_H
#define Z_BG_DANPEI_MOVEBG_H

#include "global.h"

struct BgDanpeiMovebg;

typedef void (*BgDanpeiMovebgActionFunc)(struct BgDanpeiMovebg*, PlayState*);

#define BGDANPEIMOVEBG_GET_PATH_INDEX(thisx) ((thisx)->params & 0x7F)
#define BGDANPEIMOVEBG_GET_TYPE(thisx)       (((thisx)->params >> 0xE) & 3)

#define DANPEI_MOVEBG_FLAG_1 (1 << 0)
#define DANPEI_MOVEBG_FLAG_2 (1 << 1)

typedef struct BgDanpeiMovebg {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgDanpeiMovebgActionFunc actionFunc;
    /* 0x160 */ ActorPathing actorPath;
    /* 0x1CC */ u16 flags;
    /* 0x1CE */ u16 prevFlags;
    /* 0x1D0 */ Gfx* dList;
    /* 0x1D4 */ s8 objectSlot;
} BgDanpeiMovebg; // size = 0x1D8

#endif // Z_BG_DANPEI_MOVEBG_H

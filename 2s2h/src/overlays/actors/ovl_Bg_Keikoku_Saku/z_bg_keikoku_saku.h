#ifndef Z_BG_KEIKOKU_SAKU_H
#define Z_BG_KEIKOKU_SAKU_H

#include "global.h"

#define BGKEIKOKUSAKU_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

struct BgKeikokuSaku;

typedef void (*BgKeikokuSakuActionFunc)(struct BgKeikokuSaku*, PlayState*);

typedef struct BgKeikokuSaku {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgKeikokuSakuActionFunc actionFunc;
    /* 0x160 */ s16 switchFlag;
    /* 0x162 */ s16 timer;
} BgKeikokuSaku; // Size = 0x164

#endif // Z_BG_KEIKOKU_SAKU_H

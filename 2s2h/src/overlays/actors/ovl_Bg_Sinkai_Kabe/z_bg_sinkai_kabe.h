#ifndef Z_BG_SINKAI_KABE_H
#define Z_BG_SINKAI_KABE_H

#include "global.h"
#include "overlays/actors/ovl_En_Dragon/z_en_dragon.h"

struct BgSinkaiKabe;

typedef void (*BgSinkaiKabeActionFunc)(struct BgSinkaiKabe*, PlayState*);

typedef struct BgSinkaiKabe {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgSinkaiKabeActionFunc actionFunc;
    /* 0x160 */ Actor* deepPython;
    /* 0x164 */ s16 csIdList[2];
    /* 0x168 */ UNK_TYPE1 unk_168[0x4];
    /* 0x16C */ s32 pythonIndex;
} BgSinkaiKabe; // size = 0x170

#endif // Z_BG_SINKAI_KABE_H

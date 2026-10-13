#ifndef Z_BG_FU_MIZU_H
#define Z_BG_FU_MIZU_H

#include "global.h"

struct BgFuMizu;

typedef struct BgFuMizu {
    /* 0x000 */ DynaPolyActor dyna;   
    /* 0x15C */ UNK_TYPE1 pad15C[0x4];    
    /* 0x160 */ s16 unk_160;
} BgFuMizu; // size = 0x164

#endif // Z_BG_FU_MIZU_H

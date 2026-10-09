#ifndef Z_BG_FU_KAITEN_H
#define Z_BG_FU_KAITEN_H

#include "global.h"

struct BgFuKaiten;

typedef struct BgFuKaiten {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ UNK_TYPE1 pad15C[0x4];
    /* 0x160 */ f32 elevation;
    /* 0x164 */ f32 bounceHeight;
    /* 0x168 */ s16 rotationSpeed;
    /* 0x16A */ s16 bounceSpeed;
    /* 0x16C */ s16 bounce;
} BgFuKaiten; // size = 0x170

#endif // Z_BG_FU_KAITEN_H

#ifndef Z_BG_IKNIN_SUSCEIL_H
#define Z_BG_IKNIN_SUSCEIL_H

#include "global.h"

#define SUSCEIL_GET_SWITCH_FLAG(thisx) (((thisx)->params) & 0x7F)

struct BgIkninSusceil;

typedef void (*BgIkninSusceilActionFunc)(struct BgIkninSusceil*, PlayState*);

typedef struct BgIkninSusceil {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ AnimatedMaterial* animatedTexture;
    /* 0x160 */ BgIkninSusceilActionFunc actionFunc;
    /* 0x164 */ s16 timer;
    /* 0x166 */ s16 unk166;
    /* 0x168 */ s16 unk168;
    /* 0x16A */ UNK_TYPE2 unk16A;
} BgIkninSusceil; // size = 0x16C

#endif // Z_BG_IKNIN_SUSCEIL_H

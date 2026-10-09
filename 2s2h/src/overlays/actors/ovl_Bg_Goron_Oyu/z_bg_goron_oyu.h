#ifndef Z_BG_GORON_OYU_H
#define Z_BG_GORON_OYU_H

#include "global.h"

struct BgGoronOyu;

typedef void (*BgGoronOyuActionFunc)(struct BgGoronOyu*, PlayState*);

typedef struct BgGoronOyu {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgGoronOyuActionFunc actionFunc;
    /* 0x160 */ UNK_TYPE unk_160;
    /* 0x164 */ f32 unk_164;
    /* 0x168 */ Vec3f waterBoxPos;
    /* 0x174 */ f32 waterBoxXLength;
    /* 0x178 */ f32 waterBoxZLength;
    /* 0x17C */ s16 initCsId;
    /* 0x17E */ u16 unk_17E;
} BgGoronOyu; // size = 0x180

#endif // Z_BG_GORON_OYU_H

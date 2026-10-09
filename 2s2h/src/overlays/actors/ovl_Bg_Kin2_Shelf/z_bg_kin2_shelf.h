#ifndef Z_BG_KIN2_SHELF_H
#define Z_BG_KIN2_SHELF_H

#include "global.h"

struct BgKin2Shelf;

typedef void (*BgKin2ShelfActionFunc)(struct BgKin2Shelf*, PlayState*);

#define BGKIN2SHELF_GET_1(thisx) ((thisx)->params & 1)

typedef struct BgKin2Shelf {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgKin2ShelfActionFunc actionFunc;
    /* 0x160 */ f32 unk_160;
    /* 0x164 */ u8 unk_164;
    /* 0x165 */ s8 unk_165;
    /* 0x166 */ s8 unk_166;
    /* 0x167 */ s8 unk_167;
} BgKin2Shelf; // size = 0x168

#endif // Z_BG_KIN2_SHELF_H

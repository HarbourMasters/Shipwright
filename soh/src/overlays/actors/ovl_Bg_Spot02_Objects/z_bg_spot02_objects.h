#ifndef Z_BG_SPOT02_OBJECTS_H
#define Z_BG_SPOT02_OBJECTS_H

#include <libultraship/libultra.h>
#include "global.h"

struct BgSpot02Objects;

typedef void (*BgSpot02ObjectsActionFunc)(struct BgSpot02Objects*, PlayState*);

typedef struct BgSpot02Objects {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot02ObjectsActionFunc actionFunc;
    /* 0x0168 */ s16 timer;
    /* 0x016A */ u8 unk_16A;
    /* 0x016B */ u8 unk_16B;
    /* 0x016C */ char unk_16C[4];
    /* 0x0170 */ u16 unk_170;
    /* 0x0172 */ u16 unk_172;
} BgSpot02Objects; // size = 0x0174

void func_808ACC34(BgSpot02Objects* bgSpot02Objects, PlayState* play);
void func_808AC908(BgSpot02Objects* bgSpot02Objects, PlayState* play);

#endif

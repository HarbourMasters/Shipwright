#ifndef Z_BG_SPOT02_OBJECTS_H
#define Z_BG_SPOT02_OBJECTS_H

#include "ultra64.h"
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

#endif

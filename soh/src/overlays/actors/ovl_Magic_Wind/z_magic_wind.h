#ifndef Z_MAGIC_WIND_H
#define Z_MAGIC_WIND_H

#include "ultra64.h"
#include "global.h"

struct MagicWind;

typedef void (*MagicWindFunc)(struct MagicWind* this, PlayState* play);

typedef struct MagicWind {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnimeCurve skelCurve;
    /* 0x016C */ s16 timer;
    /* 0x0170 */ MagicWindFunc actionFunc;
} MagicWind; // size = 0x0174

#endif

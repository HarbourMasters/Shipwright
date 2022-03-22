#ifndef Z_BG_VB_SIMA_H
#define Z_BG_VB_SIMA_H

#include "ultra64.h"
#include "global.h"

struct BgVbSima;

typedef enum {
    /* 0 */ VBSIMA_STAND,
    /* 1 */ VBSIMA_COLLAPSE,
    /* 2 */ VBSIMA_KILL
} BgVbSimaSignal;

typedef struct BgVbSima {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ char unk_164[0x10];
    /* 0x0174 */ s16 shakeTimer;
    /* 0x0176 */ char unk_176[6];
} BgVbSima; // size = 0x017C

#endif

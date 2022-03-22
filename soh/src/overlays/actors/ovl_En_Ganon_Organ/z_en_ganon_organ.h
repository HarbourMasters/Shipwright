#ifndef Z_EN_GANON_ORGAN_H
#define Z_EN_GANON_ORGAN_H

#include "ultra64.h"
#include "global.h"

struct EnGanonOrgan;

typedef struct EnGanonOrgan {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[0x4];
} EnGanonOrgan; // size = 0x0150

#endif

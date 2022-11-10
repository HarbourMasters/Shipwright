#ifndef Z_EN_OE2_H
#define Z_EN_OE2_H

#include "ultra64.h"
#include "global.h"

struct EnOE2;

typedef void (*EnOE2ActionFunc)(struct EnOE2*, PlayState*);

typedef struct EnOE2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[0x44];
    /* 0x0190 */ EnOE2ActionFunc actionFunc;
} EnOE2; // size = 0x0194

#endif

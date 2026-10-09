#ifndef Z_EFF_STK_H
#define Z_EFF_STK_H

#include "global.h"

struct EffStk;

typedef void (*EffStkActionFunc)(struct EffStk*, PlayState*);

typedef struct EffStk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 unk144;
    /* 0x146 */ s16 unk146;
    /* 0x148 */ f32 unk148;
    /* 0x14C */ EffStkActionFunc actionFunc;
} EffStk; // size = 0x150

#endif // Z_EFF_STK_H

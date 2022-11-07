#ifndef Z_EN_EX_RUPPY_H
#define Z_EN_EX_RUPPY_H

#include "ultra64.h"
#include "global.h"

struct EnExRuppy;

typedef void (*EnExRuppyActionFunc)(struct EnExRuppy*, PlayState*);

typedef struct EnExRuppy {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnExRuppyActionFunc actionFunc;
    /* 0x0150 */ s16 colorIdx;
    /* 0x0152 */ s16 type;
    /* 0x0154 */ s16 invisible;
    /* 0x0156 */ s16 timer;
    /* 0x0158 */ s16 rupeeValue;
    /* 0x015A */ s16 unk_15A; 
    /* 0x015C */ s16 galleryFlag;
    /* 0x0160 */ f32 unk_160;
} EnExRuppy; // size = 0x0164

#endif

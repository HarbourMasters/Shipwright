#ifndef Z_EN_PART_H
#define Z_EN_PART_H

#include "ultra64.h"
#include "global.h"

struct EnPart;

typedef void (*EnPartActionFunc)(struct EnPart*, PlayState*);

typedef struct EnPart {
    /* 0x000 */ Actor actor;
    /* 0x14C */ u8 action;
    /* 0x14E */ s16 timer;
    /* 0x150 */ Gfx* displayList;
    /* 0x154 */ f32 rotZ;
    /* 0x158 */ f32 rotZSpeed;
} EnPart; // size = 0x015C

#endif

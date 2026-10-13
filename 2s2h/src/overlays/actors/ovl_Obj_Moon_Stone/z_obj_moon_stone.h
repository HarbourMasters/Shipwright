#ifndef Z_OBJ_MOON_STONE_H
#define Z_OBJ_MOON_STONE_H

#include "global.h"

struct ObjMoonStone;

typedef void (*ObjMoonStoneActionFunc)(struct ObjMoonStone*, PlayState*);

typedef struct ObjMoonStone {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x4C];
    /* 0x190 */ ObjMoonStoneActionFunc actionFunc;
    /* 0x194 */ s16 unk194;
} ObjMoonStone; // size = 0x198

#endif // Z_OBJ_MOON_STONE_H

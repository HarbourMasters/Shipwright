#ifndef Z_OBJ_GHAKA_H
#define Z_OBJ_GHAKA_H

#include "global.h"

struct ObjGhaka;

typedef void (*ObjGhakaActionFunc)(struct ObjGhaka*, PlayState*);

typedef struct ObjGhaka {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjGhakaActionFunc actionFunc;
    /* 0x160 */ UNK_TYPE1 unk160[0x8];
    /* 0x168 */ s16 unk_168;
} ObjGhaka; // size = 0x16C

#endif // Z_OBJ_GHAKA_H

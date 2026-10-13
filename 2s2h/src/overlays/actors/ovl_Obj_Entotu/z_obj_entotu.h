#ifndef Z_OBJ_ENTOTU_H
#define Z_OBJ_ENTOTU_H

#include "global.h"

struct ObjEntotu;

typedef struct ObjEntotu {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk144[4];
    /* 0x148 */ Vtx unk_148[7];
    /* 0x1B8 */ Vec3f unk_1B8;
    /* 0x1C4 */ u8 unk_1C4;
    /* 0x1C6 */ s16 unk_1C6;
} ObjEntotu; // size = 0x1C8

#endif // Z_OBJ_ENTOTU_H

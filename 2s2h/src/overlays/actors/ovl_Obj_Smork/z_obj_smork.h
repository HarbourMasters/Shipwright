#ifndef Z_OBJ_SMORK_H
#define Z_OBJ_SMORK_H

#include "global.h"

struct ObjSmork;

typedef struct ObjSmork {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[4];
    /* 0x148 */ Vtx unk_148[7];
    /* 0x1B8 */ f32 unk_1B8;
    /* 0x1BC */ f32 unk_1BC;
    /* 0x1C0 */ f32 unk_1C0;
    /* 0x1C4 */ u8 unk_1C4;
    /* 0x1C6 */ s16 unk_1C6;
} ObjSmork; // size = 0x1C8

#endif // Z_OBJ_SMORK_H

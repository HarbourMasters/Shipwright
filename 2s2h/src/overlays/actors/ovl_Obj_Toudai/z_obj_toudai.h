#ifndef Z_OBJ_TOUDAI_H
#define Z_OBJ_TOUDAI_H

#include "global.h"

struct ObjToudai;

typedef struct ObjToudai {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 pad144[4];
    /* 0x148 */ Vtx unk_148[14];
    /* 0x228 */ f32 unk_228;
    /* 0x22C */ f32 unk_22C;
    /* 0x230 */ f32 unk_230;
    /* 0x234 */ s16 unk_234;
    /* 0x236 */ u8 unk_236;
    /* 0x238 */ s16 unk_238;
    /* 0x23A */ UNK_TYPE1 pad23A[6];
} ObjToudai;// size = 0x240

#endif // Z_OBJ_TOUDAI_H

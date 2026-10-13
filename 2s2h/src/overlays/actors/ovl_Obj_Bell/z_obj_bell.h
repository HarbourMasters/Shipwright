#ifndef Z_OBJ_BELL_H
#define Z_OBJ_BELL_H

#include "global.h"

struct ObjBell;

typedef struct ObjBell {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderSphere collider1;
    /* 0x1B4 */ ColliderSphere collider2;
    /* 0x20C */ s16 unk_20C; // bell rotation angle?
    /* 0x20E */ s16 unk_20E;
    /* 0x210 */ s16 unk_210;
    /* 0x212 */ s16 unk_212;
    /* 0x214 */ s16 unk_214;
    /* 0x216 */ UNK_TYPE1 unk_216[0x06];
    /* 0x21C */ f32 unk_21C; // bell speed?
    /* 0x220 */ f32 unk_220;
    /* 0x224 */ UNK_TYPE1 unk_224[0x04];
} ObjBell; // size = 0x228

#endif // Z_OBJ_BELL_H

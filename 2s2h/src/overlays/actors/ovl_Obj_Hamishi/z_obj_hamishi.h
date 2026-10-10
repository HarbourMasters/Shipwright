#ifndef Z_OBJ_HAMISHI_H
#define Z_OBJ_HAMISHI_H

#include "global.h"

struct ObjHamishi;

#define OBJHAMISHI_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

typedef struct ObjHamishi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ f32 unk_190;
    /* 0x194 */ f32 unk_194;
    /* 0x198 */ s16 unk_198;
    /* 0x19A */ s16 unk_19A;
    /* 0x19C */ s16 unk_19C;
    /* 0x19E */ s16 unk_19E;
    /* 0x1A0 */ s8 unk_1A0;
    /* 0x1A1 */ s8 unk_1A1;
    /* 0x1A2 */ u8 unk_1A2;
} ObjHamishi; // size = 0x1A4

#endif // Z_OBJ_HAMISHI_H

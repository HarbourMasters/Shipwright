#ifndef Z_OBJ_SPIDERTENT_H
#define Z_OBJ_SPIDERTENT_H

#include "global.h"

struct ObjSpidertent;

typedef void (*ObjSpidertentActionFunc)(struct ObjSpidertent*, PlayState*);

#define OBJSPIDERTENT_GET_1(thisx) ((thisx)->params & 1)
#define OBJSPIDERTENT_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)

typedef struct ObjSpidertent {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderTris collider;
    /* 0x17C */ ColliderTrisElement colliderElements[6];
    /* 0x3A4 */ Vec3f unk_3A4;
    /* 0x3B0 */ f32 unk_3B0[4];
    /* 0x3C0 */ u8 unk_3C0;
    /* 0x3C1 */ s8 unk_3C1;
    /* 0x3C2 */ u8 unk_3C2;
    /* 0x3C3 */ u8 unk_3C3;
    /* 0x3C4 */ u8 unk_3C4;
    /* 0x3C5 */ u8 unk_3C5;
    /* 0x3C6 */ s8 unk_3C6;
    /* 0x3C7 */ s8 unk_3C7;
    /* 0x3C8 */ ObjSpidertentActionFunc actionFunc;
} ObjSpidertent; // size = 0x3CC

#endif // Z_OBJ_SPIDERTENT_H

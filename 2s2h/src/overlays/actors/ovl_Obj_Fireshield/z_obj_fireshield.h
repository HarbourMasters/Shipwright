#ifndef Z_OBJ_FIRESHIELD_H
#define Z_OBJ_FIRESHIELD_H

#include "global.h"

struct ObjFireshield;

typedef void (*ObjFireshieldActionFunc)(struct ObjFireshield*);

#define OBJFIRESHIELD_GET_FLAGS(thisx) ((thisx)->params & 0x7F)
#define OBJFIRESHIELD_GET_1F00(thisx) (((thisx)->params >> 8) & 0x1F)
#define OBJFIRESHIELD_GET_2000(thisx) (((thisx)->params >> 0xD) & 1)
#define OBJFIRESHIELD_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 7) & 0x7F)
#define OBJFIRESHIELD_GET_C000(thisx) (((thisx)->params >> 0xE) & 3)
#define OBJFIRESHIELD_GET_ROTX(thisx) ((thisx)->home.rot.x & 1)

typedef struct ObjFireshield {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ ObjFireshieldActionFunc actionFunc;
    /* 0x194 */ s32 unk_194;
    /* 0x198 */ f32 unk_198;
    /* 0x19C */ s32 unk_19C;
    /* 0x1A0 */ s32 unk_1A0;
    /* 0x1A4 */ s16 unk_1A4;
    /* 0x1A6 */ u8 unk_1A6;
    /* 0x1A7 */ s8 unk_1A7;
    /* 0x1A8 */ s8 unk_1A8;
} ObjFireshield; // size = 0x1AC

#endif // Z_OBJ_FIRESHIELD_H

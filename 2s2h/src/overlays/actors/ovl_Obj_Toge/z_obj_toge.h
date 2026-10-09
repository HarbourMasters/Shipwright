#ifndef Z_OBJ_TOGE_H
#define Z_OBJ_TOGE_H

#include "global.h"

struct ObjToge;

typedef void (*ObjTogeActionFunc)(struct ObjToge*, PlayState*);

#define OBJTOGE_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)
#define OBJTOGE_GET_700(thisx) (((thisx)->params >> 8) & 7)
#define OBJTOGE_GET_3800(thisx) (((thisx)->params >> 0xB) & 7)
#define OBJTOGE_GET_4000(thisx) (((thisx)->params >> 0xE) & 1)

#define OBJTOGE_PATH_INDEX_NONE 0xFF

typedef struct ObjToge {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ ObjTogeActionFunc actionFunc;
    /* 0x194 */ s32 unk_194;
    /* 0x198 */ Vec3f unk_198[2];
    /* 0x1B0 */ s16 unk_1B0;
    /* 0x1B2 */ s16 unk_1B2;
    /* 0x1B4 */ u8 unk_1B4;
    /* 0x1B8 */ f32 unk_1B8;
    /* 0x1BC */ f32 unk_1BC;
    /* 0x1C0 */ f32 unk_1C0;
    /* 0x1C4 */ f32 unk_1C4;
    /* 0x1C8 */ f32 unk_1C8;
    /* 0x1CC */ f32 unk_1CC;
} ObjToge; // size = 0x1D0

#endif // Z_OBJ_TOGE_H

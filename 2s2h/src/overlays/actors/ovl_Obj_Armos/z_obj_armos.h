#ifndef Z_OBJ_ARMOS_H
#define Z_OBJ_ARMOS_H

#include "global.h"
#include "objects/object_am/object_am.h"

struct ObjArmos;

typedef void (*ObjArmosActionFunc)(struct ObjArmos*, PlayState*);

#define OBJARMOS_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define OBJARMOS_GET_ROTZ_7(thisx) ((thisx)->home.rot.z & 7)
#define OBJARMOS_GET_ROTX_F(thisx) ((thisx)->home.rot.x & 0xF)

typedef enum {
    /* 0 */ OBJARMOS_ROT_7_0,
    /* 1 */ OBJARMOS_ROT_7_1,
    /* 2 */ OBJARMOS_ROT_7_2,
    /* 3 */ OBJARMOS_ROT_7_3,
    /* 4 */ OBJARMOS_ROT_7_4,
    /* 5 */ OBJARMOS_ROT_7_5,
    /* 6 */ OBJARMOS_ROT_7_6
} ObjArmosParam;

typedef struct ObjArmos {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ Vec3s jointTable[OBJECT_AM_LIMB_MAX];
    /* 0x1F4 */ Vec3s morphTable[OBJECT_AM_LIMB_MAX];
    /* 0x248 */ ObjArmosActionFunc actionFunc;
    /* 0x24C */ u32 updBgCheckInfoFlags;
    /* 0x250 */ Vec3f unk_250;
    /* 0x25C */ f32* unk_25C;
    /* 0x260 */ f32 unk_260;
    /* 0x264 */ s16 unk_264;
    /* 0x266 */ s16 unk_266[4];
    /* 0x26E */ s16 unk_26E;
    /* 0x270 */ s16 unk_270;
} ObjArmos; // size = 0x274

#endif // Z_OBJ_ARMOS_H

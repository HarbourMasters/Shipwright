#ifndef Z_EN_JS_H
#define Z_EN_JS_H

#include "global.h"
#include "objects/object_ob/object_ob.h"

#define ENJS_GET_TYPE(thisx) ((thisx)->params & 0xF)
#define ENJS_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 10)
#define ENJS_GET_EXIT_INDEX(thisx)  (((thisx)->params & 0x3F0) >> 4)

#define ENJS_PATH_INDEX_NONE 0x3F

struct EnJs;

typedef void (*EnJsActionFunc)(struct EnJs*, PlayState*);

typedef struct EnJs {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ ColliderCylinder collider;
    /* 0x1D4 */ Vec3s jointTable[MOONCHILD_LIMB_MAX];
    /* 0x240 */ Vec3s morphTable[MOONCHILD_LIMB_MAX];
    /* 0x2AC */ Path* path;
    /* 0x2B0 */ s32 unk_2B0;
    /* 0x2B4 */ f32 unk_2B4;
    /* 0x2B8 */ u16 unk_2B8;
    /* 0x2BA */ s16 maskType;
    /* 0x2BC */ s16 unk_2BC;
    /* 0x2BE */ s16 csIdList[2];
    /* 0x2C2 */ s16 csIdIndex;
    /* 0x2C4 */ EnJsActionFunc actionFunc;
} EnJs; // size = 0x2C8

#endif // Z_EN_JS_H

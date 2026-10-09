#ifndef Z_EN_SCOPECROW_H
#define Z_EN_SCOPECROW_H

#include "global.h"
#include "objects/object_crow/object_crow.h"

struct EnScopecrow;

typedef void (*EnScopecrowActionFunc)(struct EnScopecrow*, PlayState*);

#define ENSCOPECROW_GET_1F(thisx) ((thisx)->params & 0x1F)
#define ENSCOPECROW_GET_3E0(thisx) (((thisx)->params & 0x3E0) >> 5)
#define ENSCOPECROW_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 0xA)

#define ENSCOPECROW_PATH_INDEX_NONE 0x3F

typedef struct EnScopecrow {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnScopecrowActionFunc actionFunc;
    /* 0x18C */ Vec3s jointTable[OBJECT_CROW_LIMB_MAX];
    /* 0x1C2 */ Vec3s morphTable[OBJECT_CROW_LIMB_MAX];
    /* 0x1F8 */ Path* path;
    /* 0x1FC */ s32 unk_1FC;
    /* 0x200 */ ColliderJntSph collider;
    /* 0x220 */ ColliderJntSphElement colliderElements[1];
    /* 0x260 */ s16 unk_260;
    /* 0x262 */ s16 unk_262;
    /* 0x264 */ s16 unk_264;
} EnScopecrow; // size = 0x268

#endif // Z_EN_SCOPECROW_H

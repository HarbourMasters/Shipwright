#ifndef Z_EN_AKINDONUTS_H
#define Z_EN_AKINDONUTS_H

#include "global.h"
#include "objects/object_dnt/object_dnt.h"

struct EnAkindonuts;

typedef void (*EnAkindonutsActionFunc)(struct EnAkindonuts*, PlayState*);
typedef void (*EnAkindonutsUnkFunc)(struct EnAkindonuts*, PlayState*);

#define ENAKINDONUTS_GET_3(thisx) ((thisx)->params & 3)
#define ENAKINDONUTS_GET_4(thisx) (((thisx)->params & 4) >> 2)
#define ENAKINDONUTS_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 0xA)

#define ENAKINDONUTS_PATH_INDEX_NONE 0x3F

typedef enum {
    /* 1 */ ENAKINDONUTS_3_1 = 1,
    /* 2 */ ENAKINDONUTS_3_2
} EnAkindonutsParam;

typedef struct EnAkindonuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BUSINESS_SCRUB_LIMB_MAX];
    /* 0x230 */ Vec3s morphTable[BUSINESS_SCRUB_LIMB_MAX];
    /* 0x2D8 */ EnAkindonutsActionFunc actionFunc;
    /* 0x2DC */ EnAkindonutsUnkFunc unk_2DC;
    /* 0x2E0 */ ColliderCylinder collider;
    /* 0x32C */ u16 unk_32C;
    /* 0x330 */ Path* path;
    /* 0x334 */ s32 unk_334;
    /* 0x338 */ s16 animIndex;
    /* 0x33A */ s16 unk_33A;
    /* 0x33C */ u16 unk_33C;
    /* 0x33E */ s16 unk_33E;
    /* 0x340 */ f32 unk_340;
    /* 0x344 */ f32 unk_344;
    /* 0x348 */ f32 unk_348;
    /* 0x34C */ f32 unk_34C;
    /* 0x350 */ s16 unk_350;
    /* 0x352 */ s16 unk_352;
    /* 0x354 */ UNK_TYPE1 unk354[0x2];
    /* 0x356 */ s16 unk_356;
    /* 0x358 */ f32 unk_358;
    /* 0x35C */ s16 unk_35C;
    /* 0x35E */ s16 unk_35E;
    /* 0x360 */ s16 csId;
    /* 0x362 */ s16 unk_362;
    /* 0x364 */ s16 unk_364;
    /* 0x366 */ s8 unk_366;
    /* 0x368 */ s16 unk_368;
} EnAkindonuts; // size = 0x36C

#endif // Z_EN_AKINDONUTS_H

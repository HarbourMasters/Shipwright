#ifndef Z_EN_SCOPENUTS_H
#define Z_EN_SCOPENUTS_H

#include "global.h"
#include "objects/object_dnt/object_dnt.h"

struct EnScopenuts;

typedef void (*EnScopenutsActionFunc)(struct EnScopenuts*, PlayState*);

#define ENSCOPENUTS_GET_3E0(thisx) (((thisx)->params & 0x3E0) >> 5)
#define ENSCOPENUTS_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 0xA)

#define ENSCOPENUTS_PATH_INDEX_NONE 0x3F

#define ENSCOPENUTS_3E0_0 0
#define ENSCOPENUTS_3E0_1 1

typedef struct EnScopenuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BUSINESS_SCRUB_LIMB_MAX];
    /* 0x230 */ Vec3s morphTable[BUSINESS_SCRUB_LIMB_MAX];
    /* 0x2D8 */ EnScopenutsActionFunc actionFunc;
    /* 0x2DC */ ColliderCylinder collider;
    /* 0x328 */ u16 unk_328;
    /* 0x32C */ f32 unk_32C;
    /* 0x330 */ Path* path;
    /* 0x334 */ s32 unk_334;
    /* 0x338 */ s16 csId;
    /* 0x33A */ UNK_TYPE1 unk33A[2];
    /* 0x33C */ u16 unk_33C;
    /* 0x33E */ s16 unk_33E;
    /* 0x340 */ s16 unk_340;
    /* 0x342 */ UNK_TYPE1 unk342[0x6];
    /* 0x348 */ s32 animIndex;
    /* 0x34C */ s16 unk_34C;
    /* 0x34E */ s16 unk_34E;
    /* 0x350 */ f32 unk_350;
    /* 0x354 */ UNK_TYPE1 unk354[4];
    /* 0x358 */ s16 unk_358;
    /* 0x35A */ s16 unk_35A;
    /* 0x35C */ f32 unk_35C;
    /* 0x360 */ f32 unk_360;
    /* 0x364 */ f32 unk_364;
    /* 0x368 */ f32 unk_368;
    /* 0x36C */ s16 unk_36C;
    /* 0x36E */ s16 unk_36E;
    /* 0x370 */ s16 unk_370;
    /* 0x372 */ s16 unk_372;
} EnScopenuts; // size = 0x374

#endif // Z_EN_SCOPENUTS_H

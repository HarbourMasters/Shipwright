#ifndef Z_EN_MUSHI2_H
#define Z_EN_MUSHI2_H

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

struct EnMushi2;

typedef void (*EnMushi2ActionFunc)(struct EnMushi2*, PlayState*);

#define ENMUSHI2_GET_3(thisx) ((thisx)->params & 3)

#define ENMUSHI2_PARAMS(param) ((param) & 3)

typedef enum {
    /* 0 */ ENMUSHI2_0,
    /* 1 */ ENMUSHI2_1,
    /* 2 */ ENMUSHI2_2
} EnMush2Param;

typedef struct {
    /* 0x0 */ s16 unk_00;
    /* 0x2 */ s16 unk_02;
} EnMushi2Struct; // size = 0x4

typedef struct EnMushi2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BUG_LIMB_MAX];
    /* 0x218 */ Vec3s morphTable[BUG_LIMB_MAX];
    /* 0x2A8 */ ColliderJntSph collider;
    /* 0x2C8 */ ColliderJntSphElement colliderElements[1];
    /* 0x308 */ EnMushi2ActionFunc actionFunc;
    /* 0x30C */ s32 unk_30C;
    /* 0x310 */ Vec3f unk_310;
    /* 0x31C */ Vec3f unk_31C;
    /* 0x328 */ Vec3f unk_328;
    /* 0x334 */ CollisionPoly* poly;
    /* 0x338 */ s32 polyBgId;
    /* 0x33C */ Vec3f unk_33C;
    /* 0x348 */ f32 unk_348;
    /* 0x34C */ struct ObjBean* unk_34C;
    /* 0x350 */ f32 unk_350;
    /* 0x354 */ f32 unk_354;
    /* 0x358 */ f32 unk_358;
    /* 0x35C */ f32 unk_35C;
    /* 0x360 */ f32 unk_360;
    /* 0x364 */ s16 unk_364;
    /* 0x366 */ s16 unk_366;
    /* 0x368 */ s16 unk_368;
    /* 0x36A */ s16 unk_36A;
    /* 0x36C */ s16 unk_36C;
    /* 0x36E */ s16 unk_36E;
    /* 0x370 */ s16 unk_370;
} EnMushi2; // size = 0x374

#endif // Z_EN_MUSHI2_H

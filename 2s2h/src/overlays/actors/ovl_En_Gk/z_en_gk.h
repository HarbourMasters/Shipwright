#ifndef Z_EN_GK_H
#define Z_EN_GK_H

#include "global.h"
#include "objects/object_gk/object_gk.h"

struct EnGk;

typedef void (*EnGkActionFunc)(struct EnGk*, PlayState*);

#define ENGK_GET_F(thisx) ((thisx)->params & 0xF)
#define ENGK_GET_PATH_INDEX(thisx) (((thisx)->params & 0xF0) >> 4)
#define ENGK_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0x3F00) >> 8)

#define ENGK_PATH_INDEX_NONE 0xF

typedef enum {
    /* 0 */ ENGK_F_0,
    /* 1 */ ENGK_F_1,
    /* 2 */ ENGK_F_2,
    /* 3 */ ENGK_F_3,
    /* 4 */ ENGK_F_4,
    /* 5 */ ENGK_F_5
} EnGkParam;

typedef struct EnGk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnGkActionFunc actionFunc;
    /* 0x1D8 */ Vec3s headRot;
    /* 0x1DE */ Vec3s torsoRot;
    /* 0x1E4 */ u16 unk_1E4;
    /* 0x1E8 */ Path* path;
    /* 0x1EC */ s32 unk_1EC;
    /* 0x1F0 */ Vec3s jointTable[OBJECT_GK_LIMB_MAX];
    /* 0x1F0 */ Vec3s morphTable[OBJECT_GK_LIMB_MAX];
    /* 0x2E0 */ s16 unk_2E0;
    /* 0x2E2 */ s16 unk_2E2;
    /* 0x2E4 */ s16 animIndex;
    /* 0x2E8 */ Vec3f unk_2E8;
    /* 0x2F4 */ Vec3f unk_2F4;
    /* 0x300 */ Vec3f unk_300;
    /* 0x30C */ Vec3f unk_30C;
    /* 0x318 */ s16 csId;
    /* 0x31A */ u8 csAnimIndex;
    /* 0x31B */ u8 cueId;
    /* 0x31C */ u16 unk_31C;
    /* 0x31E */ s16 unk_31E;
    /* 0x320 */ s16 unk_320;
    /* 0x322 */ s16 unk_322;
    /* 0x324 */ s16 unk_324;
    /* 0x328 */ Vec3f unk_328;
    /* 0x334 */ Vec3s unk_334;
    /* 0x33C */ Vec3f unk_33C;
    /* 0x348 */ Vec3s unk_348;
    /* 0x34E */ s16 unk_34E;
    /* 0x350 */ s16 unk_350;
    /* 0x354 */ f32 unk_354;
} EnGk; // size = 0x358

#endif // Z_EN_GK_H

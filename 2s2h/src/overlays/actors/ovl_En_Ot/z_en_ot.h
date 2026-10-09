#ifndef Z_EN_OT_H
#define Z_EN_OT_H

#include "global.h"
#include "objects/object_ot/object_ot.h"

struct EnOt;

typedef void (*EnOtActionFunc)(struct EnOt*, PlayState*);

#define SEAHORSE_GET_PATH_INDEX(thisx) ((thisx)->params & 0x7F)
#define SEAHORSE_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 7) & 0x7F)
#define SEAHORSE_GET_TYPE(thisx) (((thisx)->params >> 0xE) & 3)

#define SEAHORSE_PARAMS(type, pathIndex, switchFlag) (((pathIndex) & 0x7F) | (((switchFlag) & 0x7F) << 7) | (((type) & 3) << 0xE))
#define SEAHORSE_PARAMS_PARTNER(thisx, type) (((thisx)->params & 0x3FFF) | (((type) & 3) << 0xE))

typedef enum SeahorseType {
    /* 0 */ SEAHORSE_TYPE_0,
    /* 1 */ SEAHORSE_TYPE_1,
    /* 2 */ SEAHORSE_TYPE_2,
    /* 3 */ SEAHORSE_TYPE_3
} SeahorseType;

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ UNK_TYPE1 unk_08[0x4];
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ UNK_TYPE1 unk_18[0x18];
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ f32 unk_3C;
    /* 0x40 */ f32 unk_40;
    /* 0x44 */ UNK_TYPE1 unk_44[0x8];
    /* 0x4C */ u8 unk_4C;
    /* 0x50 */ Vec3f unk_50;
} EnOtUnkStruct; // size = 0x5C

typedef struct EnOt {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnOtActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ Vec3s jointTable[OBJECT_OT_LIMB_MAX];
    /* 0x24A */ Vec3s morphTable[OBJECT_OT_LIMB_MAX];
    /* 0x2BC */ s32 animIndex;
    /* 0x2C0 */ ActorPathing actorPath;
    /* 0x32C */ u16 unk_32C;
    /* 0x330 */ Vec3f unk_330;
    /* 0x33C */ s32 type;
    /* 0x340 */ s32 unk_340;
    /* 0x344 */ s16 unk_344;
    /* 0x346 */ s16 pathIndex;
    /* 0x348 */ Vec3f unk_348;
    /* 0x354 */ s16 unk_354;
    /* 0x356 */ s16 csIdList[4];
    /* 0x360 */ struct EnOt* unk_360;
    /* 0x364 */ LightNode* lightNode;
    /* 0x368 */ LightInfo lightInfo;
    /* 0x378 */ Vec3f unk_378;
    /* 0x384 */ u8 unk_384;
    /* 0x388 */ s32 unk_388;
    /* 0x38C */ s32 getItemId;
    /* 0x390 */ s16 unk_390;
    /* 0x394 */ Vec3f unk_394;
    /* 0x3A0 */ s16 unk_3A0;
    /* 0x3A4 */ EnOtUnkStruct unk_3A4[10];
    /* 0x73C */ s16 unk_73C;
    /* 0x740 */ f32 unk_740;
    /* 0x744 */ Color_RGB8 unk_744;
    /* 0x747 */ Color_RGB8 unk_747;
    /* 0x74C */ Vec3f unk_74C;
} EnOt; // size = 0x758

#endif // Z_EN_OT_H

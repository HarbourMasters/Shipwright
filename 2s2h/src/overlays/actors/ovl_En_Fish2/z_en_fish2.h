#ifndef Z_EN_FISH2_H
#define Z_EN_FISH2_H

#include "global.h"
#include "objects/object_fb/object_fb.h"

struct EnFish2;

typedef void (*EnFish2ActionFunc)(struct EnFish2*, PlayState*);

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ s16 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ s16 unk_1C;
    /* 0x20 */ TexturePtr unk_20;
} EnFish2UnkStruct; // size = 0x24

typedef struct EnFish2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_FB_LIMB_MAX];
    /* 0x218 */ Vec3s morphTable[OBJECT_FB_LIMB_MAX];
    /* 0x2A8 */ EnFish2ActionFunc actionFunc;
    /* 0x2AC */ s32 animIndex;
    /* 0x2B0 */ s32 unk_2B0;
    /* 0x2B4 */ s16 unk_2B4;
    /* 0x2B6 */ s16 unk_2B6;
    /* 0x2B8 */ s16 unk_2B8;
    /* 0x2BA */ s16 csIdList[3];
    /* 0x2C0 */ s32 unk_2C0;
    /* 0x2C4 */ s32 unk_2C4;
    /* 0x2C8 */ u8 unk_2C8;
    /* 0x2CA */ s16 subCamId;
    /* 0x2CC */ f32 animEndFrame;
    /* 0x2D0 */ UNK_TYPE1 unk_2D0[0x4];
    /* 0x2D4 */ f32 unk_2D4;
    /* 0x2D8 */ f32 unk_2D8;
    /* 0x2DC */ Vec3f subCamEye;
    /* 0x2E8 */ Vec3f subCamAt;
    /* 0x2F4 */ Vec3f unk_2F4;
    /* 0x300 */ Vec3f unk_300;
    /* 0x30C */ Vec3f unk_30C;
    /* 0x318 */ Vec3f unk_318;
    /* 0x324 */ Vec3f unk_324;
    /* 0x330 */ f32 unk_330;
    /* 0x334 */ f32 unk_334;
    /* 0x338 */ f32 unk_338;
    /* 0x33C */ f32 unk_33C;
    /* 0x340 */ s32 unk_340;
    /* 0x344 */ s32 unk_344;
    /* 0x348 */ s16 unk_348;
    /* 0x34A */ s16 unk_34A;
    /* 0x34C */ s16 unk_34C;
    /* 0x350 */ Actor* unk_350;
    /* 0x354 */ struct EnFish2* unk_354;
    /* 0x358 */ ColliderJntSph collider;
    /* 0x378 */ ColliderJntSphElement colliderElements[2];
    /* 0x3F8 */ EnFish2UnkStruct unk_3F8[200];
} EnFish2; // size = 0x2018

#endif // Z_EN_FISH2_H

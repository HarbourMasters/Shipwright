#ifndef Z64ENHY_CODE_H
#define Z64ENHY_CODE_H

#include "global.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"

struct EnHy;

//! @note: any actor that uses the EnHy system should 
//! have a skeleton that aligns with the enum below.
typedef enum EnHyLimb {
    /* 0x00 */ ENHY_LIMB_NONE,
    /* 0x01 */ ENHY_LIMB_PELVIS,
    /* 0x02 */ ENHY_LIMB_LEFT_THIGH,
    /* 0x03 */ ENHY_LIMB_LEFT_SHIN,
    /* 0x04 */ ENHY_LIMB_LEFT_FOOT,
    /* 0x05 */ ENHY_LIMB_RIGHT_THIGH,
    /* 0x06 */ ENHY_LIMB_RIGHT_SHIN,
    /* 0x07 */ ENHY_LIMB_RIGHT_FOOT,
    /* 0x08 */ ENHY_LIMB_TORSO,
    /* 0x09 */ ENHY_LIMB_LEFT_UPPER_ARM,
    /* 0x0A */ ENHY_LIMB_LEFT_FOREARM,
    /* 0x0B */ ENHY_LIMB_LEFT_HAND,
    /* 0x0C */ ENHY_LIMB_RIGHT_UPPER_ARM,
    /* 0x0D */ ENHY_LIMB_RIGHT_FOREARM,
    /* 0x0E */ ENHY_LIMB_RIGHT_HAND,
    /* 0x0F */ ENHY_LIMB_HEAD,
    /* 0x10 */ ENHY_LIMB_MAX
} EnHyLimb;

//! TODO: Better animaion enum names when animations are documented
typedef enum {
    /* -1 */ ENHY_ANIM_NONE = -1,
    /*  0 */ ENHY_ANIM_AOB_0,
    /*  1 */ ENHY_ANIM_BOJ_1,
    /*  2 */ ENHY_ANIM_BOJ_2,
    /*  3 */ ENHY_ANIM_BOJ_3,
    /*  4 */ ENHY_ANIM_BOJ_4,
    /*  5 */ ENHY_ANIM_BOJ_5,
    /*  6 */ ENHY_ANIM_BBA_6,
    /*  7 */ ENHY_ANIM_BJI_7,
    /*  8 */ ENHY_ANIM_BJI_8,
    /*  9 */ ENHY_ANIM_BJI_9,
    /* 10 */ ENHY_ANIM_BOJ_10,
    /* 11 */ ENHY_ANIM_OS_ANIME_11,
    /* 12 */ ENHY_ANIM_BOJ_12,
    /* 13 */ ENHY_ANIM_BOJ_13,
    /* 14 */ ENHY_ANIM_BOJ_14,
    /* 15 */ ENHY_ANIM_BOJ_15,
    /* 16 */ ENHY_ANIM_BOJ_16,
    /* 17 */ ENHY_ANIM_BOJ_17,
    /* 18 */ ENHY_ANIM_BOJ_18,
    /* 19 */ ENHY_ANIM_BOJ_19,
    /* 20 */ ENHY_ANIM_BOJ_20,
    /* 21 */ ENHY_ANIM_MAX
} EnHyAnimation;

typedef void (*EnHyActionFunc)(struct EnHy*, PlayState*);

typedef enum EnHyBodyPart {
    /*  0 */ ENHY_BODYPART_0,
    /*  1 */ ENHY_BODYPART_1,
    /*  2 */ ENHY_BODYPART_2,
    /*  3 */ ENHY_BODYPART_3,
    /*  4 */ ENHY_BODYPART_4,
    /*  5 */ ENHY_BODYPART_5,
    /*  6 */ ENHY_BODYPART_6,
    /*  7 */ ENHY_BODYPART_7,
    /*  8 */ ENHY_BODYPART_8,
    /*  9 */ ENHY_BODYPART_9,
    /* 10 */ ENHY_BODYPART_10,
    /* 11 */ ENHY_BODYPART_11,
    /* 12 */ ENHY_BODYPART_12,
    /* 13 */ ENHY_BODYPART_13,
    /* 14 */ ENHY_BODYPART_14,
    /* 15 */ ENHY_BODYPART_MAX
} EnHyBodyPart;

typedef struct EnHy {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnHyActionFunc actionFunc;
    /* 0x148 */ EnHyActionFunc prevActionFunc;
    /* 0x14C */ SkelAnime skelAnime;
    /* 0x190 */ s8 headObjectSlot; // Limb 15
    /* 0x191 */ s8 skelUpperObjectSlot; // Limbs 8-14
    /* 0x192 */ s8 skelLowerObjectSlot; // Limbs 1-7
    /* 0x193 */ s8 animObjectSlot;
    /* 0x194 */ ColliderCylinder collider;
    /* 0x1E0 */ u16 textId;
    /* 0x1E2 */ u8 waitingOnInit;
    /* 0x1E3 */ u8 msgFading;
    /* 0x1E4 */ Path* path;
    /* 0x1E8 */ s16 curPoint;
    /* 0x1EC */ Vec3f leftFootPos;
    /* 0x1F8 */ Vec3f rightFootPos;
    /* 0x204 */ u8 isLeftFootOnGround;
    /* 0x205 */ u8 isRightFootOnGround;
    /* 0x206 */ Vec3s jointTable[ENHY_LIMB_MAX];
    /* 0x266 */ Vec3s morphTable[ENHY_LIMB_MAX];
    /* 0x2C6 */ Vec3s trackTarget;
    /* 0x2CC */ Vec3s headRot;
    /* 0x2D2 */ Vec3s torsoRot;
    /* 0x2D8 */ Vec3s prevTrackTarget;
    /* 0x2DE */ Vec3s prevHeadRot;
    /* 0x2E4 */ Vec3s prevTorsoRot;
    /* 0x2EA */ s16 fidgetTableY[ENHY_LIMB_MAX];
    /* 0x30A */ s16 fidgetTableZ[ENHY_LIMB_MAX];
    /* 0x32C */ Vec3f bodyPartsPos[ENHY_BODYPART_MAX];
    /* 0x3E0 */ UNK_TYPE1 unk_3E0[0x6];
    /* 0x3E6 */ s16 eyeTexIndex;
    /* 0x3E8 */ s16 blinkTimer;
} EnHy; // size = 0x3EC

extern s8 gEnHyLimbToBodyParts[];
extern s8 gEnHyParentShadowBodyParts[];
extern u8 gEnHyShadowSizes[];

s32 EnHy_ChangeAnim(SkelAnime* skelAnime, s16 animIndex);
EnDoor* EnHy_FindNearestDoor(Actor* actor, PlayState* play);
void EnHy_ChangeObjectAndAnim(EnHy* enHy, PlayState* play, s16 animIndex);
s32 EnHy_UpdateSkelAnime(EnHy* enHy, PlayState* play);
void EnHy_Blink(EnHy* enHy, s32 eyeTexMaxIndex);
s32 EnHy_Init(EnHy* enHy, PlayState* play, FlexSkeletonHeader* skeletonHeaderSeg, s16 animIndex);
void func_800F0BB4(EnHy* enHy, PlayState* play, EnDoor* door, s16 arg3, s16 arg4);
s32 func_800F0CE4(EnHy* enHy, PlayState* play, ActorFunc draw, s16 arg3, s16 arg4, f32 arg5);
s32 func_800F0DD4(EnHy* enHy, PlayState* play, s16 arg2, s16 arg3);
s32 EnHy_SetPointFowards(EnHy* enHy, PlayState* play, f32 gravity, s16 animIndex);
s32 EnHy_SetPointBackwards(EnHy* enHy, PlayState* play, s16 animIndex);
s32 EnHy_MoveForwards(EnHy* enHy, f32 speedTarget);
s32 EnHy_MoveBackwards(EnHy* enHy, f32 speedTarget);
void EnHy_UpdateCollider(EnHy* enHy, PlayState* play);
s32 EnHy_PlayWalkingSound(EnHy* enHy, PlayState* play, f32 distAboveThreshold);

#endif // Z_EN_HY_CODE_H

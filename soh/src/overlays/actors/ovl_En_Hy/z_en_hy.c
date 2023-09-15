/*
 * File: z_en_hy.c
 * Overlay: ovl_En_Hy
 * Description: Hylian NPCs
 */

#include "z_en_hy.h"
#include "objects/object_aob/object_aob.h"
#include "objects/object_ahg/object_ahg.h"
#include "objects/object_bob/object_bob.h"
#include "objects/object_boj/object_boj.h"
#include "objects/object_bba/object_bba.h"
#include "objects/object_bji/object_bji.h"
#include "objects/object_cne/object_cne.h"
#include "objects/object_cob/object_cob.h"
#include "objects/object_os_anime/object_os_anime.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED)

void EnHy_Init(Actor* thisx, PlayState* play);
void EnHy_Destroy(Actor* thisx, PlayState* play);
void EnHy_Update(Actor* thisx, PlayState* play);
void EnHy_Draw(Actor* thisx, PlayState* play);

void EnHy_InitImpl(EnHy* this, PlayState* play);
void func_80A7134C(EnHy* this, PlayState* play);
void func_80A71530(EnHy* this, PlayState* play);
void func_80A711B4(EnHy* this, PlayState* play);
void func_80A712C0(EnHy* this, PlayState* play);
void func_80A710F8(EnHy* this, PlayState* play);
void func_80A7127C(EnHy* this, PlayState* play);
void EnHy_DoNothing(EnHy* this, PlayState* play);
void func_80A714C4(EnHy* this, PlayState* play);

const ActorInit En_Hy_InitVars = {
    ACTOR_EN_HY,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnHy),
    (ActorFunc)EnHy_Init,
    (ActorFunc)EnHy_Destroy,
    (ActorFunc)EnHy_Update,
    (ActorFunc)EnHy_Draw,
    NULL,
};

static ColliderCylinderInit sColCylInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

// NULL-terminated arrays of eye textures
static void* sEyeTexturesAOB[] = { gDogLadyEyeOpenTex, gDogLadyEyeHalfTex, gDogLadyEyeClosedTex, NULL };
static void* sEyeTexturesAHG7[] = { object_ahg_Tex_00057C, object_ahg_Tex_00067C, object_ahg_Tex_00077C, NULL };
static void* sEyeTexturesBBA[] = { object_bba_Tex_0004C8, NULL };
static void* sEyeTexturesBJI13[] = { object_bji_Tex_0005FC, object_bji_Tex_0009FC, object_bji_Tex_000DFC, NULL };
static void* sEyeTexturesBOJ2[] = { object_boj_Tex_0005FC, object_boj_Tex_0006FC, object_boj_Tex_0007FC, NULL };
static void* sEyeTexturesBOB[] = { object_bob_Tex_0007C8, object_bob_Tex_000FC8, object_bob_Tex_0017C8, NULL };

typedef struct {
    /* 0x00 */ s16 objectId;
    /* 0x04 */ Gfx* headDList;
    /* 0x08 */ void** eyeTextures;
} EnHyHeadInfo; // size = 0xC

typedef enum {
    /*  0 */ ENHY_HEAD_AOB,
    /*  1 */ ENHY_HEAD_BOB,
    /*  2 */ ENHY_HEAD_BOJ_2,
    /*  3 */ ENHY_HEAD_BOJ_3,
    /*  4 */ ENHY_HEAD_BOJ_4,
    /*  5 */ ENHY_HEAD_BOJ_5,
    /*  6 */ ENHY_HEAD_BOJ_6,
    /*  7 */ ENHY_HEAD_AHG_7,
    /*  8 */ ENHY_HEAD_AHG_8,
    /*  9 */ ENHY_HEAD_AHG_9,
    /* 10 */ ENHY_HEAD_BBA,
    /* 11 */ ENHY_HEAD_CNE_11,
    /* 12 */ ENHY_HEAD_CNE_12,
    /* 13 */ ENHY_HEAD_BJI_13,
    /* 14 */ ENHY_HEAD_BJI_14,
    /* 15 */ ENHY_HEAD_COB
} EnHyHeadIndex;

static EnHyHeadInfo sHeadInfo[] = {
    /* ENHY_HEAD_AOB */ { OBJECT_AOB, gDogLadyHeadDL, sEyeTexturesAOB },
    /* ENHY_HEAD_BOB */ { OBJECT_BOB, object_bob_DL_003B78, sEyeTexturesBOB },
    /* ENHY_HEAD_BOJ_2 */ { OBJECT_BOJ, object_boj_DL_0026F0, sEyeTexturesBOJ2 },
    /* ENHY_HEAD_BOJ_3 */ { OBJECT_BOJ, object_boj_DL_0052E0, NULL },
    /* ENHY_HEAD_BOJ_4 */ { OBJECT_BOJ, object_boj_DL_005528, NULL },
    /* ENHY_HEAD_BOJ_5 */ { OBJECT_BOJ, object_boj_DL_005738, NULL },
    /* ENHY_HEAD_BOJ_6 */ { OBJECT_BOJ, object_boj_DL_0059B0, NULL },
    /* ENHY_HEAD_AHG_7 */ { OBJECT_AHG, object_ahg_DL_0030F0, sEyeTexturesAHG7 },
    /* ENHY_HEAD_AHG_8 */ { OBJECT_AHG, object_ahg_DL_005508, NULL },
    /* ENHY_HEAD_AHG_9 */ { OBJECT_AHG, object_ahg_DL_005728, NULL },
    /* ENHY_HEAD_BBA */ { OBJECT_BBA, object_bba_DL_002948, sEyeTexturesBBA },
    /* ENHY_HEAD_CNE_11 */ { OBJECT_CNE, gCneHeadBrownHairDL, NULL },
    /* ENHY_HEAD_CNE_12 */ { OBJECT_CNE, gCneHeadOrangeHairDL, NULL },
    /* ENHY_HEAD_BJI_13 */ { OBJECT_BJI, object_bji_DL_002560, sEyeTexturesBJI13 },
    /* ENHY_HEAD_BJI_14 */ { OBJECT_BJI, object_bji_DL_003F68, NULL },
    /* ENHY_HEAD_COB */ { OBJECT_COB, object_cob_DL_001300, NULL },
};

typedef struct {
    /* 0x00 */ s16 objectId;
    /* 0x04 */ FlexSkeletonHeader* skeleton;
} EnHySkeletonInfo; // size = 0x8

typedef enum {
    /* 0 */ ENHY_SKEL_AOB,
    /* 1 */ ENHY_SKEL_BOB,
    /* 2 */ ENHY_SKEL_BOJ,
    /* 3 */ ENHY_SKEL_AHG,
    /* 4 */ ENHY_SKEL_BBA,
    /* 5 */ ENHY_SKEL_CNE,
    /* 6 */ ENHY_SKEL_BJI,
    /* 7 */ ENHY_SKEL_COB
} EnHySkeletonIndex;

static EnHySkeletonInfo sSkeletonInfo[] = {
    /* ENHY_SKEL_AOB */ { OBJECT_AOB, &gDogLadySkel },
    /* ENHY_SKEL_BOB */ { OBJECT_BOB, &object_bob_Skel_0000F0 },
    /* ENHY_SKEL_BOJ */ { OBJECT_BOJ, &object_boj_Skel_0000F0 },
    /* ENHY_SKEL_AHG */ { OBJECT_AHG, &object_ahg_Skel_0000F0 },
    /* ENHY_SKEL_BBA */ { OBJECT_BBA, &object_bba_Skel_0000F0 },
    /* ENHY_SKEL_CNE */ { OBJECT_CNE, &gCneSkel },
    /* ENHY_SKEL_BJI */ { OBJECT_BJI, &object_bji_Skel_0000F0 },
    /* ENHY_SKEL_COB */ { OBJECT_COB, &object_cob_Skel_0021F8 },
};

typedef enum {
    /*  0 */ ENHY_ANIM_0,
    /*  1 */ ENHY_ANIM_1,
    /*  2 */ ENHY_ANIM_2,
    /*  3 */ ENHY_ANIM_3,
    /*  4 */ ENHY_ANIM_4,
    /*  5 */ ENHY_ANIM_5,
    /*  6 */ ENHY_ANIM_6,
    /*  7 */ ENHY_ANIM_7,
    /*  8 */ ENHY_ANIM_8,
    /*  9 */ ENHY_ANIM_9,
    /* 10 */ ENHY_ANIM_10,
    /* 11 */ ENHY_ANIM_11,
    /* 12 */ ENHY_ANIM_12,
    /* 13 */ ENHY_ANIM_13,
    /* 14 */ ENHY_ANIM_14,
    /* 15 */ ENHY_ANIM_15,
    /* 16 */ ENHY_ANIM_16,
    /* 17 */ ENHY_ANIM_17,
    /* 18 */ ENHY_ANIM_18,
    /* 19 */ ENHY_ANIM_19,
    /* 20 */ ENHY_ANIM_20,
    /* 21 */ ENHY_ANIM_21,
    /* 22 */ ENHY_ANIM_22,
    /* 23 */ ENHY_ANIM_23,
    /* 24 */ ENHY_ANIM_24,
    /* 25 */ ENHY_ANIM_25,
    /* 26 */ ENHY_ANIM_26
} EnHyAnimationIndex;

static AnimationInfo sAnimationInfo[] = {
    /* ENHY_ANIM_0 */ { &gObjOsAnim_092C, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_1 */ { &gObjOsAnim_0228, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_2 */ { &gObjOsAnim_4CF4, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_3 */ { &gObjOsAnim_16EC, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_4 */ { &gObjOsAnim_265C, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_5 */ { &gObjOsAnim_42AC, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_6 */ { &gObjOsAnim_28DC, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_7 */ { &gObjOsAnim_2160, 1.0f, 0.0f, -1.0f, 0x00, -10.0f },
    /* ENHY_ANIM_8 */ { &gObjOsAnim_265C, 1.0f, 0.0f, -1.0f, 0x00, -10.0f },
    /* ENHY_ANIM_9 */ { &gObjOsAnim_4E90, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_10 */ { &gObjOsAnim_1E7C, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_11 */ { &gObjOsAnim_0170, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_12 */ { &gObjOsAnim_00B4, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_13 */ { &gObjOsAnim_3D84, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_14 */ { &gObjOsAnim_41F8, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_15 */ { &gObjOsAnim_300C, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_16 */ { &gObjOsAnim_31B0, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_17 */ { &gObjOsAnim_31B0, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    /* ENHY_ANIM_18 */ { &gObjOsAnim_2D0C, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_19 */ { &gObjOsAnim_2DC0, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_20 */ { &gObjOsAnim_4408, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_21 */ { &gObjOsAnim_1F18, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_22 */ { &gObjOsAnim_4F28, 1.0f, 0.0f, -1.0f, 0x00, 0.0f },
    /* ENHY_ANIM_23 */ { &gObjOsAnim_33B4, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    /* ENHY_ANIM_24 */ { &gObjOsAnim_12E8, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    /* ENHY_ANIM_25 */ { &gObjOsAnim_0FE4, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    /* ENHY_ANIM_26 */ { &gObjOsAnim_0BFC, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
};

typedef struct {
    /* 0x00 */ u8 headInfoIndex;  // EnHyHeadIndex
    /* 0x01 */ u8 skelInfoIndex2; // EnHySkeletonIndex, see EnHy#objBankIndexSkel2
    /* 0x02 */ Color_RGBA8 envColorSeg8;
    /* 0x06 */ u8 skelInfoIndex1; // EnHySkeletonIndex, see EnHy#objBankIndexSkel1
    /* 0x07 */ Color_RGBA8 envColorSeg9;
    /* 0x0B */ u8 animInfoIndex; // EnHyAnimationIndex
} EnHyModelInfo;                 // size = 0xC

static EnHyModelInfo sModelInfo[] = {
    /* ENHY_TYPE_AOB */
    { ENHY_HEAD_AOB, ENHY_SKEL_AOB, { 255, 255, 255, 255 }, ENHY_SKEL_AOB, { 255, 255, 255, 255 }, ENHY_ANIM_0 },
    /* ENHY_TYPE_COB */
    { ENHY_HEAD_COB, ENHY_SKEL_COB, { 255, 255, 255, 255 }, ENHY_SKEL_COB, { 255, 255, 255, 255 }, ENHY_ANIM_22 },
    /* ENHY_TYPE_AHG_2 */
    { ENHY_HEAD_AHG_7, ENHY_SKEL_AHG, { 255, 255, 255, 255 }, ENHY_SKEL_AHG, { 255, 255, 255, 255 }, ENHY_ANIM_1 },
    /* ENHY_TYPE_BOJ_3 */
    { ENHY_HEAD_BOJ_3, ENHY_SKEL_BOJ, { 255, 255, 255, 0 }, ENHY_SKEL_BOJ, { 55, 55, 255, 0 }, ENHY_ANIM_15 },
    /* ENHY_TYPE_AHG_4 */
    { ENHY_HEAD_AHG_8, ENHY_SKEL_AHG, { 0, 0, 0, 0 }, ENHY_SKEL_AHG, { 255, 0, 0, 0 }, ENHY_ANIM_11 },
    /* ENHY_TYPE_BOJ_5 */
    { ENHY_HEAD_BOJ_4, ENHY_SKEL_BOJ, { 50, 80, 0, 0 }, ENHY_SKEL_BOJ, { 50, 80, 0, 0 }, ENHY_ANIM_16 },
    /* ENHY_TYPE_BBA */
    { ENHY_HEAD_BBA, ENHY_SKEL_BBA, { 255, 255, 255, 255 }, ENHY_SKEL_BBA, { 255, 255, 255, 255 }, ENHY_ANIM_10 },
    /* ENHY_TYPE_BJI_7 */
    { ENHY_HEAD_BJI_13, ENHY_SKEL_BJI, { 0, 50, 160, 0 }, ENHY_SKEL_BJI, { 255, 255, 255, 0 }, ENHY_ANIM_4 },
    /* ENHY_TYPE_CNE_8 */
    { ENHY_HEAD_CNE_11, ENHY_SKEL_CNE, { 160, 180, 255, 0 }, ENHY_SKEL_CNE, { 160, 180, 255, 0 }, ENHY_ANIM_9 },
    /* ENHY_TYPE_BOJ_9 */
    { ENHY_HEAD_BOJ_2, ENHY_SKEL_BOJ, { 220, 0, 80, 0 }, ENHY_SKEL_BOJ, { 255, 255, 255, 0 }, ENHY_ANIM_13 },
    /* ENHY_TYPE_BOJ_10 */
    { ENHY_HEAD_BOJ_2, ENHY_SKEL_BOJ, { 0, 130, 220, 0 }, ENHY_SKEL_BOJ, { 255, 255, 255, 0 }, ENHY_ANIM_14 },
    /* ENHY_TYPE_CNE_11 */
    { ENHY_HEAD_CNE_12, ENHY_SKEL_CNE, { 70, 160, 230, 0 }, ENHY_SKEL_CNE, { 255, 255, 100, 0 }, ENHY_ANIM_20 },
    /* ENHY_TYPE_BOJ_12 */
    { ENHY_HEAD_BOJ_5, ENHY_SKEL_BOJ, { 150, 60, 90, 0 }, ENHY_SKEL_BOJ, { 255, 240, 150, 0 }, ENHY_ANIM_18 },
    /* ENHY_TYPE_AHG_13 */
    { ENHY_HEAD_AHG_9, ENHY_SKEL_AHG, { 200, 180, 255, 0 }, ENHY_SKEL_AHG, { 200, 180, 255, 0 }, ENHY_ANIM_12 },
    /* ENHY_TYPE_BOJ_14 */
    { ENHY_HEAD_BOJ_6, ENHY_SKEL_BOJ, { 140, 255, 110, 0 }, ENHY_SKEL_BOJ, { 255, 255, 255, 0 }, ENHY_ANIM_19 },
    /* ENHY_TYPE_BJI_15 */
    { ENHY_HEAD_BJI_14, ENHY_SKEL_BJI, { 130, 70, 20, 0 }, ENHY_SKEL_BJI, { 130, 180, 255, 0 }, ENHY_ANIM_21 },
    /* ENHY_TYPE_BOJ_16 */
    { ENHY_HEAD_BOJ_2, ENHY_SKEL_BOJ, { 255, 255, 255, 255 }, ENHY_SKEL_BOJ, { 255, 255, 255, 255 }, ENHY_ANIM_5 },
    /* ENHY_TYPE_AHG_17 */
    { ENHY_HEAD_AHG_8, ENHY_SKEL_AHG, { 90, 100, 20, 255 }, ENHY_SKEL_AHG, { 100, 140, 50, 255 }, ENHY_ANIM_11 },
    /* ENHY_TYPE_BOB_18 */
    { ENHY_HEAD_BOB, ENHY_SKEL_BOB, { 255, 255, 255, 255 }, ENHY_SKEL_BOB, { 255, 255, 255, 255 }, ENHY_ANIM_6 },
    /* ENHY_TYPE_BJI_19 */
    { ENHY_HEAD_BJI_14, ENHY_SKEL_BJI, { 160, 0, 100, 0 }, ENHY_SKEL_BJI, { 70, 130, 210, 0 }, ENHY_ANIM_21 },
    /* ENHY_TYPE_AHG_20 */
    { ENHY_HEAD_AHG_9, ENHY_SKEL_AHG, { 160, 230, 0, 0 }, ENHY_SKEL_AHG, { 0, 150, 110, 0 }, ENHY_ANIM_12 },
};

typedef struct {
    /* 0x00 */ Vec3s offset;
    /* 0x06 */ s16 radius;
    /* 0x08 */ s16 height;
} EnHyColliderInfo; // size 0xA

static EnHyColliderInfo sColliderInfo[] = {
    /* ENHY_TYPE_AOB */ { { 0, 0, 4 }, 24, 70 },
    /* ENHY_TYPE_COB */ { { 0, 0, 8 }, 28, 62 },
    /* ENHY_TYPE_AHG_2 */ { { 0, 0, 4 }, 20, 60 },
    /* ENHY_TYPE_BOJ_3 */ { { 0, 0, 2 }, 20, 60 },
    /* ENHY_TYPE_AHG_4 */ { { 0, 0, -2 }, 20, 60 },
    /* ENHY_TYPE_BOJ_5 */ { { 0, 0, 8 }, 24, 40 },
    /* ENHY_TYPE_BBA */ { { 0, 0, 10 }, 26, 40 },
    /* ENHY_TYPE_BJI_7 */ { { 0, 0, 12 }, 26, 58 },
    /* ENHY_TYPE_CNE_8 */ { { 0, 0, 2 }, 18, 68 },
    /* ENHY_TYPE_BOJ_9 */ { { 0, 0, 4 }, 20, 60 },
    /* ENHY_TYPE_BOJ_10 */ { { 0, 0, 4 }, 20, 60 },
    /* ENHY_TYPE_CNE_11 */ { { 0, 0, 6 }, 20, 64 },
    /* ENHY_TYPE_BOJ_12 */ { { 0, 0, 0 }, 18, 60 },
    /* ENHY_TYPE_AHG_13 */ { { 0, 0, 0 }, 16, 60 },
    /* ENHY_TYPE_BOJ_14 */ { { 0, 0, 0 }, 16, 64 },
    /* ENHY_TYPE_BJI_15 */ { { 0, 0, 8 }, 20, 58 },
    /* ENHY_TYPE_BOJ_16 */ { { 4, 0, 0 }, 18, 62 },
    /* ENHY_TYPE_AHG_17 */ { { 4, 0, 0 }, 18, 62 },
    /* ENHY_TYPE_BOB_18 */ { { 0, 0, 8 }, 28, 62 },
    /* ENHY_TYPE_BJI_19 */ { { 0, 0, 0 }, 16, 60 },
    /* ENHY_TYPE_AHG_20 */ { { 0, 0, 8 }, 20, 58 },
};

typedef struct {
    /* 0x00 */ u8 unkPresetIndex;
    /* 0x04 */ f32 unkValueChild;
    /* 0x08 */ f32 unkValueAdult;
} EnHyInit1Info; // size = 0xC

static EnHyInit1Info sInit1Info[] = {
    /* ENHY_TYPE_AOB */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_COB */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_AHG_2 */ { 0x07, 40.0f, 20.0f },
    /* ENHY_TYPE_BOJ_3 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_AHG_4 */ { 0x07, 40.0f, 20.0f },
    /* ENHY_TYPE_BOJ_5 */ { 0x08, 0.0f, -20.0f },
    /* ENHY_TYPE_BBA */ { 0x09, 20.0f, 0.0f },
    /* ENHY_TYPE_BJI_7 */ { 0x09, 20.0f, 0.0f },
    /* ENHY_TYPE_CNE_8 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_BOJ_9 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_BOJ_10 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_CNE_11 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_BOJ_12 */ { 0x00, 0.0f, 0.0f },
    /* ENHY_TYPE_AHG_13 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_BOJ_14 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_BJI_15 */ { 0x0A, 20.0f, 0.0f },
    /* ENHY_TYPE_BOJ_16 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_AHG_17 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_BOB_18 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_BJI_19 */ { 0x06, 20.0f, 10.0f },
    /* ENHY_TYPE_AHG_20 */ { 0x0A, 20.0f, 0.0f },
};

typedef struct {
    /* 0x00 */ f32 shadowScale;
    /* 0x04 */ Vec3f modelOffset;
    /* 0x10 */ f32 scale;
    /* 0x14 */ s8 targetMode;
    /* 0x18 */ f32 unkRange;
} EnHyInit2Info; // size = 0x1C

static EnHyInit2Info sInit2Info[] = {
    /* ENHY_TYPE_AOB */ { 36.0f, { 0.0f, 0.0f, 600.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_COB */ { 40.0f, { -100.0f, 0.0f, 400.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_AHG_2 */ { 22.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOJ_3 */ { 20.0f, { -100.0f, 0.0f, 0.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_AHG_4 */ { 22.0f, { 0.0f, 0.0f, 0.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOJ_5 */ { 21.0f, { 0.0f, 0.0f, 0.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BBA */ { 25.0f, { -100.0f, 0.0f, 600.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BJI_7 */ { 28.0f, { -100.0f, 0.0f, 800.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_CNE_8 */ { 17.0f, { 0.0f, 0.0f, 700.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOJ_9 */ { 18.0f, { 0.0f, 0.0f, 100.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOJ_10 */ { 18.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_CNE_11 */ { 17.0f, { 0.0f, 0.0f, 700.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOJ_12 */ { 21.0f, { 0.0f, 0.0f, -300.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_AHG_13 */ { 20.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOJ_14 */ { 18.0f, { -200.0f, 0.0f, -200.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BJI_15 */ { 27.0f, { -100.0f, 0.0f, 800.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOJ_16 */ { 19.0f, { 400.0f, 0.0f, 0.0f }, 0.01f, 0x04, 30.0f },
    /* ENHY_TYPE_AHG_17 */ { 19.0f, { 400.0f, 0.0f, 0.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BOB_18 */ { 40.0f, { -100.0f, 0.0f, 400.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_BJI_19 */ { 17.0f, { 0.0f, 0.0f, 700.0f }, 0.01f, 0x06, 30.0f },
    /* ENHY_TYPE_AHG_20 */ { 20.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 0x06, 30.0f },
};

s32 EnHy_FindSkelAndHeadObjects(EnHy* this, PlayState* play) {
    u8 headInfoIndex = sModelInfo[this->actor.params & 0x7F].headInfoIndex;
    u8 skelInfoIndex2 = sModelInfo[this->actor.params & 0x7F].skelInfoIndex2;
    u8 skelInfoIndex1 = sModelInfo[this->actor.params & 0x7F].skelInfoIndex1;

    this->objBankIndexSkel1 = Object_GetIndex(&play->objectCtx, sSkeletonInfo[skelInfoIndex1].objectId);
    if (this->objBankIndexSkel1 < 0) {
        return false;
    }

    this->objBankIndexSkel2 = Object_GetIndex(&play->objectCtx, sSkeletonInfo[skelInfoIndex2].objectId);
    if (this->objBankIndexSkel2 < 0) {
        return false;
    }

    this->objBankIndexHead = Object_GetIndex(&play->objectCtx, sHeadInfo[headInfoIndex].objectId);
    if (this->objBankIndexHead < 0) {
        return false;
    }

    return true;
}

s32 EnHy_AreSkelAndHeadObjectsLoaded(EnHy* this, PlayState* play) {
    if (!Object_IsLoaded(&play->objectCtx, this->objBankIndexSkel1)) {
        return false;
    }

    if (!Object_IsLoaded(&play->objectCtx, this->objBankIndexSkel2)) {
        return false;
    }

    if (!Object_IsLoaded(&play->objectCtx, this->objBankIndexHead)) {
        return false;
    }

    return true;
}

s32 EnHy_FindOsAnimeObject(EnHy* this, PlayState* play) {
    this->objBankIndexOsAnime = Object_GetIndex(&play->objectCtx, OBJECT_OS_ANIME);

    if (this->objBankIndexOsAnime < 0) {
        return false;
    }

    return true;
}

s32 EnHy_IsOsAnimeObjectLoaded(EnHy* this, PlayState* play) {
    if (!Object_IsLoaded(&play->objectCtx, this->objBankIndexOsAnime)) {
        return false;
    }

    return true;
}

void func_80A6F7CC(EnHy* this, PlayState* play, s32 getItemId) {
    this->unkGetItemId = getItemId;
    func_8002F434(&this->actor, play, getItemId, this->actor.xzDistToPlayer + 1.0f,
                  fabsf(this->actor.yDistToPlayer) + 1.0f);
}

u16 func_80A6F810(PlayState* play, Actor* thisx) {
    Player* player = GET_PLAYER(play);
    EnHy* this = (EnHy*)thisx;
    u16 textId = Text_GetFaceReaction(play, (this->actor.params & 0x7F) + 37);

    if (textId != 0) {
        if ((this->actor.params & 0x7F) == ENHY_TYPE_BOJ_5) {
            player->exchangeItemId = EXCH_ITEM_BLUE_FIRE;
        }
        return textId;
    }

    switch (this->actor.params & 0x7F) {
        case ENHY_TYPE_AOB:
            if (play->sceneNum == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return (this->unk_330 & 0x800) ? 0x508D : ((Flags_GetInfTable(INFTABLE_CB)) ? 0x508C : 0x508B);
            } else if (play->sceneNum == SCENE_MARKET_DAY) {
                return (gSaveContext.eventInf[3] & 1) ? 0x709B : 0x709C;
            } else if (gSaveContext.dogIsLost) {
                s16 followingDog = (gSaveContext.dogParams & 0xF00) >> 8;

                if (followingDog != 0) {
                    this->unk_215 = false;
                    return (followingDog == 1) ? 0x709F : 0x709E;
                } else {
                    return 0x709D;
                }
            } else {
                return 0x70A0;
            }
        case ENHY_TYPE_COB:
            if (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) {
                return (Flags_GetInfTable(INFTABLE_C1)) ? 0x7017 : 0x7045;
            } else {
                return (Flags_GetInfTable(INFTABLE_C0)) ? 0x7017 : 0x7016;
            }
        case ENHY_TYPE_AHG_2:
            if (play->sceneNum == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return 0x5086;
            } else if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
                return 0x5085;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) {
                return (Flags_GetInfTable(INFTABLE_C3)) ? 0x701A : 0x7047;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                return 0x701A;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET)) {
                return 0x701B;
            } else if (Flags_GetInfTable(INFTABLE_C2)) {
                return 0x701C;
            } else {
                return 0x701A;
            }
        case ENHY_TYPE_BOJ_3:
            return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? ((Flags_GetInfTable(INFTABLE_C4)) ? 0x7001 : 0x70EB) : 0x7001;
        case ENHY_TYPE_AHG_4:
            return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? 0x704B : ((Flags_GetInfTable(INFTABLE_C5)) ? 0x7024 : 0x7023);
        case ENHY_TYPE_BOJ_5:
            player->exchangeItemId = EXCH_ITEM_BLUE_FIRE;
            return 0x700C;
        case ENHY_TYPE_BBA:
            return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? 0x704A : ((Flags_GetInfTable(INFTABLE_C6)) ? 0x7022 : 0x7021);
        case ENHY_TYPE_BJI_7:
            if (play->sceneNum == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return 0x5088;
            } else if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
                return 0x5087;
            } else {
                return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? 0x704D
                                                         : ((Flags_GetInfTable(INFTABLE_C7)) ? 0x7028 : 0x7027);
            }
        case ENHY_TYPE_CNE_8:
            if (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) {
                return (Flags_GetInfTable(INFTABLE_C9)) ? 0x701E : 0x7048;
            } else {
                return (Flags_GetInfTable(INFTABLE_C8)) ? 0x701E : 0x701D;
            }
        case ENHY_TYPE_BOJ_9:
            if (play->sceneNum == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return (Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL)) ? 0x5082 : 0x5081;
            } else if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
                return CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) ? 0x5080 : 0x507F;
            } else {
                return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? 0x7049
                                                         : ((Flags_GetInfTable(INFTABLE_CA)) ? 0x7020 : 0x701F);
            }
        case ENHY_TYPE_BOJ_10:
            if (play->sceneNum == SCENE_IMPAS_HOUSE) {
                return (Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL)) ? 0x507E : 0x507D;
            } else if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
                return CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) ? 0x507C : 0x507B;
            } else {
                return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? 0x7046
                                                         : ((Flags_GetInfTable(INFTABLE_CD)) ? 0x7019 : 0x7018);
            }
        case ENHY_TYPE_CNE_11:
            return (Flags_GetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE)) ? ((Flags_GetInfTable(INFTABLE_CC)) ? 0x7014 : 0x70A4)
                                                      : 0x7014;
        case ENHY_TYPE_BOJ_12:
            if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
                return !IS_DAY ? 0x5084 : 0x5083;
            } else {
                return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? 0x7044 : 0x7015;
            }
        case ENHY_TYPE_AHG_13:
            return 0x7055;
        case ENHY_TYPE_BOJ_14:
            return 0x7089;
        case ENHY_TYPE_BJI_15:
            return 0x708A;
        case ENHY_TYPE_BOJ_16:
            return 0x700E;
        case ENHY_TYPE_AHG_17:
            if (!LINK_IS_ADULT) {
                if (IS_DAY) {
                    return (Flags_GetInfTable(INFTABLE_160)) ? 0x5058 : 0x5057;
                } else {
                    return (Flags_GetInfTable(INFTABLE_161)) ? 0x505A : 0x5059;
                }
            } else if (IS_DAY) {
                return (Flags_GetInfTable(INFTABLE_162)) ? 0x505C : 0x505B;
            } else {
                return 0x5058;
            }
        case ENHY_TYPE_BOB_18:
            if (!LINK_IS_ADULT) {
                return (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) ? 0x505F : ((Flags_GetInfTable(INFTABLE_163)) ? 0x505E : 0x505D);
            } else {
                return (this->unk_330 & 0x800) ? 0x5062 : ((Flags_GetInfTable(INFTABLE_164)) ? 0x5061 : 0x5060);
            }
        case ENHY_TYPE_BJI_19:
            return 0x7120;
        case ENHY_TYPE_AHG_20:
            return 0x7121;
        default:
            return 0;
    }
}

s16 func_80A70058(PlayState* play, Actor* thisx) {
    EnHy* this = (EnHy*)thisx;
    s16 beggarItems[] = { ITEM_BLUE_FIRE, ITEM_FISH, ITEM_BUG, ITEM_FAIRY };
    s16 beggarRewards[] = { 150, 100, 50, 25 };

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return NPC_TALK_STATE_TALKING;
        case TEXT_STATE_DONE_FADING:
            switch (this->actor.textId) {
                case 0x709E:
                case 0x709F:
                    if (!this->unk_215) {
                        Audio_PlaySoundGeneral(this->actor.textId == 0x709F ? NA_SE_SY_CORRECT_CHIME : NA_SE_SY_ERROR,
                                               &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        this->unk_215 = true;
                    }
                    break;
                case 0x70F0:
                case 0x70F1:
                case 0x70F2:
                case 0x70F3:
                    if (this->skelAnime.animation != &gObjOsAnim_33B4) {
                        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_23);
                        Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
                    }
                    break;
            }
            return NPC_TALK_STATE_TALKING;
        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x70F0:
                case 0x70F1:
                case 0x70F2:
                case 0x70F3:
                    Rupees_ChangeBy(beggarRewards[this->actor.textId - 0x70F0]);
                    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_17);
                    Player_UpdateBottleHeld(play, GET_PLAYER(play), ITEM_BOTTLE, PLAYER_IA_BOTTLE);
                    break;
                case 0x7016:
                    Flags_SetInfTable(INFTABLE_C0);
                    break;
                case 0x7045:
                    Flags_SetInfTable(INFTABLE_C1);
                    break;
                case 0x701B:
                    Flags_SetInfTable(INFTABLE_C2);
                    break;
                case 0x7047:
                    Flags_SetInfTable(INFTABLE_C3);
                    break;
                case 0x70EB:
                    Flags_SetInfTable(INFTABLE_C4);
                    break;
                case 0x7023:
                    Flags_SetInfTable(INFTABLE_C5);
                    break;
                case 0x7021:
                    Flags_SetInfTable(INFTABLE_C6);
                    break;
                case 0x7027:
                    Flags_SetInfTable(INFTABLE_C7);
                    break;
                case 0x701D:
                    Flags_SetInfTable(INFTABLE_C8);
                    break;
                case 0x7048:
                    Flags_SetInfTable(INFTABLE_C9);
                    break;
                case 0x701F:
                    Flags_SetInfTable(INFTABLE_CA);
                    break;
                case 0x7018:
                    Flags_SetInfTable(INFTABLE_CD);
                    break;
                case 0x70A4:
                    Flags_SetInfTable(INFTABLE_CC);
                    break;
                case 0x5057:
                    Flags_SetInfTable(INFTABLE_160);
                    break;
                case 0x5059:
                    Flags_SetInfTable(INFTABLE_161);
                    break;
                case 0x505B:
                    Flags_SetInfTable(INFTABLE_162);
                    break;
                case 0x505D:
                    Flags_SetInfTable(INFTABLE_163);
                    break;
                case 0x5060:
                    Flags_SetInfTable(INFTABLE_164);
                    break;
                case 0x508B:
                    Flags_SetInfTable(INFTABLE_CB);
                    break;
                case 0x709E:
                    gSaveContext.dogParams = 0;
                    break;
                case 0x709F:
                    if (Flags_GetInfTable(INFTABLE_191)) { // Already brought the lost dog back
                        func_80A6F7CC(this, play, GI_RUPEE_BLUE);
                    } else {
                        if (!gSaveContext.n64ddFlag) {
                            func_80A6F7CC(this, play, GI_HEART_PIECE);
                        } else {
                            this->getItemEntry = Randomizer_GetItemFromKnownCheck(RC_MARKET_LOST_DOG, GI_HEART_PIECE);
                            // The follownig line and last arguments of GiveItemEntryFromActor are copied from func_80A6F7CC
                            this->unkGetItemId = this->getItemEntry.getItemId;
                            GiveItemEntryFromActor(&this->actor, play, this->getItemEntry, this->actor.xzDistToPlayer + 1.0f, fabsf(this->actor.yDistToPlayer) + 1.0f);
                        }
                    }
                    this->actionFunc = func_80A714C4;
                    break;
            }
            return NPC_TALK_STATE_IDLE;
        case TEXT_STATE_EVENT:
            if (!Message_ShouldAdvance(play)) {
                return NPC_TALK_STATE_TALKING;
            } else {
                return NPC_TALK_STATE_ACTION;
            }
    }

    return NPC_TALK_STATE_TALKING;
}

void EnHy_UpdateEyes(EnHy* this) {
    if (DECR(this->nextEyeIndexTimer) == 0) {
        u8 headInfoIndex = sModelInfo[this->actor.params & 0x7F].headInfoIndex;

        this->curEyeIndex++;
        if ((sHeadInfo[headInfoIndex].eyeTextures != NULL) &&
            (sHeadInfo[headInfoIndex].eyeTextures[this->curEyeIndex] == NULL)) {
            this->nextEyeIndexTimer = Rand_S16Offset(30, 30);
            this->curEyeIndex = 0;
        }
    }
}

void EnHy_InitCollider(EnHy* this) {
    u8 type = this->actor.params & 0x7F;

    this->collider.dim.radius = sColliderInfo[type].radius;
    this->collider.dim.height = sColliderInfo[type].height;
}

void EnHy_InitSetProperties(EnHy* this) {
    u8 type = this->actor.params & 0x7F;

    this->actor.shape.shadowScale = sInit2Info[type].shadowScale;
    Actor_SetScale(&this->actor, sInit2Info[type].scale);
    this->actor.targetMode = sInit2Info[type].targetMode;
    this->modelOffset = sInit2Info[type].modelOffset;
    this->unkRange = sInit2Info[type].unkRange;
    this->unkRange += this->collider.dim.radius;
}

void EnHy_UpdateCollider(EnHy* this, PlayState* play) {
    Vec3s pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;
    pos.x += sColliderInfo[this->actor.params & 0x7F].offset.x;
    pos.y += sColliderInfo[this->actor.params & 0x7F].offset.y;
    pos.z += sColliderInfo[this->actor.params & 0x7F].offset.z;
    this->collider.dim.pos = pos;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80A70834(EnHy* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actor.params & 0x7F) == ENHY_TYPE_BOJ_5) {
        if (!Inventory_HasSpecificBottle(ITEM_BLUE_FIRE) && !Inventory_HasSpecificBottle(ITEM_BUG) &&
            !Inventory_HasSpecificBottle(ITEM_FISH)) {
            switch (func_8002F368(play)) {
                case EXCH_ITEM_POE:
                case EXCH_ITEM_BIG_POE:
                case EXCH_ITEM_LETTER_RUTO:
                    this->actor.textId = 0x70EF;
                    break;
                default:
                    if (Player_GetMask(play) == PLAYER_MASK_NONE) {
                        this->actor.textId = 0x70ED;
                    }
                    break;
            }
        } else {
            switch (func_8002F368(play)) {
                case EXCH_ITEM_BLUE_FIRE:
                    this->actor.textId = 0x70F0;
                    break;
                case EXCH_ITEM_FISH:
                    this->actor.textId = 0x70F1;
                    break;
                case EXCH_ITEM_BUG:
                    this->actor.textId = 0x70F2;
                    break;
                default:
                    if (Player_GetMask(play) == PLAYER_MASK_NONE) {
                        this->actor.textId = 0x700C;
                    }
                    break;
            }
        }

        player->actor.textId = this->actor.textId;
    }
}

void func_80A70978(EnHy* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    switch (this->actor.params & 0x7F) {
        case ENHY_TYPE_BOJ_3:
        case ENHY_TYPE_BJI_7:
        case ENHY_TYPE_BOJ_9:
        case ENHY_TYPE_BOJ_10:
            trackingMode =
                (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) ? NPC_TRACKING_NONE : NPC_TRACKING_HEAD_AND_TORSO;
            break;
        case ENHY_TYPE_BOJ_12:
            trackingMode = NPC_TRACKING_NONE;
            break;
        case ENHY_TYPE_AHG_2:
        case ENHY_TYPE_AHG_17:
            trackingMode = NPC_TRACKING_FULL_BODY;
            break;
        case ENHY_TYPE_AOB:
        case ENHY_TYPE_BOB_18:
            trackingMode = (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) ? NPC_TRACKING_HEAD_AND_TORSO
                                                                                 : NPC_TRACKING_FULL_BODY;
            break;
        default:
            trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
            break;
    }

    this->interactInfo.trackPos = player->actor.world.pos;

    if (LINK_IS_ADULT) {
        this->interactInfo.yOffset = sInit1Info[this->actor.params & 0x7F].unkValueAdult;
    } else {
        this->interactInfo.yOffset = sInit1Info[this->actor.params & 0x7F].unkValueChild;
    }

    Npc_TrackPoint(&this->actor, &this->interactInfo, sInit1Info[this->actor.params & 0x7F].unkPresetIndex,
                   trackingMode);

    if (Npc_UpdateTalking(play, &this->actor, &this->interactInfo.talkState, this->unkRange, func_80A6F810,
                          func_80A70058)) {
        func_80A70834(this, play);
    }
}

s32 EnHy_ShouldSpawn(EnHy* this, PlayState* play) {
    switch (play->sceneNum) {
        case SCENE_KAKARIKO_VILLAGE:
            if (!((this->actor.params & 0x7F) == ENHY_TYPE_BOJ_9 || (this->actor.params & 0x7F) == ENHY_TYPE_BOJ_10 ||
                  (this->actor.params & 0x7F) == ENHY_TYPE_BOJ_12 || (this->actor.params & 0x7F) == ENHY_TYPE_AHG_2 ||
                  (this->actor.params & 0x7F) == ENHY_TYPE_BJI_7)) {
                return true;
            } else if (!LINK_IS_ADULT) {
                return true;
            } else if ((this->actor.params & 0x7F) != ENHY_TYPE_BOJ_12 && IS_NIGHT) {
                return false;
            } else {
                return true;
            }
        case SCENE_IMPAS_HOUSE:
            if ((this->actor.params & 0x7F) != ENHY_TYPE_BOJ_10) {
                return true;
            } else if (LINK_IS_CHILD) {
                return false;
            } else if ((this->actor.params & 0x7F) == ENHY_TYPE_BOJ_10 && IS_DAY) {
                return false;
            } else {
                return true;
            }
        case SCENE_DOG_LADY_HOUSE:
            if ((this->actor.params & 0x7F) != ENHY_TYPE_AOB) {
                return true;
            } else if (IS_DAY) {
                return false;
            } else {
                return true;
            }
        case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
            if ((this->actor.params & 0x7F) == ENHY_TYPE_AOB) {
                return !LINK_IS_ADULT ? false : true;
            } else if (!((this->actor.params & 0x7F) == ENHY_TYPE_BOJ_9 ||
                         (this->actor.params & 0x7F) == ENHY_TYPE_AHG_2 ||
                         (this->actor.params & 0x7F) == ENHY_TYPE_BJI_7)) {
                return true;
            } else if (IS_DAY) {
                return false;
            } else if (LINK_IS_CHILD) {
                return false;
            } else {
                return true;
            }
        case SCENE_BACK_ALLEY_DAY:
        case SCENE_BACK_ALLEY_NIGHT:
            if ((this->actor.params & 0x7F) != ENHY_TYPE_BOJ_14) {
                return true;
            } else if (IS_NIGHT) {
                return false;
            } else if ((Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) && !Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
                return false;
            } else {
                return true;
            }
        default:
            switch (this->actor.params & 0x7F) {
                case ENHY_TYPE_BJI_19:
                case ENHY_TYPE_AHG_20:
                    if (LINK_IS_ADULT) {
                        return false;
                    }
            }
            return true;
    }
}

void EnHy_Init(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;

    if ((this->actor.params & 0x7F) >= ENHY_TYPE_MAX || !EnHy_FindOsAnimeObject(this, play) ||
        !EnHy_FindSkelAndHeadObjects(this, play)) {
        Actor_Kill(&this->actor);
    }

    if (!EnHy_ShouldSpawn(this, play)) {
        Actor_Kill(&this->actor);
    }

    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    this->actionFunc = EnHy_InitImpl;
}

void EnHy_Destroy(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnHy_InitImpl(EnHy* this, PlayState* play) {
    if (EnHy_IsOsAnimeObjectLoaded(this, play) && EnHy_AreSkelAndHeadObjectsLoaded(this, play)) {
        this->actor.objBankIndex = this->objBankIndexSkel1;
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->actor.objBankIndex].segment);
        SkelAnime_InitFlex(play, &this->skelAnime,
                           sSkeletonInfo[sModelInfo[this->actor.params & 0x7F].skelInfoIndex1].skeleton, NULL,
                           this->jointTable, this->morphTable, 16);
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objBankIndexOsAnime].segment);
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sColCylInit);
        EnHy_InitCollider(this);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, sModelInfo[this->actor.params & 0x7F].animInfoIndex);

        if ((play->sceneNum == SCENE_BACK_ALLEY_DAY) || (play->sceneNum == SCENE_MARKET_DAY)) {
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_WHILE_CULLED;
            this->actor.uncullZoneScale = 0.0f;
        }

        if (play->sceneNum == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
            this->unk_330 = gSaveContext.eventChkInf[6];
        }

        EnHy_InitSetProperties(this);
        this->path = Path_GetByIndex(play, (this->actor.params & 0x780) >> 7, 15);

        switch (this->actor.params & 0x7F) {
            case ENHY_TYPE_BOJ_3:
                if (this->path != NULL) {
                    this->actor.speedXZ = 3.0f;
                }
                this->actionFunc = func_80A711B4;
                break;
            case ENHY_TYPE_BJI_7:
                this->pathReverse = false;
                this->actionFunc = func_80A712C0;
                break;
            case ENHY_TYPE_AOB:
                if (play->sceneNum == SCENE_MARKET_DAY) {
                    this->actionFunc = func_80A710F8;
                    break;
                }
                // fall-through
            case ENHY_TYPE_COB:
            case ENHY_TYPE_AHG_2:
            case ENHY_TYPE_AHG_4:
            case ENHY_TYPE_BBA:
            case ENHY_TYPE_CNE_8:
            case ENHY_TYPE_AHG_13:
            case ENHY_TYPE_BOJ_14:
            case ENHY_TYPE_BJI_15:
            case ENHY_TYPE_BOJ_16:
            case ENHY_TYPE_AHG_17:
            case ENHY_TYPE_BOB_18:
            case ENHY_TYPE_BJI_19:
            case ENHY_TYPE_AHG_20:
                this->actionFunc = func_80A7127C;
                break;
            case ENHY_TYPE_BOJ_5:
            case ENHY_TYPE_BOJ_9:
            case ENHY_TYPE_BOJ_10:
            case ENHY_TYPE_CNE_11:
            case ENHY_TYPE_BOJ_12:
                this->actionFunc = EnHy_DoNothing;
                break;
            default:
                Actor_Kill(&this->actor);
                break;
        }
    }
}

void func_80A710F8(EnHy* this, PlayState* play) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (this->skelAnime.animation != &gObjOsAnim_0BFC) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_26);
        }
    } else if (gSaveContext.eventInf[3] & 1) {
        if (this->skelAnime.animation != &gObjOsAnim_0FE4) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_25);
        }
    } else if (this->skelAnime.animation != &gObjOsAnim_12E8) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_24);
    }
}

void func_80A711B4(EnHy* this, PlayState* play) {
    s16 yaw;
    f32 distSq;

    distSq = Path_OrientAndGetDistSq(&this->actor, this->path, this->waypoint, &yaw);
    Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 10, 1000, 1);
    this->actor.shape.rot = this->actor.world.rot;

    if ((distSq > 0.0f) && (distSq < 1000.0f)) {
        this->waypoint++;
        if (this->waypoint > (this->path->count - 1)) {
            this->waypoint = 0;
        }
    }
}

void func_80A7127C(EnHy* this, PlayState* play) {
    func_80034F54(play, this->unk_21C, this->unk_23C, 16);
}

void EnHy_DoNothing(EnHy* this, PlayState* play) {
}

void func_80A712C0(EnHy* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer <= 100.0f) && (this->path != NULL)) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_7);
        this->actor.speedXZ = 0.4f;
        this->actionFunc = func_80A7134C;
    }

    func_80034F54(play, this->unk_21C, this->unk_23C, 16);
}

void func_80A7134C(EnHy* this, PlayState* play) {
    s16 yaw;
    f32 distSq;

    if ((this->skelAnime.animation == &gObjOsAnim_2160) && (this->interactInfo.talkState != NPC_TALK_STATE_IDLE)) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_8);
    }

    if ((this->skelAnime.animation == &gObjOsAnim_265C) && (this->interactInfo.talkState == NPC_TALK_STATE_IDLE)) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENHY_ANIM_7);
    }

    this->actor.speedXZ = 0.4f;
    distSq = Path_OrientAndGetDistSq(&this->actor, this->path, this->waypoint, &yaw);
    Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 10, 1000, 1);
    this->actor.shape.rot = this->actor.world.rot;

    if (!(distSq <= 0.0f) && !(distSq >= 1000.0f)) {
        if (!this->pathReverse) {
            this->waypoint++;
            if (this->waypoint > (this->path->count - 1)) {
                this->pathReverse = true;
                this->waypoint = this->path->count - 2;
            }
        } else {
            this->waypoint--;
            if (this->waypoint < 0) {
                this->pathReverse = false;
                this->waypoint = 1;
            }
        }
    }
}

void func_80A714C4(EnHy* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actionFunc = func_80A71530;
    } else {
        if (!gSaveContext.n64ddFlag || this->getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, play, this->unkGetItemId, this->actor.xzDistToPlayer + 1.0f, fabsf(this->actor.yDistToPlayer) + 1.0f);
        } else {
            GiveItemEntryFromActor(&this->actor, play, this->getItemEntry, this->actor.xzDistToPlayer + 1.0f, fabsf(this->actor.yDistToPlayer) + 1.0f);
        }
    }
}

void func_80A71530(EnHy* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (gSaveContext.n64ddFlag) {
            if (!Flags_GetInfTable(INFTABLE_191)) {
                Flags_SetInfTable(INFTABLE_191);
            }
            gSaveContext.dogParams = 0;
            gSaveContext.dogIsLost = false;
        } else {
            switch (this->unkGetItemId) {
                case GI_HEART_PIECE:
                    gSaveContext.dogParams = 0;
                    gSaveContext.dogIsLost = false;
                    Flags_SetInfTable(INFTABLE_191);
                    break;
                case GI_RUPEE_BLUE:
                    Rupees_ChangeBy(5);
                    gSaveContext.dogParams = 0;
                    gSaveContext.dogIsLost = false;
                    break;
            }
        }
        this->actionFunc = func_80A7127C;
    }
}

void EnHy_Update(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;

    if (this->actionFunc != EnHy_InitImpl) {
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objBankIndexOsAnime].segment);
        SkelAnime_Update(&this->skelAnime);
        EnHy_UpdateEyes(this);

        if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            Actor_MoveForward(&this->actor);
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    }

    this->actionFunc(this, play);
    func_80A70978(this, play);
    EnHy_UpdateCollider(this, play);
}

s32 EnHy_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHy* this = (EnHy*)thisx;
    s32 pad;
    Vec3s sp48;
    u8 i;
    UNK_PTR ptr;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 15) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[this->objBankIndexHead].segment);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objBankIndexHead].segment);
        i = sModelInfo[this->actor.params & 0x7F].headInfoIndex;
        *dList = sHeadInfo[i].headDList;

        if (sHeadInfo[i].eyeTextures != NULL) {
            ptr = sHeadInfo[i].eyeTextures[this->curEyeIndex];
            gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(ptr));
        }

        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objBankIndexSkel1].segment);
    }

    if (limbIndex == 15) {
        Matrix_Translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp48 = this->interactInfo.headRot;
        Matrix_RotateX((sp48.y / (f32)0x8000) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((sp48.x / (f32)0x8000) * M_PI, MTXMODE_APPLY);
        Matrix_Translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 8) {
        sp48 = this->interactInfo.torsoRot;
        Matrix_RotateX((-sp48.y / (f32)0x8000) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((sp48.x / (f32)0x8000) * M_PI, MTXMODE_APPLY);
    }

    if ((limbIndex == 8) || (limbIndex == 9) || (limbIndex == 12)) {
        rot->y += Math_SinS(this->unk_21C[limbIndex]) * 200.0f;
        rot->z += Math_CosS(this->unk_23C[limbIndex]) * 200.0f;
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return false;
}

void EnHy_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnHy* this = (EnHy*)thisx;
    s32 pad;
    Vec3f sp3C = { 400.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 7) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[this->objBankIndexSkel2].segment);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objBankIndexSkel2].segment);
    }

    if ((this->actor.params & 0x7F) == ENHY_TYPE_BOJ_3 && limbIndex == 8) {
        gSPDisplayList(POLY_OPA_DISP++, object_boj_DL_005BC8);
    }

    if (limbIndex == 15) {
        Matrix_MultVec3f(&sp3C, &this->actor.focus.pos);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

Gfx* EnHy_SetEnvColor(GraphicsContext* play, u8 envR, u8 envG, u8 envB, u8 envA) {
    Gfx* dList;

    dList = Graph_Alloc(play, 2 * sizeof(Gfx));
    gDPSetEnvColor(dList, envR, envG, envB, envA);
    gSPEndDisplayList(dList + 1);

    return dList;
}

void EnHy_Draw(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;
    Color_RGBA8 envColorSeg8;
    Color_RGBA8 envColorSeg9;
    Color_RGBA8 envColorSeg10;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actionFunc != EnHy_InitImpl) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        Matrix_Translate(this->modelOffset.x, this->modelOffset.y, this->modelOffset.z, MTXMODE_APPLY);
        envColorSeg8 = sModelInfo[this->actor.params & 0x7F].envColorSeg8;
        envColorSeg9 = sModelInfo[this->actor.params & 0x7F].envColorSeg9;

        switch (this->actor.params & 0x7F) {
            // ENHY_TYPE_AOB
            // ENHY_TYPE_COB
            case ENHY_TYPE_AHG_2:
            case ENHY_TYPE_BOJ_3:
            case ENHY_TYPE_AHG_4:
            case ENHY_TYPE_BOJ_5:
            // ENHY_TYPE_BBA
            case ENHY_TYPE_BJI_7:
            case ENHY_TYPE_CNE_8:
            case ENHY_TYPE_BOJ_9:
            case ENHY_TYPE_BOJ_10:
            case ENHY_TYPE_CNE_11:
            case ENHY_TYPE_BOJ_12:
            case ENHY_TYPE_AHG_13:
            case ENHY_TYPE_BOJ_14:
            case ENHY_TYPE_BJI_15:
            case ENHY_TYPE_BOJ_16:
            case ENHY_TYPE_AHG_17:
            // ENHY_TYPE_BOB_18
            case ENHY_TYPE_BJI_19:
            case ENHY_TYPE_AHG_20:
                gSPSegment(POLY_OPA_DISP++, 0x08,
                           EnHy_SetEnvColor(play->state.gfxCtx, envColorSeg8.r, envColorSeg8.g, envColorSeg8.b,
                                            envColorSeg8.a));
                gSPSegment(POLY_OPA_DISP++, 0x09,
                           EnHy_SetEnvColor(play->state.gfxCtx, envColorSeg9.r, envColorSeg9.g, envColorSeg9.b,
                                            envColorSeg9.a));

                if ((this->actor.params & 0x7F) == ENHY_TYPE_CNE_8 || (this->actor.params & 0x7F) == ENHY_TYPE_CNE_11) {
                    if ((this->actor.params & 0x7F) == ENHY_TYPE_CNE_8) {
                        envColorSeg10 = envColorSeg8;
                    }
                    if ((this->actor.params & 0x7F) == ENHY_TYPE_CNE_11) {
                        envColorSeg10.r = envColorSeg10.g = envColorSeg10.b = 255;
                        envColorSeg10.a = 0;
                    }
                    gSPSegment(POLY_OPA_DISP++, 0x0A,
                               EnHy_SetEnvColor(play->state.gfxCtx, envColorSeg10.r, envColorSeg10.g,
                                                envColorSeg10.b, envColorSeg10.a));
                }
                break;
        }

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, EnHy_OverrideLimbDraw, EnHy_PostLimbDraw, &this->actor);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

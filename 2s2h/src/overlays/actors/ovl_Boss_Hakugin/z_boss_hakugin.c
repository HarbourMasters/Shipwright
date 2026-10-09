/*
 * File: z_boss_hakugin.c
 * Overlay: ovl_Boss_Hakugin
 * Description: Goht
 *
 * This actor is responsible for handling most things involving the Goht boss fight. In addition to Goht itself, this
 * also includes the block of ice it is encased in before the fight begins, the lightning and electric ball attacks it
 * can fire at the player, the various cutscenes that play during the fight, etc.
 */

#include "z_boss_hakugin.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Hakurock/z_en_hakurock.h"
#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define GOHT_LIMB_DRAW_FLAG(limbIndex) (1 << ((limbIndex)-1))

void BossHakugin_Init(Actor* thisx, PlayState* play2);
void BossHakugin_Destroy(Actor* thisx, PlayState* play);
void BossHakugin_Update(Actor* thisx, PlayState* play);
void func_80B0E548(Actor* thisx, PlayState* play2);
void BossHakugin_Draw(Actor* thisx, PlayState* play);
void func_80B0E5A4(Actor* thisx, PlayState* play);

void func_80B058C0(BossHakugin* this);
void func_80B05EE0(BossHakugin* this, PlayState* play);
void BossHakugin_GenShadowTex(u8* tex, BossHakugin* this);
void BossHakugin_DrawShadowTex(u8* tex, BossHakugin* this, PlayState* play);
void func_80B0D9CC(BossHakugin* this);
void func_80B0DFA8(BossHakugin* this);

void BossHakugin_SpawnLargeStalactiteWalls(BossHakugin* this);
void BossHakugin_SpawnBlueWarpAndHeartContainer(BossHakugin* this, PlayState* play);
void BossHakugin_EntranceCutscene(BossHakugin* this, PlayState* play);
void BossHakugin_SetupFrozenBeforeFight(BossHakugin* this);
void BossHakugin_FrozenBeforeFight(BossHakugin* this, PlayState* play);
void BossHakugin_IntroCutsceneThaw(BossHakugin* this, PlayState* play);
void BossHakugin_SetupIntroCutsceneWakeUp(BossHakugin* this, PlayState* play);
void BossHakugin_IntroCutsceneWakeUp(BossHakugin* this, PlayState* play);
void BossHakugin_SetupIntroCutsceneRun(BossHakugin* this, PlayState* play);
void BossHakugin_IntroCutsceneRun(BossHakugin* this, PlayState* play);
void BossHakugin_SetupRun(BossHakugin* this);
void BossHakugin_Run(BossHakugin* this, PlayState* play);
void BossHakugin_SetupCharge(BossHakugin* this);
void BossHakugin_Charge(BossHakugin* this, PlayState* play);
void BossHakugin_Downed(BossHakugin* this, PlayState* play);
void BossHakugin_Throw(BossHakugin* this, PlayState* play);
void BossHakugin_SetupWait(BossHakugin* this, PlayState* play);
void BossHakugin_Wait(BossHakugin* this, PlayState* play);
void BossHakugin_SetupShootLightning(BossHakugin* this);
void BossHakugin_ShootLightning(BossHakugin* this, PlayState* play);
void BossHakugin_SetupCutsceneStart(BossHakugin* this);
void BossHakugin_CutsceneStart(BossHakugin* this, PlayState* play);
void BossHakugin_SetupDeathCutsceneRun(BossHakugin* this);
void BossHakugin_DeathCutsceneRun(BossHakugin* this, PlayState* play);
void BossHakugin_SetupDeathCutsceneSwerveIntoWall(BossHakugin* this);
void BossHakugin_DeathCutsceneSwerveIntoWall(BossHakugin* this, PlayState* play);
void BossHakugin_SetupDeathCutsceneCrushedByRocks(BossHakugin* this);
void BossHakugin_DeathCutsceneCrushedByRocks(BossHakugin* this, PlayState* play);
void BossHakugin_GenShadowTex(u8* tex, BossHakugin* this);
void BossHakugin_DrawShadowTex(u8* tex, BossHakugin* this, PlayState* play);
void BossHakugin_SpawnCrushingRocks(BossHakugin* this);
void BossHakugin_UpdateCrushingRocksCollision(BossHakugin* this);
void BossHakugin_UpdateStationaryCrushingRocks(Actor* thisx, PlayState* play2);
void BossHakugin_DrawCrushingRocks(Actor* thisx, PlayState* play);

typedef enum GohtElectricBallState {
    // The electric ball is not active. Goht will start charging the electric ball the next time the player hits it with
    // Goron Spikes while it's running.
    /* 0 */ GOHT_ELECTRIC_BALL_STATE_NONE,

    // Goht is charging the electric ball up and will shoot it forward after a short delay.
    /* 1 */ GOHT_ELECTRIC_BALL_STATE_CHARGE,

    // The electric ball is flying forward from Goht's head and slowing down until its speed reaches 50.0f.
    /* 2 */ GOHT_ELECTRIC_BALL_STATE_FLY_FORWARD,

    // The electric ball is rotating to track the player and gradually moves towards them.
    /* 3 */ GOHT_ELECTRIC_BALL_STATE_FLY_TOWARDS_PLAYER,

    // The electric ball hit the player or the environment, so it's fading away. It can't hurt the player in this state.
    /* 4 */ GOHT_ELECTRIC_BALL_STATE_FADE_OUT
} GohtElectricBallState;

typedef enum GohtDirection {
    /* -1 */ GOHT_DIRECTION_COUNTERCLOCKWISE = -1,
    /*  1 */ GOHT_DIRECTION_CLOCKWISE = 1
} GohtDirection;

typedef enum GohtNextCutsceneType {
    /* 0 */ GOHT_NEXT_CUTSCENE_TYPE_ENTRANCE,
    /* 1 */ GOHT_NEXT_CUTSCENE_TYPE_INTRO
} GohtNextCutsceneType;

typedef enum GohtShadowSize {
    /* 0 */ GOHT_SHADOW_SIZE_MEDIUM,
    /* 1 */ GOHT_SHADOW_SIZE_LARGE,
    /* 2 */ GOHT_SHADOW_SIZE_EXTRA_LARGE,
    /* 3 */ GOHT_SHADOW_SIZE_SMALL
} GohtShadowSize;

ActorProfile Boss_Hakugin_Profile = {
    /**/ ACTOR_BOSS_HAKUGIN,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS_HAKUGIN,
    /**/ sizeof(BossHakugin),
    /**/ BossHakugin_Init,
    /**/ BossHakugin_Destroy,
    /**/ BossHakugin_Update,
    /**/ BossHakugin_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[GOHT_COLLIDER_BODYPART_MAX] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_HEAD, { { 1300, 200, 0 }, 30 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_THORAX, { { 1400, 300, 0 }, 35 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_PELVIS, { { 1600, -400, 0 }, 26 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_FRONT_RIGHT_UPPER_LEG, { { 1700, 200, 400 }, 18 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_FRONT_RIGHT_LOWER_LEG, { { 1000, 0, 0 }, 10 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_FRONT_RIGHT_HOOF, { { 500, 0, 0 }, 9 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_FRONT_LEFT_UPPER_LEG, { { 1700, 200, -400 }, 18 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_FRONT_LEFT_LOWER_LEG, { { 1000, 0, 0 }, 10 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_FRONT_LEFT_HOOF, { { 500, 0, 0 }, 9 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_RIGHT_THIGH, { { 800, 0, 400 }, 15 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_RIGHT_SHIN, { { 500, 200, 0 }, 9 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_RIGHT_PASTERN, { { 300, 0, 0 }, 8 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_RIGHT_HOOF, { { 500, 100, 0 }, 9 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_LEFT_THIGH, { { 800, 0, -400 }, 15 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_LEFT_SHIN, { { 500, 200, 0 }, 9 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_LEFT_PASTERN, { { 300, 0, 0 }, 8 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_BACK_LEFT_HOOF, { { 500, 100, 0 }, 9 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_HEAD, { { 2400, -400, -2900 }, 12 }, 270 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { GOHT_LIMB_HEAD, { { 2400, -400, 2900 }, 12 }, 270 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static ColliderTrisElementInit sTrisElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x20000000, 0x03, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { { { 22.0f, 0.0f, 100.0f }, { 0.0f, 0.0f, -100.0f }, { -22.0f, 0.0f, 100.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK5,
        { 0x20000000, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 1, { { 0, 0, 0 }, 23 }, 100 },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x02, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 170, 40, 0, { 0, 0, 0 } },
};

typedef enum GohtDamageEffect {
    // If an attack with this damage effect hits Goht while it's standing upright or running, it will deal no damage and
    // spawn some spark effects. If it hits Goht while it's downed, however, it will deal damage with no special effect.
    /* 0x0 */ GOHT_DMGEFF_DOWNED_ONLY,

    // Deals damage and surrounds Goht with fire.
    /* 0x2 */ GOHT_DMGEFF_FIRE = 2,

    // Deals damage and surrounds Goht with ice that instantly shatters.
    /* 0x3 */ GOHT_DMGEFF_FREEZE,

    // Deals damage and surrounds Goht with yellow light orbs.
    /* 0x4 */ GOHT_DMGEFF_LIGHT_ORB,

    // Deals damage and can additionally knock Goht down if it hits Goht on the head, thorax, pelvis, or upper legs
    // while it isn't charging.
    /* 0xC */ GOHT_DMGEFF_EXPLOSIVE = 0xC,

    // Deals damage and surrounds Goht with blue light orbs.
    /* 0xD */ GOHT_DMGEFF_BLUE_LIGHT_ORB,

    // Deals damage with no special effect.
    /* 0xE */ GOHT_DMGEFF_NONE,

    // Deals damage and can additionally knock Goht down in the same manner as GOHT_DMGEFF_EXPLOSIVE. If Goht is hit by
    // an attack with this damage effect and it does *not* knock it down, however, then Goht will charge up and fire an
    // electric ball attack.
    /* 0xF */ GOHT_DMGEFF_GORON_SPIKES
} GohtDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Deku Stick     */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Horse trample  */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Explosives     */ DMG_ENTRY(1, GOHT_DMGEFF_EXPLOSIVE),
    /* Zora boomerang */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Normal arrow   */ DMG_ENTRY(1, GOHT_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Hookshot       */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Goron punch    */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Sword          */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Goron pound    */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Fire arrow     */ DMG_ENTRY(1, GOHT_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, GOHT_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(1, GOHT_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, GOHT_DMGEFF_GORON_SPIKES),
    /* Deku spin      */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Deku bubble    */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Deku launch    */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Zora barrier   */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Normal shield  */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Light ray      */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Thrown object  */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Zora punch     */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Spin attack    */ DMG_ENTRY(1, GOHT_DMGEFF_DOWNED_ONLY),
    /* Sword beam     */ DMG_ENTRY(1, GOHT_DMGEFF_BLUE_LIGHT_ORB),
    /* Normal Roll    */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Unblockable    */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, GOHT_DMGEFF_DOWNED_ONLY),
    /* Powder Keg     */ DMG_ENTRY(1, GOHT_DMGEFF_EXPLOSIVE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 30, 80, 100, MASS_IMMOVABLE };

/**
 * For whatever reason, the chin of the "face" on Goht's head relies on the actor to put the texture in segment 0x08 to
 * render properly. Since the texture never changes, the reason for doing it like this is unclear.
 */
TexturePtr sChinTexture = gGohtMetalPlateWithCirclePatternTex;

static s8 sLimbToBodyParts[GOHT_LIMB_MAX] = {
    BODYPART_NONE,                       // GOHT_LIMB_NONE
    BODYPART_NONE,                       // GOHT_LIMB_ROOT
    GOHT_BODYPART_PELVIS,                // GOHT_LIMB_PELVIS
    BODYPART_NONE,                       // GOHT_LIMB_THORAX_ROOT
    BODYPART_NONE,                       // GOHT_LIMB_THORAX_WRAPPER
    BODYPART_NONE,                       // GOHT_LIMB_FRONT_RIGHT_LEG_ROOT
    GOHT_BODYPART_FRONT_RIGHT_UPPER_LEG, // GOHT_LIMB_FRONT_RIGHT_UPPER_LEG
    BODYPART_NONE,                       // GOHT_LIMB_FRONT_RIGHT_LOWER_LEG_ROOT
    GOHT_BODYPART_FRONT_RIGHT_LOWER_LEG, // GOHT_LIMB_FRONT_RIGHT_LOWER_LEG
    GOHT_BODYPART_FRONT_RIGHT_HOOF,      // GOHT_LIMB_FRONT_RIGHT_HOOF
    BODYPART_NONE,                       // GOHT_LIMB_FRONT_LEFT_LEG_ROOT
    GOHT_BODYPART_FRONT_LEFT_UPPER_LEG,  // GOHT_LIMB_FRONT_LEFT_UPPER_LEG
    BODYPART_NONE,                       // GOHT_LIMB_FRONT_LEFT_LOWER_LEG_ROOT
    GOHT_BODYPART_FRONT_LEFT_LOWER_LEG,  // GOHT_LIMB_FRONT_LEFT_LOWER_LEG
    GOHT_BODYPART_FRONT_LEFT_HOOF,       // GOHT_LIMB_FRONT_LEFT_HOOF
    GOHT_BODYPART_THORAX,                // GOHT_LIMB_THORAX
    GOHT_BODYPART_HEAD,                  // GOHT_LIMB_HEAD
    BODYPART_NONE,                       // GOHT_LIMB_JAW_ROOT
    BODYPART_NONE,                       // GOHT_LIMB_JAW
    BODYPART_NONE,                       // GOHT_LIMB_BACK_RIGHT_LEG_ROOT
    BODYPART_NONE,                       // GOHT_LIMB_BACK_RIGHT_LEG_WRAPPER
    GOHT_BODYPART_BACK_RIGHT_THIGH,      // GOHT_LIMB_BACK_RIGHT_THIGH
    GOHT_BODYPART_BACK_RIGHT_SHIN,       // GOHT_LIMB_BACK_RIGHT_SHIN
    BODYPART_NONE,                       // GOHT_LIMB_BACK_RIGHT_PASTERN_ROOT
    BODYPART_NONE,                       // GOHT_LIMB_BACK_RIGHT_PASTERN
    GOHT_BODYPART_BACK_RIGHT_HOOF,       // GOHT_LIMB_BACK_RIGHT_HOOF
    BODYPART_NONE,                       // GOHT_LIMB_BACK_LEFT_LEG_ROOT
    BODYPART_NONE,                       // GOHT_LIMB_BACK_LEFT_LEG_WRAPPER
    GOHT_BODYPART_BACK_LEFT_THIGH,       // GOHT_LIMB_BACK_LEFT_THIGH
    GOHT_BODYPART_BACK_LEFT_SHIN,        // GOHT_LIMB_BACK_LEFT_SHIN
    BODYPART_NONE,                       // GOHT_LIMB_BACK_LEFT_PASTERN_ROOT
    BODYPART_NONE,                       // GOHT_LIMB_BACK_LEFT_PASTERN
    GOHT_BODYPART_BACK_LEFT_HOOF,        // GOHT_LIMB_BACK_LEFT_HOOF
};

/**
 * Each type of malfunction effect is tied to a specific limb, and this array can be used to convert between the
 * malfunction effect type and the limb with which it's associated.
 */
s32 sMalfunctionTypeToLimbIndex[GOHT_MALFUNCTION_NUM_TYPES] = {
    GOHT_LIMB_BACK_LEFT_THIGH,       // FHGFLASH_SHOCK_GOHT_BACK_LEFT_THIGH
    GOHT_LIMB_BACK_RIGHT_PASTERN,    // FHGFLASH_SHOCK_GOHT_BACK_RIGHT_PASTERN
    GOHT_LIMB_JAW_ROOT,              // FHGFLASH_SHOCK_GOHT_JAW_ROOT
    GOHT_LIMB_FRONT_LEFT_LOWER_LEG,  // FHGFLASH_SHOCK_GOHT_FRONT_LEFT_LOWER_LEG
    GOHT_LIMB_FRONT_RIGHT_UPPER_LEG, // FHGFLASH_SHOCK_GOHT_FRONT_RIGHT_UPPER_LEG
};

/**
 * These colors are used for the sparkles that appear around the block of ice when Goht is frozen before the fight.
 */
static Color_RGBA8 sSparklePrimColor = { 250, 250, 250, 255 };
static Color_RGBA8 sSparkleEnvColor = { 180, 180, 180, 255 };

/**
 * This is used for the dynamic light that can light up the arena when various effects are active, the light orb that
 * appears near Goht's head when it's charging up an attack, and the light orbs that make up the electric ball attack.
 */
static Color_RGB8 sLightColor = { 0, 150, 255 };

/**
 * This is used for both the lightning that appears near Goht's head when it's charging up an attack and the lightning
 * segments that Goht fires to attack the player.
 */
static Color_RGB8 sLightningColor = { 0, 255, 255 };

void BossHakugin_Init(Actor* thisx, PlayState* play2) {
    static s32 sTexturesDesegmented = false;
    static InitChainEntry sInitChain[] = {
        ICHAIN_S8(hintId, TATL_HINT_ID_GOHT, ICHAIN_CONTINUE),
        ICHAIN_VEC3F_DIV1000(scale, 27, ICHAIN_CONTINUE),
        ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
    };
    PlayState* play = play2;
    BossHakugin* this = (BossHakugin*)thisx;
    Actor** actorPtr;
    s32 pad;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGohtSkel, &gGohtRunAnim, this->jointTable, this->morphTable,
                       GOHT_LIMB_MAX);

    if (!sTexturesDesegmented) {
        sTexturesDesegmented = true;
        sChinTexture = Lib_SegmentedToVirtual(sChinTexture);
    }

    Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sJntSphInit, this->bodyColliderElements);
    for (i = 0; i < GOHT_LIGHTNING_SEGMENT_COUNT; i++) {
        Collider_InitAndSetTris(play, &this->lightningSegments[i].collider, &this->actor, &sTrisInit,
                                this->lightningSegments[i].colliderElements);
    }

    Collider_InitAndSetSphere(play, &this->electricBallCollider, &this->actor, &sSphereInit);
    this->electricBallCollider.dim.worldSphere.radius = 40;
    Collider_InitAndSetCylinder(play, &this->iceCollider, &this->actor, &sCylinderInit);
    this->iceCollider.dim.pos.x = this->actor.world.pos.x - 50.0f;
    this->iceCollider.dim.pos.y = this->actor.world.pos.y;
    this->iceCollider.dim.pos.z = this->actor.world.pos.z;
    this->iceCollider.dim.radius = 150;
    this->iceCollider.dim.height = 300;
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Lights_PointNoGlowSetInfo(&this->lightInfo, 0, 0, 0, 255, 255, 180, -1);
    this->direction = GOHT_DIRECTION_COUNTERCLOCKWISE;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
        BossHakugin_SpawnBlueWarpAndHeartContainer(this, play);
        Actor_Kill(&this->actor);
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->boulders); i++) {
        //! FAKE:
        actorPtr = &this->boulders[i];
        *actorPtr = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_HAKUROCK, 0.0f, 0.0f, 0.0f, 0, 0,
                                       0, EN_HAKUROCK_TYPE_BOULDER);
    }

    for (i = 0; i < ARRAY_COUNT(this->stalactites); i++) {
        this->stalactites[i] = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_HAKUROCK, 0.0f, 0.0f,
                                                  0.0f, 0, 0, 0, EN_HAKUROCK_TYPE_FALLING_STALACTITE);
    }

    for (i = 0; i < GOHT_ROCK_EFFECT_COUNT; i++) {
        this->rockEffects[i].timer = -1;
    }

    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->baseRotY = this->actor.shape.rot.y;
    this->limbDrawFlags = 0xFFFFFFFF;

    if (CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_GOHT)) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_BOSS | SEQ_FLAG_ASYNC);
        this->actor.world.pos.x = 0.0f;
        BossHakugin_SetupRun(this);
    } else if (CHECK_EVENTINF(EVENTINF_ENTR_CS_WATCHED_GOHT)) {
        this->iceAlpha = 255;
        this->iceScaleY = 2.7f;
        BossHakugin_SpawnLargeStalactiteWalls(this);
        Animation_PlayOnce(&this->skelAnime, &gGohtThawAndBreakWallAnim);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        BossHakugin_SetupFrozenBeforeFight(this);
    } else {
        this->iceAlpha = 255;
        this->iceScaleY = 2.7f;
        BossHakugin_SpawnLargeStalactiteWalls(this);
        BossHakugin_SetupCutsceneStart(this);
    }
}

void BossHakugin_Destroy(Actor* thisx, PlayState* play) {
    BossHakugin* this = (BossHakugin*)thisx;
    s32 pad;
    s32 i;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    Collider_DestroyJntSph(play, &this->bodyCollider);
    Collider_DestroyCylinder(play, &this->iceCollider);

    for (i = 0; i < GOHT_LIGHTNING_SEGMENT_COUNT; i++) {
        Collider_DestroyTris(play, &this->lightningSegments[i].collider);
    }

    Collider_DestroySphere(play, &this->electricBallCollider);
    AudioSfx_StopByPos(&this->sfxPos);
    AudioSfx_StopByPos(&this->electricBallSfxPos);
}

s32 BossHakugin_Vec3fNormalize(Vec3f* vec) {
    f32 magnitude = Math3D_Vec3fMagnitude(vec);

    if (magnitude < 0.0001f) {
        return false;
    }

    Math_Vec3f_Scale(vec, 1.0f / magnitude);
    return true;
}

/**
 * The intention behind this function is to call it repeatedly to gradually rotate `norm` to match the `targetNorm`.
 * Specifically, this function rotates `norm` around the axis perpendicular to both `norm` and `targetNorm` (in other
 * words, it rotates around the cross product). The amount by which to rotate `norm` per call is determined by
 * `angleStep`; if rotating `norm` by this amount would cause it to overshoot the `targetNorm`, then the rotation will
 * be clamped such that the `norm` will reach its target exactly.
 *
 * This function assumes that both `norm` and `targetNorm` are normalized unit vectors; if non-unit vectors are
 * supplied, then this function will do nothing at all if the dot product of the two non-unit vectors is positive. It is
 * also possible for this function to undershoot the `targetNorm` if the angle between the `norm` and the `targetNorm`
 * is less than one degree; in this case, this function will return without doing any rotation.
 */
void BossHakugin_StepVector(Vec3f* norm, Vec3f* targetNorm, f32 angleStep) {
    Vec3f crossProduct;
    Vec3f result;
    f32 dotProduct = (norm->x * targetNorm->x) + (norm->y * targetNorm->y) + (norm->z * targetNorm->z);
    f32 maxAngle;

    if (fabsf(dotProduct) < 1.0f) {
        maxAngle = Math_FAcosF(dotProduct);
    } else if (dotProduct >= 1.0f) {
        // Either the dot product is exactly 1.0f, or a non-unit vector was passed into this function.
        maxAngle = 0.0f;
    } else {
        // Either the dot product is exactly -1.0f, or a non-unit vector was passed into this function.
        maxAngle = M_PIf;
    }

    angleStep = CLAMP_MAX(angleStep, maxAngle);
    if (angleStep < DEG_TO_RAD(1)) {
        return;
    }

    Math3D_Vec3f_Cross(norm, targetNorm, &crossProduct);

    if (BossHakugin_Vec3fNormalize(&crossProduct)) {
        Matrix_RotateAxisF(angleStep, &crossProduct, MTXMODE_NEW);
        Matrix_MultVec3f(norm, &result);
        Math_Vec3f_Copy(norm, &result);
    }
}

/**
 * Sets up the walls made of large stalactites that confine the player to the part of the boss room near the door; these
 * are the stalactites that are destroyed when Goht is thawed just before the fight begins.
 */
void BossHakugin_SpawnLargeStalactiteWalls(BossHakugin* this) {
    s32 i;
    Actor* last;
    Actor* now = this->stalactites[0];

    now->params = EN_HAKUROCK_TYPE_LARGE_STALACTITE;
    now->world.pos.x = -500.0f;
    now->world.pos.y = this->actor.world.pos.y;
    now->world.pos.z = -3800.0f / 3.0f;
    now->shape.rot.y = this->actor.shape.rot.y + 0x4000;

    // Spawns the six large stalactites in front of Goht
    for (i = 1; i < 6; i++) {
        last = now;
        now = this->stalactites[i];

        now->params = EN_HAKUROCK_TYPE_LARGE_STALACTITE;
        now->world.pos.x = last->world.pos.x;
        now->world.pos.y = this->actor.world.pos.y;
        now->world.pos.z = last->world.pos.z - 133.333333f; // 4000 / 3
        now->shape.rot.y = last->shape.rot.y;
    }

    // Spawns the four large stalactites to the left and right of Goht's ice block
    for (; i < ARRAY_COUNT(this->stalactites); i++) {
        last = (i < 8) ? this->stalactites[i - 6] : this->stalactites[i - 4];
        now = this->stalactites[i];

        now->params = EN_HAKUROCK_TYPE_LARGE_STALACTITE;
        now->world.pos.x = this->actor.world.pos.x;
        now->world.pos.y = this->actor.world.pos.y;
        now->world.pos.z = last->world.pos.z;
        now->shape.rot.y = this->actor.shape.rot.y - 0x4000;
    }
}

void BossHakugin_RequestQuakeAndRumble(BossHakugin* this, PlayState* play, s16 speed, s32 verticalMag, s16 duration) {
    s16 quakeIndex;

    if (verticalMag > 0) {
        quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, speed);
        Quake_SetPerturbations(quakeIndex, verticalMag, 0, 0, 0);
        Quake_SetDuration(quakeIndex, duration);

        Rumble_Request(this->actor.xyzDistToPlayerSq, 180, 20, 100);
    }
}

/**
 * When Goht is frozen before the fight starts, this function has a 10% chance of spawning a single sparkle effect in a
 * random position somewhere around the ice.
 */
void BossHakugin_SpawnIceSparkle(BossHakugin* this, PlayState* play) {
    Vec3f pos;

    if (((this->actionFunc == BossHakugin_FrozenBeforeFight) || (this->actionFunc == BossHakugin_EntranceCutscene)) &&
        (Rand_ZeroOne() < 0.1f)) {
        pos.x =
            this->actor.world.pos.x + ((15.0f + (Rand_ZeroOne() * 15.0f)) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1) * 4.0f);
        pos.y = this->actor.world.pos.y + ((10.0f + (Rand_ZeroOne() * 90.0f)) * 2.7f);
        pos.z =
            this->actor.world.pos.z + ((15.0f + (Rand_ZeroOne() * 15.0f)) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1) * 4.0f);

        EffectSsKirakira_SpawnDispersed(play, &pos, &gZeroVec3f, &gZeroVec3f, &sSparklePrimColor, &sSparkleEnvColor,
                                        2000, 5);
    }
}

/**
 * Boosts the player's y-velocity and linear velocity whenever they jump off a ramp while Goron rolling with spikes.
 * This makes the player's jump go much higher, and the increase to the linear velocity was seemingly done to make the
 * player's speed in the air more closely match their speed on the ground. This boost is only applied once per jump.
 */
void BossHakugin_ApplyGoronSpikeBoost(BossHakugin* this, Player* player) {
    if (!this->hasAppliedGoronSpikeBoost && (player->stateFlags3 & PLAYER_STATE3_80000) &&
        !(player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (player->actor.velocity.y > 5.0f)) {
        player->actor.velocity.y *= 1.3f;
        player->speedXZ *= 1.3f;
        this->hasAppliedGoronSpikeBoost = true;
    } else if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->hasAppliedGoronSpikeBoost = false;
    }
}

/**
 * Updates the position and radius of a single dynamic light. Since there's only one light, all of Goht's various
 * effects have to share it, so the following list is used to determine priority; if an effect higher up the list is
 * using the light, effects lower down the list will not be able to use it. In practice, it is uncommon for effects to
 * overlap such that this priority list actually matters:
 * - If an electric ball is active, the light will be attached to it. Since the electric ball is made up of multiple
 *   light orbs of different opacities, the light will be attached to the most-opaque light orb.
 * - If Goht is charging up a lightning or electric ball attack, then the light will be attached to the charging effect
 *   in front of Goht's head.
 * - If Goht is shooting lightning, then the light will be attached to the last lightning segment in the list that has
 *   an alpha of 255. This results in the light following the lightning down from Goht's head.
 */
void BossHakugin_UpdateLight(BossHakugin* this) {
    Vec3f* lightPos = &this->actor.world.pos;
    s32 i;
    s16 radius = 0;

    for (i = 0; i < GOHT_LIGHTNING_SEGMENT_COUNT; i++) {
        if (this->lightningSegments[i].alpha == 255) {
            lightPos = &this->lightningSegments[i].pos;
            radius = 5000;
            break;
        }
    }

    if (i == GOHT_LIGHTNING_SEGMENT_COUNT) {
        if (this->chargingLightOrbScale > 0.0f) {
            lightPos = &this->chargingLightningPos;
            radius = (this->chargingLightOrbScale / 30.0f) * 5000.0f;
        } else if ((this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FLY_TOWARDS_PLAYER) ||
                   (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FLY_FORWARD)) {
            lightPos = &this->electricBallPos[0];
            radius = 5000;
        } else if (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FADE_OUT) {
            lightPos = &this->electricBallPos[this->electricBallCount];
            radius = (ARRAY_COUNT(this->electricBallPos) - this->electricBallCount) * 500.0f;
        }
    }

    Lights_PointNoGlowSetInfo(&this->lightInfo, lightPos->x, lightPos->y, lightPos->z, sLightColor.r, sLightColor.g,
                              sLightColor.b, radius);
}

/**
 * Spawns the blue warp at one of four predetermined locations based on Goht's x and z-coordinates, then spawns the
 * heart container offset from the blue warp.
 */
void BossHakugin_SpawnBlueWarpAndHeartContainer(BossHakugin* this, PlayState* play) {
    s16 atan = Math_Atan2S_XY(this->actor.world.pos.z, this->actor.world.pos.x);
    Vec3f warpPos;
    Vec3f heartPos;
    f32 offset;
    f32 sin;
    f32 cos;

    if (atan > 0x4000) {
        warpPos.x = 1400.0f;
        warpPos.y = 0.0f;
        warpPos.z = -1400.0f;
    } else if (atan < -0x4000) {
        warpPos.x = -1400.0f;
        warpPos.y = 0.0f;
        warpPos.z = -1400.0f;
    } else if (atan > 0) {
        warpPos.x = 1400.0f;
        warpPos.y = -320.0f;
        warpPos.z = 1400.0f;
    } else {
        warpPos.x = -1400.0f;
        warpPos.y = -160.0f;
        warpPos.z = 1400.0f;
    }

    offset = this->direction * 300.0f;
    sin = Math_SinS(this->targetRotY);
    cos = Math_CosS(this->targetRotY);
    heartPos.x = ((100.0f * sin) + warpPos.x) + (offset * cos);
    heartPos.z = ((100.0f * cos) + warpPos.z) - (offset * sin);
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, warpPos.x, warpPos.y, warpPos.z, 0, 0, 0,
                       ENDOORWARP1_FF_1);
    Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, heartPos.x, warpPos.y, heartPos.z, 0, 0, 0,
                BHEART_PARAM_NORMAL);
}

/**
 * This function updates Goht's base y-rotation to make sure it turns to follow the curves in its arena. It also moves
 * Goht away from the sides of the room if it strays too close to them.
 *
 * Goht's "base" rotation is one component of its final rotation, and it is specifically the component that perfectly
 * follows the walls of the room; while running along the arena's straightaways, the base rotation is exactly parallel
 * to the two side walls. This function uses the angles of the normals of the left and right wall polygons to determine
 * how much Goht should turn to keep following the walls.
 */
void BossHakugin_UpdateBaseRot(BossHakugin* this, PlayState* play) {
    CollisionPoly* rightWallPoly = NULL;
    CollisionPoly* leftWallPoly = NULL;
    s32 bgId;
    s16 rightWallYawDiff;
    s16 leftWallYawDiff;
    s16 averagedYawDiff;
    Vec3f rightWallPos;
    Vec3f leftWallPos;
    Vec3f posA;
    Vec3f posB;
    f32 offsetFromWall;

    // The initial point of our line is slightly above and slightly in front of Goht. The point is slightly above Goht's
    // position because, near the floor of the arena, the walls are sloped, so elevating the check ensures it looks at
    // the flat upper walls instead. The point is placed a variable distance in front of Goht depending on its speed so
    // that it will turn sooner when it's running faster.
    posA.x = this->actor.world.pos.x + (Math_SinS(this->baseRotY) * (5.0f * this->actor.speed));
    posA.y = this->actor.world.pos.y + 450.0f;
    posA.z = this->actor.world.pos.z + (Math_CosS(this->baseRotY) * (5.0f * this->actor.speed));

    posB.x = posA.x - Math_CosS(this->baseRotY) * 1000.0f;
    posB.y = posA.y;
    posB.z = posA.z + (Math_SinS(this->baseRotY) * 1000.0f);

    if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &rightWallPos, &rightWallPoly, true, true, false, true,
                                &bgId)) {
        rightWallYawDiff =
            Math_Atan2S_XY(COLPOLY_GET_NORMAL(rightWallPoly->normal.z), COLPOLY_GET_NORMAL(rightWallPoly->normal.x)) -
            this->baseRotY;
        this->distToRightWall = Math_Vec3f_DistXZ(&posA, &rightWallPos);
    } else {
        // There is no wall close enough to Goht's right. In practice, this will never happen inside its boss arena.
        rightWallYawDiff = 0;
        this->distToRightWall = 30000.0f;
    }

    posB.x = (2.0f * posA.x) - posB.x;
    posB.z = (2.0f * posA.z) - posB.z;

    if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &leftWallPos, &leftWallPoly, true, true, false, true,
                                &bgId)) {
        leftWallYawDiff =
            Math_Atan2S_XY(COLPOLY_GET_NORMAL(leftWallPoly->normal.z), COLPOLY_GET_NORMAL(leftWallPoly->normal.x)) -
            this->baseRotY;
        this->distToLeftWall = Math_Vec3f_DistXZ(&posA, &leftWallPos);
    } else {
        // There is no wall close enough to Goht's left. In practice, this will never happen inside its boss arena.
        leftWallYawDiff = 0;
        this->distToLeftWall = 30000.0f;
    }

    if (this->direction == GOHT_DIRECTION_CLOCKWISE) {
        this->distToLeftWall -= 50.0f;
    } else {
        this->distToRightWall -= 50.0f;
    }

    if (this->distToRightWall <= (89100.0f * 0.001f)) {
        offsetFromWall = (this->direction == GOHT_DIRECTION_CLOCKWISE) ? (89100.0f * 0.001f) : (139100.0f * 0.001f);
        this->actor.world.pos.x = (rightWallPos.x + (offsetFromWall * Math_CosS(this->baseRotY))) -
                                  (Math_SinS(this->baseRotY) * (5.0f * this->actor.speed));
        this->actor.world.pos.z = (rightWallPos.z - (offsetFromWall * Math_SinS(this->baseRotY))) -
                                  (Math_CosS(this->baseRotY) * (5.0f * this->actor.speed));
    } else if (this->distToLeftWall <= (89100.0f * 0.001f)) {
        offsetFromWall = (this->direction == GOHT_DIRECTION_CLOCKWISE) ? (139100.0f * 0.001f) : (89100.0f * 0.001f);
        this->actor.world.pos.x = (leftWallPos.x - (offsetFromWall * Math_CosS(this->baseRotY))) -
                                  (Math_SinS(this->baseRotY) * (5.0f * this->actor.speed));
        this->actor.world.pos.z = (leftWallPos.z + (offsetFromWall * Math_SinS(this->baseRotY))) -
                                  (Math_CosS(this->baseRotY) * (5.0f * this->actor.speed));
    }

    if ((this->distToLeftWall < 30000.0f) && (this->distToRightWall < 30000.0f)) {
        // If Goht is running along a straightaway (in other words, if the walls to its side are parallel), then the
        // angles of the walls' normal vectors have the same absolute value; one is positive while the other is
        // negative. As a result, the differences between these two angles and Goht's base rotation will cancel out. If
        // Goht approaches a turn, however, this will not be the case, so the average difference is used to keep Goht
        // parallel to the walls at this point in the turn.
        averagedYawDiff = (s16)(rightWallYawDiff + leftWallYawDiff) >> 1;
        if (((this->direction == GOHT_DIRECTION_CLOCKWISE) && (averagedYawDiff < 0)) ||
            ((this->direction == GOHT_DIRECTION_COUNTERCLOCKWISE) && (averagedYawDiff > 0))) {
            this->baseRotY += averagedYawDiff;
        }
    }
}

void BossHakugin_SetLightningSegmentColliderVertices(GohtLightningSegment* lightningSegment) {
    s32 i;
    Vec3f vertices[3];

    Matrix_SetTranslateRotateYXZ(lightningSegment->pos.x, lightningSegment->pos.y, lightningSegment->pos.z,
                                 &lightningSegment->rot);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

    for (i = 0; i < ARRAY_COUNT(vertices); i++) {
        Matrix_MultVec3f(&sTrisElementsInit[0].dim.vtx[i], &vertices[i]);
    }

    Collider_SetTrisVertices(&lightningSegment->collider, 0, &vertices[0], &vertices[1], &vertices[2]);
}

/**
 * Initializes the lightning segments that Goht shoots at the player. Each segment is initially rotated to point towards
 * the player, but a random offset is added to the rotation so that the lightning doesn't travel in a perfectly straight
 * line. The end result is a chain of lightning segments that generally travels towards the player, but does so in an
 * unpredictable way.
 */
void BossHakugin_AddLightningSegments(BossHakugin* this, Vec3f* startPos, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);
    CollisionPoly* poly;
    Vec3f rootPos;
    Vec3f targetPos;
    Vec3f nextRootPos;
    Vec3f resultPos;
    Vec3f transformedTargetPos;
    Vec3f transformedRootPos;
    GohtLightningSegment* lightningSegment;
    s32 bgId;

    Math_Vec3f_Copy(&rootPos, startPos);
    targetPos.x = player->actor.world.pos.x - (Math_SinS(this->actor.shape.rot.y) * 50.0f);
    targetPos.y = player->actor.world.pos.y + 40.0f;
    targetPos.z = player->actor.world.pos.z - (Math_CosS(this->actor.shape.rot.y) * 50.0f);
    Actor_WorldToActorCoords(&this->actor, &transformedTargetPos, &targetPos);
    Audio_PlaySfx_AtPos(&this->sfxPos, NA_SE_EN_COMMON_THUNDER_THR);

    for (i = 0; i < GOHT_LIGHTNING_SEGMENT_COUNT; i++) {
        lightningSegment = &this->lightningSegments[i];
        Actor_WorldToActorCoords(&this->actor, &transformedRootPos, &rootPos);

        if (transformedTargetPos.z < transformedRootPos.z) {
            lightningSegment->rot.y = this->actor.shape.rot.y + ((s32)Rand_Next() >> 0x13);
        } else {
            lightningSegment->rot.y = Math_Vec3f_Yaw(&rootPos, &targetPos) + ((s32)Rand_Next() >> 0x13);
        }

        lightningSegment->rot.x = Math_Vec3f_Pitch(&rootPos, &targetPos) + ((s32)Rand_Next() >> 0x13);
        lightningSegment->rot.z = 0;
        lightningSegment->pos.x =
            rootPos.x + (80.0f * Math_CosS(lightningSegment->rot.x) * Math_SinS(lightningSegment->rot.y));
        lightningSegment->pos.y = rootPos.y - (80.0f * Math_SinS(lightningSegment->rot.x));
        lightningSegment->pos.z =
            rootPos.z + (80.0f * Math_CosS(lightningSegment->rot.x) * Math_CosS(lightningSegment->rot.y));
        nextRootPos.x = (2.0f * lightningSegment->pos.x) - rootPos.x;
        nextRootPos.y = (2.0f * lightningSegment->pos.y) - rootPos.y;
        nextRootPos.z = (2.0f * lightningSegment->pos.z) - rootPos.z;

        if (BgCheck_EntityLineTest1(&play->colCtx, &rootPos, &nextRootPos, &resultPos, &poly, false, true, false, true,
                                    &bgId)) {
            lightningSegment->rot.x -= 0x2000;
            lightningSegment->pos.x =
                rootPos.x + (80.0f * Math_CosS(lightningSegment->rot.x) * Math_SinS(lightningSegment->rot.y));
            lightningSegment->pos.y = rootPos.y - (80.0f * Math_SinS(lightningSegment->rot.x));
            lightningSegment->pos.z =
                rootPos.z + (80.0f * Math_CosS(lightningSegment->rot.x) * Math_CosS(lightningSegment->rot.y));
            rootPos.x = (2.0f * lightningSegment->pos.x) - rootPos.x;
            rootPos.y = (2.0f * lightningSegment->pos.y) - rootPos.y;
            rootPos.z = (2.0f * lightningSegment->pos.z) - rootPos.z;
        } else {
            Math_Vec3f_Copy(&rootPos, &nextRootPos);
        }

        lightningSegment->alpha = 255 + 20 * (i + 1);
        BossHakugin_SetLightningSegmentColliderVertices(lightningSegment);
        lightningSegment->rot.z = (s32)Rand_Next() >> 0x10;
    }
}

/**
 * Spawns a small rock that flies backwards from the supplied position. This is called repeatedly while Goht is running
 * to make it look like Goht's feet are kicking up rocks.
 */
void BossHakugin_SpawnGravel(BossHakugin* this, Vec3f* pos) {
    s32 i;
    GohtRockEffect* rockEffect;

    for (i = 0; i < GOHT_ROCK_EFFECT_COUNT; i++) {
        rockEffect = &this->rockEffects[i];
        if (rockEffect->timer < 0) {
            VecGeo velocityGeo;

            Math_Vec3f_Copy(&rockEffect->pos, pos);
            velocityGeo.pitch = Rand_S16Offset(0x1000, 0x3000);
            velocityGeo.yaw = this->actor.shape.rot.y + ((s32)Rand_Next() >> 0x12) + 0x8000;
            velocityGeo.r = Rand_ZeroFloat(5.0f) + 7.0f;
            rockEffect->velocity.x = velocityGeo.r * Math_CosS(velocityGeo.pitch) * Math_SinS(velocityGeo.yaw);
            rockEffect->velocity.y = velocityGeo.r * Math_SinS(velocityGeo.pitch);
            rockEffect->velocity.z = velocityGeo.r * Math_CosS(velocityGeo.pitch) * Math_CosS(velocityGeo.yaw);
            rockEffect->pos.x = pos->x + (Rand_ZeroFloat(3.0f) * rockEffect->velocity.x);
            rockEffect->pos.y = pos->y + (Rand_ZeroFloat(3.0f) * rockEffect->velocity.y);
            rockEffect->pos.z = pos->z + (Rand_ZeroFloat(3.0f) * rockEffect->velocity.z);
            rockEffect->scale = (Rand_ZeroFloat(6.0f) + 15.0f) * 0.0001f;
            rockEffect->timer = 40;
            rockEffect->type = GOHT_ROCK_EFFECT_TYPE_BOULDER;
            break;
        }
    }
}

/**
 * This function can spawn a large boulder that flies into the air behind Goht and can damage the player. The boulder
 * can only be spawned if the player isn't too far behind Goht and if Goht's speed is 10.0f or higher. Assuming those
 * conditions have been met, the boulder still only has a 40% chance of spawning.
 *
 * This function will not spawn a boulder if the `preventBoulderSpawnCount` variable on Goht's instance is non-zero.
 * Instead, calling this function will decrement the variable; when a boulder is spawned, this variable is set to 4 to
 * prevent Goht from spawning a lot of boulders in a short period of time.
 */
void BossHakugin_SpawnBoulder(BossHakugin* this, Vec3f* pos) {
    Actor* boulder;
    s32 i;

    if (((this->transformedPlayerPos.z < -200.0f) && (this->actor.speed > 10.0f) &&
         (this->preventBoulderSpawnCount == 0) && (Rand_ZeroOne() < 0.4f))) {
        this->preventBoulderSpawnCount = 4;
    } else {
        if (this->preventBoulderSpawnCount > 0) {
            this->preventBoulderSpawnCount--;
        }
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->boulders); i++) {
        boulder = this->boulders[i];

        if (EN_HAKUROCK_GET_TYPE(boulder) == EN_HAKUROCK_TYPE_NONE) {
            Math_Vec3f_Copy(&boulder->world.pos, pos);
            boulder->params = EN_HAKUROCK_TYPE_BOULDER;
            break;
        }
    }
}

/**
 * This function can spawn a stalactite that falls to the floor and can slow the player to a stop if they Goron Roll
 * into it. The stalactite can only be spawned if Goht's health is below 20. Assuming Goht's health is low enough, the
 * stalactite still only has a 35% chance of spawning.
 *
 * This function will not spawn a stalactite if the `preventStalactiteSpawnCount` variable on Goht's instance is
 * non-zero. Instead, calling this function will decrement the variable; when a stalactite is spawned, this variable is
 * set to 4 to prevent Goht from spawning a lot of stalactites in a short period of time.
 */
void BossHakugin_SpawnStalactite(BossHakugin* this) {
    Actor* stalactite;
    s32 i;

    if ((this->preventStalactiteSpawnCount == 0) && (this->actor.colChkInfo.health < 20) && (Rand_ZeroOne() < 0.35f)) {
        this->preventStalactiteSpawnCount = 4;
    } else {
        if (this->preventStalactiteSpawnCount > 0) {
            this->preventStalactiteSpawnCount--;
        }
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->stalactites); i++) {
        stalactite = this->stalactites[i];

        if (EN_HAKUROCK_GET_TYPE(stalactite) == EN_HAKUROCK_TYPE_NONE) {
            stalactite->params = EN_HAKUROCK_TYPE_FALLING_STALACTITE;
            return;
        }
    }
}

/**
 * This function can spawn a bomb that flies into the air behind Goht. The bomb can only be spawned if Goht's health is
 * below 10 and if Goht's speed is 10.0f or higher. Assuming those conditions have been met, the bomb still only has a
 * 35% chance of spawning.
 *
 * This function will not spawn a bomb if the `preventBombSpawnCount` variable on Goht's instance is non-zero. Instead,
 * calling this function will decrement the variable; when a bomb is spawned, this variable is set to 4 to prevent Goht
 * from spawning a lot of bombs in a short period of time.
 */
void BossHakugin_SpawnBomb(BossHakugin* this, PlayState* play) {
    EnBom* bomb;
    s16 yawDiff;

    if ((this->actor.speed > 10.0f) && ((s32)this->actor.colChkInfo.health < 10) &&
        (this->preventBombSpawnCount == 0) && (Rand_ZeroOne() < 0.35f)) {
        this->preventBombSpawnCount = 4;
    } else {
        if (this->preventBombSpawnCount > 0) {
            this->preventBombSpawnCount--;
        }
        return;
    }

    bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM,
                               this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_THORAX].dim.worldSphere.center.x -
                                   (100.0f * Math_SinS(this->actor.shape.rot.y)),
                               this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_THORAX].dim.worldSphere.center.y +
                                   100.0f,
                               this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_THORAX].dim.worldSphere.center.z -
                                   (Math_CosS(this->actor.shape.rot.y) * 100.0f),
                               BOMB_EXPLOSIVE_TYPE_BOMB, 0, 0, BOMB_TYPE_BODY);

    if (bomb != NULL) {
        yawDiff = (this->actor.yawTowardsPlayer - this->actor.shape.rot.y) - 0x8000;
        if (yawDiff > 0x2000) {
            bomb->actor.world.rot.y = this->actor.shape.rot.y + 0xA000;
        } else if (yawDiff < -0x2000) {
            bomb->actor.world.rot.y = this->actor.shape.rot.y + 0x6000;
        } else {
            bomb->actor.world.rot.y = this->actor.yawTowardsPlayer;
        }

        bomb->timer = (Rand_Next() >> 0x1C) + 17;
        bomb->actor.velocity.y = 2.0f;
        bomb->actor.speed = this->actor.xzDistToPlayer * 0.01f;
        bomb->actor.speed = CLAMP(bomb->actor.speed, 6.0f, 12.0f);

        Actor_SetScale(&bomb->actor, 0.02f);
    }
}

/**
 * This function is responsible for spawning the smoke effects that billow from various body parts as Goht takes damage.
 * It also spawns the related electrical effect from the same body parts, which is handled by `EffectSsFhgFlash`. These
 * effects only begin to spawn when Goht has 15 or lower health (it starts with 30), and effects will appear on
 * different body parts for every 3 health that Goht loses after that.
 */
void BossHakugin_AddMalfunctionEffects(BossHakugin* this, PlayState* play) {
    GohtMalfunctionEffect* malfunctionEffect;
    Vec3f* currentPos;
    Vec3f* prevPos;
    Vec3f diff;
    s32 effectIndex;
    s32 effectCount;
    s32 type;
    s32 i;
    s32 pad;

    // The `effectIndex` variable is used here to represent the previous frame's effect index. The effect index is
    // decreased by 3 every frame, so we add 3 here to counteract that.
    effectIndex = this->malfunctionEffectIndex + 3;
    if (effectIndex >= GOHT_MALFUNCTION_EFFECTS_PER_TYPE) {
        effectIndex = 0;
    }

    for (type = 0; type < GOHT_MALFUNCTION_NUM_TYPES; type++) {
        if (((15 - (3 * type)) < this->actor.colChkInfo.health) ||
            !(GOHT_LIMB_DRAW_FLAG(sMalfunctionTypeToLimbIndex[type]) & this->limbDrawFlags)) {
            break;
        }

        prevPos = &this->malfunctionEffects[type][effectIndex].pos;
        currentPos = &this->malfunctionEffects[type][this->malfunctionEffectIndex].pos;
        Math_Vec3f_Diff(prevPos, currentPos, &diff);

        // This is done to counteract the fact that we add 3.5f to the malfunction effect's y-coordinate in
        // `BossHakugin_UpdateMalfunctionEffects`, which is called once before the effect is ever rendered. If we didn't
        // do this, the effect would appear to spawn a little "too high", though it would be very hard to notice.
        diff.y -= 3.5f;

        // Depending on the difference between where the effect is on this frame compared to where it was on the
        // previous frame, we can spawn 1 to 3 effects in the loop below. This is done so that, if Goht is running
        // around, the smoke will "trail" behind him.
        effectCount = Math3D_Vec3fMagnitude(&diff) / 10.0f;
        if (effectCount > 1) {
            effectCount = CLAMP_MAX(effectCount, 3);
            Math_Vec3f_Scale(&diff, 1.0f / effectCount);
        } else {
            effectCount = 1;
        }

        for (i = 0; i < effectCount; i++) {
            malfunctionEffect = &this->malfunctionEffects[type][this->malfunctionEffectIndex + i];

            malfunctionEffect->pos.x = Rand_CenteredFloat(20.0f) + (currentPos->x + (diff.x * i));
            malfunctionEffect->pos.y = Rand_CenteredFloat(20.0f) + (currentPos->y + (diff.y * i));
            malfunctionEffect->pos.z = Rand_CenteredFloat(20.0f) + (currentPos->z + (diff.z * i));
            malfunctionEffect->scaleXY = 0.01f;
            malfunctionEffect->alpha = 150;
            malfunctionEffect->timer = 5 - i;
        }

        if ((play->gameplayFrames % 2) != 0) {
            EffectSsFhgFlash_SpawnShock(play, &this->actor,
                                        &this->malfunctionEffects[type][this->malfunctionEffectIndex].pos, 250,
                                        FHGFLASH_SHOCK_GOHT_BACK_LEFT_THIGH + type);
        }
    }

    // The `effectIndex` variable is used here to represent the value `malfunctionEffectIndex` should have on the next
    // frame. Because of the loop above, up to three effects can be initialized for any given `malfunctionEffectIndex`,
    // so we subtract 3 from the current index to make room for those three potential effects.
    effectIndex = this->malfunctionEffectIndex - 3;
    if (effectIndex < 0) {
        this->malfunctionEffectIndex = GOHT_MALFUNCTION_EFFECTS_PER_TYPE - 3;
    } else {
        this->malfunctionEffectIndex = effectIndex;
    }
}

/**
 * This function is responsible for multiple things which are all related to Goht charging up its electric ball or
 * lightning attacks. Actions that want Goht to charge up an attack should call this function repeatedly on every frame.
 * - If the charge up timer is 5, this function will make Goht slowly raise its head and increase the scale of its
 *   charging lightning. When the head reaches its target rotation, the charge up timer will be decremented to signal
 *   that we should move onto the next phase.
 * - If the charge up timer is above 0 but below 5, this function will do nothing besides decrement the charge up timer.
 *   It will also do that if the charge up timer is above 5, though this never happens in the final game.
 * - If the charge up timer is 0, this function will make Goht lower its head quickly. Once the head reaches its target
 *   rotation, this function will return true.
 *
 * The return value of this function is used to indicate whether Goht should "fire off" its attack or not. When it
 * returns true, Goht should unleash its charged electric ball or lightning attack. When it returns false, this
 * indicates that Goht should keep charging its attack.
 */
s32 BossHakugin_ChargeUpAttack(BossHakugin* this) {
    s16 rand = (Rand_Next() >> 0x12) + 0x4000;

    this->lightOrbRotZ += rand;
    if (this->chargeUpTimer == 5) {
        if (!Math_SmoothStepToS(&this->frontHalfRotZ, -0x900, 4, 0x200, 0x80)) {
            this->chargeUpTimer--;
        }

        this->chargingLightOrbScale = (this->frontHalfRotZ + 0x480) * (-30.0f / 0x480);
        this->chargingLightOrbScale = CLAMP_MIN(this->chargingLightOrbScale, 0.001f);

        this->chargingLightningScale = (this->frontHalfRotZ - 0x700) * (-0.62f / 0xB80);
        this->chargingLightningScale = CLAMP_MAX(this->chargingLightningScale, 0.62f);

        if (this->chargingLightningScale > 0.0f) {
            this->chargingLightningTranslateZ = -100.0f - (((0.62f / this->chargingLightningScale) - 1.0f) * 200.0f);
        } else {
            this->chargingLightningTranslateZ = -200.0f;
        }
    } else if (this->chargeUpTimer > 0) {
        this->chargeUpTimer--;
    } else if (Math_ScaledStepToS(&this->frontHalfRotZ, 0x700, 0x380)) {
        this->chargingLightningScale = (0x700 - this->frontHalfRotZ) / 4096.0f * 0.62f;
        return true;
    }

    return false;
}

/**
 * Handles functionality common to all of Goht's running actions, including:
 * - Playing Goht's footstep sounds
 * - Requesting a quake and rumble when the player is close to Goht
 * - Spawning boulders, stalactites, and bombs under certain conditions to damage or impede the player
 * - Spwaning dust and gravel effects at Goht's feet whenever they hit the ground
 */
void BossHakugin_RunUpdateCommon(BossHakugin* this, PlayState* play) {
    Vec3f velocity;
    Vec3f pos;
    f32 quakeVerticalMag;
    s32 i;

    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_FOOTSTEP_OLD);
        Math_Vec3f_Copy(&pos, &this->bodyPartsPos[sLimbToBodyParts[GOHT_LIMB_FRONT_LEFT_HOOF]]);
        if (this->actor.xzDistToPlayer < 1500.0f) {
            quakeVerticalMag = (1500.0f - this->actor.xzDistToPlayer) * (1.0f / 1500.0f);
            BossHakugin_RequestQuakeAndRumble(this, play, 17000, 6.5f * quakeVerticalMag, 10);
        }

        BossHakugin_SpawnStalactite(this);
    } else if (Animation_OnFrame(&this->skelAnime, 2.0f)) {
        Math_Vec3f_Copy(&pos, &this->bodyPartsPos[sLimbToBodyParts[GOHT_LIMB_BACK_LEFT_HOOF]]);
        BossHakugin_SpawnBoulder(this, &pos);
    } else if (Animation_OnFrame(&this->skelAnime, 3.0f)) {
        Math_Vec3f_Copy(&pos, &this->bodyPartsPos[sLimbToBodyParts[GOHT_LIMB_FRONT_RIGHT_HOOF]]);
        BossHakugin_SpawnBomb(this, play);
    } else if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
        Math_Vec3f_Copy(&pos, &this->bodyPartsPos[sLimbToBodyParts[GOHT_LIMB_BACK_RIGHT_HOOF]]);
        BossHakugin_SpawnBoulder(this, &pos);
    } else {
        return;
    }

    velocity.x = Math_SinS(this->actor.shape.rot.y) * -1.0f;
    velocity.y = 1.0f;
    velocity.z = Math_CosS(this->actor.shape.rot.y) * -1.0f;
    pos.x += 40.0f * velocity.x;
    pos.y += 5.0f;
    pos.z += 40.0f * velocity.z;
    func_800B12F0(play, &pos, &velocity, &gZeroVec3f, Rand_S16Offset(650, 100), Rand_S16Offset(20, 10), 30);

    for (i = 0; i < 4; i++) {
        BossHakugin_SpawnGravel(this, &pos);
    }
}

/**
 * Spawns steam effects that are used when Goht is thawed out in its intro cutscene. The steam will usually spawn in
 * random locations around Goht's body, however if `steamFollowsMeltingIce` is true, then the steam will spawn with
 * increasingly-lower y-coordinates as the ice block that surrounds Goht melts and shrinks downwards.
 */
void BossHakugin_SpawnSteam(BossHakugin* this, PlayState* play, s32 steamFollowsMeltingIce) {
    Vec3f velocity;
    Vec3f pos;

    if (steamFollowsMeltingIce) {
        velocity.y = (this->timer - 30) * (1 / 70.0f);
    } else {
        velocity.y = 0.1f;
    }

    velocity.x = Rand_CenteredFloat(2.0f) * velocity.y;
    velocity.z = Rand_CenteredFloat(2.0f) * velocity.y;

    pos.x = ((velocity.x >= 0.0f ? 1.0f : -1.0f) * (Rand_ZeroFloat(20.0f) + 5.0f) * 4.0f) + this->iceCollider.dim.pos.x;
    pos.z = ((velocity.z >= 0.0f ? 1.0f : -1.0f) * (Rand_ZeroFloat(20.0f) + 5.0f) * 4.0f) + this->iceCollider.dim.pos.z;

    if (steamFollowsMeltingIce) {
        pos.y = this->iceCollider.dim.pos.y + (this->iceCollider.dim.height * velocity.y);
    } else {
        pos.y = (Rand_ZeroFloat(this->iceCollider.dim.height) * 0.8f) + this->iceCollider.dim.pos.y;
    }

    velocity.y += 0.8f;
    EffectSsIceSmoke_Spawn(play, &pos, &velocity, &gZeroVec3f, 600);
}

/**
 * Returns true if Goht should stop running and either wait for the player to get in view (to shoot lightning at them)
 * or wait 150 frames (after which, Goht will turn around and charge them).
 */
s32 BossHakugin_ShouldWait(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;
    s32 absYawDiff;
    f32 posZ;
    f32 posX;

    if (this->transformedPlayerPos.z > 0.0f) {
        return false;
    }

    yawDiff = this->baseRotY - Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
    absYawDiff = ABS_ALT(yawDiff);

    if (absYawDiff < 0x4000) {
        if (absYawDiff < 0x1800) {
            this->lightningHitSomething = false;
        }
        return false;
    }

    // If Goht hit the player with lightning the last time it waited, then the only way for `lightningHitSomething` to
    // be false (in other words, the only way for us continue in this function) is because it was set to false by the
    // code above in a previous call to this function; this only happens if the difference between Goht's base yaw and
    // the active cam's yaw is small enough.
    if (this->lightningHitSomething == true) {
        return false;
    }

    posX = this->actor.world.pos.x * this->direction;
    posZ = this->actor.world.pos.z * this->direction;

    /**
     * Goht's arena is a square with rounded corners. If the player is standing in one of the "sides" of the square, and
     * Goht is standing near one of the opposite corners (which specific corner depends on whether Goht is running
     * clockwise or counterclockwise), then Goht will wait. The diagram below illustrates this; if the player is
     * standing in the box labeled P, and Goht is standing in the box G1 (if Goht is running counterclockwise) or G2 (if
     * Goht is running clockwise), then Goht will wait:
     *    _____________________
     *   /    |           |    \
     *  /     |     P     |     \
     * |      |___________|      |
     * |      |           |      |
     * |______|           |______|
     * |      |           |      |
     * |  G1  |           |  G2  |
     * |      |‾‾‾‾‾‾‾‾‾‾‾|      |
     *  \     |           |     /
     *   \____|___________|____/
     */
    if (((player->actor.world.pos.x > 1200.0f) && (player->actor.world.pos.z < 1200.0f) &&
         (player->actor.world.pos.z > -1200.0f) && (this->actor.world.pos.x < 0.0f) && (posZ > 1200.0f)) ||
        ((player->actor.world.pos.x < -1200.0f) && (player->actor.world.pos.z < 1200.0f) &&
         (player->actor.world.pos.z > -1200.0f) && (this->actor.world.pos.x > 0.0f) && (posZ < -1200.0f)) ||
        ((player->actor.world.pos.z > 1200.0f) && (player->actor.world.pos.x < 1200.0f) &&
         (player->actor.world.pos.x > -1200.0f) && (this->actor.world.pos.z < 0.0f) && (posX < -1200.0f)) ||
        ((player->actor.world.pos.z < -1200.0f) && (player->actor.world.pos.x < 1200.0f) &&
         (player->actor.world.pos.x > -1200.0f) && (this->actor.world.pos.z > 0.0f) && (posX > 1200.0f))) {
        return true;
    }

    return false;
}

/**
 * Spawns ice effects from Goht's body parts that fly off in random directions. Not to be confused with
 * `BossHakugin_IntroCutsceneThaw`, which is exclusively used during the intro cutscene; this function is used during
 * gameplay when Goht is hit by an Ice Arrow.
 */
void BossHakugin_Thaw(BossHakugin* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, GOHT_BODYPART_MAX, 3, 0.7f, 0.5f);
    }
}

/**
 * Handles various things related to damage effects, like setting the effect alpha and scale and spawning light rays at
 * the appropriate position when Goht is hit by Light Arrows or by a sword beam.
 */
void BossHakugin_UpdateDrawDmgEffect(BossHakugin* this, PlayState* play, s32 colliderIndex) {
    if (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_FIRE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 3.0f;
        this->drawDmgEffScale = 2.5f;
    } else if (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_LIGHT_ORB) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffAlpha = 3.0f;
        this->drawDmgEffScale = 2.5f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                    this->bodyCollider.elements[colliderIndex].base.acDmgInfo.hitPos.x,
                    this->bodyCollider.elements[colliderIndex].base.acDmgInfo.hitPos.y,
                    this->bodyCollider.elements[colliderIndex].base.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
    } else if (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_FREEZE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        this->drawDmgEffScale = 2.5f;
        this->drawDmgEffFrozenSteamScale = 3.75f;
        this->drawDmgEffAlpha = 1.0f;
    } else if (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_BLUE_LIGHT_ORB) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS;
        this->drawDmgEffScale = 2.5f;
        this->drawDmgEffAlpha = 3.0f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                    this->bodyCollider.elements[colliderIndex].base.acDmgInfo.hitPos.x,
                    this->bodyCollider.elements[colliderIndex].base.acDmgInfo.hitPos.y,
                    this->bodyCollider.elements[colliderIndex].base.acDmgInfo.hitPos.z, 0, 0, 3,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
    }
}

/**
 * The intention behind this function is to call it repeatedly to gradually move the subcam around and rotate it to its
 * target rotation. The amount by which the position of the subcam's eye is updated per call depends on `posStep`, and
 * the amount by which to rotate the subcam towards its target per call is determined by `angleStep`.
 */
void BossHakugin_UpdateSubCam(BossHakugin* this, PlayState* play, f32 posStep, s16 angleStep) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f diff;
    Vec3f targetDiff;

    Math_Vec3f_Diff(&subCam->at, &subCam->eye, &diff);
    Math_Vec3f_StepTo(&subCam->eye, &this->subCamEye, posStep);

    if (BossHakugin_Vec3fNormalize(&diff)) {
        targetDiff.x = Math_CosS(this->subCamRot.x) * Math_SinS(this->subCamRot.y);
        targetDiff.y = Math_SinS(this->subCamRot.x);
        targetDiff.z = Math_CosS(this->subCamRot.x) * Math_CosS(this->subCamRot.y);
        BossHakugin_StepVector(&diff, &targetDiff, BINANG_TO_RAD(angleStep));
    }

    subCam->at.x = subCam->eye.x + (50.0f * diff.x);
    subCam->at.y = subCam->eye.y + (50.0f * diff.y);
    subCam->at.z = subCam->eye.z + (50.0f * diff.z);
    Play_SetCameraAtEye(play, this->subCamId, &subCam->at, &subCam->eye);
}

void BossHakugin_SetupEntranceCutscene(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f subCamAt;

    Animation_PlayOnce(&this->skelAnime, &gGohtThawAndBreakWallAnim);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);

    subCamAt.x = player->actor.focus.pos.x;
    subCamAt.y = player->actor.focus.pos.y;
    subCamAt.z = player->actor.focus.pos.z;

    this->subCamEye.x = player->actor.focus.pos.x + (Math_SinS(player->actor.shape.rot.y) * 70.0f);
    this->subCamEye.y = player->actor.focus.pos.y + 20.0f;
    this->subCamEye.z = player->actor.focus.pos.z + (Math_CosS(player->actor.shape.rot.y) * 70.0f);

    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &this->subCamEye);

    this->subCamRot.x = Math_Atan2S_XY(70.0f, -20.0f);
    this->subCamRot.y = player->actor.shape.rot.y + 0x8000;
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_21);
    this->timer = 0;
    this->actionFunc = BossHakugin_EntranceCutscene;
}

/**
 * Handles the cutscene that plays when the player first enters Goht's arena which shows it encased in ice. After 120
 * frames, the cutscene ends and control is returned to the player.
 */
void BossHakugin_EntranceCutscene(BossHakugin* this, PlayState* play) {
    static Vec3f sSubCamAt = { 492.0f, 28.0f, -1478.0f };

    this->timer++;
    if (this->timer == 30) {
        this->subCamEye.x = 417.0f;
        this->subCamEye.y = 40.0f;
        this->subCamEye.z = -1417.0f;
        Play_SetCameraAtEye(play, this->subCamId, &sSubCamAt, &this->subCamEye);
        func_800BE33C(&this->subCamEye, &sSubCamAt, &this->subCamRot, true);
    } else if (this->timer == 50) {
        this->subCamRot.x = 0x1500;
    } else if (this->timer == 120) {
        CutsceneManager_Stop(this->actor.csId);
        SET_EVENTINF(EVENTINF_ENTR_CS_WATCHED_GOHT);
        BossHakugin_SetupFrozenBeforeFight(this);
        return;
    }

    if (this->timer > 30) {
        BossHakugin_UpdateSubCam(this, play, 5.0f, 0xC0);
    }
}

void BossHakugin_SetupFrozenBeforeFight(BossHakugin* this) {
    this->timer = 0;
    this->actionFunc = BossHakugin_FrozenBeforeFight;
}

/**
 * Waits around until the player strikes the ice block with a Fire Arrow, which starts Goht's intro cutscene. In this
 * state, Goht's ice block can freeze the player if they touch it; if this happens, this function will ensure the player
 * can't be frozen by the ice block again until 40 frames pass.
 */
void BossHakugin_FrozenBeforeFight(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags2 & PLAYER_STATE2_4000)) {
        DECR(this->timer);
    }

    if (GameInteractor_Should(VB_GOHT_UNFREEZE,
                              (this->iceCollider.base.acFlags & AC_HIT) &&
                                  (this->iceCollider.elem.acHitElem->atDmgInfo.dmgFlags == DMG_FIRE_ARROW))) {
        this->iceCollider.base.atFlags &= ~AT_HIT;
        this->iceCollider.base.acFlags &= ~AC_HIT;
        this->iceCollider.base.ocFlags1 &= ~OC1_HIT;
        BossHakugin_SetupCutsceneStart(this);
        return;
    }

    if (this->iceCollider.base.atFlags & AT_HIT) {
        this->timer = 40;
        this->iceCollider.base.atFlags &= ~AT_HIT;
    }

    if (this->timer == 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->iceCollider.base);
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->iceCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->iceCollider.base);
    this->disableBodyCollidersTimer = 20;
}

void BossHakugin_SetupIntroCutsceneThaw(BossHakugin* this) {
    this->timer = 100;
    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
    Play_EnableMotionBlur(140);
    this->actionFunc = BossHakugin_IntroCutsceneThaw;
}

/**
 * Handles the first part of Goht's intro cutscene which shows close ups of various parts of Goht while the ice
 * surrounding it melts and spawns steam. After 100 frames, this function switches to the action where Goht wakes up.
 */
void BossHakugin_IntroCutsceneThaw(BossHakugin* this, PlayState* play) {
    Vec3f subCamEye;
    Vec3f subCamAt;
    s32 alpha;

    this->timer--;
    if ((this->timer >= 30) && ((this->timer % 2) != 0)) {
        BossHakugin_SpawnSteam(this, play, true);
    } else if ((this->timer % 4) == 0) {
        BossHakugin_SpawnSteam(this, play, false);
    }

    if (this->timer == 99) {
        subCamAt.x = 479.0f;
        subCamAt.y = 181.0f;
        subCamAt.z = -1637.0f;
        subCamEye.x = 328.0f;
        subCamEye.y = 164.0f;
        subCamEye.z = -1693.0f;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    } else if (this->timer == 75) {
        subCamAt.x = 693.0f;
        subCamAt.y = 162.0f;
        subCamAt.z = -1550.0f;
        subCamEye.x = 638.0f;
        subCamEye.y = 200.0f;
        subCamEye.z = -1407.0f;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    } else if (this->timer == 50) {
        subCamAt.x = 825.0f;
        subCamAt.y = 123.0f;
        subCamAt.z = -1637.0f;
        subCamEye.x = 734.0f;
        subCamEye.y = 145.0f;
        subCamEye.z = -1762.0f;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    } else if (this->timer == 25) {
        subCamAt.x = 711.0f;
        subCamAt.y = 76.0f;
        subCamAt.z = -1602.0f;
        subCamEye.x = 634.0f;
        subCamEye.y = 54.0f;
        subCamEye.z = -1735.0f;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    }

    this->iceScaleY -= 0.027f;
    alpha = (s32)(this->timer * 5.1f) + 50;
    this->iceAlpha = CLAMP_MAX(alpha, 255);

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);

    if (this->timer == 0) {
        this->iceAlpha = 0;
        BossHakugin_SetupIntroCutsceneWakeUp(this, play);
    }
}

void BossHakugin_SetupIntroCutsceneWakeUp(BossHakugin* this, PlayState* play) {
    static Vec3f sSubCamAt = { 894.0f, 176.0f, -1600.0f };
    static Vec3f sSubCamEye = { 972.0f, 176.0f, -1600.0f };
    Player* player = GET_PLAYER(play);

    Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_STEAM_DEMO_UP_OLD);
    Play_SetCameraAtEye(play, this->subCamId, &sSubCamAt, &sSubCamEye);
    this->subCamRot.x = -0x1F00;
    this->subCamRot.y = -0x4000;
    this->timer = 70;
    this->subCamEye.x = 680.0f;
    this->subCamEye.y = 370.0f;
    this->subCamEye.z = -1600.0f;
    player->actor.world.pos.x = 250.0f;
    player->actor.world.pos.z = -1560.0f;
    player->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;
    player->yaw = player->actor.world.rot.y = player->actor.shape.rot.y;
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_131);
    this->actionFunc = BossHakugin_IntroCutsceneWakeUp;
}

/**
 * Handles the second part of Goht's intro cutscene where Goht wakes up from being frozen. For the first 70 frames of
 * the cutscene, the camera travels up Goht's back until the player is in view in front of Goht. Afterwards, the camera
 * focuses on Goht head-on as it looks around and destroys the wall of stalactites to its sides; this part of the
 * cutscene is where the boss theme begins playing and Goht's title card appears.
 */
void BossHakugin_IntroCutsceneWakeUp(BossHakugin* this, PlayState* play) {
    static Vec3f sSubCamAt = { 377.0f, 140.0f, -1600.0f };
    static Vec3f sSubCamEye = { 282.0f, 108.0f, -1600.0f };
    s32 i;

    this->timer--;
    if (this->timer > 0) {
        BossHakugin_UpdateSubCam(this, play, 7.0f, 0xC0);
    } else if (this->timer == 0) {
        Play_SetCameraAtEye(play, this->subCamId, &sSubCamAt, &sSubCamEye);
        this->subCamRot.y = 0x4000;
        this->subCamRot.x = Math_Atan2S_XY(95.0f, 32.0f);
        this->subCamEye.x = 232.0f;
        this->subCamEye.y = 43.0f;
        this->subCamEye.z = -1600.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_STEAMS_DEMO_OLD);
    } else {
        BossHakugin_UpdateSubCam(this, play, 1.0f, 0xC0);

        if (Animation_OnFrame(&this->skelAnime, 20.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_STEAM_DEMO_UP_OLD);
        }

        if (Animation_OnFrame(&this->skelAnime, 45.0f)) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_BOSS | SEQ_FLAG_ASYNC);
        }

        if (Animation_OnFrame(&this->skelAnime, 65.0f)) {
            TitleCard_InitBossName(&play->state, &play->actorCtx.titleCtx, Lib_SegmentedToVirtual(&gGohtTitleCardTex),
                                   160, 180, 128, 40);
        }

        if (Animation_OnFrame(&this->skelAnime, 86.0f)) {
            Audio_PlaySfx_AtPos(&this->sfxPos, NA_SE_EN_ICEB_CRY_OLD);
        }

        if (Animation_OnFrame(&this->skelAnime, 90.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DEMO_BREAK);
            for (i = 6; i < ARRAY_COUNT(this->stalactites); i++) {
                this->stalactites[i]->params = EN_HAKUROCK_TYPE_NONE;
            }
        }

        if (SkelAnime_Update(&this->skelAnime)) {
            BossHakugin_SetupIntroCutsceneRun(this, play);
            return;
        }
    }

    if (((this->timer + 12) >= 0) && ((this->timer % 4) == 0)) {
        BossHakugin_SpawnSteam(this, play, false);
    }
}

void BossHakugin_SetupIntroCutsceneRun(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f subCamAt;

    this->subCamEye.x = player->actor.world.pos.x - 150.0f;
    this->subCamEye.y = player->actor.world.pos.y + 30.0f;
    this->subCamEye.z = player->actor.world.pos.z - 30.0f;
    subCamAt.x = this->actor.world.pos.x - 100.0f;
    subCamAt.y = this->actor.world.pos.y + 100.0f;
    subCamAt.z = this->actor.world.pos.z;
    func_800BE33C(&this->subCamEye, &subCamAt, &this->subCamRot, true);
    Animation_Change(&this->skelAnime, &gGohtRunAnim, 1.5f, 0.0f, 0.0f, ANIMMODE_LOOP, -3.0f);
    this->timer = 0;
    this->actor.speed = 5.0f;
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
    player->stateFlags1 |= PLAYER_STATE1_20;
    play->actorCtx.isOverrideInputOn = true;
    this->actionFunc = BossHakugin_IntroCutsceneRun;
}

/**
 * Handles the final part of Goht's intro cutscene where Goht runs forward and smashes through the wall of stalactites
 * in front of it. This function manually overrides the control stick input to make the player run away from Goht, and
 * it manually knocks the player back when Goht collides with them to make it appear as if Goht is running them over.
 */
void BossHakugin_IntroCutsceneRun(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f subCamAt;
    f32 controlStickMagnitude;
    s32 i;

    Math_ScaledStepToS(&this->frontHalfRotZ, 0x800, 0x80);
    Math_StepToF(&this->actor.speed, 25.0f, 0.5f);
    this->skelAnime.playSpeed = (this->actor.speed / 32.0f) + 0.5f;
    BossHakugin_RunUpdateCommon(this, play);

    // Setting this to 10 prevents Goht from spawning boulders during the cutscene and shortly afterwards, since
    // `BossHakugin_SpawnBoulder` is called fewer than 10 times during the cutscene.
    this->preventBoulderSpawnCount = 10;

    SkelAnime_Update(&this->skelAnime);
    this->timer++;
    if (this->timer < 8) {
        BossHakugin_UpdateSubCam(this, play, 25.0f, 0x120);
    } else if (this->timer == 22) {
        this->subCamEye.x = subCam->eye.x;
        this->subCamEye.y = subCam->eye.y;
        this->subCamEye.z = subCam->eye.z + 450.0f;
    } else if (this->timer == 62) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        CutsceneManager_Stop(this->actor.csId);
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_GOHT);
        Play_DisableMotionBlur();
        BossHakugin_SetupRun(this);
        return;
    } else if (this->timer > 22) {
        subCamAt.x = this->actor.world.pos.x;
        subCamAt.y = this->actor.world.pos.y + 150.0f;
        subCamAt.z = this->actor.world.pos.z;
        Math_Vec3f_StepTo(&subCam->eye, &this->subCamEye, 35.0f);
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCam->eye);

        if (this->timer == 42) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST1_DEMO_WALL);
            for (i = 0; i < 6; i++) {
                this->stalactites[i]->params = EN_HAKUROCK_TYPE_NONE;
            }
        }
    }

    if (this->bodyCollider.base.atFlags & AT_HIT) {
        func_800B8D50(play, &this->actor, 10.0f, 0, 6.0f, 0);
    } else {
        play->actorCtx.isOverrideInputOn = true;
    }

    if (this->timer > 4) {
        controlStickMagnitude = (this->timer - 4) * 5.0f;
        controlStickMagnitude = CLAMP_MAX(controlStickMagnitude, 60.0f);
        Actor_SetControlStickData(play, &play->actorCtx.overrideInput, controlStickMagnitude, -0x4000);
    }
}

void BossHakugin_SetupRun(BossHakugin* this) {
    Animation_Change(&this->skelAnime, &gGohtRunAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -5.0f);
    this->runOffsetRot = 0;
    this->chargingLightOrbScale = 0.0f;
    this->targetRotY = Rand_CenteredFloat(3072.0f);
    this->timer = 40;
    this->targetSpeed = 16.0f;
    this->actionFunc = BossHakugin_Run;
}

/**
 * This function handles everything for when Goht is running in a circle around the boss arena, including:
 * - Charging up and firing an electric ball when Goht is hit by Goron Spikes
 * - Checking if Goht should wait around instead running and transitioning it to the waiting state if it should
 * - Changing Goht's target speed and smooth-stepping its actual speed to match the target speed
 * - Changing the play speed of Goht's running animation depending on its actual speed
 * - Giving Goht's run a slightly-random drift so that it doesn't run perfectly straight
 * - Checking if Goht should charge the player and transitioning it to the charging state if it should
 */
void BossHakugin_Run(BossHakugin* this, PlayState* play) {
    s16 yawDiff;

    if (this->disableBodyCollidersTimer == 0) {
        this->bodyCollider.base.atFlags |= AT_ON;
    }

    if (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_CHARGE) {
        if (BossHakugin_ChargeUpAttack(this)) {
            Math_Vec3f_Copy(&this->electricBallPos[0], &this->chargingLightningPos);
            this->electricBallSpeed = this->actor.speed + 100.0f;
            this->electricBallRot.x = Math_CosS(0xA00) * Math_SinS(this->actor.shape.rot.y);
            this->electricBallRot.y = Math_SinS(0xA00);
            this->electricBallRot.z = Math_CosS(0xA00) * Math_CosS(this->actor.shape.rot.y);
            this->electricBallState = GOHT_ELECTRIC_BALL_STATE_FLY_FORWARD;
            this->chargingLightOrbScale = 0.0f;
            Audio_PlaySfx_AtPos(&this->sfxPos, NA_SE_EN_COMMON_E_BALL_THR);
            this->electricBallCollider.base.atFlags |= AT_ON;
        }
    } else {
        Math_ScaledStepToS(&this->frontHalfRotZ, 0, 0x100);
    }

    if (BossHakugin_ShouldWait(this, play)) {
        BossHakugin_SetupWait(this, play);
    } else {
        // The `damagedSpeedUpCounter` increases by 35 every time the player hits Goht (when it's not downed) and
        // decreases by 1 every frame. Thus, if the player repeatedly hits Goht in a short amount of time, this will
        // exceed 55; in this circumstance, Goht will run twice as fast as it normally does to get away from the player.
        if (this->damagedSpeedUpCounter > 55) {
            this->damagedSpeedUpCounter = 0;
            this->timer = 40;
            this->targetSpeed = 27.0f;
        }

        if ((this->timer == 40) &&
            (Math_SmoothStepToF(&this->actor.speed, this->targetSpeed, 0.15f, 1.0f, 0.1f) < 0.05f)) {
            this->timer--;
        } else if (this->timer != 40) {
            this->timer--;
            if (this->timer == 0) {
                if (this->actor.xzDistToPlayer > 1500.0f) {
                    this->targetSpeed = 12.5f;
                } else if (this->transformedPlayerPos.z > 0.0f) {
                    this->targetSpeed = 21.0f;
                } else {
                    this->targetSpeed = 14.5f;
                }

                this->targetSpeed += (30 - this->actor.colChkInfo.health) * (1.0f / 30.0f);
                this->timer = 40;
            }
        }

        this->skelAnime.playSpeed = (this->actor.speed / 32.0f) + 0.5f;
        SkelAnime_Update(&this->skelAnime);
        BossHakugin_RunUpdateCommon(this, play);

        if ((this->distToRightWall < (this->distToLeftWall * 0.5f)) || (this->distToRightWall < (89100.0f * 0.001f))) {
            this->targetRotY = (Rand_ZeroFloat(0.4f) + 0.6f) * 1536.0f;
        } else if ((this->distToLeftWall < (this->distToRightWall * 0.5f)) ||
                   (this->distToLeftWall < (89100.0f * 0.001f))) {
            this->targetRotY = -((Rand_ZeroFloat(0.4f) + 0.6f) * 1536.0f);
        } else if ((this->runOffsetRot == this->targetRotY) && (Rand_ZeroOne() < 0.005f)) {
            if (this->targetRotY > 0) {
                this->targetRotY = -((Rand_ZeroFloat(0.4f) + 0.6f) * 1536.0f);
            } else {
                this->targetRotY = ((Rand_ZeroFloat(0.4f) + 0.6f) * 1536.0f);
            }
        }

        Math_SmoothStepToS(&this->runOffsetRot, this->targetRotY, 5, 0x800, 0x10);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->baseRotY + this->runOffsetRot, 5, 0x800, 0x10);

        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if ((this->lightningHitSomething != true) && (this->actor.xzDistToPlayer > 750.0f) &&
            (this->actor.xzDistToPlayer < 1650.0f) && (ABS_ALT(yawDiff) < 0x3000)) {
            BossHakugin_SetupCharge(this);
        }
    }
}

void BossHakugin_SetupCharge(BossHakugin* this) {
    Animation_Change(&this->skelAnime, &gGohtRunAnim, 1.5f, 0.0f, 0.0f, ANIMMODE_LOOP, -3.0f);
    this->actionFunc = BossHakugin_Charge;
}

/**
 * This function makes Goht lower its head and charge quickly towards the player. If Goht hits the player, or if Goht
 * misses and the player ends up behind it, then this function will transition Goht back to running.
 */
void BossHakugin_Charge(BossHakugin* this, PlayState* play) {
    s16 angle;

    Math_ScaledStepToS(&this->frontHalfRotZ, 0x800, 0x100);
    Math_StepToF(&this->actor.speed, 25.0f, 2.0f);
    SkelAnime_Update(&this->skelAnime);
    BossHakugin_RunUpdateCommon(this, play);

    if ((this->transformedPlayerPos.z < 0.0f) || (this->bodyCollider.base.atFlags & AT_HIT)) {
        BossHakugin_SetupRun(this);
    } else {
        if ((this->distToLeftWall < (89100.0f * 0.001f)) || (this->distToRightWall < (89100.0f * 0.001f))) {
            this->targetRotY = this->baseRotY;
        } else {
            if (((this->transformedPlayerPos.x > 0.0f) && (this->transformedPlayerPos.x < this->distToLeftWall)) ||
                ((this->transformedPlayerPos.x <= 0.0f) && (-this->transformedPlayerPos.x < this->distToRightWall))) {
                angle = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                angle = CLAMP(angle, -0x3000, 0x3000);
                this->targetRotY = this->actor.shape.rot.y + angle;
            } else {
                this->targetRotY =
                    ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) >> 1) + this->actor.shape.rot.y;
            }
        }

        angle = this->targetRotY - this->baseRotY;
        if (angle > 0x1800) {
            this->targetRotY = this->baseRotY + 0x1800;
        } else if (angle < -0x1800) {
            this->targetRotY = this->baseRotY - 0x1800;
        }

        Math_SmoothStepToS(&this->actor.shape.rot.y, this->targetRotY, 5, 0x800, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void BossHakugin_SetupDowned(BossHakugin* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGohtFallDownAnim, -3.0f);
    this->bodyCollider.base.atFlags &= ~AT_ON;
    this->bodyCollider.base.acFlags &= ~AC_HARD;
    this->frontHalfRotZ = 0;
    this->finishedFallingDown = false;
    this->chargingLightOrbScale = 0.0f;

    if (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_CHARGE) {
        Math_Vec3f_Copy(&this->electricBallPos[0], &this->chargingLightningPos);
        this->electricBallSpeed = this->actor.speed + 100.0f;
        this->electricBallRot.x = Math_CosS(0xA00) * Math_SinS(this->actor.shape.rot.y);
        this->electricBallRot.y = Math_SinS(0xA00);
        this->electricBallRot.z = Math_CosS(0xA00) * Math_CosS(this->actor.shape.rot.y);
        this->electricBallState = GOHT_ELECTRIC_BALL_STATE_FLY_FORWARD;
        this->chargingLightOrbScale = 0.0f;
        Audio_PlaySfx_AtPos(&this->sfxPos, NA_SE_EN_COMMON_E_BALL_THR);
        this->electricBallCollider.base.atFlags |= AT_ON;
    }

    this->timer = 60;
    this->actor.speed = 20.0f;
    this->actionFunc = BossHakugin_Downed;
}

/**
 * This function will play Goht's falling down animation and make it skid to a complete stop. Once it stops, it will
 * randomly shake in place for 60 frames, then it will get up and go back to running.
 */
void BossHakugin_Downed(BossHakugin* this, PlayState* play) {
    CollisionPoly* floorPoly;
    Vec3f velocity;
    Vec3f pos;
    s32 i;
    s32 bgId;

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_WEAKENED - SFX_FLAG);

    if (SkelAnime_Update(&this->skelAnime)) {
        if (!this->finishedFallingDown) {
            this->finishedFallingDown = true;
            BossHakugin_RequestQuakeAndRumble(this, play, 17000, 7, 10);
        }

        // This will spawn dust on Goht's sides as it skids to a stop.
        if (this->actor.speed > 5.0f) {
            for (i = -1; i < 2; i += 2) {
                s16 angle = this->actor.shape.rot.y + (i * 0x7800);
                f32 offset;

                velocity.x = 2.0f * Math_SinS(angle);
                velocity.y = 1.0f;
                velocity.z = 2.0f * Math_CosS(angle);

                angle = this->actor.shape.rot.y + (i * 0x4000);
                offset = Rand_ZeroFloat(190.0f);

                pos.x = this->actor.world.pos.x + (80.0f * Math_SinS(angle)) +
                        (Math_SinS(this->actor.shape.rot.y) * offset);
                pos.z = this->actor.world.pos.z + (80.0f * Math_CosS(angle)) +
                        (Math_CosS(this->actor.shape.rot.y) * offset);
                pos.y = this->actor.world.pos.y + 300.0f;
                pos.y =
                    BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &floorPoly, &bgId, &this->actor, &pos) + 10.0f;

                func_800B12F0(play, &pos, &velocity, &gZeroVec3f, Rand_S16Offset(750, 100), 10, 30);
            }
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->baseRotY, 5, 0x800, 0x10);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (this->actor.speed < 0.1f) {
        this->timer--;
        this->actor.world.pos.x = this->actor.home.pos.x + Rand_CenteredFloat(5.0f);
        this->actor.world.pos.y = this->actor.home.pos.y + Rand_CenteredFloat(5.0f);
        this->actor.world.pos.z = this->actor.home.pos.z + Rand_CenteredFloat(5.0f);

        if (this->timer == 0) {
            this->bodyCollider.base.acFlags |= AC_HARD;
            Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
            BossHakugin_SetupRun(this);
            this->disableBodyCollidersTimer = 20;
        }
    } else if (Math_StepToF(&this->actor.speed, 5.0f, 0.3f)) {
        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
        this->actor.speed = 0.0f;
    }
}

void BossHakugin_SetupThrow(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Animation_MorphToPlayOnce(&this->skelAnime, &gGohtStationaryAnim, -10.0f);
    player->actor.parent = &this->actor;
    this->bodyCollider.base.atFlags &= ~AT_ON;
    this->bodyCollider.base.ocFlags1 &= ~OC1_ON;
    this->timer = 10;
    this->actionFunc = BossHakugin_Throw;
}

/**
 * This function stops Goht in place and makes it rotate its front half upwards; the player is "attached" to Goht's head
 * and is lifted into the air as Goht rotates. When Goht finishes its rotation, it throws the player backwards and waits
 * 10 frames before running again.
 */
void BossHakugin_Throw(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angle;
    f32 cos;

    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.speed, 0.0f, 2.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->baseRotY, 5, 0x800, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (this->timer < 10) {
        this->timer--;
        if (this->timer == 0) {
            this->bodyCollider.base.atFlags |= AT_ON;
            this->bodyCollider.base.ocFlags1 |= OC1_ON;
            BossHakugin_SetupRun(this);
        }
    } else if (Math_ScaledStepToS(&this->frontHalfRotZ, -0x800, 0x1C0)) {
        player->av2.actionVar2 = 100;
        player->actor.parent = NULL;
        player->invincibilityTimer = 0;
        player->actor.shape.rot.x = 0;
        func_800B8D50(play, &this->actor, 10.0f, (this->actor.shape.rot.y + 0x8000), 15.0f, 0x10);
        player->actor.world.pos.y += 30.0f;
        this->timer--;
    } else {
        player->av2.actionVar2 = 0;
        angle = -this->headRot.z + 0x1F40;
        player->actor.shape.rot.x = -this->headRot.z + 0x8FC0;

        cos = Math_CosS(angle);
        player->actor.world.pos.x = this->actor.focus.pos.x - (Math_SinS(this->baseRotY) * (20.0f * cos));
        player->actor.world.pos.y = this->actor.focus.pos.y - (Math_SinS(angle) * 20.0f);
        player->actor.world.pos.z = this->actor.focus.pos.z - (Math_CosS(this->baseRotY) * (20.0f * cos));
    }
}

void BossHakugin_SetupWait(BossHakugin* this, PlayState* play) {
    f32 direction;
    s16 atan;

    if (this->actionFunc != BossHakugin_ShootLightning) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gGohtStationaryAnim, -5.0f);
        this->actor.speed = 0.0f;
        atan = Math_Atan2S_XY(this->actor.world.pos.z, this->actor.world.pos.x);

        if (atan < -0x4000) {
            this->actor.world.pos.x = -1200.0f;
            this->actor.shape.rot.y = 0x4000;
            this->actor.world.pos.z = -1200.0f;
        } else if (atan < 0) {
            this->actor.world.pos.x = -1200.0f;
            this->actor.shape.rot.y = -0x8000;
            this->actor.world.pos.z = 1200.0f;
        } else if (atan < 0x4000) {
            this->actor.world.pos.x = 1200.0f;
            this->actor.shape.rot.y = -0x4000;
            this->actor.world.pos.z = 1200.0f;
        } else {
            this->actor.world.pos.x = 1200.0f;
            this->actor.shape.rot.y = 0;
            this->actor.world.pos.z = -1200.0f;
        }

        if (this->direction == GOHT_DIRECTION_CLOCKWISE) {
            direction = -1.0f;
            this->actor.shape.rot.y -= 0x4000;
        } else {
            direction = 1.0f;
        }

        this->baseRotY = this->actor.shape.rot.y + (this->direction * 0x6000);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->actor.world.pos.x += direction * 300.0f * Math_CosS(this->actor.shape.rot.y);
        this->actor.world.pos.z -= direction * 300.0f * Math_SinS(this->actor.shape.rot.y);
        this->actor.world.pos.y += 500.0f;
        this->actor.world.pos.y =
            BgCheck_EntityRaycastFloor1(&play->colCtx, &this->actor.floorPoly, &this->actor.world.pos);
        this->lightningHitSomething = false;
    }

    this->frontHalfRotZ = 0x700;
    this->timer = 150;
    this->chargingLightningTranslateZ = -100.0f;
    this->actionFunc = BossHakugin_Wait;
}

/**
 * This function makes Goht wait while facing the direction opposite of the direction in which it was last running. If
 * the player gets in range in front of Goht, it will stop waiting and start shooting lightning. Goht will stop waiting
 * and start running in its original direction if the player gets behind Goht, if the player is too close to Goht, or if
 * the player gets hit by one of Goht's lightning attacks. If Goht is still waiting after 150 frames, then it will
 * reverse its direction and run towards the player.
 */
void BossHakugin_Wait(BossHakugin* this, PlayState* play) {
    f32 absTransformedPlayerPosX;

    SkelAnime_Update(&this->skelAnime);
    absTransformedPlayerPosX = fabsf(this->transformedPlayerPos.x);
    Math_StepToF(&this->chargingLightOrbScale, 0.0f, 6.0f);

    if ((this->transformedPlayerPos.z < 0.0f) || (this->actor.xzDistToPlayer < 750.0f) ||
        (this->lightningHitSomething == true)) {
        BossHakugin_SetupRun(this);
        return;
    }

    if ((this->lightningSegments[0].alpha == 0) && (this->chargingLightOrbScale < 0.1f) &&
        (absTransformedPlayerPosX < 400.0f) && (this->transformedPlayerPos.z > 0.0f)) {
        BossHakugin_SetupShootLightning(this);
        return;
    }

    if ((absTransformedPlayerPosX > 400.0f) && ((play->gameplayFrames & 15) == 15)) {
        BossHakugin_SpawnStalactite(this);
        return;
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->direction = -this->direction;
        this->baseRotY = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        BossHakugin_SetupRun(this);
    }
}

void BossHakugin_SetupShootLightning(BossHakugin* this) {
    this->chargeUpTimer = 5;
    this->chargingLightOrbScale = 0.0f;
    this->actionFunc = BossHakugin_ShootLightning;
}

/**
 * This function will make Goht charge up an attack and then fire a single burst of lightning. After this, it will go
 * back to waiting.
 */
void BossHakugin_ShootLightning(BossHakugin* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Audio_PlaySfx_AtPos(&this->sfxPos, NA_SE_EN_COMMON_THUNDER - SFX_FLAG);

    if (BossHakugin_ChargeUpAttack(this)) {
        BossHakugin_AddLightningSegments(this, &this->chargingLightningPos, play);
        BossHakugin_SetupWait(this, play);
    }
}

void BossHakugin_SetupCutsceneStart(BossHakugin* this) {
    CutsceneManager_Queue(this->actor.csId);
    if (this->actionFunc == BossHakugin_FrozenBeforeFight) {
        this->nextCutsceneType = GOHT_NEXT_CUTSCENE_TYPE_INTRO;
    } else {
        this->nextCutsceneType = GOHT_NEXT_CUTSCENE_TYPE_ENTRANCE;
    }

    this->actionFunc = BossHakugin_CutsceneStart;
}

/**
 * Starts either the entrance cutscene, the intro cutscene, or the death cutscene, depending on Goht's current action
 * function and health.
 */
void BossHakugin_CutsceneStart(BossHakugin* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        if (this->actor.colChkInfo.health == 0) {
            BossHakugin_SetupDeathCutsceneRun(this);
            GameInteractor_ExecuteOnBossDefeated(this->actor.id); // 2S2H Time Splits
        } else if (this->nextCutsceneType == GOHT_NEXT_CUTSCENE_TYPE_INTRO) {
            BossHakugin_SetupIntroCutsceneThaw(this);
        } else {
            BossHakugin_SetupEntranceCutscene(this, play);
        }
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void BossHakugin_SetupDeathCutsceneRun(BossHakugin* this) {
    f32 direction;

    Animation_Change(&this->skelAnime, &gGohtRunAnim, 1.3f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);

    if ((this->actor.world.pos.z >= 0.0f) && (this->actor.world.pos.x >= 0.0f)) {
        this->targetRotY = -0x4000;
    } else if ((this->actor.world.pos.z >= 0.0f) && (this->actor.world.pos.x < 0.0f)) {
        this->targetRotY = -0x8000;
    } else if ((this->actor.world.pos.x >= 0.0f) && (this->actor.world.pos.z < 0.0f)) {
        this->targetRotY = 0;
    } else {
        this->targetRotY = 0x4000;
    }

    if (this->direction == GOHT_DIRECTION_CLOCKWISE) {
        this->targetRotY -= 0x4000;
    }

    direction = this->direction;
    if ((this->targetRotY > 0x6000) || (this->targetRotY < -0x6000)) {
        this->subCamEye.x = direction * -1200.0f;
        this->subCamEye.y = (direction * 80.0f) + -240.0f + 60.0f;
        this->subCamEye.z = 1200.0f;
    } else if ((this->targetRotY < 0x2000) && (this->targetRotY > -0x2000)) {
        this->subCamEye.x = direction * 1200.0f;
        this->subCamEye.y = 60.0f;
        this->subCamEye.z = -1200.0f;
    } else if (this->targetRotY >= 0x2000) {
        this->subCamEye.x = -1200.0f;
        this->subCamEye.y = (80.0f * direction) + -80.0f + 60.0f;
        this->subCamEye.z = direction * -1200.0f;
    } else {
        this->subCamEye.x = 1200.0f;
        this->subCamEye.y = (-160.0f * direction) + -160.0f + 60.0f;
        this->subCamEye.z = direction * 1200.0f;
    }

    this->chargingLightOrbScale = 0.0f;
    this->frontHalfRotZ = 0;
    Play_EnableMotionBlur(140);
    this->runOffsetRot = Rand_S16Offset(0x800, 0x800) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    this->actor.home.rot.y = this->actor.shape.rot.y;
    this->timer = 18;
    if ((this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FLY_FORWARD) ||
        (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FLY_TOWARDS_PLAYER)) {
        this->electricBallState = GOHT_ELECTRIC_BALL_STATE_FADE_OUT;
        this->electricBallCount = 0;
    }

    this->deathCutsceneRandomHeadRot = Rand_ZeroFloat(6144.0f);
    this->headRot.y = 0;
    this->headRot.z = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = CLAMP_MIN(this->actor.speed, 16.0f);
    this->actionFunc = BossHakugin_DeathCutsceneRun;
}

/**
 * Handles the first part of Goht's death cutscene where Goht runs forward while turning side-to-side wildly. Once
 * Goht's position and y-rotation reach a specific range of values, this function will make Goht swerve into the wall.
 */
void BossHakugin_DeathCutsceneRun(BossHakugin* this, PlayState* play) {
    Vec3f subCamAt;
    f32 absPosX;
    f32 absPosZ;
    s32 absRotY;

    Math_StepToF(&this->actor.speed, 15.0f, 2.0f);
    SkelAnime_Update(&this->skelAnime);

    // Setting these to 10 prevents Goht from spawning boulders, stalactites, and bombs during the cutscene, since the
    // relevant spawn functions are called fewer than 10 times during the cutscene.
    this->preventBoulderSpawnCount = 10;
    this->preventStalactiteSpawnCount = 10;
    this->preventBombSpawnCount = 10;

    BossHakugin_RunUpdateCommon(this, play);
    Math_SmoothStepToS(&this->actor.home.rot.y, this->baseRotY, 5, 0x800, 0x100);
    this->timer--;
    this->actor.shape.rot.y =
        (s32)(Math_SinF(this->timer * (M_PIf / 18.0f)) * this->runOffsetRot) + this->actor.home.rot.y;

    if (this->timer == 0) {
        if (this->runOffsetRot > 0) {
            this->runOffsetRot = -Rand_S16Offset(0x800, 0x800);
        } else {
            this->runOffsetRot = Rand_S16Offset(0x800, 0x800);
        }

        this->timer = 18;
        this->deathCutsceneRandomHeadRot = Rand_ZeroFloat(6144.0f);
    }

    this->headRot.z = (8192.0f * Math_SinS(this->deathCutsceneRandomHeadRot)) * Math_SinF(this->timer * (M_PIf / 9));
    this->headRot.y = (8192.0f * Math_CosS(this->deathCutsceneRandomHeadRot)) * Math_SinF(this->timer * (M_PIf / 9));

    this->actor.shape.rot.z = Math_SinF(this->timer * (M_PIf / 18)) * -(this->runOffsetRot * 0.3f);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    subCamAt.x = (Math_SinS(this->actor.shape.rot.y) * 100.0f) + this->actor.world.pos.x;
    subCamAt.y = this->actor.world.pos.y + 200.0f;
    subCamAt.z = (Math_CosS(this->actor.shape.rot.y) * 100.0f) + this->actor.world.pos.z;
    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &this->subCamEye);

    if (this->targetRotY == this->baseRotY) {
        absPosX = fabsf(this->actor.world.pos.x);
        absPosZ = fabsf(this->actor.world.pos.z);
        absRotY = ABS_ALT(this->actor.shape.rot.y);

        if (((absPosX > 1289.1f) && (absPosZ < 1110.9f) && ((absRotY < 0x2000) || (absRotY > 0x6000))) ||
            ((absPosZ > 1289.1f) && (absPosX < 1110.9f) && (absRotY > 0x2000) && (absRotY < 0x6000))) {
            BossHakugin_SetupDeathCutsceneSwerveIntoWall(this);
        }
    }
}

void BossHakugin_SetupDeathCutsceneSwerveIntoWall(BossHakugin* this) {
    if (this->direction == GOHT_DIRECTION_CLOCKWISE) {
        this->targetRotY -= 0x4000;
    } else {
        this->targetRotY += 0x4000;
    }

    this->actor.speed = 15.0f;
    this->actionFunc = BossHakugin_DeathCutsceneSwerveIntoWall;
}

/**
 * Handles the second part of Goht's death cutscene where it swerves hard to the side and slams into the wall. Once
 * Goht's y-rotation matches the target rotation, this function will transition Goht into being crushed by rocks.
 */
void BossHakugin_DeathCutsceneSwerveIntoWall(BossHakugin* this, PlayState* play) {
    Vec3f subCamAt;
    Vec3f eyeTarget;
    s32 absTargetRotY;
    s16 eyeTargetAngle;

    SkelAnime_Update(&this->skelAnime);

    subCamAt.x = (Math_SinS(this->actor.shape.rot.y) * 100.0f) + this->actor.world.pos.x;
    subCamAt.y = this->actor.world.pos.y + 200.0f;
    subCamAt.z = (Math_CosS(this->actor.shape.rot.y) * 100.0f) + this->actor.world.pos.z;

    eyeTargetAngle = this->actor.shape.rot.y - (this->direction * 0x6000);
    eyeTarget.x = (Math_SinS(eyeTargetAngle) * 400.0f) + this->actor.world.pos.x;
    eyeTarget.y = this->actor.world.pos.y + 100.0f;
    eyeTarget.z = (Math_CosS(eyeTargetAngle) * 400.0f) + this->actor.world.pos.z;

    Math_Vec3f_StepTo(&this->subCamEye, &eyeTarget, 25.0f);
    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &this->subCamEye);

    // Setting these to 10 prevents Goht from spawning boulders, stalactites, and bombs during the cutscene, since the
    // relevant spawn functions are called fewer than 10 times during the cutscene.
    this->preventBoulderSpawnCount = 10;
    this->preventStalactiteSpawnCount = 10;
    this->preventBombSpawnCount = 10;

    BossHakugin_RunUpdateCommon(this, play);

    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetRotY, 0x300) &&
        ((this->distToRightWall <= 189.00002f) || (this->distToLeftWall <= 189.00002f))) {
        BossHakugin_SetupDeathCutsceneCrushedByRocks(this);
    } else if (((this->direction == GOHT_DIRECTION_CLOCKWISE) && (this->distToRightWall <= 189.00002f)) ||
               ((this->direction == GOHT_DIRECTION_COUNTERCLOCKWISE) && (this->distToLeftWall <= 189.00002f))) {
        absTargetRotY = ABS_ALT(this->targetRotY);
        if (absTargetRotY < 0x2000) {
            this->actor.world.pos.z = -1389.0f;
        } else if (absTargetRotY > 0x6000) {
            this->actor.world.pos.z = 1389.0f;
        } else if (this->targetRotY > 0) {
            this->actor.world.pos.x = -1389.0f;
        } else {
            this->actor.world.pos.x = 1389.0f;
        }
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void BossHakugin_SetupDeathCutsceneCrushedByRocks(BossHakugin* this) {
    this->timer = 0;
    this->actor.speed = 0.0f;
    this->skelAnime.playSpeed = 0.5f;
    BossHakugin_SpawnCrushingRocks(this);
    this->actionFunc = BossHakugin_DeathCutsceneCrushedByRocks;
}

typedef struct {
    /* 0x0 */ GohtColliderBodyPart colliderIndex;
    /* 0x4 */ u32 limbHideFlags;
} ExplosionLimbHideInfo; // size = 0x8

/**
 * This array is used to determine the timing of when certain rocks should fall to crush Goht. Rocks fall in four
 * distinct waves, and which rocks fall in which wave is dependent on the values in this array. For example, for the
 * first wave, rocks with an index of 0 to 14 all fall at once; for the second wave, rocks with an index of 15 to 25 all
 * fall at once. You can see how this array is used in `BossHakugin_DeathCutsceneCrushedByRocks` for more information.
 */
static s32 sRockNums[5] = { 0, 15, 26, 33, 36 };

/**
 * Various explosions go off as rocks fall to crush Goht. Each element of this array is used to determine:
 * - At which body part collider a given explosion should originate
 * - Which limbs should be hidden when an explosion occurs. This is necessary to do because Goht will still be
 *   performing its running animation as it gets crushed; if its limbs were not hidden, the player would be able
 *   to see them sticking out of the rocks.
 */
static ExplosionLimbHideInfo sExplosionLimbHideInfo[] = {
    { GOHT_COLLIDER_BODYPART_HEAD, GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_HEAD) | GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_JAW) },
    { GOHT_COLLIDER_BODYPART_FRONT_RIGHT_LOWER_LEG, GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_FRONT_RIGHT_UPPER_LEG) |
                                                        GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_FRONT_RIGHT_LOWER_LEG) |
                                                        GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_FRONT_RIGHT_HOOF) },
    { GOHT_COLLIDER_BODYPART_FRONT_LEFT_LOWER_LEG, GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_FRONT_LEFT_UPPER_LEG) |
                                                       GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_FRONT_LEFT_LOWER_LEG) |
                                                       GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_FRONT_LEFT_HOOF) },
    { GOHT_COLLIDER_BODYPART_BACK_LEFT_SHIN, GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_LEFT_SHIN) |
                                                 GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_LEFT_PASTERN) |
                                                 GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_LEFT_HOOF) },
    { GOHT_COLLIDER_BODYPART_BACK_RIGHT_SHIN, GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_RIGHT_SHIN) |
                                                  GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_RIGHT_PASTERN) |
                                                  GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_RIGHT_HOOF) },
    { GOHT_COLLIDER_BODYPART_THORAX, GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_PELVIS) | GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_THORAX) |
                                         GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_RIGHT_THIGH) |
                                         GOHT_LIMB_DRAW_FLAG(GOHT_LIMB_BACK_LEFT_THIGH) },
};

/**
 * Handles the final part of Goht's death cutscene where rocks fall from the ceiling to crush it. As the rocks cover
 * Goht's body, this function will set off various explosions and hide Goht's limbs. After 30 frames, this function will
 * start the "boss defeated" theme, and after 180 frames, it will end the cutscene, spawn the heart container and
 * blue warp, enable collision on the fallen rocks, and stop running Goht's regular update and draw functions.
 */
void BossHakugin_DeathCutsceneCrushedByRocks(BossHakugin* this, PlayState* play) {
    EnBom* bomb;
    Vec3s* pos;
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f eyeTarget;
    s16 eyeTargetAngle;
    s32 index;
    ExplosionLimbHideInfo* explosionLimbHideInfo;

    SkelAnime_Update(&this->skelAnime);
    index = this->timer / 6;

    eyeTargetAngle = this->actor.shape.rot.y + (0x6000 * this->direction);
    eyeTarget.x = (Math_SinS(eyeTargetAngle) * 500.0f) + this->actor.world.pos.x;
    eyeTarget.y = this->actor.world.pos.y + 250.0f;
    eyeTarget.z = (Math_CosS(eyeTargetAngle) * 500.0f) + this->actor.world.pos.z;
    Math_Vec3f_StepTo(&this->subCamEye, &eyeTarget, 25.0f);
    Play_SetCameraAtEye(play, this->subCamId, &subCam->at, &this->subCamEye);

    if (index == 30) {
        Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

        Play_DisableMotionBlur();
        CutsceneManager_Stop(this->actor.csId);
        Play_SetCameraAtEye(play, this->subCamId, &mainCam->at, &mainCam->eye);
        BossHakugin_SpawnBlueWarpAndHeartContainer(this, play);
        BossHakugin_UpdateCrushingRocksCollision(this);
        this->actor.draw = BossHakugin_DrawCrushingRocks;
        this->actor.update = BossHakugin_UpdateStationaryCrushingRocks;
    } else if (((this->timer % 6) == 0) && (index < 6)) {
        explosionLimbHideInfo = &sExplosionLimbHideInfo[index];
        pos = &this->bodyCollider.elements[explosionLimbHideInfo->colliderIndex].dim.worldSphere.center;
        bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, pos->x, pos->y, pos->z,
                                   BOMB_EXPLOSIVE_TYPE_POWDER_KEG, 0, 0, BOMB_TYPE_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        this->limbDrawFlags &= ~explosionLimbHideInfo->limbHideFlags;
        if (index == 5) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CLEAR_BOSS | SEQ_FLAG_ASYNC);
        }
    }

    if (((this->timer + 3) % 6) == 0) {
        s32 i;

        index = ((this->timer + 3) / 6) - 1;
        if (index < ARRAY_COUNT(sRockNums) - 1) {
            for (i = sRockNums[index]; i < sRockNums[index + 1]; i++) {
                this->crushingRocks[i].fallingSpeed = Rand_ZeroFloat(5.0f) + 5.0f;
            }
        }

        if (index < 6) {
            if ((index % 2) != 0) {
                Audio_PlaySfx_AtPos(&this->sfxPos, NA_SE_EN_LAST1_DEMO_WALL);
            } else {
                Audio_PlaySfx_AtPos(&this->sfxPos, NA_SE_EN_LAST1_DEMO_BREAK);
            }
        }
    }

    this->timer++;
}

/**
 * This function checks to see if any of Goht's body colliders hit the player. If Goht is charging and the player
 * touched Goht's head collider, this function can throw them. Otherwise, if the player doesn't currently have Goron
 * Spikes and didn't block the collider with their shield, then this function will knock the player back.
 */
void BossHakugin_CheckForBodyColliderHit(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->bodyCollider.base.atFlags & AT_HIT) &&
        ((this->actionFunc == BossHakugin_Charge) || !(player->stateFlags3 & PLAYER_STATE3_80000))) {
        if ((this->actionFunc == BossHakugin_Charge) &&
            (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_HEAD].base.atElemFlags & ATELEM_HIT) &&
            !(this->bodyCollider.base.atFlags & AT_BOUNCED) && play->grabPlayer(play, player)) {
            BossHakugin_SetupThrow(this, play);
        } else if (player->stateFlags3 & PLAYER_STATE3_1000) {
            //! @bug This block is unreachable. This is trying to check if the player curled up as a Goron without
            //! spikes, but if the player gets hit in this state, they will be forcibly uncurled before this function
            //! gets called; it's impossible to have the state flags necessary to end up here in the final game.
            player->unk_B08 = player->speedXZ = -5.0f;
            player->unk_B0C += player->unk_B08 * 0.05f;
            player->actor.velocity.y = 10.0f;
            player->unk_B8C = 4;
            player->actor.shape.rot.y = player->actor.home.rot.y = player->yaw = player->actor.world.rot.y;
        } else if (!(this->bodyCollider.base.atFlags & AT_BOUNCED)) {
            s16 knockbackYaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if (knockbackYaw > 0) {
                knockbackYaw = this->actor.shape.rot.y + (s32)(knockbackYaw / 2.0f) + 0x4000;
            } else {
                knockbackYaw = this->actor.shape.rot.y + (s32)(knockbackYaw / 2.0f) - 0x4000;
            }

            func_800B8D50(play, &this->actor, 5.0f, knockbackYaw, 6.0f, 0);
        }
    }
}

/**
 * Checks if Goht has been hit by an attack and updates its health if it has been hit. This function is also responsible
 * for starting Goht's death cutscene when its health reaches 0. Returns true if Goht has been hit by an attack.
 */
s32 BossHakugin_UpdateDamage(BossHakugin* this, PlayState* play) {
    if (this->bodyCollider.base.acFlags & AC_HIT) {
        s32 i;

        for (i = 0; i < GOHT_COLLIDER_BODYPART_MAX; i++) {
            if (this->bodyCollider.elements[i].base.acElemFlags & ACELEM_HIT) {
                break;
            }
        }

        if (i == GOHT_COLLIDER_BODYPART_MAX) {
            return false;
        }

        // DMG_DEKU_NUT | DMG_DEKU_STICK | DMG_ZORA_BOOMERANG | DMG_NORMAL_ARROW | DMG_HOOKSHOT | DMG_ICE_ARROW
        // | DMG_LIGHT_ARROW | DMG_DEKU_SPIN | DMG_DEKU_BUBBLE | DMG_DEKU_LAUNCH | DMG_ZORA_BARRIER
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
            (this->bodyCollider.elements[i].base.acHitElem->atDmgInfo.dmgFlags & 0x000DB0B3)) {
            return false;
        }

        BossHakugin_Thaw(this, play);

        if (this->actionFunc == BossHakugin_Downed) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 15);
            BossHakugin_UpdateDrawDmgEffect(this, play, i);

            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_DEAD_OLD);
                BossHakugin_SetupCutsceneStart(this);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_DAMAGE_OLD);
            }

            this->disableBodyCollidersTimer = 15;
            return true;
        }

        if (((this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_GORON_SPIKES) ||
             (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_NONE) ||
             (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_FIRE) ||
             (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_FREEZE) ||
             (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_LIGHT_ORB) ||
             (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_BLUE_LIGHT_ORB) ||
             (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_EXPLOSIVE)) &&
            ((this->actionFunc == BossHakugin_Run) || (this->actionFunc == BossHakugin_ShootLightning) ||
             (this->actionFunc == BossHakugin_Wait) || (this->actionFunc == BossHakugin_Charge))) {
            Player* player = GET_PLAYER(play);

            if (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_GORON_SPIKES) {
                player->pushedSpeed = 15.0f;
                if ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) > 0) {
                    player->pushedYaw = this->actor.shape.rot.y + 0x4000;
                } else {
                    player->pushedYaw = this->actor.shape.rot.y - 0x4000;
                }
            }

            this->disableBodyCollidersTimer = 15;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 15);
            this->damagedSpeedUpCounter += 35;
            BossHakugin_UpdateDrawDmgEffect(this, play, i);
            this->actor.colChkInfo.damage = this->bodyCollider.elements[i].base.acHitElem->atDmgInfo.damage;

            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_DEAD_OLD);
                BossHakugin_SetupCutsceneStart(this);
            } else {
                if ((this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_EXPLOSIVE) ||
                    ((this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_GORON_SPIKES) &&
                     (this->actionFunc != BossHakugin_Charge) &&
                     ((this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_HEAD].base.acElemFlags & ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_THORAX].base.acElemFlags & ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_PELVIS].base.acElemFlags & ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_FRONT_RIGHT_UPPER_LEG].base.acElemFlags &
                       ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_FRONT_LEFT_UPPER_LEG].base.acElemFlags &
                       ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_BACK_RIGHT_THIGH].base.acElemFlags &
                       ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_BACK_LEFT_THIGH].base.acElemFlags &
                       ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_RIGHT_HORN].base.acElemFlags & ACELEM_HIT) ||
                      (this->bodyCollider.elements[GOHT_COLLIDER_BODYPART_LEFT_HORN].base.acElemFlags & ACELEM_HIT)))) {
                    BossHakugin_SetupDowned(this);
                } else if ((this->electricBallState == GOHT_ELECTRIC_BALL_STATE_NONE) &&
                           (this->electricBallCount == 0) && (this->actionFunc == BossHakugin_Run) &&
                           (this->actor.colChkInfo.damageEffect == GOHT_DMGEFF_GORON_SPIKES)) {
                    this->chargeUpTimer = 5;
                    this->electricBallState = GOHT_ELECTRIC_BALL_STATE_CHARGE;
                }

                Actor_PlaySfx(&this->actor, NA_SE_EN_ICEB_DAMAGE_OLD);
            }

            return true;
        } else {
            // This block is for attacks with the effect of `GOHT_DMGEFF_DOWNED_ONLY` hitting Goht while it's standing
            // upright. These attacks deal no damage and instead just spawn sparks and play a metal sound.
            s32 j;

            this->disableBodyCollidersTimer = 20;
            for (j = 0; j < ARRAY_COUNT(this->bodyColliderElements); j++) {
                Vec3f hitPos;
                ColliderElement* elem = &this->bodyCollider.elements[j].base;

                if ((elem->acElemFlags & ACELEM_HIT) && (elem->acHitElem != NULL) &&
                    !(elem->acHitElem->atElemFlags & ATELEM_SFX_NONE)) {
                    Math_Vec3s_ToVec3f(&hitPos, &elem->acDmgInfo.hitPos);
                    EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_METAL, &hitPos);
                    CollisionCheck_SpawnShieldParticlesMetalSound(play, &hitPos, &this->actor.projectedPos);
                    break;
                }
            }
        }
    }

    return false;
}

/**
 * Updates all of the various rock effects that Goht and EnHakurock can spawn. If the rock effect's timer is above zero,
 * then it's y-velocity will be decreased to make it fall faster. Once the effect reaches a certain point below the
 * floor, the timer is set to -1, effectively disabling the effect and allowing it to be reused later.
 */
void BossHakugin_UpdateRockEffects(BossHakugin* this) {
    GohtRockEffect* rockEffect;
    s32 i;

    for (i = 0; i < GOHT_ROCK_EFFECT_COUNT; i++) {
        rockEffect = &this->rockEffects[i];

        if (rockEffect->timer >= 0) {
            rockEffect->timer--;
            rockEffect->velocity.y += -1.0f;
            Math_Vec3f_Sum(&rockEffect->pos, &rockEffect->velocity, &rockEffect->pos);

            if (rockEffect->pos.y < -500.0f) {
                rockEffect->timer = -1;
            } else {
                rockEffect->rot.x += (s16)(0x700 + (Rand_Next() >> 0x17));
                rockEffect->rot.y += (s16)(0x900 + (Rand_Next() >> 0x17));
                rockEffect->rot.z += (s16)(0xB00 + (Rand_Next() >> 0x17));
            }
        }
    }
}

/**
 * Updates the rocks that crush Goht as they fall onto it during its death cutscene. The rocks gradually speed up as
 * they fall until they approach their target position, at which point they completely stop moving.
 */
void BossHakugin_UpdateFallingCrushingRocks(BossHakugin* this) {
    GohtCrushingRock* crushingRock;
    s32 i;

    for (i = 0; i < GOHT_CRUSHING_ROCK_COUNT; i++) {
        crushingRock = &this->crushingRocks[i];
        Math_StepToF(&crushingRock->pos.y, crushingRock->targetPosY, crushingRock->fallingSpeed);
        if (crushingRock->fallingSpeed > 0.0f) {
            crushingRock->fallingSpeed += 6.0f;
        }
    }
}

/**
 * Updates the black smoke component of the malfunction effect. The smoke will drift upwards and increase in scale; if
 * the timer is below 0, the smoke will also decrease in alpha until it completely fades out.
 */
void BossHakugin_UpdateMalfunctionEffects(BossHakugin* this) {
    s32 i;
    s32 j;

    for (i = 0; i < ARRAY_COUNT(this->malfunctionEffects); i++) {
        for (j = 0; j < ARRAY_COUNT(this->malfunctionEffects[0]); j++) {
            GohtMalfunctionEffect* malfunctionEffect = &this->malfunctionEffects[i][j];

            if (malfunctionEffect->alpha > 0) {
                malfunctionEffect->timer--;
                malfunctionEffect->pos.y += 3.5f;
                malfunctionEffect->scaleXY += 0.003f;

                if (malfunctionEffect->timer < 0) {
                    malfunctionEffect->alpha -= 15;
                    if (malfunctionEffect->alpha < 0) {
                        malfunctionEffect->alpha = 0;
                    }
                }
            }
        }
    }
}

/**
 * Updates the segments of the lightning attack that Goht can shoot at the player while it's waiting. Each segment's
 * alpha is decreased by 20 for every frame; if the alpha is below 60, the lightning segment will no longer be able to
 * hurt the player.
 */
void BossHakugin_UpdateLightningSegments(BossHakugin* this, PlayState* play) {
    GohtLightningSegment* lightningSegment;
    s16 rand;
    s32 i;

    for (i = 0; i < GOHT_LIGHTNING_SEGMENT_COUNT; i++) {
        lightningSegment = &this->lightningSegments[i];

        if (lightningSegment->collider.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(play);

            this->lightningHitSomething = true;
            Player_PlaySfx(player, NA_SE_EN_COMMON_E_BALL_HIT);
            lightningSegment->collider.base.atFlags &= ~AT_HIT;
        }

        if (lightningSegment->alpha > 0) {
            lightningSegment->alpha -= 20;

            if (lightningSegment->alpha <= 0) {
                lightningSegment->alpha = 0;
            } else if (lightningSegment->alpha <= 255) {
                rand = (s32)Rand_Next() >> 0x10;
                lightningSegment->rot.z = rand;

                if (lightningSegment->alpha > 60) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &lightningSegment->collider.base);
                }
            }
        }
    }
}

/**
 * Updates the electric balls that Goht can shoot at the player.
 */
void BossHakugin_UpdateElectricBalls(BossHakugin* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    CollisionPoly* poly = NULL;
    Vec3f resultPos;
    Vec3f posB;
    Vec3f normal;
    Vec3f targetRot;
    Vec3f* firstBallPos;
    Vec3f* ballPosIter;
    s32 pad;
    f32 angleStep;
    s32 bgId;
    s32 playedHitSfx = false;

    firstBallPos = &this->electricBallPos[0];

    if (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FLY_FORWARD) {
        if (Math_StepToF(&this->electricBallSpeed, 50.0f, 3.5f)) {
            this->electricBallState = GOHT_ELECTRIC_BALL_STATE_FLY_TOWARDS_PLAYER;
        }
    } else if (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FLY_TOWARDS_PLAYER) {
        angleStep =
            BINANG_TO_RAD(Math3D_Vec3fDistSq(firstBallPos, &player->actor.world.pos) * (1.0f / SQ(120.0f)) * 32.0f);
        angleStep = CLAMP_MAX(angleStep, M_PIf / 4.0f);

        Math_Vec3f_Diff(&player->actor.world.pos, firstBallPos, &targetRot);

        if (BossHakugin_Vec3fNormalize(&targetRot)) {
            BossHakugin_StepVector(&this->electricBallRot, &targetRot, angleStep);
        }
    } else if (this->electricBallState != GOHT_ELECTRIC_BALL_STATE_FADE_OUT) {
        return;
    }

    Audio_PlaySfx_AtPos(&this->electricBallSfxPos, NA_SE_EN_COMMON_E_BALL - SFX_FLAG);

    if (this->electricBallCount < GOHT_ELECTRIC_BALL_COUNT_MAX) {
        this->electricBallCount++;
    }

    if ((this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FADE_OUT) &&
        (this->electricBallCount == GOHT_ELECTRIC_BALL_COUNT_MAX - 1)) {
        this->electricBallState = GOHT_ELECTRIC_BALL_STATE_NONE;
        this->electricBallCount = 0;
    } else {
        // The electric ball attack is actually made up of a chain of light orbs; the first light orb is the one to
        // which the collider is attached and the one that all other light orbs follow. This loop below will make it so
        // that the position of `this->electricBallPos[N + 1]` will equal the position of `this->electricBallPos[N]` on
        // the previous frame, creating a trail effect.
        ballPosIter = &this->electricBallPos[GOHT_ELECTRIC_BALL_COUNT_MAX - 1];
        while (ballPosIter != firstBallPos) {
            Math_Vec3f_Copy(ballPosIter, ballPosIter - 1);
            ballPosIter--;
        }

        this->lightOrbRotZ += (s16)(0x4000 + (Rand_Next() >> 0x12));

        if (this->electricBallState != GOHT_ELECTRIC_BALL_STATE_FADE_OUT) {
            if (this->electricBallCollider.base.atFlags & AT_HIT) {
                this->electricBallState = GOHT_ELECTRIC_BALL_STATE_FADE_OUT;
                this->electricBallCount = 0;
                this->electricBallCollider.base.atFlags &= ~AT_HIT;
                this->electricBallCollider.base.atFlags &= ~AT_ON;
                playedHitSfx = true;
                Audio_PlaySfx_AtPos(&this->electricBallSfxPos, NA_SE_EN_COMMON_E_BALL_HIT);
            }

            posB.x = firstBallPos->x + (this->electricBallSpeed * this->electricBallRot.x);
            posB.y = firstBallPos->y + (this->electricBallSpeed * this->electricBallRot.y);
            posB.z = firstBallPos->z + (this->electricBallSpeed * this->electricBallRot.z);

            if (BgCheck_EntityLineTest1(&play->colCtx, firstBallPos, &posB, &resultPos, &poly, true, true, true, true,
                                        &bgId)) {
                s16 yawDiff = false;

                Math_Vec3f_Copy(firstBallPos, &resultPos);

                if ((poly->normal.y > -0x6665) && (poly->normal.y < 0x3FFF)) {
                    yawDiff = Math_Vec3f_Yaw(firstBallPos, &player->actor.world.pos) -
                              Math_Atan2S_XY(COLPOLY_GET_NORMAL(poly->normal.z), COLPOLY_GET_NORMAL(poly->normal.x));
                    yawDiff = (ABS_ALT(yawDiff) > 0x4000) ? true : false;
                }

                if ((this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FLY_TOWARDS_PLAYER) &&
                    (yawDiff || (poly->normal.y >= 0x4000))) {
                    this->electricBallState = GOHT_ELECTRIC_BALL_STATE_FADE_OUT;
                    this->electricBallCount = 0;
                    if (!playedHitSfx) {
                        Audio_PlaySfx_AtPos(&this->electricBallSfxPos, NA_SE_EN_COMMON_E_BALL_HIT);
                    }
                } else {
                    normal.x = COLPOLY_GET_NORMAL(poly->normal.x);
                    normal.y = COLPOLY_GET_NORMAL(poly->normal.y);
                    normal.z = COLPOLY_GET_NORMAL(poly->normal.z);
                    Math3D_Vec3fReflect(&this->electricBallRot, &normal, &targetRot);
                    Math_Vec3f_Copy(&this->electricBallRot, &targetRot);
                }
            } else {
                Math_Vec3f_Copy(firstBallPos, &posB);
            }

            this->electricBallCollider.dim.worldSphere.center.x = firstBallPos->x;
            this->electricBallCollider.dim.worldSphere.center.y = firstBallPos->y;
            this->electricBallCollider.dim.worldSphere.center.z = firstBallPos->z;
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->electricBallCollider.base);
        }
    }
}

void BossHakugin_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BossHakugin* this = (BossHakugin*)thisx;
    Vec3s targetRot;
    Player* player = GET_PLAYER(play);

    this->blockMalfunctionEffects = false;
    DECR(this->damagedSpeedUpCounter);

    if (this->actionFunc != BossHakugin_CutsceneStart) {
        if (!BossHakugin_UpdateDamage(this, play)) {
            BossHakugin_CheckForBodyColliderHit(this, play);
        }
    }

    BossHakugin_UpdateBaseRot(this, play);
    Actor_WorldToActorCoords(&this->actor, &this->transformedPlayerPos, &player->actor.world.pos);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 450.0f, (89100.0f * 0.001f), 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    func_800BE3D0(&this->actor, this->actor.shape.rot.y, &targetRot);
    Math_ScaledStepToS(&this->actor.shape.rot.x, targetRot.x, 0x100);
    Math_ScaledStepToS(&this->actor.shape.rot.z, targetRot.z, 0x100);

    if (this->actionFunc == BossHakugin_DeathCutsceneCrushedByRocks) {
        BossHakugin_UpdateFallingCrushingRocks(this);
    } else {
        BossHakugin_UpdateRockEffects(this);
    }

    BossHakugin_UpdateMalfunctionEffects(this);
    BossHakugin_UpdateLightningSegments(this, play);
    BossHakugin_UpdateElectricBalls(this, play);
    BossHakugin_SpawnIceSparkle(this, play);
    BossHakugin_ApplyGoronSpikeBoost(this, player);
    BossHakugin_UpdateLight(this);

    if ((this->bodyCollider.base.atFlags & AT_ON) && (this->actor.colorFilterTimer == 0)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->bodyCollider.base);
    } else {
        this->bodyCollider.base.atFlags &= ~AT_HIT;
    }

    if (this->disableBodyCollidersTimer == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
    } else {
        this->bodyCollider.base.acFlags &= ~AC_HIT;
        this->disableBodyCollidersTimer--;
    }

    if (this->bodyCollider.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    } else {
        this->bodyCollider.base.ocFlags1 &= ~OC1_HIT;
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 1.25f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 2.5f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 2.5f, 5.0f / 12.0f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        } else {
            BossHakugin_Thaw(this, play);
        }
    }

    // The lowest valid `floorHeight` the player can have while still being within the bounds of the arena is -320.0f.
    // Thus, this is checking if the player is below the floor; this entire block is about moving the player to a valid
    // position if they've somehow left the arena.
    if (player->actor.floorHeight < -400.0f) {
        CollisionPoly* poly = NULL;
        Vec3f resultPos;
        Vec3f posA;
        s32 bgId;

        if ((fabsf(player->actor.world.pos.z) < 1200.0f) && (fabsf(player->actor.world.pos.x) < 1200.0f)) {
            s16 atan = Math_Atan2S_XY(player->actor.world.pos.z, player->actor.world.pos.x);

            posA.x = Math_SinS(atan) * 2000.0f;
            posA.z = Math_CosS(atan) * 2000.0f;
        } else {
            if (player->actor.world.pos.z > 1200.0f) {
                posA.z = 1500.0f;
            } else if (player->actor.world.pos.z < -1200.0f) {
                posA.z = -1500.0f;
            } else {
                posA.z = player->actor.world.pos.z;
            }
            if (player->actor.world.pos.x > 1200.0f) {
                posA.x = 1500.0f;
            } else if (player->actor.world.pos.x < -1200.0f) {
                posA.x = -1500.0f;
            } else {
                posA.x = player->actor.world.pos.x;
            }
        }

        posA.y = 100.0f;

        if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &player->actor.world.pos, &resultPos, &poly, true, true,
                                    false, true, &bgId)) {
            Math_Vec3f_Copy(&player->actor.world.pos, &resultPos);
            Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);
            Math_Vec3f_Copy(&player->actor.prevPos, &player->actor.world.pos);
            player->actor.floorHeight = player->actor.world.pos.y;
            player->actor.world.pos.y = player->actor.world.pos.y + 1.0f;
        } else {
            player->actor.world.pos.x = 0.0f;
            player->actor.world.pos.y = 1.0f;
            player->actor.world.pos.z = -1200.0f;
            Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);
            Math_Vec3f_Copy(&player->actor.prevPos, &player->actor.world.pos);
            player->actor.floorHeight = player->actor.world.pos.y;
        }
    }
}

s32 BossHakugin_OverrideLimbDraw(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 Actor* thisx) {
    static s32 sCurrentMalfunctionType = GOHT_MALFUNCTION_NUM_TYPES - 1;
    static s32 sCurrentLimbIndex = GOHT_LIMB_FRONT_RIGHT_UPPER_LEG;
    BossHakugin* this = (BossHakugin*)thisx;

    if (this->actionFunc == BossHakugin_DeathCutsceneCrushedByRocks) {
        if (limbIndex == GOHT_LIMB_ROOT) {
            pos->y -= this->actor.shape.yOffset;
        }

        if (!(this->limbDrawFlags & GOHT_LIMB_DRAW_FLAG(limbIndex))) {
            *dList = NULL;
        }
    }

    if (!this->blockMalfunctionEffects && (limbIndex == sCurrentLimbIndex)) {
        Matrix_MultZero(&this->malfunctionEffects[sCurrentMalfunctionType][this->malfunctionEffectIndex].pos);

        sCurrentMalfunctionType--;
        if (sCurrentMalfunctionType < 0) {
            sCurrentMalfunctionType = GOHT_MALFUNCTION_NUM_TYPES - 1;
        }

        sCurrentLimbIndex = sMalfunctionTypeToLimbIndex[sCurrentMalfunctionType];
    }

    if (limbIndex == GOHT_LIMB_HEAD) {
        rot->z += this->frontHalfRotZ;
        if (this->actionFunc == BossHakugin_DeathCutsceneRun) {
            rot->z += this->headRot.z;
            rot->y += this->headRot.y;
        }
    } else if (limbIndex == GOHT_LIMB_THORAX_WRAPPER) {
        rot->z += (s16)(this->frontHalfRotZ * 2);
    } else if ((limbIndex == GOHT_LIMB_FRONT_LEFT_LEG_ROOT) || (limbIndex == GOHT_LIMB_FRONT_RIGHT_LEG_ROOT)) {
        rot->z -= (s16)(this->frontHalfRotZ * 3);
    }

    return false;
}

void BossHakugin_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    BossHakugin* this = (BossHakugin*)thisx;
    s8 bodyPartIndex;
    s32 i;
    f32 cos;
    s16 angle;

    Collider_UpdateSpheres(limbIndex, &this->bodyCollider);
    bodyPartIndex = sLimbToBodyParts[limbIndex];

    if (bodyPartIndex > BODYPART_NONE) {
        for (i = 0; i < GOHT_COLLIDER_BODYPART_MAX; i++) {
            if (this->bodyCollider.elements[i].dim.limb == limbIndex) {
                Math_Vec3s_ToVec3f(&this->bodyPartsPos[bodyPartIndex],
                                   &this->bodyCollider.elements[i].dim.worldSphere.center);
                break;
            }
        }

        if (i == GOHT_COLLIDER_BODYPART_MAX) {
            Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
        }
    }

    if (limbIndex == GOHT_LIMB_HEAD) {
        Matrix_MultVecX(3500.0f, &this->actor.focus.pos);
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        if (this->actionFunc != BossHakugin_DeathCutsceneRun) {
            Matrix_MtxFToYXZRot(Matrix_GetCurrent(), &this->headRot, false);
        }

        if (this->chargingLightOrbScale > 0.0f) {
            angle = 0x12C0 - this->headRot.z;
            cos = Math_CosS(angle);
            this->chargingLightningPos.x =
                this->actor.focus.pos.x - (Math_SinS(this->actor.shape.rot.y) * (60.0f * cos));
            this->chargingLightningPos.y = this->actor.focus.pos.y - (Math_SinS(angle) * 60.0f);
            this->chargingLightningPos.z =
                this->actor.focus.pos.z - (Math_CosS(this->actor.shape.rot.y) * (60.0f * cos));
        }
    }
}

/**
 * Draws the various rock effects that Goht or EnHakurock can spawn.
 */
void BossHakugin_DrawRockEffects(BossHakugin* this, PlayState* play) {
    GohtRockEffect* rockEffect;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gGohtRockMaterialDL);
    for (i = 0; i < GOHT_ROCK_EFFECT_COUNT; i++) {
        rockEffect = &this->rockEffects[i];
        if ((rockEffect->timer >= 0) && (rockEffect->type == GOHT_ROCK_EFFECT_TYPE_BOULDER)) {
            FrameInterpolation_RecordOpenChild(rockEffect, rockEffect->type);
            Matrix_SetTranslateRotateYXZ(rockEffect->pos.x, rockEffect->pos.y, rockEffect->pos.z, &rockEffect->rot);
            Matrix_Scale(rockEffect->scale, rockEffect->scale, rockEffect->scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gGohtRockModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    gSPDisplayList(POLY_OPA_DISP++, gGohtStalactiteMaterialDL);
    for (i = 0; i < GOHT_ROCK_EFFECT_COUNT; i++) {
        rockEffect = &this->rockEffects[i];
        if ((rockEffect->timer >= 0) && (rockEffect->type == GOHT_ROCK_EFFECT_TYPE_STALACTITE)) {
            FrameInterpolation_RecordOpenChild(rockEffect, rockEffect->type);
            Matrix_SetTranslateRotateYXZ(rockEffect->pos.x, rockEffect->pos.y, rockEffect->pos.z, &rockEffect->rot);
            Matrix_Scale(rockEffect->scale, rockEffect->scale, rockEffect->scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gGohtStalactiteModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws the black smoke component of the malfunction effect. The electrical component is drawn by EffectSsFhgFlash.
 */
void BossHakugin_DrawMalfunctionEffects(BossHakugin* this, PlayState* play) {
    GohtMalfunctionEffect* malfunctionEffect;
    s32 i;
    s32 j;
    s16 camYaw;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamMaterialDL);
    camYaw = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;
    gDPSetEnvColor(POLY_XLU_DISP++, 1, 1, 1, 128);

    for (i = 0; i < ARRAY_COUNT(this->malfunctionEffects); i++) {
        for (j = 0; j < ARRAY_COUNT(this->malfunctionEffects[0]); j++) {
            malfunctionEffect = &this->malfunctionEffects[i][j];
            if (malfunctionEffect->alpha > 0) {
                FrameInterpolation_RecordOpenChild(malfunctionEffect, j);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, malfunctionEffect->alpha);
                gSPSegment(POLY_XLU_DISP++, 0x08,
                           Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, malfunctionEffect->timer * 3,
                                              malfunctionEffect->timer * 15, 32, 64, 1, 0, 0, 32, 32, 3, 15, 0, 0));
                Matrix_Translate(malfunctionEffect->pos.x, malfunctionEffect->pos.y, malfunctionEffect->pos.z,
                                 MTXMODE_NEW);
                Matrix_RotateYS(camYaw, MTXMODE_APPLY);
                Matrix_Scale(malfunctionEffect->scaleXY, malfunctionEffect->scaleXY, 1.0f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws the electricity and light orb that appears in front of Goht's head as it charges up certain attacks.
 */
void BossHakugin_DrawChargingLightning(BossHakugin* this, PlayState* play) {
    s32 pad;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if (this->chargingLightOrbScale > 0.0f) {
        gDPSetEnvColor(POLY_XLU_DISP++, sLightningColor.r, sLightningColor.g, sLightningColor.b, 0);
        gSPDisplayList(POLY_XLU_DISP++, gGohtLightningMaterialDL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);

        for (i = -1; i <= 1; i += 2) {
            FrameInterpolation_RecordOpenChild(this, i);
            Matrix_SetTranslateRotateYXZ(this->chargingLightningPos.x, this->chargingLightningPos.y,
                                         this->chargingLightningPos.z, &this->headRot);
            Matrix_RotateYS(0x1400 * i, MTXMODE_APPLY);
            Matrix_RotateXS(0xC00 * i, MTXMODE_APPLY);
            Matrix_RotateZS(this->lightOrbRotZ, MTXMODE_APPLY);
            Matrix_Scale(0.62f, 0.62f, this->chargingLightningScale, MTXMODE_APPLY);
            Matrix_Translate(0.0f, 0.0f, this->chargingLightningTranslateZ * i, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gGohtLightningModelDL);

            Matrix_RotateZS(0x4000, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gGohtLightningModelDL);
            FrameInterpolation_RecordCloseChild();
        }

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, sLightColor.r, sLightColor.g, sLightColor.b, 0);
        Matrix_Translate(this->chargingLightningPos.x, this->chargingLightningPos.y, this->chargingLightningPos.z,
                         MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(this->chargingLightOrbScale, this->chargingLightOrbScale, this->chargingLightOrbScale,
                     MTXMODE_APPLY);
        Matrix_RotateZS(this->lightOrbRotZ, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gGohtLightOrbMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++, gGohtLightOrbModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws the segments of the lightning attack that Goht can shoot at the player while it's waiting.
 */
void BossHakugin_DrawLightningSegments(BossHakugin* this, PlayState* play) {
    GohtLightningSegment* lightningSegment;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, sLightningColor.r, sLightningColor.g, sLightningColor.b, 0);
    gSPDisplayList(POLY_XLU_DISP++, gGohtLightningMaterialDL);

    for (i = 0; i < GOHT_LIGHTNING_SEGMENT_COUNT; i++) {
        lightningSegment = &this->lightningSegments[i];

        if ((lightningSegment->alpha > 0) && (lightningSegment->alpha <= 255)) {
            FrameInterpolation_RecordOpenChild(lightningSegment, i);
            Matrix_SetTranslateRotateYXZ(lightningSegment->pos.x, lightningSegment->pos.y, lightningSegment->pos.z,
                                         &lightningSegment->rot);
            Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, lightningSegment->alpha);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gGohtLightningModelDL);

            Matrix_RotateZS(0x4000, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gGohtLightningModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws the electric balls that Goht can shoot at the player.
 */
void BossHakugin_DrawElectricBalls(BossHakugin* this, PlayState* play2) {
    s32 pad[4];
    PlayState* play = play2;
    s32 end;
    f32 scale;
    s32 i;
    s16 rotZ;
    s32 alpha;
    Vec3f* pos;

    if (this->electricBallState == GOHT_ELECTRIC_BALL_STATE_FADE_OUT) {
        i = GOHT_ELECTRIC_BALL_COUNT_MAX - 1;
        end = this->electricBallCount;
    } else if (this->electricBallCount != 0) {
        i = this->electricBallCount - 1;
        end = 0;
    } else {
        return;
    }

    scale = 30.0f - ((i - 1) * 1.5f);
    alpha = 215 - (i * 15);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, sLightColor.r, sLightColor.g, sLightColor.b, 0);
    rotZ = this->lightOrbRotZ;
    gSPDisplayList(POLY_XLU_DISP++, gGohtLightOrbMaterialDL);

    for (; i >= end; i--) {
        pos = &this->electricBallPos[i];

        FrameInterpolation_RecordOpenChild(pos, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, alpha);
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        Matrix_RotateZS(rotZ, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gGohtLightOrbModelDL);
        FrameInterpolation_RecordCloseChild();

        scale += 1.5f;
        alpha += 15;
        rotZ += 0x1000;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws the ice that encases Goht before the fight begins.
 */
void BossHakugin_DrawIce(BossHakugin* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    func_800B8118(&this->actor, play, 0);
    Matrix_Translate(this->iceCollider.dim.pos.x, this->iceCollider.dim.pos.y, this->iceCollider.dim.pos.z,
                     MTXMODE_NEW);
    Matrix_Scale(4.0f, this->iceScaleY, 4.0f, MTXMODE_APPLY);
    Matrix_RotateYS(0xF00, MTXMODE_APPLY);
    Matrix_RotateXS(0x500, MTXMODE_APPLY);
    Matrix_RotateZS(-0x500, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, 0, play->gameplayFrames & 0xFF, 32, 16, 1, 0,
                                  (play->gameplayFrames * 2) & 0xFF, 64, 32, 0, 1, 0, 2));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, this->iceAlpha);
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BossHakugin_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BossHakugin* this = (BossHakugin*)thisx;
    u8* tex = GRAPH_ALLOC(play->state.gfxCtx, GOHT_SHADOW_TEX_SIZE);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, sChinTexture);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          BossHakugin_OverrideLimbDraw, BossHakugin_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, GOHT_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->chargingLightningPos, &this->sfxPos);
    SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->electricBallPos[0], &this->electricBallSfxPos);

    if (!this->blockMalfunctionEffects) {
        BossHakugin_AddMalfunctionEffects(this, play);
    }

    // Setting `blockMalfunctionEffects` to true here ensures that `BossHakugin_AddMalfunctionEffects` won't be called
    // until `BossHakugin_Update` is called again, which is the only function that sets this to false. The player can
    // call draw functions over and over without ever calling update functions by pausing and unpausing rapidly; by
    // setting this to true, the developers ensured that the player couldn't add a potentially-infinite amount of
    // malfunction effects by repeatedly pausing.
    this->blockMalfunctionEffects = true;

    if (this->actor.colorFilterTimer != 0) {
        func_800AE5A0(play);
    }

    if (this->actionFunc == BossHakugin_DeathCutsceneCrushedByRocks) {
        BossHakugin_DrawCrushingRocks(&this->actor, play);
    } else {
        BossHakugin_DrawRockEffects(this, play);
    }

    BossHakugin_DrawMalfunctionEffects(this, play);
    BossHakugin_DrawChargingLightning(this, play);
    BossHakugin_DrawLightningSegments(this, play);
    BossHakugin_DrawElectricBalls(this, play);

    if (this->iceAlpha > 0) {
        BossHakugin_DrawIce(this, play);
    }

    if (this->actionFunc != BossHakugin_DeathCutsceneCrushedByRocks) {
        BossHakugin_GenShadowTex(tex, this);
        BossHakugin_DrawShadowTex(tex, this, play);
    }
}

/**
 * These four arrays encode circular shadow maps of various sizes. For an array of length N, the shadow map is N rows
 * tall, and each entry in the array describes the start and end point of the shadow within a given row (the exact
 * values of the start and end points are determined by the loops within BossHakugin_FillShadowTex). To illustrate using
 * the sShadowSmallMap as an example:
 * -3 -2 -1  0  1
 *  -------------
 *  0  0  1  0  0
 *  0  1  1  1  0
 *  1  1  1  1  1
 *  1  1  1  1  1
 *  0  1  1  1  0
 *  0  0  1  0  0
 */
static s32 sShadowSmallMap[] = {
    1, 2, 3, 3, 2, 1,
};

static s32 sShadowMediumMap[] = {
    2, 3, 4, 4, 4, 3, 2,
};

static s32 sShadowLargeMap[] = {
    2, 3, 4, 4, 4, 4, 3, 2,
};

static s32 sShadowExtraLargeMap[] = {
    2, 4, 5, 6, 7, 8, 8, 8, 8, 7, 6, 5, 4, 2,
};

static s32 sParentShadowBodyParts[GOHT_BODYPART_MAX] = {
    GOHT_BODYPART_THORAX,                // GOHT_BODYPART_PELVIS
    BODYPART_NONE,                       // GOHT_BODYPART_THORAX
    GOHT_BODYPART_THORAX,                // GOHT_BODYPART_HEAD
    GOHT_BODYPART_THORAX,                // GOHT_BODYPART_FRONT_RIGHT_UPPER_LEG
    GOHT_BODYPART_FRONT_RIGHT_UPPER_LEG, // GOHT_BODYPART_FRONT_RIGHT_LOWER_LEG
    GOHT_BODYPART_FRONT_RIGHT_LOWER_LEG, // GOHT_BODYPART_FRONT_RIGHT_HOOF
    GOHT_BODYPART_THORAX,                // GOHT_BODYPART_FRONT_LEFT_UPPER_LEG
    GOHT_BODYPART_FRONT_LEFT_UPPER_LEG,  // GOHT_BODYPART_FRONT_LEFT_LOWER_LEG
    GOHT_BODYPART_FRONT_LEFT_LOWER_LEG,  // GOHT_BODYPART_FRONT_LEFT_HOOF
    GOHT_BODYPART_PELVIS,                // GOHT_BODYPART_BACK_RIGHT_THIGH
    GOHT_BODYPART_BACK_RIGHT_THIGH,      // GOHT_BODYPART_BACK_RIGHT_SHIN
    GOHT_BODYPART_BACK_RIGHT_SHIN,       // GOHT_BODYPART_BACK_RIGHT_HOOF
    GOHT_BODYPART_PELVIS,                // GOHT_BODYPART_BACK_LEFT_THIGH
    GOHT_BODYPART_BACK_LEFT_THIGH,       // GOHT_BODYPART_BACK_LEFT_SHIN
    GOHT_BODYPART_BACK_LEFT_SHIN         // GOHT_BODYPART_BACK_LEFT_HOOF
};

static u8 sShadowSizes[GOHT_BODYPART_MAX] = {
    GOHT_SHADOW_SIZE_LARGE,       // GOHT_BODYPART_PELVIS
    GOHT_SHADOW_SIZE_EXTRA_LARGE, // GOHT_BODYPART_THORAX
    GOHT_SHADOW_SIZE_LARGE,       // GOHT_BODYPART_HEAD
    GOHT_SHADOW_SIZE_MEDIUM,      // GOHT_BODYPART_FRONT_RIGHT_UPPER_LEG
    GOHT_SHADOW_SIZE_SMALL,       // GOHT_BODYPART_FRONT_RIGHT_LOWER_LEG
    GOHT_SHADOW_SIZE_SMALL,       // GOHT_BODYPART_FRONT_RIGHT_HOOF
    GOHT_SHADOW_SIZE_MEDIUM,      // GOHT_BODYPART_FRONT_LEFT_UPPER_LEG
    GOHT_SHADOW_SIZE_SMALL,       // GOHT_BODYPART_FRONT_LEFT_LOWER_LEG
    GOHT_SHADOW_SIZE_SMALL,       // GOHT_BODYPART_FRONT_LEFT_HOOF
    GOHT_SHADOW_SIZE_MEDIUM,      // GOHT_BODYPART_BACK_RIGHT_THIGH
    GOHT_SHADOW_SIZE_SMALL,       // GOHT_BODYPART_BACK_RIGHT_SHIN
    GOHT_SHADOW_SIZE_SMALL,       // GOHT_BODYPART_BACK_RIGHT_HOOF
    GOHT_SHADOW_SIZE_MEDIUM,      // GOHT_BODYPART_BACK_LEFT_THIGH
    GOHT_SHADOW_SIZE_SMALL,       // GOHT_BODYPART_BACK_LEFT_SHIN
    GOHT_SHADOW_SIZE_SMALL        // GOHT_BODYPART_BACK_LEFT_HOOF
};

void BossHakugin_FillShadowTex(BossHakugin* this, u8* tex, f32 weight) {
    s32 index;
    s32 i;
    s32 baseX;
    s32 baseY;
    s32 addY;
    s32 x;
    s32 y = BODYPART_NONE;
    Vec3f lerp;
    Vec3f* parentBodyPartPos;
    Vec3f* bodyPartPos;
    Vec3f pos;
    Vec3f startVec;

    for (i = 0; i < GOHT_BODYPART_MAX; i++) {
        // TODO: match with a continue
        if ((weight == 0.0f) || (y = sParentShadowBodyParts[i]) > BODYPART_NONE) {
            bodyPartPos = &this->bodyPartsPos[i];
            if (weight > 0.0f) {
                parentBodyPartPos = &this->bodyPartsPos[y];

                VEC3F_LERPIMPDST(&lerp, bodyPartPos, parentBodyPartPos, weight);

                pos.x = lerp.x - this->actor.world.pos.x;
                pos.y = lerp.y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 600.0f;
                pos.z = lerp.z - this->actor.world.pos.z;
            } else {
                pos.x = bodyPartPos->x - this->actor.world.pos.x;
                pos.y = bodyPartPos->y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 600.0f;
                pos.z = bodyPartPos->z - this->actor.world.pos.z;
            }
            Matrix_MultVec3f(&pos, &startVec);

            startVec.x *= 0.07f;
            startVec.y *= 0.07f;

            baseX = (u16)(s32)(startVec.x + 32.0f);
            baseY = (u16)((s32)startVec.y * 64);

            if (sShadowSizes[i] == GOHT_SHADOW_SIZE_EXTRA_LARGE) {
                for (y = 0, addY = -384; y < ARRAY_COUNT(sShadowExtraLargeMap); y++, addY += 64) {
                    for (x = -sShadowExtraLargeMap[y]; x < sShadowExtraLargeMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < GOHT_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (sShadowSizes[i] == GOHT_SHADOW_SIZE_LARGE) {
                for (y = 0, addY = -256; y < ARRAY_COUNT(sShadowLargeMap); y++, addY += 64) {
                    for (x = -sShadowLargeMap[y]; x < sShadowLargeMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < GOHT_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (sShadowSizes[i] == GOHT_SHADOW_SIZE_MEDIUM) {
                for (y = 0, addY = -192; y < ARRAY_COUNT(sShadowMediumMap); y++, addY += 64) {
                    for (x = -sShadowMediumMap[y]; x < sShadowMediumMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < GOHT_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else {
                for (y = 0, addY = -128; y < ARRAY_COUNT(sShadowSmallMap); y++, addY += 64) {
                    for (x = -sShadowSmallMap[y]; x < sShadowSmallMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < GOHT_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            }
        }
    }
}

void BossHakugin_GenShadowTex(u8* tex, BossHakugin* this) {
    s32* iter = (s32*)tex;
    s16 i;

    for (i = 0; i < (s32)(GOHT_SHADOW_TEX_SIZE / sizeof(s32)); i++, iter++) {
        *iter = 0;
    }

    Matrix_RotateXFNew(1.0f);

    for (i = 0; i <= 5; i++) {
        BossHakugin_FillShadowTex(this, tex, i / 5.0f);
    }
}

/**
 * Draws Goht's dynamic shadow underneath it.
 */
void BossHakugin_DrawShadowTex(u8* tex, BossHakugin* this, PlayState* play) {
    s32 pad[2];
    f32 alpha;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    alpha = (400.0f - this->actor.world.pos.y) * (1.0f / 400.0f);
    alpha = CLAMP_MIN(alpha, 0.0f);
    alpha = CLAMP_MAX(alpha, 1.0f);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (s8)(alpha * 80.0f));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    if (this->actor.floorPoly != NULL) {
        func_800C0094(this->actor.floorPoly, this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z,
                      &mtx);
        Matrix_Put(&mtx);
    }

    Matrix_Scale(4.25f, 1.0f, 4.25f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gShadowMaterialDL);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_I, G_IM_SIZ_8b, GOHT_SHADOW_TEX_WIDTH, GOHT_SHADOW_TEX_HEIGHT, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, gShadowModelDL);

    CLOSE_DISPS(gfxCtx);
}

/**
 * Spawns the rocks that fall down and crush Goht in its death cutscene. Each rock is placed in the air with a falling
 * speed of zero; `BossHakugin_DeathCutsceneCrushedByRocks` is responsible for setting the falling speed to a non-zero
 * value when it wants a rock to start falling.
 */
void BossHakugin_SpawnCrushingRocks(BossHakugin* this) {
    s32 i;
    Vec3f pos;
    s32 num = 15;
    GohtCrushingRock* crushingRock = this->crushingRocks;
    s32 j;
    s32 rockCount;
    f32 sinY = Math_SinS(this->actor.shape.rot.y) * 65.0f;
    f32 cosY = Math_CosS(this->actor.shape.rot.y) * 65.0f;
    f32 sinZ = Math_SinS(this->actor.shape.rot.z) * 65.0f;
    f32 scale;
    f32 offsetX1;
    f32 offsetZ1;
    f32 offsetX2;
    f32 offsetZ2;
    s32 pad;

    for (i = 4; i > 0; i--, num -= 4) {
        offsetX2 = (30.0f / 13.0f) * sinY;
        offsetZ2 = (30.0f / 13.0f) * cosY;

        pos.x = this->actor.world.pos.x - (i * sinY) + offsetX2;
        pos.y = this->actor.world.pos.y + (85.0f * (4 - i)) + 20.0f;
        pos.z = this->actor.world.pos.z - (i * cosY) + offsetZ2;

        crushingRock->scale = ((i * 4.5f) + 22.0f) * 0.001f;
        Math_Vec3f_Copy(&crushingRock->pos, &pos);

        rockCount = num >> 1;
        if (rockCount == 0) {
            break;
        }

        crushingRock++;

        scale = i / (f32)rockCount;
        offsetX1 = (cosY + sinY) * scale;
        offsetZ1 = (cosY - sinY) * scale;

        for (j = 0; j < rockCount; j++, crushingRock++) {
            crushingRock->pos.x = pos.x + offsetX1 * (rockCount - j);
            crushingRock->pos.y = pos.y + i * sinZ;
            crushingRock->pos.z = pos.z + offsetZ1 * (rockCount - j);
            crushingRock->scale = ((i * 4.5f) + 22.0f) * 0.001f;
        }

        offsetX2 = (sinY - cosY) * scale;
        offsetZ2 = (cosY + sinY) * scale;

        for (j = 0; j < rockCount; j++, crushingRock++) {
            crushingRock->pos.x = pos.x + offsetX2 * (rockCount - j);
            crushingRock->pos.y = pos.y - i * sinZ;
            crushingRock->pos.z = pos.z + offsetZ2 * (rockCount - j);
            crushingRock->scale = ((i * 4.5f) + 22.0f) * 0.001f;
        }
    }

    for (i = 0; i < GOHT_CRUSHING_ROCK_COUNT; i++) {
        crushingRock = &this->crushingRocks[i];

        crushingRock->scale += Rand_ZeroFloat(5.0f) * 0.001f;
        crushingRock->rot.x = (s32)Rand_Next() >> 0x10;
        crushingRock->rot.y = (s32)Rand_Next() >> 0x10;
        crushingRock->rot.z = (s32)Rand_Next() >> 0x10;
        crushingRock->targetPosY = crushingRock->pos.y;
        crushingRock->fallingSpeed = 0.0f;
        crushingRock->pos.y += 850.0f;
    }
}

/**
 * This function uses Goht's body parts colliders to set the collision info for the rocks that crushed Goht. Since Goht
 * is always buried under rocks when this function is called, using its body parts colliders in this manner is fine;
 * they aren't currently being used for their original purpose or for anything else.
 */
void BossHakugin_UpdateCrushingRocksCollision(BossHakugin* this) {
    s32 i;
    GohtCrushingRock* crushingRock;
    ColliderJntSphElement* jntSphElem;

    for (i = 0; i < GOHT_CRUSHING_ROCK_COUNT / 2; i++) {
        crushingRock = &this->crushingRocks[i << 1];
        jntSphElem = &this->bodyCollider.elements[i];

        jntSphElem->dim.worldSphere.center.x = crushingRock->pos.x;
        jntSphElem->dim.worldSphere.center.y = crushingRock->pos.y;
        jntSphElem->dim.worldSphere.center.z = crushingRock->pos.z;
        jntSphElem->dim.worldSphere.radius = crushingRock->scale * 3000.0f;
        jntSphElem->base.acDmgInfo.dmgFlags = 0xF3CFBBFF;
        jntSphElem->base.acElemFlags &= ~ACELEM_NO_HITMARK;
        jntSphElem->base.elemMaterial = ELEM_MATERIAL_UNK0;
    }

    // This for-loop will update the collider for GOHT_COLLIDER_BODYPART_LEFT_HORN.
    for (; i < ARRAY_COUNT(this->bodyColliderElements); i++) {
        this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_ON;
        this->bodyCollider.elements[i].base.ocElemFlags &= ~OCELEM_ON;
    }

    this->bodyCollider.base.colMaterial = COL_MATERIAL_HARD;
}

/**
 * Updates the rocks that crushed Goht after the boss fight is over. The rocks in this state don't actually move; the
 * only thing this function does is update the collision for the rocks.
 */
void BossHakugin_UpdateStationaryCrushingRocks(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossHakugin* this = (BossHakugin*)thisx;

    BossHakugin_UpdateCrushingRocksCollision(this);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
}

/**
 * Draws the rocks that crush Goht, both when they are falling down and after they become stationary.
 */
void BossHakugin_DrawCrushingRocks(Actor* thisx, PlayState* play) {
    BossHakugin* this = (BossHakugin*)thisx;
    s32 i;
    GohtCrushingRock* crushingRock;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gGohtRockMaterialDL);

    for (i = 0; i < GOHT_CRUSHING_ROCK_COUNT; i++) {
        crushingRock = &this->crushingRocks[i];

        FrameInterpolation_RecordOpenChild(crushingRock, i);
        Matrix_SetTranslateRotateYXZ(crushingRock->pos.x, crushingRock->pos.y, crushingRock->pos.z, &crushingRock->rot);
        Matrix_Scale(crushingRock->scale, crushingRock->scale, crushingRock->scale, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gGohtRockModelDL);
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

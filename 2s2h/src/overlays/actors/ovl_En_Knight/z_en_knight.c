/*
 * File: z_en_knight.c
 * Overlay: ovl_En_Knight
 * Description: Igos du Ikana and his lackeys
 */

#include "prevent_bss_reordering.h"
#include "z_en_knight.h"
#include "z64shrink_window.h"
#include "overlays/actors/ovl_Mir_Ray3/z_mir_ray3.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_knight/object_knight.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnKnight_Init(Actor* thisx, PlayState* play);
void EnKnight_Destroy(Actor* thisx, PlayState* play);
void EnKnight_Update(Actor* thisx, PlayState* play);
void EnKnight_Draw(Actor* thisx, PlayState* play);

void EnKnight_UpdateAfterImage(Actor* thisx, PlayState* play);
void EnKnight_DrawAfterImage(Actor* thisx, PlayState* play);

void EnKnight_TelegraphHeavyAttack(EnKnight* this, PlayState* play);
void EnKnight_SetupHeavyAttack(EnKnight* this, PlayState* play);
void EnKnight_HeavyAttack(EnKnight* this, PlayState* play);
void EnKnight_SetupLowSwing(EnKnight* this, PlayState* play);
void EnKnight_LowSwing(EnKnight* this, PlayState* play);
void EnKnight_SetupLowSwingEnd(EnKnight* this, PlayState* play);
void EnKnight_LowSwingEnd(EnKnight* this, PlayState* play);
void EnKnight_SetupBasicSwing(EnKnight* this, PlayState* play);
void EnKnight_BasicSwing(EnKnight* this, PlayState* play);
void EnKnight_JumpAttack(EnKnight* this, PlayState* play);
void EnKnight_Blocking(EnKnight* this, PlayState* play);
void EnKnight_SetupWait(EnKnight* this, PlayState* play);
void EnKnight_Wait(EnKnight* this, PlayState* play);
void EnKnight_Strafe(EnKnight* this, PlayState* play);
void EnKnight_SetupTurnToPlayer(EnKnight* this, PlayState* play);
void EnKnight_TurnToPlayer(EnKnight* this, PlayState* play);
void EnKnight_SetupApproachPlayer(EnKnight* this, PlayState* play);
void EnKnight_ApproachPlayer(EnKnight* this, PlayState* play);
void EnKnight_Retreat(EnKnight* this, PlayState* play);
void EnKnight_GetBackUp(EnKnight* this, PlayState* play);
void EnKnight_Stunned(EnKnight* this, PlayState* play);
void EnKnight_RecoilFromDamage(EnKnight* this, PlayState* play);
void EnKnight_FallOver(EnKnight* this, PlayState* play);
void EnKnight_Die(EnKnight* this, PlayState* play);
void EnKnight_Dead(EnKnight* this, PlayState* play);
void EnKnight_SetupJumpBackwardsIgos(EnKnight* this, PlayState* play);
void EnKnight_JumpBackwards(EnKnight* this, PlayState* play);
void EnKnight_IgosReactToCurtains(EnKnight* this, PlayState* play);
void EnKnight_LookAtOther(EnKnight* this, PlayState* play);
void EnKnight_March(EnKnight* this, PlayState* play);
void EnKnight_SetupIntroCutscene(EnKnight* this, PlayState* play);
void EnKnight_SetupIgosSitting(EnKnight* this, PlayState* play);
void EnKnight_IgosSitting(EnKnight* this, PlayState* play);
void EnKnight_SetupDetachHead(EnKnight* this, PlayState* play);
void EnKnight_DetachHead(EnKnight* this, PlayState* play);
void EnKnight_FlyingHeadDone(EnKnight* this, PlayState* play);
void EnKnight_SetupBreathAttack(EnKnight* this, PlayState* play);
void EnKnight_BreathAttack(EnKnight* this, PlayState* play);
void EnKnight_SetupIgosStandCS(EnKnight* this, PlayState* play);
void EnKnight_IgosStandCS(EnKnight* this, PlayState* play);
void EnKnight_KnightCaptainsHatCS(EnKnight* this, PlayState* play);
void EnKnight_CaptainsHatCS(EnKnight* this, PlayState* play);
void EnKnight_WaitIntroCutscene(EnKnight* this, PlayState* play);
void EnKnight_IntroCutscene(EnKnight* this, PlayState* play);
void EnKnight_SetupFlyingHead(EnKnight* this, PlayState* play);
void EnKnight_FlyingHead(EnKnight* this, PlayState* play);
void EnKnight_SetupFlyingHeadAttack(EnKnight* this, PlayState* play);
void EnKnight_FlyingHeadAttack(EnKnight* this, PlayState* play);

void EnKnight_UpdateEffects(EnKnight* this, PlayState* play);
void EnKnight_DrawEffects(EnKnight* this, PlayState* play);

void EnKnight_SetupTelegraphHeavyAttack(EnKnight* this, PlayState* play, s32 noTelegraph);
void EnKnight_SetupStrafe(EnKnight* this, PlayState* play, s16 strafeAngle);
void EnKnight_SetupRetreat(EnKnight* this, PlayState* play, u8 shielding);

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ UNK_TYPE1 unk_24[0x6];
    /* 0x2A */ u8 active;
    /* 0x2C */ s16 scrollTimer;
    /* 0x2E */ s16 alpha;
    /* 0x30 */ s16 unk_30; // set to 0 but unused
    /* 0x34 */ f32 scale;
    /* 0x38 */ f32 scaleTarget;
} EnKnightEffect; // size = 0x3C

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

u8 sSpawnIndex = 0;

u8 sKnightMusicStartTimer = 0;

typedef enum {
    // None
    KNIGHT_DMGEFF_STATE_0 = 0,

    // Fire
    KNIGHT_DMGEFF_STATE_1,
    KNIGHT_DMGEFF_STATE_2,

    // Ice
    KNIGHT_DMGEFF_STATE_10 = 10,
    KNIGHT_DMGEFF_STATE_11,

    // Light
    KNIGHT_DMGEFF_STATE_20 = 20,
    KNIGHT_DMGEFF_STATE_21,

    // Light again?
    KNIGHT_DMGEFF_STATE_30 = 30,
    KNIGHT_DMGEFF_STATE_31,

    // Zora Barrier?
    KNIGHT_DMGEFF_STATE_40 = 40,
    KNIGHT_DMGEFF_STATE_41
} KnightDamageEffectState;

typedef enum {
    /*  0 */ KNIGHT_DMGEFF_NONE,
    /*  1 */ KNIGHT_DMGEFF_STUN,
    /*  2 */ KNIGHT_DMGEFF_FIRE,
    /*  3 */ KNIGHT_DMGEFF_ICE,
    /*  4 */ KNIGHT_DMGEFF_LIGHT,
    /* 10 */ KNIGHT_DMGEFF_ZORA_BARRIER = 10,
    /* 13 */ KNIGHT_DMGEFF_GORON_POUND = 13,
    /* 14 */ KNIGHT_DMGEFF_LIGHT_RAY,
    /* 15 */ KNIGHT_DMGEFF_OTHER
} EnKnightDamageEffect;

static DamageTable sDamageTableStanding = {
    /* Deku Nut       */ DMG_ENTRY(0, KNIGHT_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Horse trample  */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Zora boomerang */ DMG_ENTRY(0, KNIGHT_DMGEFF_STUN),
    /* Normal arrow   */ DMG_ENTRY(3, KNIGHT_DMGEFF_OTHER),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, KNIGHT_DMGEFF_OTHER),
    /* Goron punch    */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Sword          */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Goron pound    */ DMG_ENTRY(0, KNIGHT_DMGEFF_GORON_POUND),
    /* Fire arrow     */ DMG_ENTRY(1, KNIGHT_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, KNIGHT_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, KNIGHT_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Deku spin      */ DMG_ENTRY(0, KNIGHT_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(2, KNIGHT_DMGEFF_OTHER),
    /* Deku launch    */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(0, KNIGHT_DMGEFF_ZORA_BARRIER),
    /* Normal shield  */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, KNIGHT_DMGEFF_LIGHT_RAY),
    /* Thrown object  */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Zora punch     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Spin attack    */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Sword beam     */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
};

static DamageTable sDamageTableFallenOver = {
    /* Deku Nut       */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Horse trample  */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Zora boomerang */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Normal arrow   */ DMG_ENTRY(3, KNIGHT_DMGEFF_OTHER),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, KNIGHT_DMGEFF_OTHER),
    /* Goron punch    */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Sword          */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Goron pound    */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Fire arrow     */ DMG_ENTRY(1, KNIGHT_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, KNIGHT_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, KNIGHT_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Deku spin      */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Deku bubble    */ DMG_ENTRY(2, KNIGHT_DMGEFF_OTHER),
    /* Deku launch    */ DMG_ENTRY(2, KNIGHT_DMGEFF_OTHER),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(0, KNIGHT_DMGEFF_ZORA_BARRIER),
    /* Normal shield  */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(1, KNIGHT_DMGEFF_GORON_POUND),
    /* Thrown object  */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Zora punch     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Spin attack    */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
    /* Sword beam     */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, KNIGHT_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, KNIGHT_DMGEFF_OTHER),
};

static ColliderJntSphElementInit sKnightSwordColliderJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 27 }, 100 },
    },
};

static ColliderJntSphElementInit sIgosSwordColliderJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 32 }, 100 },
    },
};

static ColliderJntSphInit sKnightSwordColliderJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sKnightSwordColliderJntSphElementsInit),
    sKnightSwordColliderJntSphElementsInit,
};

static ColliderJntSphInit sIgosSwordColliderJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sIgosSwordColliderJntSphElementsInit),
    sIgosSwordColliderJntSphElementsInit,
};

static ColliderJntSphElementInit sShieldColliderJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 27 }, 100 },
    },
};

static ColliderJntSphInit sShieldColliderJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sShieldColliderJntSphElementsInit),
    sShieldColliderJntSphElementsInit,
};

static ColliderJntSphElementInit sBodyColliderJntSphElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7EFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7EFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 17 }, 100 },
    },
};

static ColliderJntSphInit sBodyColliderJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sBodyColliderJntSphElementsInit),
    sBodyColliderJntSphElementsInit,
};

static ColliderCylinderInit sHeadAttackColliderCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x10 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 15, 20, -10, { 0, 0, 0 } },
};

static ColliderCylinderInit sHeadColliderCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ALL,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_ENEMY | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7FFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

static Color_RGBA8 sDustPrimColor = { 60, 50, 20, 255 };

static Color_RGBA8 sDustEnvColor = { 40, 30, 30, 255 };

const ActorProfile En_Knight_Profile = {
    /**/ ACTOR_EN_KNIGHT,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_KNIGHT,
    /**/ sizeof(EnKnight),
    /**/ EnKnight_Init,
    /**/ EnKnight_Destroy,
    /**/ EnKnight_Update,
    /**/ EnKnight_Draw,
};

u16 sKnightWalkSfx[3] = { NA_SE_EN_BOSU_WALK, NA_SE_EN_YASE_WALK, NA_SE_EN_DEBU_WALK };
u16 sKnightPauseSfx[3] = { NA_SE_EN_BOSU_LAUGH_K, NA_SE_EN_YASE_PAUSE_K, NA_SE_EN_DEBU_PAUSE_K };
u16 sKnightLaughSfx[3] = { NA_SE_EN_BOSU_LAUGH_K, NA_SE_EN_YASE_LAUGH_K, NA_SE_EN_DEBU_LAUGH_K };
u16 sKnightAttackSfx[3] = { NA_SE_EN_BOSU_ATTACK_W, NA_SE_EN_YASE_ATTACK_W, NA_SE_EN_DEBU_ATTACK_W };
u16 sKnightDamagedSfx[3] = { NA_SE_EN_BOSU_DAMAGE, NA_SE_EN_YASE_DAMAGE, NA_SE_EN_DEBU_DAMAGE };
u16 sKnightDefeatedSfx[3] = { NA_SE_EN_BOSU_DEAD, NA_SE_EN_YASE_DEAD, NA_SE_EN_DEBU_DEAD };
u16 sKnightVoiceSfx[3] = { NA_SE_EN_BOSU_DEAD_VOICE, NA_SE_EN_YASE_DEAD_VOICE, NA_SE_EN_DEBU_DEAD_VOICE };

EnKnight* sIgosInstance;
EnKnight* sThinKnightInstance;
EnKnight* sWideKnightInstance;
EnKnight* sIgosHeadInstance;
EnKnight* sTargetKnight; // During some actions, the other knight will use this knight as a target
MirRay3* sMirRayInstance;
EnKnightEffect sEnKnightEffects[100];

// Cutscene states for the intro cutscene
typedef enum {
    /* 0 */ KNIGHT_INTRO_CS_STATE_0,
    /* 1 */ KNIGHT_INTRO_CS_STATE_1,
    /* 2 */ KNIGHT_INTRO_CS_STATE_2,
    /* 3 */ KNIGHT_INTRO_CS_STATE_3,
    /* 4 */ KNIGHT_INTRO_CS_STATE_4,
    /* 5 */ KNIGHT_INTRO_CS_STATE_5,
    /* 6 */ KNIGHT_INTRO_CS_STATE_6,
    /* 7 */ KNIGHT_INTRO_CS_STATE_7,
    /* 8 */ KNIGHT_INTRO_CS_STATE_8
} EnKnightIntroCsState;

// Cutscene states for the end of phase 1 cutscene
typedef enum {
    /* 0 */ KNIGHT_CS_1_STATE_0,
    /* 1 */ KNIGHT_CS_1_STATE_1,
    /* 2 */ KNIGHT_CS_1_STATE_2
} EnKnightIgosStandCsState;

// Cutscene states for the captains hat cutscene
typedef enum {
    /* 0 */ KNIGHT_CS_2_STATE_0,
    /* 1 */ KNIGHT_CS_2_STATE_1,
    /* 2 */ KNIGHT_CS_2_STATE_2,
    /* 3 */ KNIGHT_CS_2_STATE_3,
    /* 4 */ KNIGHT_CS_2_STATE_4
} EnKnightCaptainsHatCsState;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_RETREAT_0,
    /* 1 */ KNIGHT_SUB_ACTION_RETREAT_1,
    /* 2 */ KNIGHT_SUB_ACTION_RETREAT_2,
    /* 3 */ KNIGHT_SUB_ACTION_RETREAT_3
} EnKnightSubActionRetreat;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_RECOIL_0,
    /* 1 */ KNIGHT_SUB_ACTION_RECOIL_1
} EnKnightSubActionRecoil;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_JUMP_BACKWARDS_0,
    /* 1 */ KNIGHT_SUB_ACTION_JUMP_BACKWARDS_1
} EnKnightSubActionJumpBackwards;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_MARCH_0,
    /* 1 */ KNIGHT_SUB_ACTION_MARCH_1,
    /* 2 */ KNIGHT_SUB_ACTION_MARCH_2
} EnKnightSubActionMarch;

typedef enum {
    /*  0 */ KNIGHT_SUB_ACTION_IGOS_SITTING_0,
    /*  1 */ KNIGHT_SUB_ACTION_IGOS_SITTING_1,
    /*  2 */ KNIGHT_SUB_ACTION_IGOS_SITTING_2,
    /*  5 */ KNIGHT_SUB_ACTION_IGOS_SITTING_5 = 5,
    /*  6 */ KNIGHT_SUB_ACTION_IGOS_SITTING_6,
    /* 10 */ KNIGHT_SUB_ACTION_IGOS_SITTING_10 = 10,
    /* 11 */ KNIGHT_SUB_ACTION_IGOS_SITTING_11
} EnKnightSubActionIgosSitting;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_0,
    /* 1 */ KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_1,
    /* 2 */ KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_2
} EnKnightSubActionFlyingHeadDone;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_BREATH_ATTACK_0,
    /* 1 */ KNIGHT_SUB_ACTION_BREATH_ATTACK_1,
    /* 2 */ KNIGHT_SUB_ACTION_BREATH_ATTACK_2
} EnKnightSubActionBreathAttack;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_FLYING_HEAD_0,
    /* 1 */ KNIGHT_SUB_ACTION_FLYING_HEAD_1,
    /* 2 */ KNIGHT_SUB_ACTION_FLYING_HEAD_2,
    /* 3 */ KNIGHT_SUB_ACTION_FLYING_HEAD_3
} EnKnightSubActionFlyingHead;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_0,
    /* 1 */ KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_1,
    /* 2 */ KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_2
} EnKnightSubActionFlyingHeadAttack;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_CAPTAINS_HAT_CS_IGOS_SITTING,
    /* 1 */ KNIGHT_SUB_ACTION_CAPTAINS_HAT_CS_IGOS_STANDING
} EnKnightSubActionCaptainsHatCS;

void EnKnight_EffectAdd(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, f32 scaleTarget,
                        s16 alpha) {
    s16 i;
    EnKnightEffect* eff;

    for (eff = play->specialEffects, i = 0; i < ARRAY_COUNT(sEnKnightEffects); i++, eff++) {
        if (!eff->active) {
            eff->active = true;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = *accel;
            eff->unk_30 = 0;
            eff->scale = (BREG(50) * 0.0001f + 0.001f) * scale;
            eff->scaleTarget = (BREG(51) * 0.0001f + 0.001f) * scaleTarget;
            eff->alpha = alpha;
            eff->scrollTimer = 0;
            break;
        }
    }
}

void EnKnight_SetColliderSphere(EnKnight* this, s32 elemNum, ColliderJntSph* jntSph, Vec3f* pos) {
    f32 scale = (this == sIgosInstance) ? 1.3076924f : 1.0f;

    if (this->actionFunc == EnKnight_FallOver) {
        scale *= 2.0f;
    }

    jntSph->elements[elemNum].dim.worldSphere.center.x = pos->x;
    jntSph->elements[elemNum].dim.worldSphere.center.y = pos->y;
    jntSph->elements[elemNum].dim.worldSphere.center.z = pos->z;
    jntSph->elements[elemNum].dim.worldSphere.radius =
        jntSph->elements[elemNum].dim.modelSphere.radius * scale * jntSph->elements[elemNum].dim.scale;
}

void EnKnight_SpawnDustAtFeet(EnKnight* this, PlayState* play, u8 timerMask) {
    u8 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if (!(this->randTimer & timerMask) && !this->isHeadless &&
        ((this->animMovementX > 1.0f) || (this->animMovementZ > 1.0f) || (timerMask == 0) ||
         (this->actor.speed > 1.0f))) {
        for (i = 0; i < ARRAY_COUNT(this->feetPositions); i++) {
            velocity.x = Rand_CenteredFloat(3.0f);
            velocity.y = Rand_ZeroFloat(1.0f);
            velocity.z = Rand_CenteredFloat(3.0f);
            accel.y = -0.1f;
            accel.x = accel.z = 0.0f;
            pos.x = this->feetPositions[i].x + Rand_CenteredFloat(10.0f);
            pos.y = (Rand_ZeroFloat(5.0f) + this->actor.floorHeight) - 5.0f;
            pos.z = this->feetPositions[i].z + Rand_CenteredFloat(10.0f);
            func_800B0EB0(play, &pos, &velocity, &accel, &sDustPrimColor, &sDustEnvColor,
                          Rand_ZeroFloat(80.0f) + 150.0f, 10, Rand_ZeroFloat(5.0f) + 14.0f);
        }
    }
}

void EnKnight_Init(Actor* thisx, PlayState* play) {
    EnKnight* this = (EnKnight*)thisx;
    s32 pad;
    s32 sfxIndex;

    // Common init

    this->actor.attentionRangeType = ATTENTION_RANGE_5;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.damageTable = &sDamageTableStanding;
    this->bodyAlpha = 255.0f;
    this->randTimer = Rand_ZeroFloat(1000.0f);
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;

    if (this->actor.params == EN_KNIGHT_PARAM_IGOS_HEAD) {
        // Flying head init

        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 12.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gIgosSkel, &gKnightIgosRemoveHeadAnim, this->jointTable,
                           this->morphTable, IGOS_LIMB_MAX);
        Actor_SetScale(&this->actor, KREG(12) * 0.001f + 0.017f);
        EnKnight_SetupFlyingHead(this, play);
        Collider_InitAndSetCylinder(play, &this->headAttackCollider, &this->actor, &sHeadAttackColliderCylinderInit);
        Collider_InitAndSetCylinder(play, &this->headCollider, &this->actor, &sHeadColliderCylinderInit);
        this->actor.colChkInfo.health = 10 - BREG(40);
        return;
    }

    if (this->actor.params >= EN_KNIGHT_PARAM_KNIGHT_BODY_AFTERIMAGE) {
        // Afterimages (and cutscene?) init

        this->actor.update = EnKnight_UpdateAfterImage;
        this->actor.draw = EnKnight_DrawAfterImage;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

        if (1) {} //! FAKE:

        if (this->actor.params == EN_KNIGHT_PARAM_KNIGHT_BODY_AFTERIMAGE) {
            SkelAnime_InitFlex(play, &this->skelAnime, &gKnightSkel, &gKnightIdleAnim, this->jointTable,
                               this->morphTable, KNIGHT_LIMB_MAX);
            Actor_SetScale(&this->actor, KREG(13) * 0.001f + 0.013f);
        } else {
            SkelAnime_InitFlex(play, &this->skelAnime, &gIgosSkel, &gKnightIdleAnim, this->jointTable, this->morphTable,
                               IGOS_LIMB_MAX);
            Actor_SetScale(&this->actor, KREG(13) * 0.001f + 0.017f);
        }

        if (this->actor.params == EN_KNIGHT_PARAM_IGOS_HEAD_AFTERIMAGE) {
            this->bodyAlpha = BREG(79) + 100.0f;
        } else {
            this->bodyAlpha = BREG(77) + 120.0f;
        }
        return;
    }

    // Igos + Others main init

    this->actor.hintId = TATL_HINT_ID_IGOS_DU_IKANA;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    Collider_InitAndSetJntSph(play, &this->shieldCollider, &this->actor, &sShieldColliderJntSphInit,
                              this->shieldColliderElements);
    Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sBodyColliderJntSphInit,
                              this->bodyColliderElements);

    if (this->actor.params == EN_KNIGHT_PARAM_OTHERS) {
        // Others

        Collider_InitAndSetJntSph(play, &this->swordCollider, &this->actor, &sKnightSwordColliderJntSphInit,
                                  this->swordColliderElements);
        SkelAnime_InitFlex(play, &this->skelAnime, &gKnightSkel, &gKnightIdleAnim, this->jointTable, this->morphTable,
                           KNIGHT_LIMB_MAX);
        this->actor.colChkInfo.health = 6 - BREG(40);
        Actor_SetScale(&this->actor, KREG(13) * 0.001f + 0.013f);
    } else {
        // Igos

        Collider_InitAndSetJntSph(play, &this->swordCollider, &this->actor, &sIgosSwordColliderJntSphInit,
                                  this->swordColliderElements);
        SkelAnime_InitFlex(play, &this->skelAnime, &gIgosSkel, &gKnightIdleAnim, this->jointTable, this->morphTable,
                           IGOS_LIMB_MAX);
        this->actor.colChkInfo.health = 14 - BREG(41);
        Actor_SetScale(&this->actor, KREG(12) * 0.001f + 0.017f);
        this->igosCurtainReaction = Rand_ZeroFloat(1.9999f); // random 0 or 1
    }

    this->neckRotationStepScale = 3;
    this->shadowAlphaFactor = 180.0f;
    sfxIndex = sSpawnIndex & 3;
    sSpawnIndex++;
    this->walkSfx = sKnightWalkSfx[sfxIndex];
    this->pauseSfx = sKnightPauseSfx[sfxIndex];
    this->laughSfx = sKnightLaughSfx[sfxIndex];
    this->attackSfx = sKnightAttackSfx[sfxIndex];
    this->damagedSfx = sKnightDamagedSfx[sfxIndex];
    this->defeatedSfx = sKnightDefeatedSfx[sfxIndex];
    this->voiceSfx = sKnightVoiceSfx[sfxIndex];

    if (this->actor.params != EN_KNIGHT_PARAM_OTHERS) {
        // Igos
        s32 i;

        play->specialEffects = sEnKnightEffects;

        for (i = 0; i < ARRAY_COUNT(sEnKnightEffects); i++) {
            sEnKnightEffects[i].active = false;
        }

        sIgosInstance = this;
        sTargetKnight = NULL;
        sIgosHeadInstance = NULL;
        this->actor.shape.rot.y = -0x4000;
        sThinKnightInstance = (EnKnight*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KNIGHT, 1398.0f, 66.0f, 2730.0f,
                                                     this->actor.shape.rot.x, this->actor.shape.rot.y,
                                                     this->actor.shape.rot.z, EN_KNIGHT_PARAM_OTHERS);
        sWideKnightInstance = (EnKnight*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KNIGHT, 1398.0f, 66.0f, 2969.0f,
                                                     this->actor.shape.rot.x, this->actor.shape.rot.y,
                                                     this->actor.shape.rot.z, EN_KNIGHT_PARAM_OTHERS);

        // Spawn curtains
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_06, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_06, 0.0f, 0.0f, 0.0f, 0, 0, 0, 1);
    }

    if (CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_IGOS_DU_IKANA) && !Flags_GetClear(play, play->roomCtx.curRoom.num)) {
        if (this == sIgosInstance) {
            this->actor.world.pos.x = BREG(27) + 1376.0f;
            this->actor.world.pos.y = 45.0f;
            this->actor.world.pos.z = BREG(28) + 2864.0f;
            EnKnight_SetupIgosSitting(this, play);
            this->skelAnime.curFrame = 25.0f;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            sKnightMusicStartTimer = KREG(83) + 40;
        } else {
            EnKnight_SetupWait(this, play);
            this->doBgChecks = true;
            this->swordScale = this->shieldScale = 1.0f;
            this->actor.gravity = -1.5f;
            this->subCamFov = 60.0f;
        }

        play->envCtx.lightSettingOverride = 7;
    } else {
        EnKnight_SetupIntroCutscene(this, play);
        play->envCtx.lightSettingOverride = 0;
    }
}

void EnKnight_Destroy(Actor* thisx, PlayState* play) {
}

void EnKnight_CheckRetreat(EnKnight* this, PlayState* play) {
    Player* player;
    Actor* explosive;
    f32 px;
    f32 pz;
    f32 dx;
    f32 dz;

    if (this->isHeadless) {
        return;
    }

    player = GET_PLAYER(play);

    for (explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first; explosive != NULL;
         explosive = explosive->next) {
        px = this->actor.world.pos.x;
        pz = this->actor.world.pos.z;
        dx = explosive->world.pos.x - px;
        dz = explosive->world.pos.z - pz;
        if (sqrtf(SQ(dx) + SQ(dz)) < 100.0f) {
            this->retreatTowards.x = px - dx * 100.0f;
            this->retreatTowards.z = pz - dz * 100.0f;
            EnKnight_SetupRetreat(this, play, false);
        }
    }

    if ((sMirRayInstance != NULL) && (sMirRayInstance->unk_214 > 0.1f) &&
        (this->actor.xzDistToPlayer <= (BREG(70) + 300.0f))) {
        px = this->actor.world.pos.x;
        pz = this->actor.world.pos.z;
        dx = player->actor.world.pos.x - px;
        dz = player->actor.world.pos.z - pz;
        this->retreatTowards.x = px - dx * 100.0f;
        this->retreatTowards.z = pz - dz * 100.0f;
        EnKnight_SetupRetreat(this, play, true);
    }
}

s32 EnKnight_SelectAttack(EnKnight* this, PlayState* play) {
    f32 scale;
    s32 actionChanged = false;
    Player* player = GET_PLAYER(play);

    scale = (this == sIgosInstance) ? 1.3076924f : 1.0f;

    if ((this->actor.xzDistToPlayer <= 90.0f * scale) && (this->randTimer % 8 == 0)) {
        if ((Rand_ZeroOne() < 0.5f) && (sIgosHeadInstance != (EnKnight*)player->actor.parent)) {
            EnKnight_SetupBasicSwing(this, play);
        } else if (Rand_ZeroOne() < 0.5f) {
            EnKnight_SetupTelegraphHeavyAttack(this, play, false);
        } else {
            EnKnight_SetupLowSwing(this, play);
        }
        actionChanged = true;
    }

    if ((this == sIgosInstance) && !this->isHeadless && ((this->timers[2] == 0) || (BREG(15) == 1)) &&
        (this->actor.xzDistToPlayer > KREG(71) + 100.0f)) {
        actionChanged = true;

        if (this->actor.xzDistToPlayer < KREG(72) + 300.0f) {
            if (Rand_ZeroOne() > 0.5f) {
                EnKnight_SetupBreathAttack(this, play);
            } else {
                EnKnight_SetupDetachHead(this, play);
            }
        } else {
            EnKnight_SetupDetachHead(this, play);
        }
    }
    return actionChanged;
}

s32 EnKnight_IsFacingPlayer(EnKnight* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawToPlayer = this->yawToPlayer;
    s16 yawDiff = yawToPlayer - this->actor.shape.rot.y;

    if (ABS_ALT(yawDiff) < 0x3000) {
        s16 invYawDiff = yawToPlayer - (s16)(player->actor.shape.rot.y + 0x8000);
        s32 absInvYawDiff = ABS_ALT(invYawDiff);

        if (absInvYawDiff < 0x3000) {
            return true;
        }
    }
    return false;
}

s32 EnKnight_IsFacingPlayerFocus(EnKnight* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff = this->yawToPlayer - (s16)(player->actor.focus.rot.y + 0x8000);

    if (ABS_ALT(yawDiff) < (s16)(KREG(91) * 0x100 + 0x400)) {
        s16 pitchDiff = (s16) - this->pitchToPlayer - player->actor.focus.rot.x;
        s32 absPitchDiff = ABS_ALT(pitchDiff);

        if (absPitchDiff < (s16)(KREG(92) * 0x100 + 0x300)) {
            return true;
        }
    }
    return false;
}

void EnKnight_SetupTelegraphHeavyAttack(EnKnight* this, PlayState* play, s32 noTelegraph) {
    Animation_MorphToLoop(&this->skelAnime, &gKnightTelegraphHeavyAttackAnim, -5.0f);
    this->actionFunc = EnKnight_TelegraphHeavyAttack;

    if (this == sIgosInstance) {
        if (noTelegraph) {
            this->timers[0] = 0;
        } else {
            this->timers[0] = 6;
        }
    } else {
        this->timers[0] = 15;
    }
}

void EnKnight_TelegraphHeavyAttack(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 2, 0x500);
    if (this->timers[0] == 0) {
        EnKnight_SetupHeavyAttack(this, play);
    }
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
}

void EnKnight_SetupHeavyAttack(EnKnight* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightHeavyAttackAnim, 0.0f);
    this->animEndFrame = Animation_GetLastFrame(&gKnightHeavyAttackAnim);
    this->actionFunc = EnKnight_HeavyAttack;
}

void EnKnight_HeavyAttack(EnKnight* this, PlayState* play) {
    Vec3f translation;

    this->neckYawTarget = 0;
    if (this->skelAnime.curFrame >= 3.0f && this->skelAnime.curFrame <= 6.0f) {
        this->blureAlpha = 255.0f;
        this->blureTranslation.x = -19.0f;
        this->blureTranslation.y = 46.0f;
        this->blureTranslation.z = 17.0f;
        this->blureRotation.x = -0.5497787f;
        this->blureRotation.y = 2.9059734f;
        this->blureRotation.z = -1.1780972f;
    }

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if ((this->skelAnime.curFrame >= 2.0f) && (this->skelAnime.curFrame <= 5.0f)) {
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);
        Matrix_MultVecZ(5.0f, &translation);
        this->animMovementX = translation.x;
        this->animMovementZ = translation.z;
    }

    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Actor_PlaySfx(&this->actor, this->attackSfx);
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        if ((Rand_ZeroOne() < 0.5f) && (this->actor.xzDistToPlayer <= 100.0f)) {
            EnKnight_SetupLowSwing(this, play);
        } else {
            EnKnight_SetupWait(this, play);
        }
    }

    this->swordColliderActive = true;
}

void EnKnight_SetupLowSwing(EnKnight* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightLowSwingAnim, -5.0f);
    this->animEndFrame = Animation_GetLastFrame(&gKnightLowSwingAnim);
    this->actionFunc = EnKnight_LowSwing;
}

void EnKnight_LowSwing(EnKnight* this, PlayState* play) {
    Vec3f translation;

    this->neckYawTarget = 0;

    if (this == sIgosInstance) {
        Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 2, 0x800);
    }

    if ((this->skelAnime.curFrame >= 12.0f) && (this->skelAnime.curFrame <= 15.0f)) {
        this->blureAlpha = 255.0f;
        this->blureTranslation.x = -20.0f;
        this->blureTranslation.y = 47.0f;
        this->blureTranslation.z = 0.0f;
        this->blureRotation.x = M_PIf;
        this->blureRotation.y = 0.f;
        this->blureRotation.z = 0.47123894f; // 3 * pi / 20
    }

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if ((this->skelAnime.curFrame >= 12.0f) && (this->skelAnime.curFrame <= 15.0f)) {
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);
        Matrix_MultVecZ(5.0f, &translation);
        this->animMovementX = translation.x;
        this->animMovementZ = translation.z;
    }

    if (Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Actor_PlaySfx(&this->actor, this->attackSfx);
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        EnKnight_SetupLowSwingEnd(this, play);
    }

    if (this->skelAnime.curFrame >= 12.0f) {
        this->swordColliderActive = true;
    }
}

void EnKnight_SetupLowSwingEnd(EnKnight* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gKnightLowSwingEndAnim, 0.0f);
    this->actionFunc = EnKnight_LowSwingEnd;

    if (this == sIgosInstance) {
        this->timers[0] = 6;
    } else {
        this->timers[0] = 15;
    }
}

void EnKnight_LowSwingEnd(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timers[0] == 0) {
        EnKnight_SetupWait(this, play);
    }

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
}

void EnKnight_SetupBasicSwing(EnKnight* this, PlayState* play) {
    if (Rand_ZeroOne() < 0.5f) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightRightSwingAnim, -2.0f);
        this->animEndFrame = Animation_GetLastFrame(&gKnightRightSwingAnim);
    } else {
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightLeftSwingAnim, -2.0f);
        this->animEndFrame = Animation_GetLastFrame(&gKnightLeftSwingAnim);
    }

    this->actionFunc = EnKnight_BasicSwing;
    this->timers[0] = 20;
}

void EnKnight_BasicSwing(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        if ((Rand_ZeroOne() < 0.6f) && (this->actor.xzDistToPlayer <= 100.0f)) {
            if (Rand_ZeroOne() < 0.5f) {
                EnKnight_SetupBasicSwing(this, play);
            } else {
                EnKnight_SetupTelegraphHeavyAttack(this, play, false);
            }
        } else {
            EnKnight_SetupWait(this, play);
        }
    }

    Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 5, 0x500);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if (this->timers[0] == 16) {
        this->swordColliderActive = true;
    }
}

void EnKnight_SetupJumpAttack(EnKnight* this, PlayState* play) {
    s32 i;
    Vec3f translation;

    if (this->actor.xzDistToPlayer <= 200.0f) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightJumpAttackBeginAnim, -3.0f);
        this->animEndFrame = Animation_GetLastFrame(&gKnightJumpAttackBeginAnim);
        this->actionFunc = EnKnight_JumpAttack;
        Matrix_RotateYS(this->yawToPlayer, MTXMODE_NEW);
        Matrix_MultVecZ(KREG(49) + 7.0f, &translation);
        this->animMovementX = translation.x;
        this->animMovementZ = translation.z;
        this->actor.velocity.y = 10.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_TEKU_JUMP);
        this->timers[0] = 15;

        for (i = 0; i < 5; i++) {
            EnKnight_SpawnDustAtFeet(this, play, 0);
        }
    }
}

void EnKnight_JumpAttack(EnKnight* this, PlayState* play) {
    if ((this->timers[0] > 1) && (this->timers[0] < 6)) {
        this->blureAlpha = 255.0f;
        this->blureTranslation.x = -19.0f;
        this->blureTranslation.y = 46.0f;
        this->blureTranslation.z = 17.0f;
        this->blureRotation.x = -0.5497787f;
        this->blureRotation.y = 2.9059734f;
        this->blureRotation.z = -1.1780972f;
    }

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightJumpAttackEndAnim, 0.0f);
        Actor_PlaySfx(&this->actor, this->attackSfx);
        this->animEndFrame = 1000.0f;
    }

    this->actor.speed = 0.0f;
    if ((this->actor.velocity.y <= 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->animMovementZ = 0.0f;
        this->animMovementX = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
    }

    if (this->timers[0] == 0) {
        EnKnight_SetupWait(this, play);
    }

    this->swordColliderActive = true;
}

void EnKnight_SetupBlocking(EnKnight* this, PlayState* play) {
    if ((this == sIgosInstance) && (this->actor.xzDistToPlayer <= 120.0f) && (Rand_ZeroOne() < 0.1f)) {
        EnKnight_SetupJumpBackwardsIgos(this, play);
        return;
    }

    if (this->actionFunc != EnKnight_Blocking) {
        this->prevActionFunc = this->actionFunc;
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightFastBlockStandingAnim, -2.0f);
        this->animEndFrame = Animation_GetLastFrame(&gKnightFastBlockStandingAnim);
        this->actionFunc = EnKnight_Blocking;
    }
    this->timers[0] = 5;
}

void EnKnight_Blocking(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 2, 0x800);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if (this->timers[0] != 0) {
        this->shieldColliderActive = true;
    } else if (this->prevActionFunc == EnKnight_Retreat) {
        EnKnight_SetupRetreat(this, play, true);
    } else {
        EnKnight_SetupWait(this, play);
        if ((this == sIgosInstance) && (Rand_ZeroOne() < 0.4f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_CYNICAL);
        }
    }

    this->canRetreat = true;
}

void EnKnight_SetupWait(EnKnight* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -5.0f);
    this->actionFunc = EnKnight_Wait;
    this->timers[0] = Rand_ZeroFloat(50.0f) + 20.0f;
}

void EnKnight_Wait(EnKnight* this, PlayState* play) {
    s16 yawDiff;
    s32 yawCap;

    SkelAnime_Update(&this->skelAnime);

    yawDiff = this->yawToPlayer - this->actor.shape.rot.y;
    yawCap = (this == sIgosInstance) ? 0x7FFF : 0x6000;
    if (ABS_ALT(yawDiff) < yawCap) {
        EnKnight_SetupTurnToPlayer(this, play);
    } else {
        if ((this->randTimer % 8 == 0) && (Rand_ZeroOne() < 0.75f)) {
            this->neckYawTarget = Rand_CenteredFloat(28672.0f);
        }
        if (this->timers[0] == 0) {
            EnKnight_SetupStrafe(this, play, 0x4000);
        }
    }

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    this->canRetreat = true;
}

void EnKnight_SetupStrafe(EnKnight* this, PlayState* play, s16 strafeAngle) {
    this->actionFunc = EnKnight_Strafe;
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightStrafeAnim, -2.0f);

    if (Rand_ZeroOne() < 0.5f) {
        this->strafeTarget = this->actor.shape.rot.y + strafeAngle;
        this->neckYawTarget = 0x3800;
    } else {
        this->strafeTarget = this->actor.shape.rot.y - strafeAngle;
        this->neckYawTarget = -0x3800;
    }

    this->strafeMaxStep = (strafeAngle == 0x4000) ? 0 : 0x800;
    this->timers[0] = 20;
}

void EnKnight_Strafe(EnKnight* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (this->timers[0] < 15) {
        Math_ApproachS(&this->actor.world.rot.y, this->strafeTarget, 2, this->strafeMaxStep);
        Math_ApproachS(&this->strafeMaxStep, 0x800, 1, 0x200);
    }

    yawDiff = this->yawToPlayer - this->actor.shape.rot.y;
    if (ABS_ALT(yawDiff) < 0x6000) {
        EnKnight_SetupTurnToPlayer(this, play);
    } else if (this->timers[0] == 0) {
        EnKnight_SetupWait(this, play);
    }

    this->neckRotationMaxStep = 0x3000;
    this->neckRotationStepScale = 2;
}

void EnKnight_SetupTurnToPlayer(EnKnight* this, PlayState* play) {
    this->actionFunc = EnKnight_TurnToPlayer;
    this->timers[0] = 10;
}

void EnKnight_TurnToPlayer(EnKnight* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);

    yawDiff = this->yawToPlayer - this->actor.shape.rot.y;
    if (yawDiff > 0x3800) {
        this->neckYawTarget = 0x3800;
    } else if (yawDiff < -0x3800) {
        this->neckYawTarget = -0x3800;
    } else {
        this->neckYawTarget = yawDiff;
    }

    if (this->timers[0] == 0) {
        EnKnight_SetupApproachPlayer(this, play);
    }

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    this->neckRotationMaxStep = 0x3000;
    this->neckRotationStepScale = 2;
    this->canRetreat = true;
}

void EnKnight_SetupApproachPlayer(EnKnight* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gKnightSlowWalkShieldingAnim, -5.0f);
    this->actionFunc = EnKnight_ApproachPlayer;
    this->actor.speed = 0.0f;
}

void EnKnight_ApproachPlayer(EnKnight* this, PlayState* play) {
    f32 speed;
    f32 dx;
    f32 dz;
    f32 scale;
    Vec3f moveOffsetLocal;
    Vec3f moveOffset;
    s16 yawDiff;
    s16 yaw;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    if ((this->randTimer % 64 == 0) && (Rand_ZeroOne() < 0.5f)) {
        if (Rand_ZeroOne() < 0.6666f) {
            Actor_PlaySfx(&this->actor, this->pauseSfx);
        } else {
            Actor_PlaySfx(&this->actor, this->laughSfx);
        }
    }

    if ((this != sIgosInstance) && (this->actor.xzDistToPlayer > 150.0f)) {
        Matrix_RotateYS(player->actor.world.rot.y, MTXMODE_NEW);

        if (this == sThinKnightInstance) {
            moveOffsetLocal.x = 100.0f;
        } else {
            moveOffsetLocal.x = -100.0f;
        }
        moveOffsetLocal.y = 0.0f;
        moveOffsetLocal.z = 100.0f;
        Matrix_MultVec3f(&moveOffsetLocal, &moveOffset);
        this->retreatTowards.x = player->actor.world.pos.x + moveOffset.x;
        this->retreatTowards.z = player->actor.world.pos.z + moveOffset.z;
        dx = this->retreatTowards.x - this->actor.world.pos.x;
        dz = this->retreatTowards.z - this->actor.world.pos.z;
        yaw = Math_Atan2S_XY(dz, dx);
    } else {
        yaw = this->yawToPlayer;
    }

    Math_ApproachS(&this->actor.world.rot.y, yaw, 5, 0xA00);
    Math_ApproachS(&this->actor.shape.rot.y, this->yawToPlayer, 5, 0x500);

    speed = this->actor.speed;
    scale = (this == sIgosInstance) ? 1.3076924f : 1.0f;

    if (this->actor.xzDistToPlayer <= (150.0f * scale)) {
        if (this->actor.xzDistToPlayer <= (80.0f * scale)) {
            if (this->actor.xzDistToPlayer <= (40.0f * scale)) {
                Math_ApproachF(&this->actor.speed, -2.5f, 1.0f, 1.0f);
            } else {
                Math_ApproachF(&this->actor.speed, 0.0f, 1.0f, 1.0f);
            }
        } else {
            Math_ApproachF(&this->actor.speed, 2.5f, 1.0f, 1.0f);
        }
    } else {
        Math_ApproachF(&this->actor.speed, 5.0f, 1.0f, 1.0f);
        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
            Actor_PlaySfx(&this->actor, this->walkSfx);
        }
        EnKnight_SpawnDustAtFeet(this, play, 3);
    }

    if ((this->actor.speed > 3.0f) && (speed <= 3.0f)) {
        Animation_MorphToLoop(&this->skelAnime, &gKnightFastWalkShieldingAnim, -3.0f);
        if (Rand_ZeroOne() < 0.25f) {
            EnKnight_SetupJumpAttack(this, play);
        }
    }

    if (this->actionFunc == EnKnight_ApproachPlayer) {
        if ((this->actor.speed < 4.0f) && (speed >= 4.0f)) {
            Animation_MorphToLoop(&this->skelAnime, &gKnightSlowWalkShieldingAnim, -5.0f);
            if (Rand_ZeroOne() < 0.25f) {
                EnKnight_SetupJumpAttack(this, play);
            }
        }
        if ((this->actor.speed < 1.0f) && (speed >= 1.0f)) {
            Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -10.0f);
        }
        if ((this->actor.speed >= 1.0f) && (speed < 1.0f)) {
            Animation_MorphToLoop(&this->skelAnime, &gKnightSlowWalkShieldingAnim, -5.0f);
        }
    }

    yawDiff = yaw - this->actor.shape.rot.y;
    if (ABS_ALT(yawDiff) < 0x6000) {
        if (yawDiff > 0x3800) {
            this->neckYawTarget = 0x3800;
        } else if (yawDiff < -0x3800) {
            this->neckYawTarget = -0x3800;
        } else {
            this->neckYawTarget = yawDiff;
        }
        if (EnKnight_SelectAttack(this, play)) {
            return;
        }
    } else {
        EnKnight_SetupWait(this, play);
    }

    this->canRetreat = true;
}

void EnKnight_SetupRetreat(EnKnight* this, PlayState* play, u8 shielding) {
    if (this->actionFunc != EnKnight_Retreat) {
        Animation_MorphToLoop(&this->skelAnime, &gKnightFastWalkShieldingAnim, -5.0f);
        this->subAction = KNIGHT_SUB_ACTION_RETREAT_0;
        this->actionFunc = EnKnight_Retreat;
    }

    this->timers[0] = Rand_ZeroFloat(15.0f) + 10.0f;
    this->retreatWhileShielding = shielding;
}

void EnKnight_Retreat(EnKnight* this, PlayState* play) {
    f32 dx;
    f32 dz;

    SkelAnime_Update(&this->skelAnime);
    this->neckYawTarget = 0;

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_RETREAT_0:
            Math_ApproachF(&this->actor.speed, 5.0f, 1.0f, 1.0f);
            dx = this->retreatTowards.x - this->actor.world.pos.x;
            dz = this->retreatTowards.z - this->actor.world.pos.z;
            Math_ApproachS(&this->actor.world.rot.y, Math_Atan2S_XY(dz, dx), 5, 0xA00);

            if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
                Actor_PlaySfx(&this->actor, this->walkSfx);
            }

            EnKnight_SpawnDustAtFeet(this, play, 3);

            if (this->retreatWhileShielding &&
                (((sMirRayInstance != NULL) && (sMirRayInstance->unk_214 < 0.1f)) ||
                 (BREG(70) + 300.0f < this->actor.xzDistToPlayer) || (this->lightRayDamageTimer != 0))) {
                if (this->lightRayDamageTimer != 0) {
                    this->subAction = KNIGHT_SUB_ACTION_RETREAT_3;
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_30;
                    Animation_MorphToLoop(&this->skelAnime, &gKnightStruckByLightRayAnim, -2.0f);
                    this->animEndFrame = Animation_GetLastFrame(&gKnightStruckByLightRayAnim);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STAL_FREEZE_LIGHTS);
                } else {
                    this->subAction = KNIGHT_SUB_ACTION_RETREAT_1;
                    Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -5.0f);
                    this->animEndFrame = 1000.0f;
                }
            }
            break;

        case KNIGHT_SUB_ACTION_RETREAT_1:
            if (this->lightRayDamageTimer != 0) {
                this->subAction = KNIGHT_SUB_ACTION_RETREAT_2;
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightFastBlockStandingAnim, -2.0f);
                Actor_PlaySfx(&this->actor, this->pauseSfx);
            }
            goto common_case;

        case KNIGHT_SUB_ACTION_RETREAT_2:
            if (this->lightRayDamageTimer == 0) {
                this->subAction = KNIGHT_SUB_ACTION_RETREAT_1;
                Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -5.0f);
            }

            if (this->lightRayDamageTimer > 7) {
                Math_ApproachF(&this->shieldLightReflectionScale, 1.0f, 1.0f, 0.5f);
            }
            goto common_case;

        case KNIGHT_SUB_ACTION_RETREAT_3:
            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                this->subAction = KNIGHT_SUB_ACTION_RETREAT_2;
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightFastBlockStandingAnim, -2.0f);
                this->animEndFrame = 1000.0f;
            }
            goto common_case;

        common_case:
            this->bodyCollider.base.colMaterial = COL_MATERIAL_NONE;
            Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

            if (this->animEndFrame > 10.0f) {
                Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 2, 0xE00);
            }

            if ((sMirRayInstance != NULL) && (sMirRayInstance->unk_214 > 0.1f)) {
                this->timers[0] = Rand_ZeroFloat(15.0f) + 10.0f;
            }

            this->canRetreat = true;
            break;
    }

    if (this->timers[0] == 0) {
        EnKnight_SetupStrafe(this, play, 0x7000);
    }
}

void EnKnight_SetupGetBackUp(EnKnight* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gKnightLaugh1Anim, -25.0f);
    Actor_PlaySfx(&this->actor, this->pauseSfx);
    this->actionFunc = EnKnight_GetBackUp;
    this->timers[0] = 35;
}

void EnKnight_GetBackUp(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->neckYawTarget = 0;
    Math_ApproachF(&this->shadowAlphaFactor, 180.0f, 1.0f, 10.0f);

    if (this->timers[0] == 0) {
        EnKnight_SetupWait(this, play);
    }

    if ((this->timers[0] > (s16)(BREG(74) + 10)) && !(this->randTimer & (BREG(76) + 3))) {
        EnKnight* afterImage = (EnKnight*)Actor_SpawnAsChild(
            &play->actorCtx, &this->actor, play, ACTOR_EN_KNIGHT, this->actor.world.pos.x, this->actor.world.pos.y,
            this->actor.world.pos.z, this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z,
            (this == sIgosInstance) ? EN_KNIGHT_PARAM_IGOS_BODY_AFTERIMAGE : EN_KNIGHT_PARAM_KNIGHT_BODY_AFTERIMAGE);

        if (afterImage != NULL) {
            s32 i;

            for (i = 0; i < KNIGHT_LIMB_MAX; i++) {
                afterImage->skelAnime.jointTable[i] = this->skelAnime.jointTable[i];
                afterImage->skelAnime.skeleton = this->skelAnime.skeleton;
                afterImage->skelAnime.dListCount = this->skelAnime.dListCount;
            }
        }
    }
}

void EnKnight_SetupStunned(EnKnight* this, PlayState* play, u16 duration) {
    this->actionFunc = EnKnight_Stunned;
    this->timers[0] = duration;
    this->timers[1] = 30;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, 0, duration);
}

void EnKnight_Stunned(EnKnight* this, PlayState* play) {
    if ((this->timers[1] > 0) && (this->timers[1] < 23)) {
        if (this->timers[1] == 1) {
            sTargetKnight = NULL;
        } else {
            sTargetKnight = this;
        }
    }

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if (this->timers[0] < 20) {
        if (this->timers[0] % 2 != 0) {
            this->neckYaw = 0x708;
        } else {
            this->neckYaw = -0x708;
        }
    } else {
        this->neckYawTarget = 0;
    }

    if (this->timers[0] == 0) {
        EnKnight_SetupWait(this, play);
    }
}

void EnKnight_SetupRecoilFromDamage(EnKnight* this, PlayState* play) {
    Vec3f translation;
    s16 yawDiff;

    this->actionFunc = EnKnight_RecoilFromDamage;

    yawDiff = this->yawToPlayer - this->actor.shape.rot.y;
    if (ABS_ALT(yawDiff) < 0x4000) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightDamagedFrontAnim, 0.0f);
        this->subAction = KNIGHT_SUB_ACTION_RECOIL_0;
    } else {
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightDamagedBackAnim, 0.0f);
        this->subAction = KNIGHT_SUB_ACTION_RECOIL_1;
    }

    Matrix_RotateYS(this->yawToPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-15.0f, &translation);
    this->animMovementX = translation.x;
    this->animMovementZ = translation.z;
    this->timers[0] = 10;
}

void EnKnight_RecoilFromDamage(EnKnight* this, PlayState* play) {
    f32 rand = Rand_ZeroOne();

    SkelAnime_Update(&this->skelAnime);

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if (this->timers[0] == 0) {
        if ((this == sIgosInstance) && (this->actor.xzDistToPlayer < 150.0f)) {
            if (rand < 0.25f) {
                EnKnight_SetupLowSwing(this, play);
            } else if (rand < 0.5f) {
                EnKnight_SetupTelegraphHeavyAttack(this, play, false);
            } else if (rand < 0.75f) {
                EnKnight_SetupJumpBackwardsIgos(this, play);
            } else {
                EnKnight_SetupBlocking(this, play);
            }
        } else if (this->subAction == KNIGHT_SUB_ACTION_RECOIL_0) {
            EnKnight_SetupWait(this, play);
        } else {
            EnKnight_SetupStrafe(this, play, 0x6000);
        }
    }

    if ((this == sIgosInstance) && (this->timers[0] < 4)) {
        this->canRetreat = true;
    }
}

void EnKnight_SetupFallOver(EnKnight* this, PlayState* play) {
    Vec3f translation;
    s16 yawDiff;

    this->actionFunc = EnKnight_FallOver;

    yawDiff = this->yawToPlayer - this->actor.shape.rot.y;
    if (ABS_ALT(yawDiff) < 0x4000) {
        // Fall over backwards
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightFallBackwardsAnim, 0.0f);
        this->animEndFrame = 1.0f;
    } else {
        // Fall over forwards
        Animation_MorphToPlayOnce(&this->skelAnime, &gKnightFallForwardsAnim, 0.0f);
        this->animEndFrame = -1.0f;
    }

    Matrix_RotateYS(this->yawToPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(KREG(29) + -15.0f, &translation);
    this->animMovementX = translation.x;
    this->animMovementZ = translation.z;
    this->timers[0] = 35;
    this->timers[1] = 150;
    this->actor.colChkInfo.damageTable = &sDamageTableFallenOver;
}

void EnKnight_FallOver(EnKnight* this, PlayState* play) {
    s32 pad1;
    f32 floorHeight;
    f32 scale;
    Vec3f raycastPos;
    CollisionPoly* collisionPoly;
    f32 dx;
    f32 dy;
    f32 dz;
    s16 timerTarget;

    if (this->timers[0] < (s16)(KREG(6) + 25)) {
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

        if (this == sIgosInstance) {
            scale = -75.0f;
        } else {
            scale = -65.0f;
        }

        Matrix_MultVecZ((KREG(9) + 1.0f) * (scale * this->animEndFrame), &raycastPos);
        raycastPos.x += this->actor.world.pos.x;
        raycastPos.y += this->actor.world.pos.y + 300.0f;
        raycastPos.z += this->actor.world.pos.z;
        floorHeight = BgCheck_EntityRaycastFloor1(&play->colCtx, &collisionPoly, &raycastPos);

        if ((floorHeight > -10.0f) && (floorHeight != this->actor.floorHeight)) {
            dx = raycastPos.x - this->actor.world.pos.x;
            dy = floorHeight - this->actor.world.pos.y;
            dz = raycastPos.z - this->actor.world.pos.z;
            Math_ApproachS(&this->actor.shape.rot.x, Math_Atan2S(dy, sqrtf(SQ(dx) + SQ(dz))) * (s32)this->animEndFrame,
                           1, 0x800);
        } else {
            Math_ApproachS(&this->actor.shape.rot.x, 0, 1, 0x800);
        }
    }

    if ((this->timers[1] >= 120) && (this->timers[1] < 140)) {
        if (this->timers[1] == 120) {
            sTargetKnight = NULL;
        } else {
            sTargetKnight = this;
        }
    }

    SkelAnime_Update(&this->skelAnime);

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    Math_ApproachZeroF(&this->shadowAlphaFactor, 1.0f, 10.0f);
    if (this->animEndFrame > 0.0f) {
        timerTarget = 20;
    } else {
        timerTarget = KREG(8) + 8;
    }

    if (timerTarget == this->timers[0]) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GERUDOFT_DOWN);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            this->drawDmgEffDuration = 0;
        }
    }

    if ((this->timers[1] == 0) && (sIgosInstance->actionFunc != EnKnight_CaptainsHatCS)) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        EnKnight_SetupGetBackUp(this, play);

        if (this == sIgosInstance) {
            this->actor.colChkInfo.health = 14 - BREG(41);
        } else {
            this->actor.colChkInfo.health = 6 - BREG(40);
        }
        this->bodyCollider.elements[0].base.acElemFlags &= ~ACELEM_HIT;
        this->bodyCollider.elements[1].base.acElemFlags &= ~ACELEM_HIT;
        this->actor.colChkInfo.damageTable = &sDamageTableStanding;
        this->invincibilityTimer = 25;
    }
}

void EnKnight_SetupDie(EnKnight* this, PlayState* play) {
    this->actionFunc = EnKnight_Die;
    this->timers[2] = 50;
    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_20;

    if (this == sIgosInstance) {
        Audio_RestorePrevBgm();
    }

    Audio_PlaySfx_AtPos(&this->projectedPos, this->voiceSfx);
}

void EnKnight_Die(EnKnight* this, PlayState* play) {
    Vec3f accel;
    Vec3f pos;
    s16 bodyPartIndex;

    if ((this->timers[2] >= 10) && (this->timers[2] < 40)) {
        if (this->timers[2] == 10) {
            sTargetKnight = NULL;
        } else {
            sTargetKnight = this;
        }
    }

    if (this->timers[2] == 0) {
        this->doBgChecks = false;
        this->actor.draw = NULL;
        this->actor.world.pos.y = 1000.0f;
        this->actor.world.pos.x = 1300.0f;
        this->actor.world.pos.z = 2864.0f;
        this->actor.gravity = 0.0f;
    }

    Math_ApproachZeroF(&this->actor.scale.y, 0.1f, 0.0015f);

    if (this->timers[2] != 0) {
        bodyPartIndex = (s32)Rand_ZeroFloat(ARRAY_COUNT(this->bodyPartsPos) - 0.1f);
        accel = sZeroVec;
        accel.y = 0.03f;
        pos.x = this->bodyPartsPos[bodyPartIndex].x + Rand_CenteredFloat(20.0f);
        pos.y = this->bodyPartsPos[bodyPartIndex].y - 5.0f;
        pos.z = this->bodyPartsPos[bodyPartIndex].z + Rand_CenteredFloat(20.0f);
        EffectSsKFire_Spawn(play, &pos, &sZeroVec, &accel, Rand_ZeroFloat(20.0f) + 20.0f, 0);
        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
    }

    if (this->timers[2] == 0) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

        if (this == sIgosInstance) {
            if (CutsceneManager_IsNext(this->actor.csId)) {
                CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
                this->actionFunc = EnKnight_Dead;
            } else {
                CutsceneManager_Queue(this->actor.csId);
            }
        } else {
            this->actionFunc = EnKnight_Dead;
        }

        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        this->timers[0] = 30;
    }
}

void EnKnight_Dead(EnKnight* this, PlayState* play) {
    if ((this == sIgosInstance) && (this->timers[0] == 0)) {
        Actor_Kill(&this->actor);
        Actor_Kill(&sThinKnightInstance->actor);
        Actor_Kill(&sWideKnightInstance->actor);
    }
}

void EnKnight_SetupJumpBackwardsKnight(EnKnight* this, PlayState* play) {
    s32 i;
    Vec3f translation;

    this->actionFunc = EnKnight_JumpBackwards;
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightJumpBackAnim, 0.0f);
    Matrix_RotateYS(this->yawToPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-15.0f, &translation);
    this->animMovementX = translation.x;
    this->animMovementZ = translation.z;
    this->actor.velocity.y = 7.5f;

    for (i = 0; i < 5; i++) {
        EnKnight_SpawnDustAtFeet(this, play, 0);
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_TEKU_JUMP);
    this->subAction = KNIGHT_SUB_ACTION_JUMP_BACKWARDS_0;
    this->invincibilityTimer = 10;
}

void EnKnight_SetupJumpBackwardsIgos(EnKnight* this, PlayState* play) {
    s32 i;
    Vec3f translation;

    this->actionFunc = EnKnight_JumpBackwards;
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightJumpBackAnim, 0.0f);
    Matrix_RotateYS(this->yawToPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(KREG(90) + 14.0f, &translation);
    this->animMovementX = translation.x;
    this->animMovementZ = translation.z;
    this->actor.velocity.y = KREG(91) + 13.0f;

    for (i = 0; i < 5; i++) {
        EnKnight_SpawnDustAtFeet(this, play, 0);
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_JUMP1);
    this->subAction = KNIGHT_SUB_ACTION_JUMP_BACKWARDS_1;
    this->invincibilityTimer = 20;
}

void EnKnight_JumpBackwards(EnKnight* this, PlayState* play) {
    if (this->subAction == KNIGHT_SUB_ACTION_JUMP_BACKWARDS_1) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_JUMP2 - SFX_FLAG);
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.speed = 0.0f;

    if (this == sIgosInstance) {
        Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 2, 0x1000);

        if (this->invincibilityTimer == 13) {
            if (this->subAction == KNIGHT_SUB_ACTION_JUMP_BACKWARDS_1) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_LAUGH_DEMO);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_CYNICAL);
            }
        }
    }

    if ((this->actor.velocity.y <= 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->animMovementX = this->animMovementZ = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);

        if (this->subAction == KNIGHT_SUB_ACTION_JUMP_BACKWARDS_1) {
            if (this->isHeadless) {
                if (Rand_ZeroOne() < 0.5f) {
                    EnKnight_SetupJumpAttack(this, play);
                } else {
                    EnKnight_SetupTelegraphHeavyAttack(this, play, false);
                }
            } else if (Rand_ZeroOne() < 0.5f) {
                EnKnight_SetupJumpAttack(this, play);
            } else {
                EnKnight_SetupBreathAttack(this, play);
            }
        } else if ((this == sIgosInstance) && (Rand_ZeroOne() < 0.3f)) {
            if ((Rand_ZeroOne() < 0.5f) || this->isHeadless) {
                EnKnight_SetupJumpAttack(this, play);
            } else {
                EnKnight_SetupBreathAttack(this, play);
            }
        } else {
            EnKnight_SetupWait(this, play);
        }
    }
}

void EnKnight_SetupIgosReactToCurtains(EnKnight* this, PlayState* play) {
    this->actionFunc = EnKnight_IgosReactToCurtains;
    this->subAction = KNIGHT_SUB_ACTION_CURTAIN_REACT_WAIT;
}

void EnKnight_IgosReactToCurtains(EnKnight* this, PlayState* play) {
    this->inCurtainReaction = true;

    if (this->subAction != KNIGHT_SUB_ACTION_CURTAIN_REACT_WAIT) {
        SkelAnime_Update(&this->skelAnime);
    }

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_CURTAIN_REACT_WAIT:
            // Wait for Boss06 (the curtains) to advance the subAction.
            break;

        case KNIGHT_SUB_ACTION_CURTAIN_REACT_BEGIN:
            // Setting up standing and performing one of two reactions. The reaction to the first curtain is random
            // while the reaction to the second curtain is always the other one.

            if (this->igosCurtainReaction == 0) {
                // Mad
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosCurtainReactionAngryAnim, 0.0f);
            } else {
                // Shocked
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosCurtainReactionShockedAnim, 0.0f);
            }

            Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_STAND);

            this->igosCurtainReaction = 1 - this->igosCurtainReaction;
            this->subAction = KNIGHT_SUB_ACTION_CURTAIN_REACT_PERFORM;
            this->timers[0] = 2;
            FALLTHROUGH;
        case KNIGHT_SUB_ACTION_CURTAIN_REACT_PERFORM:
            // Playing the reaction

            if (this->timers[0] == 1) {
                if (this->igosCurtainReaction == 0) {
                    // shocked
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_SHOCK);
                } else {
                    // mad
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_SHIT);
                }
            }

            if (!this->inCurtainCutscene) {
                // Curtain cutscene is over, return to sitting
                EnKnight_SetupIgosSitting(this, play);
                this->skelAnime.curFrame = 25.0f;
            }
            break;
    }

    this->neckYaw = this->neckYawTarget = this->neckRoll = this->neckRollTarget = 0;
}

void EnKnight_SetupLookAtOther(EnKnight* this, PlayState* play) {
    this->actionFunc = EnKnight_LookAtOther;
    Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -5.0f);
}

void EnKnight_LookAtOther(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if (sTargetKnight == NULL) {
        EnKnight_SetupWait(this, play);
    } else {
        f32 dx = sTargetKnight->actor.focus.pos.x - this->actor.world.pos.x;
        f32 dz = sTargetKnight->actor.focus.pos.z - this->actor.world.pos.z;
        s16 yawDiff = Math_Atan2S_XY(dz, dx) - this->actor.shape.rot.y;

        if (yawDiff > 0x3800) {
            this->neckYawTarget = 0x3800;
        } else if (yawDiff < -0x3800) {
            this->neckYawTarget = -0x3800;
        } else {
            this->neckYawTarget = yawDiff;
        }

        this->neckRotationMaxStep = 0x3000;
        this->neckRotationStepScale = 2;
    }
}

void EnKnight_SetupMarch(EnKnight* this, PlayState* play) {
    this->actionFunc = EnKnight_March;
    Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -5.0f);
    this->subAction = KNIGHT_SUB_ACTION_MARCH_0;
    this->timers[0] = Rand_ZeroFloat(10.0f) + 65.0f;
}

void EnKnight_March(EnKnight* this, PlayState* play) {
    f32 dx;
    f32 dz;
    s16 yawDiff;
    EnKnight* otherKnight;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    otherKnight = (this == sThinKnightInstance) ? sWideKnightInstance : sThinKnightInstance;

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_MARCH_0:
            Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 2, 0x500);
            Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

            if ((this->timers[0] < 40) && (this->timers[0] >= 16)) {
                dx = otherKnight->actor.focus.pos.x - this->actor.world.pos.x;
                dz = otherKnight->actor.focus.pos.z - this->actor.world.pos.z;
                yawDiff = Math_Atan2S_XY(dz, dx) - this->actor.shape.rot.y;

                if (yawDiff > 0x3800) {
                    this->neckYawTarget = 0x3800;
                } else if (yawDiff < -0x3800) {
                    this->neckYawTarget = -0x3800;
                } else {
                    this->neckYawTarget = yawDiff;
                }

                this->neckRotationMaxStep = 0x3000;
                this->neckRotationStepScale = 2;
            } else {
                this->neckYawTarget = 0;
            }

            if ((this->timers[0] == 0) && (func_801A46F8() == true)) {
                Animation_MorphToLoop(&this->skelAnime, &gKnightMarchAnim, -3.0f);
                this->subAction = KNIGHT_SUB_ACTION_MARCH_1;
                this->timers[0] = 200;
            }

            if (!(player->stateFlags3 & PLAYER_STATE3_20000000)) {
                this->subAction = KNIGHT_SUB_ACTION_MARCH_2;
                this->timers[0] = 10;
            }
            break;

        case KNIGHT_SUB_ACTION_MARCH_1:
            this->actor.speed = KREG(38) + 1.0f;

            if (this == sThinKnightInstance) {
                this->actor.world.rot.y = (this->actor.world.rot.y + KREG(39) * 0x100) + 0x100;
            } else {
                this->actor.world.rot.y = (this->actor.world.rot.y - KREG(39) * 0x100) - 0x100;
            }

            if ((this->timers[0] == 0) || !(player->stateFlags3 & PLAYER_STATE3_20000000)) {
                this->subAction = KNIGHT_SUB_ACTION_MARCH_2;
                this->timers[0] = 10;
            }
            goto handle_walk_sfx;

        case KNIGHT_SUB_ACTION_MARCH_2:
            if (this->timers[0] == 0) {
                EnKnight_SetupWait(this, play);
            }
            goto handle_walk_sfx;

        handle_walk_sfx:
            if (Animation_OnFrame(&this->skelAnime, KREG(53) + 1.0f) ||
                Animation_OnFrame(&this->skelAnime, KREG(54) + 11.0f)) {
                Actor_PlaySfx(&this->actor, this->walkSfx);
            }
            break;
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 5, 0x500);
}

void EnKnight_SetupIntroCutscene(EnKnight* this, PlayState* play) {
    if (this == sIgosInstance) {
        this->actionFunc = EnKnight_IntroCutscene;
        Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -5.0f);
    } else {
        this->actionFunc = EnKnight_WaitIntroCutscene;
    }
    this->actor.world.pos.y = 1000.0f;
}

void EnKnight_SpawnBreathEffects(EnKnight* this, PlayState* play, s16 pitchOffset) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    s16 breathAlphaTarget;

    Math_Vec3f_Copy(&pos, &this->breathBasePos);
    Matrix_RotateYS(this->neckYaw + this->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->actor.shape.rot.x + pitchOffset, MTXMODE_APPLY);
    Matrix_MultVecZ(BREG(48) + 20.0f, &velocity);

    breathAlphaTarget = (this->timers[0] < 40) ? 0 : 200;
    Math_ApproachS(&this->breathAlpha, breathAlphaTarget, 1, 5);

    if (this->breathAlpha > 60) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);
    }

    accel.x = accel.y = accel.z = 0.0f;
    EnKnight_EffectAdd(play, &pos, &velocity, &accel, 2.0f, 25.0f, this->breathAlpha);

    pos.x -= velocity.x * 0.5f;
    pos.y -= velocity.y * 0.5f;
    pos.z -= velocity.z * 0.5f;
    EnKnight_EffectAdd(play, &pos, &velocity, &accel, 2.0f, 25.0f, this->breathAlpha);
}

void EnKnight_SetupIgosSitting(EnKnight* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosSitDownAnim, -5.0f);
    this->actionFunc = EnKnight_IgosSitting;
    this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_0;
}

void EnKnight_SpawnDust(PlayState* play, u8 numEffects) {
    static Color_RGBA8 sPrimColor = { 60, 50, 20, 255 };
    static Color_RGBA8 sEnvColor = { 40, 30, 30, 255 };
    Player* player = GET_PLAYER(play);
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    u8 i;

    for (i = 0; i < numEffects; i++) {
        velocity.x = Rand_CenteredFloat(3.0f);
        velocity.y = Rand_ZeroFloat(2.0f) + 1.0f;
        velocity.z = Rand_CenteredFloat(3.0f);
        accel.x = accel.z = 0.0f;
        accel.y = -0.1f;
        pos.x = player->actor.world.pos.x + Rand_CenteredFloat(30.0f);
        pos.y = player->actor.world.pos.y + 20.0f + Rand_CenteredFloat(30.0f);
        pos.z = player->actor.world.pos.z + Rand_CenteredFloat(30.0f);
        func_800B0EB0(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, Rand_ZeroFloat(50.0f) + 100.0f, 10,
                      Rand_ZeroFloat(5.0f) + 14.0f);
    }
}

void EnKnight_IgosSitting(EnKnight* this, PlayState* play) {
    s16 yawDiff;
    s16 yaw = 0;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    yawDiff = this->yawToPlayer - this->actor.shape.rot.y;
    if (yawDiff > 0x3800) {
        this->neckYawTarget = 0x3800;
    } else if (yawDiff < -0x3800) {
        this->neckYawTarget = -0x3800;
    } else {
        this->neckYawTarget = yawDiff;
    }

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_IGOS_SITTING_0:
            if (this->actor.xzDistToPlayer < 35.0f) {
                if (player->actor.world.pos.z < this->actor.world.pos.z) {
                    this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_1;
                } else {
                    this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_5;
                }
                this->timers[0] = 8;
                Actor_PlaySfx(&this->actor, this->attackSfx);
            }

            if ((player->unk_D57 == 4) && EnKnight_IsFacingPlayerFocus(this, play)) {
                // fast block
                this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_10;
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightFastBlockSittingAnim, 0.0f);
                this->animEndFrame = Animation_GetLastFrame(&gKnightFastBlockSittingAnim);
            }
            break;

        case KNIGHT_SUB_ACTION_IGOS_SITTING_1:
            if (this->timers[0] == 0) {
                this->subAction++; // = KNIGHT_SUB_ACTION_IGOS_SITTING_2
                this->timers[0] = 20;
            } else {
                Math_ApproachS(&this->rightLegLowerRotation, 0x3C00, 1, 0x1770);
                Math_ApproachS(&this->rightLegUpperRotation, 0x1400, 1, 0x1770);

                if (this->timers[0] == 7) {
                    yaw = -0x2000;
                }
                break;
            }
            FALLTHROUGH;
        case KNIGHT_SUB_ACTION_IGOS_SITTING_2:
            Math_ApproachS(&this->rightLegLowerRotation, 0, 1, 0xBB8);
            Math_ApproachS(&this->rightLegUpperRotation, 0, 1, 0xBB8);

            if (this->timers[0] == 0) {
                this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_0;
            }
            break;

        case KNIGHT_SUB_ACTION_IGOS_SITTING_5:
            if (this->timers[0] == 0) {
                this->subAction++; // = KNIGHT_SUB_ACTION_IGOS_SITTING_5
                this->timers[0] = 20;
            } else {
                Math_ApproachS(&this->leftLegLowerRotation, 0x3C00, 1, 0x1770);
                Math_ApproachS(&this->leftLegUpperRotation, 0x1400, 1, 0x1770);

                if (this->timers[0] == 7) {
                    yaw = 0x2000;
                }
                break;
            }
            FALLTHROUGH;
        case KNIGHT_SUB_ACTION_IGOS_SITTING_6:
            Math_ApproachS(&this->leftLegLowerRotation, 0, 1, 0xBB8);
            Math_ApproachS(&this->leftLegUpperRotation, 0, 1, 0xBB8);

            if (this->timers[0] == 0) {
                this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_0;
            }
            break;

        case KNIGHT_SUB_ACTION_IGOS_SITTING_10:
            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                // return to idle
                this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_11;
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightEndBlockAndSitAnim, 0.0f);
                this->animEndFrame = Animation_GetLastFrame(&gKnightEndBlockAndSitAnim);
            }
            break;

        case KNIGHT_SUB_ACTION_IGOS_SITTING_11:
            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosSitDownAnim, -15.0f);
                this->subAction = KNIGHT_SUB_ACTION_IGOS_SITTING_0;
            }
            break;
    }

    if (this->subAction >= KNIGHT_SUB_ACTION_IGOS_SITTING_10) {
        Vec3f colliderPos;

        colliderPos.x = this->actor.world.pos.x + 40.0f;
        colliderPos.y = this->actor.world.pos.y + 70.0f;
        colliderPos.z = this->actor.world.pos.z;
        EnKnight_SetColliderSphere(this, 0, &this->shieldCollider, &colliderPos);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->shieldCollider.base);
    }

    if ((sThinKnightInstance->actor.draw == NULL) && (sWideKnightInstance->actor.draw == NULL)) {
        // phase 1 over
        EnKnight_SetupIgosStandCS(this, play);
        this->rightLegLowerRotation = this->leftLegLowerRotation = this->rightLegUpperRotation =
            this->leftLegUpperRotation = 0;
    }

    if (yaw != 0) {
        Vec3f hitmarkPos;

        Math_Vec3f_Copy(&hitmarkPos, &player->actor.world.pos);
        hitmarkPos.y += 25.0f;
        EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, &hitmarkPos);

        func_800B8D50(play, NULL, KREG(53) + 12.0f, this->actor.shape.rot.y + yaw, KREG(54) + 7.0f, 0x10);
        EnKnight_SpawnDust(play, 12);
        Actor_PlaySfx(&this->actor, NA_SE_EV_BLOCK_SHAKE);
    }
}

void EnKnight_SetupDetachHead(EnKnight* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosRemoveHeadAnim, -5.0f);
    this->actionFunc = EnKnight_DetachHead;
    this->timers[0] = 50;
}

void EnKnight_DetachHead(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Animation_OnFrame(&this->skelAnime, 19.0f)) {
        this->isHeadless = true;
        sIgosHeadInstance =
            (EnKnight*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KNIGHT, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, this->actor.shape.rot.x,
                                   this->actor.shape.rot.y, this->actor.shape.rot.z, EN_KNIGHT_PARAM_IGOS_HEAD);
        Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_HEAD_SHORT);
    }

    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if (this->timers[0] == 0) {
        EnKnight_SetupWait(this, play);
        Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_LAUGH_DEMO);
    }
}

void EnKnight_SetupFlyingHeadDone(EnKnight* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosPutHeadBackOnAnim, -10.0f);
    this->actionFunc = EnKnight_FlyingHeadDone;
    this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_0;
    this->timers[0] = 60;
    this->skelAnime.playSpeed = 0.0f;
}

void EnKnight_FlyingHeadDone(EnKnight* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->invincibilityTimer = 5;
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_1:
            this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_2;
            this->timers[0] = 40;
            this->skelAnime.playSpeed = 1.0f;
            FALLTHROUGH;
        case KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_2:
            if (this->timers[0] == 34) {
                this->isHeadless = false;
                Actor_Kill(&sIgosHeadInstance->actor);
                sIgosHeadInstance = NULL;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                player->focusActor = &this->actor;
                play->actorCtx.attention.tatlHoverActor = &this->actor;
                play->actorCtx.attention.reticleActor = &this->actor;
            }

            if (this->timers[0] == 15) {
                EnKnight_SetupWait(this, play);
                this->timers[2] = Rand_ZeroFloat(100.0f) + 150.0f;
            }
            FALLTHROUGH;
        case KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_0:
            // Wait for head
            break;
    }
}

void EnKnight_SetupBreathAttack(EnKnight* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosBreathAttackStartAnim, -5.0f);
    this->animEndFrame = Animation_GetLastFrame(&gKnightIgosBreathAttackStartAnim);
    this->actionFunc = EnKnight_BreathAttack;
    this->subAction = KNIGHT_SUB_ACTION_BREATH_ATTACK_0;
    this->timers[0] = KREG(57) + 150;
    this->breathAlpha = 0;
}

void EnKnight_BreathAttack(EnKnight* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    Math_ApproachS(&this->actor.world.rot.y, this->yawToPlayer, 5, 0x500);

    yawDiff = this->yawToPlayer - this->actor.shape.rot.y;
    if (ABS_ALT(yawDiff) < 0x6000) {
        if (yawDiff > 0x3800) {
            this->neckYawTarget = 0x3800;
        } else if (yawDiff < -0x3800) {
            this->neckYawTarget = -0x3800;
        } else {
            this->neckYawTarget = yawDiff;
        }
    }

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_BREATH_ATTACK_0:
            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                this->subAction = KNIGHT_SUB_ACTION_BREATH_ATTACK_1;
                Animation_MorphToLoop(&this->skelAnime, &gKnightIgosBreathAttackAnim, 0.0f);
            }

            EnKnight_SpawnBreathEffects(this, play, 0x1600);
            break;

        case KNIGHT_SUB_ACTION_BREATH_ATTACK_1:
            EnKnight_SpawnBreathEffects(this, play, 0x1600);

            if (KREG(21) == 0) {
                this->neckYawTarget = Math_SinS((KREG(84) + 0x11DC) * this->randTimer) * (KREG(85) * 0x800 + 0x1800) +
                                      this->neckYawTarget;
            }

            if (this->timers[0] == 0) {
                this->subAction = KNIGHT_SUB_ACTION_BREATH_ATTACK_2;
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosBreathAttackStopAnim, -5.0f);
                this->animEndFrame = Animation_GetLastFrame(&gKnightIgosBreathAttackStopAnim);
            }
            break;

        case KNIGHT_SUB_ACTION_BREATH_ATTACK_2:
            EnKnight_SpawnBreathEffects(this, play, 0x1600);

            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                EnKnight_SetupWait(this, play);
                this->timers[2] = Rand_ZeroFloat(150.0f) + 150.0f;
            }
            break;
    }

    this->canRetreat = true;
}

void EnKnight_SetupIgosStandCS(EnKnight* this, PlayState* play) {
    this->actionFunc = EnKnight_IgosStandCS;
    this->csTimer = 0;
    this->csState = KNIGHT_CS_1_STATE_0;
    this->csStepValue = 0.0f;
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
}

void EnKnight_IgosStandCS(EnKnight* this, PlayState* play) {
    this->csTimer++;
    SkelAnime_Update(&this->skelAnime);

    switch (this->csState) {
        case KNIGHT_CS_1_STATE_0:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->csTimer = 0;
            this->csState = KNIGHT_CS_1_STATE_1;
            this->subCamEye.x = 1342.0f;
            this->subCamEye.y = 56.0f;
            this->subCamEye.z = 2810.0f;
            this->subCamAt.x = 1400.0f;
            this->subCamAt.y = 57.0f;
            this->subCamAt.z = 2870.0f;
            this->subCamFov = 60.0f;
            FALLTHROUGH;
        case KNIGHT_CS_1_STATE_1:
            if (this->csTimer == (u32)(sREG(64) + 13)) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosStandAndDrawAnim, sREG(65));
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_STAND);
            }

            if (this->csTimer != (u32)(sREG(66) + 23)) {
                break;
            }

            this->subCamEye.x = 1253.0f;
            this->subCamEye.y = 79.0f;
            this->subCamEye.z = 2903.0f;
            this->subCamAt.x = 1354.0f;
            this->subCamAt.y = 83.0f;
            this->subCamAt.z = 2865.0f;
            Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosStandAndDrawAnim, 0.0f);
            this->csTimer = 0;
            this->csState = KNIGHT_CS_1_STATE_2;
            FALLTHROUGH;
        case KNIGHT_CS_1_STATE_2:
            this->csStepValue = sREG(67) * 0.001f + 0.05f;
            Math_ApproachF(&this->subCamAt.y, 119.0f, 0.2f, this->csStepValue * 36.0f);
            if (this->csTimer >= (u32)(sREG(68) + 10)) {
                f32 weaponsScale;

                if (this->csTimer == (u32)(sREG(68) + 10)) {
                    this->timers[1] = 17;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_SWORD);
                }

                weaponsScale = Math_SinS(this->timers[1] * 0x6000);
                weaponsScale = (weaponsScale * this->timers[1] * 0.0175f) + (this->timers[1] * 0.0175f) + 1.0f;

                Math_ApproachF(&this->swordScale, weaponsScale, 1.0f, 0.5f);
                Math_ApproachF(&this->shieldScale, weaponsScale, 1.0f, 0.5f);
            }

            if (this->csTimer == (u32)(sREG(69) + 50)) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosLaughAnim, sREG(65) + -10.0f);
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_ATTACK_K);
            }

            if (this->csTimer >= (u32)(sREG(69) + 50)) {
                Math_ApproachF(&this->subCamFov, sREG(70) + 40.0f, 0.2f, sREG(71) * 0.1f + 3.0f);
            }

            if (this->csTimer >= (u32)(sREG(72) + 90)) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);
                s32 pad;

                this->csState = KNIGHT_CS_1_STATE_0;

                EnKnight_SetupWait(this, play);

                this->timers[2] = 300;
                this->doBgChecks = true;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.gravity = -1.5f;
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                sKnightMusicStartTimer = 1;
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);
    }
}

void EnKnight_SetupKnightCaptainsHatCS(EnKnight* this, PlayState* play) {
    if ((this->actionFunc != EnKnight_FallOver) && (this->actionFunc != EnKnight_Die) &&
        (this->actionFunc != EnKnight_Dead)) {
        Animation_MorphToLoop(&this->skelAnime, &gKnightIdleAnim, -5.0f);
        this->actionFunc = EnKnight_KnightCaptainsHatCS;
    }
}

void EnKnight_KnightCaptainsHatCS(EnKnight* this, PlayState* play) {
    Vec3f offsetFromPlayer;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    this->neckYawTarget = 0;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.speed = 0.0f;

    if (sIgosInstance->subAction == KNIGHT_SUB_ACTION_CAPTAINS_HAT_CS_IGOS_SITTING) {
        // If Igos is still sitting, position the other two at specific positions behind the player
        if (this == sThinKnightInstance) {
            Matrix_RotateYS(sIgosInstance->actor.shape.rot.y + 0x1400, MTXMODE_NEW);
        } else {
            Matrix_RotateYS(sIgosInstance->actor.shape.rot.y - 0x1400, MTXMODE_NEW);
        }

        Matrix_MultVecZ((KREG(19) + 350.0f) - 220.0f, &offsetFromPlayer);
        this->actor.world.pos.x = player->actor.world.pos.x + offsetFromPlayer.x;
        this->actor.world.pos.z = player->actor.world.pos.z + offsetFromPlayer.z;
    }

    if (sIgosInstance->actionFunc != EnKnight_CaptainsHatCS) {
        EnKnight_SetupWait(this, play);
    }
}

void EnKnight_SetupCaptainsHatCS(EnKnight* this, PlayState* play) {
    this->csTimer = 0;
    this->csState = KNIGHT_CS_2_STATE_0;
    this->csStepValue = 0.0f;

    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 10);

    if (this->actionFunc != EnKnight_IgosSitting) {
        this->subAction = KNIGHT_SUB_ACTION_CAPTAINS_HAT_CS_IGOS_STANDING;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    } else {
        this->subAction = KNIGHT_SUB_ACTION_CAPTAINS_HAT_CS_IGOS_SITTING;
    }

    EnKnight_SetupKnightCaptainsHatCS(sThinKnightInstance, play);
    EnKnight_SetupKnightCaptainsHatCS(sWideKnightInstance, play);
    this->prevActionFunc = this->actionFunc;
    this->actionFunc = EnKnight_CaptainsHatCS;
}

void EnKnight_CaptainsHatCS(EnKnight* this, PlayState* play) {
    Vec3f posOffset;
    Player* player = GET_PLAYER(play);

    this->neckYawTarget = 0;
    this->actor.speed = 0.0f;
    this->csTimer++;
    this->animMovementX = 0.0f;
    this->animMovementZ = 0.0f;

    SkelAnime_Update(&this->skelAnime);

    switch (this->csState) {
        case KNIGHT_CS_2_STATE_0:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->csTimer = 0;
            this->csState = KNIGHT_CS_2_STATE_1;
            this->subCamFov = 60.0f;

            if (this->prevActionFunc == EnKnight_IgosSitting) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightCaptainsHatCSBeginSittingAnim, 0.0f);
            } else {
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightCaptainsHatCSBeginStandingAnim, 0.0f);
            }

            Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_STAND);
            FALLTHROUGH;
        case KNIGHT_CS_2_STATE_1:
            Matrix_RotateYS(KREG(29) + this->actor.shape.rot.y + 0x1B58, MTXMODE_NEW);
            Matrix_MultVecZ(KREG(16) + 90.0f, &posOffset);

            this->subCamEye.x = this->actor.world.pos.x + posOffset.x;
            this->subCamEye.y = this->actor.world.pos.y + KREG(17) + 40.0f;
            this->subCamEye.z = this->actor.world.pos.z + posOffset.z;

            this->subCamAt.x = this->actor.focus.pos.x;
            this->subCamAt.y = this->actor.focus.pos.y + KREG(18) - 10.0f;
            this->subCamAt.z = this->actor.focus.pos.z;

            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

            player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;

            Matrix_MultVecZ((KREG(19) + 350.0f) - 220.0f, &posOffset);

            player->actor.world.pos.x = this->actor.world.pos.x + posOffset.x;
            player->actor.world.pos.z = this->actor.world.pos.z + posOffset.z;

            if (this->csTimer == (u32)(KREG(31) + 25)) {
                this->csState = KNIGHT_CS_2_STATE_2;
                this->csTimer = 0;
                this->csStepValue = KREG(20) + 80.0f;
            }
            break;

        case KNIGHT_CS_2_STATE_2:
            Matrix_RotateYS(player->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_RotateXS(KREG(26) - 0x7D0, MTXMODE_APPLY);
            Matrix_MultVecZ(this->csStepValue, &posOffset);

            if (this->csTimer > 25) {
                Math_ApproachF(&this->csStepValue, 40.0f, 0.2f, 2.0f);
            }

            this->subCamEye.x = player->actor.world.pos.x + posOffset.x;
            this->subCamEye.y = KREG(21) + (player->actor.world.pos.y + posOffset.y) + 44.0f;
            this->subCamEye.z = player->actor.world.pos.z + posOffset.z;

            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = (player->actor.world.pos.y + KREG(22) + 44.0f) - 5.0f;
            this->subCamAt.z = player->actor.world.pos.z;

            if (this->csTimer == (u32)(KREG(48) + 85)) {
                this->csState = KNIGHT_CS_2_STATE_3;
                this->csTimer = 0;
                this->animEndFrame = 1000.0f;
            }
            break;

        case KNIGHT_CS_2_STATE_3:
            if (this->csTimer == 7) {
                if (this->prevActionFunc == EnKnight_IgosSitting) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightCaptainsHatCSIgosShockedSittingAnim, 0.0f);
                    this->animEndFrame = Animation_GetLastFrame(&gKnightCaptainsHatCSIgosShockedSittingAnim);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_STAND_RAPID);
                } else {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightCaptainsHatCSIgosShockedStandingAnim, 0.0f);
                    this->animEndFrame = Animation_GetLastFrame(&gKnightCaptainsHatCSIgosShockedStandingAnim);
                }
                Message_StartTextbox(play, 0x153E, NULL);
            }

            Matrix_RotateYS(KREG(29) + this->actor.shape.rot.y + 0x1B58, MTXMODE_NEW);
            Matrix_MultVecZ(KREG(16) + 90.0f, &posOffset);

            this->subCamEye.x = this->actor.world.pos.x + posOffset.x;
            this->subCamEye.y = this->actor.world.pos.y + KREG(17) + 40.0f;
            this->subCamEye.z = this->actor.world.pos.z + posOffset.z;

            if (this->csTimer < (u32)(KREG(24) + 35)) {
                this->subCamAt.x = this->actor.focus.pos.x;
                this->subCamAt.y = this->actor.focus.pos.y + KREG(18) - 10.0f;
                this->subCamAt.z = this->actor.focus.pos.z;
            }

            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                Animation_MorphToLoop(&this->skelAnime, &gKnightCaptainsHatCSIgosShockedAnim, 0.0f);
                this->animEndFrame = 1000.0f;
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_HAND);
            }

            if (this->csTimer == (u32)(KREG(25) + 120)) {
                this->csState = KNIGHT_CS_2_STATE_4;
                this->csTimer = 0;
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightCaptainsHatCSIgosShockedAnim, 0.0f);
            }
            break;

        case KNIGHT_CS_2_STATE_4:
            Matrix_RotateYS(KREG(29) * 0x100 + this->actor.shape.rot.y + 0x2400, MTXMODE_NEW);
            Matrix_MultVecZ(KREG(16) + 230.0f, &posOffset);

            this->subCamEye.x = this->actor.world.pos.x + posOffset.x;
            this->subCamEye.y = this->actor.world.pos.y + KREG(17) + 40.0f;
            this->subCamEye.z = this->actor.world.pos.z + posOffset.z;

            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_MultVecZ(KREG(49) + 65.0f, &posOffset);

            this->subCamAt.x = this->actor.world.pos.x + posOffset.x;
            this->subCamAt.y = (this->actor.world.pos.y + KREG(50) + 50.0f) - 20.0f;
            this->subCamAt.z = this->actor.world.pos.z + posOffset.z;

            if (this->csTimer == 60) {
                Message_StartTextbox(play, 0x153F, NULL);
            }

            if (this->csTimer == (u32)(BREG(16) + 120)) {
                Message_CloseTextbox(play);
            }

            if (this->csTimer == (u32)(BREG(17) + 160)) {
                Message_StartTextbox(play, 0x1542, NULL);
                Animation_MorphToLoop(&this->skelAnime, &gKnightCaptainsHatCSIgosSteppingAnim, 0.0f);
            }

            if (this->csTimer == (u32)(BREG(18) + 170)) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);
            }

            if ((this->csTimer > 156) && (this->csTimer <= 220)) {
                if (!(this->csTimer & 1)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_WALK);
                }
                if (!(this->csTimer & 3)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_SIT);
                }
            }

            if (this->csTimer == (u32)(BREG(19) + 220)) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosShrugStartAnim, -3.0f);
            }

            if (this->csTimer == (u32)(BREG(20) + 230)) {
                Message_StartTextbox(play, 0x1540, NULL);
            }

            if (this->csTimer == (u32)(BREG(20) + 240)) {
                Animation_MorphToLoop(&this->skelAnime, &gKnightIgosShrugAnim, 0.0f);
            }

            if (this->csTimer == (u32)(BREG(21) + 320)) {
                if (this->prevActionFunc == EnKnight_IgosSitting) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightCaptainsHatCSSitBackDownAnim, 0.0f);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_SIT);
                } else {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosShrugStopAnim, 0.0f);
                }
                Message_CloseTextbox(play);
            }

            if (this->csTimer >= (u32)(BREG(22) + 340)) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);
                s32 pad;

                this->csState = KNIGHT_CS_2_STATE_0;
                if (this->prevActionFunc == EnKnight_IgosSitting) {
                    EnKnight_SetupIgosSitting(this, play);
                    this->skelAnime.curFrame = 25.0f;
                } else {
                    EnKnight_SetupWait(this, play);
                    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                }

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;

                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                sKnightMusicStartTimer = 5;
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);
    }
}

void EnKnight_WaitIntroCutscene(EnKnight* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnKnight_IntroCutscene(EnKnight* this, PlayState* play) {
    f32 scale = 0.0f;
    Player* player = GET_PLAYER(play);

    this->csTimer++;
    SkelAnime_Update(&this->skelAnime);

    switch (this->csState) {
        case KNIGHT_INTRO_CS_STATE_0:
            // If the room clear flag is set, or there is a cs playing, or the player is too far away, don't play the cs
            if (Flags_GetClear(play, play->roomCtx.curRoom.num) || (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) ||
                !(player->actor.world.pos.x >= 870.0f)) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->csTimer = 0;
            this->csState = KNIGHT_INTRO_CS_STATE_1;
            this->subCamFov = 60.0f;
            this->subCamEye.x = 621.0f;
            this->subCamEye.y = 54.0f;
            this->subCamEye.z = 2865.0f;
            this->subCamAt.x = 661.0f;
            this->subCamAt.y = 110.0f;
            this->subCamAt.z = 2939.0f;

            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 80);
            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_1:
            player->actor.world.pos.x = 870.0f;
            player->actor.world.pos.z = 2865.0f;
            this->csPlayerYaw = 0x4000;
            this->subCamEye.x = player->actor.world.pos.x + BREG(16) + 100.0f;
            this->subCamEye.y = player->actor.world.pos.y + BREG(17) + 20.0f;
            this->subCamEye.z = player->actor.world.pos.z + BREG(18);
            this->subCamAt.x = player->actor.world.pos.x + BREG(19);
            this->subCamAt.y = player->actor.world.pos.y + BREG(20) + 40.0f;
            this->subCamAt.z = player->actor.world.pos.z + BREG(21);

            if (this->csTimer >= (u32)(BREG(22) + 20)) {
                scale = 0.5f;
                Audio_PlaySfx(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
            }

            if (this->csTimer == (u32)(BREG(22) + 30)) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_21);
            }

            if (this->csTimer == (u32)(BREG(22) + 50)) {
                Message_StartTextbox(play, 0x1518, NULL);
            }

            if (this->csTimer != (u32)(BREG(23) + 150)) {
                break;
            }

            Message_CloseTextbox(play);

            this->csState = KNIGHT_INTRO_CS_STATE_2;
            this->csTimer = 0;
            this->closeCurtainAction = KNIGHT_CLOSE_CURTAIN_ACTION_1;
            play->envCtx.lightSettingOverride = 1;
            play->envCtx.lightBlendOverride = 0;
            sIgosInstance->roomLightingState = 0;
            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_2:
            this->curtainsLightRayAngle = 0x8000;

            if (this->csTimer == 10) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);
            }

            if (this->csTimer >= 10) {
                Math_ApproachS(&this->csPlayerYaw, 0, 2, 0x1000);
            }

            this->subCamEye.x = 896.0f;
            this->subCamEye.y = 4.0f;
            this->subCamEye.z = 2752.0f;
            this->subCamAt.x = 894.0f;
            this->subCamAt.y = 46.0f;
            this->subCamAt.z = 2860.0f;

            if (this->csTimer == (u32)(BREG(65) + 40)) {
                Message_StartTextbox(play, 0x1519, NULL);
            }

            if ((u32)(BREG(94) + 130) >= this->csTimer) {
                scale = 0.5f;
                Audio_PlaySfx(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
            }

            if (this->csTimer == (u32)(BREG(65) + 140)) {
                Message_CloseTextbox(play);
            }

            if (this->csTimer == (u32)(BREG(65) + 170)) {
                Message_StartTextbox(play, 0x151A, NULL);
            }

            if ((this->csTimer >= 170) && (this->csTimer <= 190)) {
                this->jawRotationAmplitudeTarget = KREG(42) + 200.0f;
            }

            if ((this->csTimer == 150) || (this->csTimer == 180)) {
                Actor_PlaySfx(&this->actor, this->walkSfx);
            }

            if (this->csTimer != (u32)(BREG(24) + 190)) {
                break;
            }

            this->csState = KNIGHT_INTRO_CS_STATE_3;
            this->csTimer = 0;
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_133);
            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_3:
            Math_ApproachS(&this->csPlayerYaw, 0x4000, 2, 0x1000);

            if (this->csTimer != (u32)(BREG(26) + 15)) {
                break;
            }

            this->csState = KNIGHT_INTRO_CS_STATE_4;
            this->csTimer = 0;
            Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIntroSitDownAnim, 0.0f);
            this->animEndFrame = Animation_GetLastFrame(&gKnightIntroSitDownAnim);

            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_4:
            this->csStepValue = BREG(29) * 0.01f + 0.1f;
            Math_ApproachF(&this->subCamEye.x, 1076.0f, 0.5f, this->csStepValue * 180.0f);
            Math_ApproachF(&this->subCamEye.y, 20.0f, 0.5f, this->csStepValue * 16.0f);
            Math_ApproachF(&this->subCamEye.z, 2741.0f, 0.5f, this->csStepValue * 11.0f);
            Math_ApproachF(&this->subCamAt.x, 1155.0f, 0.5f, this->csStepValue * 261.0f);
            Math_ApproachF(&this->subCamAt.y, 50.0f, 0.5f, this->csStepValue * 4.0f);
            Math_ApproachF(&this->subCamAt.z, 2822.0f, 0.5f, this->csStepValue * 38.0f);

            this->actor.world.pos.x = BREG(27) + 1376.0f;
            this->actor.world.pos.y = 45.0f;
            this->actor.world.pos.z = BREG(28) + 2864.0f;

            if (this->csTimer != (u32)(BREG(29) + 20)) {
                break;
            }

            this->csState = KNIGHT_INTRO_CS_STATE_5;
            this->csTimer = 0;
            Animation_MorphToLoop(&sWideKnightInstance->skelAnime, &gKnightIntroWalkAnim, 0.0f);
            sWideKnightInstance->actor.world.pos.x = BREG(30) + 1363.0f + 120.0f;
            Message_StartTextbox(play, 0x1533, NULL);
            this->subCamFov = 30.0f;
            play->envCtx.lightSettingOverride = 2;
            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_5:
            if (this->csTimer == (u32)(BREG(71) + 2)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_SIT);
            }

            if ((this->csTimer == (u32)(BREG(72) + 10)) || (this->csTimer == (u32)(BREG(73) + 25)) ||
                (this->csTimer == (u32)(BREG(74) + 40))) {
                Actor_PlaySfx(&sWideKnightInstance->actor, sWideKnightInstance->walkSfx);
            }

            if ((this->csTimer > 8) && (this->csTimer < 44)) {
                this->jawRotationAmplitudeTarget = KREG(42) + 200.0f;
            }

            if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                Animation_MorphToLoop(&this->skelAnime, &gKnightIgosSittingAnim, 0.0f);
                this->animEndFrame = 1000.0f;
            }

            this->subCamEye.x = 1349.0f;
            this->subCamEye.y = 126.0f;
            this->subCamEye.z = 2740.0f;
            this->subCamAt.x = 1377.0f;
            this->subCamAt.y = 121.0f;
            this->subCamAt.z = 2851.0f;
            Math_ApproachF(&sWideKnightInstance->actor.world.pos.x, BREG(30) + 1363.0f + 30.0f, 1.0f, BREG(32) + 2.0f);
            sWideKnightInstance->actor.world.pos.y = 45.0f;
            sWideKnightInstance->actor.world.pos.z = BREG(31) + 2864.0f + 60.0f;

            if (this->csTimer == (u32)(BREG(35) + 45)) {
                Animation_MorphToPlayOnce(&sWideKnightInstance->skelAnime, &gKnightLaugh2Anim, -10.0f);
                Actor_PlaySfx(&sWideKnightInstance->actor, NA_SE_EN_DEBU_PAUSE_K);
            }

            if (this->csTimer != (u32)(BREG(33) + 80)) {
                break;
            }

            this->csState = KNIGHT_INTRO_CS_STATE_6;
            this->csTimer = 0;
            Animation_MorphToLoop(&sThinKnightInstance->skelAnime, &gKnightIntroWalkAnim, 0.0f);
            sThinKnightInstance->actor.world.pos.x = BREG(30) + 1363.0f + 120.0f;
            sWideKnightInstance->actor.world.pos.z = 3164.0f;
            Message_StartTextbox(play, 0x151B, NULL);
            play->envCtx.lightSettingOverride = 3;
            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_6:
            if ((this->csTimer == (u32)(BREG(72) + 10)) || (this->csTimer == (u32)(BREG(73) + 25)) ||
                (this->csTimer == (u32)(BREG(74) + 40))) {
                Actor_PlaySfx(&sThinKnightInstance->actor, sThinKnightInstance->walkSfx);
            }

            if ((this->csTimer >= 10) && (this->csTimer < 34)) {
                this->jawRotationAmplitudeTarget = KREG(42) + 200.0f;
            }

            this->subCamEye.x = 1343.0f;
            this->subCamEye.y = 146.0f;
            this->subCamEye.z = 2975.0f;
            this->subCamAt.x = 1372.0f;
            this->subCamAt.y = 122.0f;
            this->subCamAt.z = 2867.0f;
            Math_ApproachF(&sThinKnightInstance->actor.world.pos.x, BREG(30) + 1363.0f + 30.0f, 1.0f, BREG(32) + 2.0f);
            sThinKnightInstance->actor.world.pos.y = 45.0f;
            sThinKnightInstance->actor.world.pos.z = (BREG(31) + 2864.0f) - 60.0f;

            if (this->csTimer == (u32)(BREG(35) + 45)) {
                Animation_MorphToPlayOnce(&sThinKnightInstance->skelAnime, &gKnightLaugh2Anim, -10.0f);
                Actor_PlaySfx(&sThinKnightInstance->actor, NA_SE_EN_YASE_PAUSE_K);
            }

            if (this->csTimer != (u32)(BREG(33) + 80)) {
                break;
            }

            this->csState = KNIGHT_INTRO_CS_STATE_7;
            this->csTimer = 0;
            sThinKnightInstance->actor.world.pos.z = ((BREG(31) + 2864.0f) - 60.0f) - 10.0f;
            sWideKnightInstance->actor.world.pos.z = BREG(31) + 2864.0f + 60.0f + 10.0f;
            sThinKnightInstance->actor.world.pos.x = (BREG(30) + 1363.0f + 30.0f) - 40.0f;
            sWideKnightInstance->actor.world.pos.x = (BREG(30) + 1363.0f + 30.0f) - 40.0f;
            this->subCamEye.x = 1339.0f;
            this->subCamEye.y = 117.0f;
            this->subCamEye.z = 2864.0f;
            this->subCamAt.x = 1452.0f;
            this->subCamAt.y = 120.0f;
            this->subCamAt.z = 2864.0f;
            this->csStepValue = 0.0f;
            Message_StartTextbox(play, 0x1534, NULL);
            this->subCamFov = 60.0f;
            play->envCtx.lightSettingOverride = 4;
            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_7:
            if (this->csTimer >= (u32)(BREG(37) + 20)) {
                if (this->csTimer == (u32)(BREG(37) + 20)) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosStandAndDrawAnim, 0.0f);
                    this->animEndFrame = Animation_GetLastFrame(&gKnightIgosStandAndDrawAnim);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_STAND);
                }

                if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
                    Animation_MorphToLoop(&this->skelAnime, &gKnightIgosLaughAnim, 0.0f);
                    this->animEndFrame = 1000.0f;
                }
            }

            if (this->csTimer < (u32)(BREG(87) + 20)) {
                this->jawRotationAmplitudeTarget = 200.0f;
            }

            if (this->csTimer == (u32)(BREG(37) + 25)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_HAND);
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_ATTACK);
            }

            if (this->csTimer == (u32)(BREG(44) + 40)) {
                Animation_MorphToLoop(&sThinKnightInstance->skelAnime, &gKnightLaugh1Anim, -5.0f);
                Animation_MorphToLoop(&sWideKnightInstance->skelAnime, &gKnightLaugh1Anim, -5.0f);
            }

            if (this->csTimer == 40) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_HEAD_MID);
            }

            if (this->csTimer == 45) {
                Actor_PlaySfx(&sWideKnightInstance->actor, NA_SE_EN_DEBU_LAUGH_K);
            }

            if (this->csTimer == 50) {
                Actor_PlaySfx(&sThinKnightInstance->actor, NA_SE_EN_YASE_LAUGH_K);
            }

            Math_ApproachF(&this->subCamEye.x, 1196.0f, 0.1f, this->csStepValue * 143.0f);
            Math_ApproachF(&this->subCamEye.y, 52.0f, 0.1f, this->csStepValue * 65.0f);
            Math_ApproachF(&this->subCamAt.x, 1302.0f, 0.1f, this->csStepValue * 150.0f);
            Math_ApproachF(&this->subCamAt.y, 90.0f, 0.1f, this->csStepValue * 30.0f);
            Math_ApproachF(&this->csStepValue, BREG(45) * 0.01f + 0.1f, 1.0f, BREG(45) * 0.0001f + 0.01f);

            if (this->csTimer != (u32)(BREG(18) + 60)) {
                break;
            }

            this->csState = KNIGHT_INTRO_CS_STATE_8;
            this->csTimer = 0;

            this->subCamEye.x = 1309.0f;
            this->subCamEye.y = 103.0f;
            this->subCamEye.z = 2790.0f;

            this->subCamAt.x = 1421.0f;
            this->subCamAt.y = 103.0f;
            this->subCamAt.z = 2790.0f;

            play->envCtx.lightSettingOverride = 5;
            Actor_PlaySfx(&sThinKnightInstance->actor, NA_SE_EN_YASE_LAUGH_K);
            FALLTHROUGH;
        case KNIGHT_INTRO_CS_STATE_8:
            if (this->csTimer == 15) {
                this->subCamEye.x = 1309.0f;
                this->subCamEye.y = 103.0f;
                this->subCamEye.z = 2930.0f;
                this->subCamAt.x = 1421.0f;
                this->subCamAt.y = 103.0f;
                this->subCamAt.z = 2930.0f;
                Actor_PlaySfx(&sWideKnightInstance->actor, NA_SE_EN_DEBU_LAUGH_K);
                Actor_PlaySfx(&sThinKnightInstance->actor, NA_SE_EN_YASE_LAUGH_K);
            }

            if (this->csTimer == 30) {
                this->subCamEye.x = 1324.0f;
                this->subCamEye.y = 144.0f;
                this->subCamEye.z = 2864.0f;
                this->subCamAt.x = 1436.0f;
                this->subCamAt.y = 144.0f;
                this->subCamAt.z = 2864.0f;
                play->envCtx.lightSettingOverride = 6;
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_LAUGH_DEMO_K);
                Actor_PlaySfx(&sWideKnightInstance->actor, NA_SE_EN_DEBU_LAUGH_K);
                Actor_PlaySfx(&sThinKnightInstance->actor, NA_SE_EN_YASE_LAUGH_K);
            }

            if (this->csTimer == (u32)(BREG(19) + 55)) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                this->csState = KNIGHT_INTRO_CS_STATE_0;

                EnKnight_SetupIgosSitting(this, play);
                EnKnight_SetupTurnToPlayer(sThinKnightInstance, play);
                EnKnight_SetupTurnToPlayer(sWideKnightInstance, play);

                sThinKnightInstance->actor.gravity = sWideKnightInstance->actor.gravity = -1.5f;
                sThinKnightInstance->doBgChecks = sWideKnightInstance->doBgChecks = true;

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;

                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                sKnightMusicStartTimer = 1;
                this->closeCurtainAction = KNIGHT_CLOSE_CURTAIN_ACTION_2;
                Message_CloseTextbox(play);
                play->envCtx.lightSettingOverride = 7;
                SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_IGOS_DU_IKANA);
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Vec3f eye;
        Vec3f at;
        f32 yOffset;

        ShrinkWindow_Letterbox_SetSizeTarget(27);
        yOffset = Math_SinS(this->randTimer * 0x6000) * 0.5f * scale;

        eye.x = this->subCamEye.x;
        eye.y = this->subCamEye.y + yOffset;
        eye.z = this->subCamEye.z;
        at.x = this->subCamAt.x;
        at.y = this->subCamAt.y + 2.0f * yOffset;
        at.z = this->subCamAt.z;
        Play_SetCameraAtEye(play, this->subCamId, &at, &eye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->csPlayerYaw;
    }

    Math_ApproachF(&this->jawRotationAmplitude, this->jawRotationAmplitudeTarget, 1.0f, 2.0f * KREG(41) + 200.0f);
    this->jawRotation = Math_SinS((KREG(40) * 0x100 + 0x3F00) * this->randTimer) * this->jawRotationAmplitude +
                        this->jawRotationAmplitude;
    Math_ApproachZeroF(&this->jawRotationAmplitude, 1.0f, KREG(41) + 100.0f);

    if ((this->jawRotationAmplitudeTarget >= 50.0f) && (this->randTimer % 2 == 0) && (Rand_ZeroOne() < 0.5f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_TALK);
    }
}

void EnKnight_UpdateDamage(EnKnight* this, PlayState* play) {
    s32 j;
    s32 i;
    Vec3f translation;
    Player* player = GET_PLAYER(play);

    if (this->shieldCollider.elements[0].base.acElemFlags & ACELEM_HIT) {
        this->shieldCollider.elements[0].base.acElemFlags &= ~ACELEM_HIT;
        this->shieldingInvulnerabilityTimer = 5;

        if ((player->meleeWeaponState != PLAYER_MWA_FORWARD_SLASH_1H) &&
            (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) && (this->shieldHitTimer == 0)) {
            Audio_PlaySfx(NA_SE_IT_SHIELD_REFLECT_SW);
            CollisionCheck_SpawnShieldParticlesMetal(play, &this->shieldParticlesPos);
            this->shieldHitTimer = 5;
        }
        Matrix_RotateYS(this->yawToPlayer, MTXMODE_NEW);
        Matrix_MultVecZ((this == sIgosInstance) ? -6.0f : -4.0f, &translation);
        this->animMovementX = translation.x;
        this->animMovementZ = translation.z;
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->bodyColliderElements); i++) {
        if (this->invincibilityTimer != 0) {
            continue;
        }

        if (this->bodyCollider.elements[i].base.acElemFlags & ACELEM_HIT) {
            this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;

            switch (this->actor.colChkInfo.damageEffect) {
                case KNIGHT_DMGEFF_ICE:
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_10;
                    EnKnight_SetupStunned(this, play, 80);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    this->invincibilityTimer = 15;
                    break;

                case KNIGHT_DMGEFF_STUN:
                    EnKnight_SetupStunned(this, play, 40);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    this->invincibilityTimer = 15;
                    break;

                case KNIGHT_DMGEFF_FIRE:
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_1;
                    break;

                case KNIGHT_DMGEFF_LIGHT:
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_20;
                    break;

                case KNIGHT_DMGEFF_LIGHT_RAY:
                    this->lightRayDamageTimer = 10;
                    break;

                case KNIGHT_DMGEFF_ZORA_BARRIER:
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_40;
                    EnKnight_SetupStunned(this, play, 40);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    this->invincibilityTimer = 15;
                    break;

                case KNIGHT_DMGEFF_GORON_POUND:
                    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                        Matrix_RotateYS(this->yawToPlayer, MTXMODE_NEW);
                        Matrix_MultVecZ(-4.0f, &translation);
                        this->animMovementX = translation.x;
                        this->animMovementZ = translation.z;
                        this->actor.velocity.y = 5.0f;

                        for (j = 0; j < 5; j++) {
                            EnKnight_SpawnDustAtFeet(this, play, 0);
                        }
                    }
                    break;
            }

            if (this->actor.colChkInfo.damage != 0) {
                this->actor.colChkInfo.health -= this->actor.colChkInfo.damage;

                if ((s8)this->actor.colChkInfo.health <= 0) {
                    EnKnight_SetupFallOver(this, play);
                    Enemy_StartFinishingBlow(play, &this->actor);
                    Actor_PlaySfx(&this->actor, this->defeatedSfx);
                } else if (this->actor.colChkInfo.damageEffect != KNIGHT_DMGEFF_ICE) {
                    EnKnight_SetupRecoilFromDamage(this, play);
                    Actor_PlaySfx(&this->actor, this->damagedSfx);
                }
                this->invincibilityTimer = 10;
                this->damageFlashTimer = 15;
            }
            this->canRetreat = false;
        }
    }
}

void EnKnight_UpdateDamageFallenOver(EnKnight* this, PlayState* play) {
    s32 i;

    if (this->invincibilityTimer != 0) {
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->bodyColliderElements); i++) {
        ColliderJntSphElement* jntSphElem = &this->bodyCollider.elements[i];
        ColliderElement* acHitElem;

        if (jntSphElem->base.acElemFlags & ACELEM_HIT) {
            jntSphElem->base.acElemFlags &= ~ACELEM_HIT;

            acHitElem = jntSphElem->base.acHitElem;

            this->invincibilityTimer = 10;
            if (acHitElem->atDmgInfo.dmgFlags & DMG_LIGHT_RAY) {
                this->damageFlashTimer = 15;
                this->invincibilityTimer = 1000;
                EnKnight_SetupDie(this, play);
            }
        }
    }
}

void EnKnight_SpawnIceShards(EnKnight* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 170, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 200, 200, 255, 255 };
    static Vec3f sAccel = { 0.0f, -1.0f, 0.0f };
    Vec3f pos;
    Vec3f velocity;
    s32 i;

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->bodyPartsPos[KNIGHT_BODYPART_JAW], 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < ARRAY_COUNT(this->bodyPartsPos); i++) {
        velocity.x = Rand_CenteredFloat(7.0f);
        velocity.z = Rand_CenteredFloat(7.0f);
        velocity.y = Rand_ZeroFloat(6.0f) + 4.0f;
        pos.x = velocity.x + this->bodyPartsPos[i].x;
        pos.y = velocity.y + this->bodyPartsPos[i].y;
        pos.z = velocity.z + this->bodyPartsPos[i].z;
        EffectSsEnIce_Spawn(play, &pos, Rand_ZeroFloat(0.5f) + 0.7f, &velocity, &sAccel, &sPrimColor, &sEnvColor, 30);
    }
}

void EnKnight_UpdateDamageFlyingHead(EnKnight* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->invincibilityTimer != 0) {
        return;
    }

    if (this->headCollider.base.acFlags & AC_HIT) {
        this->headCollider.base.acFlags &= ~AC_HIT;
        this->invincibilityTimer = 15;

        if (&this->actor == player->actor.parent) {
            player->av2.actionVar2 = 101;
            player->actor.parent = NULL;
            player->csAction = 0;
            Play_DisableMotionBlur();
            func_800B8D50(play, &this->actor, 10.0f, sIgosInstance->yawToPlayer, 5.0f, 16);
            EnKnight_SetupFlyingHead(this, play);
            this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_1;
            this->timers[1] = 30;
            Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_LAUGH_DEMO_K);
        }
    }
}

void EnKnight_SetupFlyingHead(EnKnight* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosRemoveHeadAnim, 0.0f);
    this->skelAnime.curFrame = 19.0f;
    this->actionFunc = EnKnight_FlyingHead;
    this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_0;
    this->timers[1] = KREG(77) + 200;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
}

void EnKnight_FlyingHead(EnKnight* this, PlayState* play) {
    f32 dx;
    f32 dy;
    f32 dz;
    s32 pad;
    f32 dist;
    s16 pitch;
    s16 yaw;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_HEAD_FLOAT - SFX_FLAG);

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_FLYING_HEAD_0:
            this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_1;
            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_MultVecZ(500.0f, &this->retreatTowards);
            this->retreatTowards.x += this->actor.world.pos.x;
            this->retreatTowards.y = this->actor.world.pos.y + 500.0f;
            this->retreatTowards.z += this->actor.world.pos.z;
            this->actor.world.rot.x = KREG(39) * 0x1000 + 0x2000;
            this->timers[0] = 20;
            sIgosInstance->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            player->focusActor = &this->actor;
            play->actorCtx.attention.tatlHoverActor = &this->actor;
            play->actorCtx.attention.reticleActor = &this->actor;
            Math_Vec3f_Copy(&this->actor.world.pos, &sIgosInstance->actor.world.pos);
            Math_Vec3s_Copy(&this->actor.world.rot, &sIgosInstance->actor.world.rot);
            Math_Vec3s_Copy(&this->actor.shape.rot, &sIgosInstance->actor.world.rot);
            this->invincibilityTimer = 10;
            this->doBgChecks = true;
            break;

        case KNIGHT_SUB_ACTION_FLYING_HEAD_1:
            this->jawRotation = Math_SinS((KREG(40) * 0x100 + 0x1200) * this->randTimer) * 1000.0f + 1000.0f;
            dx = this->retreatTowards.x - this->actor.world.pos.x;
            dy = this->retreatTowards.y - this->actor.world.pos.y;
            dz = this->retreatTowards.z - this->actor.world.pos.z;
            dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));

            if ((this->timers[1] == 1) || (KREG(5) != 0)) {
                KREG(5) = 0;
                if ((sIgosInstance->actionFunc != EnKnight_FallOver) && (sIgosInstance->actionFunc != EnKnight_Die)) {
                    this->timers[1] = 0;
                    EnKnight_SetupFlyingHeadDone(sIgosInstance, play);
                    Animation_MorphToPlayOnce(&this->skelAnime, &gKnightIgosPutHeadBackOnAnim, -10.0f);
                    this->doBgChecks = false;
                    this->skelAnime.playSpeed = 0.0f;
                } else {
                    this->timers[1] = 50;
                }
            }

            if (this->timers[1] == 0) {
                Math_Vec3f_Copy(&this->retreatTowards, &sIgosInstance->actor.world.pos);
                this->retreatTowards.y += 100.0f;

                if (dist < 100.0f) {
                    Math_ApproachF(&this->actor.speed, (f32)sREG(48) + 1.5f, 1.0f, 0.3f);
                }
                if (dist < 30.0f) {
                    this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_2;
                    this->timers[0] = 50;
                    this->invincibilityTimer = 1000;
                    break;
                }
            } else {
                Math_ApproachF(&this->actor.speed, sREG(48) + 5.0f, 1.0f, 5.0f);

                if ((this->timers[0] == 0) || (dist < 50.0f)) {
                    this->timers[0] = Rand_ZeroFloat(50.0f) + 50.0f;

                    if (Rand_ZeroOne() < 0.2f) {
                        Math_Vec3f_Copy(&this->retreatTowards, &player->actor.world.pos);
                        this->retreatTowards.y += 25.0f;
                    } else {
                        this->retreatTowards.x = Rand_CenteredFloat(500.0f) + player->actor.world.pos.x;
                        this->retreatTowards.y = Rand_ZeroFloat(100.0f) + player->actor.world.pos.y + 50.0f;
                        this->retreatTowards.z = Rand_CenteredFloat(500.0f) + player->actor.world.pos.z;
                    }
                }
            }

            yaw = Math_Atan2S(dx, dz);
            pitch = Math_Atan2S(dy, sqrtf(SQ(dx) + SQ(dz)));
            Math_ApproachS(&this->actor.world.rot.y, yaw, 0xA, 0x800);
            Math_ApproachS(&this->actor.world.rot.x, pitch, 0xA, 0x800);
            Math_ApproachF(&this->afterImageOffset.x, 300.0f, 0.1f, 3.0f);
            Math_ApproachF(&this->afterImageOffset.y, -3800.0f, 0.1f, 38.0f);
            Math_ApproachF(&this->afterImageOffset.z, -900.0f, 0.1f, 9.0f);
            Math_ApproachS(&this->actor.shape.rot.y, this->yawToPlayer, 0xA, 0x200);
            Math_ApproachS(&this->actor.shape.rot.x, this->pitchToPlayer, 0xA, 0x200);

            if (this->timers[1] == 40) {
                EnKnight_SetupFlyingHeadAttack(this, play);
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_CYNICAL);
            }
            break;

        case KNIGHT_SUB_ACTION_FLYING_HEAD_2:
        case KNIGHT_SUB_ACTION_FLYING_HEAD_3:
            this->invincibilityTimer = 10;
            Math_ApproachZeroF(&this->actor.speed, 1.0f, 0.1f);
            Math_ApproachS(&this->actor.shape.rot.y, sIgosInstance->actor.world.rot.y, 5, 0x800);
            Math_ApproachS(&this->actor.shape.rot.x, 0, 5, 0x800);
            Math_ApproachZeroF(&this->afterImageOffset.x, 0.5f, (sREG(73) * 0.01f + 0.05f) * 300.0f);
            Math_ApproachZeroF(&this->afterImageOffset.y, 0.5f, (sREG(73) * 0.01f + 0.05f) * 3800.0f);
            Math_ApproachZeroF(&this->afterImageOffset.z, 0.5f, (sREG(73) * 0.01f + 0.05f) * 900.0f);
            Math_ApproachF(&this->actor.world.pos.x, sIgosInstance->actor.world.pos.x, 0.5f, sREG(74) * 0.1f + 4.0f);
            Math_ApproachF(&this->actor.world.pos.y, sIgosInstance->actor.world.pos.y, 0.5f, sREG(74) * 0.1f + 4.0f);
            Math_ApproachF(&this->actor.world.pos.z, sIgosInstance->actor.world.pos.z, 0.5f, sREG(74) * 0.1f + 4.0f);

            if (this->subAction == KNIGHT_SUB_ACTION_FLYING_HEAD_2) {
                if ((fabsf(this->actor.world.pos.x - sIgosInstance->actor.world.pos.x) < 30.0f) &&
                    (fabsf(this->actor.world.pos.z - sIgosInstance->actor.world.pos.z) < 30.0f) &&
                    (fabsf(this->actor.world.pos.y - sIgosInstance->actor.world.pos.y) < sREG(75) * 0.1f + 30.0f)) {
                    this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_3;
                    sIgosInstance->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_DONE_1;
                    this->skelAnime.playSpeed = 1.0f;
                }
            }
            break;
    }
}

void EnKnight_SetupFlyingHeadAttack(EnKnight* this, PlayState* play) {
    this->actionFunc = EnKnight_FlyingHeadAttack;
    this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_0;
    this->timers[0] = BREG(16) + 60;
    this->timers[2] = 250;
}

void EnKnight_FlyingHeadAttack(EnKnight* this, PlayState* play) {
    Vec3f targetHeight;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 dxz;
    s16 pitch;
    s16 yaw;
    Player* player = GET_PLAYER(play);
    s32 pad;

    Matrix_RotateYS(player->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_MultVecZ(7.0f, &targetHeight);

    if (player->transformation == PLAYER_FORM_GORON) {
        targetHeight.y = 67.0f;
    } else if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
        targetHeight.y = 85.0f;
    } else if (player->transformation == PLAYER_FORM_ZORA) {
        targetHeight.y = 60.0f;
    } else if (player->transformation == PLAYER_FORM_DEKU) {
        targetHeight.y = 34.0f;
    } else { // PLAYER_FORM_HUMAN
        targetHeight.y = 40.0f;
    }

    dx = player->actor.world.pos.x - this->actor.world.pos.x;
    dy = BREG(17) + (player->actor.world.pos.y + 100.0f) - this->actor.world.pos.y;
    dz = player->actor.world.pos.z - this->actor.world.pos.z;

    if (this->subAction != KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_1) {
        f32 jawRotationAdd = (this->subAction == KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_2) ? 3000.0f : 1000.0f;

        this->jawRotation = Math_SinS((KREG(40) * 0x100 + 0x1200) * this->randTimer) * 1000.0f + jawRotationAdd;
    }

    dxz = sqrtf(SQ(dx) + SQ(dz));

    switch (this->subAction) {
        case KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_0:
            Math_ApproachF(&this->actor.speed, BREG(18) + 10.0f, 1.0f, 1.0f);
            yaw = Math_Atan2S(dx, dz);
            pitch = Math_Atan2S(dy, dxz);
            Math_ApproachS(&this->actor.world.rot.y, yaw, 0xA, 0x800);
            Math_ApproachS(&this->actor.world.rot.x, pitch, 0xA, 0x800);

            if (this->timers[2] == 0) {
                EnKnight_SetupFlyingHead(this, play);
                this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_1;
                this->timers[1] = 30;
                break;
            }

            if ((this->timers[0] == 0) && (dxz < BREG(19) + 100.0f)) {
                this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_1;
                this->timers[2] = 60;
                this->csStepValue = 0.0f;
            }
            break;

        case KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_1:
            this->doBgChecks = false;
            Math_ApproachS(&this->jawRotation, 0xFA0, 5, 0x1000);
            Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
            Math_ApproachF(&this->actor.world.pos.x, player->actor.world.pos.x + targetHeight.x, 1.0f,
                           this->csStepValue);
            Math_ApproachF(&this->actor.world.pos.y, player->actor.world.pos.y + targetHeight.y, 1.0f,
                           this->csStepValue);
            Math_ApproachF(&this->actor.world.pos.z, player->actor.world.pos.z + targetHeight.z, 1.0f,
                           this->csStepValue);
            Math_ApproachF(&this->csStepValue, BREG(20) + 20.0f, 1.0f, BREG(21) + 0.5f);

            if ((this->timers[2] == 0) || (player->stateFlags3 & PLAYER_STATE3_1000)) {
                EnKnight_SetupFlyingHead(this, play);
                this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_1;
                this->timers[1] = 30;
                break;
            }

            if (dxz < BREG(22) + 20.0f) {
                if (play->grabPlayer(play, player)) {
                    Play_EnableMotionBlur(150);
                    player->actor.parent = &this->actor;
                    AudioSfx_PlaySfx(NA_SE_VO_LI_DAMAGE_S, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                    this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_2;
                    this->timers[0] = 80;
                } else {
                    this->timers[2] = 0;
                }
            }
            break;

        case KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_2:
            if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
                CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B)) {

                DECR(this->timers[0]);
                DECR(this->timers[0]);
                DECR(this->timers[0]);
                DECR(this->timers[0]);
                DECR(this->timers[0]);
            }

            Math_ApproachF(&this->actor.world.pos.x, player->actor.world.pos.x + targetHeight.x, 1.0f,
                           this->csStepValue);
            Math_ApproachF(&this->actor.world.pos.y, player->actor.world.pos.y + targetHeight.y, 1.0f,
                           this->csStepValue);
            Math_ApproachF(&this->actor.world.pos.z, player->actor.world.pos.z + targetHeight.z, 1.0f,
                           this->csStepValue);
            Math_ApproachF(&this->csStepValue, BREG(20) + 20.0f, 1.0f, BREG(21) + 0.5f);
            Math_ApproachS(&this->actor.shape.rot.y, player->actor.shape.rot.y + 0x8000, 2, 0x1000);
            Math_ApproachS(&this->actor.shape.rot.x, BREG(83) * 0x800 + 0x4000, 2, 0x1000);
            player->av2.actionVar2 = 90;
            Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_HEAD_BITE - SFX_FLAG);

            if ((this->timers[0] == 0) && (&this->actor == player->actor.parent)) {
                player->av2.actionVar2 = 101;
                player->actor.parent = NULL;
                player->csAction = 0;
                Play_DisableMotionBlur();
                EnKnight_SetupFlyingHead(this, play);
                this->subAction = KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_1;
                this->timers[1] = 30;
                Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_LAUGH_DEMO_K);
            }
            break;
    }

    if (this->subAction != KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_2) {
        Math_ApproachS(&this->actor.shape.rot.y, this->yawToPlayer, 0xA, 0x200);
        Math_ApproachS(&this->actor.shape.rot.x, this->pitchToPlayer, 0xA, 0x200);
        Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_HEAD_FLOAT - SFX_FLAG);
    }
}

void EnKnight_Update(Actor* thisx, PlayState* play) {
    EnKnight* this = (EnKnight*)thisx;
    Input* input;
    PlayState* play2 = play;
    Player* player = GET_PLAYER(play);
    f32 dx;
    f32 dy;
    f32 dz;
    u8 i;
    s32 limbIndex;

    Math_Vec3f_Copy(&this->projectedPos, &this->actor.projectedPos);

    dx = player->actor.world.pos.x - this->actor.focus.pos.x;
    dy = KREG(93) + ((player->actor.world.pos.y + 34.0f) - this->actor.focus.pos.y);
    dz = player->actor.world.pos.z - this->actor.focus.pos.z;

    this->yawToPlayer = Math_Atan2S_XY(dz, dx);
    this->pitchToPlayer = Math_Atan2S(-dy, sqrtf(SQ(dx) + SQ(dz)));

    this->randTimer++;

    if (this == sIgosHeadInstance) {
        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            if (this->timers[i] != 0) {
                this->timers[i]--;
            }
        }

        if (this->invincibilityTimer != 0) {
            this->invincibilityTimer--;
        }

        if (this->damageFlashTimer != 0) {
            this->damageFlashTimer--;
        }

        this->actionFunc(this, play);

        Math_ApproachS(&this->neckYaw, this->neckYawTarget, 5, 0x2000);
        this->neckYawTarget = 0;
        Actor_UpdateVelocityWithoutGravity(&this->actor);
        Actor_UpdatePos(&this->actor);
        this->actor.world.pos.x += this->animMovementX;
        this->actor.world.pos.z += this->animMovementZ;
        Math_ApproachZeroF(&this->animMovementX, 1.0f, KREG(86) + 2.0f);
        Math_ApproachZeroF(&this->animMovementZ, 1.0f, KREG(86) + 2.0f);

        if (this->doBgChecks) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 20.0f, 100.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
        }

        EnKnight_UpdateDamageFlyingHead(this, play);

        this->headAttackCollider.dim.pos.x = (s32)this->actor.focus.pos.x;
        this->headAttackCollider.dim.pos.y = (s32)this->actor.focus.pos.y;
        this->headAttackCollider.dim.pos.z = (s32)this->actor.focus.pos.z;

        if (this->actionFunc == EnKnight_FlyingHeadAttack) {
            if (this->subAction < KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_2) {
                if (this->subAction == KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_0) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->headAttackCollider.base);
                }
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->headAttackCollider.base);
            } else {
                this->headCollider.dim.radius = KREG(34) + 20;
                this->headCollider.dim.height = KREG(35) + 40;
                this->headCollider.dim.yShift = 0;
                this->headCollider.dim.pos.x = (s32)player->actor.world.pos.x;
                this->headCollider.dim.pos.y = (s32)player->actor.world.pos.y;
                this->headCollider.dim.pos.z = (s32)player->actor.world.pos.z;
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->headCollider.base);
            }
        } else {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->headAttackCollider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->headAttackCollider.base);
        }

        if (((this->actionFunc != EnKnight_FlyingHeadAttack) ||
             (this->subAction != KNIGHT_SUB_ACTION_FLYING_HEAD_ATTACK_2)) &&
            !(this->randTimer & BREG(76))) {
            EnKnight* afterImage = (EnKnight*)Actor_SpawnAsChild(
                &play->actorCtx, &this->actor, play, ACTOR_EN_KNIGHT, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z,
                EN_KNIGHT_PARAM_IGOS_HEAD_AFTERIMAGE);
            if (afterImage != NULL) {
                for (limbIndex = 0; limbIndex < IGOS_LIMB_MAX; limbIndex++) {
                    afterImage->skelAnime.jointTable[limbIndex] = this->skelAnime.jointTable[limbIndex];
                    afterImage->skelAnime.skeleton = this->skelAnime.skeleton;
                    afterImage->skelAnime.dListCount = this->skelAnime.dListCount;
                }
                Math_Vec3f_Copy(&afterImage->afterImageOffset, &this->afterImageOffset);
            }
        }
        return;
    }

    this->inCurtainReaction = false;

    if (this == sIgosInstance) {
        Actor* findMirRay;

        sMirRayInstance = NULL;

        for (findMirRay = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first; findMirRay != NULL;
             findMirRay = findMirRay->next) {
            if (findMirRay->id == ACTOR_MIR_RAY3) {
                sMirRayInstance = (MirRay3*)findMirRay;
                break;
            }
        }

        this->curtainsLightRayAngle = CURRENT_TIME;
        Math_ApproachS(&this->jawRotation, 0, 2, 0x200);
    } else if (sIgosInstance->roomLightingState == 0) {
        this->actor.hintId = TATL_HINT_ID_KINGS_LACKEYS_DARK_ROOM;
    } else {
        this->actor.hintId = TATL_HINT_ID_KINGS_LACKEYS_LIGHT_ROOM;
    }

    if (sIgosInstance->inCurtainCutscene) {
        // Boss06 sets this when the curtain burning cutscene begins

        if ((this == sIgosInstance) && (this->actionFunc == EnKnight_IgosSitting)) {
            // Igos, setup reaction
            EnKnight_SetupIgosReactToCurtains(this, play);
        }

        if (!((this == sIgosInstance) && (this->actionFunc == EnKnight_IgosReactToCurtains))) {
            // Not Igos, do nothing
            return;
        }
    }

    this->swordColliderActive = false;
    this->shieldColliderActive = false;
    this->canRetreat = false;
    this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT3;

    if (((KREG(63) == 0) && (this != sIgosInstance)) || ((KREG(63) != 2) && (this == sIgosInstance))) {
        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            if (this->timers[i] != 0) {
                this->timers[i]--;
            }
        }

        if (this->invincibilityTimer != 0) {
            this->invincibilityTimer--;
        }

        if (this->damageFlashTimer != 0) {
            this->damageFlashTimer--;
        }

        if (this->shieldHitTimer != 0) {
            this->shieldHitTimer--;
        }

        if (this->drawDmgEffDuration != 0) {
            this->drawDmgEffDuration--;
        }

        if (this->effectTimer != 0) {
            this->effectTimer--;
        }

        if (this->lightRayDamageTimer != 0) {
            this->lightRayDamageTimer--;
        }

        Math_ApproachZeroF(&this->shieldLightReflectionScale, 1.0f, 0.2f);

        if ((this->actionFunc != EnKnight_FallOver) && (this->actionFunc != EnKnight_Die)) {
            Math_ApproachS(&this->actor.shape.rot.x, 0, 10, 0x200);
        }

        this->actionFunc(this, play);

        Actor_MoveWithGravity(&this->actor);
        this->actor.world.pos.x += this->animMovementX;
        this->actor.world.pos.z += this->animMovementZ;

        if (this->canRetreat || (this->actionFunc == EnKnight_Retreat)) {
            EnKnight_CheckRetreat(this, play);

            if (this->canRetreat && (sTargetKnight != NULL) && (this != sTargetKnight)) {
                EnKnight_SetupLookAtOther(this, play);
            } else if ((this != sIgosInstance) && (player->stateFlags3 & PLAYER_STATE3_20000000)) {
                EnKnight_SetupMarch(this, play);
            }
        }
    }

    if (this->doBgChecks) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 100.0f, 50.0f, 150.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.velocity.y <= 0.0f) {
            this->actor.velocity.y = -2.0f;
        }

        Math_ApproachZeroF(&this->animMovementX, 1.0f, 1.0f);
        Math_ApproachZeroF(&this->animMovementZ, 1.0f, 1.0f);
    }

    Math_ApproachS(&this->neckYaw, this->neckYawTarget, this->neckRotationStepScale, this->neckRotationMaxStep);
    this->neckRotationMaxStep = 0x1000;
    this->neckRotationStepScale = 3;
    Math_ApproachS(&this->neckRoll, this->neckRollTarget, this->neckRotationStepScale, this->neckRotationMaxStep);
    this->neckRollTarget = 0;

    if (this->actionFunc != EnKnight_ApproachPlayer) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0xA00);
    }

    if ((this->actionFunc != EnKnight_WaitIntroCutscene) && (this->actionFunc != EnKnight_Die) &&
        (this->actionFunc != EnKnight_Dead) && (this->actionFunc != EnKnight_IgosSitting) &&
        (this->actionFunc != EnKnight_DetachHead) && (this->actionFunc != EnKnight_FlyingHeadDone) &&
        !Play_InCsMode(play)) {

        if (this->actionFunc != EnKnight_FallOver) {
            EnKnight_UpdateDamage(this, play);

            if (this->swordColliderActive) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->swordCollider.base);
                CollisionCheck_SetAT(play, &play2->colChkCtx, &this->swordCollider.base);
            }

            if (this->shieldColliderActive) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->shieldCollider.base);
            }

            if (!this->isHeadless) {
                if (this->shieldingInvulnerabilityTimer == 0) {
                    CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
                } else {
                    this->shieldingInvulnerabilityTimer--;
                }
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);

                if (this->canRetreat &&
                    ((player->unk_D57 != 0) || ((player->unk_ADC != 0) && (this->actor.xzDistToPlayer <= 120.0f))) &&
                    EnKnight_IsFacingPlayer(this, play)) {
                    f32 randThreshold = (this == sIgosInstance) ? 0.65f : 0.25f;

                    if ((Rand_ZeroOne() < randThreshold) && (player->unk_ADC != 0) &&
                        (this->actionFunc != EnKnight_Blocking) && !(this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
                        EnKnight_SetupJumpBackwardsKnight(this, play);
                    } else {
                        EnKnight_SetupBlocking(this, play);
                    }
                }
            }
        } else {
            EnKnight_UpdateDamageFallenOver(this, play);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }
    }

    input = CONTROLLER4(&play->state);
    if (CHECK_BTN_ALL(input->press.button, BTN_L) && (this == sIgosInstance)) {
        // Left over debug feature to skip the first phase
        EnKnight_SetupDie(sThinKnightInstance, play);
        EnKnight_SetupDie(sWideKnightInstance, play);
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_R) && (this == sIgosInstance)) {
        // Left over debug feature to test damage effects
        this->drawDmgEffState = KREG(17);
    }

    switch (this->drawDmgEffState) {
        case KNIGHT_DMGEFF_STATE_0:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffDuration = 0;
            this->drawDmgEffAlpha = 0.0f;
            break;

        case KNIGHT_DMGEFF_STATE_1:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffDuration = 80;
            this->drawDmgEffState++; // = KNIGHT_DMGEFF_STATE_2
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = 0.0f;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 120, 0, 60);
            FALLTHROUGH;
        case KNIGHT_DMGEFF_STATE_2:
            if (this->drawDmgEffDuration == 0) {
                Math_ApproachZeroF(&this->drawDmgEffAlpha, 1.0f, 0.02f);
                if (this->drawDmgEffAlpha == 0.0f) {
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_0;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 0.5f, 0.1f, 0.5f);
            }
            break;

        case KNIGHT_DMGEFF_STATE_10:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
            this->drawDmgEffDuration = 80;
            this->drawDmgEffState++; // = KNIGHT_DMGEFF_STATE_11
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = 0.0f;
            this->drawDmgEffIceMeltingScale = 1.0f;
            FALLTHROUGH;
        case KNIGHT_DMGEFF_STATE_11:
            if (this->drawDmgEffDuration == 0) {
                EnKnight_SpawnIceShards(this, play);
                this->drawDmgEffState = KNIGHT_DMGEFF_STATE_0;
            } else {
                if (this->drawDmgEffDuration == 50) {
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
                }
                Math_ApproachF(&this->drawDmgEffScale, 0.5f, 1.0f, 0.08f);
                Math_ApproachF(&this->drawDmgEffIceMeltingScale, 0.5f, 0.05f, 0.05f);
            }
            break;

        case KNIGHT_DMGEFF_STATE_20:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
            this->drawDmgEffDuration = 40;
            this->drawDmgEffState++; // = KNIGHT_DMGEFF_STATE_21
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = 0.0f;
            FALLTHROUGH;
        case KNIGHT_DMGEFF_STATE_21:
            if (this->drawDmgEffDuration == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.03f);
                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_0;
                    this->drawDmgEffAlpha = 0.0f;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 0.75f, 0.5f, 0.5f);
            }
            break;

        case KNIGHT_DMGEFF_STATE_30:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
            this->drawDmgEffDuration = 10;
            this->drawDmgEffState++; // = KNIGHT_DMGEFF_STATE_31
            this->drawDmgEffAlpha = 0.5f;
            FALLTHROUGH;
        case KNIGHT_DMGEFF_STATE_31:
            if (this->drawDmgEffDuration == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.03f);
                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_0;
                    this->drawDmgEffAlpha = 0.0f;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 0.5f, 0.5f, 0.3f);
            }
            break;

        case KNIGHT_DMGEFF_STATE_40:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
            this->drawDmgEffDuration = 50;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = KREG(18) * 0.1f + 0.5f;
            this->drawDmgEffState++; // = KNIGHT_DMGEFF_STATE_40
            FALLTHROUGH;
        case KNIGHT_DMGEFF_STATE_41:
            if (this->drawDmgEffDuration == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.05f);
                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = KNIGHT_DMGEFF_STATE_0;
                    this->drawDmgEffAlpha = 0.0f;
                }
            }
            break;
    }

    this->jawRotationAmplitudeTarget = 0.0f;

    if (this == sIgosInstance) {
        if (sKnightMusicStartTimer != 0) {
            sKnightMusicStartTimer--;
            if (sKnightMusicStartTimer == 0) {
                Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
            }
        }

        if (this->isHeadless) {
            Math_ApproachF(&this->bodyAlpha, 60.0f, 1.0f, 8.0f);
        } else {
            Math_ApproachF(&this->bodyAlpha, 255.0f, 1.0f, 8.0f);
        }

        EnKnight_UpdateEffects(this, play);

        if (this->roomLightingState != 0) {
            u16 time;

            play->envCtx.lightSettingOverride = 0;
            play->envCtx.lightBlendOverride = 2;
            if (this->roomLightingState == 1) {
                play->envCtx.lightSetting = 8;
                play->envCtx.prevLightSetting = 12;
            } else {
                play->envCtx.lightSetting = 0;
                play->envCtx.prevLightSetting = 10;
            }

            time = CURRENT_TIME;
            if (time > CLOCK_TIME(12, 0)) {
                time = (DAY_LENGTH - 1) - time;
            }
            play->envCtx.lightBlend = time * (2.0f / 0x10000);
        }

        // Captain's Hat easter egg
        if (!this->seenCaptainsHat && !this->isHeadless && (Player_GetMask(play) == PLAYER_MASK_CAPTAIN) &&
            ((this->actionFunc == EnKnight_IgosSitting) || this->canRetreat) &&
            (this->actor.xzDistToPlayer < (KREG(82) + 130.0f)) && EnKnight_IsFacingPlayer(this, play)) {
            this->seenCaptainsHat = true;
            EnKnight_SetupCaptainsHatCS(this, play);
        }
    }

    this->actor.shape.shadowAlpha = (this->shadowAlphaFactor * this->bodyAlpha) / 255.0f;
}

void EnKnight_DrawEffectBlure(EnKnight* this, PlayState* play) {
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(&gKnightBlureTexAnim));

    // BENTODO, will this work on 64 bit.
    gSPDisplayList(POLY_XLU_DISP++, 0x08000000 | 1);

    if (this == sIgosInstance) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (u8)(sREG(11) + 180), 255, 255,
                        (u8)((sREG(14) * 0.1f + 1.0f) * this->blureAlpha));
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, (u8)((sREG(14) * 0.1f + 1.0f) * this->blureAlpha));
    }

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(this->blureTranslation.x, this->blureTranslation.y, this->blureTranslation.z, MTXMODE_APPLY);
    Matrix_RotateXFApply(this->blureRotation.x);
    Matrix_RotateZF(this->blureRotation.z, MTXMODE_APPLY);
    Matrix_RotateYF(this->blureRotation.y, MTXMODE_APPLY);

    scale = (this == sIgosInstance) ? (17.0f / 13) : 1.0f;
    Matrix_Scale(0.021f * scale, 0.021f * scale, 0.021f * scale, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    switch (this->blureState) {
        case 0:
            gSPDisplayList(POLY_XLU_DISP++, gKnightBlureState0DL);
            break;

        case 1:
            gSPDisplayList(POLY_XLU_DISP++, gKnightBlureState1DL);
            break;

        default:
        case 2:
            gSPDisplayList(POLY_XLU_DISP++, gKnightBlureState2DL);
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnKnight_OverrideLimbDrawHead(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnKnight* this = (EnKnight*)thisx;

    if (limbIndex == IGOS_LIMB_NECK) {
        rot->x += (f32)this->neckYaw;
    }

    if (limbIndex == IGOS_LIMB_JAW) {
        rot->z += this->jawRotation;
    } else if (limbIndex == IGOS_LIMB_HEAD) {
        rot->z -= this->jawRotation;
    }

    if ((limbIndex != IGOS_LIMB_JAW) && (limbIndex != IGOS_LIMB_HEAD)) {
        *dList = NULL;
    }
    return false;
}

s32 EnKnight_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                              Gfx** gfx) {
    EnKnight* this = (EnKnight*)thisx;

    if (limbIndex == KNIGHT_LIMB_NECK) {
        rot->x += (f32)this->neckYaw;
        rot->z += this->neckRoll;
    }

    if (this == sIgosInstance) {
        if (limbIndex == IGOS_LIMB_JAW) {
            rot->z += this->jawRotation;
        } else if (limbIndex == IGOS_LIMB_HEAD) {
            rot->z -= this->jawRotation;
        }

        if (limbIndex == IGOS_LIMB_LEFT_LEG_UPPER) {
            rot->z -= this->leftLegUpperRotation;
        }

        if (limbIndex == IGOS_LIMB_LEFT_LEG_LOWER) {
            rot->z -= this->leftLegLowerRotation;
        }

        if (limbIndex == IGOS_LIMB_RIGHT_LEG_UPPER) {
            rot->z -= this->rightLegUpperRotation;
        }

        if (limbIndex == IGOS_LIMB_RIGHT_LEG_LOWER) {
            rot->z -= this->rightLegLowerRotation;
        }

        if (((limbIndex == IGOS_LIMB_JAW) || (limbIndex == IGOS_LIMB_HEAD)) && this->isHeadless) {
            *dList = NULL;
        }
    } else if (this == sWideKnightInstance) {
        if (limbIndex == KNIGHT_LIMB_NECK) {
            *dList = gKnightWideNeckDL;
        } else if (limbIndex == KNIGHT_LIMB_JAW) {
            *dList = gKnightWideJawDL;
        } else if (limbIndex == KNIGHT_LIMB_HEAD) {
            *dList = gKnightWideHeadDL;
        }
    }
    return false;
}

void EnKnight_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    static Vec3f sFocusPosModelOffset = { 100.0f, 0.0f, 0.0f };
    static Vec3f sShieldColliderModelOffset = { 0.0f, 600.0f, 0.0f };
    static Vec3f sShieldParticlesModelOffset = { 0.0f, 600.0f, 1000.0f };
    static Vec3f sSwordColliderModelOffset = { 1000.0f, 0.0f, 0.0f };
    static s8 sLimbToBodyParts[KNIGHT_LIMB_MAX + 1] = {
        BODYPART_NONE,
        BODYPART_NONE,
        BODYPART_NONE,
        BODYPART_NONE,
        KNIGHT_BODYPART_LEFT_UPPER_ARM,
        KNIGHT_BODYPART_LEFT_FOREARM,
        KNIGHT_BODYPART_SHIELD,
        BODYPART_NONE,
        BODYPART_NONE,
        KNIGHT_BODYPART_RIGHT_UPPER_ARM,
        KNIGHT_BODYPART_RIGHT_FOREARM,
        KNIGHT_BODYPART_SWORD,
        BODYPART_NONE,
        BODYPART_NONE,
        BODYPART_NONE,
        BODYPART_NONE,
        BODYPART_NONE,
        BODYPART_NONE,
        KNIGHT_BODYPART_JAW,
        BODYPART_NONE,
        KNIGHT_BODYPART_TORSO,
        KNIGHT_BODYPART_PELVIS,
        BODYPART_NONE,
        KNIGHT_BODYPART_LEFT_LEG_UPPER,
        KNIGHT_BODYPART_LEFT_LEG_LOWER,
        KNIGHT_BODYPART_LEFT_FOOT,
        KNIGHT_BODYPART_RIGHT_LEG_UPPER,
        KNIGHT_BODYPART_RIGHT_LEG_LOWER,
        KNIGHT_BODYPART_RIGHT_FOOT,
        BODYPART_NONE,
    };
    static Vec3f sBreathBaseModelOffset = { 300.0f, 500.0f, 0.0f };
    static Vec3f sBodyCollider0ModelOffset = { 1000.0f, 0.0f, 0.0f };
    static Vec3f sBodyCollider1ModelOffset = { 700.0f, -500.0f, 0.0f };
    EnKnight* this = (EnKnight*)thisx;
    Vec3f colliderPos;

    if ((this->actionFunc == EnKnight_Die) || (this->drawDmgEffAlpha > 0.0f)) {
        s8 bodyPartIndex = sLimbToBodyParts[limbIndex];
        if (bodyPartIndex > BODYPART_NONE) {
            Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
        }
    }

    if (limbIndex == KNIGHT_LIMB_NECK) {
        Matrix_MultVec3f(&sFocusPosModelOffset, &this->actor.focus.pos);
        Matrix_MultVec3f(&sBreathBaseModelOffset, &this->breathBasePos);
    }

    if (limbIndex == KNIGHT_LIMB_TORSO) {
        Matrix_MultVec3f(&sBodyCollider0ModelOffset, &colliderPos);
        EnKnight_SetColliderSphere(this, 0, &this->bodyCollider, &colliderPos);
    }

    if (limbIndex == KNIGHT_LIMB_PELVIS) {
        Matrix_MultVec3f(&sBodyCollider1ModelOffset, &colliderPos);
        EnKnight_SetColliderSphere(this, 1, &this->bodyCollider, &colliderPos);
    }

    if ((limbIndex == KNIGHT_LIMB_SHIELD) && (this->actionFunc != EnKnight_IgosSitting)) {
        Matrix_MultVec3f(&sShieldColliderModelOffset, &colliderPos);
        EnKnight_SetColliderSphere(this, 0, &this->shieldCollider, &colliderPos);
        Matrix_MultVec3f(&sShieldParticlesModelOffset, &this->shieldParticlesPos);
    }

    if (limbIndex == KNIGHT_LIMB_SWORD) {
        Matrix_MultVec3f(&sSwordColliderModelOffset, &colliderPos);
        EnKnight_SetColliderSphere(this, 0, &this->swordCollider, &colliderPos);
    }

    if (limbIndex == KNIGHT_LIMB_RIGHT_FOOT) {
        Matrix_MultZero(&this->feetPositions[0]);
    }

    if (limbIndex == KNIGHT_LIMB_LEFT_FOOT) {
        Matrix_MultZero(&this->feetPositions[1]);
    }
}

void EnKnight_PostLimbDrawHead(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusPosModelOffset = { 300.0f, 500.0f, 0.0f };
    EnKnight* this = (EnKnight*)thisx;

    if (limbIndex == KNIGHT_LIMB_NECK) {
        Matrix_MultVec3f(&sFocusPosModelOffset, &this->actor.focus.pos);
        Math_Vec3f_Copy(&this->breathBasePos, &this->actor.focus.pos);
    }
}

void EnKnight_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx, Gfx** gfx) {
    EnKnight* this = (EnKnight*)thisx;

    if (this == sIgosInstance) {
        if (limbIndex == IGOS_LIMB_SHIELD) {
            Matrix_Scale(this->shieldScale, this->shieldScale, this->shieldScale, MTXMODE_APPLY);
        } else if (limbIndex == IGOS_LIMB_SWORD) {
            Matrix_Scale(this->swordScale, this->swordScale, this->swordScale, MTXMODE_APPLY);
        }
    }
}

Gfx* EnKnight_BuildEmptyDL(GraphicsContext* gfxCtx) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 1 * sizeof(Gfx));
    gSPEndDisplayList(gfx);
    return gfx;
}

Gfx* EnKnight_BuildXluMaterialDL(GraphicsContext* gfxCtx, u8 alpha) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 6 * sizeof(Gfx));
    Gfx* gfxHead = gfx;

    gDPPipeSync(gfxHead++);
    gDPSetCombineLERP(gfxHead++, TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, COMBINED, 0,
                      ENVIRONMENT, 0);
    gDPSetEnvColor(gfxHead++, 255, 255, 255, alpha);
    gDPSetRenderMode(gfxHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gSPClearGeometryMode(gfxHead++, G_CULL_BACK);
    gSPEndDisplayList(gfxHead++);
    return gfx;
}

void EnKnight_Draw(Actor* thisx, PlayState* play) {
    EnKnight* this = (EnKnight*)thisx;
    f32 scale;
    s32 drawXlu = false;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0A, EnKnight_BuildEmptyDL(play->state.gfxCtx));
    gSPSegment(POLY_XLU_DISP++, 0x0A, EnKnight_BuildEmptyDL(play->state.gfxCtx));

    if (this->bodyAlpha >= 254.0f) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x09, EnKnight_BuildEmptyDL(play->state.gfxCtx));
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x09, EnKnight_BuildXluMaterialDL(play->state.gfxCtx, (u32)this->bodyAlpha));
        drawXlu = true;
    }

    if (this->damageFlashTimer % 2 != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }

    if (this->actionFunc == EnKnight_Die) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 0, 0, 0, 255, 900, 1099);
    }

    if (this == sIgosHeadInstance) {
        Matrix_Translate(this->afterImageOffset.x, this->afterImageOffset.y, this->afterImageOffset.z, MTXMODE_APPLY);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnKnight_OverrideLimbDrawHead, EnKnight_PostLimbDrawHead, &this->actor);
    } else {
        if ((this == sIgosInstance) && drawXlu) {
            POLY_XLU_DISP =
                SubS_DrawTransformFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnKnight_OverrideLimbDraw, EnKnight_PostLimbDraw,
                                       EnKnight_TransformLimbDraw, &this->actor, POLY_XLU_DISP);
        } else {
            POLY_OPA_DISP =
                SubS_DrawTransformFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnKnight_OverrideLimbDraw, EnKnight_PostLimbDraw,
                                       EnKnight_TransformLimbDraw, &this->actor, POLY_OPA_DISP);
        }

        if (this->blureAlpha > 0.0f) {
            EnKnight_DrawEffectBlure(this, play);
            Math_ApproachZeroF(&this->blureAlpha, 1.0f, 40.0f);
            this->blureState++;
            if (this->blureState >= 3) {
                this->blureState = 2;
            }
        } else {
            this->blureState = 0;
        }

        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ARRAY_COUNT(this->bodyPartsPos),
                                this->drawDmgEffScale, this->drawDmgEffIceMeltingScale, this->drawDmgEffAlpha,
                                this->drawDmgEffType);

        if (this->shieldLightReflectionScale > 0.01f) {
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, (u8)(sREG(18) + 220), (u8)(sREG(16) + 170));
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, (u8)(sREG(22) + 100), 128);

            Matrix_Translate(this->shieldParticlesPos.x, this->shieldParticlesPos.y, this->shieldParticlesPos.z,
                             MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_RotateZS(play->gameplayFrames * 0x80, MTXMODE_APPLY);

            scale = (sREG(17) + 600) * 0.01f * this->shieldLightReflectionScale;
            Matrix_Scale(scale, scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
        }
    }

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    if (this == sIgosInstance) {
        EnKnight_DrawEffects(this, play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnKnight_UpdateAfterImage(Actor* thisx, PlayState* play) {
    EnKnight* this = (EnKnight*)thisx;
    f32 maxStep;

    // Set alpha step
    if (this->actor.params == EN_KNIGHT_PARAM_IGOS_HEAD_AFTERIMAGE) {
        maxStep = BREG(75) + 15.0f;
    } else {
        maxStep = BREG(78) + 8.0f;
    }

    // Step alpha towards 0
    Math_ApproachZeroF(&this->bodyAlpha, 1.0f, maxStep);

    // Once alpha is sufficiently low, kill it
    if (this->bodyAlpha < 0.1f) {
        Actor_Kill(&this->actor);
    }
}

s32 EnKnight_OverrideLimbDrawAfterImage(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        Actor* thisx, Gfx** gfx) {
    EnKnight* this = (EnKnight*)thisx;

    if ((this->actor.params == EN_KNIGHT_PARAM_IGOS_HEAD_AFTERIMAGE) && (limbIndex != IGOS_LIMB_JAW) &&
        (limbIndex != IGOS_LIMB_HEAD)) {
        *dList = NULL;
    }
    return false;
}

void EnKnight_DrawAfterImage(Actor* thisx, PlayState* play) {
    EnKnight* this = (EnKnight*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x09, EnKnight_BuildXluMaterialDL(play->state.gfxCtx, this->bodyAlpha));
    gSPSegment(POLY_XLU_DISP++, 0x0A, EnKnight_BuildXluMaterialDL(play->state.gfxCtx, this->bodyAlpha));
    Matrix_Translate(this->afterImageOffset.x, this->afterImageOffset.y, this->afterImageOffset.z, MTXMODE_APPLY);
    POLY_XLU_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           EnKnight_OverrideLimbDrawAfterImage, NULL, &this->actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnKnight_UpdateEffects(EnKnight* this, PlayState* play) {
    EnKnightEffect* eff = play->specialEffects;
    Player* player = GET_PLAYER(play);
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sEnKnightEffects); i++, eff++) {
        if (eff->active) {
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;

            eff->scrollTimer++;

            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;

            eff->accel.y = BREG(56) * 0.01f + 1.0f;

            if (eff->active == true) {
                Math_ApproachF(&eff->scale, (KREG(59) * 0.01f + 1.0f) * eff->scaleTarget, 0.1f,
                               eff->scaleTarget * 0.1f);

                if (((s16)(BREG(60) + 110) >= eff->alpha) ||
                    (eff->pos.y < (BREG(62) + (this->actor.floorHeight + 30.0f)))) {
                    if ((s16)(BREG(60) + 110) < eff->alpha) {
                        eff->alpha = BREG(60) + 110;
                    }
                    eff->accel.y = BREG(61) * 0.01f + 0.3f;

                    eff->velocity.x *= 0.85f + BREG(59) * 0.01f;
                    eff->velocity.z *= 0.85f + BREG(59) * 0.01f;

                    eff->scaleTarget = (BREG(58) * 0.1f + 2.0f) * 0.025f;

                    if (eff->velocity.y < 0.0f) {
                        eff->velocity.y = 0.0f;
                    }
                }
                eff->alpha -= (s16)(BREG(49) + 10);
                if (eff->alpha <= 0) {
                    eff->alpha = 0;
                    eff->active = false;
                }
                if (this->effectTimer == 0) {
                    f32 dx = player->actor.world.pos.x - eff->pos.x;
                    f32 dy = (player->actor.world.pos.y + 25.0f) - eff->pos.y;
                    f32 dz = player->actor.world.pos.z - eff->pos.z;
                    if (SQ(dx) + SQ(dy) + SQ(dz) < (BREG(57) * 0.1f + 1.0f) * ((2500.0f * eff->scale) / 0.025f)) {
                        this->effectTimer = 10;
                        func_800B8D10(play, &this->actor, 0.0f, 0, 0.0f, 1, 4);
                    }
                }
            }
        }
    }
}

void EnKnight_DrawEffects(EnKnight* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    u8 materialFlag = 0;
    EnKnightEffect* eff = play->specialEffects;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetColorDither(POLY_XLU_DISP++, G_CD_BAYER);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_PATTERN);

    for (i = 0; i < ARRAY_COUNT(sEnKnightEffects); i++, eff++) {
        if (eff->active != true) {
            continue;
        }

        gDPPipeSync(POLY_XLU_DISP++);

        if (materialFlag == 0) {
            gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamMaterialDL);
            gDPSetEnvColor(POLY_XLU_DISP++, 155, 145, 155, 128);
            materialFlag++;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 155, 155, 255, eff->alpha);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, (eff->scrollTimer + i * 3) * 3,
                                      (eff->scrollTimer + i * 3) * 15, 32, 64, 1, 0, 0, 32, 32, 3, 15, 0, 0));

        Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamModelDL);
    }

    CLOSE_DISPS(gfxCtx);
}

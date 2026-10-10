/*
 * File: z_en_ik.c
 * Overlay: ovl_En_Ik
 * Description: Iron Knuckle
 */

#include "z_en_ik.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnIk_Init(Actor* thisx, PlayState* play);
void EnIk_Destroy(Actor* thisx, PlayState* play);
void EnIk_Update(Actor* thisx, PlayState* play2);
void EnIk_Draw(Actor* thisx, PlayState* play);

void EnIk_Thaw(EnIk* this, PlayState* play);
s32 EnIk_IsChangingAction(EnIk* this, PlayState* play);
s32 EnIk_ChooseAttack(EnIk* this);
void EnIk_CheckActions(EnIk* this, PlayState* play);
void EnIk_SetupIdle(EnIk* this);
void EnIk_Idle(EnIk* this, PlayState* play);
void EnIk_SetupWalk(EnIk* this);
void EnIk_WalkTowardsPlayer(EnIk* this, PlayState* play);
void EnIk_SetupRun(EnIk* this);
void EnIk_RunTowardsPlayer(EnIk* this, PlayState* play);
void EnIk_SetupVerticalAttack(EnIk* this);
void EnIk_VerticalAttack(EnIk* this, PlayState* play);
void EnIk_SetupTakeOutAxe(EnIk* this);
void EnIk_TakeOutAxe(EnIk* this, PlayState* play);
void EnIk_SetupHorizontalDoubleAttack(EnIk* this);
void EnIk_HorizontalDoubleAttack(EnIk* this, PlayState* play);
void EnIk_SetupSingleHorizontalAttack(EnIk* this);
void EnIk_SingleHorizontalAttack(EnIk* this, PlayState* play);
void EnIk_SetupEndHorizontalAttack(EnIk* this);
void EnIk_EndHorizontalAttack(EnIk* this, PlayState* play);
void EnIk_SetupBlock(EnIk* this);
void EnIk_Block(EnIk* this, PlayState* play);
void EnIk_SetupReactToAttack(EnIk* this, s32 arg1);
void EnIk_ReactToAttack(EnIk* this, PlayState* play);
void EnIk_SetupDie(EnIk* this);
void EnIk_Die(EnIk* this, PlayState* play);
void EnIk_Frozen(EnIk* this, PlayState* play);
void EnIk_SetupCutscene(EnIk* this);
void EnIk_PlayCutscene(EnIk* this, PlayState* play);
void EnIk_UpdateDamage(EnIk* this, PlayState* play);
void EnIk_UpdateArmor(EnIk* this, PlayState* play);

typedef struct {
    /* 0x0 */ Gfx* unk00;
    /* 0x4 */ s16 unk04;
} EnIkUnkStruct; // size = 0x8

EnIkUnkStruct sIronKnuckleArmorMarkings[IRON_KNUCKLE_ARMOR_BODYPART_MAX] = {
    { gIronKnuckleHelmetMarkingDL, 0x0000 },          // IRON_KNUCKLE_ARMOR_BODYPART_HELMET
    { gIronKnuckleFrontTorsoArmorMarkingDL, 0x0000 }, // IRON_KNUCKLE_ARMOR_BODYPART_CHEST_FRONT
    { gIronKnuckleRearTorsoArmorMarkingDL, 0x7FFF },  // IRON_KNUCKLE_ARMOR_BODYPART_CHEST_BACK
    { gIronKnuckleRivetsMarking2DL, 0x4000 },         // IRON_KNUCKLE_ARMOR_BODYPART_UPPER_LEFT_PAULDRON
    { gIronKnuckleRivetsMarking1DL, -0x4000 },        // IRON_KNUCKLE_ARMOR_BODYPART_UPPER_RIGHT_PAULDRON
    { NULL, 0x4000 },                                 // IRON_KNUCKLE_ARMOR_BODYPART_LOWER_LEFT_PAULDRON
    { NULL, -0x4000 },                                // IRON_KNUCKLE_ARMOR_BODYPART_LOWER_RIGHT_PAULDRON
};

// sIronKnuckleArmorType[PARAM_VALUE][ARMOR_SECTIONS]
// ARMOR SECTIONS:
// 1 - Main armor
// 2 - Armor Accent (Side of armor and boot tips)
// 3 - Axe Color
static Gfx* sIronKnuckleArmorType[3][3] = {
    { gIronKnuckleTanArmorMaterialDL, gIronKnuckleBronzeArmorMaterialDL, gIronKnuckleSilverArmorMaterialDL },
    { gIronKnuckleBlackArmorMaterialDL, gIronKnuckleBrownArmorMaterialDL, gIronKnuckleBrownArmorMaterialDL },
    { gIronKnuckleWhiteArmorMaterialDL, gIronKnuckleGoldArmorMaterialDL, gIronKnuckleGoldArmorMaterialDL },
};

ActorProfile En_Ik_Profile = {
    /**/ ACTOR_EN_IK,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_IK,
    /**/ sizeof(EnIk),
    /**/ EnIk_Init,
    /**/ EnIk_Destroy,
    /**/ EnIk_Update,
    /**/ EnIk_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 25, 80, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x04, 0x40 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ DMG_EFF_NONE,
    /* 0x4 */ DMG_EFF_FIRE = 0x2,
    /* 0x0 */ DMG_EFF_ICE,
    /* 0x4 */ DMG_EFF_LIGHT_SPARKS,
    /* 0xF */ DMG_EFF_IMMUNE = 0xF
} EnIkDmgEff;

static DamageTable sDamageTableArmor = {
    /* Deku Nut       */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Horse trample  */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Normal arrow   */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(2, DMG_EFF_NONE),
    /* Sword          */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Goron pound    */ DMG_ENTRY(3, DMG_EFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Ice arrow      */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Light arrow    */ DMG_ENTRY(1, DMG_EFF_LIGHT_SPARKS),
    /* Goron spikes   */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Deku spin      */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Deku bubble    */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Deku launch    */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Normal shield  */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Zora punch     */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Spin attack    */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, DMG_EFF_NONE),
};

static DamageTable sDamageTableNoArmor = {
    /* Deku Nut       */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, DMG_EFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(2, DMG_EFF_NONE),
    /* Sword          */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Goron pound    */ DMG_ENTRY(3, DMG_EFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, DMG_EFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, DMG_EFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, DMG_EFF_LIGHT_SPARKS),
    /* Goron spikes   */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Normal shield  */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, DMG_EFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, DMG_EFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, DMG_EFF_NONE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 18, 25, 80, MASS_HEAVY };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2916, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 12, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_STOP),
};

static EffectBlureInit2 sBlureInit = {
    0,
    EFFECT_BLURE_ELEMENT_FLAG_8,
    0,
    { 255, 255, 150, 200 },
    { 255, 255, 255, 64 },
    { 255, 255, 150, 0 },
    { 255, 255, 255, 0 },
    8,
    0,
    EFF_BLURE_DRAW_MODE_SMOOTH,
    0,
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
};

void EnIk_Init(Actor* thisx, PlayState* play) {
    static s32 sDisplayListDesegmented = false;
    s32 i;
    EnIk* this = (EnIk*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gIronKnuckleSkel, &gIronKnuckleWalkAnim, this->jointTable,
                       this->morphTable, IRON_KNUCKLE_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
    Collider_InitAndSetTris(play, &this->colliderTris, &this->actor, &sTrisInit, this->shieldColliderElements);
    Collider_InitAndSetQuad(play, &this->colliderQuad, &this->actor, &sQuadInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTableArmor, &sColChkInfoInit);
    this->actor.params = IK_GET_ARMOR_TYPE(&this->actor);
    this->actor.params--;

    Effect_Add(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &sBlureInit);
    if (!sDisplayListDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sIronKnuckleArmorType); i++) {
            sIronKnuckleArmorType[i][0] = Lib_SegmentedToVirtual(sIronKnuckleArmorType[i][0]);
            sIronKnuckleArmorType[i][1] = Lib_SegmentedToVirtual(sIronKnuckleArmorType[i][1]);
            sIronKnuckleArmorType[i][2] = Lib_SegmentedToVirtual(sIronKnuckleArmorType[i][2]);
        }
        sDisplayListDesegmented = true;
    }
    EnIk_SetupIdle(this);
}

void EnIk_Destroy(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    Collider_DestroyTris(play, &this->colliderTris);
    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroyQuad(play, &this->colliderQuad);
    Effect_Destroy(play, this->effectIndex);
}

void EnIk_Freeze(EnIk* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.65f;
    this->drawDmgEffFrozenSteamScale = 97.5f * 0.01f;
    this->drawDmgEffAlpha = 1.0f;
    this->timer = 80;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void EnIk_Thaw(EnIk* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, IRON_KNUCKLE_BODYPART_MAX, 2, 0.3f, 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }
}

void EnIk_HitArmor(EnIk* this, PlayState* play) {
    this->drawDmgEffAlpha = 4.0f;
    this->drawDmgEffScale = 0.65f;
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->colliderCylinder.elem.acDmgInfo.hitPos.x,
                this->colliderCylinder.elem.acDmgInfo.hitPos.y, this->colliderCylinder.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
}

s32 EnIk_IsChangingAction(EnIk* this, PlayState* play) {
    if ((this->drawArmorFlags) && (this->actionFunc != EnIk_Block) &&
        func_800BE184(play, &this->actor, 100.0f, 0x2710, 0x4000, this->actor.shape.rot.y)) {
        EnIk_SetupBlock(this);
        return true;
    }
    return false;
}

s32 EnIk_ChooseAttack(EnIk* this) {
    s32 absYawDiff;
    s32 detectionThreshold; // angle threshold an Iron Knuckle will use to trigger a horizontal attack

    if ((this->actor.xzDistToPlayer < 100.0f) && (fabsf(this->actor.playerHeightRel) < 150.0f)) {

        detectionThreshold = (this->drawArmorFlags == 0) ? 0xAAA : 0x3FFC;
        absYawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y));

        if (detectionThreshold >= absYawDiff) {
            if (Rand_ZeroOne() < 0.5f) {
                EnIk_SetupVerticalAttack(this);
                return true;
            }
            EnIk_SetupHorizontalDoubleAttack(this);
            return true;
        } else if ((this->drawArmorFlags) || ((absYawDiff > 0x4000) && (Rand_ZeroOne() < 0.1f))) {
            EnIk_SetupSingleHorizontalAttack(this);
            return true;
        }
    }
    return false;
}

void EnIk_CheckActions(EnIk* this, PlayState* play) {
    if (!EnIk_IsChangingAction(this, play) && !EnIk_ChooseAttack(this)) {
        if (this->drawArmorFlags != 0) {
            EnIk_SetupRun(this);
        } else {
            EnIk_SetupWalk(this);
        }
    }
}

void EnIk_SetupIdle(EnIk* this) {
    f32 endFrame = Animation_GetLastFrame(&gIronKnuckleHorizontalAttackAnim);

    if (this->drawArmorFlags != 0) {
        this->timer = 10;
    } else {
        this->timer = 0;
    }
    Animation_Change(&this->skelAnime, &gIronKnuckleEndHorizontalAttackAnim, 1.0f, endFrame, endFrame, ANIMMODE_ONCE,
                     this->timer);
    this->actionFunc = EnIk_Idle;
    this->actor.speed = 0.0f;
}

void EnIk_Idle(EnIk* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        SkelAnime_Update(&this->skelAnime);
        if (this->timer == 0) {
            EnIk_CheckActions(this, play);
        }
    } else if (this->colliderCylinder.base.acFlags & AC_HIT) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_ARMOR_HIT);
        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
        this->actor.hintId = TATL_HINT_ID_IRON_KNUCKLE;
        this->colliderCylinder.base.acFlags &= ~AC_HIT;
        this->invincibilityFrames = 12;
        EnIk_SetupWalk(this);
    }
}

void EnIk_SetupWalk(EnIk* this) {
    Animation_MorphToLoop(&this->skelAnime, &gIronKnuckleWalkAnim, -4.0f);
    this->actor.speed = 0.9f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnIk_WalkTowardsPlayer;
}

void EnIk_WalkTowardsPlayer(EnIk* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 16.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_WALK);
    }
    yawDiff = this->actor.wallYaw - this->actor.shape.rot.y;
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (ABS_ALT(yawDiff) >= 0x4000)) {
        yawDiff = (this->actor.yawTowardsPlayer > 0) ? this->actor.wallYaw - 0x4000 : this->actor.wallYaw + 0x4000;
        Math_ScaledStepToS(&this->actor.shape.rot.y, yawDiff, 0x320);
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x320);
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (!EnIk_IsChangingAction(this, play)) {
        EnIk_ChooseAttack(this);
    }
}

void EnIk_SetupRun(EnIk* this) {
    Animation_MorphToLoop(&this->skelAnime, &gIronKnuckleRunAnim, -4.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_TWINROBA_SHOOT_VOICE);
    this->actor.speed = 2.5f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnIk_RunTowardsPlayer;
}

void EnIk_RunTowardsPlayer(EnIk* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 9.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_WALK);
    }
    yawDiff = this->actor.wallYaw - this->actor.shape.rot.y;
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (ABS_ALT(yawDiff) >= 0x4000)) {
        yawDiff = (this->actor.yawTowardsPlayer > 0) ? this->actor.wallYaw - 0x4000 : this->actor.wallYaw + 0x4000;
        Math_ScaledStepToS(&this->actor.shape.rot.y, yawDiff, 0x4B0);
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x4B0);
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (!EnIk_IsChangingAction(this, play)) {
        EnIk_ChooseAttack(this);
    }
}

void EnIk_SetupVerticalAttack(EnIk* this) {
    s32 pad;
    f32 playbackSpeed;

    this->actor.speed = 0.0f;
    if (this->drawArmorFlags != 0) {
        playbackSpeed = 1.5f;
    } else {
        playbackSpeed = 1.2f;
    }
    Animation_Change(&this->skelAnime, &gIronKnuckleVerticalAttackAnim, playbackSpeed, 0.0f,
                     Animation_GetLastFrame(&gIronKnuckleVerticalAttackAnim), ANIMMODE_ONCE_INTERP, -4.0f);
    this->timer = 0;
    this->blurEffectSpawnLock = -1;
    this->actionFunc = EnIk_VerticalAttack;
}

void EnIk_VerticalAttack(EnIk* this, PlayState* play) {
    Vec3f particlePos;

    this->timer++;
    if (!(this->skelAnime.curFrame < 7.0f) || !EnIk_IsChangingAction(this, play)) {
        if (Animation_OnFrame(&this->skelAnime, 15.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_SWING_AXE);

        } else if (Animation_OnFrame(&this->skelAnime, 21.0f)) {
            particlePos.x = this->actor.world.pos.x + (Math_SinS(this->actor.shape.rot.y + 0x6A4) * 70.0f);
            particlePos.z = this->actor.world.pos.z + (Math_CosS(this->actor.shape.rot.y + 0x6A4) * 70.0f);
            particlePos.y = this->actor.world.pos.y;
            Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_HIT_GND);
            Camera_AddQuake(GET_ACTIVE_CAM(play), 2, 25, 5);
            Rumble_Request(this->actor.xyzDistToPlayerSq, 180, 20, 100);
            CollisionCheck_SpawnShieldParticles(play, &particlePos);
        }

        if ((this->skelAnime.curFrame > 13.0f) && (this->skelAnime.curFrame < 23.0f)) {
            this->colliderQuad.base.atFlags |= AT_ON;
            if (this->drawArmorFlags != 0) {
                this->actor.speed = Math_SinF((this->skelAnime.curFrame - 13.0f) * (M_PIf / 20)) * 10.0f;
            }
        } else {
            this->colliderQuad.base.atFlags &= ~AT_ON;
            this->actor.speed = 0.0f;
        }

        if ((this->drawArmorFlags) && (this->skelAnime.curFrame < 13.0f)) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x5DC);
            this->actor.world.rot.y = this->actor.shape.rot.y;
        }

        if (SkelAnime_Update(&this->skelAnime)) {
            EnIk_SetupTakeOutAxe(this);
        }
    }
}

void EnIk_SetupTakeOutAxe(EnIk* this) {
    Animation_PlayOnce(&this->skelAnime, &gIronKnuckleEndVerticalAttackAnim);
    Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_PULLOUT);
    this->actionFunc = EnIk_TakeOutAxe;
}

void EnIk_TakeOutAxe(EnIk* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.animation == &gIronKnuckleRecoverVerticalAttackAnim) {
            EnIk_CheckActions(this, play);
        } else {
            Animation_Change(
                &this->skelAnime, &gIronKnuckleRecoverVerticalAttackAnim, (this->drawArmorFlags) ? 1.5f : 1.0f, 0.0f,
                Animation_GetLastFrame(&gIronKnuckleRecoverVerticalAttackAnim), ANIMMODE_ONCE_INTERP, 0.0f);
        }
    }
}

void EnIk_SetupHorizontalDoubleAttack(EnIk* this) {
    this->actor.speed = 0.0f;
    Animation_Change(&this->skelAnime, &gIronKnuckleHorizontalAttackAnim, (this->drawArmorFlags) ? 1.3f : 1.0f, 0.0f,
                     Animation_GetLastFrame(&gIronKnuckleHorizontalAttackAnim), ANIMMODE_ONCE_INTERP,
                     (this->drawArmorFlags) ? 4.0f : 10.0f);
    this->timer = 0;
    this->blurEffectSpawnLock = -1;
    this->actionFunc = EnIk_HorizontalDoubleAttack;
}

void EnIk_HorizontalDoubleAttack(EnIk* this, PlayState* play) {
    f32 phi_f2;

    this->timer++;
    if (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 13.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_SWING_AXE);
    }
    if (((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 9.0f)) ||
        ((this->skelAnime.curFrame > 12.0f) && (this->skelAnime.curFrame < 20.0f))) {
        if (this->drawArmorFlags != 0) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x5DC);
            this->actor.world.rot.y = this->actor.shape.rot.y;
            if (this->skelAnime.curFrame > 12.0f) {
                phi_f2 = this->skelAnime.curFrame - 12.0f;
            } else {
                phi_f2 = this->skelAnime.curFrame - 1.0f;
            }
            this->actor.speed = Math_SinF((M_PIf / 8) * phi_f2) * 4.5f;
        }
        this->colliderQuad.base.atFlags |= AT_ON;
    } else {
        this->colliderQuad.base.atFlags &= ~AT_ON;
        this->actor.speed = 0.0f;
    }
    if (SkelAnime_Update(&this->skelAnime)) {
        EnIk_SetupEndHorizontalAttack(this);
    }
}

void EnIk_SetupSingleHorizontalAttack(EnIk* this) {
    f32 playSpeed;

    this->actor.speed = 0.0f;
    if (this->drawArmorFlags != 0) {
        this->actor.world.rot.z = 0x1000;
        playSpeed = 1.3f;
    } else {
        this->actor.world.rot.z = 0xB00;
        playSpeed = 1.0f;
    }
    Animation_Change(&this->skelAnime, &gIronKnuckleHorizontalAttackAnim, playSpeed, 12.0f,
                     Animation_GetLastFrame(&gIronKnuckleHorizontalAttackAnim), ANIMMODE_ONCE_INTERP, 5.0f);
    this->timer = 0;
    this->blurEffectSpawnLock = -1;
    this->actionFunc = EnIk_SingleHorizontalAttack;
}

void EnIk_SingleHorizontalAttack(EnIk* this, PlayState* play) {
    this->timer++;
    if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_SWING_AXE);
    }
    if ((this->skelAnime.curFrame > 12.0f) && (this->skelAnime.curFrame < 20.0f)) {
        this->colliderQuad.base.atFlags |= AT_ON;
    } else {
        this->colliderQuad.base.atFlags &= ~AT_ON;
    }
    this->actor.shape.rot.y += this->actor.world.rot.z;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (SkelAnime_Update(&this->skelAnime)) {
        EnIk_SetupEndHorizontalAttack(this);
    }
}

void EnIk_SetupEndHorizontalAttack(EnIk* this) {
    Animation_Change(&this->skelAnime, &gIronKnuckleEndHorizontalAttackAnim, (this->drawArmorFlags) ? 2.0f : 1.0f, 0.0f,
                     Animation_GetLastFrame(&gIronKnuckleHorizontalAttackAnim), ANIMMODE_ONCE_INTERP, -4.0f);
    this->actionFunc = EnIk_EndHorizontalAttack;
}

void EnIk_EndHorizontalAttack(EnIk* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnIk_CheckActions(this, play);
    }
}

void EnIk_SetupBlock(EnIk* this) {
    this->actor.speed = 0.0f;
    Animation_Change(&this->skelAnime, &gIronKnuckleBlockAnim, 2.0f, 0.0f,
                     Animation_GetLastFrame(&gIronKnuckleBlockAnim), ANIMMODE_ONCE, -2.0f);
    this->timer = 20;
    this->actionFunc = EnIk_Block;
}

void EnIk_Block(EnIk* this, PlayState* play) {
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderTris.base);
    if (SkelAnime_Update(&this->skelAnime)) {
        this->timer--;
        if (this->timer == 0) {
            EnIk_CheckActions(this, play);
        }
    }
}

void EnIk_SetupReactToAttack(EnIk* this, s32 arg1) {
    s16 temp_v0;

    if (arg1) {
        func_800BE504(&this->actor, &this->colliderCylinder);
    }
    this->actor.speed = 6.0f;
    temp_v0 = (this->actor.world.rot.y - this->actor.shape.rot.y) + 0x8000;
    if (ABS_ALT(temp_v0) <= 0x4000) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gIronKnuckleFrontHitAnim, -4.0f);
    } else {
        Animation_MorphToPlayOnce(&this->skelAnime, &gIronKnuckleBackHitAnim, -4.0f);
    }
    this->actionFunc = EnIk_ReactToAttack;
}

void EnIk_ReactToAttack(EnIk* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &Play_GetCamera(play, this->subCamId)->eye);
    }
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->subCamId != SUB_CAM_ID_DONE) {
            CutsceneManager_Stop(this->actor.csId);
            this->subCamId = SUB_CAM_ID_DONE;
            EnIk_SetupIdle(this);
        } else {
            EnIk_CheckActions(this, play);
        }
    } else {
        this->actor.colorFilterTimer = 12;
        this->invincibilityFrames = 12;
    }
}

void EnIk_SetupDie(EnIk* this) {
    this->actor.speed = 0.0f;
    Animation_MorphToPlayOnce(&this->skelAnime, &gIronKnuckleDeathAnim, -4.0f);
    this->timer = 24;
    Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_DEAD);
    this->actionFunc = EnIk_Die;
}

void EnIk_Die(EnIk* this, PlayState* play) {
    static Vec3f sEffectVelAndAccel = { 0.0f, 0.5f, 0.0f };
    s32 i;
    Vec3f effectPos;

    this->invincibilityFrames = 12;
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer == 24) {
            SoundSource_PlaySfxEachFrameAtFixedWorldPos(play, &this->actor.world.pos, 35,
                                                        NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        }

        if (this->timer != 0) {
            this->timer--;
            for (i = 6 - (this->timer >> 2); i >= 0; i--) {
                effectPos.x = Rand_CenteredFloat(80.0f) + this->actor.world.pos.x;
                effectPos.z = Rand_CenteredFloat(80.0f) + this->actor.world.pos.z;
                effectPos.y = Rand_CenteredFloat(50.0f) + (this->actor.world.pos.y + 20.0f);
                func_800B3030(play, &effectPos, &sEffectVelAndAccel, &sEffectVelAndAccel, 100, 0, 2);
            }
            if (this->timer == 0) {
                Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xB0);
                CutsceneManager_Stop(this->actor.csId);
                Actor_Kill(&this->actor);
            }
        }
    } else if (Animation_OnFrame(&this->skelAnime, 23.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_WALK);
    }
}

void EnIk_SetupFrozen(EnIk* this) {
    this->invincibilityFrames = 0;
    this->actionFunc = EnIk_Frozen;
    this->actor.speed = 0.0f;
}

void EnIk_Frozen(EnIk* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        EnIk_Thaw(this, play);
        if (this->actor.colChkInfo.health == 0) {
            EnIk_SetupCutscene(this);
        } else {
            EnIk_CheckActions(this, play);
        }
    }
}

void EnIk_SetupCutscene(EnIk* this) {
    CutsceneManager_Queue(this->actor.csId);
    this->actor.speed = 0.0f;
    if (this->actor.colChkInfo.health != 0) {
        func_800BE504(&this->actor, &this->colliderCylinder);
    }
    this->actionFunc = EnIk_PlayCutscene;
}

void EnIk_PlayCutscene(EnIk* this, PlayState* play) {
    Vec3f subCamEye;

    this->invincibilityFrames = 12;
    if (CutsceneManager_IsNext(this->actor.csId)) {
        if (this->actor.csId != CS_ID_NONE) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->actor.csId, &this->actor);
            this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
            subCamEye.x = this->actor.focus.pos.x + (Math_SinS(this->actor.shape.rot.y - 0x2000) * 120.0f);
            subCamEye.y = this->actor.focus.pos.y + 20.0f;
            subCamEye.z = this->actor.focus.pos.z + (Math_CosS(this->actor.shape.rot.y - 0x2000) * 120.0f);
            Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCamEye);
        }
        if (this->actor.colChkInfo.health != 0) {
            EnIk_SetupReactToAttack(this, false);
        } else {
            EnIk_SetupDie(this);
        }
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnIk_UpdateDamage(EnIk* this, PlayState* play) {
    s32 pad;

    if (this->actionFunc == EnIk_Idle) {
        return;
    }
    if (this->colliderTris.base.acFlags & AC_BOUNCED) {
        f32 endFrame = Animation_GetLastFrame(&gIronKnuckleBlockAnim) - 2.0f;

        if (this->skelAnime.curFrame < endFrame) {
            this->skelAnime.curFrame = endFrame;
        }
        this->colliderTris.base.acFlags &= ~AC_BOUNCED;
        this->colliderCylinder.base.acFlags &= ~AC_HIT;
    } else if (this->colliderCylinder.base.acFlags & AC_HIT) {
        s32 isArmorBroken = false;

        Actor_SetDropFlag(&this->actor, &this->colliderCylinder.elem);
        this->colliderCylinder.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect != DMG_EFF_IMMUNE) &&
            ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
             !(this->colliderCylinder.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3))) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);
            this->invincibilityFrames = 12;
            EnIk_Thaw(this, play);
            this->colliderQuad.base.atFlags &= ~AT_ON;
            if (Actor_ApplyDamage(&this->actor) == 0) {
                if (this->drawArmorFlags == 0) {
                    this->actor.colChkInfo.health = 9;
                    isArmorBroken = true;
                } else {
                    Enemy_StartFinishingBlow(play, &this->actor);
                    Audio_RestorePrevBgm();
                }
            }
            if (isArmorBroken == true) {
                this->drawArmorFlags = 1;
                this->colliderCylinder.base.colMaterial = COL_MATERIAL_HIT3;
                this->actor.colChkInfo.damageTable = &sDamageTableNoArmor;
                Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_ARMOR_OFF_DEMO);
                EnIk_SetupCutscene(this);
            } else if (this->drawArmorFlags != 0) {
                if (this->actor.colChkInfo.damageEffect == DMG_EFF_ICE) {
                    EnIk_Freeze(this);
                    EnIk_SetupFrozen(this);
                    if (this->actor.colChkInfo.health == 0) {
                        this->timer = 3;
                        this->invincibilityFrames = 12;
                    }
                } else {
                    if (this->actor.colChkInfo.damageEffect == DMG_EFF_FIRE) {
                        this->drawDmgEffAlpha = 4.0f;
                        this->drawDmgEffScale = 0.65f;
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    } else if (this->actor.colChkInfo.damageEffect == DMG_EFF_LIGHT_SPARKS) {
                        EnIk_HitArmor(this, play);
                    }
                    Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_DAMAGE);
                    if (this->actor.colChkInfo.health != 0) {
                        EnIk_SetupReactToAttack(this, true);
                    } else {
                        EnIk_SetupCutscene(this);
                    }
                }
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_ARMOR_HIT);
                Actor_PlaySfx(&this->actor, NA_SE_EN_IRONNACK_DAMAGE);
                if (this->actor.colChkInfo.damageEffect == 4) {
                    EnIk_HitArmor(this, play);
                }
            }
        }
    }
}

void EnIk_UpdateArmor(EnIk* this, PlayState* play) {
    s32 pad;
    s32 i;
    IronKnuckleEffect* ikEffect;
    Vec3f effectPos;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        ikEffect = &this->effects[i];

        if (ikEffect->enabled) {
            Math_Vec3f_Sum(&ikEffect->pos, &ikEffect->velocity, &ikEffect->pos);
            ikEffect->velocity.y += -1.5f;
            if (ikEffect->pos.y < this->actor.floorHeight) {
                ikEffect->enabled = false;
                ikEffect->pos.y = this->actor.floorHeight;

                for (i = 0; i < 4; i++) {
                    effectPos.x = Rand_CenteredFloat(20.0f) + ikEffect->pos.x;
                    effectPos.y = Rand_ZeroFloat(20.0f) + ikEffect->pos.y;
                    effectPos.z = Rand_CenteredFloat(20.0f) + ikEffect->pos.z;
                    func_800B3030(play, &effectPos, &gZeroVec3f, &gZeroVec3f, 40, 7, 2);
                }

                SoundSource_PlaySfxAtFixedWorldPos(play, &ikEffect->pos, 11, NA_SE_EN_EXTINCT);
            }
        }
    }
}

void EnIk_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnIk* this = (EnIk*)thisx;

    if (this->actionFunc != EnIk_PlayCutscene) {
        EnIk_UpdateDamage(this, play);
    } else {
        this->colliderTris.base.acFlags &= ~AC_BOUNCED;
        this->colliderCylinder.base.acFlags &= ~AC_HIT;
    }
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    this->actor.focus.rot.y = this->actor.shape.rot.y;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
    if (this->invincibilityFrames == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);
    } else {
        this->invincibilityFrames--;
    }
    if (this->colliderQuad.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderQuad.base);
    }
    if (this->actionFunc == EnIk_Block) {
        f32 cos;
        f32 sin;
        Vec3f sp5C;
        Vec3f sp50;
        Vec3f sp44;

        cos = Math_CosS(this->actor.shape.rot.y);
        sin = Math_SinS(this->actor.shape.rot.y);

        sp44.x = (this->actor.world.pos.x - (30.0f * cos)) + (20.0f * sin);
        sp44.y = this->actor.world.pos.y;
        sp44.z = this->actor.world.pos.z + (30.0f * sin) + (20.0f * cos);

        sp50.x = this->actor.world.pos.x + (30.0f * cos) + (20.0f * sin);
        sp50.y = this->actor.world.pos.y + 80.0f;
        sp50.z = (this->actor.world.pos.z - (30.0f * sin)) + (20.0f * cos);

        sp5C.x = sp44.x;
        sp5C.y = sp50.y;
        sp5C.z = sp44.z;

        Collider_SetTrisVertices(&this->colliderTris, 0, &sp44, &sp50, &sp5C);
        sp5C.x = sp50.x;
        sp5C.y = sp44.y;
        sp5C.z = sp50.z;
        Collider_SetTrisVertices(&this->colliderTris, 1, &sp44, &sp5C, &sp50);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderTris.base);
    }
    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.325f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.65f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.65f, 0.01625f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
    EnIk_UpdateArmor(this, play);
}

static s8 sLimbToArmorBodyParts[IRON_KNUCKLE_LIMB_MAX] = {
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_NONE
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_ROOT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_TASSET_CENTER
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_TASSET_RIGHT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_RIGHT_LEG
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_RIGHT_FOOT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_TASSET_LEFT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_LEFT_LEG
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_LEFT_FOOT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_UPPER_BODY_ROOT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_UNK_ROOT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_HEAD_ROOT
    IRON_KNUCKLE_ARMOR_BODYPART_HELMET,               // IRON_KNUCKLE_LIMB_HELMET_ARMOR
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_HEAD
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_AXE_ROOT
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_AXE
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_RIGHT_HAND
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_LEFT_FOREARM
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_LEFT_HAND
    IRON_KNUCKLE_ARMOR_BODYPART_UPPER_LEFT_PAULDRON,  // IRON_KNUCKLE_LIMB_UPPER_LEFT_PAULDRON
    IRON_KNUCKLE_ARMOR_BODYPART_LOWER_LEFT_PAULDRON,  // IRON_KNUCKLE_LIMB_LOWER_LEFT_PAULDRON
    IRON_KNUCKLE_ARMOR_BODYPART_UPPER_RIGHT_PAULDRON, // IRON_KNUCKLE_LIMB_UPPER_RIGHT_PAULDRON
    IRON_KNUCKLE_ARMOR_BODYPART_LOWER_RIGHT_PAULDRON, // IRON_KNUCKLE_LIMB_LOWER_RIGHT_PAULDRON
    IRON_KNUCKLE_ARMOR_BODYPART_CHEST_FRONT,          // IRON_KNUCKLE_LIMB_CHEST_ARMOR_FRONT
    IRON_KNUCKLE_ARMOR_BODYPART_CHEST_BACK,           // IRON_KNUCKLE_LIMB_CHEST_ARMOR_BACK
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_TORSO
    BODYPART_NONE,                                    // IRON_KNUCKLE_LIMB_WAIST
};

s32 EnIk_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnIk* this = (EnIk*)thisx;

    if (this->drawArmorFlags != 0) {
        if (sLimbToArmorBodyParts[limbIndex] >= IRON_KNUCKLE_ARMOR_BODYPART_CHEST_FRONT) {
            *dList = NULL;
        }
    }
    return false;
}

static Vec3f D_8092C1C8 = { 2000.0f, -200.0f, -5200.0f };
static Vec3f D_8092C1D4 = { 300.0f, -200.0f, 0.0f };

static s8 sLimbToBodyParts[IRON_KNUCKLE_LIMB_MAX] = {
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_NONE
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_ROOT
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_TASSET_CENTER
    IRON_KNUCKLE_BODYPART_TASSET_RIGHT,         // IRON_KNUCKLE_LIMB_TASSET_RIGHT
    IRON_KNUCKLE_BODYPART_RIGHT_LEG,            // IRON_KNUCKLE_LIMB_RIGHT_LEG
    IRON_KNUCKLE_BODYPART_RIGHT_FOOT,           // IRON_KNUCKLE_LIMB_RIGHT_FOOT
    IRON_KNUCKLE_BODYPART_TASSET_LEFT,          // IRON_KNUCKLE_LIMB_TASSET_LEFT
    IRON_KNUCKLE_BODYPART_LEFT_LEG,             // IRON_KNUCKLE_LIMB_LEFT_LEG
    IRON_KNUCKLE_BODYPART_LEFT_FOOT,            // IRON_KNUCKLE_LIMB_LEFT_FOOT
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_UPPER_BODY_ROOT
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_UNK_ROOT
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_HEAD_ROOT
    IRON_KNUCKLE_BODYPART_HELMET_ARMOR,         // IRON_KNUCKLE_LIMB_HELMET_ARMOR
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_HEAD
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_RIGHT_UPPER_ARM
    IRON_KNUCKLE_BODYPART_RIGHT_FOREARM,        // IRON_KNUCKLE_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_AXE_ROOT
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_AXE
    IRON_KNUCKLE_BODYPART_RIGHT_HAND,           // IRON_KNUCKLE_LIMB_RIGHT_HAND
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_LEFT_UPPER_ARM
    IRON_KNUCKLE_BODYPART_LEFT_FOREARM,         // IRON_KNUCKLE_LIMB_LEFT_FOREARM
    IRON_KNUCKLE_BODYPART_LEFT_HAND,            // IRON_KNUCKLE_LIMB_LEFT_HAND
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_UPPER_LEFT_PAULDRON
    IRON_KNUCKLE_BODYPART_LOWER_LEFT_PAULDRON,  // IRON_KNUCKLE_LIMB_LOWER_LEFT_PAULDRON
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_UPPER_RIGHT_PAULDRON
    IRON_KNUCKLE_BODYPART_LOWER_RIGHT_PAULDRON, // IRON_KNUCKLE_LIMB_LOWER_RIGHT_PAULDRON
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_CHEST_ARMOR_FRONT
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_CHEST_ARMOR_BACK
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_TORSO
    BODYPART_NONE,                              // IRON_KNUCKLE_LIMB_WAIST
};

void EnIk_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnIk* this = (EnIk*)thisx;
    s32 armorBodyPart = sLimbToArmorBodyParts[limbIndex];
    Gfx* xlu;
    IronKnuckleEffect* ikEffect;
    s16 sp76;
    Vec3f vtxC;
    Vec3f vtxD;
    Vec3f vtxB;
    Vec3f vtxA;
    MtxF* mf;

    if (this->drawArmorFlags == 0x1) {
        if (armorBodyPart >= IRON_KNUCKLE_ARMOR_BODYPART_CHEST_FRONT) {
            ikEffect = &this->effects[armorBodyPart];
            mf = Matrix_GetCurrent();
            ikEffect->pos.x = mf->mf[3][0];
            ikEffect->pos.y = mf->mf[3][1];
            ikEffect->pos.z = mf->mf[3][2];
            Matrix_MtxFToYXZRot(mf, &ikEffect->rot, false);
            ikEffect->enabled = true;
            sp76 =
                sIronKnuckleArmorMarkings[armorBodyPart].unk04 + (((s32)Rand_Next() >> 0x13) + this->actor.shape.rot.y);
            ikEffect->velocity.x = Math_SinS(sp76) * 5.0f;
            ikEffect->velocity.y = 6.0f;
            ikEffect->velocity.z = Math_CosS(sp76) * 5.0f;
            ikEffect->dList = *dList;
        }
        if (limbIndex == IRON_KNUCKLE_LIMB_WAIST) {
            this->drawArmorFlags |= 0x2;
        }
    }

    if (limbIndex == IRON_KNUCKLE_LIMB_ROOT) {
        Matrix_MultZero(&this->actor.focus.pos);
        this->colliderCylinder.dim.pos.x = this->actor.focus.pos.x;
        this->colliderCylinder.dim.pos.y = this->actor.world.pos.y;
        this->colliderCylinder.dim.pos.z = this->actor.focus.pos.z;
    }

    if ((limbIndex == IRON_KNUCKLE_LIMB_AXE) && (this->blurEffectSpawnLock != this->timer) &&
        ((this->actionFunc == EnIk_VerticalAttack) || (this->actionFunc == EnIk_HorizontalDoubleAttack) ||
         (this->actionFunc == EnIk_SingleHorizontalAttack))) {
        Math_Vec3f_Copy(&vtxC, &this->colliderQuad.dim.quad[0]);
        Math_Vec3f_Copy(&vtxD, &this->colliderQuad.dim.quad[1]);
        Matrix_MultVec3f(&D_8092C1C8, &vtxB);
        Matrix_MultVec3f(&D_8092C1D4, &vtxA);
        Collider_SetQuadVertices(&this->colliderQuad, &vtxA, &vtxB, &vtxC, &vtxD);
        if (this->colliderQuad.base.atFlags & AT_ON) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->effectIndex), &vtxB, &vtxA);
        }
        this->blurEffectSpawnLock = this->timer;
    }

    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if ((armorBodyPart == IRON_KNUCKLE_ARMOR_BODYPART_HELMET) ||
        ((armorBodyPart != BODYPART_NONE) && (this->drawArmorFlags == 0) &&
         (sIronKnuckleArmorMarkings[armorBodyPart].unk00 != NULL))) {
        OPEN_DISPS(play->state.gfxCtx);

        xlu = POLY_XLU_DISP;

        MATRIX_FINALIZE_AND_LOAD(&xlu[0], play->state.gfxCtx);
        gSPDisplayList(&xlu[1], sIronKnuckleArmorMarkings[armorBodyPart].unk00);
        POLY_XLU_DISP = &xlu[2];

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnIk_UpdateArmorDraw(EnIk* this, PlayState* play) {
    Gfx* gfxOpa;
    Gfx* gfxXlu;
    s32 sp54;
    IronKnuckleEffect* ikEffect;
    s32 i;

    if (this->drawArmorFlags == (0x1 | 0x2)) {
        sp54 = 0;

        OPEN_DISPS(play->state.gfxCtx);

        gfxOpa = POLY_OPA_DISP;
        gfxXlu = POLY_XLU_DISP;

        for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
            ikEffect = &this->effects[i];
            if (ikEffect->enabled) {
                Matrix_SetTranslateRotateYXZ(ikEffect->pos.x, ikEffect->pos.y, ikEffect->pos.z, &ikEffect->rot);
                Matrix_Scale(0.012f, 0.012f, 0.012f, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(gfxOpa++, play->state.gfxCtx);
                gSPDisplayList(gfxOpa++, ikEffect->dList);

                if (sIronKnuckleArmorMarkings[i].unk00 != NULL) {
                    MATRIX_FINALIZE_AND_LOAD(gfxXlu++, play->state.gfxCtx);
                    gSPDisplayList(gfxXlu++, sIronKnuckleArmorMarkings[i].unk00);
                }
            } else {
                sp54++;
            }
        }

        if (sp54 == ARRAY_COUNT(this->effects)) {
            this->drawArmorFlags = (0x1 | 0x4);
        }

        POLY_XLU_DISP = gfxXlu;
        POLY_OPA_DISP = gfxOpa;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnIk_Draw(Actor* thisx, PlayState* play) {
    static Vec3f sScale = { 0.53f, 0.53f, 0.53f };
    EnIk* this = (EnIk*)thisx;
    Gfx* gfx;
    Gfx** gfxArmorType;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    func_800B8050(&this->actor, play, 0);
    func_800B8118(&this->actor, play, 0);
    gfx = POLY_XLU_DISP;
    gSPDisplayList(&gfx[0], gSetupDLs[SETUPDL_25]);
    POLY_XLU_DISP = &gfx[1];
    gfx = POLY_OPA_DISP;
    gSPDisplayList(&gfx[0], gSetupDLs[SETUPDL_25]);
    gfxArmorType = sIronKnuckleArmorType[this->actor.params];
    gSPSegment(&gfx[1], 0x08, gfxArmorType[0]);
    gSPSegment(&gfx[2], 0x09, gfxArmorType[1]);
    gSPSegment(&gfx[3], 0x0A, gfxArmorType[2]);
    POLY_OPA_DISP = &gfx[4];

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnIk_OverrideLimbDraw, EnIk_PostLimbDraw, &this->actor);
    EnIk_UpdateArmorDraw(this, play);
    if (this->actor.colorFilterTimer != 0) {
        func_800AE5A0(play);
    }
    func_800BC620(&this->actor.focus.pos, &sScale, 255, play);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, IRON_KNUCKLE_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}

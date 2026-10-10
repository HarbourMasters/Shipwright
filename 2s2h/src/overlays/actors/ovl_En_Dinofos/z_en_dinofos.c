/*
 * File: z_en_dinofos.c
 * Overlay: ovl_En_Dinofos
 * Description: Dinolfos
 */

#include "z_en_dinofos.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnDinofos_Init(Actor* thisx, PlayState* play);
void EnDinofos_Destroy(Actor* thisx, PlayState* play);
void EnDinofos_Update(Actor* thisx, PlayState* play2);
void EnDinofos_Draw(Actor* thisx, PlayState* play);
void EnDinofos_Reset(void);

void EnDinofos_Idle(EnDinofos* this, PlayState* play);
void EnDinofos_PlayCutscene(EnDinofos* this, PlayState* play);
void EnDinofos_SetupCircleAroundPlayer(EnDinofos* this, PlayState* play);
void EnDinofos_SetupWalk(EnDinofos* this, PlayState* play);
void EnDinofos_Slash(EnDinofos* this, PlayState* play);
void EnDinofos_Land(EnDinofos* this, PlayState* play);
void EnDinofos_TurnToPlayer(EnDinofos* this, PlayState* play);
void EnDinofos_ChooseJump(EnDinofos* this, PlayState* play);
void EnDinofos_Die(EnDinofos* this, PlayState* play);
void EnDinofos_IntroCutsceneBeforeFall(EnDinofos* this, PlayState* play);
void EnDinofos_CircleAroundPlayer(EnDinofos* this, PlayState* play);
void EnDinofos_DodgeProjectile(EnDinofos* this, PlayState* play);
void EnDinofos_Walk(EnDinofos* this, PlayState* play);
void EnDinofos_StartBreatheFire(EnDinofos* this, PlayState* play);
void EnDinofos_IntroCutsceneFall(EnDinofos* this, PlayState* play);
void EnDinofos_SetupBreatheFire(EnDinofos* this, PlayState* play);
void EnDinofos_BreatheFire(EnDinofos* this, PlayState* play);
void EnDinofos_SetupEndBreatheFire(EnDinofos* this, PlayState* play);
void EnDinofos_EndBreatheFire(EnDinofos* this, PlayState* play);
void EnDinofos_IntroCutsceneLandAndBreatheFire(EnDinofos* this, PlayState* play);
void EnDinofos_IntroCutsceneYell(EnDinofos* this, PlayState* play);
void EnDinofos_JumpSlash(EnDinofos* this, PlayState* play);
void EnDinofos_Jump(EnDinofos* this, PlayState* play);
void EnDinofos_Recoil(EnDinofos* this, PlayState* play);
void EnDinofos_Stunned(EnDinofos* this, PlayState* play);
void EnDinofos_Damaged(EnDinofos* this, PlayState* play);
void EnDinofos_SetupSlash(EnDinofos* this);
void EnDinofos_SetupTurnToPlayer(EnDinofos* this);
void EnDinofos_SetupChooseJump(EnDinofos* this, s32 jumpType);
void EnDinofos_SetupDodgeProjectile(EnDinofos* this, s16 angleToProjectile);
void EnDinofos_SetupIdle(EnDinofos* this);
void EnDinofos_SetupPlayCutscene(EnDinofos* this);
void EnDinofos_SetupStartBreatheFire(EnDinofos* this);
void EnDinofos_SetupIntroCutsceneFall(EnDinofos* this);
void EnDinofos_SetupIntroCutsceneLandAndBreatheFire(EnDinofos* this);
void EnDinofos_SetupIntroCutsceneYell(EnDinofos* this);
void EnDinofos_SetupDie(EnDinofos* this);
void EnDinofos_SetupLand(EnDinofos* this);
void EnDinofos_SetupJump(EnDinofos* this);
void EnDinofos_SetupJumpSlash(EnDinofos* this);

ActorProfile En_Dinofos_Profile = {
    /**/ ACTOR_EN_DINOFOS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DINOFOS,
    /**/ sizeof(EnDinofos),
    /**/ EnDinofos_Init,
    /**/ EnDinofos_Destroy,
    /**/ EnDinofos_Update,
    /**/ EnDinofos_Draw,
    /**/ EnDinofos_Reset,
};

static ColliderJntSphElementInit sJntSphElementsInit[9] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { DINOLFOS_LIMB_LOWER_BODY, { { 200, 300, 0 }, 19 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { DINOLFOS_LIMB_UPPER_BODY, { { 200, 200, 0 }, 17 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { DINOLFOS_LIMB_HEAD, { { 600, 200, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { DINOLFOS_LIMB_NECK, { { 700, 100, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { DINOLFOS_LIMB_LEFT_UPPER_LEG, { { 1300, 100, 0 }, 12 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { DINOLFOS_LIMB_RIGHT_UPPER_LEG, { { 1300, 100, 0 }, 12 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x09, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { DINOLFOS_LIMB_MAX, { { 0, -10, 35 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x09, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { DINOLFOS_LIMB_MAX, { { 0, -10, 70 }, 28 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x09, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { DINOLFOS_LIMB_MAX, { { 0, -5, 110 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum EnDinofosDamageEffect {
    /* 0x0 */ DINOLFOS_DMGEFF_NONE,
    /* 0x1 */ DINOLFOS_DMGEFF_STUN,
    /* 0x2 */ DINOLFOS_DMGEFF_FIRE,
    /* 0x3 */ DINOLFOS_DMGEFF_ICE,
    /* 0x4 */ DINOLFOS_DMGEFF_LIGHT_SPARKS,
    /* 0x5 */ DINOLFOS_DMGEFF_ZORA_MAGIC,
    /* 0xF */ DINOLFOS_DMGEFF_IMMUNE = 0xF
} EnDinofosDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, DINOLFOS_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, DINOLFOS_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, DINOLFOS_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, DINOLFOS_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, DINOLFOS_DMGEFF_LIGHT_SPARKS),
    /* Goron spikes   */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, DINOLFOS_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, DINOLFOS_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, DINOLFOS_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, DINOLFOS_DMGEFF_ZORA_MAGIC),
    /* Normal shield  */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, DINOLFOS_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, DINOLFOS_DMGEFF_NONE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 4, 40, 100, 80 };

static TexturePtr sEyeTextures[] = {
    gDinolfosEyeOpenTex,
    gDinolfosEyeHalfTex,
    gDinolfosEyeClosedTex,
    gDinolfosEyeHalfTex,
};

static s16 sCsId = CS_ID_NONE;
static s32 sNumAlive = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_S8(hintId, TATL_HINT_ID_DINOLFOS, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_STOP),
};

void EnDinofos_Init(Actor* thisx, PlayState* play) {
    static s32 sTexturesDesegmented = false;
    static EffectBlureInit2 sBlureInit = {
        0,
        EFFECT_BLURE_ELEMENT_FLAG_8,
        0,
        { 255, 255, 255, 255 },
        { 255, 255, 255, 64 },
        { 255, 255, 255, 0 },
        { 255, 255, 255, 0 },
        8,
        0,
        EFF_BLURE_DRAW_MODE_SMOOTH,
        0,
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    };
    EnDinofos* this = (EnDinofos*)thisx;
    s32 i;
    ColliderJntSphElementDim* dim;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, 90.0f);
    Effect_Add(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &sBlureInit);
    Collider_InitAndSetJntSph(play, &this->bodyAndFireCollider, &this->actor, &sJntSphInit,
                              this->bodyAndFireColliderElements);
    Collider_InitAndSetQuad(play, &this->knifeCollider, &this->actor, &sQuadInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDinolfosSkel, &gDinolfosIdleAnim, this->jointTable, this->morphTable,
                       DINOLFOS_LIMB_MAX);

    if (!sTexturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sEyeTextures); i++) {
            sEyeTextures[i] = Lib_SegmentedToVirtual(sEyeTextures[i]);
        }
        sTexturesDesegmented = true;
    }

    this->envColorAlpha = 255;

    for (i = DINOFOS_COLLIDER_FIRE_START_INDEX; i < ARRAY_COUNT(this->bodyAndFireColliderElements); i++) {
        dim = &this->bodyAndFireCollider.elements[i].dim;
        dim->worldSphere.radius = dim->modelSphere.radius;
    }

    this->isDodgingGoronPound = false;

    if (this->actor.csId == CS_ID_NONE) {
        EnDinofos_SetupIdle(this);
    } else {
        this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
        sCsId = thisx->csId;
        EnDinofos_SetupPlayCutscene(this);
    }

    sNumAlive++;
}

void EnDinofos_Destroy(Actor* thisx, PlayState* play) {
    EnDinofos* this = (EnDinofos*)thisx;

    Effect_Destroy(play, this->effectIndex);
    Collider_DestroyJntSph(play, &this->bodyAndFireCollider);
    Collider_DestroyQuad(play, &this->knifeCollider);
}

void EnDinofos_Blink(EnDinofos* this) {
    if (this->eyeTexIndex != 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex == 4) {
            this->eyeTexIndex = 0;
        }
    } else if (Rand_ZeroOne() < 0.05f) {
        this->eyeTexIndex = 1;
    }
}

s32 EnDinofos_IsFacingPlayer(EnDinofos* this) {
    s16 angleToPlayer = (this->actor.yawTowardsPlayer - this->headRotY) - this->actor.shape.rot.y;

    if (ABS_ALT(angleToPlayer) < 0x3000) {
        return true;
    }
    return false;
}

void EnDinofos_ChooseAction(EnDinofos* this, PlayState* play) {
    if (EnDinofos_IsFacingPlayer(this)) {
        if (this->actor.xzDistToPlayer < 100.0f) {
            if (!Actor_OtherIsTargeted(play, &this->actor) &&
                (((this->actionFunc != EnDinofos_Slash) && (Rand_ZeroOne() > 0.35f)) ||
                 ((this->actionFunc == EnDinofos_Slash) && (Rand_ZeroOne() > 0.8f)))) {
                EnDinofos_SetupSlash(this);
            } else {
                EnDinofos_SetupCircleAroundPlayer(this, play);
            }
        } else if ((this->actor.xzDistToPlayer < 260.0f) && (this->actor.xzDistToPlayer > 180.0f)) {
            if (((this->actionFunc != EnDinofos_Land) && (Rand_ZeroOne() < 0.1f)) ||
                ((this->actionFunc == EnDinofos_Land) && (Rand_ZeroOne() < 0.05f))) {
                EnDinofos_SetupChooseJump(this, DINOFOS_JUMP_TYPE_FORWARD);
            } else {
                EnDinofos_SetupCircleAroundPlayer(this, play);
            }
        } else if (Rand_ZeroOne() < 0.8f) {
            EnDinofos_SetupWalk(this, play);
        } else {
            EnDinofos_SetupCircleAroundPlayer(this, play);
        }
    } else if (Rand_ZeroOne() < 0.6f) {
        EnDinofos_SetupTurnToPlayer(this);
    } else {
        EnDinofos_SetupIdle(this);
    }
}

void EnDinofos_EndCutscene(EnDinofos* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Camera* subCam = Play_GetCamera(play, this->subCamId);

        Play_SetCameraAtEye(play, CAM_ID_MAIN, &subCam->at, &subCam->eye);
        this->subCamId = SUB_CAM_ID_DONE;
        CutsceneManager_Stop(this->actor.csId);
        if (this->actor.colChkInfo.health == 0) {
            Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_END);
        }
    }
}

void EnDinofos_Freeze(EnDinofos* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.55f;
    this->bodyAndFireCollider.base.colMaterial = COL_MATERIAL_HIT3;
    this->drawDmgEffFrozenSteamScale = 825.0f * 0.001f;
    this->drawDmgEffAlpha = 1.0f;
    this->stunTimer = 80;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void EnDinofos_ThawIfFrozen(EnDinofos* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->bodyAndFireCollider.base.colMaterial = COL_MATERIAL_HIT0;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, DINOFOS_BODYPART_MAX, 2, 0.3f, 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }
}

/**
 * After this function is run, Dinolfos resumes being vulnerable to goron pound if it had dodged one.
 */
void EnDinofos_EnableBumperCollision(EnDinofos* this) {
    s32 i;

    if (this->isDodgingGoronPound) {
        for (i = 0; i < DINOFOS_COLLIDER_FIRE_START_INDEX; i++) {
            this->bodyAndFireCollider.elements[i].base.acDmgInfo.dmgFlags |= 0x400;
        }
        this->isDodgingGoronPound = false;
    }
}

s32 EnDinofos_Dodge(EnDinofos* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angleToProjectile;
    s16 angleToBombchu;
    Actor* actor;
    s32 i;

    // Subtracts headRotY from angleToProjectile to then add it back.
    actor = func_800BC270(play, &this->actor, 80.0f, 0x138B0);
    if (actor != NULL) {
        angleToProjectile = (Actor_WorldYawTowardActor(&this->actor, actor) - this->actor.shape.rot.y) - this->headRotY;
        if (ABS_ALT(angleToProjectile) < 0x3000) {
            EnDinofos_SetupDodgeProjectile(this, angleToProjectile + this->headRotY);
            Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_WARAU);
            return true;
        }
    }

    actor = func_800BC444(play, &this->actor, 80.0f);
    if ((actor != NULL) && (actor->id == ACTOR_EN_BOM_CHU)) {
        angleToBombchu = (Actor_WorldYawTowardActor(&this->actor, actor) - this->actor.shape.rot.y) - this->headRotY;
        if (ABS_ALT(angleToBombchu) < 0x3000) {
            EnDinofos_SetupChooseJump(this, DINOFOS_JUMP_TYPE_IN_PLACE);
            Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_WARAU);
            return true;
        }
    }

    if (play->actorCtx.unk2 != 0) {
        EnDinofos_SetupChooseJump(this, DINOFOS_JUMP_TYPE_IN_PLACE);
        return true;
    }

    if ((this->actor.xzDistToPlayer < 100.0f) && (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
        this->actor.isLockedOn && (Rand_ZeroOne() < 0.5f) && EnDinofos_IsFacingPlayer(this) &&
        Player_IsFacingActor(&this->actor, 0x2000, play)) {
        if (Rand_ZeroOne() < 0.5f) {
            EnDinofos_SetupChooseJump(this, DINOFOS_JUMP_TYPE_BACKWARD);
        } else {
            EnDinofos_SetupChooseJump(this, DINOFOS_JUMP_TYPE_SLASH);
        }
        Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_WARAU);
        return true;
    }

    if ((GET_PLAYER_FORM == PLAYER_FORM_GORON) && (player->actor.velocity.y < -5.0f) && (player->av1.actionVar1 == 1) &&
        (!this->isDodgingGoronPound)) {
        this->isDodgingGoronPound = true;
        for (i = 0; i < DINOFOS_COLLIDER_FIRE_START_INDEX; i++) {
            this->bodyAndFireCollider.elements[i].base.acDmgInfo.dmgFlags &= ~0x400;
        }
    }

    return false;
}

void EnDinofos_SetupIntroCutsceneBeforeFall(EnDinofos* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Player* player = GET_PLAYER(play);
    Vec3f diffToPlayer;

    Animation_Change(&this->skelAnime, &gDinolfosJumpAnim, 1.0f, Animation_GetLastFrame(&gDinolfosJumpAnim),
                     Animation_GetLastFrame(&gDinolfosJumpAnim), ANIMMODE_ONCE, 0.0f);
    func_800BE33C(&subCam->eye, &subCam->at, &this->subCamRot, true);
    Math_Vec3f_Diff(&this->actor.world.pos, &player->actor.world.pos, &diffToPlayer);
    this->subCamEye.x = player->actor.world.pos.x + (0.4f * diffToPlayer.x);
    this->subCamEye.y = player->actor.world.pos.y + 5.0f;
    this->subCamEye.z = player->actor.world.pos.z + (0.4f * diffToPlayer.z);
    this->subCamAt.x = this->actor.world.pos.x;
    this->subCamAt.y = this->actor.focus.pos.y - 400.0f;
    this->subCamAt.z = this->actor.world.pos.z;
    this->subCamEyeStep = Math_Vec3f_DistXYZ(&subCam->eye, &this->subCamEye) * 0.05f;
    this->subCamAtStep = Math_Vec3f_DistXYZ(&subCam->at, &this->subCamAt) * 0.05f;
    this->cutsceneTimer = 20;
    this->actionFunc = EnDinofos_IntroCutsceneBeforeFall;
}

void EnDinofos_IntroCutsceneBeforeFall(EnDinofos* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);

    this->cutsceneTimer--;
    Math_Vec3f_StepTo(&subCam->eye, &this->subCamEye, this->subCamEyeStep);
    Math_Vec3f_StepTo(&subCam->at, &this->subCamAt, this->subCamAtStep);
    Play_SetCameraAtEye(play, this->subCamId, &subCam->at, &subCam->eye);

    if (this->cutsceneTimer == 0) {
        EnDinofos_SetupIntroCutsceneFall(this);
    }
}

void EnDinofos_SetupIntroCutsceneFall(EnDinofos* this) {
    this->actor.gravity = -2.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_CRY);
    this->subCamEye.x = (Math_SinS(this->actor.shape.rot.y - 0x0E00) * 86.0f) + this->actor.world.pos.x;
    this->subCamEye.y = this->actor.floorHeight + 6.0f;
    this->subCamEye.z = (Math_CosS(this->actor.shape.rot.y - 0x0E00) * 86.0f) + this->actor.world.pos.z;
    this->cutsceneTimer = 0;
    this->actionFunc = EnDinofos_IntroCutsceneFall;
}

void EnDinofos_IntroCutsceneFall(EnDinofos* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f subCamAt;

    Math_Vec3f_StepTo(&subCam->eye, &this->subCamEye, 10.0f);
    this->cutsceneTimer++;

    if (this->cutsceneTimer == 10) {
        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
    }

    subCamAt.x = this->actor.world.pos.x;
    subCamAt.z = this->actor.world.pos.z;
    if (this->actor.focus.pos.y <= subCam->at.y) {
        subCamAt.y = this->actor.focus.pos.y;
    } else {
        subCamAt.y = subCam->at.y;
    }

    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCam->eye);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnDinofos_SetupIntroCutsceneLandAndBreatheFire(this);
    }
}

void EnDinofos_SetupIntroCutsceneLandAndBreatheFire(EnDinofos* this) {
    if (this->cutsceneTimer < 10) {
        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
    }
    Animation_PlayOnce(&this->skelAnime, &gDinolfosIntroAnim);
    this->subCamEye.x = (Math_SinS(this->actor.shape.rot.y + 0x200) * 123.0f) + this->actor.world.pos.x;
    this->subCamEye.y = this->actor.floorHeight + 11.0f;
    this->subCamEye.z = (Math_CosS(this->actor.shape.rot.y + 0x200) * 123.0f) + this->actor.world.pos.z;
    Actor_PlaySfx(&this->actor, NA_SE_EN_BOMCHU_WALK);
    this->cutsceneTimer = 0;
    this->timer2 = -1;
    this->actionFunc = EnDinofos_IntroCutsceneLandAndBreatheFire;
}

void EnDinofos_IntroCutsceneLandAndBreatheFire(EnDinofos* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);

    this->cutsceneTimer++;
    if (this->cutsceneTimer < 8) {
        Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCam->eye);
    }

    if (this->skelAnime.curFrame > 35.0f) {
        if ((play->sceneId == SCENE_MITURIN) && Animation_OnFrame(&this->skelAnime, 38.0f)) {
            play->envCtx.lightSettingOverride = 11;
        }

        Math_Vec3f_StepTo(&subCam->eye, &this->subCamEye, 10.0f);
        Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCam->eye);
        if (this->skelAnime.curFrame <= 55.0f) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_DODO_J_FIRE - SFX_FLAG);
        }
    }

    if ((play->sceneId == SCENE_MITURIN) && Animation_OnFrame(&this->skelAnime, 55.0f)) {
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        EnDinofos_SetupIntroCutsceneYell(this);
    }
}

void EnDinofos_SetupIntroCutsceneYell(EnDinofos* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDinolfosCryAnim, -3.0f);
    this->actionFunc = EnDinofos_IntroCutsceneYell;
}

void EnDinofos_IntroCutsceneYell(EnDinofos* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 2.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_ATTACK);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        EnDinofos_EndCutscene(this, play);
        this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
        this->actor.csId = CS_ID_NONE;
        EnDinofos_SetupIdle(this);
    }
}

void EnDinofos_SetupIdle(EnDinofos* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDinolfosIdleAnim, -4.0f);
    this->actionTimer = (s32)Rand_ZeroFloat(20.0f) + 40;
    this->idleTimer = 30;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnDinofos_EnableBumperCollision(this);
    this->actionFunc = EnDinofos_Idle;
}

void EnDinofos_Idle(EnDinofos* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->idleTimer != 0) {
        this->idleTimer--;
    } else if (EnDinofos_IsFacingPlayer(this)) {
        this->actionTimer = 0;
    }

    if (this->actionTimer != 0) {
        this->actionTimer--;
    } else {
        EnDinofos_ChooseAction(this, play);
    }
}

void EnDinofos_SetupWalk(EnDinofos* this, PlayState* play) {
    f32 targetDist;

    if (this->actionFunc != EnDinofos_Walk) {
        Animation_MorphToLoop(&this->skelAnime, &gDinolfosWalkAnim, -4.0f);
        if (Actor_OtherIsTargeted(play, &this->actor)) {
            targetDist = 170.0f;
        } else {
            targetDist = 70.0f;
        }
        if (this->actor.xzDistToPlayer <= targetDist) {
            this->actor.speed = -3.5f;
        } else {
            this->actor.speed = 3.5f;
        }
    }

    this->walkTimer = (s32)Rand_ZeroFloat(10.0f) + 20;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnDinofos_Walk;
}

void EnDinofos_Walk(EnDinofos* this, PlayState* play) {
    f32 targetDist;

    SkelAnime_Update(&this->skelAnime);
    if (!EnDinofos_Dodge(this, play)) {
        if (Actor_OtherIsTargeted(play, &this->actor)) {
            targetDist = 170.0f;
        } else {
            targetDist = 70.0f;
        }

        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->actor.xzDistToPlayer <= targetDist) {
            Math_StepToF(&this->actor.speed, -7.0f, 0.5f);
        } else {
            Math_StepToF(&this->actor.speed, 7.0f, 0.5f);
        }

        if (this->actor.xzDistToPlayer < 80.0f) {
            this->walkTimer = 0;
        }

        if (this->walkTimer != 0) {
            this->walkTimer--;
        } else {
            EnDinofos_ChooseAction(this, play);
        }

        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_WALK);
        }
    }
}

void EnDinofos_SetupTurnToPlayer(EnDinofos* this) {
    if (this->actionFunc != EnDinofos_TurnToPlayer) {
        Animation_MorphToLoop(&this->skelAnime, &gDinolfosWalkAnim, -4.0f);
        this->actor.speed = 0.0f;
    }

    if (BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y) > 0) {
        this->targetRotY = BINANG_ADD(this->actor.shape.rot.y, 0x4000);
    } else {
        this->targetRotY = BINANG_SUB(this->actor.shape.rot.y, 0x4000);
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnDinofos_EnableBumperCollision(this);
    this->actionFunc = EnDinofos_TurnToPlayer;
}

void EnDinofos_TurnToPlayer(EnDinofos* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetRotY, 0x400)) {
        EnDinofos_ChooseAction(this, play);
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnDinofos_SetupCircleAroundPlayer(EnDinofos* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actionFunc != EnDinofos_CircleAroundPlayer) {
        s16 rotY = player->actor.shape.rot.y - this->actor.shape.rot.y;
        if (ABS_ALT(rotY) > 0x7800) {
            if (Rand_ZeroOne() < 0.5f) {
                this->actor.speed = 6.0f;
            } else {
                this->actor.speed = -6.0f;
            }
        } else if (rotY >= 0) {
            this->actor.speed = 6.0f;
        } else {
            this->actor.speed = -6.0f;
        }

        if (this->actionFunc == EnDinofos_DodgeProjectile) {
            this->skelAnime.playSpeed = this->actor.speed * (1.0f / 6.0f);
        } else {
            Animation_Change(&this->skelAnime, &gDinolfosSidestepAnim, this->actor.speed * (1.0f / 6.0f), 0.0f, 0.0f,
                             ANIMMODE_LOOP, -4.0f);
        }

        this->actor.world.rot.y = BINANG_ADD(this->actor.shape.rot.y, 0x4000);
        this->circlingRate = 0;
        this->sidestepTimer = (s32)Rand_ZeroFloat(10.0f) + 5;
    }

    this->actionFunc = EnDinofos_CircleAroundPlayer;
}

void EnDinofos_CircleAroundPlayer(EnDinofos* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angle;
    f32 targetDistAdjustment = 0.0f;

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xBB8);
    if (!EnDinofos_Dodge(this, play)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (this->actor.speed >= 0.0f) {
                angle = BINANG_ADD(this->actor.shape.rot.y, 0x4000);
            } else {
                angle = BINANG_SUB(this->actor.shape.rot.y, 0x4000);
            }

            angle = this->actor.wallYaw - angle;

            // facing wall
            if (ABS_ALT(angle) > 0x4000) {
                this->actor.speed *= -0.8f;
                if (this->actor.speed < 0.0f) {
                    this->actor.speed -= 0.5f;
                } else {
                    this->actor.speed += 0.5f;
                }
            }
        }

        angle = BINANG_SUB(player->actor.shape.rot.y, this->actor.shape.rot.y);
        if ((angle >= 0) && (angle < 0x7800)) {
            this->actor.speed += 0.125f;
        } else if ((angle < 0) && (angle > -0x7800)) {
            this->actor.speed -= 0.125f;
        }

        if (this->actor.speed > 0.0f) {
            this->skelAnime.playSpeed = 1.0f;
        } else {
            this->skelAnime.playSpeed = -1.0f;
        }

        this->actor.world.rot.y = BINANG_ADD(this->actor.shape.rot.y, 0x4000);
        if (Actor_OtherIsTargeted(play, &this->actor)) {
            targetDistAdjustment = 100.0f;
        }

        if (this->actor.xzDistToPlayer <= (70.0f + targetDistAdjustment)) {
            Math_StepToF(&this->circlingRate, -4.0f, 1.5f);
        } else if ((90.0f + targetDistAdjustment) < this->actor.xzDistToPlayer) {
            Math_StepToF(&this->circlingRate, 4.0f, 1.5f);
        } else {
            Math_StepToF(&this->circlingRate, 0.0f, 5.65f);
        }

        this->actor.world.pos.x += Math_SinS(this->actor.shape.rot.y) * this->circlingRate;
        this->actor.world.pos.z += Math_CosS(this->actor.shape.rot.y) * this->circlingRate;
        SkelAnime_Update(&this->skelAnime);
        if (this->sidestepTimer != 0) {
            this->sidestepTimer--;
        } else {
            EnDinofos_ChooseAction(this, play);
        }

        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_WALK);
        }
    }
}

void EnDinofos_SetupChooseJump(EnDinofos* this, s32 jumpType) {
    if (jumpType == DINOFOS_JUMP_TYPE_FORWARD) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gDinolfosJumpAnim, 2.0f);
        EnDinofos_EnableBumperCollision(this);
    } else {
        Animation_MorphToPlayOnce(&this->skelAnime, &gDinolfosJumpAnim, -1.0f);
        if ((jumpType == DINOFOS_JUMP_TYPE_BACKWARD) || (jumpType == DINOFOS_JUMP_TYPE_SLASH)) {
            this->bodyAndFireCollider.base.acFlags &= ~AC_ON;
            EnDinofos_EnableBumperCollision(this);
        }
    }

    this->actor.speed = 0.0f;
    this->jumpType = jumpType;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnDinofos_ChooseJump;
}

void EnDinofos_ChooseJump(EnDinofos* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x1000);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        if (this->jumpType == DINOFOS_JUMP_TYPE_SLASH) {
            EnDinofos_SetupJumpSlash(this);
        } else {
            EnDinofos_SetupJump(this);
        }
    }
}

void EnDinofos_SetupJump(EnDinofos* this) {
    if (this->jumpType == DINOFOS_JUMP_TYPE_BACKWARD) {
        this->actor.speed = -10.0f;
        this->actor.velocity.y = 9.0f;
        this->bodyAndFireCollider.base.acFlags |= AC_ON;
    } else {
        this->actor.velocity.y = 12.5f;
        if (this->jumpType == DINOFOS_JUMP_TYPE_FORWARD) {
            this->actor.speed = 4.5f;
        }
    }

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_JUMP);
    this->actionFunc = EnDinofos_Jump;
}

void EnDinofos_Jump(EnDinofos* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnDinofos_SetupLand(this);
    }
}

void EnDinofos_SetupJumpSlash(EnDinofos* this) {
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.speed = 8.0f;
    this->actor.velocity.y = 16.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_JUMP);
    this->attackTimer = 0;
    this->timer2 = -1;
    this->actionFunc = EnDinofos_JumpSlash;
}

/**
 * The Dinolfos will always swing its knife when it begins falling from jumping over the player.
 */
void EnDinofos_JumpSlash(EnDinofos* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->attackTimer++;
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x1800);

    if ((this->actor.velocity.y < 0.0f) && (this->skelAnime.animation == &gDinolfosJumpAnim)) {
        this->knifeCollider.base.atFlags |= AT_ON;
        this->bodyAndFireCollider.base.acFlags |= AC_ON;
        Animation_Change(&this->skelAnime, &gDinolfosAttackAnim, 1.0f, 7.0f, 13.0f, ANIMMODE_ONCE, -2.0f);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->knifeCollider.base.atFlags &= ~AT_ON;
        EnDinofos_SetupLand(this);
    }
}

void EnDinofos_SetupLand(EnDinofos* this) {
    if (this->actionFunc != EnDinofos_JumpSlash) {
        Animation_PlayOnce(&this->skelAnime, &gDinolfosLandAnim);
    } else {
        this->skelAnime.endFrame = Animation_GetLastFrame(&gDinolfosAttackAnim);
    }

    if (this->actor.speed < 0.0f) {
        this->isJumpingBackward = true;
    } else {
        this->isJumpingBackward = false;
    }

    this->actor.speed = 0.0f;
    EnDinofos_EnableBumperCollision(this);
    Actor_PlaySfx(&this->actor, NA_SE_EN_BOMCHU_WALK);
    this->actionFunc = EnDinofos_Land;
}

void EnDinofos_Land(EnDinofos* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if ((this->isJumpingBackward == true) && (this->actor.xzDistToPlayer < 280.0f) &&
            EnDinofos_IsFacingPlayer(this) && (Rand_ZeroOne() < 0.6f)) {
            EnDinofos_SetupStartBreatheFire(this);
        } else {
            EnDinofos_ChooseAction(this, play);
        }
    }
}

void EnDinofos_SetupSlash(EnDinofos* this) {
    Animation_PlayOnce(&this->skelAnime, &gDinolfosAttackAnim);
    this->knifeCollider.base.atFlags &= ~AT_BOUNCED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_CRY);
    this->attackTimer = 0;
    this->timer2 = -1;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnDinofos_Slash;
}

void EnDinofos_Slash(EnDinofos* this, PlayState* play) {
    this->attackTimer++;
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x800);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (SkelAnime_Update(&this->skelAnime)) {
        EffectBlure_AddSpace(Effect_GetByIndex(this->effectIndex));
        this->knifeCollider.base.atFlags &= ~AT_ON;
        EnDinofos_ChooseAction(this, play);
    } else if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
        this->knifeCollider.base.atFlags |= AT_ON;
        EnDinofos_EnableBumperCollision(this);
    } else if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
        this->knifeCollider.base.atFlags &= ~AT_ON;
    } else if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_ATTACK);
    } else if (this->skelAnime.curFrame < 7.0f) {
        EnDinofos_Dodge(this, play);
    }
}

void EnDinofos_SetupRecoil(EnDinofos* this) {
    Animation_Change(&this->skelAnime, &gDinolfosAttackAnim, -1.0f, this->skelAnime.curFrame, 0.0f, ANIMMODE_ONCE,
                     0.0f);
    this->knifeCollider.base.atFlags &= ~(AT_ON | AT_BOUNCED);
    if (this->actionFunc != EnDinofos_JumpSlash) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else {
        this->actor.speed = 3.0f;
    }
    this->actionFunc = EnDinofos_Recoil;
}

void EnDinofos_Recoil(EnDinofos* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        EnDinofos_ChooseAction(this, play);
    }
}

void EnDinofos_SetupStunned(EnDinofos* this) {
    this->actor.speed = 0.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    this->actionFunc = EnDinofos_Stunned;
}

void EnDinofos_Stunned(EnDinofos* this, PlayState* play) {
    if (this->stunTimer != 0) {
        this->stunTimer--;
    }

    if (this->stunTimer == 0) {
        EnDinofos_ThawIfFrozen(this, play);
        if (this->actor.colChkInfo.health == 0) {
            if (this->actor.csId == CS_ID_NONE) {
                EnDinofos_SetupDie(this);
            } else {
                EnDinofos_SetupPlayCutscene(this);
            }
        } else if (EnDinofos_IsFacingPlayer(this) && (this->actor.xzDistToPlayer < 100.0f)) {
            EnDinofos_SetupStartBreatheFire(this);
        } else {
            EnDinofos_ChooseAction(this, play);
        }
    }
}

void EnDinofos_SetupDamaged(EnDinofos* this, s32 colliderIndex) {
    Animation_PlayOnce(&this->skelAnime, &gDinolfosHitAnim);
    func_800BE5CC(&this->actor, &this->bodyAndFireCollider, colliderIndex);
    this->actor.shape.rot.y = BINANG_ROT180(this->actor.world.rot.y);
    this->actor.speed = 10.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    this->headRotY = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_DAMAGE);
    this->bodyAndFireCollider.base.acFlags &= ~AC_ON;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 18);
    this->actionFunc = EnDinofos_Damaged;
}

void EnDinofos_Damaged(EnDinofos* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    if (SkelAnime_Update(&this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->actor.colChkInfo.health == 0) {
            if (this->actor.csId == CS_ID_NONE) {
                EnDinofos_SetupDie(this);
            } else {
                EnDinofos_SetupPlayCutscene(this);
            }
        } else if (this->actor.xzDistToPlayer < 280.0f) {
            EnDinofos_SetupStartBreatheFire(this);
        } else {
            this->bodyAndFireCollider.base.acFlags |= AC_ON;
            EnDinofos_ChooseAction(this, play);
        }
    }
}

void EnDinofos_SetupStartBreatheFire(EnDinofos* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDinolfosFireStartAnim, -5.0f);
    this->bodyAndFireCollider.base.acFlags |= AC_ON;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnDinofos_StartBreatheFire;
}

void EnDinofos_StartBreatheFire(EnDinofos* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x800);
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDinofos_SetupBreatheFire(this, play);
    } else if (!EnDinofos_Dodge(this, play) && Animation_OnFrame(&this->skelAnime, 12.0f)) {
        this->actor.speed = 8.0f;
    }
}

void EnDinofos_SetupBreatheFire(EnDinofos* this, PlayState* play) {
    s32 i;
    Sphere16* worldSphere;

    Animation_PlayLoop(&this->skelAnime, &gDinolfosFireLoopAnim);
    this->attackTimer = 20;
    this->actor.speed = 0.0f;
    this->bodyAndFireCollider.base.atFlags |= AT_ON;
    EnDinofos_EnableBumperCollision(this);

    for (i = 6; i < ARRAY_COUNT(this->bodyAndFireColliderElements); i++) {
        worldSphere = &this->bodyAndFireCollider.elements[i].dim.worldSphere;
        worldSphere->center.x = this->bodyPartsPos[DINOFOS_BODYPART_JAW].x;
        worldSphere->center.y = this->bodyPartsPos[DINOFOS_BODYPART_JAW].y;
        worldSphere->center.z = this->bodyPartsPos[DINOFOS_BODYPART_JAW].z;
    }

    if (play->sceneId == SCENE_MITURIN) {
        play->envCtx.lightSettingOverride = 11;
    }

    this->actionFunc = EnDinofos_BreatheFire;
}

void EnDinofos_BreatheFire(EnDinofos* this, PlayState* play) {
    s16 fireRotY = ((s32)Rand_CenteredFloat(0x1000) + this->actor.shape.rot.y) + this->headRotY;
    Vec3f velocity;
    Vec3f accel;
    ColliderJntSphElementDim* dim;
    s32 i;
    s32 end;
    f32 sin;
    f32 cos;
    s32 dimAlphaStep;

    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x800);
    sin = Math_SinS(fireRotY);
    cos = Math_CosS(fireRotY);

    dimAlphaStep = 10 - this->attackTimer;
    dimAlphaStep = CLAMP_MIN(dimAlphaStep, 0);

    velocity.x = 11.0f * sin;
    velocity.y = Rand_CenteredFloat(2.0f) + -5.4f;
    velocity.z = 11.0f * cos;
    accel.x = 0.9f * sin;
    accel.y = Rand_CenteredFloat(0.6f) + 1.4f;
    accel.z = 0.9f * cos;
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_DODO_J_FIRE - SFX_FLAG);
    EffectSsDFire_Spawn(play, &this->bodyPartsPos[DINOFOS_BODYPART_JAW], &velocity, &accel, 30, 22,
                        255 - (dimAlphaStep * 20), 20, 3, 8);

    // This lets the fire balls go away from Dinolfos' jaw one after the other.
    end = DINOFOS_COLLIDER_FIRE_START_INDEX;
    for (i = ARRAY_COUNT(this->bodyAndFireColliderElements) - DINOFOS_COLLIDER_FIRE_START_INDEX; i > 0; i--) {
        if (this->attackTimer < (20 + -(i * 2))) {
            end = i + DINOFOS_COLLIDER_FIRE_START_INDEX;
            break;
        }
    }

    // The fire balls sweep in front of Dinolfos.
    for (i = DINOFOS_COLLIDER_FIRE_START_INDEX; i < end; i++) {
        dim = &this->bodyAndFireCollider.elements[i].dim;
        fireRotY =
            (s32)(Math_CosF((this->attackTimer + ((i - 5) << 1)) * (M_PIf / 20)) * 0x2C00) + this->actor.shape.rot.y;

        dim->worldSphere.center.x =
            (s32)this->bodyPartsPos[DINOFOS_BODYPART_JAW].x + (s32)(Math_SinS(fireRotY) * dim->modelSphere.center.z);
        dim->worldSphere.center.y = (s32)this->bodyPartsPos[DINOFOS_BODYPART_JAW].y + (s32)dim->modelSphere.center.y;
        dim->worldSphere.center.z =
            (s32)this->bodyPartsPos[DINOFOS_BODYPART_JAW].z + (s32)(Math_CosS(fireRotY) * dim->modelSphere.center.z);
    }

    if (this->attackTimer != 0) {
        this->attackTimer--;
    } else {
        EnDinofos_SetupEndBreatheFire(this, play);
    }
}

void EnDinofos_SetupEndBreatheFire(EnDinofos* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gDinolfosFireEndAnim);
    this->bodyAndFireCollider.base.atFlags &= ~AT_ON;
    if (play->sceneId == SCENE_MITURIN) {
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    }
    this->actionFunc = EnDinofos_EndBreatheFire;
}

void EnDinofos_EndBreatheFire(EnDinofos* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDinofos_ChooseAction(this, play);
    }
}

void EnDinofos_SetupDie(EnDinofos* this) {
    Animation_PlayOnce(&this->skelAnime, &gDinolfosDieAnim);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_DEAD);
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnDinofos_Die;
}

void EnDinofos_Die(EnDinofos* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        s32 envColorAlpha = this->envColorAlpha - 10;

        if (this->actor.category == ACTORCAT_ENEMY) {
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
            EnDinofos_EndCutscene(this, play);
        }

        if (envColorAlpha <= 0) {
            Actor_Kill(&this->actor);
            this->envColorAlpha = 0;
        } else {
            this->envColorAlpha = envColorAlpha;
        }

        if (this->drawDmgEffAlpha > 0.0f) {
            this->drawDmgEffAlpha = this->envColorAlpha * (1.0f / 255.0f);
        }

        this->actor.shape.shadowAlpha = this->envColorAlpha;
    } else if (Animation_OnFrame(&this->skelAnime, 26.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GERUDOFT_DOWN);
    }
}

void EnDinofos_SetupDodgeProjectile(EnDinofos* this, s16 angleToProjectile) {
    if (angleToProjectile >= 0) {
        this->actor.speed = -15.0f;
    } else {
        this->actor.speed = 15.0f;
    }
    Animation_Change(&this->skelAnime, &gDinolfosSidestepAnim, this->actor.speed * (1.0f / 7.5f), 0.0f, 0.0f,
                     ANIMMODE_LOOP, -4.0f);
    this->actor.world.rot.y = BINANG_ADD(this->actor.shape.rot.y, 0x4000);
    this->sidestepTimer = 10;
    this->circlingRate = 0.0f;
    EnDinofos_EnableBumperCollision(this);
    this->actionFunc = EnDinofos_DodgeProjectile;
}

void EnDinofos_DodgeProjectile(EnDinofos* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xBB8);
    Math_StepToF(&this->actor.speed, 0.0f, 2.0f);

    this->skelAnime.playSpeed =
        (1.0f + fabsf(this->actor.speed * (1.0f / 15.0f))) * ((this->actor.speed >= 0.0f) ? 1.0f : -1.0f);
    this->actor.world.rot.y = BINANG_ADD(this->actor.shape.rot.y, 0x4000);
    SkelAnime_Update(&this->skelAnime);
    if (this->sidestepTimer != 0) {
        this->sidestepTimer--;
    } else {
        EnDinofos_ChooseAction(this, play);
    }

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_RIZA_WALK);
    }
}

void EnDinofos_SetupPlayCutscene(EnDinofos* this) {
    CutsceneManager_Queue(this->actor.csId);
    this->actionFunc = EnDinofos_PlayCutscene;
}

void EnDinofos_PlayCutscene(EnDinofos* this, PlayState* play) {
    Vec3f subCamEye;

    if (CutsceneManager_IsNext(this->actor.csId)) {
        if (this->actor.colChkInfo.health == 0) {
            CutsceneManager_Start(this->actor.csId, &this->actor);
            Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_WAIT);
        } else {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        }
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
        if (this->actor.colChkInfo.health == 0) {
            subCamEye.x = (Math_SinS(this->actor.shape.rot.y) * 150.0f) + this->actor.focus.pos.x;
            subCamEye.y = this->actor.focus.pos.y;
            subCamEye.z = (Math_CosS(this->actor.shape.rot.y) * 150.0f) + this->actor.focus.pos.z;
            Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCamEye);
            EnDinofos_SetupDie(this);
        } else {
            EnDinofos_SetupIntroCutsceneBeforeFall(this, play);
        }
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnDinofos_RotateHead(EnDinofos* this, PlayState* play) {
    s16 headRotYDiff;

    if ((this->actionFunc == EnDinofos_Idle) && (this->headTimer != 0)) {
        Math_ScaledStepToS(&this->headRotY, Math_SinS(this->headTimer * 1400) * 0x2C00, 0x300);
    } else if (this->actionFunc == EnDinofos_StartBreatheFire) {
        Math_ScaledStepToS(&this->headRotY, Math_CosF(M_PIf) * 0x2C00, 0x2C00 / 20);
    } else if (this->actionFunc == EnDinofos_BreatheFire) {
        this->headRotY = Math_CosF(this->headTimer * (M_PIf / 20)) * 0x2C00;
    } else if (!Play_InCsMode(play)) {
        headRotYDiff = this->headRotY + this->actor.shape.rot.y;
        headRotYDiff = BINANG_SUB(this->actor.yawTowardsPlayer, headRotYDiff);
        headRotYDiff = CLAMP(headRotYDiff, -0x300, 0x300);
        this->headRotY += headRotYDiff;
        this->headRotY = CLAMP(this->headRotY, -0x2C00, 0x2C00);
    }
}

s32 EnDinofos_UpdateDamage(EnDinofos* this, PlayState* play) {
    s32 i;

    if (this->bodyAndFireCollider.base.acFlags & AC_HIT) {
        this->bodyAndFireCollider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&this->actor, &this->bodyAndFireCollider);

        for (i = 0; i < ARRAY_COUNT(this->bodyAndFireColliderElements); i++) {
            if (this->bodyAndFireCollider.elements[i].base.acElemFlags & ACELEM_HIT) {
                break;
            }
        }

        if (i == ARRAY_COUNT(this->bodyAndFireColliderElements)) {
            return false;
        }

        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
            (this->bodyAndFireCollider.elements[i].base.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            return false;
        }

        if (this->actor.colChkInfo.damageEffect == DINOLFOS_DMGEFF_IMMUNE) {
            return false;
        }

        if (!Actor_ApplyDamage(&this->actor)) {
            Enemy_StartFinishingBlow(play, &this->actor);
            sNumAlive--;
            if (sNumAlive == 0) {
                if (sCsId != CS_ID_NONE) {
                    this->actor.csId = sCsId;
                }
            }

            if (this->actor.csId != CS_ID_NONE) {
                Audio_RestorePrevBgm();
            }
        }

        EnDinofos_ThawIfFrozen(this, play);
        EnDinofos_EnableBumperCollision(this);
        if (play->sceneId == SCENE_MITURIN) {
            play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
        }

        this->knifeCollider.base.atFlags &= ~(AT_ON | AT_BOUNCED);
        this->bodyAndFireCollider.base.atFlags &= ~AT_ON;
        if (this->actor.colChkInfo.damageEffect == DINOLFOS_DMGEFF_ICE) {
            EnDinofos_Freeze(this);
            if (this->actor.colChkInfo.health == 0) {
                this->stunTimer = 3;
                this->bodyAndFireCollider.base.acFlags &= ~AC_ON;
            }
            EnDinofos_SetupStunned(this);
            return true;
        }

        if (this->actor.colChkInfo.damageEffect == DINOLFOS_DMGEFF_STUN) {
            this->stunTimer = 40;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
            Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
            EnDinofos_SetupStunned(this);
            return true;
        }

        if (this->actor.colChkInfo.damageEffect == DINOLFOS_DMGEFF_ZORA_MAGIC) {
            this->stunTimer = 40;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
            this->drawDmgEffScale = 0.55f;
            this->drawDmgEffAlpha = 2.0f;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM;
            Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
            EnDinofos_SetupStunned(this);
            return true;
        }

        if (this->actor.colChkInfo.damageEffect == DINOLFOS_DMGEFF_FIRE) {
            this->drawDmgEffAlpha = 4.0f;
            this->drawDmgEffScale = 0.55f;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        } else if (this->actor.colChkInfo.damageEffect == DINOLFOS_DMGEFF_LIGHT_SPARKS) {
            this->drawDmgEffAlpha = 4.0f;
            this->drawDmgEffScale = 0.55f;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                        this->bodyAndFireCollider.elements[i].base.acDmgInfo.hitPos.x,
                        this->bodyAndFireCollider.elements[i].base.acDmgInfo.hitPos.y,
                        this->bodyAndFireCollider.elements[i].base.acDmgInfo.hitPos.z, 0, 0, 0,
                        CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }
        EnDinofos_SetupDamaged(this, i);
        return true;
    }

    return false;
}

void EnDinofos_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDinofos* this = (EnDinofos*)thisx;
    s32 pad;
    Vec3f bodyPartPos;

    if (this->actionFunc != EnDinofos_Stunned) {
        EnDinofos_Blink(this);
    }

    if (!EnDinofos_UpdateDamage(this, play) && (this->knifeCollider.base.atFlags & AT_BOUNCED)) {
        EnDinofos_SetupRecoil(this);
    }

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 25.0f, 30.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_40);
    if (this->actionFunc != EnDinofos_Stunned) {
        if ((this->actor.depthInWater > 0.0f) && (this->actor.depthInWater < 10.0f)) {
            if (((play->gameplayFrames % 4) & 1) == 0) {
                Math_Vec3f_Copy(&bodyPartPos,
                                &this->bodyPartsPos[DINOFOS_BODYPART_LEFT_CLAWS + (play->gameplayFrames % 4)]);
                bodyPartPos.y = this->actor.world.pos.y + this->actor.depthInWater;
                EffectSsGRipple_Spawn(play, &bodyPartPos, 100, 320, 0);
            }
        }
        if (this->actionFunc != EnDinofos_Die) {
            EnDinofos_RotateHead(this, play);
        }
    }

    if ((this->actionFunc == EnDinofos_Die) || (this->actionFunc == EnDinofos_Damaged)) {
        Math_ScaledStepToS(&this->headRotY, 0, 2000);
    }

    Math_Vec3f_Copy(&this->actor.focus.pos, &this->bodyPartsPos[DINOFOS_BODYPART_UPPER_BODY]);
    this->actor.focus.rot.y = this->actor.shape.rot.y + this->headRotY;
    if (this->bodyAndFireCollider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyAndFireCollider.base);
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyAndFireCollider.base);
    if (this->bodyAndFireCollider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->bodyAndFireCollider.base);
    }

    if (this->knifeCollider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->knifeCollider.base);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * (11.0f / 40.0f);
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.55f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.55f, (11.0f / 800.0f))) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

s32 EnDinofos_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                               Gfx** gfx) {
    EnDinofos* this = (EnDinofos*)thisx;

    if (limbIndex == DINOLFOS_LIMB_HEAD) {
        rot->y -= this->headRotY;
    }

    return false;
}

static Vec3f sKnifeTipQuadOffset = { 400.0f, -3600.0f, 0.0f };
static Vec3f sKnifeBaseQuadOffset = { 300.0f, 500.0f, 0.0f };
static Vec3f sCsFireVelocityOffset = { 700.0f, 400.0f, 0.0f };

static s8 sLimbToBodyParts[DINOLFOS_LIMB_MAX] = {
    BODYPART_NONE,                    // DINOLFOS_LIMB_NONE
    BODYPART_NONE,                    // DINOLFOS_LIMB_LOWER_BODY
    DINOFOS_BODYPART_LEFT_UPPER_LEG,  // DINOLFOS_LIMB_LEFT_UPPER_LEG
    BODYPART_NONE,                    // DINOLFOS_LIMB_LEFT_LOWER_LEG
    DINOFOS_BODYPART_LEFT_CLAWS,      // DINOLFOS_LIMB_LEFT_CLAWS
    DINOFOS_BODYPART_RIGHT_UPPER_LEG, // DINOLFOS_LIMB_RIGHT_UPPER_LEG
    BODYPART_NONE,                    // DINOLFOS_LIMB_RIGHT_LOWER_LEG
    DINOFOS_BODYPART_RIGHT_CLAWS,     // DINOLFOS_LIMB_RIGHT_CLAWS
    DINOFOS_BODYPART_UPPER_BODY,      // DINOLFOS_LIMB_UPPER_BODY
    BODYPART_NONE,                    // DINOLFOS_LIMB_LEFT_UPPER_ARM
    DINOFOS_BODYPART_LEFT_FOREARM,    // DINOLFOS_LIMB_LEFT_FOREARM
    DINOFOS_BODYPART_LEFT_HAND,       // DINOLFOS_LIMB_LEFT_HAND
    BODYPART_NONE,                    // DINOLFOS_LIMB_RIGHT_UPPER_ARM
    DINOFOS_BODYPART_RIGHT_FOREARM,   // DINOLFOS_LIMB_RIGHT_FOREARM
    DINOFOS_BODYPART_RIGHT_HAND,      // DINOLFOS_LIMB_RIGHT_HAND
    BODYPART_NONE,                    // DINOLFOS_LIMB_NECK
    DINOFOS_BODYPART_HEAD,            // DINOLFOS_LIMB_HEAD
    DINOFOS_BODYPART_JAW,             // DINOLFOS_LIMB_JAW
    BODYPART_NONE,                    // DINOLFOS_LIMB_UPPER_TAIL
    DINOFOS_BODYPART_LOWER_TAIL,      // DINOLFOS_LIMB_LOWER_TAIL
    BODYPART_NONE,                    // DINOLFOS_LIMB_KNIFE
};

void EnDinofos_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnDinofos* this = (EnDinofos*)thisx;
    Vec3f prevKnifeTipQuadPos;
    Vec3f prevKnifeBaseQuadPos;
    Vec3f knifeTipQuadPos;
    Vec3f knifeBaseQuadPos;
    s32 dimAlphaStep;
    Vec3f fireVelocity;
    MtxF* matrix;

    Collider_UpdateSpheres(limbIndex, &this->bodyAndFireCollider);
    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if ((limbIndex == DINOLFOS_LIMB_RIGHT_HAND) && (this->timer2 != this->attackTimer) &&
        ((this->actionFunc == EnDinofos_Slash) || (this->actionFunc == EnDinofos_JumpSlash))) {
        Math_Vec3f_Copy(&prevKnifeBaseQuadPos, &this->knifeCollider.dim.quad[0]);
        Math_Vec3f_Copy(&prevKnifeTipQuadPos, &this->knifeCollider.dim.quad[1]);
        Matrix_MultVec3f(&sKnifeTipQuadOffset, &knifeTipQuadPos);
        Matrix_MultVec3f(&sKnifeBaseQuadOffset, &knifeBaseQuadPos);
        Collider_SetQuadVertices(&this->knifeCollider, &knifeBaseQuadPos, &knifeTipQuadPos, &prevKnifeBaseQuadPos,
                                 &prevKnifeTipQuadPos);
        if (this->knifeCollider.base.atFlags & AT_ON) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->effectIndex), &knifeTipQuadPos, &knifeBaseQuadPos);
        }
        this->timer2 = this->attackTimer;
    }

    if (limbIndex == DINOLFOS_LIMB_LEFT_CLAWS) {
        Matrix_MultVecX(300.0f, &this->actor.shape.feetPos[0]);
    } else if (limbIndex == DINOLFOS_LIMB_RIGHT_CLAWS) {
        Matrix_MultVecX(300.0f, &this->actor.shape.feetPos[1]);
    }

    if ((limbIndex == DINOLFOS_LIMB_HEAD) && (this->actionFunc == EnDinofos_IntroCutsceneLandAndBreatheFire)) {
        if ((this->skelAnime.curFrame > 38.0f) && (this->skelAnime.curFrame <= 55.0f) &&
            (this->timer2 != this->cutsceneTimer)) {
            matrix = Matrix_GetCurrent();
            dimAlphaStep = 48 - (s32)this->skelAnime.curFrame;
            dimAlphaStep = CLAMP_MIN(dimAlphaStep, 0);
            Matrix_MultVec3f(&sCsFireVelocityOffset, &fireVelocity);
            fireVelocity.x -= matrix->mf[3][0];
            fireVelocity.y -= matrix->mf[3][1];
            fireVelocity.z -= matrix->mf[3][2];
            EffectSsDFire_Spawn(play, &this->bodyPartsPos[DINOFOS_BODYPART_JAW], &fireVelocity, &gZeroVec3f, 30, 22,
                                255 - (dimAlphaStep * 20), 20, 3, 8);
            this->timer2 = this->cutsceneTimer;
        }
    }
}

void EnDinofos_Draw(Actor* thisx, PlayState* play) {
    EnDinofos* this = (EnDinofos*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->envColorAlpha == 255) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        func_800B8050(&this->actor, play, 0);

        gSPSegment(POLY_OPA_DISP++, 0x08, sEyeTextures[this->eyeTexIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, 20, 40, 40, 255);

        Scene_SetRenderModeXlu(play, 0, 1);
        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnDinofos_OverrideLimbDraw, EnDinofos_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        func_800B8118(&this->actor, play, 0);

        gSPSegment(POLY_XLU_DISP++, 0x08, sEyeTextures[this->eyeTexIndex]);
        gDPSetEnvColor(POLY_XLU_DISP++, 20, 40, 40, this->envColorAlpha);

        Scene_SetRenderModeXlu(play, 1, 2);
        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnDinofos_OverrideLimbDraw, EnDinofos_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, DINOFOS_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDinofos_Reset(void) {
    sCsId = CS_ID_NONE;
    sNumAlive = 0;
}

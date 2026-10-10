/*
 * File: z_en_po_sisters.c
 * Overlay: ovl_En_Po_Sisters
 * Description: Poe Sisters (fought in a mini-game in Ikana for a HP)
 */

#include "z_en_po_sisters.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void EnPoSisters_Init(Actor* thisx, PlayState* play);
void EnPoSisters_Destroy(Actor* thisx, PlayState* play);
void EnPoSisters_Update(Actor* thisx, PlayState* play);
void EnPoSisters_Draw(Actor* thisx, PlayState* play);

void EnPoSisters_SetupObserverIdle(EnPoSisters* this);
void EnPoSisters_ObserverIdle(EnPoSisters* this, PlayState* play);
void EnPoSisters_AimlessIdleFlying2(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupAimlessIdleFlying(EnPoSisters* this);
void EnPoSisters_AimlessIdleFlying(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupInvestigating(EnPoSisters* this);
void EnPoSisters_Investigating(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupSpinUp(EnPoSisters* this);
void EnPoSisters_SpinUp(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupSpinAttack(EnPoSisters* this);
void EnPoSisters_SpinAttack(EnPoSisters* this, PlayState* play);
void EnPoSisters_AttackConnectDrift(EnPoSisters* this, PlayState* play);
void EnPoSisters_DamageFlinch(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupFlee(EnPoSisters* this);
void EnPoSisters_Flee(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupSpinToInvis(EnPoSisters* this);
void EnPoSisters_SpinToInvis(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupSpinBackToVisible(EnPoSisters* this, PlayState* play);
void EnPoSisters_SpinBackToVisible(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupDeathStage1(EnPoSisters* this);
void EnPoSisters_DeathStage1(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupDeathStage2(EnPoSisters* this, PlayState* play);
void EnPoSisters_DeathStage2(EnPoSisters* this, PlayState* play);
void EnPoSisters_SpawnMegClones(EnPoSisters* this, PlayState* play);
void EnPoSisters_MegCloneVanish(EnPoSisters* this, PlayState* play);
void EnPoSisters_MegCloneWaitForSpinBack(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupMegSurroundPlayer(EnPoSisters* this);
void EnPoSisters_MegSurroundPlayer(EnPoSisters* this, PlayState* play);
void EnPoSisters_SetupSpawnPo(EnPoSisters* this);
void EnPoSisters_PoeSpawn(EnPoSisters* this, PlayState* play);

static Color_RGBA8 sPoSisterFlameColors[] = {
    { 255, 170, 255, 255 }, // Meg
    { 255, 200, 0, 255 },   // Jo
    { 0, 170, 255, 255 },   // Beth
    { 170, 255, 0, 255 },   // Amy
};

static Color_RGBA8 sPoSisterEnvColors[] = {
    { 100, 0, 255, 255 }, // Meg
    { 255, 0, 0, 255 },   // Jo
    { 0, 0, 255, 255 },   // Beth
    { 0, 150, 0, 255 },   // Amy
};

ActorProfile En_Po_Sisters_Profile = {
    /**/ ACTOR_EN_PO_SISTERS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PO_SISTERS,
    /**/ sizeof(EnPoSisters),
    /**/ EnPoSisters_Init,
    /**/ EnPoSisters_Destroy,
    /**/ EnPoSisters_Update,
    /**/ EnPoSisters_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x08 },
        { 0xF7CBFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 18, 60, 15, { 0, 0, 0 } },
};

// health:6, mass:50
static CollisionCheckInfoInit sColChkInfoInit = { 6, 25, 60, 50 };

typedef enum {
    /* 0x0 */ POE_SISTERS_DMGEFF_NONE,
    /* 0x1 */ POE_SISTERS_DMGEFF_UNKDMG12, // set in DamageTable, but unused
    /* 0x4 */ POE_SISTERS_DMGEFF_LIGHTARROWS = 0x4,
    /* 0xE */ POE_SISTERS_DMGEFF_SPINATTACK = 0xE,
    /* 0xF */ POE_SISTERS_DMGEFF_DEKUNUT
} PoeSisterDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_DEKUNUT),
    /* Deku Stick     */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Ice arrow      */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Light arrow    */ DMG_ENTRY(2, POE_SISTERS_DMGEFF_LIGHTARROWS),
    /* Goron spikes   */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, POE_SISTERS_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_UNKDMG12),
    /* Zora barrier   */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Normal shield  */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_SPINATTACK),
    /* Sword beam     */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, POE_SISTERS_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, POE_SISTERS_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 7, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 6000, ICHAIN_STOP),
};

// clang-format off
// PoSisters have their own flags variable for cross function behavior detection
#define POE_SISTERS_FLAG_CHECK_AC              (1 << 0)
#define POE_SISTERS_FLAG_UPDATE_SHAPE_ROT      (1 << 1)
#define POE_SISTERS_FLAG_CHECK_Z_TARGET        (1 << 2) // Meg doesnt go invisible if you ztarget her for too long
#define POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT   (1 << 3) // The Poe is attempting to level with player's height
#define POE_SISTERS_FLAG_UPDATE_BGCHECK_INFO   (1 << 4)
#define POE_SISTERS_FLAG_UPDATE_FIRES          (1 << 5) // firePos updated to match limb in PostLimbDraw
#define POE_SISTERS_FLAG_REAL_MEG_ROTATION     (1 << 6) // Real Meg rotates different than her clones for one cycle
#define POE_SISTERS_FLAG_DRAW_TORCH            (1 << 7)
// clang-format on

void EnPoSisters_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPoSisters* this = (EnPoSisters*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 50.0f);
    SkelAnime_Init(play, &this->skelAnime, &gPoeSistersSkel, &gPoeSistersSwayAnim, this->jointTable, this->morphTable,
                   POE_SISTERS_LIMB_MAX);

    this->color.r = 255;
    this->color.g = 255;
    this->color.b = 210;
    this->color.a = 255;
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Lights_PointGlowSetInfo(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, 0,
                            0, 0, 0);

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->type = POE_SISTERS_GET_TYPE(thisx);
    this->actor.hintId = this->type + TATL_HINT_ID_POE_SISTER_MEG;
    this->megCloneId = POE_SISTERS_GET_MEG_CLONE_ID(thisx);
    this->floatingBobbingTimer = 32;
    this->zTargetTimer = 20;
    this->fireCount = 1;
    this->poSisterFlags = POE_SISTERS_FLAG_UPDATE_FIRES;
    this->megDistToPlayer = 110.0f;
    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    if (POE_SISTERS_GET_OBSERVER_FLAG(&this->actor)) {
        // "Flagged observer": non-enemy floating prop spawned by EnGb2 (Poe Hut Proprieter) for display
        EnPoSisters_SetupObserverIdle(this);
    } else if (this->type == POE_SISTERS_TYPE_MEG) {
        if (this->megCloneId == POE_SISTERS_MEG_REAL) {
            this->actor.colChkInfo.health = 8;
            this->collider.elem.atDmgInfo.damage = 16;
            this->collider.base.ocFlags1 = (OC1_TYPE_PLAYER | OC1_ON);
            EnPoSisters_SpawnMegClones(this, play);
            EnPoSisters_SetupSpawnPo(this);
        } else {
            this->actor.flags &= ~(ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR | ACTOR_FLAG_CAN_ATTACH_TO_ARROW);
            this->collider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
            this->collider.elem.acDmgInfo.dmgFlags |= (0x40000 | 0x1);
            this->collider.base.ocFlags1 = OC1_NONE;
            EnPoSisters_MegCloneVanish(this, NULL);
        }
    } else {
        EnPoSisters_SetupSpawnPo(this);
    }

    // ! @Weird: this actor doesn't accept params in this range, so params is just cleared
    this->actor.params &= 0xFF;
}

void EnPoSisters_Destroy(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * For some reason, it uses both deathTimer (param) and this->deathTimer.
 */
void EnPoSisters_UpdateDeathFlameSwirl(EnPoSisters* this, s32 deathTimerParam, Vec3f* pos) {
    s32 i;
    Vec3f* firePos;
    f32 deathTimerParamF = deathTimerParam;

    for (i = 0; i < this->fireCount; i++) {
        firePos = &this->firePos[i];
        firePos->x = pos->x + Math_SinS(this->actor.shape.rot.y + (this->deathTimer * 0x800) + (i * 0x2000)) *
                                  (SQ(deathTimerParamF) * 0.1f);
        firePos->z = pos->z + Math_CosS(this->actor.shape.rot.y + (this->deathTimer * 0x800) + (i * 0x2000)) *
                                  (SQ(deathTimerParamF) * 0.1f);
        firePos->y = pos->y + deathTimerParamF;
    }
}

void EnPoSisters_MatchPlayerXZ(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 dist;

    if ((this->megCloneId == POE_SISTERS_MEG_REAL) || (this->actionFunc != EnPoSisters_DamageFlinch)) {
        if (((player->meleeWeaponState == PLAYER_MELEE_WEAPON_STATE_0) ||
             (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H)) &&
            ((player->actor.world.pos.y - player->actor.floorHeight) < 1.0f)) {
            Math_StepToF(&this->megDistToPlayer, 110.0f, 3.0f);
        } else {
            Math_StepToF(&this->megDistToPlayer, 170.0f, 10.0f);
        }
        dist = this->megDistToPlayer;
    } else if (this->megCloneId != POE_SISTERS_MEG_REAL) {
        dist = this->actor.parent->xzDistToPlayer;
    }

    this->actor.world.pos.x = player->actor.world.pos.x + (Math_SinS(BINANG_ROT180(this->actor.shape.rot.y)) * dist);
    this->actor.world.pos.z = player->actor.world.pos.z + (Math_CosS(BINANG_ROT180(this->actor.shape.rot.y)) * dist);
}

void EnPoSisters_MatchPlayerY(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // equalize to player height
    Math_ApproachF(&this->actor.world.pos.y, player->actor.world.pos.y + 5.0f, 0.5f, 3.0f);

    if ((u32)this->floatingBobbingTimer == 0) {
        this->floatingBobbingTimer = 32;
    }

    DECR(this->floatingBobbingTimer);

    // random float/flying wobble
    this->actor.world.pos.y += (2.0f + (0.5f * Rand_ZeroOne())) * Math_SinS(this->floatingBobbingTimer * 0x800);

    // fully opaque
    if ((this->color.a == 255) && (this->actionFunc != EnPoSisters_SpinAttack) &&
        (this->actionFunc != EnPoSisters_SpinUp)) {
        if (this->actionFunc == EnPoSisters_Flee) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
        } else {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
        }
    }
}

// check for z target
void EnPoSisters_CheckZTarget(EnPoSisters* this, PlayState* play) {
    if (this->actor.isLockedOn && (this->color.a == 255)) {
        DECR(this->zTargetTimer);
    } else {
        this->zTargetTimer = 20;
    }

    if (this->color.a == 0) {
        DECR(this->invisibleTimer);
    }

    if ((this->actionFunc != EnPoSisters_SpinUp) && (this->actionFunc != EnPoSisters_SpinAttack) &&
        (this->actionFunc != EnPoSisters_DamageFlinch)) {
        if (this->zTargetTimer == 0) {
            EnPoSisters_SetupSpinToInvis(this);
        } else if ((this->invisibleTimer == 0) && (this->color.a == 0)) {
            EnPoSisters_SetupSpinBackToVisible(this, play);
        }
    }
}

void EnPoSisters_SetupObserverIdle(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    this->actor.speed = 0.0f;
    this->laughTimer = Rand_S16Offset(100, 50);
    this->actionFunc = EnPoSisters_ObserverIdle;
}

void EnPoSisters_ObserverIdle(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (DECR(this->laughTimer) == 0) {
        this->laughTimer = Rand_S16Offset(100, 50);
        Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH2);
    }
}

/**
 *  Change animation, but keep flying idle in straight line. Flying through walls possible.
 *
 *  This is never reached because conditions are never met in EnPoSisters_AimlessIdleFlying
 */
void EnPoSisters_SetupAimlessIdleFlying2(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    this->idleFlyingAnimationCounter = Rand_S16Offset(2, 3);
    this->actor.speed = 0.0f;
    this->actionFunc = EnPoSisters_AimlessIdleFlying2;
}

void EnPoSisters_AimlessIdleFlying2(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        DECR(this->idleFlyingAnimationCounter);
    }

    if ((this->idleFlyingAnimationCounter == 0) || (this->actor.xzDistToPlayer < 600.0f)) {
        EnPoSisters_SetupAimlessIdleFlying(this);
    }
}

/**
 *  Flying in a straight line, until wall or player comes near.
 */
void EnPoSisters_SetupAimlessIdleFlying(EnPoSisters* this) {
    if (this->actionFunc != EnPoSisters_Investigating) {
        Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    }
    this->idleFlyingAnimationCounter = Rand_S16Offset(15, 3);
    this->poSisterFlags |=
        (POE_SISTERS_FLAG_CHECK_Z_TARGET | POE_SISTERS_FLAG_UPDATE_SHAPE_ROT | POE_SISTERS_FLAG_CHECK_AC);
    this->actionFunc = EnPoSisters_AimlessIdleFlying;
}

void EnPoSisters_AimlessIdleFlying(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.speed, 1.0f, 0.2f);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        DECR(this->idleFlyingAnimationCounter);
    }

    if ((this->actor.xzDistToPlayer < 600.0f) && (fabsf(this->actor.playerHeightRel + 5.0f) < 30.0f)) {
        EnPoSisters_SetupInvestigating(this);
    } else if ((this->idleFlyingAnimationCounter == 0) && Math_StepToF(&this->actor.speed, 0.0f, 0.2f)) {
        // ! @bug: this is never reached because speedXZ is reduced
        //         at the same rate it is increased at the top of this function
        EnPoSisters_SetupAimlessIdleFlying2(this);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        Math_ScaledStepToS(&this->actor.world.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           0x71C);
    } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 600.0f) {
        Math_ScaledStepToS(&this->actor.world.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           0x71C);
    }
}

/**
 * Not yet agressive, gently flying/steering toward the player.
 */
void EnPoSisters_SetupInvestigating(EnPoSisters* this) {
    this->actionFunc = EnPoSisters_Investigating;
}

void EnPoSisters_Investigating(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    yawDiff = this->actor.yawTowardsPlayer - player->actor.shape.rot.y;
    Math_StepToF(&this->actor.speed, 2.0f, 0.2f);

    if (yawDiff > 0x3000) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x3000, 0x71C);
    } else if (yawDiff < -0x3000) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x3000, 0x71C);
    } else {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x71C);
    }

    if ((this->actor.xzDistToPlayer < 320.0f) && (fabsf(this->actor.playerHeightRel + 5.0f) < 30.0f)) {
        EnPoSisters_SetupSpinUp(this);
    } else if (this->actor.xzDistToPlayer > 720.0f) {
        EnPoSisters_SetupAimlessIdleFlying(this);
    }
}

/**
 * Gaining speed for spin attack before the actual attack, winding up.
 */
void EnPoSisters_SetupSpinUp(EnPoSisters* this) {
    if (this->color.a != 0) {
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
        this->collider.base.acFlags |= AC_HARD;
    }

    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    this->actor.speed = 0.0f;
    this->spinupTimer = Animation_GetLastFrame(&gPoeSistersAttackAnim) * 3 + 3;
    this->poSisterFlags &= ~POE_SISTERS_FLAG_UPDATE_SHAPE_ROT;
    this->actionFunc = EnPoSisters_SpinUp;
}

void EnPoSisters_SpinUp(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    DECR(this->spinupTimer);
    this->actor.shape.rot.y += ((s32)((this->skelAnime.endFrame + 1.0f) * 3.0f) - this->spinupTimer) * 0x180;
    if ((this->spinupTimer == 18) || (this->spinupTimer == 7)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_PO_ROLL);
    } else if (this->spinupTimer == 0) {
        EnPoSisters_SetupSpinAttack(this);
    }
}

void EnPoSisters_SetupSpinAttack(EnPoSisters* this) {
    this->actor.speed = 5.0f;
    if (this->type == POE_SISTERS_TYPE_MEG) {
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
        this->collider.base.acFlags |= AC_HARD;
        Animation_MorphToLoop(&this->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    }

    this->spinTimer = 5;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->poSisterFlags |= POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT;
    this->actionFunc = EnPoSisters_SpinAttack;
}

void EnPoSisters_SpinAttack(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        DECR(this->spinTimer);
    }

    this->actor.shape.rot.y += (s32)(1152.0f * this->skelAnime.endFrame);

    if (this->spinTimer == 0) {
        s16 rotY = this->actor.shape.rot.y - this->actor.world.rot.y;

        if (ABS_ALT(rotY) < 0x1000) {
            if (this->type != POE_SISTERS_TYPE_MEG) {
                this->collider.base.colMaterial = COL_MATERIAL_HIT3;
                this->collider.base.acFlags &= ~AC_HARD;
                EnPoSisters_SetupAimlessIdleFlying(this);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH2);
                EnPoSisters_MegCloneVanish(this, play);
            }
        }
    }

    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_PO_ROLL);
    }
}

/**
 * Attack has connected with player.
 */
void EnPoSisters_SetupAttackConnect(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->actor.world.rot.y = BINANG_ROT180(this->actor.yawTowardsPlayer);
    if (this->type != POE_SISTERS_TYPE_MEG) {
        this->collider.base.colMaterial = COL_MATERIAL_HIT3;
        this->collider.base.acFlags &= ~AC_HARD;
    }

    this->actionFunc = EnPoSisters_AttackConnectDrift;
}

void EnPoSisters_AttackConnectDrift(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y -= TRUNCF_BINANG(this->actor.speed * 10.0f * 128.0f);

    if (Math_StepToF(&this->actor.speed, 0.0f, 0.1f)) { // wait to stop moving
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->type != POE_SISTERS_TYPE_MEG) {
            EnPoSisters_SetupAimlessIdleFlying(this);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH2);
            EnPoSisters_MegCloneVanish(this, play);
        }
    }
}

void EnPoSisters_SetupDamageFlinch(EnPoSisters* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gPoeSistersDamagedAnim, -3.0f);
    if (this->collider.base.ac != NULL) {
        func_800BE504(&this->actor, &this->collider);
    }

    if (this->type != POE_SISTERS_TYPE_MEG) {
        this->actor.speed = 10.0f;
    }

    this->poSisterFlags &=
        ~(POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT | POE_SISTERS_FLAG_UPDATE_SHAPE_ROT | POE_SISTERS_FLAG_CHECK_AC);

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 16);
    this->actionFunc = EnPoSisters_DamageFlinch;
}

void EnPoSisters_DamageFlinch(EnPoSisters* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && !(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if (this->actor.colChkInfo.health != 0) {
            if (this->type != POE_SISTERS_TYPE_MEG) {
                EnPoSisters_SetupFlee(this);
            } else if (this->megCloneId != POE_SISTERS_MEG_REAL) {
                EnPoSisters_MegCloneVanish(this, NULL);
            } else {
                EnPoSisters_MegCloneVanish(this, play);
            }
        } else {
            EnPoSisters_SetupDeathStage1(this);
        }
    }

    if (this->megCloneId != POE_SISTERS_MEG_REAL) {
        s32 alpha;

        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.parent->shape.rot.y,
                           (this->megCloneId == POE_SISTERS_MEG_CLONE2) ? 0x800 : 0x400);
        alpha = ((this->skelAnime.endFrame - this->skelAnime.curFrame) * 255.0f) / this->skelAnime.endFrame;
        this->color.a = CLAMP(alpha, 0, 255);
        this->actor.world.pos.y = this->actor.parent->world.pos.y;
        EnPoSisters_MatchPlayerXZ(this, play);

    } else if (this->type != POE_SISTERS_TYPE_MEG) {
        Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    }
}

void EnPoSisters_SetupFlee(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFleeAnim, -3.0f);
    this->actor.world.rot.y = BINANG_ROT180(this->actor.shape.rot.y);
    this->fleeTimer = 5;
    this->poSisterFlags |=
        (POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT | POE_SISTERS_FLAG_UPDATE_SHAPE_ROT | POE_SISTERS_FLAG_CHECK_AC);
    this->actor.speed = 5.0f;
    this->actionFunc = EnPoSisters_Flee;
}

void EnPoSisters_Flee(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.world.rot.y, BINANG_ROT180(this->actor.yawTowardsPlayer), 0x71C);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        DECR(this->fleeTimer);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->poSisterFlags |= POE_SISTERS_FLAG_UPDATE_SHAPE_ROT;
        EnPoSisters_SetupSpinToInvis(this);
    } else if ((this->fleeTimer == 0) && (this->actor.xzDistToPlayer > 480.0f)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        EnPoSisters_SetupAimlessIdleFlying(this);
    }
}

void EnPoSisters_SetupSpinToInvis(EnPoSisters* this) {
    Animation_Change(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    this->invisibleTimer = 100; // 5 seconds
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->poSisterFlags &= ~(POE_SISTERS_FLAG_CHECK_Z_TARGET | POE_SISTERS_FLAG_CHECK_AC);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH2);
    this->actionFunc = EnPoSisters_SpinToInvis;
}

void EnPoSisters_SpinToInvis(EnPoSisters* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->color.a = 0;
        this->collider.elem.acDmgInfo.dmgFlags = (0x40000 | 0x1);
        EnPoSisters_SetupAimlessIdleFlying(this);
    } else {
        s32 alpha = ((this->skelAnime.endFrame - this->skelAnime.curFrame) * 255.0f) / this->skelAnime.endFrame;

        this->color.a = CLAMP(alpha, 0, 255);
    }
}

void EnPoSisters_SetupSpinBackToVisible(EnPoSisters* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    if (this->type == POE_SISTERS_TYPE_MEG) {
        this->megDistToPlayer = 110.0f;
        EnPoSisters_MatchPlayerXZ(this, play);
        this->color.a = 0;
        this->actor.draw = EnPoSisters_Draw;
    } else {
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    this->spinInvisibleTimer = 15;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_APPEAR);
    this->poSisterFlags &= ~POE_SISTERS_FLAG_CHECK_AC;
    this->actionFunc = EnPoSisters_SpinBackToVisible;
}

void EnPoSisters_SpinBackToVisible(EnPoSisters* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->color.a = 255; // fully visible
        if (this->type != POE_SISTERS_TYPE_MEG) {
            this->poSisterFlags |= POE_SISTERS_FLAG_CHECK_AC;
            this->collider.elem.acDmgInfo.dmgFlags = ~(0x8000000 | 0x200000 | 0x100000 | 0x40000 | 0x1);

            DECR(this->spinInvisibleTimer);

            if (this->spinInvisibleTimer == 0) {
                this->zTargetTimer = 20;
                EnPoSisters_SetupAimlessIdleFlying(this);
            }
        } else {
            EnPoSisters_SetupMegSurroundPlayer(this);
        }
    } else {
        s32 alpha = (this->skelAnime.curFrame * 255.0f) / this->skelAnime.endFrame;

        this->color.a = CLAMP(alpha, 0, 255);
        if (this->type == POE_SISTERS_TYPE_MEG) {
            EnPoSisters_MatchPlayerXZ(this, play);
        }
    }
}

void EnPoSisters_SetupDeathStage1(EnPoSisters* this) {
    this->deathTimer = 0;
    this->actor.speed = 0.0f;
    this->actor.world.pos.y += 42.0f;
    this->actor.shape.yOffset = -6000.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->poSisterFlags = 0;
    this->actionFunc = EnPoSisters_DeathStage1;
}

void EnPoSisters_DeathStage1(EnPoSisters* this, PlayState* play) {
    s32 i;
    s32 end = this->fireCount;

    this->deathTimer++;
    end++;
    if (end > ARRAY_COUNT(this->firePos)) {
        this->fireCount = 8;
    } else {
        this->fireCount = end;
    }

    for (end = this->fireCount - 1; end > 0; end--) {
        this->firePos[end] = this->firePos[end - 1];
    }

    this->firePos[0].x =
        this->actor.world.pos.x +
        (Math_SinS((this->actor.shape.rot.y + (this->deathTimer * 0x3000)) - 0x4000) * (3000.0f * this->actor.scale.x));
    this->firePos[0].z =
        this->actor.world.pos.z +
        (Math_CosS((this->actor.shape.rot.y + (this->deathTimer * 0x3000)) - 0x4000) * (3000.0f * this->actor.scale.x));

    if (this->deathTimer < 8) {
        this->firePos[0].y = this->firePos[1].y - 9.0f;
    } else {
        this->firePos[0].y = this->firePos[1].y + 2.0f;
        if (this->deathTimer >= 16) {
            if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.001f)) {
                EnPoSisters_SetupDeathStage2(this, play);
            }
            this->actor.scale.z = this->actor.scale.x;
            this->actor.scale.y = this->actor.scale.x;
        }
    }

    if (this->deathTimer == 16) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_DISAPPEAR);
    }
}

/**
 *  Fading away, the fire has split and circles outward and away.
 */
void EnPoSisters_SetupDeathStage2(EnPoSisters* this, PlayState* play) {
    this->deathTimer = 0;
    this->actor.world.pos.y = this->firePos[0].y;
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, (0x8 << 4)); // drop table 8
    this->actionFunc = EnPoSisters_DeathStage2;
}

void EnPoSisters_DeathStage2(EnPoSisters* this, PlayState* play) {
    this->deathTimer++;

    if (this->deathTimer == 32) {
        Actor_Kill(&this->actor);
        return;
    }

    EnPoSisters_UpdateDeathFlameSwirl(this, this->deathTimer, &this->actor.world.pos);
}

void EnPoSisters_SpawnMegClones(EnPoSisters* this, PlayState* play) {
    Actor* clone1 = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PO_SISTERS,
                                       this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0,
                                       0, POE_SISTERS_PARAMS(false, POE_SISTERS_MEG_CLONE1, POE_SISTERS_TYPE_MEG));
    Actor* clone2 = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PO_SISTERS,
                                       this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0,
                                       0, POE_SISTERS_PARAMS(false, POE_SISTERS_MEG_CLONE2, POE_SISTERS_TYPE_MEG));
    Actor* clone3 = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PO_SISTERS,
                                       this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0,
                                       0, POE_SISTERS_PARAMS(false, POE_SISTERS_MEG_CLONE3, POE_SISTERS_TYPE_MEG));

    // If we cannot spawn all clones: Abort
    if ((clone1 == NULL) || (clone2 == NULL) || (clone3 == NULL)) {
        if (clone1 != NULL) {
            Actor_Kill(clone1);
        }
        if (clone2 != NULL) {
            Actor_Kill(clone2);
        }
        if (clone3 != NULL) {
            Actor_Kill(clone3);
        }
        Actor_Kill(&this->actor);
    }
}

/**
 * Meg and her clones vanish instantly and surround the player, instead of slowing fading while spinning to get
 * Invulnerability.
 *
 * PlayState is an optional parameter, passed only when drawing the fire flash as they vanish.
 */
void EnPoSisters_MegCloneVanish(EnPoSisters* this, PlayState* play) {
    Vec3f pos;

    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->invisibleTimer = 100; // 5 seconds
    this->poSisterFlags = POE_SISTERS_FLAG_UPDATE_FIRES;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->collider.base.acFlags &= ~AC_HARD;

    if (play != NULL) {
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 45.0f;
        pos.z = this->actor.world.pos.z;
        func_800B3030(play, &pos, &gZeroVec3f, &gZeroVec3f, 150, 0, 3); // Spawns EffectSsDeadDb (flame effects)
    }

    Lights_PointSetColorAndRadius(&this->lightInfo, 0, 0, 0, 0); // Light OFF
    this->actionFunc = EnPoSisters_MegCloneWaitForSpinBack;
}

void EnPoSisters_MegCloneWaitForSpinBack(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnPoSisters* parent = (EnPoSisters*)this->actor.parent;

    if (this->megCloneId == POE_SISTERS_MEG_REAL) {
        DECR(this->invisibleTimer);
        if (this->invisibleTimer == 0) {
            s32 rand = Rand_ZeroFloat(4.0f);

            this->actor.shape.rot.y = (rand * 0x4000) + this->actor.yawTowardsPlayer;
            this->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
            EnPoSisters_SetupSpinBackToVisible(this, play);
        }

    } else if (parent->actionFunc == EnPoSisters_SpinBackToVisible) {
        this->actor.shape.rot.y = this->actor.parent->shape.rot.y + (this->megCloneId * 0x4000);
        this->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
        EnPoSisters_SetupSpinBackToVisible(this, play);

    } else if (parent->actionFunc == EnPoSisters_DeathStage1) {
        Actor_Kill(&this->actor);
    }
}

void EnPoSisters_SetupMegSurroundPlayer(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->color.a = 255;
    this->megSurroundTimer = 300; // 15 seconds
    this->megClonesRemaining = 3;
    this->poSisterFlags |= (POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT | POE_SISTERS_FLAG_CHECK_AC);
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnPoSisters_MegSurroundPlayer;
}

void EnPoSisters_MegSurroundPlayer(EnPoSisters* this, PlayState* play) {
    EnPoSisters* parent;

    DECR(this->megSurroundTimer);

    if ((this->megClonesRemaining > 0) && (this->megSurroundTimer >= 16)) {
        SkelAnime_Update(&this->skelAnime);
        if (this->megCloneId == POE_SISTERS_MEG_REAL) {
            if (ABS_ALT(16 - this->floatingBobbingTimer) < 14) {
                // Every N frames rotate around player. The fewer Meg clones remaining the faster they spin.
                this->actor.shape.rot.y += TRUNCF_BINANG((0x580 - (this->megClonesRemaining * 0x180)) *
                                                         fabsf(Math_SinS(this->floatingBobbingTimer * 0x800)));
            }

            // Twirl the real Meg backwards for a bit for a visual tell to player.
            if ((this->megSurroundTimer >= 284) || (this->megSurroundTimer <= 30)) {
                this->poSisterFlags |= POE_SISTERS_FLAG_REAL_MEG_ROTATION;
            } else {
                this->poSisterFlags &= ~POE_SISTERS_FLAG_REAL_MEG_ROTATION;
            }
        } else {
            this->actor.shape.rot.y = this->actor.parent->shape.rot.y + (this->megCloneId * 0x4000);
        }
    }

    if (this->megCloneId == POE_SISTERS_MEG_REAL) {
        if ((this->megSurroundTimer >= 284) || ((this->megSurroundTimer <= 30) && (this->megSurroundTimer >= 16))) {
            this->poSisterFlags |= POE_SISTERS_FLAG_REAL_MEG_ROTATION;
        } else {
            this->poSisterFlags &= ~POE_SISTERS_FLAG_REAL_MEG_ROTATION;
        }
    }

    if (this->megSurroundTimer == 0) {
        if (this->megCloneId == POE_SISTERS_MEG_REAL) {
            EnPoSisters_SetupSpinAttack(this);
        } else {
            EnPoSisters_MegCloneVanish(this, play);
        }
    } else if (this->megCloneId != POE_SISTERS_MEG_REAL) {
        parent = (EnPoSisters*)this->actor.parent;
        if (parent->actionFunc == EnPoSisters_DamageFlinch) {
            // Clones flinch if you hit the real Meg
            EnPoSisters_SetupDamageFlinch(this);
        }
    } else if (this->megClonesRemaining == 0) {
        // All Meg clones have been killed: Real Meg waits 15 frames then spin attacks in retaliation
        // Timer is negative (inrecrements to zero)
        //  because megClonesRemaining and megAttackTimer are the same union'd variable
        this->megAttackTimer = -15;
    } else if (this->megAttackTimer < 0) {
        this->megAttackTimer++;
        if (this->megAttackTimer == 0) {
            EnPoSisters_SetupSpinAttack(this);
        }
    }

    EnPoSisters_MatchPlayerXZ(this, play);
}

/**
 *  First ActionFunc for all Combat variants (including Meg)
 */
void EnPoSisters_SetupSpawnPo(EnPoSisters* this) {
    Animation_PlayOnce(&this->skelAnime, &gPoeSistersAppearDisappearAnim);
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_APPEAR);
    this->color.a = 0;
    this->poSisterFlags = POE_SISTERS_FLAG_UPDATE_FIRES;
    this->actionFunc = EnPoSisters_PoeSpawn;
}

void EnPoSisters_PoeSpawn(EnPoSisters* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->color.a = 255;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->poSisterFlags |= (POE_SISTERS_FLAG_UPDATE_BGCHECK_INFO | POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT);
        if (this->type == POE_SISTERS_TYPE_MEG) {
            EnPoSisters_MegCloneVanish(this, play);
        } else {
            EnPoSisters_SetupAimlessIdleFlying(this);
        }
    } else {
        f32 alphaPercent = this->skelAnime.curFrame / this->skelAnime.endFrame;
        s32 alpha = 255.0f * alphaPercent;

        this->color.a = CLAMP(alpha, 0, 255);
    }
}

void EnPoSisters_CheckCollision(EnPoSisters* this, PlayState* play) {
    Vec3f pos;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elem);

        if (this->megCloneId != POE_SISTERS_MEG_REAL) {
            ((EnPoSisters*)this->actor.parent)->megClonesRemaining--;
            Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH2);
            EnPoSisters_MegCloneVanish(this, play);
            if (Rand_ZeroOne() < 0.2f) {
                pos.x = this->actor.world.pos.x;
                pos.y = this->actor.world.pos.y;
                pos.z = this->actor.world.pos.z;
                Item_DropCollectible(play, &pos, ITEM00_ARROWS_10);
            }
        } else if (this->collider.base.colMaterial != COL_MATERIAL_METAL) {
            if (this->actor.colChkInfo.damageEffect == POE_SISTERS_DMGEFF_DEKUNUT) {
                this->actor.world.rot.y = this->actor.shape.rot.y;
                this->poSisterFlags |= POE_SISTERS_FLAG_UPDATE_SHAPE_ROT;
                EnPoSisters_SetupSpinBackToVisible(this, play);
            } else if ((this->type == POE_SISTERS_TYPE_MEG) &&
                       (this->actor.colChkInfo.damageEffect == POE_SISTERS_DMGEFF_SPINATTACK) &&
                       (this->actionFunc == EnPoSisters_MegSurroundPlayer)) {
                if (this->megClonesRemaining == 0) {
                    // All Meg clones have been killed: Real Meg waits 45 frames then spin attacks
                    // Timer is negative because megClonesRemaining and megAttackTimer are the same union'd variable
                    this->megAttackTimer = -45;
                }
            } else {
                if (Actor_ApplyDamage(&this->actor)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_PO_DAMAGE);
                } else {
                    Enemy_StartFinishingBlow(play, &this->actor);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_PO_SISTER_DEAD);
                }

                if (this->actor.colChkInfo.damageEffect == POE_SISTERS_DMGEFF_LIGHTARROWS) {
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffScale = 0.5f;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                                this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                }
                EnPoSisters_SetupDamageFlinch(this);
            }
        }
    }
}

void EnPoSisters_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPoSisters* this = (EnPoSisters*)thisx;
    f32 alpha;
    Vec3f checkPos;
    s32 bgId;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        EnPoSisters_SetupAttackConnect(this);
    }

    EnPoSisters_CheckCollision(this, play);
    if (this->poSisterFlags & POE_SISTERS_FLAG_CHECK_Z_TARGET) {
        EnPoSisters_CheckZTarget(this, play);
    }

    this->actionFunc(this, play);

    if (this->poSisterFlags & POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT) {
        EnPoSisters_MatchPlayerY(this, play);
    }

    Actor_MoveWithGravity(&this->actor);

    if (this->poSisterFlags & POE_SISTERS_FLAG_UPDATE_BGCHECK_INFO) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    } else {
        checkPos.x = this->actor.world.pos.x;
        checkPos.y = this->actor.world.pos.y + 10.0f;
        checkPos.z = this->actor.world.pos.z;
        this->actor.floorHeight =
            BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &checkPos);
    }

    this->actor.shape.shadowAlpha = this->color.a;
    Actor_SetFocus(&this->actor, 40.0f);

    if (this->drawDmgEffAlpha > 0.0f) {
        Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
        if (this->color.a != 255) {
            alpha = this->color.a * (1.0f / 255);
            if (alpha < this->color.a) {
                this->drawDmgEffAlpha = alpha;
            }
        }

        this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.25f;
        this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.5f);
    }

    if (this->poSisterFlags &
        (POE_SISTERS_FLAG_UPDATE_BGCHECK_INFO | POE_SISTERS_FLAG_MATCH_PLAYER_HEIGHT | POE_SISTERS_FLAG_CHECK_Z_TARGET |
         POE_SISTERS_FLAG_UPDATE_SHAPE_ROT | POE_SISTERS_FLAG_CHECK_AC)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        if ((this->actionFunc == EnPoSisters_SpinAttack) || (this->actionFunc == EnPoSisters_SpinUp)) {
            this->fireCount++;
            this->fireCount = CLAMP_MAX(this->fireCount, ARRAY_COUNT(this->firePos));
        } else if (this->actionFunc != EnPoSisters_DeathStage1) {
            this->fireCount = CLAMP_MIN(this->fireCount - 1, 1);
        }

        if (this->actionFunc == EnPoSisters_SpinAttack) {
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->poSisterFlags & POE_SISTERS_FLAG_CHECK_AC) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->actionFunc != EnPoSisters_MegCloneWaitForSpinBack) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->actionFunc == EnPoSisters_Flee) {
            this->actor.shape.rot.y = BINANG_ROT180(this->actor.world.rot.y);
        } else if (this->poSisterFlags & POE_SISTERS_FLAG_UPDATE_SHAPE_ROT) {
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }
    }
}

void EnPoSisters_UpdateColors(EnPoSisters* this) {
    if (this->skelAnime.animation == &gPoeSistersAttackAnim) {
        this->color.r = CLAMP_MAX(this->color.r + 5, 255);
        this->color.g = CLAMP_MIN(this->color.g - 5, 50);
        this->color.b = CLAMP_MIN(this->color.b - 5, 0);
    } else if (this->skelAnime.animation == &gPoeSistersFleeAnim) {
        this->color.r = CLAMP_MAX(this->color.r + 5, 80);
        this->color.g = CLAMP_MAX(this->color.g + 5, 255);
        this->color.b = CLAMP_MAX(this->color.b + 5, 225);
    } else if (this->skelAnime.animation == &gPoeSistersDamagedAnim) {
        // Flash every other frame after taking damage
        if (this->actor.colorFilterTimer & 0x2) {
            this->color.r = 0;
            this->color.g = 0;
            this->color.b = 0;
        } else {
            this->color.r = 80;
            this->color.g = 255;
            this->color.b = 225;
        }
    } else {
        this->color.r = CLAMP_MAX(this->color.r + 5, 255);
        this->color.g = CLAMP_MAX(this->color.g + 5, 255);

        if (this->color.b > 210) {
            //! FAKE:
            if (this->color.b && this->color.b && this->color.b) {}
            this->color.b = CLAMP_MIN(this->color.b - 5, 210);
        } else {
            this->color.b = CLAMP_MAX(this->color.b + 5, 210);
        }
    }
}

s32 EnPoSisters_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                                 Gfx** gfx) {
    static Gfx* gPoSisterBodyDisplayLists[] = {
        gPoeSistersMegBodyDL,
        gPoeSistersJoelleBodyDL,
        gPoeSistersBethBodyDL,
        gPoeSistersAmyBodyDL,
    };
    static Gfx* gPoSisterFaceDisplayLists[] = {
        gPoeSistersMegFaceDL,
        gPoeSistersJoelleFaceDL,
        gPoeSistersBethFaceDL,
        gPoeSistersAmyFaceDL,
    };
    static Color_RGBA8 gPoSisterColors[] = {
        { 80, 0, 100, 0 },
        { 80, 15, 0, 0 },
        { 0, 70, 50, 0 },
        { 70, 70, 0, 0 },
    };
    EnPoSisters* this = (EnPoSisters*)thisx;

    if ((limbIndex == POE_SISTERS_LIMB_ROOT) && (this->poSisterFlags & POE_SISTERS_FLAG_REAL_MEG_ROTATION)) {
        if (this->megSurroundTimer >= 284) {
            rot->x += (this->megSurroundTimer - 284) * 0x1000;
        } else {
            rot->x += (this->megSurroundTimer - 15) * 0x1000;
        }
    }

    if ((this->color.a == 0) || (limbIndex == POE_SISTERS_LIMB_TORCH) ||
        ((this->actionFunc == EnPoSisters_DeathStage1) && (this->deathTimer >= 8))) {
        *dList = NULL;
    } else if (limbIndex == POE_SISTERS_LIMB_MAIN_BODY) {
        *dList = gPoSisterBodyDisplayLists[this->type];
    } else if (limbIndex == POE_SISTERS_LIMB_FACE) {
        *dList = gPoSisterFaceDisplayLists[this->type];

        gDPPipeSync((*gfx)++);
        gDPSetEnvColor((*gfx)++, this->color.r, this->color.g, this->color.b, this->color.a);
    } else if (limbIndex == POE_SISTERS_LIMB_LOWER_BODY) {
        Color_RGBA8* color = &gPoSisterColors[this->type];

        gDPPipeSync((*gfx)++);
        gDPSetEnvColor((*gfx)++, color->r, color->g, color->b, this->color.a);
    }

    return false;
}

static Vec3f D_80B1DAFC = { 1000.0f, -1700.0f, 0.0f };

static s8 sLimbToBodyParts[POE_SISTERS_LIMB_MAX] = {
    BODYPART_NONE,                        // POE_SISTERS_LIMB_NONE
    BODYPART_NONE,                        // POE_SISTERS_LIMB_ROOT
    POE_SISTERS_BODYPART_LEFT_ARM,        // POE_SISTERS_LIMB_LEFT_ARM
    POE_SISTERS_BODYPART_LEFT_HAND,       // POE_SISTERS_LIMB_LEFT_HAND
    POE_SISTERS_BODYPART_RIGHT_UPPER_ARM, // POE_SISTERS_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                        // POE_SISTERS_LIMB_RIGHT_FOREARM
    POE_SISTERS_BODYPART_TORCH_ROOT,      // POE_SISTERS_LIMB_TORCH_ROOT
    BODYPART_NONE,                        // POE_SISTERS_LIMB_RIGHT_HAND
    BODYPART_NONE,                        // POE_SISTERS_LIMB_TORCH
    BODYPART_NONE,                        // POE_SISTERS_LIMB_MAIN_BODY
    BODYPART_NONE,                        // POE_SISTERS_LIMB_FACE
    BODYPART_NONE,                        // POE_SISTERS_LIMB_LOWER_BODY
};

void EnPoSisters_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    s32 end;
    f32 brightness;

    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    } else if (limbIndex == POE_SISTERS_LIMB_MAIN_BODY) {
        Matrix_MultVecY(-2500.0f, &this->bodyPartsPos[POE_SISTERS_BODYPART_MAIN_BODY_0]);
        Matrix_MultVecY(3000.0f, &this->bodyPartsPos[POE_SISTERS_BODYPART_MAIN_BODY_1]);
    } else if (limbIndex == POE_SISTERS_LIMB_FACE) {
        Matrix_MultVecY(-4000.0f, &this->bodyPartsPos[POE_SISTERS_BODYPART_FACE]);
    } else if (limbIndex == POE_SISTERS_LIMB_LOWER_BODY) {
        Matrix_MultVecX(3000.0f, &this->bodyPartsPos[POE_SISTERS_BODYPART_LOWER_BODY]);
    }

    if ((this->actionFunc == EnPoSisters_DeathStage1) && (this->deathTimer >= 8) &&
        (limbIndex == POE_SISTERS_LIMB_MAIN_BODY)) {
        MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx);
        gSPDisplayList((*gfx)++, gPoeSistersBurnBodyDL);
    }

    if (limbIndex == POE_SISTERS_LIMB_TORCH) {
        if (this->poSisterFlags & POE_SISTERS_FLAG_UPDATE_FIRES) {
            for (end = this->fireCount - 1; end > 0; end--) {
                this->firePos[end] = this->firePos[end - 1];
            }

            Matrix_MultVec3f(&D_80B1DAFC, this->firePos);
        }

        if (this->fireCount > 0) {
            Color_RGBA8* flameColor = &sPoSisterFlameColors[this->type];

            brightness = Rand_ZeroFloat(0.3f) + 0.7f; // Flickering torch light level

            if (this->actionFunc == EnPoSisters_DeathStage2) {
                Lights_PointNoGlowSetInfo(&this->lightInfo, this->firePos[0].x, this->firePos[0].y + 15.0f,
                                          this->firePos[0].z, flameColor->r * brightness, flameColor->g * brightness,
                                          flameColor->b * brightness, 200);
            } else {
                Lights_PointGlowSetInfo(&this->lightInfo, this->firePos[0].x, this->firePos[0].y + 15.0f,
                                        this->firePos[0].z, flameColor->r * brightness, flameColor->g * brightness,
                                        flameColor->b * brightness, 200);
            }
        } else {
            Lights_PointSetColorAndRadius(&this->lightInfo, 0, 0, 0, 0);
        }

        if (!(this->poSisterFlags & POE_SISTERS_FLAG_DRAW_TORCH)) {
            Matrix_Get(&this->mtxf);
        }
    }
}

void EnPoSisters_Draw(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    Color_RGBA8* sisterEnvColor = &sPoSisterEnvColors[this->type];
    Color_RGBA8* flameColor = &sPoSisterFlameColors[this->type];
    s32 pad;
    s32 i;
    u8 alpha;
    f32 scale;
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx);

    EnPoSisters_UpdateColors(this);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if ((this->color.a == 255) || (this->color.a == 0)) {
        gDPSetEnvColor(POLY_OPA_DISP++, this->color.r, this->color.g, this->color.b, this->color.a);
        gSPSegment(POLY_OPA_DISP++, 0x09, D_801AEFA0); // Empty DL
        POLY_OPA_DISP =
            SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnPoSisters_OverrideLimbDraw,
                           EnPoSisters_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->color.a);
        gSPSegment(POLY_XLU_DISP++, 0x09, D_801AEF88); // XLU only DL
        POLY_XLU_DISP =
            SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnPoSisters_OverrideLimbDraw,
                           EnPoSisters_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }

    if (!(this->poSisterFlags & POE_SISTERS_FLAG_DRAW_TORCH)) {
        Matrix_Put(&this->mtxf);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gPoeSistersTorchDL);
    }

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (play->gameplayFrames * -20) % 512, 32,
                                  128, 0, 0, 0, -20));
    gDPSetEnvColor(POLY_XLU_DISP++, sisterEnvColor->r, sisterEnvColor->g, sisterEnvColor->b, sisterEnvColor->a);

    if (this->actionFunc == EnPoSisters_DeathStage2) {
        alpha = ((-this->deathTimer * 255) + 0x1FE0) / 32;
        scale = (7 / 1.25f * 0.001f);
    } else {
        alpha = 0;
        scale = this->actor.scale.x * 0.5f;
    }

    for (i = 0; i < this->fireCount; i++) {
        if (this->actionFunc != EnPoSisters_DeathStage2) {
            alpha = (-i * 31) + 248;
        }

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, flameColor->r, flameColor->g, flameColor->b, alpha);

        Matrix_Translate(this->firePos[i].x, this->firePos[i].y, this->firePos[i].z, MTXMODE_NEW);
        Matrix_RotateZYX(0, BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))), 0, MTXMODE_APPLY);

        if (this->actionFunc == EnPoSisters_DeathStage1) {
            scale = ((this->deathTimer - i) * 0.025f) + 0.5f;
            scale = CLAMP(scale, 0.5f, 0.8f) * 0.007f;
        }
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, POE_SISTERS_BODYPART_MAX,
                            this->actor.scale.x * (1.0f / 0.007f) * this->drawDmgEffScale, 0.0f, this->drawDmgEffAlpha,
                            ACTOR_DRAW_DMGEFF_LIGHT_ORBS);

    CLOSE_DISPS(play->state.gfxCtx);
}

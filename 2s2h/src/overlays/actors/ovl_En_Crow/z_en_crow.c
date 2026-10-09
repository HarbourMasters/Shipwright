/*
 * File: z_en_crow.c
 * Overlay: ovl_En_Crow
 * Description: Guay
 */

#include "z_en_crow.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void EnCrow_Init(Actor* thisx, PlayState* play);
void EnCrow_Destroy(Actor* thisx, PlayState* play);
void EnCrow_Update(Actor* thisx, PlayState* play);
void EnCrow_Draw(Actor* thisx, PlayState* play);

void EnCrow_SetupFlyIdle(EnCrow* this);
void EnCrow_FlyIdle(EnCrow* this, PlayState* play);
void EnCrow_SetupDiveAttack(EnCrow* this);
void EnCrow_DiveAttack(EnCrow* this, PlayState* play);
void EnCrow_CheckIfFrozen(EnCrow* this, PlayState* play);
void EnCrow_Damaged(EnCrow* this, PlayState* play);
void EnCrow_SetupDie(EnCrow* this);
void EnCrow_Die(EnCrow* this, PlayState* play);
void EnCrow_SetupRespawn(EnCrow* this);
void EnCrow_TurnAway(EnCrow* this, PlayState* play);
void EnCrow_Respawn(EnCrow* this, PlayState* play);

ActorProfile En_Crow_Profile = {
    /**/ ACTOR_EN_CROW,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_CROW,
    /**/ sizeof(EnCrow),
    /**/ EnCrow_Init,
    /**/ EnCrow_Destroy,
    /**/ EnCrow_Update,
    /**/ EnCrow_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 30, 30 };

typedef enum {
    /* 0 */ GUAY_DMGEFF_NONE,
    /* 1 */ GUAY_DMGEFF_STUN,
    /* 2 */ GUAY_DMGEFF_FIRE,
    /* 3 */ GUAY_DMGEFF_ICE,
    /* 4 */ GUAY_DMGEFF_LIGHT,
    /* 5 */ GUAY_DMGEFF_ELECTRIC
} GuayDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, GUAY_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, GUAY_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, GUAY_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, GUAY_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, GUAY_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, GUAY_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, GUAY_DMGEFF_ELECTRIC),
    /* Normal shield  */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, GUAY_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, GUAY_DMGEFF_NONE),
};

#define GUAY_NUMBER_OF_DEAD_TO_SPAWN_MEGAGUAY 10

static s32 sDeadCount = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),
    ICHAIN_S8(hintId, TATL_HINT_ID_GUAY, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

void EnCrow_Init(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGuaySkel, &gGuayFlyAnim, this->jointTable, this->morphTable,
                       OBJECT_CROW_LIMB_MAX);
    Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 2000.0f, ActorShadow_DrawCircle, 20.0f);

    sDeadCount = 0;

    if (this->actor.parent != NULL) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
    EnCrow_SetupFlyIdle(this);
}

void EnCrow_Destroy(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnCrow_SetupFlyIdle(EnCrow* this) {
    this->timer = 100;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = EnCrow_FlyIdle;
    this->skelAnime.playSpeed = 1.0f;
}

void EnCrow_FlyIdle(EnCrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 dist;
    s32 onAnimFirstFrame;
    s16 yaw;

    SkelAnime_Update(&this->skelAnime);
    onAnimFirstFrame = Animation_OnFrame(&this->skelAnime, 0.0f);
    this->actor.speed = (Rand_ZeroOne() * 1.5f) + 3.0f;

    if ((this->actor.parent != NULL) && (this->actor.parent->home.rot.z == 0)) {
        this->actor.home.pos.x = this->actor.parent->world.pos.x;
        this->actor.home.pos.z = this->actor.parent->world.pos.z;
        dist = Actor_WorldDistXZToPoint(&this->actor, &this->actor.parent->world.pos);
    } else {
        dist = 450.0f;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->yawTarget = this->actor.wallYaw;
    } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 300.0f) {
        this->yawTarget = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    }

    if ((Math_SmoothStepToS(&this->actor.shape.rot.y, this->yawTarget, 5, 0x300, 0x10) == 0) && onAnimFirstFrame &&
        (Rand_ZeroOne() < 0.1f)) {

        yaw = (Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos) - this->actor.shape.rot.y);
        if (yaw > 0) {
            this->yawTarget += Rand_S16Offset(0x1000, 0x1000);
        } else {
            this->yawTarget -= Rand_S16Offset(0x1000, 0x1000);
        }
        Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_CRY);
    }

    if ((this->actor.depthInWater > -40.0f) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->pitchTarget = -0x1000;
    } else if (this->actor.world.pos.y < (this->actor.home.pos.y - 50.0f)) {
        this->pitchTarget = -Rand_S16Offset(0x800, 0x800);
    } else if (this->actor.world.pos.y > (this->actor.home.pos.y + 50.0f)) {
        this->pitchTarget = Rand_S16Offset(0x800, 0x800);
    }

    if (!Math_SmoothStepToS(&this->actor.shape.rot.x, this->pitchTarget, 0xA, 0x100, 8) && onAnimFirstFrame &&
        (Rand_ZeroOne() < 0.1f)) {
        if (this->actor.home.pos.y < this->actor.world.pos.y) {
            this->pitchTarget -= Rand_S16Offset(0x400, 0x400);
        } else {
            this->pitchTarget += Rand_S16Offset(0x400, 0x400);
        }

        this->pitchTarget = CLAMP(this->pitchTarget, -0x1000, 0x1000);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x100, 0x400);
    }

    if (this->timer != 0) {
        this->timer--;
    }
    if ((this->timer == 0) &&
        (((this->actor.xzDistToPlayer < 300.0f) && !(player->stateFlags1 & PLAYER_STATE1_800000)) || (dist < 300.0f)) &&
        (this->actor.depthInWater < -40.0f) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
        if (dist < this->actor.xzDistToPlayer) {
            this->actor.child = this->actor.parent;
        } else {
            this->actor.child = &player->actor;
        }
        EnCrow_SetupDiveAttack(this);
    }
}

void EnCrow_SetupDiveAttack(EnCrow* this) {
    this->timer = 300;
    this->actionFunc = EnCrow_DiveAttack;
    this->actor.speed = 4.0f;
    this->skelAnime.playSpeed = 2.0f;
}

void EnCrow_DiveAttack(EnCrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 isFacingActor;
    Vec3f targetPos;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }
    isFacingActor = Actor_ActorAIsFacingActorB(&this->actor, this->actor.child, 0x2800);

    if (isFacingActor) {
        s16 pitchTarget;

        if (&player->actor == this->actor.child) {
            targetPos.y = this->actor.child->world.pos.y + 20.0f;
        } else {
            targetPos.y = this->actor.child->world.pos.y + 40.0f;
        }
        targetPos.x = this->actor.child->world.pos.x;
        targetPos.z = this->actor.child->world.pos.z;
        pitchTarget = Actor_WorldPitchTowardPoint(&this->actor, &targetPos);
        pitchTarget = CLAMP(pitchTarget, -0x3000, 0x3000);
        Math_SmoothStepToS(&this->actor.shape.rot.x, pitchTarget, 2, 0x400, 0x40);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.x, -0x800, 2, 0x100, 0x10);
    }

    if (isFacingActor || (Actor_WorldDistXZToActor(&this->actor, this->actor.child) > 80.0f)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, this->actor.child), 4,
                           0xC00, 0xC0);
    }

    if (((this->timer == 0) || ((&player->actor != this->actor.child) && (this->actor.child->home.rot.z != 0)) ||
         ((&player->actor == this->actor.child) &&
          ((Player_GetMask(play) == PLAYER_MASK_STONE) || (player->stateFlags1 & PLAYER_STATE1_800000))) ||
         ((this->collider.base.atFlags & AT_HIT) ||
          (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL)))) ||
        (this->actor.depthInWater > -40.0f)) {

        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_ATTACK);
        }
        EnCrow_SetupFlyIdle(this);
    }
}

void EnCrow_CheckIfFrozen(EnCrow* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, GUAY_BODYPART_MAX, 2, 0.2f, 0.2f);
    }
}

void EnCrow_SetupDamaged(EnCrow* this, PlayState* play) {
    f32 scale;

    this->actor.speed *= Math_CosS(this->actor.world.rot.x);
    this->actor.velocity.y = 0.0f;
    Animation_Change(&this->skelAnime, &gGuayFlyAnim, 0.4f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    this->actor.shape.yOffset = 0.0f;
    this->actor.lockOnArrowOffset = 0.0f;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    scale = (this->actor.scale.x * 100.0f);
    this->actor.world.pos.y += 20.0f * scale;
    Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_DEAD);

    if (this->actor.colChkInfo.damageEffect == GUAY_DMGEFF_ICE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        this->drawDmgEffAlpha = 1.0f;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffFrozenSteamScale = 0.5f;
    } else if (this->actor.colChkInfo.damageEffect == GUAY_DMGEFF_LIGHT) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffFrozenSteamScale = 0.5f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elements[0].base.acDmgInfo.hitPos.x,
                    this->collider.elements[0].base.acDmgInfo.hitPos.y,
                    this->collider.elements[0].base.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
    } else if (this->actor.colChkInfo.damageEffect == GUAY_DMGEFF_FIRE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffFrozenSteamScale = 0.5f;
    }

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        this->actor.speed = 0.0f;
    }

    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

    this->actionFunc = EnCrow_Damaged;
}

void EnCrow_Damaged(EnCrow* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    this->actor.colorFilterTimer = 40;

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4000, 0x200);
            this->actor.shape.rot.z += 0x1780;
        }
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
            EnCrow_CheckIfFrozen(this, play);
            func_800B3030(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, this->actor.scale.x * 10000.0f, 0, 0);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 11, NA_SE_EN_EXTINCT);

            if (this->actor.parent != NULL) {
                Actor_Kill(&this->actor);
                return;
            }
            EnCrow_SetupDie(this);
        }
    }
}

void EnCrow_SetupDie(EnCrow* this) {
    this->actor.colorFilterTimer = 0;
    this->actionFunc = EnCrow_Die;
}

void EnCrow_Die(EnCrow* this, PlayState* play) {
    f32 stepScale;

    if (this->actor.params != GUAY_TYPE_NORMAL) {
        stepScale = 0.006f;
    } else {
        stepScale = 0.002f;
    }
    if (Math_StepToF(&this->actor.scale.x, 0.0f, stepScale)) {
        if (this->actor.params == GUAY_TYPE_NORMAL) {
            sDeadCount++;
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x80);
        } else {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
        }
        EnCrow_SetupRespawn(this);
    }
    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
}

void EnCrow_SetupTurnAway(EnCrow* this) {
    this->timer = 100;
    this->pitchTarget = -0x1000;
    this->actor.speed = 3.5f;
    this->yawTarget = this->actor.yawTowardsPlayer + 0x8000;
    this->skelAnime.playSpeed = 2.0f;
    if (this->actor.colChkInfo.damageEffect == GUAY_DMGEFF_STUN) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
    } else {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = EnCrow_TurnAway;
}

void EnCrow_TurnAway(EnCrow* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->yawTarget = this->actor.wallYaw;
    } else {
        this->yawTarget = this->actor.yawTowardsPlayer + 0x8000;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->yawTarget, 3, 0xC00, 0xC0);
    Math_SmoothStepToS(&this->actor.shape.rot.x, this->pitchTarget, 5, 0x100, 0x10);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        EnCrow_SetupFlyIdle(this);
    }
}

void EnCrow_SetupRespawn(EnCrow* this) {
    if (sDeadCount == GUAY_NUMBER_OF_DEAD_TO_SPAWN_MEGAGUAY) {
        this->actor.params = GUAY_TYPE_MEGA;
        sDeadCount = 0;
        this->collider.elements[0].dim.worldSphere.radius =
            sJntSphInit.elements[0].dim.modelSphere.radius * 0.03f * 100.0f;
    } else {
        this->actor.params = GUAY_TYPE_NORMAL;
        this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    }
    Animation_PlayLoop(&this->skelAnime, &gGuayFlyAnim);
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->timer = 300;
    this->actor.draw = NULL;
    this->actor.shape.yOffset = 2000.0f;
    this->actor.lockOnArrowOffset = 2000.0;
    this->drawDmgEffAlpha = 0.0f;
    this->actionFunc = EnCrow_Respawn;
}

void EnCrow_Respawn(EnCrow* this, PlayState* play) {
    f32 scaleTarget;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        SkelAnime_Update(&this->skelAnime);
        this->actor.draw = EnCrow_Draw;
        if (this->actor.params != GUAY_TYPE_NORMAL) {
            scaleTarget = 0.03f;
        } else {
            scaleTarget = 0.01f;
        }
        if (Math_StepToF(&this->actor.scale.x, scaleTarget, scaleTarget * 0.1f)) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.colChkInfo.health = 1;
            EnCrow_SetupFlyIdle(this);
        }
        this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    }
}

void EnCrow_UpdateDamage(EnCrow* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elements[0].base);

        if (this->actor.colChkInfo.damageEffect == GUAY_DMGEFF_STUN) {
            EnCrow_SetupTurnAway(this);

        } else if (this->actor.colChkInfo.damageEffect == GUAY_DMGEFF_ELECTRIC) {
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM;
            this->drawDmgEffAlpha = 2.0f;
            this->drawDmgEffFrozenSteamScale = 0.5f;
            EnCrow_SetupTurnAway(this);

        } else {
            this->actor.colChkInfo.health = 0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Enemy_StartFinishingBlow(play, &this->actor);
            EnCrow_SetupDamaged(this, play);
        }
    }
}

void EnCrow_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnCrow* this = (EnCrow*)thisx;
    f32 height;
    f32 scale;

    EnCrow_UpdateDamage(this, play);
    this->actionFunc(this, play);
    scale = this->actor.scale.x * 100.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.world.rot.x = -this->actor.shape.rot.x;

    if (this->actionFunc != EnCrow_Respawn) {
        if (this->actor.colChkInfo.health != 0) {
            height = 20.0f * scale;
            Actor_MoveWithoutGravity(&this->actor);
        } else {
            height = 0.0f;
            Actor_MoveWithGravity(&this->actor);
        }
        Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f * scale, 25.0f * scale, 50.0f * scale,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
    } else {
        height = 0.0f;
    }

    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y + height;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    if (this->actionFunc == EnCrow_DiveAttack) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
    if (this->actionFunc != EnCrow_Respawn) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    Actor_SetFocus(&this->actor, height);

    if ((this->actor.colChkInfo.health != 0) && Animation_OnFrame(&this->skelAnime, 3.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_FLUTTER);
    }
    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffFrozenSteamScale = (this->drawDmgEffAlpha + 1.0f) * 0.25f;
            this->drawDmgEffFrozenSteamScale = CLAMP_MAX(this->drawDmgEffFrozenSteamScale, 0.5f);
        } else if (!Math_StepToF(&this->drawDmgEffScale, 0.5f, 0.5f * 0.025f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

s32 EnCrow_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnCrow* this = (EnCrow*)thisx;

    if (this->actor.colChkInfo.health != 0) {
        if (limbIndex == OBJECT_CROW_LIMB_UPPER_TAIL) {
            rot->y += TRUNCF_BINANG(0xC00 * Math_SinF(this->skelAnime.curFrame * (M_PIf / 4)));
        } else if (limbIndex == OBJECT_CROW_LIMB_TAIL) {
            rot->y += TRUNCF_BINANG(0x1400 * Math_SinF((this->skelAnime.curFrame + 2.5f) * (M_PIf / 4)));
        }
    }
    return false;
}

void EnCrow_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnCrow* this = (EnCrow*)thisx;

    if (limbIndex == OBJECT_CROW_LIMB_BODY) {
        Matrix_MultVecX(2500.0f, &this->bodyPartsPos[GUAY_BODYPART_BODY]);
    } else if ((limbIndex == OBJECT_CROW_LIMB_RIGHT_WING_TIP) || (limbIndex == OBJECT_CROW_LIMB_LEFT_WING_TIP) ||
               (limbIndex == OBJECT_CROW_LIMB_TAIL)) {
        Matrix_MultZero(&this->bodyPartsPos[(limbIndex >> 1) - 1]);
    }
}

void EnCrow_Draw(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnCrow_OverrideLimbDraw, EnCrow_PostLimbDraw, &this->actor);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, GUAY_BODYPART_MAX,
                            this->actor.scale.x * 100.0f * this->drawDmgEffFrozenSteamScale, this->drawDmgEffScale,
                            this->drawDmgEffAlpha, this->drawDmgEffType);
}

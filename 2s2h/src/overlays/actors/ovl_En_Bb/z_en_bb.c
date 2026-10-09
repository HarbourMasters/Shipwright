/*
 * File: z_en_bb.c
 * Overlay: ovl_En_Bb
 * Description: Blue Bubble
 */

#include "z_en_bb.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void EnBb_Init(Actor* thisx, PlayState* play);
void EnBb_Destroy(Actor* thisx, PlayState* play);
void EnBb_Update(Actor* thisx, PlayState* play);
void EnBb_Draw(Actor* thisx, PlayState* play);

void EnBb_SetupFlyIdle(EnBb* this);
void EnBb_FlyIdle(EnBb* this, PlayState* play);
void EnBb_SetupAttack(EnBb* this);
void EnBb_Attack(EnBb* this, PlayState* play);
void EnBb_Down(EnBb* this, PlayState* play);
void EnBb_Dead(EnBb* this, PlayState* play);
void EnBb_Damage(EnBb* this, PlayState* play);
void EnBb_Frozen(EnBb* this, PlayState* play);
void EnBb_SetupWaitForRevive(EnBb* this);
void EnBb_WaitForRevive(EnBb* this, PlayState* play);
void EnBb_SetupRevive(EnBb* this);
void EnBb_Revive(EnBb* this, PlayState* play);

typedef enum {
    /* -1 */ BB_BODY_PART_DRAW_STATUS_BROKEN = -1,
    /*  0 */ BB_BODY_PART_DRAW_STATUS_ALIVE,
    /*  1 */ BB_BODY_PART_DRAW_STATUS_DEAD
} EnBbBodyPartDrawStatus;

ActorProfile En_Bb_Profile = {
    /**/ ACTOR_EN_BB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BB,
    /**/ sizeof(EnBb),
    /**/ EnBb_Init,
    /**/ EnBb_Destroy,
    /**/ EnBb_Update,
    /**/ EnBb_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x08 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 20 }, 100 },
};

typedef enum {
    /* 0x0 */ EN_BB_DMGEFF_NONE,
    /* 0x1 */ EN_BB_DMGEFF_STUN,
    /* 0x3 */ EN_BB_DMGEFF_ICE_ARROW = 0x3,
    /* 0x4 */ EN_BB_DMGEFF_LIGHT_ARROW,
    /* 0x5 */ EN_BB_DMGEFF_ZORA_MAGIC,
    /* 0xE */ EN_BB_DMGEFF_HOOKSHOT = 0xE
} EnBbDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_BB_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, EN_BB_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Ice arrow      */ DMG_ENTRY(2, EN_BB_DMGEFF_ICE_ARROW),
    /* Light arrow    */ DMG_ENTRY(2, EN_BB_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EN_BB_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_BB_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, EN_BB_DMGEFF_ZORA_MAGIC),
    /* Normal shield  */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_BB_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_BB_DMGEFF_NONE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 20, 40, 50 };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_BLUE_BUBBLE, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 10, ICHAIN_STOP),
};

void EnBb_Init(Actor* thisx, PlayState* play) {
    EnBb* this = (EnBb*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_Init(play, &this->skelAnime, &gBubbleSkel, &gBubbleFlyingAnim, this->jointTable, this->morphTable,
                   BUBBLE_LIMB_MAX);
    Collider_InitAndSetSphere(play, &this->collider, &this->actor, &sSphereInit);
    ActorShape_Init(&this->actor.shape, 1500.0f, ActorShadow_DrawCircle, 35.0f);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->flameScaleY = 0.8f;
    this->flameScaleX = 1.0f;
    this->actor.world.pos.y += 50.0f;

    if (EN_BB_ATTACK_RANGE(&this->actor) == 0xFF) {
        this->attackRange = 200.0f;
    } else {
        this->attackRange = EN_BB_ATTACK_RANGE(&this->actor) * 4.0f;
    }

    EnBb_SetupFlyIdle(this);
}

void EnBb_Destroy(Actor* thisx, PlayState* play) {
    EnBb* this = (EnBb*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

/**
 * Checks to see if the Bubble is touching a wall. If it is, and if the
 * Bubble is facing directly "into" the wall, then rotate it away from
 * the wall.
 */
void EnBb_CheckForWall(EnBb* this) {
    s16 yawDiff;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        yawDiff = this->actor.shape.rot.y - this->actor.wallYaw;
        if (ABS_ALT(yawDiff) > 0x4000) {
            this->actor.shape.rot.y = ((this->actor.wallYaw * 2) - this->actor.shape.rot.y) - 0x8000;
        }

        this->targetYRotation = this->actor.shape.rot.y;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    }
}

void EnBb_Freeze(EnBb* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.4f;
    this->drawDmgEffFrozenSteamScale = 0.6f;
    this->timer = 80;
    this->drawDmgEffAlpha = 1.0f;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void EnBb_Thaw(EnBb* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, BUBBLE_BODYPART_MAX, 2, 0.2f, 0.15f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    }
}

/**
 * Updates certain variables that relate to flying like the Bubble's
 * Y-position, bob phase, and flame scale.
 */
void EnBb_UpdateStateForFlying(EnBb* this) {
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.floorHeight > BGCHECK_Y_MIN) {
        Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight + this->flyHeightMod, 0.5f);
    }

    this->actor.world.pos.y += Math_CosS(this->bobPhase);
    this->bobPhase += 0x826;
    Math_StepToF(&this->flameScaleY, 0.8f, 0.1f);
    Math_StepToF(&this->flameScaleX, 1.0f, 0.1f);
    EnBb_CheckForWall(this);
    Math_StepToF(&this->actor.speed, this->maxSpeed, 0.5f);
    Math_ApproachS(&this->actor.shape.rot.y, this->targetYRotation, 5, 0x3E8);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnBb_SetupFlyIdle(EnBb* this) {
    if (this->actionFunc != EnBb_FlyIdle) {
        Animation_PlayLoop(&this->skelAnime, &gBubbleFlyingAnim);
    }

    if (this->actionFunc == EnBb_Attack) {
        this->attackWaitTimer = 40;
    } else {
        this->attackWaitTimer = 0;
    }

    this->timer = (s32)Rand_ZeroFloat(20.0f) + 40;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->flyHeightMod = (Math_CosS(this->bobPhase) * 10.0f) + 30.0f;
    this->targetYRotation = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);

    if ((this->actor.xzDistToPlayer < (this->attackRange + 120.0f)) ||
        (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < 300.0f)) {
        this->targetYRotation += (s16)((s32)Rand_Next() >> 0x11);
    }

    this->collider.base.atFlags |= AT_ON;
    this->maxSpeed = Rand_ZeroFloat(1.5f) + 1.0f;
    this->actionFunc = EnBb_FlyIdle;
}

/**
 * Makes the Bubble fly in circles around its home.
 */
void EnBb_FlyIdle(EnBb* this, PlayState* play) {
    EnBb_UpdateStateForFlying(this);

    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_WING);
    } else if (Animation_OnFrame(&this->skelAnime, 0.0f) && (Rand_ZeroOne() < 0.1f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }

    DECR(this->attackWaitTimer);
    this->timer--;

    if ((this->attackWaitTimer == 0) && (this->actor.xzDistToPlayer < this->attackRange) &&
        (Player_GetMask(play) != PLAYER_MASK_STONE)) {
        EnBb_SetupAttack(this);
    } else if (this->timer == 0) {
        EnBb_SetupFlyIdle(this);
    }
}

void EnBb_SetupAttack(EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &gBubbleAttackAnim);
    this->timer = (s32)Rand_ZeroFloat(20.0f) + 60;
    this->flyHeightMod = (Math_CosS(this->bobPhase) * 10.0f) + 30.0f;
    this->targetYRotation = this->actor.yawTowardsPlayer;
    this->maxSpeed = Rand_ZeroFloat(1.5f) + 4.0f;
    this->actionFunc = EnBb_Attack;
}

/**
 * Makes the Bubble actively move towards the player.
 */
void EnBb_Attack(EnBb* this, PlayState* play) {
    this->targetYRotation = this->actor.yawTowardsPlayer;
    EnBb_UpdateStateForFlying(this);

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_MOUTH);
    } else if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 7.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_WING);
    } else if (Animation_OnFrame(&this->skelAnime, 0.0f) && (Rand_ZeroOne() < 0.1f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }

    this->timer--;

    if (((this->attackRange + 120.0f) < this->actor.xzDistToPlayer) || (this->timer == 0) ||
        (Player_GetMask(play) == PLAYER_MASK_STONE) ||
        (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 400.0f)) {
        EnBb_SetupFlyIdle(this);
    }
}

void EnBb_SetupDown(EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &gBubbleFlyingAnim);
    this->collider.base.atFlags |= AT_ON;
    this->timer = 140;
    this->collider.base.acFlags |= AC_ON;
    this->actor.speed = 2.0f;
    this->flameScaleY = 0.0f;
    this->flameScaleX = 0.0f;
    this->actor.gravity = -2.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_DOWN);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnBb_Down;
}

/**
 * Makes the Bubble hop along the ground.
 */
void EnBb_Down(EnBb* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnBb_CheckForWall(this);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
        if (this->timer == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_UP);
            EnBb_SetupFlyIdle(this);
            return;
        }

        if (this->actor.velocity.y < -14.0f) {
            this->actor.velocity.y *= -0.7f;
        } else {
            this->actor.velocity.y = 10.0f;
        }

        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, false);
        Math_ScaledStepToS(&this->actor.shape.rot.y, BINANG_ADD(this->actor.yawTowardsPlayer, 0x8000), 0xBB8);
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_WING);
    }

    if (this->timer > 0) {
        this->timer--;
    }
}

void EnBb_SetupDead(EnBb* this, PlayState* play) {
    Vec3f* bodyPartVelocity;
    Vec3f posDiff;
    f32 magnitude;
    s32 i;

    func_800BE568(&this->actor, &this->collider);
    this->timer = 15;
    this->actor.shape.rot.x += 0x4E20;
    this->actor.speed = 0.0f;
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_BUBLE_DEAD);
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x70);
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->bodyPartDrawStatus = BB_BODY_PART_DRAW_STATUS_DEAD;
    this->actor.gravity = -1.5f;

    bodyPartVelocity = &this->bodyPartsVelocity[0];
    for (i = 0; i < BUBBLE_BODYPART_MAX; i++, bodyPartVelocity++) {
        Math_Vec3f_Diff(&this->bodyPartsPos[i], &this->actor.world.pos, &posDiff);
        magnitude = Math3D_Vec3fMagnitude(&posDiff);
        if (magnitude > 1.0f) {
            magnitude = 2.5f / magnitude;
        }

        bodyPartVelocity->x = posDiff.x * magnitude;
        bodyPartVelocity->z = posDiff.z * magnitude;
        bodyPartVelocity->y = Rand_ZeroFloat(3.5f) + 10.0f;
    }

    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnBb_Dead;
}

void EnBb_Dead(EnBb* this, PlayState* play) {
    s32 i;

    this->timer--;
    Math_SmoothStepToS(&this->actor.world.rot.z, 0x4000, 4, 0x1000, 0x400);

    if (this->timer == 0) {
        for (i = 0; i < BUBBLE_BODYPART_MAX; i++) {
            func_800B3030(play, &this->bodyPartsPos[i], &gZeroVec3f, &gZeroVec3f, 40, 7, 2);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->bodyPartsPos[i], 11, NA_SE_EN_EXTINCT);
        }

        EnBb_SetupWaitForRevive(this);
    } else {
        for (i = 0; i < BUBBLE_BODYPART_MAX; i++) {
            Math_Vec3f_Sum(&this->bodyPartsPos[i], &this->bodyPartsVelocity[i], &this->bodyPartsPos[i]);
            this->bodyPartsVelocity[i].y += this->actor.gravity;
        }
    }
}

void EnBb_SetupDamage(EnBb* this) {
    this->collider.base.acFlags &= ~AC_ON;
    Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_DAMAGE);
    func_800BE568(&this->actor, &this->collider);

    if (this->actor.colChkInfo.damageEffect == EN_BB_DMGEFF_ZORA_MAGIC) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
        this->drawDmgEffAlpha = 2.0f;
        this->drawDmgEffScale = 0.4f;
    } else if (this->actor.colChkInfo.damageEffect == EN_BB_DMGEFF_STUN) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 20);
        this->actor.speed = 0.0f;
    } else if (this->actor.colChkInfo.damageEffect == EN_BB_DMGEFF_HOOKSHOT) {
        this->actor.speed = 0.0f;
    } else {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
        this->actor.speed = 7.0f;
    }

    this->actor.gravity = -1.0f;
    this->actionFunc = EnBb_Damage;
}

void EnBb_Damage(EnBb* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.speed < 0.1f)) {
        EnBb_SetupDown(this);
    }
}

void EnBb_SetupFrozen(EnBb* this) {
    this->actor.speed = 0.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }

    this->actor.gravity = -2.0f;
    this->actionFunc = EnBb_Frozen;
}

void EnBb_Frozen(EnBb* this, PlayState* play) {
    DECR(this->timer);

    if (this->timer == 0) {
        EnBb_Thaw(this, play);
        if (this->actor.colChkInfo.health == 0) {
            EnBb_SetupDead(this, play);
        } else {
            EnBb_SetupDown(this);
        }
    }
}

void EnBb_SetupWaitForRevive(EnBb* this) {
    this->actor.draw = NULL;
    this->bodyPartDrawStatus = BB_BODY_PART_DRAW_STATUS_ALIVE;
    this->drawDmgEffAlpha = 0.0f;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.shape.rot.x = 0;
    this->actor.world.pos.y += 50.0f;
    this->timer = 200;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
    this->actionFunc = EnBb_WaitForRevive;
}

void EnBb_WaitForRevive(EnBb* this, PlayState* play) {
    this->timer--;
    if (this->timer == 0) {
        EnBb_SetupRevive(this);
    }
}

void EnBb_SetupRevive(EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &gBubbleAttackAnim);
    this->actor.draw = EnBb_Draw;
    this->actor.scale.x = 0.0f;
    this->actor.scale.y = 0.015f;
    this->actor.scale.z = 0.0f;
    this->flameScaleX = 1.0f;
    this->flameScaleY = 0.8f;
    this->actor.colChkInfo.health = sColChkInfoInit.health;
    this->actionFunc = EnBb_Revive;
}

void EnBb_Revive(EnBb* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y += 0x1F00;

    if (Math_StepToF(&this->actor.scale.x, 0.01f, 0.0005f)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->collider.base.acFlags |= AC_ON;
        this->collider.base.atFlags |= AT_ON;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        EnBb_SetupFlyIdle(this);
    }

    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = ((0.01f - this->actor.scale.x) * 0.5f) + 0.01f;
}

void EnBb_UpdateDamage(EnBb* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        this->collider.base.atFlags &= ~AT_ON;
        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            Actor_SetDropFlag(&this->actor, &this->collider.elem);
            this->flameScaleY = 0.0f;
            this->flameScaleX = 0.0f;
            EnBb_Thaw(this, play);

            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
            }

            if (this->actor.colChkInfo.damageEffect == EN_BB_DMGEFF_ICE_ARROW) {
                EnBb_Freeze(this);
                if (this->actor.colChkInfo.health == 0) {
                    this->timer = 3;
                    this->collider.base.acFlags &= ~AC_ON;
                }

                EnBb_SetupFrozen(this);
            } else if (this->actor.colChkInfo.health == 0) {
                EnBb_SetupDead(this, play);
            } else {
                EnBb_SetupDamage(this);
            }

            if (this->actor.colChkInfo.damageEffect == EN_BB_DMGEFF_LIGHT_ARROW) {
                this->drawDmgEffAlpha = 4.0f;
                this->drawDmgEffScale = 0.4f;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                            this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                            CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
            }
        }
    } else if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        if (this->actionFunc != EnBb_Down) {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            EnBb_SetupDown(this);
        }
    } else if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_BITE);

        if (this->flameScaleX > 0.0f) {
            gSaveContext.jinxTimer = 1200;
        }

        if (this->actionFunc == EnBb_Attack) {
            EnBb_SetupFlyIdle(this);
        }
    }
}

void EnBb_Update(Actor* thisx, PlayState* play) {
    EnBb* this = (EnBb*)thisx;

    EnBb_UpdateDamage(this, play);
    this->actionFunc(this, play);
    if ((this->actionFunc != EnBb_Dead) && (this->actionFunc != EnBb_WaitForRevive)) {
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 25.0f, 40.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);

        this->collider.dim.worldSphere.center.x = this->actor.world.pos.x;
        this->collider.dim.worldSphere.center.y = this->actor.world.pos.y + 15.0f;
        this->collider.dim.worldSphere.center.z = this->actor.world.pos.z;
        this->collider.dim.worldSphere.radius = this->flameScaleX * 30.0f;
        this->collider.dim.worldSphere.radius = CLAMP_MIN(this->collider.dim.worldSphere.radius, 20);

        Math_Vec3s_ToVec3f(&this->actor.focus.pos, &this->collider.dim.worldSphere.center);

        if (this->collider.base.atFlags & AT_ON) {
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->collider.base.ocFlags1 & OC1_ON) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->drawDmgEffAlpha > 0.0f) {
            if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
                this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.2f;
                this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.4f);
            } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.4f, 0.01f)) {
                Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
            }
        }
    }
}

s32 EnBb_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBb* this = (EnBb*)thisx;

    if (this->bodyPartDrawStatus == BB_BODY_PART_DRAW_STATUS_BROKEN) {
        this->limbDList = *dList;
        *dList = NULL;
    }

    return false;
}

/**
 * This maps a given limb based on its limbIndex to its appropriate index in the `bodyPartsPos/Velocity` arrays.
 */
static s8 sLimbToBodyParts[BUBBLE_LIMB_MAX] = {
    BODYPART_NONE,                      // BUBBLE_LIMB_NONE
    BODYPART_NONE,                      // BUBBLE_LIMB_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_CRANIUM_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_JAW_ROOT
    BUBBLE_BODYPART_JAW,                // BUBBLE_LIMB_JAW
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_WRAPPER
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_WEBBING_ROOT
    BUBBLE_BODYPART_LEFT_WING_WEBBING,  // BUBBLE_LIMB_LEFT_WING_WEBBING
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_BONE
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_WRAPPER
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_WEBBING_ROOT
    BUBBLE_BODYPART_RIGHT_WING_WEBBING, // BUBBLE_LIMB_RIGHT_WING_WEBBING
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_BONE
    BUBBLE_BODYPART_CRANIUM,            // BUBBLE_LIMB_CRANIUM
};

/**
 * The last element of the `bodyParts` arrays is not tied to any particular limb and is instead used to control the
 * placement of effects. The positions of these effects are offset by a certain amount from the Bubble's cranium limb.
 */
static Vec3f sEffectsBodyPartOffset = { 1000.0f, -700.0f, 0.0f };

void EnBb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnBb* this = (EnBb*)thisx;
    MtxF* currentMatrixState;

    if (this->bodyPartDrawStatus == BB_BODY_PART_DRAW_STATUS_ALIVE) {
        if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
            if (sLimbToBodyParts[limbIndex] == BUBBLE_BODYPART_JAW) {
                Matrix_MultVecX(1000.0f, &this->bodyPartsPos[BUBBLE_BODYPART_JAW]);
            } else if (sLimbToBodyParts[limbIndex] == BUBBLE_BODYPART_CRANIUM) {
                Matrix_MultVecX(-1000.0f, &this->bodyPartsPos[BUBBLE_BODYPART_CRANIUM]);
                Matrix_MultVec3f(&sEffectsBodyPartOffset, &this->bodyPartsPos[BUBBLE_BODYPART_EFFECTS]);
            } else {
                Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
            }
        }
    } else if (this->bodyPartDrawStatus > BB_BODY_PART_DRAW_STATUS_ALIVE) {
        if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
            Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
        }

        if (limbIndex == BUBBLE_LIMB_CRANIUM) {
            this->bodyPartDrawStatus = BB_BODY_PART_DRAW_STATUS_BROKEN;
        }
    } else {
        if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
            OPEN_DISPS(play->state.gfxCtx);

            currentMatrixState = Matrix_GetCurrent();
            currentMatrixState->mf[3][0] = this->bodyPartsPos[sLimbToBodyParts[limbIndex]].x;
            currentMatrixState->mf[3][1] = this->bodyPartsPos[sLimbToBodyParts[limbIndex]].y;
            currentMatrixState->mf[3][2] = this->bodyPartsPos[sLimbToBodyParts[limbIndex]].z;
            Matrix_RotateZS(thisx->world.rot.z, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, this->limbDList);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void EnBb_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBb* this = (EnBb*)thisx;
    MtxF* currentMatrixState;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_OPA_DISP;
    gSPDisplayList(&gfx[0], gSetupDLs[SETUPDL_25]);
    POLY_OPA_DISP = &gfx[1];
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnBb_OverrideLimbDraw,
                      EnBb_PostLimbDraw, &this->actor);

    if (this->flameScaleX > 0.0f) {
        currentMatrixState = Matrix_GetCurrent();
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_RotateYS(((Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y) + 0x8000),
                        MTXMODE_APPLY);
        Matrix_Scale(this->flameScaleX, this->flameScaleY, 1.0f, MTXMODE_APPLY);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);
        gDPSetEnvColorOverride(POLY_XLU_DISP++, 0, 0, 255, 0, COSMETIC_ID("Effects.SkullBubbleFlame"));
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (play->gameplayFrames * -20) & 0x1FF,
                                      32, 128, 0, 0, 0, -20));
        currentMatrixState->mf[3][1] -= 47.0f * this->flameScaleY;
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, BUBBLE_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}

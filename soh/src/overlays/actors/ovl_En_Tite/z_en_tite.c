/*
 * File: z_en_tite.c
 * Overlay: ovl_En_Tite
 * Description: Tektite
 */

#include "z_en_tite.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.h"
#include "vt.h"
#include "objects/object_tite/object_tite.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

// EnTite_Idle
#define vIdleTimer actionVar1

// EnTite_Attack (vQueuedJumps also used by EnTite_MoveTowardPlayer)
#define vAttackState actionVar1
#define vQueuedJumps actionVar2

//  EnTite_FlipOnBack
#define vOnBackTimer actionVar1
#define vLegTwitchTimer actionVar2

typedef enum {
    /* 0x0 */ TEKTITE_DEATH_CRY,
    /* 0x1 */ TEKTITE_UNK_1,
    /* 0x2 */ TEKTITE_UNK_2,
    /* 0x3 */ TEKTITE_RECOIL,
    /* 0x4 */ TEKTITE_UNK_4,
    /* 0x5 */ TEKTITE_FALL_APART,
    /* 0x6 */ TEKTITE_IDLE,
    /* 0x7 */ TEKTITE_STUNNED,
    /* 0x8 */ TEKTITE_UNK_8,
    /* 0x9 */ TEKTITE_ATTACK,
    /* 0xA */ TEKTITE_TURN_TOWARD_PLAYER,
    /* 0xB */ TEKTITE_UNK9,
    /* 0xC */ TEKTITE_MOVE_TOWARD_PLAYER
} EnTiteAction;

typedef enum {
    /* 0x0 */ TEKTITE_BEGIN_LUNGE,
    /* 0x1 */ TEKTITE_MID_LUNGE,
    /* 0x2 */ TEKTITE_LANDED,
    /* 0x2 */ TEKTITE_SUBMERGED
} EnTiteAttackState;

typedef enum {
    /* 0x0 */ TEKTITE_INITIAL,
    /* 0x1 */ TEKTITE_UNFLIPPED,
    /* 0x2 */ TEKTITE_FLIPPED
} EnTiteFlipState;

void EnTite_Init(Actor* thisx, PlayState* play);
void EnTite_Destroy(Actor* thisx, PlayState* play);
void EnTite_Update(Actor* thisx, PlayState* play);
void EnTite_Draw(Actor* thisx, PlayState* play);

void EnTite_SetupIdle(EnTite* this);
void EnTite_SetupTurnTowardPlayer(EnTite* this);
void EnTite_SetupMoveTowardPlayer(EnTite* this);
void EnTite_SetupDeathCry(EnTite* this);
void EnTite_SetupFlipUpright(EnTite* this);

void EnTite_Idle(EnTite* this, PlayState* play);
void EnTite_Attack(EnTite* this, PlayState* play);
void EnTite_TurnTowardPlayer(EnTite* this, PlayState* play);
void EnTite_MoveTowardPlayer(EnTite* this, PlayState* play);
void EnTite_Recoil(EnTite* this, PlayState* play);
void EnTite_Stunned(EnTite* this, PlayState* play);
void EnTite_DeathCry(EnTite* this, PlayState* play);
void EnTite_FallApart(EnTite* this, PlayState* play);
void EnTite_FlipOnBack(EnTite* this, PlayState* play);
void EnTite_FlipUpright(EnTite* this, PlayState* play);

const ActorInit En_Tite_InitVars = {
    ACTOR_EN_TITE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_TITE,
    sizeof(EnTite),
    (ActorFunc)EnTite_Init,
    (ActorFunc)EnTite_Destroy,
    (ActorFunc)EnTite_Update,
    (ActorFunc)EnTite_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { 0, { { 0, 1500, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(4, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0xE),
    /* Ice magic     */ DMG_ENTRY(3, 0xF),
    /* Light magic   */ DMG_ENTRY(0, 0xE),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x45, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -40, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_STOP),
};

static AnimationHeader* D_80B1B634[] = {
    &object_tite_Anim_00083C, &object_tite_Anim_0004F8, &object_tite_Anim_00069C, NULL, NULL, NULL,
};

// Some kind of offset for the position of each tektite foot
static Vec3f sFootOffset = { 2800.0f, -200.0f, 0.0f };

// Relative positions to spawn ice chunks when tektite is frozen
static Vec3f sIceChunks[12] = {
    { 20.0f, 20.0f, 0.0f },   { 10.0f, 40.0f, 10.0f },   { -10.0f, 40.0f, 10.0f }, { -20.0f, 20.0f, 0.0f },
    { 10.0f, 40.0f, -10.0f }, { -10.0f, 40.0f, -10.0f }, { 0.0f, 20.0f, -20.0f },  { 10.0f, 0.0f, 10.0f },
    { 10.0f, 0.0f, -10.0f },  { 0.0f, 20.0f, 20.0f },    { -10.0f, 0.0f, 10.0f },  { -10.0f, 0.0f, -10.0f },
};

void EnTite_SetupAction(EnTite* this, EnTiteActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnTite_Init(Actor* thisx, PlayState* play) {
    EnTite* this = (EnTite*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->targetMode = 3;
    Actor_SetScale(thisx, 0.01f);
    SkelAnime_Init(play, &this->skelAnime, &object_tite_Skel_003A20, &object_tite_Anim_0012E4, this->jointTable,
                   this->morphTable, 25);
    ActorShape_Init(&thisx->shape, -200.0f, ActorShadow_DrawCircle, 70.0f);
    this->flipState = TEKTITE_INITIAL;
    thisx->colChkInfo.damageTable = sDamageTable;
    this->actionVar1 = 0;
    this->bodyBreak.val = BODYBREAK_STATUS_FINISHED;
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 20.0f;
    thisx->colChkInfo.health = 2;
    thisx->colChkInfo.mass = MASS_HEAVY;
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, thisx, &sJntSphInit, &this->colliderItem);
    this->unk_2DC = 0x1D;
    if (this->actor.params == TEKTITE_BLUE) {
        this->unk_2DC |= 0x40; // Don't use the actor engine's ripple spawning code
        thisx->colChkInfo.health = 4;
        thisx->naviEnemyId += 1;
    }
    EnTite_SetupIdle(this);
}

void EnTite_Destroy(Actor* thisx, PlayState* play) {
    EnTite* this = (EnTite*)thisx;
    EnEncount1* spawner;

    if (thisx->parent != NULL) {
        spawner = (EnEncount1*)thisx->parent;
        if (spawner->curNumSpawn > 0) {
            spawner->curNumSpawn--;
        }
        osSyncPrintf("\n\n");
        // "Number of simultaneous occurrences"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 同時発生数 ☆☆☆☆☆%d\n" VT_RST, spawner->curNumSpawn);
        osSyncPrintf("\n\n");
    }
    Collider_DestroyJntSph(play, &this->collider);
}

void EnTite_SetupIdle(EnTite* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_tite_Anim_0012E4, 4.0f);
    this->action = TEKTITE_IDLE;
    this->vIdleTimer = Rand_S16Offset(15, 30);
    this->actor.speedXZ = 0.0f;
    EnTite_SetupAction(this, EnTite_Idle);
}

void EnTite_Idle(EnTite* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (this->actor.params == TEKTITE_BLUE) {
        if (this->actor.bgCheckFlags & 0x20) {
            // Float on water surface
            this->actor.gravity = 0.0f;
            Math_SmoothStepToF(&this->actor.velocity.y, 0.0f, 1.0f, 2.0f, 0.0f);
            Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.world.pos.y + this->actor.yDistToWater, 1.0f, 2.0f,
                               0.0f);
        } else {
            this->actor.gravity = -1.0f;
        }
    }
    if ((this->actor.bgCheckFlags & 3) && (this->actor.velocity.y <= 0.0f)) {
        this->actor.velocity.y = 0.0f;
    }
    if (this->vIdleTimer > 0) {
        this->vIdleTimer--;
    } else if ((this->actor.xzDistToPlayer < 300.0f) && (this->actor.yDistToPlayer <= 80.0f)) {
        EnTite_SetupTurnTowardPlayer(this);
    }
}

void EnTite_SetupAttack(EnTite* this) {
    Animation_PlayOnce(&this->skelAnime, &object_tite_Anim_00083C);
    this->action = TEKTITE_ATTACK;
    this->vAttackState = TEKTITE_BEGIN_LUNGE;
    this->vQueuedJumps = Rand_S16Offset(1, 3);
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnTite_SetupAction(this, EnTite_Attack);
}

void EnTite_Attack(EnTite* this, PlayState* play) {
    s16 angleToPlayer;
    s32 attackState;
    Vec3f ripplePos;

    if (SkelAnime_Update(&this->skelAnime) != 0) {
        attackState = this->vAttackState; // for deciding whether to change animation
        switch (this->vAttackState) {
            case TEKTITE_BEGIN_LUNGE:
                // Snap to ground or water, then lunge into the air with some initial speed
                this->vAttackState = TEKTITE_MID_LUNGE;
                if ((this->actor.params != TEKTITE_BLUE) || !(this->actor.bgCheckFlags & 0x20)) {
                    if (this->actor.floorHeight > BGCHECK_Y_MIN) {
                        this->actor.world.pos.y = this->actor.floorHeight;
                    }
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
                } else {
                    this->actor.world.pos.y += this->actor.yDistToWater;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_JUMP_WATER);
                }
                this->actor.velocity.y = 8.0f;
                this->actor.gravity = -1.0f;
                this->actor.speedXZ = 4.0f;
                break;
            case TEKTITE_MID_LUNGE:
                // Continue trajectory until tektite has negative velocity and has landed on ground/water surface
                // Snap to ground/water surface, or if falling fast dip into the water and slow fall speed
                this->actor.flags |= ACTOR_FLAG_24;
                if ((this->actor.bgCheckFlags & 3) ||
                    ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20))) {
                    if (this->actor.velocity.y <= 0.0f) {
                        this->vAttackState = TEKTITE_LANDED;
                        if ((this->actor.params != TEKTITE_BLUE) || !(this->actor.bgCheckFlags & 0x20)) {
                            if (BGCHECK_Y_MIN < this->actor.floorHeight) {
                                this->actor.world.pos.y = this->actor.floorHeight;
                            }
                            this->actor.velocity.y = 0.0f;
                            this->actor.speedXZ = 0.0f;
                        } else {
                            this->actor.gravity = 0.0f;
                            if (this->actor.velocity.y < -8.0f) {
                                ripplePos = this->actor.world.pos;
                                ripplePos.y += this->actor.yDistToWater;
                                this->vAttackState++; // TEKTITE_SUBMERGED
                                this->actor.velocity.y *= 0.75f;
                                attackState = this->vAttackState;
                                EffectSsGRipple_Spawn(play, &ripplePos, 0, 500, 0);
                            } else {
                                this->actor.velocity.y = 0.0f;
                                this->actor.speedXZ = 0.0f;
                            }
                        }
                        this->actor.world.rot.y = this->actor.shape.rot.y;
                    }
                }
                break;
            case TEKTITE_LANDED:
                // Get ready to begin another lunge if more lunges are queued, otherwise start turning
                if (this->vQueuedJumps != 0) {
                    this->vQueuedJumps--;
                    this->vAttackState = TEKTITE_BEGIN_LUNGE;
                    this->collider.base.atFlags &= ~AT_HIT;
                } else {
                    EnTite_SetupTurnTowardPlayer(this);
                }
                break;
            case TEKTITE_SUBMERGED:
                // Check if floated to surface
                if (this->actor.yDistToWater == 0.0f) {
                    this->vAttackState = TEKTITE_LANDED;
                    attackState = this->vAttackState;
                }
                break;
        }
        // If switching attack state, change animation (unless tektite is switching between submerged and landed)
        if (attackState != this->vAttackState) {
            Animation_PlayOnce(&this->skelAnime, D_80B1B634[this->vAttackState]);
        }
    }

    switch (this->vAttackState) {
        case TEKTITE_BEGIN_LUNGE:
            // Slightly turn to player and switch to turning/idling action if the player is too far
            Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 1000, 0);
            this->actor.shape.rot.y = this->actor.world.rot.y;
            angleToPlayer = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
            if ((this->actor.xzDistToPlayer > 300.0f) && (this->actor.yDistToPlayer > 80.0f)) {
                EnTite_SetupIdle(this);
            } else if (ABS(angleToPlayer) >= 9000) {
                EnTite_SetupTurnTowardPlayer(this);
            }
            break;
        case TEKTITE_MID_LUNGE:
            // Generate sparkles at feet upon landing, set jumping animation and hurtbox and check if hit player
            if (this->actor.velocity.y >= 5.0f) {
                if (this->actor.bgCheckFlags & 1) {
                    func_800355B8(play, &this->frontLeftFootPos);
                    func_800355B8(play, &this->frontRightFootPos);
                    func_800355B8(play, &this->backRightFootPos);
                    func_800355B8(play, &this->backLeftFootPos);
                }
            }
            if (!(this->collider.base.atFlags & AT_HIT) && (this->actor.flags & ACTOR_FLAG_6)) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            } else {
                Player* player = GET_PLAYER(play);
                this->collider.base.atFlags &= ~AT_HIT;
                Animation_MorphToLoop(&this->skelAnime, &object_tite_Anim_0012E4, 4.0f);
                this->actor.speedXZ = -6.0f;
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                if (&player->actor == this->collider.base.at) {
                    if (!(this->collider.base.atFlags & AT_BOUNCED)) {
                        Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                    }
                }
                EnTite_SetupAction(this, EnTite_Recoil);
            }
            break;
        case TEKTITE_LANDED:
            // Slightly turn to player
            Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 1500, 0);
            break;
        case TEKTITE_SUBMERGED:
            // Float up to water surface
            Math_SmoothStepToF(&this->actor.velocity.y, 0.0f, 1.0f, 2.0f, 0.0f);
            Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
            Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.world.pos.y + this->actor.yDistToWater, 1.0f, 2.0f,
                               0.0f);
            break;
    }
    // Create ripples on water surface where tektite feet landed
    if (this->actor.bgCheckFlags & 2) {
        if (!(this->actor.bgCheckFlags & 0x20)) {
            func_80033480(play, &this->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backLeftFootPos, 1.0f, 2, 80, 15, 1);
        }
    }
    // if landed, kill XZ speed and play appropriate sounds
    if (this->actor.params == TEKTITE_BLUE) {
        if (this->actor.bgCheckFlags & 0x40) {
            this->actor.speedXZ = 0.0f;
            if (this->vAttackState == TEKTITE_SUBMERGED) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_LAND_WATER);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_LAND_WATER2);
            }
            this->actor.bgCheckFlags &= ~0x40;
        } else if (this->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        }
    } else if (this->actor.bgCheckFlags & 2) {
        this->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
    }
}

void EnTite_SetupTurnTowardPlayer(EnTite* this) {
    Animation_PlayLoop(&this->skelAnime, &object_tite_Anim_000A14);
    this->action = TEKTITE_TURN_TOWARD_PLAYER;
    if ((this->actor.bgCheckFlags & 3) || ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20))) {
        if (this->actor.velocity.y <= 0.0f) {
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
            this->actor.speedXZ = 0.0f;
        }
    }
    EnTite_SetupAction(this, EnTite_TurnTowardPlayer);
}

void EnTite_TurnTowardPlayer(EnTite* this, PlayState* play) {
    s16 angleToPlayer;
    s16 turnVelocity;

    if (((this->actor.bgCheckFlags & 3) ||
         ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20))) &&
        (this->actor.velocity.y <= 0.0f)) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
    }
    // Calculate turn velocity and animation speed based on angle to player
    if ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)) {
        this->actor.world.pos.y += this->actor.yDistToWater;
    }
    angleToPlayer = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;
    if (angleToPlayer > 0) {
        turnVelocity = (angleToPlayer / 42.0f) + 10.0f;
        this->actor.world.rot.y += (turnVelocity * 2);
    } else {
        turnVelocity = (angleToPlayer / 42.0f) - 10.0f;
        this->actor.world.rot.y += (turnVelocity * 2);
    }
    if (angleToPlayer > 0) {
        this->skelAnime.playSpeed = turnVelocity * 0.01f;
    } else {
        this->skelAnime.playSpeed = turnVelocity * 0.01f;
    }

    /**
     * Play sounds once every animation cycle
     */
    SkelAnime_Update(&this->skelAnime);
    if (((s16)this->skelAnime.curFrame & 7) == 0) {
        if ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_WALK_WATER);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_WALK);
        }
    }

    // Idle if player is far enough away from the tektite, move or attack if almost facing player
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if ((this->actor.xzDistToPlayer > 300.0f) && (this->actor.yDistToPlayer > 80.0f)) {
        EnTite_SetupIdle(this);
    } else if (Actor_IsFacingPlayer(&this->actor, 3640)) {
        if ((this->actor.xzDistToPlayer <= 180.0f) && (this->actor.yDistToPlayer <= 80.0f)) {
            EnTite_SetupAttack(this);
        } else {
            EnTite_SetupMoveTowardPlayer(this);
        }
    }
}

void EnTite_SetupMoveTowardPlayer(EnTite* this) {
    Animation_PlayLoop(&this->skelAnime, &object_tite_Anim_000C70);
    this->action = TEKTITE_MOVE_TOWARD_PLAYER;
    this->actor.velocity.y = 10.0f;
    this->actor.gravity = -1.0f;
    this->actor.speedXZ = 4.0f;
    this->vQueuedJumps = Rand_S16Offset(1, 3);
    if ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_JUMP_WATER);
    } else {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
    }
    EnTite_SetupAction(this, EnTite_MoveTowardPlayer);
}

/**
 *  Jumping toward player as a method of travel (different from attacking, has no hitbox)
 */
void EnTite_MoveTowardPlayer(EnTite* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.1f, 1.0f, 0.0f);
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.bgCheckFlags & 0x42) {
        if (!(this->actor.bgCheckFlags & 0x40)) {
            func_80033480(play, &this->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backLeftFootPos, 1.0f, 2, 80, 15, 1);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_LAND_WATER);
        }
    }

    if ((this->actor.bgCheckFlags & 2) || ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x40))) {
        if (this->vQueuedJumps != 0) {
            this->vQueuedJumps--;
        } else {
            EnTite_SetupIdle(this);
        }
    }

    if (((this->actor.bgCheckFlags & 3) || (this->actor.params == TEKTITE_BLUE && (this->actor.bgCheckFlags & 0x60))) &&
        (this->actor.velocity.y <= 0.0f)) {
        // slightly turn toward player upon landing and snap to ground or water.
        this->actor.speedXZ = 0.0f;
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4000, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if ((this->actor.params != TEKTITE_BLUE) || !(this->actor.bgCheckFlags & 0x20)) {
            if (this->actor.floorHeight > BGCHECK_Y_MIN) {
                this->actor.world.pos.y = this->actor.floorHeight;
            }
        } else if (this->actor.bgCheckFlags & 0x40) {
            Vec3f ripplePos = this->actor.world.pos;
            this->actor.bgCheckFlags &= ~0x40;
            ripplePos.y += this->actor.yDistToWater;
            this->actor.gravity = 0.0f;
            this->actor.velocity.y *= 0.75f;
            EffectSsGRipple_Spawn(play, &ripplePos, 0, 500, 0);
            return;
        } else {
            // If submerged, float to surface
            Math_SmoothStepToF(&this->actor.velocity.y, 0.0f, 1.0f, 2.0f, 0.0f);
            Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.world.pos.y + this->actor.yDistToWater, 1.0f, 2.0f,
                               0.0f);
            if (this->actor.yDistToWater != 0.0f) {
                // Do not change state until tekite has floated to surface
                return;
            }
        }

        // Idle or turn if player is too far away, otherwise keep jumping
        if (((this->actor.xzDistToPlayer > 300.0f) && (this->actor.yDistToPlayer > 80.0f))) {
            EnTite_SetupIdle(this);
        } else if (((this->actor.xzDistToPlayer <= 180.0f)) && ((this->actor.yDistToPlayer <= 80.0f))) {
            if (this->vQueuedJumps <= 0) {
                EnTite_SetupTurnTowardPlayer(this);
            } else {
                this->actor.velocity.y = 10.0f;
                this->actor.speedXZ = 4.0f;
                this->actor.flags |= ACTOR_FLAG_24;
                this->actor.gravity = -1.0f;
                if ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_JUMP_WATER);
                } else {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
                }
            }
        } else {
            this->actor.velocity.y = 10.0f;
            this->actor.speedXZ = 4.0f;
            this->actor.flags |= ACTOR_FLAG_24;
            this->actor.gravity = -1.0f;
            if ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_JUMP_WATER);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
            }
        }
        // If in midair:
    } else {
        // Turn slowly toward player
        this->actor.flags |= ACTOR_FLAG_24;
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 1000, 0);
        if (this->actor.velocity.y >= 6.0f) {
            if (this->actor.bgCheckFlags & 1) {
                func_800355B8(play, &this->frontLeftFootPos);
                func_800355B8(play, &this->frontRightFootPos);
                func_800355B8(play, &this->backRightFootPos);
                func_800355B8(play, &this->backLeftFootPos);
            }
        }
    }
}

void EnTite_SetupRecoil(EnTite* this) {
    this->action = TEKTITE_RECOIL;
    Animation_MorphToLoop(&this->skelAnime, &object_tite_Anim_0012E4, 4.0f);
    this->actor.speedXZ = -6.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.gravity = -1.0f;
    EnTite_SetupAction(this, EnTite_Recoil);
}

/**
 * After tektite hits or gets hit, recoils backwards and slides a bit upon landing
 */
void EnTite_Recoil(EnTite* this, PlayState* play) {
    s16 angleToPlayer;

    // Snap to ground or water surface upon landing
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (((this->actor.bgCheckFlags & 3) || (this->actor.params == TEKTITE_BLUE && (this->actor.bgCheckFlags & 0x20))) &&
        (this->actor.velocity.y <= 0.0f)) {
        if ((this->actor.params != TEKTITE_BLUE) || !(this->actor.bgCheckFlags & 0x20)) {
            if (this->actor.floorHeight > BGCHECK_Y_MIN) {
                this->actor.world.pos.y = this->actor.floorHeight;
            }
        } else {
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = 0.0f;
            this->actor.world.pos.y += this->actor.yDistToWater;
        }
    }

    // play sound and generate ripples
    if (this->actor.bgCheckFlags & 0x42) {
        if (!(this->actor.bgCheckFlags & 0x40)) {
            func_80033480(play, &this->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backLeftFootPos, 1.0f, 2, 80, 15, 1);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        } else {
            this->actor.bgCheckFlags &= ~0x40;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_LAND_WATER2);
        }
    }

    // If player is far away, idle. Otherwise attack or move
    angleToPlayer = (this->actor.yawTowardsPlayer - this->actor.shape.rot.y);
    if ((this->actor.speedXZ == 0.0f) && ((this->actor.bgCheckFlags & 1) || ((this->actor.params == TEKTITE_BLUE) &&
                                                                             (this->actor.bgCheckFlags & 0x20)))) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->collider.base.atFlags &= ~AT_HIT;
        if ((this->actor.xzDistToPlayer > 300.0f) && (this->actor.yDistToPlayer > 80.0f) &&
            (ABS(this->actor.shape.rot.x) < 4000) && (ABS(this->actor.shape.rot.z) < 4000) &&
            ((this->actor.bgCheckFlags & 1) ||
             ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)))) {
            EnTite_SetupIdle(this);
        } else if ((this->actor.xzDistToPlayer < 180.0f) && (this->actor.yDistToPlayer <= 80.0f) &&
                   (ABS(angleToPlayer) <= 6000)) {
            EnTite_SetupAttack(this);
        } else {
            EnTite_SetupMoveTowardPlayer(this);
        }
    }
    SkelAnime_Update(&this->skelAnime);
}

void EnTite_SetupStunned(EnTite* this) {
    Animation_Change(&this->skelAnime, &object_tite_Anim_0012E4, 0.0f, 0.0f,
                     (f32)Animation_GetLastFrame(&object_tite_Anim_0012E4), ANIMMODE_LOOP, 4.0f);
    this->action = TEKTITE_STUNNED;
    this->actor.speedXZ = -6.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    if (this->damageEffect == 0xF) {
        this->spawnIceTimer = 48;
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnTite_SetupAction(this, EnTite_Stunned);
}

/**
 * stunned or frozen
 */
void EnTite_Stunned(EnTite* this, PlayState* play) {
    s16 angleToPlayer;

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    // Snap to ground or water
    if (((this->actor.bgCheckFlags & 3) ||
         ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20))) &&
        (this->actor.velocity.y <= 0.0f)) {
        if (((this->actor.params != TEKTITE_BLUE) || !(this->actor.bgCheckFlags & 0x20))) {
            if (this->actor.floorHeight > BGCHECK_Y_MIN) {
                this->actor.world.pos.y = this->actor.floorHeight;
            }
        } else {
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = 0.0f;
            this->actor.world.pos.y += this->actor.yDistToWater;
        }
    }
    // Play sounds and spawn dirt effects upon landing
    if (this->actor.bgCheckFlags & 0x42) {
        if (!(this->actor.bgCheckFlags & 0x40)) {
            func_80033480(play, &this->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(play, &this->backLeftFootPos, 1.0f, 2, 80, 15, 1);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        } else {
            this->actor.bgCheckFlags &= ~0x40;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_LAND_WATER2);
        }
    }
    // Decide on next action based on health, flip state and player distance
    angleToPlayer = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (((this->actor.colorFilterTimer == 0) && (this->actor.speedXZ == 0.0f)) &&
        ((this->actor.bgCheckFlags & 1) ||
         ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)))) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->actor.colChkInfo.health == 0) {
            EnTite_SetupDeathCry(this);
        } else if (this->flipState == TEKTITE_FLIPPED) {
            EnTite_SetupFlipUpright(this);
        } else if (((this->actor.xzDistToPlayer > 300.0f) && (this->actor.yDistToPlayer > 80.0f) &&
                    (ABS(this->actor.shape.rot.x) < 4000) && (ABS(this->actor.shape.rot.z) < 4000)) &&
                   ((this->actor.bgCheckFlags & 1) ||
                    ((this->actor.params == TEKTITE_BLUE) && (this->actor.bgCheckFlags & 0x20)))) {
            EnTite_SetupIdle(this);
        } else if ((this->actor.xzDistToPlayer < 180.0f) && (this->actor.yDistToPlayer <= 80.0f) &&
                   (ABS(angleToPlayer) <= 6000)) {
            EnTite_SetupAttack(this);
        } else {
            EnTite_SetupMoveTowardPlayer(this);
        }
    }
    SkelAnime_Update(&this->skelAnime);
}

void EnTite_SetupDeathCry(EnTite* this) {
    this->action = TEKTITE_DEATH_CRY;
    this->actor.colorFilterTimer = 0;
    this->actor.speedXZ = 0.0f;
    EnTite_SetupAction(this, EnTite_DeathCry);
}

/**
 * First frame of death. Scream in pain and allocate memory for EnPart data
 */
void EnTite_DeathCry(EnTite* this, PlayState* play) {
    EffectSsDeadSound_SpawnStationary(play, &this->actor.projectedPos, NA_SE_EN_TEKU_DEAD, true,
                                      DEADSOUND_REPEAT_MODE_OFF, 40);
    this->action = TEKTITE_FALL_APART;
    EnTite_SetupAction(this, EnTite_FallApart);
    BodyBreak_Alloc(&this->bodyBreak, 24, play);
}

/**
 * Spawn EnPart and drop items
 */
void EnTite_FallApart(EnTite* this, PlayState* play) {
    if (BodyBreak_SpawnParts(&this->actor, &this->bodyBreak, play, this->actor.params + 0xB)) {
        if (this->actor.params == TEKTITE_BLUE) {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xE0);
            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE]++;
        } else {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x40);
            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_RED]++;
        }
        Actor_Kill(&this->actor);
    }
}

void EnTite_SetupFlipOnBack(EnTite* this) {

    Animation_PlayLoopSetSpeed(&this->skelAnime, &object_tite_Anim_000A14, 1.5f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_REVERSE);
    this->flipState = TEKTITE_FLIPPED;
    this->vOnBackTimer = 500;
    this->actor.speedXZ = 0.0f;
    this->actor.gravity = -1.0f;
    this->vLegTwitchTimer = (Rand_ZeroOne() * 50.0f);
    this->actor.velocity.y = 11.0f;
    EnTite_SetupAction(this, EnTite_FlipOnBack);
}

/**
 * During the flip animation and also while idling on back
 */
void EnTite_FlipOnBack(EnTite* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.z, 0x7FFF, 1, 4000, 0);
    // randomly reset the leg wiggling animation whenever timer reaches 0 to give illusion of twitching legs
    this->vLegTwitchTimer--;
    if (this->vLegTwitchTimer == 0) {
        this->vLegTwitchTimer = Rand_ZeroOne() * 30.0f;
        this->skelAnime.curFrame = Rand_ZeroOne() * 5.0f;
    }
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.bgCheckFlags & 3) {
        // Upon landing, spawn dust and make noise
        if (this->actor.bgCheckFlags & 2) {
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 20.0f, 11, 4.0f, 0, 0, false);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        }
        this->vOnBackTimer--;
        if (this->vOnBackTimer == 0) {
            EnTite_SetupFlipUpright(this);
        }
    } else {
        // Gradually increase y offset during flip so that the actor position is at tektite's back instead of feet
        if (this->actor.shape.yOffset < 2800.0f) {
            this->actor.shape.yOffset += 400.0f;
        }
    }
}

void EnTite_SetupFlipUpright(EnTite* this) {
    this->flipState = TEKTITE_UNFLIPPED;
    this->actionVar1 = 1000; // value unused here and overwritten in SetupIdle
    //! @bug flying tektite: water sets gravity to 0 so y velocity will never decrease from 13
    this->actor.velocity.y = 13.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_TEKU_REVERSE);
    EnTite_SetupAction(this, EnTite_FlipUpright);
}

void EnTite_FlipUpright(EnTite* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.z, 0, 1, 0xFA0, 0);
    SkelAnime_Update(&this->skelAnime);
    //! @bug flying tektite: the following condition is never met and tektite stays stuck in this action forever
    if (this->actor.bgCheckFlags & 2) {
        func_80033480(play, &this->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
        func_80033480(play, &this->frontRightFootPos, 1.0f, 2, 80, 15, 1);
        func_80033480(play, &this->backRightFootPos, 1.0f, 2, 80, 15, 1);
        func_80033480(play, &this->backLeftFootPos, 1.0f, 2, 80, 15, 1);
        this->actor.shape.yOffset = 0.0f;
        this->actor.world.pos.y = this->actor.floorHeight;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        EnTite_SetupIdle(this);
    }
}

void EnTite_CheckDamage(Actor* thisx, PlayState* play) {
    EnTite* this = (EnTite*)thisx;

    if ((this->collider.base.acFlags & AC_HIT) && (this->action >= TEKTITE_IDLE)) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (thisx->colChkInfo.damageEffect != 0xE) { // Immune to fire magic
            this->damageEffect = thisx->colChkInfo.damageEffect;
            Actor_SetDropFlag(thisx, &this->collider.elements[0].info, 0);
            // Stun if Tektite hit by nut, boomerang, hookshot, ice arrow or ice magic
            if ((thisx->colChkInfo.damageEffect == 1) || (thisx->colChkInfo.damageEffect == 0xF)) {
                if (this->action != TEKTITE_STUNNED) {
                    Actor_SetColorFilter(thisx, 0, 0x78, 0, 0x50);
                    Actor_ApplyDamage(thisx);
                    EnTite_SetupStunned(this);
                }
                // Otherwise apply damage and handle death where necessary
            } else {
                if ((thisx->colorFilterTimer == 0) || ((thisx->colorFilterParams & 0x4000) == 0)) {
                    Actor_SetColorFilter(thisx, 0x4000, 0xFF, 0, 8);
                    Actor_ApplyDamage(thisx);
                }
                if (thisx->colChkInfo.health == 0) {
                    EnTite_SetupDeathCry(this);
                } else {
                    // Flip tektite back up if it's on its back
                    Audio_PlayActorSound2(thisx, NA_SE_EN_TEKU_DAMAGE);
                    if (this->flipState != TEKTITE_FLIPPED) {
                        EnTite_SetupRecoil(this);
                    } else {
                        EnTite_SetupFlipUpright(this);
                    }
                }
            }
        }
        // If hammer has recently hit the floor and player is close to tektite, flip over
    } else if ((thisx->colChkInfo.health != 0) && (play->actorCtx.unk_02 != 0) &&
               (thisx->xzDistToPlayer <= 400.0f) && (thisx->bgCheckFlags & 1)) {
        if (this->flipState == TEKTITE_FLIPPED) {
            EnTite_SetupFlipUpright(this);
        } else if ((this->action >= TEKTITE_IDLE) || (this->action >= TEKTITE_IDLE)) {
            EnTite_SetupFlipOnBack(this);
        }
    }
}

void EnTite_Update(Actor* thisx, PlayState* play) {
    EnTite* this = (EnTite*)thisx;
    char pad[0x4];
    CollisionPoly* floorPoly;
    WaterBox* waterBox;
    f32 waterSurfaceY;

    EnTite_CheckDamage(thisx, play);
    // Stay still if hit by immunity damage type this frame
    if (thisx->colChkInfo.damageEffect != 0xE) {
        this->actionFunc(this, play);
        Actor_MoveForward(thisx);
        Actor_UpdateBgCheckInfo(play, thisx, 25.0f, 40.0f, 20.0f, this->unk_2DC);
        // If on water, snap feet to surface and spawn ripples
        if ((this->actor.params == TEKTITE_BLUE) && (thisx->bgCheckFlags & 0x20)) {
            floorPoly = thisx->floorPoly;
            if ((((play->gameplayFrames % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->backRightFootPos.x,
                                         this->backRightFootPos.z, &waterSurfaceY, &waterBox)) &&
                (this->backRightFootPos.y <= waterSurfaceY)) {
                this->backRightFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(play, &this->backRightFootPos, 0, 220, 0);
            }
            if (((((play->gameplayFrames + 2) % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->backLeftFootPos.x,
                                         this->backLeftFootPos.z, &waterSurfaceY, &waterBox)) &&
                (this->backLeftFootPos.y <= waterSurfaceY)) {
                this->backLeftFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(play, &this->backLeftFootPos, 0, 220, 0);
            }
            if (((((play->gameplayFrames + 4) % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->frontLeftFootPos.x,
                                         this->frontLeftFootPos.z, &waterSurfaceY, &waterBox)) &&
                (this->frontLeftFootPos.y <= waterSurfaceY)) {
                this->frontLeftFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(play, &this->frontLeftFootPos, 0, 220, 0);
            }
            if (((((play->gameplayFrames + 1) % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->frontRightFootPos.x,
                                         this->frontRightFootPos.z, &waterSurfaceY, &waterBox)) &&
                (this->frontRightFootPos.y <= waterSurfaceY)) {
                this->frontRightFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(play, &this->frontRightFootPos, 0, 220, 0);
            }
            thisx->floorPoly = floorPoly;
        }

        // If on ground and currently flipped over, set tektite to be fully upside-down
        if (thisx->bgCheckFlags & 3) {
            func_800359B8(thisx, thisx->shape.rot.y, &thisx->shape.rot);
            if (this->flipState >= TEKTITE_FLIPPED) {
                thisx->shape.rot.z += 0x7FFF;
            }
            // Otherwise ensure the tektite is rotating back upright
        } else {
            Math_SmoothStepToS(&thisx->shape.rot.x, 0, 1, 1000, 0);
            if (this->flipState <= TEKTITE_UNFLIPPED) {
                Math_SmoothStepToS(&thisx->shape.rot.z, 0, 1, 1000, 0);
                if (thisx->shape.yOffset > 0) {
                    thisx->shape.yOffset -= 400.0f;
                }
            }
        }
    }
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 20.0f;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnTite_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** limbDList, Vec3s* rot, void* thisx) {
    EnTite* this = (EnTite*)thisx;

    switch (limbIndex) {
        case 8:
            Matrix_MultVec3f(&sFootOffset, &this->backRightFootPos);
            break;
        case 13:
            Matrix_MultVec3f(&sFootOffset, &this->frontRightFootPos);
            break;
        case 18:
            Matrix_MultVec3f(&sFootOffset, &this->backLeftFootPos);
            break;
        case 23:
            Matrix_MultVec3f(&sFootOffset, &this->frontLeftFootPos);
            break;
    }

    BodyBreak_SetInfo(&this->bodyBreak, limbIndex, 0, 24, 24, limbDList, BODYBREAK_OBJECT_DEFAULT);
}

void EnTite_Draw(Actor* thisx, PlayState* play) {
    EnTite* this = (EnTite*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);
    Collider_UpdateSpheres(0, &this->collider);
    if (this->actor.params == TEKTITE_BLUE) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001300));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001700));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001900));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001B00));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001F00));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(object_tite_Tex_002100));
    }
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, EnTite_PostLimbDraw,
                      thisx);
    CLOSE_DISPS(play->state.gfxCtx);

    if (this->spawnIceTimer != 0) {
        // Spawn chunks of ice all over the tektite's body
        thisx->colorFilterTimer++;
        this->spawnIceTimer--;
        if ((this->spawnIceTimer & 3) == 0) {
            Vec3f iceChunk;
            s32 idx = this->spawnIceTimer >> 2;

            iceChunk.x = thisx->world.pos.x + sIceChunks[idx].x;
            iceChunk.y = thisx->world.pos.y + sIceChunks[idx].y;
            iceChunk.z = thisx->world.pos.z + sIceChunks[idx].z;
            EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &iceChunk, 150, 150, 150, 250, 235, 245, 255, 1.0f);
        }
    }
}

/*
 * File: z_en_neo_reeba.c
 * Overlay: ovl_En_Neo_Reeba
 * Description: (New) Leevers
 */

#include "z_en_neo_reeba.h"
#include "objects/object_rb/object_rb.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void EnNeoReeba_Init(Actor* thisx, PlayState* play);
void EnNeoReeba_Destroy(Actor* thisx, PlayState* play);
void EnNeoReeba_Update(Actor* thisx, PlayState* play);
void EnNeoReeba_Draw(Actor* thisx, PlayState* play);

void EnNeoReeba_SetupWaitUnderground(EnNeoReeba* this);
void EnNeoReeba_WaitUnderground(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_ChooseAction(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_SetupSink(EnNeoReeba* this);
void EnNeoReeba_Sink(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_SetupRise(EnNeoReeba* this);
void EnNeoReeba_RiseOutOfGround(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_SetupMove(EnNeoReeba* this);
void EnNeoReeba_Move(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_SetupReturnHome(EnNeoReeba* this);
void EnNeoReeba_ReturnHome(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_Bounce(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_Stunned(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_Frozen(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_SetupDamageAnim(EnNeoReeba* this);
void EnNeoReeba_DamageAnim(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_SetupDeathEffects(EnNeoReeba* this);
void EnNeoReeba_PlayDeathEffects(EnNeoReeba* this, PlayState* play);
void EnNeoReeba_SpawnIce(EnNeoReeba* this, PlayState* play);

ActorProfile En_Neo_Reeba_Profile = {
    /**/ ACTOR_EN_NEO_REEBA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_RB,
    /**/ sizeof(EnNeoReeba),
    /**/ EnNeoReeba_Init,
    /**/ EnNeoReeba_Destroy,
    /**/ EnNeoReeba_Update,
    /**/ EnNeoReeba_Draw,
};

typedef enum {
    /* 0x0 */ EN_NEO_REEBA_DMGEFF_IMMUNE, // No effect on the leever
    /* 0x1 */ EN_NEO_REEBA_DMGEFF_STUN,   // Turns blue, frozen in place
    /* 0x2 */ EN_NEO_REEBA_DMGEFF_FIRE,
    /* 0x3 */ EN_NEO_REEBA_DMGEFF_FREEZE, // Ice effects, frozen in place
    /* 0x4 */ EN_NEO_REEBA_DMGEFF_LIGHT,
    /* 0xC */ EN_NEO_REEBA_DMGEFF_HOOKSHOT = 0xC, // No damage, used only for hookshot
    /* 0xD */ EN_NEO_REEBA_DMGEFF_ELECTRIC_STUN,  // Electric effects, frozen in place
    /* 0xE */ EN_NEO_REEBA_DMGEFF_NONE,           // Normal hit
    /* 0xF */ EN_NEO_REEBA_DMGEFF_SHATTER         // Normal hit that can break ice if leever is frozen
} EnNeoReebaDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Explosives     */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Zora boomerang */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Hookshot       */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Sword          */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Goron pound    */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Fire arrow     */ DMG_ENTRY(2, EN_NEO_REEBA_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, EN_NEO_REEBA_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_NEO_REEBA_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Deku spin      */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Zora punch     */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Spin attack    */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
    /* Sword beam     */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_NEO_REEBA_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_NEO_REEBA_DMGEFF_SHATTER),
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT5,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x08, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 18, 30, 0, { 0, 0, 0 } },
};

void EnNeoReeba_Init(Actor* thisx, PlayState* play) {
    EnNeoReeba* this = (EnNeoReeba*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    SkelAnime_Init(play, &this->skelAnime, &gLeeverSkel, &gLeeverSpinAnim, this->jointTable, this->morphTable,
                   LEEVER_LIMB_MAX);

    if (!EN_NEO_REEBA_IS_LARGE(&this->actor)) {
        Actor_SetScale(&this->actor, 0.04f);
        this->actor.colChkInfo.mass = 90;
        this->actor.colChkInfo.health = 1;
    } else {
        Actor_SetScale(&this->actor, 0.05f);
        this->actor.colChkInfo.mass = 180;
        this->actor.colChkInfo.health = 3;
    }

    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.attentionRangeType = ATTENTION_RANGE_2;
    this->actor.hintId = TATL_HINT_ID_LEEVER;
    this->actor.gravity = -0.5f;

    this->targetPos = gZeroVec3f;
    this->velToTarget = gZeroVec3f;

    this->rotationAngle = 0;
    this->sfxTimer = 0;
    this->stunTimer = 0;
    this->actionTimer = 0;
    this->sinkRiseRate = 0.0f;
    this->rotationSpeed = 0.0f;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    EnNeoReeba_SetupWaitUnderground(this);
}

void EnNeoReeba_Destroy(Actor* thisx, PlayState* play) {
    EnNeoReeba* this = (EnNeoReeba*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnNeoReeba_SetupWaitUnderground(EnNeoReeba* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.draw = NULL;
    this->actionTimer = 10;
    this->actionFunc = EnNeoReeba_WaitUnderground;
    this->actor.shape.yOffset = -2000.0f;
}

void EnNeoReeba_WaitUnderground(EnNeoReeba* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if ((Actor_WorldDistXZToPoint(&player->actor, &this->actor.home.pos) < 200.0f) &&
        (Player_GetMask(play) != PLAYER_MASK_STONE) && (fabsf(this->actor.playerHeightRel) < 100.0f)) {
        EnNeoReeba_SetupRise(this);
    }

    if (this->actionTimer == 0) {
        this->actor.world.pos = this->actor.home.pos;
        this->actionTimer = -1;
    } else {
        this->actionTimer--;
    }
}

void EnNeoReeba_SetupChooseAction(EnNeoReeba* this) {
    this->actor.shape.yOffset = 0.0f;

    if (this->actionFunc == EnNeoReeba_Move) {
        this->actionTimer = 20;
    } else {
        this->actionTimer = 10;
    }

    this->actionFunc = EnNeoReeba_ChooseAction;
    this->skelAnime.playSpeed = 1.0f;
}

void EnNeoReeba_ChooseAction(EnNeoReeba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distToPlayer = Actor_WorldDistXZToPoint(&player->actor, &this->actor.home.pos);

    if ((distToPlayer > 200.0f) || (fabsf(this->actor.playerHeightRel) > 100.0f)) {
        EnNeoReeba_SetupSink(this);
    } else if (this->actionTimer == 0) {
        if ((distToPlayer < 140.0f) && (fabsf(this->actor.playerHeightRel) < 100.0f)) {
            this->targetPos = player->actor.world.pos;
            this->targetPos.x += 10.0f * player->actor.speed * Math_SinS(player->actor.world.rot.y);
            this->targetPos.z += 10.0f * player->actor.speed * Math_CosS(player->actor.world.rot.y);
            EnNeoReeba_SetupMove(this);
        } else {
            EnNeoReeba_SetupReturnHome(this);
        }
    }

    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
}

void EnNeoReeba_SetupSink(EnNeoReeba* this) {
    this->sinkRiseRate = 0.0f;
    this->skelAnime.playSpeed = 2.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
    this->actionFunc = EnNeoReeba_Sink;
}

void EnNeoReeba_Sink(EnNeoReeba* this, PlayState* play) {
    if (Math_SmoothStepToF(&this->actor.shape.yOffset, -2000.0f, 0.5f, this->sinkRiseRate, 10.0f) == 0.0f) {
        EnNeoReeba_SetupWaitUnderground(this);
    } else if ((play->gameplayFrames % 4) == 0) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                 500, 10, true);
    }

    if (this->sinkRiseRate < 300.0f) {
        this->sinkRiseRate += 20.0f;
    }

    Math_ApproachF(&this->actor.shape.shadowScale, 0.0f, 1.0f, 1.0f);
}

void EnNeoReeba_SetupRise(EnNeoReeba* this) {
    this->actor.draw = EnNeoReeba_Draw;
    this->sinkRiseRate = 300.0f;
    this->skelAnime.playSpeed = 2.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_APPEAR);
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnNeoReeba_RiseOutOfGround;
}

void EnNeoReeba_RiseOutOfGround(EnNeoReeba* this, PlayState* play) {
    if (Math_SmoothStepToF(&this->actor.shape.yOffset, 0.0f, 0.5f, this->sinkRiseRate, 10.0f) == 0.0f) {
        EnNeoReeba_SetupChooseAction(this);
    } else if (play->gameplayFrames % 4 == 0) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                 500, 10, true);
    }

    if (this->sinkRiseRate > 20.0f) {
        this->sinkRiseRate -= 10.0f;
    }

    Math_ApproachF(&this->actor.shape.shadowScale, 12.0f, 1.0f, 1.0f);
}

void EnNeoReeba_SetupMove(EnNeoReeba* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIVA_MOVE);
    this->sfxTimer = 10;
    this->actionTimer = 60;
    this->actionFunc = EnNeoReeba_Move;
    this->skelAnime.playSpeed = 2.0f;
    this->actor.speed = 14.0f;
}

void EnNeoReeba_Move(EnNeoReeba* this, PlayState* play) {
    f32 remainingDist = Math_Vec3f_StepToXZ(&this->actor.world.pos, &this->targetPos, this->actor.speed);

    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 4.0f, 0xFA,
                             10, true);

    if (remainingDist < 2.0f) {
        EnNeoReeba_SetupChooseAction(this);
    } else if ((remainingDist < 40.0f) && (this->actor.speed > 3.0f)) {
        this->actor.speed -= 2.0f;
    }

    if (this->sfxTimer == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_RIVA_MOVE);
        this->sfxTimer = 10;
    } else {
        this->sfxTimer--;
    }

    if (this->actionTimer == 0) {
        EnNeoReeba_SetupChooseAction(this);
    } else {
        this->actionTimer--;
    }
}

void EnNeoReeba_SetupReturnHome(EnNeoReeba* this) {
    this->actionFunc = EnNeoReeba_ReturnHome;
    this->actor.speed = 6.0f;
}

void EnNeoReeba_ReturnHome(EnNeoReeba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    f32 remainingDist = Math_Vec3f_StepToXZ(&this->actor.world.pos, &this->actor.home.pos, this->actor.speed);

    if (remainingDist < 2.0f) {
        EnNeoReeba_SetupChooseAction(this);
    } else if ((remainingDist < 40.0f) && (this->actor.speed > 3.0f)) {
        this->actor.speed -= 1.0f;
    }

    if ((Actor_WorldDistXZToPoint(&player->actor, &this->actor.home.pos) > 200.0f) ||
        (fabsf(this->actor.playerHeightRel) > 100.0f)) {
        EnNeoReeba_SetupSink(this);
    }
}

void EnNeoReeba_SetupBounce(EnNeoReeba* this) {
    this->actionTimer = 60;
    this->targetPos = this->actor.world.pos;
    this->targetPos.x -= 20.0f * Math_SinS(this->actor.yawTowardsPlayer);
    this->targetPos.z -= 20.0f * Math_CosS(this->actor.yawTowardsPlayer);
    this->actionFunc = EnNeoReeba_Bounce;
}

void EnNeoReeba_Bounce(EnNeoReeba* this, PlayState* play) {
    if (Math_Vec3f_StepToXZ(&this->actor.world.pos, &this->targetPos, this->actor.speed) < 2.0f) {
        EnNeoReeba_SetupChooseAction(this);
    }

    if (this->actionTimer == 0) {
        EnNeoReeba_SetupChooseAction(this);
    } else {
        this->actionTimer--;
    }
}

void EnNeoReeba_SetupStun(EnNeoReeba* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = EnNeoReeba_Stunned;
}

void EnNeoReeba_Stunned(EnNeoReeba* this, PlayState* play) {
    if (this->stunTimer > 0) {
        this->stunTimer--;
    } else {
        this->drawEffectAlpha = 0.0f;
        this->drawEffectScale = 0.0f;
    }

    if (this->actor.colorFilterTimer == 0) {
        this->stunTimer = 0;
        EnNeoReeba_SetupChooseAction(this);
    }
}

void EnNeoReeba_SetupFrozen(EnNeoReeba* this) {
    this->drawEffectType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
    this->drawEffectScale = 0.5f;
    this->drawEffectAlpha = 1.0f;

    if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
        this->drawEffectScale *= 1.5f;
    }

    if (this->actor.colChkInfo.health != 0) {
        this->stunTimer = 80;
    } else {
        this->stunTimer = 12;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = EnNeoReeba_Frozen;
}

void EnNeoReeba_Frozen(EnNeoReeba* this, PlayState* play) {
    if (this->stunTimer == 0) {
        this->stunTimer = 0;
        this->drawEffectScale = 0.0f;
        this->drawEffectAlpha = 0.0f;

        if (this->actor.colChkInfo.health != 0) {
            EnNeoReeba_SetupDamageAnim(this);
        } else {
            EnNeoReeba_SetupDeathEffects(this);
        }
    } else if (this->stunTimer == 1) {
        this->stunTimer--;
        EnNeoReeba_SpawnIce(this, play);
    } else {
        this->stunTimer--;
    }
}

void EnNeoReeba_SetupDamageAnim(EnNeoReeba* this) {
    this->actionTimer = 10;
    this->skelAnime.playSpeed = 1.0f;
    this->velToTarget.x = Math_SinS(this->actor.yawTowardsPlayer) * -12.0f;
    this->velToTarget.z = Math_CosS(this->actor.yawTowardsPlayer) * -12.0f;
    this->rotationSpeed = 4551.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 25);
    Actor_PlaySfx(&this->actor, NA_SE_EN_RIVA_DAMAGE);
    this->actionFunc = EnNeoReeba_DamageAnim;
}

static f32 sDamageAnimXZScales[] = {
    0.04f, 0.04f, 0.039f, 0.042f, 0.045f, 0.043f, 0.04f, 0.035f, 0.03f, 0.033f, 0.04f
};
static f32 sDamageAnimYScales[] = { 0.04f, 0.04f, 0.041f, 0.038f, 0.035f, 0.037f, 0.04f, 0.045f, 0.05f, 0.047f, 0.04f };

void EnNeoReeba_DamageAnim(EnNeoReeba* this, PlayState* play) {
    if (this->actionTimer == 0) {
        this->rotationSpeed = 0.0f;
        EnNeoReeba_SetupChooseAction(this);
    } else {
        this->rotationAngle -= 0x1F40;
        Math_SmoothStepToF(&this->rotationSpeed, 0.0f, 0.5f, 182.0f, 18.0f);
        this->actor.scale.x = this->actor.scale.z = sDamageAnimXZScales[this->actionTimer];
        this->actor.scale.y = sDamageAnimYScales[this->actionTimer];

        if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
            this->actor.scale.x *= 1.5f;
            this->actor.scale.y *= 1.5f;
            this->actor.scale.z *= 1.5f;
        }

        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 4.0f,
                                 250, 10, true);
        this->actionTimer--;
    }
}

void EnNeoReeba_SetupDeathEffects(EnNeoReeba* this) {
    this->actionTimer = 10;

    this->velToTarget.x = Math_SinS(this->actor.yawTowardsPlayer) * -12.0f;
    this->velToTarget.z = Math_CosS(this->actor.yawTowardsPlayer) * -12.0f;

    this->rotationSpeed = 3640.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 25);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    Actor_PlaySfx(&this->actor, NA_SE_EN_RIVA_DEAD);
    this->actionFunc = EnNeoReeba_PlayDeathEffects;
}

void EnNeoReeba_PlayDeathEffects(EnNeoReeba* this, PlayState* play) {
    static Vec3f sDeadDbVel = { 0.0f, 0.0f, 0.0f };
    static Vec3f sDeadDbAccel = { 0.0f, 4.0f, 0.0f };

    if (this->actionTimer == 0) {
        Math_ApproachZeroF(&this->actor.scale.x, 0.1f, 0.01f);
        Math_ApproachZeroF(&this->actor.scale.y, 0.1f, 0.01f);
        Math_ApproachZeroF(&this->actor.scale.z, 0.1f, 0.01f);
        Math_ApproachZeroF(&this->drawEffectAlpha, 0.1f, 0.1f);
        Math_ApproachZeroF(&this->drawEffectScale, 0.1f, 0.1f);

        if (this->actor.scale.x < 0.01f) {
            func_800B3030(play, &this->actor.world.pos, &sDeadDbAccel, &sDeadDbVel, 120, 0, 0);
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x60);
            Actor_Kill(&this->actor);
        }
    } else {
        if (this->actionTimer <= 10) {
            this->rotationAngle -= 0x1388;
            this->actor.scale.x = this->actor.scale.z = sDamageAnimXZScales[this->actionTimer];
            this->actor.scale.y = sDamageAnimYScales[this->actionTimer];

            if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
                this->actor.scale.x *= 1.5f;
                this->actor.scale.y *= 1.5f;
                this->actor.scale.z *= 1.5f;
            }
        }

        if (this->drawEffectType == ACTOR_DRAW_DMGEFF_FIRE) {
            f32 target = 1.2f;

            if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
                target *= 1.5f;
            }

            Math_SmoothStepToF(&this->drawEffectScale, target, 0.5f, 0.3f, 0.0f);
        } else {
            f32 target = 1.6f;

            if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
                target *= 1.5f;
            }

            Math_SmoothStepToF(&this->drawEffectScale, target, 0.5f, 0.3f, 0.0f);
        }
        this->actionTimer--;
    }
}

void EnNeoReeba_HandleHit(EnNeoReeba* this, PlayState* play) {
    Actor_SetFocus(&this->actor, 20.0f);

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actionFunc == EnNeoReeba_Frozen) {
            switch (this->actor.colChkInfo.damageEffect) {
                case EN_NEO_REEBA_DMGEFF_STUN:
                case EN_NEO_REEBA_DMGEFF_FREEZE:
                case EN_NEO_REEBA_DMGEFF_LIGHT:
                case EN_NEO_REEBA_DMGEFF_ELECTRIC_STUN:
                case EN_NEO_REEBA_DMGEFF_NONE:
                    return;

                default:
                    if (this->stunTimer >= 2) {
                        EnNeoReeba_SpawnIce(this, play);
                    }
                    this->stunTimer = 0;
            }
        }

        Actor_ApplyDamage(&this->actor);

        switch (this->actor.colChkInfo.damageEffect) {
            case EN_NEO_REEBA_DMGEFF_FREEZE:
                EnNeoReeba_SetupFrozen(this);
                break;

            case EN_NEO_REEBA_DMGEFF_FIRE:
            case EN_NEO_REEBA_DMGEFF_LIGHT:
                if (this->actor.colChkInfo.damageEffect == EN_NEO_REEBA_DMGEFF_FIRE) {
                    this->drawEffectType = ACTOR_DRAW_DMGEFF_FIRE;
                    this->stunTimer = 80;
                } else {
                    this->drawEffectType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    this->stunTimer = 40;
                }
                this->drawEffectAlpha = 1.0f;
                this->drawEffectScale = 0.0f;
                // fallthrough
            case EN_NEO_REEBA_DMGEFF_NONE:
            case EN_NEO_REEBA_DMGEFF_SHATTER:
                if ((this->actor.colChkInfo.damageEffect == EN_NEO_REEBA_DMGEFF_SHATTER) ||
                    (this->actor.colChkInfo.damageEffect == EN_NEO_REEBA_DMGEFF_NONE)) {
                    this->stunTimer = 0;
                }
                if (this->actor.colChkInfo.health > 0) {
                    EnNeoReeba_SetupDamageAnim(this);
                } else {
                    Enemy_StartFinishingBlow(play, &this->actor);
                    EnNeoReeba_SetupDeathEffects(this);
                }
                break;

            case EN_NEO_REEBA_DMGEFF_ELECTRIC_STUN:
                this->drawEffectType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                this->stunTimer = 40;
                this->drawEffectAlpha = 1.0f;
                this->drawEffectScale = 2.0f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                EnNeoReeba_SetupStun(this);
                break;

            case EN_NEO_REEBA_DMGEFF_STUN:
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                EnNeoReeba_SetupStun(this);
                break;

            default:
                break;
        }
    } else if (((this->collider.base.ocFlags2 & OC2_HIT_PLAYER) || (this->collider.base.atFlags & AT_BOUNCED)) &&
               (this->actionFunc == EnNeoReeba_Move)) {
        EnNeoReeba_SetupBounce(this);
    }

    if ((this->actionFunc != EnNeoReeba_WaitUnderground) && (this->actionFunc != EnNeoReeba_Sink) &&
        (this->actionFunc != EnNeoReeba_RiseOutOfGround) && (this->actionFunc != EnNeoReeba_DamageAnim) &&
        (this->actionFunc != EnNeoReeba_PlayDeathEffects)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

        if (this->actionFunc != EnNeoReeba_Stunned) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void EnNeoReeba_UpdatePosition(EnNeoReeba* this, PlayState* play) {
    if ((this->actionFunc != EnNeoReeba_WaitUnderground) && (this->actionFunc != EnNeoReeba_Sink) &&
        (this->actionFunc != EnNeoReeba_RiseOutOfGround)) {
        this->actor.velocity.y += this->actor.gravity;

        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.x += this->actor.colChkInfo.displacement.x;
        this->actor.world.pos.z += this->actor.colChkInfo.displacement.z;

        this->actor.world.pos.x += this->velToTarget.x;
        this->actor.world.pos.z += this->velToTarget.z;

        Math_ApproachZeroF(&this->velToTarget.x, 1.0f, 2.0f);
        Math_ApproachZeroF(&this->velToTarget.z, 1.0f, 2.0f);
        Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 40.0f, 40.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }
}

void EnNeoReeba_DrawFrozenEffects(EnNeoReeba* this, PlayState* play) {
    s32 i;
    f32 bodyPartPosScaleXZ = 10.0f;
    f32 phi_f2 = 20.0f;
    f32 drawEffectScale = 0.8f;

    if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
        bodyPartPosScaleXZ *= 1.5f;
        phi_f2 *= 1.5f;
        drawEffectScale *= 1.5f;
    }

    for (i = 0; i <= EN_NEO_REEBA_BODYPART_2; i++) {
        this->bodyPartsPos[i] = this->actor.world.pos;

        this->bodyPartsPos[i].x += bodyPartPosScaleXZ * Math_SinS(BINANG_ADD(this->actor.shape.rot.y, i * 0x5555));
        this->bodyPartsPos[i].z += bodyPartPosScaleXZ * Math_CosS(BINANG_ADD(this->actor.shape.rot.y, i * 0x5555));
        this->bodyPartsPos[i].y += 5.0f;
    }

    this->bodyPartsPos[EN_NEO_REEBA_BODYPART_3] = this->actor.world.pos;
    this->bodyPartsPos[EN_NEO_REEBA_BODYPART_3].y += phi_f2;

    this->drawEffectScale = drawEffectScale;
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, EN_NEO_REEBA_BODYPART_MAX, drawEffectScale, 0.5f,
                            this->drawEffectAlpha, this->drawEffectType);
}

void EnNeoReeba_DrawEffects(EnNeoReeba* this, PlayState* play) {
    s32 i;
    f32 scale = 15.0f;

    if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
        scale *= 1.5f;
    }

    if ((this->drawEffectType == ACTOR_DRAW_DMGEFF_FIRE) || (this->drawEffectType == ACTOR_DRAW_DMGEFF_LIGHT_ORBS) ||
        (this->drawEffectType == ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL)) {
        for (i = 0; i <= EN_NEO_REEBA_BODYPART_2; i++) {
            this->bodyPartsPos[i] = this->actor.world.pos;
            this->bodyPartsPos[i].x += scale * Math_SinS(BINANG_ADD(this->actor.shape.rot.y, i * 0x5555));
            this->bodyPartsPos[i].z += scale * Math_CosS(BINANG_ADD(this->actor.shape.rot.y, i * 0x5555));
            this->bodyPartsPos[i].y += -20.0f;
        }

        this->bodyPartsPos[EN_NEO_REEBA_BODYPART_3] = this->actor.world.pos;
        if (GameInteractor_Should(VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS, true, this, this->bodyPartsPos,
                                  EN_NEO_REEBA_BODYPART_MAX)) {
            Actor_DrawDamageEffects(play, NULL, this->bodyPartsPos, EN_NEO_REEBA_BODYPART_MAX, this->drawEffectScale,
                                    0.5f, this->drawEffectAlpha, this->drawEffectType);
        }
    }
}

void EnNeoReeba_SpawnIce(EnNeoReeba* this, PlayState* play) {
    static Color_RGBA8 sIcePrimColor = { 170, 255, 255, 255 };
    static Color_RGBA8 sIceEnvColor = { 200, 200, 255, 255 };
    static Vec3f sIceAccel = { 0.0f, -1.0f, 0.0f };
    Vec3f iceVelocity;
    f32 xVelocity;
    f32 zVelocity;
    s32 i;
    s16 yaw;
    s32 j;

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < EN_NEO_REEBA_BODYPART_MAX; i++) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->bodyPartsPos[i]);
        xVelocity = Math_SinS(yaw) * 3.0f;
        zVelocity = Math_CosS(yaw) * 3.0f;

        for (j = 0; j < 4; j++) {
            iceVelocity.x = (Rand_Centered() * 3.0f) + xVelocity;
            iceVelocity.z = (Rand_Centered() * 3.0f) + zVelocity;
            iceVelocity.y = (Rand_ZeroOne() * 6.0f) + 4.0f;
            EffectSsEnIce_Spawn(play, &this->bodyPartsPos[i], 0.7f, &iceVelocity, &sIceAccel, &sIcePrimColor,
                                &sIceEnvColor, 30);
        }
    }
}

void EnNeoReeba_SinkIfStoneMask(EnNeoReeba* this, PlayState* play) {
    if ((this->actionFunc == EnNeoReeba_ChooseAction) || (this->actionFunc == EnNeoReeba_Move) ||
        (this->actionFunc == EnNeoReeba_ReturnHome)) {
        if (Player_GetMask(play) == PLAYER_MASK_STONE) {
            EnNeoReeba_SetupSink(this);
        }
    }
}

void EnNeoReeba_Update(Actor* thisx, PlayState* play) {
    EnNeoReeba* this = (EnNeoReeba*)thisx;

    if (EN_NEO_REEBA_IS_LARGE(&this->actor)) {
        this->collider.dim.radius = 27;
        this->collider.dim.height = 45;
    }

    this->actionFunc(this, play);

    if ((this->actionFunc != EnNeoReeba_WaitUnderground) && (this->actionFunc != EnNeoReeba_Stunned) &&
        (this->actionFunc != EnNeoReeba_Frozen)) {
        SkelAnime_Update(&this->skelAnime);
    }

    EnNeoReeba_HandleHit(this, play);
    EnNeoReeba_UpdatePosition(this, play);
    EnNeoReeba_SinkIfStoneMask(this, play);
}

s32 EnNeoReeba_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnNeoReeba* this = (EnNeoReeba*)thisx;

    if ((limbIndex == OBJECT_RB_LIMB_03) && (this->rotationSpeed != 0.0f)) {
        rot->y += TRUNCF_BINANG(this->rotationSpeed * Math_SinS(this->rotationAngle));
        rot->z += TRUNCF_BINANG(this->rotationSpeed * Math_CosS(this->rotationAngle));
    }

    return false;
}

void EnNeoReeba_Draw(Actor* thisx, PlayState* play) {
    EnNeoReeba* this = (EnNeoReeba*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x01, 255, 255, 255, 255);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnNeoReeba_OverrideLimbDraw, NULL,
                      &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);

    if (this->stunTimer > 0) {
        if (this->drawEffectType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) {
            EnNeoReeba_DrawFrozenEffects(this, play);
        } else {
            EnNeoReeba_DrawEffects(this, play);
        }
    }
}

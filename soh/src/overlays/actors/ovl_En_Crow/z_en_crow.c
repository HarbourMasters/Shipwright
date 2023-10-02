#include "z_en_crow.h"
#include "objects/object_crow/object_crow.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_ARROW_DRAGGABLE)

void EnCrow_Init(Actor* thisx, PlayState* play);
void EnCrow_Destroy(Actor* thisx, PlayState* play);
void EnCrow_Update(Actor* thisx, PlayState* play);
void EnCrow_Draw(Actor* thisx, PlayState* play);

void EnCrow_SetupFlyIdle(EnCrow* this);
void EnCrow_FlyIdle(EnCrow* this, PlayState* play);
void EnCrow_Respawn(EnCrow* this, PlayState* play);
void EnCrow_DiveAttack(EnCrow* this, PlayState* play);
void EnCrow_Die(EnCrow* this, PlayState* play);
void EnCrow_TurnAway(EnCrow* this, PlayState* play);
void EnCrow_Damaged(EnCrow* this, PlayState* play);

static Vec3f sZeroVecAccel = { 0.0f, 0.0f, 0.0f };

const ActorInit En_Crow_InitVars = {
    ACTOR_EN_CROW,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_CROW,
    sizeof(EnCrow),
    (ActorFunc)EnCrow_Init,
    (ActorFunc)EnCrow_Destroy,
    (ActorFunc)EnCrow_Update,
    (ActorFunc)EnCrow_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_HARD,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 30, 30 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x3),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
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

static u32 sDeathCount = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 3000, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x58, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -200, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_STOP),
};

static Vec3f sHeadVec = { 2500.0f, 0.0f, 0.0f };

void EnCrow_Init(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGuaySkel, &gGuayFlyAnim, this->jointTable, this->morphTable, 9);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);
    this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 2000.0f, ActorShadow_DrawCircle, 20.0f);
    sDeathCount = 0;
    EnCrow_SetupFlyIdle(this);
}

void EnCrow_Destroy(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    Collider_DestroyJntSph(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

// Setup Action functions

void EnCrow_SetupFlyIdle(EnCrow* this) {
    this->timer = 100;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = EnCrow_FlyIdle;
    this->skelAnime.playSpeed = 1.0f;
}

void EnCrow_SetupDiveAttack(EnCrow* this) {
    this->timer = 300;
    this->actor.speedXZ = 4.0f;
    this->skelAnime.playSpeed = 2.0f;
    this->actionFunc = EnCrow_DiveAttack;
}

void EnCrow_SetupDamaged(EnCrow* this, PlayState* play) {
    s32 i;
    f32 scale;
    Vec3f iceParticlePos;

    this->actor.speedXZ *= Math_CosS(this->actor.world.rot.x);
    this->actor.velocity.y = 0.0f;
    Animation_Change(&this->skelAnime, &gGuayFlyAnim, 0.4f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    scale = this->actor.scale.x * 100.0f;
    this->actor.world.pos.y += 20.0f * scale;
    this->actor.bgCheckFlags &= ~1;
    this->actor.shape.yOffset = 0.0f;
    this->actor.targetArrowOffset = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_KAICHO_DEAD);

    if (this->actor.colChkInfo.damageEffect == 3) { // Ice arrows
        Actor_SetColorFilter(&this->actor, 0, 255, 0, 40);
        for (i = 0; i < 8; i++) {
            iceParticlePos.x = ((i & 1 ? 7.0f : -7.0f) * scale) + this->actor.world.pos.x;
            iceParticlePos.y = ((i & 2 ? 7.0f : -7.0f) * scale) + this->actor.world.pos.y;
            iceParticlePos.z = ((i & 4 ? 7.0f : -7.0f) * scale) + this->actor.world.pos.z;
            EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &iceParticlePos, 150, 150, 150, 250, 235, 245, 255,
                                           ((Rand_ZeroOne() * 0.15f) + 0.85f) * scale);
        }
    } else if (this->actor.colChkInfo.damageEffect == 2) { // Fire arrows and Din's Fire
        Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 40);

        for (i = 0; i < 4; i++) {
            EffectSsEnFire_SpawnVec3f(play, &this->actor, &this->actor.world.pos, 50.0f * scale, 0, 0, i);
        }
    } else {
        Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 40);
    }

    if (this->actor.flags & ACTOR_FLAG_DRAGGED_BY_ARROW) {
        this->actor.speedXZ = 0.0f;
    }

    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;

    this->actionFunc = EnCrow_Damaged;
}

void EnCrow_SetupDie(EnCrow* this) {
    this->actor.colorFilterTimer = 0;
    this->actionFunc = EnCrow_Die;
    GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
}

void EnCrow_SetupTurnAway(EnCrow* this) {
    this->timer = 100;
    this->actor.speedXZ = 3.5f;
    this->aimRotX = -0x1000;
    this->aimRotY = this->actor.yawTowardsPlayer + 0x8000;
    this->skelAnime.playSpeed = 2.0f;
    Actor_SetColorFilter(&this->actor, 0, 255, 0, 5);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = EnCrow_TurnAway;
}

void EnCrow_SetupRespawn(EnCrow* this) {
    if (sDeathCount == 10) {
        this->actor.params = 1;
        sDeathCount = 0;
        this->collider.elements[0].dim.worldSphere.radius =
            sJntSphInit.elements[0].dim.modelSphere.radius * 0.03f * 100.0f;
    } else {
        this->actor.params = 0;
        this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    }

    Animation_PlayLoop(&this->skelAnime, &gGuayFlyAnim);
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->timer = 300;
    this->actor.shape.yOffset = 2000;
    this->actor.targetArrowOffset = 2000.0f;
    this->actor.draw = NULL;
    this->actionFunc = EnCrow_Respawn;
}

// Action functions

void EnCrow_FlyIdle(EnCrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 skelanimeUpdated;
    s16 var;

    SkelAnime_Update(&this->skelAnime);
    skelanimeUpdated = Animation_OnFrame(&this->skelAnime, 0.0f);
    this->actor.speedXZ = (Rand_ZeroOne() * 1.5f) + 3.0f;

    if (this->actor.bgCheckFlags & 8) {
        this->aimRotY = this->actor.wallYaw;
    } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 300.0f) {
        this->aimRotY = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    }

    if ((Math_SmoothStepToS(&this->actor.shape.rot.y, this->aimRotY, 5, 0x300, 0x10) == 0) && skelanimeUpdated &&
        (Rand_ZeroOne() < 0.1f)) {
        var = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos) - this->actor.shape.rot.y;
        if (var > 0) {
            this->aimRotY += 0x1000 + (0x1000 * Rand_ZeroOne());
        } else {
            this->aimRotY -= 0x1000 + (0x1000 * Rand_ZeroOne());
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_KAICHO_CRY);
    }

    if (this->actor.yDistToWater > -40.0f) {
        this->aimRotX = -0x1000;
    } else if (this->actor.world.pos.y < (this->actor.home.pos.y - 50.0f)) {
        this->aimRotX = -0x800 - (Rand_ZeroOne() * 0x800);
    } else if (this->actor.world.pos.y > (this->actor.home.pos.y + 50.0f)) {
        this->aimRotX = 0x800 + (Rand_ZeroOne() * 0x800);
    }

    if ((Math_SmoothStepToS(&this->actor.shape.rot.x, this->aimRotX, 10, 0x100, 8) == 0) && (skelanimeUpdated) &&
        (Rand_ZeroOne() < 0.1f)) {
        if (this->actor.home.pos.y < this->actor.world.pos.y) {
            this->aimRotX -= (0x400 * Rand_ZeroOne()) + 0x400;
        } else {
            this->aimRotX += (0x400 * Rand_ZeroOne()) + 0x400;
        }
        this->aimRotX = CLAMP(this->aimRotX, -0x1000, 0x1000);
    }

    if (this->actor.bgCheckFlags & 1) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x100, 0x400);
    }

    if (this->timer != 0) {
        this->timer--;
    }
    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 300.0f) && !(player->stateFlags1 & 0x00800000) &&
        (this->actor.yDistToWater < -40.0f) && (Player_GetMask(play) != PLAYER_MASK_SKULL)) {
        EnCrow_SetupDiveAttack(this);
    }
}

void EnCrow_DiveAttack(EnCrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 facingPlayer;
    Vec3f pos;
    s16 target;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    facingPlayer = Actor_IsFacingPlayer(&this->actor, 0x2800);

    if (facingPlayer) {
        pos.x = player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y + 20.0f;
        pos.z = player->actor.world.pos.z;
        target = Actor_WorldPitchTowardPoint(&this->actor, &pos);
        if (target > 0x3000) {
            target = 0x3000;
        }
        Math_ApproachS(&this->actor.shape.rot.x, target, 2, 0x400);
    } else {
        Math_ApproachS(&this->actor.shape.rot.x, -0x1000, 2, 0x100);
    }

    if (facingPlayer || (this->actor.xzDistToPlayer > 80.0f)) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00);
    }

    if ((this->timer == 0) || (Player_GetMask(play) == PLAYER_MASK_SKULL) ||
        (this->collider.base.atFlags & AT_HIT) || (this->actor.bgCheckFlags & 9) ||
        (player->stateFlags1 & 0x00800000) || (this->actor.yDistToWater > -40.0f)) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_KAICHO_ATTACK);
        }

        EnCrow_SetupFlyIdle(this);
    }
}

void EnCrow_Damaged(EnCrow* this, PlayState* play) {
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
    this->actor.colorFilterTimer = 40;

    if (!(this->actor.flags & ACTOR_FLAG_DRAGGED_BY_ARROW)) {
        if (this->actor.colorFilterParams & 0x4000) {
            Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4000, 0x200);
            this->actor.shape.rot.z += 0x1780;
        }
        if ((this->actor.bgCheckFlags & 1) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
            EffectSsDeadDb_Spawn(play, &this->actor.world.pos, &sZeroVecAccel, &sZeroVecAccel,
                                 this->actor.scale.x * 10000.0f, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, 1);
            EnCrow_SetupDie(this);
        }
    }
}

void EnCrow_Die(EnCrow* this, PlayState* play) {
    f32 step;

    if (this->actor.params != 0) {
        step = 0.006f;
    } else {
        step = 0.002f;
    }

    if (Math_StepToF(&this->actor.scale.x, 0.0f, step)) {
        if (this->actor.params == 0) {
            sDeathCount++;
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0);
        } else {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_RUPEE_RED);
        }
        if (!CVarGetInteger("gRandomizedEnemies", 0)) {
            EnCrow_SetupRespawn(this);
        } else {
            Actor_Kill(this);
        }
    }

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
}

void EnCrow_TurnAway(EnCrow* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.bgCheckFlags & 8) {
        this->aimRotY = this->actor.wallYaw;
    } else {
        this->aimRotY = this->actor.yawTowardsPlayer + 0x8000;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->aimRotY, 3, 0xC00);
    Math_ApproachS(&this->actor.shape.rot.x, this->aimRotX, 5, 0x100);

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        EnCrow_SetupFlyIdle(this);
    }
}

void EnCrow_Respawn(EnCrow* this, PlayState* play) {
    f32 target;

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        SkelAnime_Update(&this->skelAnime);
        this->actor.draw = EnCrow_Draw;
        if (this->actor.params != 0) {
            target = 0.03f;
        } else {
            target = 0.01f;
        }
        if (Math_StepToF(&this->actor.scale.x, target, target * 0.1f)) {
            this->actor.flags |= ACTOR_FLAG_TARGETABLE;
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_WHILE_CULLED;
            this->actor.colChkInfo.health = 1;
            EnCrow_SetupFlyIdle(this);
        }
        this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    }
}

void EnCrow_UpdateDamage(EnCrow* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elements[0].info, 1);
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (this->actor.colChkInfo.damageEffect == 1) { // Deku Nuts
                EnCrow_SetupTurnAway(this);
            } else {
                Actor_ApplyDamage(&this->actor);
                this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
                Enemy_StartFinishingBlow(play, &this->actor);
                EnCrow_SetupDamaged(this, play);
            }
        }
    }
}

void EnCrow_Update(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;
    f32 pad;
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
            func_8002D97C(&this->actor);
        } else {
            height = 0.0f;
            Actor_MoveForward(&this->actor);
        }
        Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f * scale, 25.0f * scale, 50.0f * scale, 7);
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

    if (this->actor.colChkInfo.health != 0 && Animation_OnFrame(&this->skelAnime, 3.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_KAICHO_FLUTTER);
    }
}

s32 EnCrow_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnCrow* this = (EnCrow*)thisx;

    if (this->actor.colChkInfo.health != 0) {
        if (limbIndex == 7) {
            rot->y += 0xC00 * sinf(this->skelAnime.curFrame * (M_PI / 4));
        } else if (limbIndex == 8) {
            rot->y += 0x1400 * sinf((this->skelAnime.curFrame + 2.5f) * (M_PI / 4));
        }
    }
    return false;
}

void EnCrow_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnCrow* this = (EnCrow*)thisx;
    Vec3f* vec;

    if (limbIndex == 2) {
        Matrix_MultVec3f(&sHeadVec, &this->bodyPartsPos[0]);
        this->bodyPartsPos[0].y -= 20.0f;
    } else if ((limbIndex == 4) || (limbIndex == 6) || (limbIndex == 8)) {
        vec = &this->bodyPartsPos[(limbIndex >> 1) - 1];
        Matrix_MultVec3f(&sZeroVecAccel, vec);
        vec->y -= 20.0f;
    }
}

void EnCrow_Draw(Actor* thisx, PlayState* play) {
    EnCrow* this = (EnCrow*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnCrow_OverrideLimbDraw, EnCrow_PostLimbDraw, this);
}

/*
 * File: z_en_firefly.c
 * Overlay: ovl_En_Firefly
 * Description: Keese (Normal, Fire, Ice)
 */

#include "z_en_firefly.h"
#include "objects/object_firefly/object_firefly.h"
#include "overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_12 | ACTOR_FLAG_14)

void EnFirefly_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFirefly_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFirefly_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFirefly_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnFirefly_DrawInvisible(Actor* thisx, GlobalContext* globalCtx);

void EnFirefly_FlyIdle(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_Fall(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_Die(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_DiveAttack(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_Rebound(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_FlyAway(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_Stunned(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_FrozenFall(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_Perch(EnFirefly* this, GlobalContext* globalCtx);
void EnFirefly_DisturbDiveAttack(EnFirefly* this, GlobalContext* globalCtx);

typedef enum {
    /* 0 */ KEESE_AURA_NONE,
    /* 1 */ KEESE_AURA_FIRE,
    /* 2 */ KEESE_AURA_ICE
} KeeseAuraType;

const ActorInit En_Firefly_InitVars = {
    ACTOR_EN_FIREFLY,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_FIREFLY,
    sizeof(EnFirefly),
    (ActorFunc)EnFirefly_Init,
    (ActorFunc)EnFirefly_Destroy,
    (ActorFunc)EnFirefly_Update,
    (ActorFunc)EnFirefly_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_HARD,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 1000, 0 }, 15 }, 100 },
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

static CollisionCheckInfoInit sColChkInfoInit = { 1, 10, 10, 30 };

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
    /* Fire arrow    */ DMG_ENTRY(2, 0xF),
    /* Ice arrow     */ DMG_ENTRY(4, 0x3),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x2),
    /* Ice magic     */ DMG_ENTRY(4, 0x3),
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

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 5, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -4, ICHAIN_CONTINUE),    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 4000, ICHAIN_STOP),
};

void EnFirefly_Extinguish(EnFirefly* this) {
    this->actor.params += 2;
    this->collider.elements[0].info.toucher.effect = 0; // None
    this->auraType = KEESE_AURA_NONE;
    this->onFire = false;
    this->actor.naviEnemyId = 0x12; // Keese
}

void EnFirefly_Ignite(EnFirefly* this) {
    if (this->actor.params == KEESE_ICE_FLY) {
        this->actor.params = KEESE_FIRE_FLY;
    } else {
        this->actor.params -= 2;
    }
    this->collider.elements[0].info.toucher.effect = 1; // Fire
    this->auraType = KEESE_AURA_FIRE;
    this->onFire = true;
    this->actor.naviEnemyId = 0x11; // Fire Keese
}

void EnFirefly_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFirefly* this = (EnFirefly*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_Init(globalCtx, &this->skelAnime, &gKeeseSkeleton, &gKeeseFlyAnim, this->jointTable, this->morphTable,
                   28);
    Collider_InitJntSph(globalCtx, &this->collider);
    Collider_SetJntSph(globalCtx, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if ((this->actor.params & 0x8000) != 0) {
        this->actor.flags |= ACTOR_FLAG_7;
        this->actor.draw = EnFirefly_DrawInvisible;
        this->actor.params &= 0x7FFF;
    }

    if (this->actor.params <= KEESE_FIRE_PERCH) {
        this->onFire = true;
    } else {
        this->onFire = false;
    }

    if (this->onFire) {
        this->actionFunc = EnFirefly_FlyIdle;
        this->timer = Rand_S16Offset(20, 60);
        this->actor.shape.rot.x = 0x1554;
        this->auraType = KEESE_AURA_FIRE;
        this->actor.naviEnemyId = 0x11; // Fire Keese
        this->maxAltitude = this->actor.home.pos.y;
    } else {
        if (this->actor.params == KEESE_NORMAL_PERCH) {
            this->actionFunc = EnFirefly_Perch;
        } else {
            this->actionFunc = EnFirefly_FlyIdle;
        }

        if (this->actor.params == KEESE_ICE_FLY) {
            this->collider.elements[0].info.toucher.effect = 2; // Ice
            this->actor.naviEnemyId = 0x56;                     // Ice Keese
        } else {
            this->collider.elements[0].info.toucher.effect = 0; // Nothing
            this->actor.naviEnemyId = 0x12;                     // Keese
        }

        this->maxAltitude = this->actor.home.pos.y + 100.0f;

        if (this->actor.params == KEESE_ICE_FLY) {
            this->auraType = KEESE_AURA_ICE;
        } else {
            this->auraType = KEESE_AURA_NONE;
        }
    }

    this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
}

void EnFirefly_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFirefly* this = (EnFirefly*)thisx;

    Collider_DestroyJntSph(globalCtx, &this->collider);
}

void EnFirefly_SetupFlyIdle(EnFirefly* this) {
    this->timer = Rand_S16Offset(70, 100);
    this->actor.speedXZ = (Rand_ZeroOne() * 1.5f) + 1.5f;
    Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), 0x300);
    this->targetPitch = ((this->maxAltitude < this->actor.world.pos.y) ? 0xC00 : -0xC00) + 0x1554;
    this->skelAnime.playSpeed = 1.0f;
    this->actionFunc = EnFirefly_FlyIdle;
}

void EnFirefly_SetupFall(EnFirefly* this) {
    this->timer = 40;
    this->actor.velocity.y = 0.0f;
    Animation_Change(&this->skelAnime, &gKeeseFlyAnim, 0.5f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FFLY_DEAD);
    this->actor.flags |= ACTOR_FLAG_4;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 40);
    this->actionFunc = EnFirefly_Fall;
}

void EnFirefly_SetupDie(EnFirefly* this) {
    this->timer = 15;
    this->actor.speedXZ = 0.0f;
    this->actionFunc = EnFirefly_Die;
}

void EnFirefly_SetupRebound(EnFirefly* this) {
    this->actor.world.rot.x = 0x7000;
    this->timer = 18;
    this->skelAnime.playSpeed = 1.0f;
    this->actor.speedXZ = 2.5f;
    this->actionFunc = EnFirefly_Rebound;
}

void EnFirefly_SetupDiveAttack(EnFirefly* this) {
    this->timer = Rand_S16Offset(70, 100);
    this->skelAnime.playSpeed = 1.0f;
    this->targetPitch = ((this->actor.yDistToPlayer > 0.0f) ? -0xC00 : 0xC00) + 0x1554;
    this->actionFunc = EnFirefly_DiveAttack;
}

void EnFirefly_SetupFlyAway(EnFirefly* this) {
    this->timer = 150;
    this->skelAnime.playSpeed = 1.0f;
    this->targetPitch = 0x954;
    this->actionFunc = EnFirefly_FlyAway;
}

void EnFirefly_SetupStunned(EnFirefly* this) {
    this->timer = 80;
    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 80);
    this->auraType = KEESE_AURA_NONE;
    this->actor.velocity.y = 0.0f;
    this->skelAnime.playSpeed = 3.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = EnFirefly_Stunned;
}

void EnFirefly_SetupFrozenFall(EnFirefly* this, GlobalContext* globalCtx) {
    s32 i;
    Vec3f iceParticlePos;

    this->actor.flags |= ACTOR_FLAG_4;
    this->auraType = KEESE_AURA_NONE;
    this->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 0xFF);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FFLY_DEAD);

    for (i = 0; i <= 7; i++) {
        iceParticlePos.x = (i & 1 ? 7.0f : -7.0f) + this->actor.world.pos.x;
        iceParticlePos.y = (i & 2 ? 7.0f : -7.0f) + this->actor.world.pos.y;
        iceParticlePos.z = (i & 4 ? 7.0f : -7.0f) + this->actor.world.pos.z;
        EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &this->actor, &iceParticlePos, 150, 150, 150, 250, 235, 245, 255,
                                       (Rand_ZeroOne() * 0.15f) + 0.85f);
    }

    this->actionFunc = EnFirefly_FrozenFall;
}

void EnFirefly_SetupPerch(EnFirefly* this) {
    this->timer = 1;
    this->actor.speedXZ = 0.0f;
    this->actionFunc = EnFirefly_Perch;
}

void EnFirefly_SetupDisturbDiveAttack(EnFirefly* this) {
    this->skelAnime.playSpeed = 3.0f;
    this->actor.shape.rot.x = 0x1554;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->actor.speedXZ = 3.0f;
    this->timer = 50;
    this->actionFunc = EnFirefly_DisturbDiveAttack;
}

s32 EnFirefly_ReturnToPerch(EnFirefly* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 distFromHome;

    if (this->actor.params != KEESE_NORMAL_PERCH) {
        return 0;
    }

    if (Actor_WorldDistXZToPoint(&player->actor, &this->actor.home.pos) > 300.0f) {
        distFromHome = Actor_WorldDistXYZToPoint(&this->actor, &this->actor.home.pos);

        if (distFromHome < 5.0f) {
            EnFirefly_SetupPerch(this);
            return 1;
        }

        distFromHome *= 0.05f;

        if (distFromHome < 1.0f) {
            this->actor.speedXZ *= distFromHome;
        }

        Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           0x300);
        Math_ScaledStepToS(&this->actor.shape.rot.x,
                           Actor_WorldPitchTowardPoint(&this->actor, &this->actor.home.pos) + 0x1554, 0x100);
        return 1;
    }

    return 0;
}

s32 EnFirefly_SeekTorch(EnFirefly* this, GlobalContext* globalCtx) {
    ObjSyokudai* findTorch;
    ObjSyokudai* closestTorch;
    f32 torchDist;
    f32 currentMinDist;
    Vec3f flamePos;

    findTorch = (ObjSyokudai*)globalCtx->actorCtx.actorLists[ACTORCAT_PROP].head;
    closestTorch = NULL;
    currentMinDist = 35000.0f;

    while (findTorch != NULL) {
        if ((findTorch->actor.id == ACTOR_OBJ_SYOKUDAI) && (findTorch->litTimer != 0)) {
            torchDist = Actor_WorldDistXYZToActor(&this->actor, &findTorch->actor);
            if (torchDist < currentMinDist) {
                currentMinDist = torchDist;
                closestTorch = findTorch;
            }
        }
        findTorch = (ObjSyokudai*)findTorch->actor.next;
    }

    if (closestTorch != NULL) {
        flamePos.x = closestTorch->actor.world.pos.x;
        flamePos.y = closestTorch->actor.world.pos.y + 52.0f + 15.0f;
        flamePos.z = closestTorch->actor.world.pos.z;
        if (Actor_WorldDistXYZToPoint(&this->actor, &flamePos) < 15.0f) {
            EnFirefly_Ignite(this);
            return 1;
        } else {
            Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, &closestTorch->actor),
                               0x300);
            Math_ScaledStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &flamePos) + 0x1554,
                               0x100);
            return 1;
        }
    }
    return 0;
}

void EnFirefly_FlyIdle(EnFirefly* this, GlobalContext* globalCtx) {
    s32 skelanimeUpdated;
    f32 rand;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }
    skelanimeUpdated = Animation_OnFrame(&this->skelAnime, 0.0f);
    this->actor.speedXZ = (Rand_ZeroOne() * 1.5f) + 1.5f;
    if (this->onFire || (this->actor.params == KEESE_ICE_FLY) ||
        ((EnFirefly_ReturnToPerch(this, globalCtx) == 0) && (EnFirefly_SeekTorch(this, globalCtx) == 0))) {
        if (skelanimeUpdated) {
            rand = Rand_ZeroOne();
            if (rand < 0.5f) {
                Math_ScaledStepToS(&this->actor.shape.rot.y,
                                   Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), 0x300);
            } else if (rand < 0.8f) {
                this->actor.shape.rot.y += Rand_CenteredFloat(1536.0f);
            }
            // Climb if too close to ground
            if (this->actor.world.pos.y < (this->actor.floorHeight + 20.0f)) {
                this->targetPitch = 0x954;
                // Descend if above maxAltitude
            } else if (this->maxAltitude < this->actor.world.pos.y) {
                this->targetPitch = 0x2154;
                // Otherwise ascend or descend at random, biased towards ascending
            } else if (0.35f < Rand_ZeroOne()) {
                this->targetPitch = 0x954;
            } else {
                this->targetPitch = 0x2154;
            }
        } else {
            if (this->actor.bgCheckFlags & 1) {
                this->targetPitch = 0x954;
            } else if ((this->actor.bgCheckFlags & 0x10) || (this->maxAltitude < this->actor.world.pos.y)) {
                this->targetPitch = 0x2154;
            }
        }
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->targetPitch, 0x100);
    }
    if (this->actor.bgCheckFlags & 8) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
    }
    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 200.0f) &&
        (Player_GetMask(globalCtx) != PLAYER_MASK_SKULL)) {
        EnFirefly_SetupDiveAttack(this);
    }
}

// Fall to the ground after being hit
void EnFirefly_Fall(EnFirefly* this, GlobalContext* globalCtx) {
    if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        this->skelAnime.playSpeed = 0.0f;
    }
    this->actor.colorFilterTimer = 40;
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
    if (this->actor.flags & ACTOR_FLAG_15) {
        this->actor.colorFilterTimer = 40;
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x6800, 0x200);
        this->actor.shape.rot.y = this->actor.shape.rot.y - 0x300;
        if (this->timer != 0) {
            this->timer--;
        }
        if ((this->actor.bgCheckFlags & 1) || (this->timer == 0)) {
            EnFirefly_SetupDie(this);
        }
    }
}

// Hit the ground or burn up, spawn drops
void EnFirefly_Die(EnFirefly* this, GlobalContext* globalCtx) {
    if (this->timer != 0) {
        this->timer--;
    }
    Math_StepToF(&this->actor.scale.x, 0.0f, 0.00034f);
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
    if (this->timer == 0) {
        Item_DropCollectibleRandom(globalCtx, &this->actor, &this->actor.world.pos, 0xE0);
        Actor_Kill(&this->actor);
    }
}

void EnFirefly_DiveAttack(EnFirefly* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f preyPos;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }
    Math_StepToF(&this->actor.speedXZ, 4.0f, 0.5f);
    if (this->actor.bgCheckFlags & 8) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->targetPitch, 0x100);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2800)) {
        if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
            this->skelAnime.playSpeed = 0.0f;
            this->skelAnime.curFrame = 4.0f;
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xC00, 0x300);
        preyPos.x = player->actor.world.pos.x;
        preyPos.y = player->actor.world.pos.y + 20.0f;
        preyPos.z = player->actor.world.pos.z;
        Math_SmoothStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &preyPos) + 0x1554, 2,
                           0x400, 0x100);
    } else {
        this->skelAnime.playSpeed = 1.5f;
        if (this->actor.xzDistToPlayer > 80.0f) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xC00, 0x300);
        }
        if (this->actor.bgCheckFlags & 1) {
            this->targetPitch = 0x954;
        }
        if ((this->actor.bgCheckFlags & 0x10) || (this->maxAltitude < this->actor.world.pos.y)) {
            this->targetPitch = 0x2154;
        } else {
            this->targetPitch = 0x954;
        }
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->targetPitch, 0x100);
    }
    if ((this->timer == 0) || (Player_GetMask(globalCtx) == PLAYER_MASK_SKULL)) {
        EnFirefly_SetupFlyAway(this);
    }
}

// Knockback after hitting player
void EnFirefly_Rebound(EnFirefly* this, GlobalContext* globalCtx) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x100);
    Math_StepToF(&this->actor.velocity.y, 0.0f, 0.4f);
    if (Math_StepToF(&this->actor.speedXZ, 0.0f, 0.15f)) {
        if (this->timer != 0) {
            this->timer--;
        }
        if (this->timer == 0) {
            EnFirefly_SetupFlyAway(this);
        }
    }
}

void EnFirefly_FlyAway(EnFirefly* this, GlobalContext* globalCtx) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }
    if (((fabsf(this->actor.world.pos.y - this->maxAltitude) < 10.0f) &&
         (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) < 20.0f)) ||
        (this->timer == 0)) {
        EnFirefly_SetupFlyIdle(this);
        return;
    }
    Math_StepToF(&this->actor.speedXZ, 3.0f, 0.3f);
    if (this->actor.bgCheckFlags & 1) {
        this->targetPitch = 0x954;
    } else if ((this->actor.bgCheckFlags & 0x10) || (this->maxAltitude < this->actor.world.pos.y)) {
        this->targetPitch = 0x2154;
    } else {
        this->targetPitch = 0x954;
    }
    if (this->actor.bgCheckFlags & 8) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           0x300);
    }
    Math_ScaledStepToS(&this->actor.shape.rot.x, this->targetPitch, 0x100);
}

void EnFirefly_Stunned(EnFirefly* this, GlobalContext* globalCtx) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1554, 0x100);
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        if (this->onFire) {
            this->auraType = KEESE_AURA_FIRE;
        } else if (this->actor.params == KEESE_ICE_FLY) {
            this->auraType = KEESE_AURA_ICE;
        }
        EnFirefly_SetupFlyIdle(this);
    }
}

void EnFirefly_FrozenFall(EnFirefly* this, GlobalContext* globalCtx) {
    if ((this->actor.bgCheckFlags & 1) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
        this->actor.colorFilterTimer = 0;
        EnFirefly_SetupDie(this);
    } else {
        this->actor.colorFilterTimer = 255;
    }
}

// When perching, sit on collision and flap at random intervals
void EnFirefly_Perch(EnFirefly* this, GlobalContext* globalCtx) {
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x100);

    if (this->timer != 0) {
        SkelAnime_Update(&this->skelAnime);
        if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
            this->timer--;
        }
    } else if (Rand_ZeroOne() < 0.02f) {
        this->timer = 1;
    }

    if (this->actor.xzDistToPlayer < 120.0f) {
        EnFirefly_SetupDisturbDiveAttack(this);
    }
}

void EnFirefly_DisturbDiveAttack(EnFirefly* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f preyPos;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer < 40) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0xAAC, 0x100);
    } else {
        preyPos.x = player->actor.world.pos.x;
        preyPos.y = player->actor.world.pos.y + 20.0f;
        preyPos.z = player->actor.world.pos.z;
        Math_ScaledStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &preyPos) + 0x1554,
                           0x100);
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x300);
    }

    if (this->timer == 0) {
        EnFirefly_SetupFlyIdle(this);
    }
}

void EnFirefly_Combust(EnFirefly* this, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i <= 2; i++) {
        EffectSsEnFire_SpawnVec3f(globalCtx, &this->actor, &this->actor.world.pos, 40, 0, 0, i);
    }

    this->auraType = KEESE_AURA_NONE;
}

void EnFirefly_UpdateDamage(EnFirefly* this, GlobalContext* globalCtx) {
    u8 damageEffect;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elements[0].info, 1);

        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(globalCtx, &this->actor);
                this->actor.flags &= ~ACTOR_FLAG_0;
            }

            damageEffect = this->actor.colChkInfo.damageEffect;

            if (damageEffect == 2) { // Din's Fire
                if (this->actor.params == KEESE_ICE_FLY) {
                    this->actor.colChkInfo.health = 0;
                    Enemy_StartFinishingBlow(globalCtx, &this->actor);
                    EnFirefly_Combust(this, globalCtx);
                    EnFirefly_SetupFall(this);
                } else if (!this->onFire) {
                    EnFirefly_Ignite(this);
                    if (this->actionFunc == EnFirefly_Perch) {
                        EnFirefly_SetupFlyIdle(this);
                    }
                }
            } else if (damageEffect == 3) { // Ice Arrows or Ice Magic
                if (this->actor.params == KEESE_ICE_FLY) {
                    EnFirefly_SetupFall(this);
                } else {
                    EnFirefly_SetupFrozenFall(this, globalCtx);
                }
            } else if (damageEffect == 1) { // Deku Nuts
                if (this->actionFunc != EnFirefly_Stunned) {
                    EnFirefly_SetupStunned(this);
                }
            } else { // Fire Arrows
                if ((damageEffect == 0xF) && (this->actor.params == KEESE_ICE_FLY)) {
                    EnFirefly_Combust(this, globalCtx);
                }
                EnFirefly_SetupFall(this);
            }
        }
    }
}

void EnFirefly_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnFirefly* this = (EnFirefly*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FFLY_ATTACK);
        if (this->onFire) {
            EnFirefly_Extinguish(this);
        }
        if (this->actionFunc != EnFirefly_DisturbDiveAttack) {
            EnFirefly_SetupRebound(this);
        }
    }

    EnFirefly_UpdateDamage(this, globalCtx);

    this->actionFunc(this, globalCtx);

    if (!(this->actor.flags & ACTOR_FLAG_15)) {
        if ((this->actor.colChkInfo.health == 0) || (this->actionFunc == EnFirefly_Stunned)) {
            Actor_MoveForward(&this->actor);
        } else {
            if (this->actionFunc != EnFirefly_Rebound) {
                this->actor.world.rot.x = 0x1554 - this->actor.shape.rot.x;
            }
            func_8002D97C(&this->actor);
        }
    }

    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 10.0f, 10.0f, 15.0f, 7);
    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y + 10.0f;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    if ((this->actionFunc == EnFirefly_DiveAttack) || (this->actionFunc == EnFirefly_DisturbDiveAttack)) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }

    if (this->actor.colChkInfo.health != 0) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_FFLY_FLY);
        }
    }

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    this->actor.focus.pos.x =
        (10.0f * Math_SinS(this->actor.shape.rot.x) * Math_SinS(this->actor.shape.rot.y)) + this->actor.world.pos.x;
    this->actor.focus.pos.y = (10.0f * Math_CosS(this->actor.shape.rot.x)) + this->actor.world.pos.y;
    this->actor.focus.pos.z =
        (10.0f * Math_SinS(this->actor.shape.rot.x) * Math_CosS(this->actor.shape.rot.y)) + this->actor.world.pos.z;
}

s32 EnFirefly_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx, Gfx** gfx) {
    EnFirefly* this = (EnFirefly*)thisx;

    if ((this->actor.draw == EnFirefly_DrawInvisible) && !globalCtx->actorCtx.lensActive) {
        *dList = NULL;
    } else if (limbIndex == 1) {
        pos->y += 2300.0f;
    }
    return false;
}

void EnFirefly_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Color_RGBA8 fireAuraPrimColor = { 255, 255, 100, 255 };
    static Color_RGBA8 fireAuraEnvColor = { 255, 50, 0, 0 };
    static Color_RGBA8 iceAuraPrimColor = { 100, 200, 255, 255 };
    static Color_RGBA8 iceAuraEnvColor = { 0, 0, 255, 0 };
    static Vec3f effVelocity = { 0.0f, 0.5f, 0.0f };
    static Vec3f effAccel = { 0.0f, 0.5f, 0.0f };
    static Vec3f limbSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f* limbDest;
    Color_RGBA8* effPrimColor;
    Color_RGBA8* effEnvColor;
    MtxF mtx;
    s16 effScaleStep;
    s16 effLife;
    EnFirefly* this = (EnFirefly*)thisx;
    if (CVar_GetS32("gUseKeeseCol", 0)) {
        Color_RGBA8 fireAuraPrimColor_custom = { CVar_GetS32("gKeese1_Ef_PrimR", 255), CVar_GetS32("gKeese1_Ef_PrimG", 255), CVar_GetS32("gKeese1_Ef_PrimB", 100), 255 };
        Color_RGBA8 fireAuraEnvColor_custom = { CVar_GetS32("gKeese1_Ef_EnvR", 255), CVar_GetS32("gKeese1_Ef_Env", 50), CVar_GetS32("gKeese1_Ef_EnvB", 0), 0 };
        Color_RGBA8 iceAuraPrimColor_custom = { CVar_GetS32("gKeese2_Ef_PrimR", 100), CVar_GetS32("gKeese2_Ef_PrimG", 200), CVar_GetS32("gKeese2_Ef_PrimB", 255), 255 };
        Color_RGBA8 iceAuraEnvColor_custom = { CVar_GetS32("gKeese2_Ef_EnvR", 0), CVar_GetS32("gKeese2_Ef_Env", 0), CVar_GetS32("gKeese2_Ef_EnvB", 255), 0 };
        fireAuraPrimColor = fireAuraPrimColor_custom;
        fireAuraEnvColor = fireAuraEnvColor_custom;
        iceAuraPrimColor = iceAuraPrimColor_custom;
        iceAuraEnvColor = iceAuraEnvColor_custom;
    } else {
        //Original colors are back there
        Color_RGBA8 fireAuraPrimColor_custom = { 255, 255, 100, 255 };
        Color_RGBA8 fireAuraEnvColor_custom = { 255, 50, 0, 0 };
        Color_RGBA8 iceAuraPrimColor_custom = { 100, 200, 255, 255 };
        Color_RGBA8 iceAuraEnvColor_custom = { 0, 0, 255, 0 };
        fireAuraPrimColor = fireAuraPrimColor_custom;
        fireAuraEnvColor = fireAuraEnvColor_custom;
        iceAuraPrimColor = iceAuraPrimColor_custom;
        iceAuraEnvColor = iceAuraEnvColor_custom;
    }
    if (!this->onFire && (limbIndex == 27)) {
        gSPDisplayList((*gfx)++, gKeeseEyesDL);
    } else {
        if ((this->auraType == KEESE_AURA_FIRE) || (this->auraType == KEESE_AURA_ICE)) {
            if ((limbIndex == 15) || (limbIndex == 21)) {
                if (this->actionFunc != EnFirefly_Die) {
                    Matrix_Get(&mtx);
                    effPos.x = (Rand_ZeroOne() * 5.0f) + mtx.xw;
                    effPos.y = (Rand_ZeroOne() * 5.0f) + mtx.yw;
                    effPos.z = (Rand_ZeroOne() * 5.0f) + mtx.zw;
                    effScaleStep = -40;
                    effLife = 3;
                } else {
                    if (limbIndex == 15) {
                        effPos.x = (Math_SinS(9100 * this->timer) * this->timer) + this->actor.world.pos.x;
                        effPos.z = (Math_CosS(9100 * this->timer) * this->timer) + this->actor.world.pos.z;
                    } else {
                        effPos.x = this->actor.world.pos.x - (Math_SinS(9100 * this->timer) * this->timer);
                        effPos.z = this->actor.world.pos.z - (Math_CosS(9100 * this->timer) * this->timer);
                    }

                    effPos.y = this->actor.world.pos.y + ((15 - this->timer) * 1.5f);
                    effScaleStep = -5;
                    effLife = 10;
                }

                if (this->auraType == KEESE_AURA_FIRE) {
                    effPrimColor = &fireAuraPrimColor;
                    effEnvColor = &fireAuraEnvColor;
                } else {
                    effPrimColor = &iceAuraPrimColor;
                    effEnvColor = &iceAuraEnvColor;
                }

                func_8002843C(globalCtx, &effPos, &effVelocity, &effAccel, effPrimColor, effEnvColor, 250, effScaleStep,
                              effLife);
            }
        }
    }
    if ((limbIndex == 15) || (limbIndex == 21) || (limbIndex == 10)) {
        if (limbIndex == 15) {
            limbDest = &this->bodyPartsPos[0];
        } else if (limbIndex == 21) {
            limbDest = &this->bodyPartsPos[1];
        } else {
            limbDest = &this->bodyPartsPos[2];
        }

        Matrix_MultVec3f(&limbSrc, limbDest);
        limbDest->y -= 5.0f;
    }
}

void EnFirefly_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnFirefly* this = (EnFirefly*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_80093D18(globalCtx->state.gfxCtx);

    if (this->onFire) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    }

    POLY_OPA_DISP = SkelAnime_Draw(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   EnFirefly_OverrideLimbDraw, EnFirefly_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnFirefly_DrawInvisible(Actor* thisx, GlobalContext* globalCtx) {
    EnFirefly* this = (EnFirefly*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);

    if (this->onFire) {
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 255);
    }

    POLY_XLU_DISP = SkelAnime_Draw(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   EnFirefly_OverrideLimbDraw, EnFirefly_PostLimbDraw, this, POLY_XLU_DISP);
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

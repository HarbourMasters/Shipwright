#include "z_en_eiyer.h"
#include "objects/object_ei/object_ei.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnEiyer_Init(Actor* thisx, PlayState* play);
void EnEiyer_Destroy(Actor* thisx, PlayState* play);
void EnEiyer_Update(Actor* thisx, PlayState* play);
void EnEiyer_Draw(Actor* thisx, PlayState* play);

void EnEiyer_SetupAppearFromGround(EnEiyer* this);
void EnEiyer_SetupUnderground(EnEiyer* this);
void EnEiyer_SetupInactive(EnEiyer* this);
void EnEiyer_SetupAmbush(EnEiyer* this, PlayState* play);
void EnEiyer_SetupGlide(EnEiyer* this);
void EnEiyer_SetupStartAttack(EnEiyer* this);
void EnEiyer_SetupDiveAttack(EnEiyer* this, PlayState* play);
void EnEiyer_SetupLand(EnEiyer* this);
void EnEiyer_SetupHurt(EnEiyer* this);
void EnEiyer_SetupDie(EnEiyer* this);
void EnEiyer_SetupDead(EnEiyer* this);
void EnEiyer_SetupStunned(EnEiyer* this);

void EnEiyer_AppearFromGround(EnEiyer* this, PlayState* play);
void EnEiyer_WanderUnderground(EnEiyer* this, PlayState* play);
void EnEiyer_CircleUnderground(EnEiyer* this, PlayState* play);
void EnEiyer_Inactive(EnEiyer* this, PlayState* play);
void EnEiyer_Ambush(EnEiyer* this, PlayState* play);
void EnEiyer_Glide(EnEiyer* this, PlayState* play);
void EnEiyer_StartAttack(EnEiyer* this, PlayState* play);
void EnEiyer_DiveAttack(EnEiyer* this, PlayState* play);
void EnEiyer_Land(EnEiyer* this, PlayState* play);
void EnEiyer_Hurt(EnEiyer* this, PlayState* play);
void EnEiyer_Die(EnEiyer* this, PlayState* play);
void EnEiyer_Dead(EnEiyer* this, PlayState* play);
void EnEiyer_Stunned(EnEiyer* this, PlayState* play);

const ActorInit En_Eiyer_InitVars = {
    ACTOR_EN_EIYER,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_EI,
    sizeof(EnEiyer),
    (ActorFunc)EnEiyer_Init,
    (ActorFunc)EnEiyer_Destroy,
    (ActorFunc)EnEiyer_Update,
    (ActorFunc)EnEiyer_Draw,
    NULL,
};

static ColliderCylinderInit sColCylInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x04, 0x08 },
        { 0x00000019, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_HARD,
        BUMP_ON,
        OCELEM_ON,
    },
    { 27, 17, -10, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 45, 15, 100 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(4, 0x0),
    /* Light magic   */ DMG_ENTRY(4, 0x0),
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
    ICHAIN_S8(naviEnemyId, 0x19, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 5, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2500, ICHAIN_STOP),
};

/**
 * params    0: Spawn 3 clones and circle around spawn point
 * params  1-3: Clone, spawn another clone for the main Eiyer if params < 3
 * params   10: Normal Eiyer, wander around spawn point
 */
void EnEiyer_Init(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 600.0f, ActorShadow_DrawCircle, 65.0f);
    SkelAnime_Init(play, &this->skelanime, &gStingerSkel, &gStingerIdleAnim, this->jointTable, this->morphTable,
                   19);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sColCylInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (this->actor.params < 3) {
        // Each clone spawns another clone
        if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_EIYER, this->actor.home.pos.x,
                               this->actor.home.pos.y, this->actor.home.pos.z, 0, this->actor.shape.rot.y + 0x4000, 0,
                               this->actor.params + 1) == NULL) {
            Actor_Kill(&this->actor);
            return;
        }

        if (this->actor.params == 0) {
            Actor* child = this->actor.child;
            s32 clonesSpawned;

            for (clonesSpawned = 0; clonesSpawned != 3; clonesSpawned++) {
                if (child == NULL) {
                    break;
                }
                child = child->child;
            }

            if (clonesSpawned != 3) {
                for (child = &this->actor; child != NULL; child = child->child) {
                    Actor_Kill(child);
                }
                return;
            } else {
                this->actor.child->parent = &this->actor;
                this->actor.child->child->parent = &this->actor;
                this->actor.child->child->child->parent = &this->actor;
            }
        }
    }

    if (this->actor.params == 0 || this->actor.params == 10) {
        EnEiyer_SetupAppearFromGround(this);
    } else {
        EnEiyer_SetupInactive(this);
    }
}

void EnEiyer_Destroy(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
}

void EnEiyer_RotateAroundHome(EnEiyer* this) {
    this->actor.world.pos.x = Math_SinS(this->actor.world.rot.y) * 80.0f + this->actor.home.pos.x;
    this->actor.world.pos.z = Math_CosS(this->actor.world.rot.y) * 80.0f + this->actor.home.pos.z;
    this->actor.shape.rot.y = this->actor.world.rot.y + 0x4000;
}

void EnEiyer_SetupAppearFromGround(EnEiyer* this) {
    this->collider.info.bumper.dmgFlags = 0x19;
    Animation_PlayLoop(&this->skelanime, &gStingerIdleAnim);

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y - 40.0f;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.velocity.y = 0.0f;
    this->actor.speedXZ = 0.0f;

    if (this->actor.params != 0xA) {
        if (this->actor.params == 0) {
            this->actor.world.rot.y = Rand_ZeroOne() * 0x10000;
        } else {
            this->actor.world.rot.y = this->actor.parent->world.rot.y + this->actor.params * 0x4000;
        }
        EnEiyer_RotateAroundHome(this);
    } else {
        this->actor.world.rot.y = this->actor.shape.rot.y = Rand_ZeroOne() * 0x10000;
    }

    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_12);
    this->actor.shape.shadowScale = 0.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actionFunc = EnEiyer_AppearFromGround;
}

void EnEiyer_SetupUnderground(EnEiyer* this) {
    if (this->actor.params == 0xA) {
        this->actor.speedXZ = -0.5f;
        this->actionFunc = EnEiyer_WanderUnderground;
    } else {
        this->actionFunc = EnEiyer_CircleUnderground;
    }

    this->collider.base.acFlags |= AC_ON;
    this->actor.flags &= ~ACTOR_FLAG_4;
    this->actor.flags |= ACTOR_FLAG_0;
}

void EnEiyer_SetupInactive(EnEiyer* this) {
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnEiyer_Inactive;
}

void EnEiyer_SetupAmbush(EnEiyer* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;
    Animation_PlayOnce(&this->skelanime, &gStingerBackflipAnim);
    this->collider.info.bumper.dmgFlags = ~0x00300000;
    this->basePos = this->actor.world.pos;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.flags |= ACTOR_FLAG_12;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.shape.shadowScale = 65.0f;
    this->actor.shape.yOffset = 600.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_OCTAROCK_JUMP);
    EffectSsGSplash_Spawn(play, &this->actor.world.pos, NULL, NULL, 1, 700);
    this->actionFunc = EnEiyer_Ambush;
}

void EnEiyer_SetupGlide(EnEiyer* this) {
    this->targetYaw = this->actor.shape.rot.y;
    this->basePos.y = (cosf(-M_PI / 8) * 5.0f) + this->actor.world.pos.y;
    Animation_MorphToLoop(&this->skelanime, &gStingerHitAnim, -5.0f);
    this->timer = 60;
    this->actionFunc = EnEiyer_Glide;
}

void EnEiyer_SetupStartAttack(EnEiyer* this) {
    this->actionFunc = EnEiyer_StartAttack;
}

void EnEiyer_SetupDiveAttack(EnEiyer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.velocity.y = 0.0f;
    this->basePos.y = player->actor.world.pos.y + 15.0f;
    this->collider.base.atFlags |= AT_ON;
    this->collider.base.atFlags &= ~AT_HIT;
    this->actionFunc = EnEiyer_DiveAttack;
}

void EnEiyer_SetupLand(EnEiyer* this) {
    Animation_MorphToPlayOnce(&this->skelanime, &gStingerDiveAnim, -3.0f);
    this->collider.base.atFlags &= ~AT_ON;
    this->actor.flags |= ACTOR_FLAG_4;

    // Update BgCheck info, play sound, and spawn effect on the first frame of the land action
    this->timer = -1;
    this->actor.gravity = 0.0f;
    this->collider.dim.height = sColCylInit.dim.height;
    this->actionFunc = EnEiyer_Land;
}

void EnEiyer_SetupHurt(EnEiyer* this) {
    this->basePos.y = this->actor.world.pos.y;
    Animation_Change(&this->skelanime, &gStingerHitAnim, 2.0f, 0.0f, 0.0f, 0, -3.0f);
    this->timer = 40;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.speedXZ = 5.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 200, 0, 40);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnEiyer_Hurt;
}

void EnEiyer_SetupDie(EnEiyer* this) {
    this->timer = 20;
    Actor_SetColorFilter(&this->actor, 0x4000, 200, 0, 40);

    if (this->collider.info.bumper.dmgFlags != 0x19) {
        this->actor.speedXZ = 6.0f;
        Animation_MorphToLoop(&this->skelanime, &gStingerHitAnim, -3.0f);
    } else {
        this->actor.speedXZ -= 6.0f;
    }

    this->collider.info.bumper.dmgFlags = ~0x00300000;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnEiyer_Die;
}

void EnEiyer_SetupDead(EnEiyer* this) {
    this->actor.colorFilterParams |= 0x2000;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actionFunc = EnEiyer_Dead;
}

void EnEiyer_SetupStunned(EnEiyer* this) {
    Animation_Change(&this->skelanime, &gStingerPopOutAnim, 2.0f, 0.0f, 0.0f, 0, -8.0f);
    this->timer = 80;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;
    this->collider.dim.height = sColCylInit.dim.height + 8;
    Actor_SetColorFilter(&this->actor, 0, 200, 0, 80);
    this->collider.base.atFlags &= ~AT_ON;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = EnEiyer_Stunned;
}

void EnEiyer_AppearFromGround(EnEiyer* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);

    if (Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.75f)) {
        EnEiyer_SetupUnderground(this);
    }
}

void EnEiyer_CheckPlayerCollision(EnEiyer* this, PlayState* play) {
    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        EnEiyer_SetupAmbush(this, play);
    }
}

void EnEiyer_CircleUnderground(EnEiyer* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);
    this->actor.world.rot.y += -0x60;
    EnEiyer_RotateAroundHome(this);
    EnEiyer_CheckPlayerCollision(this, play);

    // Clones disappear when the main Eiyer leaves the ground
    if (this->actor.params != 0 && ((EnEiyer*)this->actor.parent)->actionFunc != EnEiyer_CircleUnderground) {
        EnEiyer_SetupInactive(this);
    }
}

void EnEiyer_WanderUnderground(EnEiyer* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);

    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 100.0f) {
        this->targetYaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos) + 0x8000;
    } else if (this->targetYaw == this->actor.world.rot.y && Rand_ZeroOne() > 0.99f) {
        this->targetYaw =
            this->actor.world.rot.y + (Rand_ZeroOne() < 0.5f ? -1 : 1) * (Rand_ZeroOne() * 0x2000 + 0x2000);
    }

    Math_ScaledStepToS(&this->actor.world.rot.y, this->targetYaw, 0xB6);
    EnEiyer_CheckPlayerCollision(this, play);
}

void EnEiyer_Inactive(EnEiyer* this, PlayState* play) {
    EnEiyer* parent;

    if (this->actor.home.pos.y - 50.0f < this->actor.world.pos.y) {
        this->actor.world.pos.y -= 0.5f;
    }

    parent = (EnEiyer*)this->actor.parent;
    if (parent->actionFunc == EnEiyer_Dead) {
        Actor_Kill(&this->actor);
    } else if (parent->actionFunc == EnEiyer_AppearFromGround) {
        EnEiyer_SetupAppearFromGround(this);
    }
}

void EnEiyer_Ambush(EnEiyer* this, PlayState* play) {
    s32 animFinished;
    f32 curFrame;
    f32 xzOffset;
    s32 bgId;

    animFinished = SkelAnime_Update(&this->skelanime);
    curFrame = this->skelanime.curFrame;

    if (this->skelanime.curFrame < 12.0f) {
        this->actor.world.pos.y = ((1.0f - cosf((0.996f * M_PI / 12.0f) * curFrame)) * 40.0f) + this->actor.home.pos.y;
        xzOffset = sinf((0.996f * M_PI / 12.0f) * curFrame) * -40.0f;
        this->actor.world.pos.x = (Math_SinS(this->actor.shape.rot.y) * xzOffset) + this->basePos.x;
        this->actor.world.pos.z = (Math_CosS(this->actor.shape.rot.y) * xzOffset) + this->basePos.z;
    } else {
        Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y + 80.0f, 0.5f);
        this->actor.speedXZ = 0.8f;
    }

    if (animFinished) {
        this->collider.base.acFlags |= AC_ON;
        EnEiyer_SetupGlide(this);
    } else {
        this->actor.floorHeight = BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                              &this->actor, &this->actor.world.pos);
    }
}

void EnEiyer_Glide(EnEiyer* this, PlayState* play) {
    f32 curFrame;
    s32 pad;
    s16 yawChange;

    SkelAnime_Update(&this->skelanime);

    if (this->timer != 0) {
        this->timer--;
    }

    curFrame = this->skelanime.curFrame;
    Math_ApproachF(&this->basePos.y, this->actor.floorHeight + 80.0f + 5.0f, 0.3f, this->actor.speedXZ);
    this->actor.world.pos.y = this->basePos.y - cosf((curFrame - 5.0f) * (M_PI / 40)) * 5.0f;

    if (curFrame <= 45.0f) {
        Math_StepToF(&this->actor.speedXZ, 1.0f, 0.03f);
    } else {
        Math_StepToF(&this->actor.speedXZ, 1.5f, 0.03f);
    }

    if (this->actor.bgCheckFlags & 8) {
        this->targetYaw = this->actor.wallYaw;
    }

    if (Math_ScaledStepToS(&this->actor.world.rot.y, this->targetYaw, 0xB6)) {
        if (this->timer != 0 || Rand_ZeroOne() > 0.05f) {
            this->actor.world.rot.y += 0x100;
        } else {
            yawChange = Rand_S16Offset(0x2000, 0x2000);
            this->targetYaw = (Rand_ZeroOne() < 0.5f ? -1 : 1) * yawChange + this->actor.world.rot.y;
        }
    }

    if (this->timer == 0 && this->actor.yDistToPlayer < 0.0f && this->actor.xzDistToPlayer < 120.0f) {
        EnEiyer_SetupStartAttack(this);
    }

    func_8002F974(&this->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

void EnEiyer_StartAttack(EnEiyer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f focus;

    SkelAnime_Update(&this->skelanime);

    if (this->actor.shape.rot.x > 0 && this->actor.shape.rot.x < 0x8000) {
        focus.x = player->actor.world.pos.x;
        focus.y = player->actor.world.pos.y + 20.0f;
        focus.z = player->actor.world.pos.z;

        if (Math_ScaledStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &focus), 0x1000)) {
            EnEiyer_SetupDiveAttack(this, play);
        }
    } else {
        this->actor.shape.rot.x -= 0x1000;
    }

    this->actor.world.rot.x = -this->actor.shape.rot.x;
    Math_StepToF(&this->actor.speedXZ, 5.0f, 0.3f);
    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 0x71C);
    func_8002F974(&this->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

void EnEiyer_DiveAttack(EnEiyer* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);
    this->actor.speedXZ *= 1.1f;

    if (this->actor.bgCheckFlags & 8 || this->actor.bgCheckFlags & 1) {
        EnEiyer_SetupLand(this);
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~(AT_ON | AT_HIT);
    }

    func_8002F974(&this->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

void EnEiyer_Land(EnEiyer* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);
    Math_ScaledStepToS(&this->actor.world.rot.x, -0x4000, 0x450);
    Math_StepToF(&this->actor.speedXZ, 7.0f, 1.0f);

    if (this->timer == -1) {
        if (this->actor.bgCheckFlags & 8 || this->actor.bgCheckFlags & 1) {
            this->timer = 10;
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_OCTAROCK_SINK);

            if (this->actor.bgCheckFlags & 1) {
                EffectSsGSplash_Spawn(play, &this->actor.world.pos, NULL, NULL, 1, 700);
            }
        }
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            this->actor.shape.rot.x = 0;
            this->actor.world.rot.x = 0;
            EnEiyer_SetupAppearFromGround(this);
        }
    }
}

void EnEiyer_Hurt(EnEiyer* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);

    if (this->timer != 0) {
        this->timer--;
    }

    Math_ApproachF(&this->basePos.y, this->actor.floorHeight + 80.0f + 5.0f, 0.5f, this->actor.speedXZ);
    this->actor.world.pos.y = this->basePos.y - 5.0f;

    if (this->actor.bgCheckFlags & 8) {
        this->targetYaw = this->actor.wallYaw;
    } else {
        this->targetYaw = this->actor.yawTowardsPlayer + 0x8000;
    }

    Math_ScaledStepToS(&this->actor.world.rot.y, this->targetYaw, 0x38E);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x200);
    this->actor.shape.rot.z = sinf(this->timer * (M_PI / 5)) * 5120.0f;

    if (this->timer == 0) {
        this->actor.shape.rot.x = 0;
        this->actor.shape.rot.z = 0;
        this->collider.base.acFlags |= AC_ON;
        EnEiyer_SetupGlide(this);
    }
    this->actor.world.rot.x = -this->actor.shape.rot.x;
}

void EnEiyer_Die(EnEiyer* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);

    if (this->actor.speedXZ > 0.0f) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x4000, 0x400);
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4000, 0x400);
    }

    this->actor.shape.rot.z += 0x1000;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.world.rot.x = -this->actor.shape.rot.x;

    if (this->timer == 0 || this->actor.bgCheckFlags & 0x10) {
        EnEiyer_SetupDead(this);
    }
}

void EnEiyer_Dead(EnEiyer* this, PlayState* play) {
    this->actor.shape.shadowAlpha = CLAMP_MIN((s16)(this->actor.shape.shadowAlpha - 5), 0);
    this->actor.world.pos.y -= 2.0f;

    if (this->actor.shape.shadowAlpha == 0) {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 80);
        Actor_Kill(&this->actor);
    }
}

void EnEiyer_Stunned(EnEiyer* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x200);
    SkelAnime_Update(&this->skelanime);

    if (Animation_OnFrame(&this->skelanime, 0.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_EIER_FLUTTER);
    }

    if (this->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
    }

    if (this->timer == 0) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->collider.dim.height = sColCylInit.dim.height;
        EnEiyer_SetupGlide(this);
    }
}

void EnEiyer_UpdateDamage(EnEiyer* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.info, 1);

        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_EIER_DEAD);
                this->actor.flags &= ~ACTOR_FLAG_0;
                gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_STINGER]++;
            }

            // If underground, one hit kill
            if (this->collider.info.bumper.dmgFlags == 0x19) {
                if (this->actor.colChkInfo.damage == 0) {
                    EnEiyer_SetupAmbush(this, play);
                } else {
                    EnEiyer_SetupDie(this);
                }
            } else if (this->actor.colChkInfo.damageEffect == 1) {
                if (this->actionFunc != EnEiyer_Stunned) {
                    EnEiyer_SetupStunned(this);
                }
            } else if (this->actor.colChkInfo.health != 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_EIER_DAMAGE);
                EnEiyer_SetupHurt(this);
            } else {
                this->collider.dim.height = sColCylInit.dim.height;
                EnEiyer_SetupDie(this);
            }
        }
    }
}

void EnEiyer_Update(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;
    s32 pad;

    EnEiyer_UpdateDamage(this, play);
    this->actionFunc(this, play);

    if (this->actor.world.rot.x == 0 || this->actionFunc == EnEiyer_Stunned) {
        Actor_MoveForward(&this->actor);
    } else {
        func_8002D97C(&this->actor);
    }

    if (this->actionFunc == EnEiyer_Glide || this->actionFunc == EnEiyer_DiveAttack ||
        this->actionFunc == EnEiyer_Stunned || this->actionFunc == EnEiyer_Die || this->actionFunc == EnEiyer_Hurt ||
        (this->actionFunc == EnEiyer_Land && this->timer == -1)) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 5.0f, 27.0f, 30.0f, 7);
    }

    if (this->actor.params == 0xA ||
        (this->actionFunc != EnEiyer_AppearFromGround && this->actionFunc != EnEiyer_CircleUnderground)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    // only the main Eiyer can ambush the player
    if (this->actor.params == 0 || this->actor.params == 0xA) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        if (this->collider.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->actionFunc != EnEiyer_Ambush) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }

    if (this->actor.flags & ACTOR_FLAG_0) {
        this->actor.focus.pos.x = this->actor.world.pos.x + Math_SinS(this->actor.shape.rot.y) * 12.5f;
        this->actor.focus.pos.z = this->actor.world.pos.z + Math_CosS(this->actor.shape.rot.y) * 12.5f;
        this->actor.focus.pos.y = this->actor.world.pos.y;
    }
}

s32 EnEiyer_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                             Gfx** gfx) {
    EnEiyer* this = (EnEiyer*)thisx;

    if (limbIndex == 1) {
        pos->z += 2500.0f;
    }

    if (this->collider.info.bumper.dmgFlags == 0x19 && limbIndex != 9 && limbIndex != 10) {
        *dList = NULL;
    }
    return 0;
}

void EnEiyer_Draw(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    if (this->actionFunc != EnEiyer_Dead) {
        func_80093D18(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

        POLY_OPA_DISP = SkelAnime_Draw(play, this->skelanime.skeleton, this->skelanime.jointTable,
                                       EnEiyer_OverrideLimbDraw, NULL, this, POLY_OPA_DISP);
    } else {
        func_80093D84(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, D_80116280);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->actor.shape.shadowAlpha);

        POLY_XLU_DISP = SkelAnime_Draw(play, this->skelanime.skeleton, this->skelanime.jointTable,
                                       EnEiyer_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

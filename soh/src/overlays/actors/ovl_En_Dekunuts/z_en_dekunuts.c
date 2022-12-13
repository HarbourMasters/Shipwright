/*
 * File: z_en_dekunuts.c
 * Overlay: ovl_En_Dekunuts
 * Description: Mad Scrub
 */

#include "z_en_dekunuts.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_dekunuts/object_dekunuts.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

#define DEKUNUTS_FLOWER 10

void EnDekunuts_Init(Actor* thisx, PlayState* play);
void EnDekunuts_Destroy(Actor* thisx, PlayState* play);
void EnDekunuts_Update(Actor* thisx, PlayState* play);
void EnDekunuts_Draw(Actor* thisx, PlayState* play);

void EnDekunuts_SetupWait(EnDekunuts* this);
void EnDekunuts_Wait(EnDekunuts* this, PlayState* play);
void EnDekunuts_LookAround(EnDekunuts* this, PlayState* play);
void EnDekunuts_Stand(EnDekunuts* this, PlayState* play);
void EnDekunuts_ThrowNut(EnDekunuts* this, PlayState* play);
void EnDekunuts_Burrow(EnDekunuts* this, PlayState* play);
void EnDekunuts_BeginRun(EnDekunuts* this, PlayState* play);
void EnDekunuts_Run(EnDekunuts* this, PlayState* play);
void EnDekunuts_Gasp(EnDekunuts* this, PlayState* play);
void EnDekunuts_BeDamaged(EnDekunuts* this, PlayState* play);
void EnDekunuts_BeStunned(EnDekunuts* this, PlayState* play);
void EnDekunuts_Die(EnDekunuts* this, PlayState* play);

const ActorInit En_Dekunuts_InitVars = {
    ACTOR_EN_DEKUNUTS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DEKUNUTS,
    sizeof(EnDekunuts),
    (ActorFunc)EnDekunuts_Init,
    (ActorFunc)EnDekunuts_Destroy,
    (ActorFunc)EnDekunuts_Update,
    (ActorFunc)EnDekunuts_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 32, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0x01, 0x0012, 0x0020, MASS_IMMOVABLE };

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
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
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

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4D, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2600, ICHAIN_STOP),
};

void EnDekunuts_Init(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (thisx->params == DEKUNUTS_FLOWER) {
        thisx->flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
    } else {
        ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
        SkelAnime_Init(play, &this->skelAnime, &gDekuNutsSkel, &gDekuNutsStandAnim, this->jointTable,
                       this->morphTable, 25);
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);
        this->shotsPerRound = ((thisx->params >> 8) & 0xFF);
        thisx->params &= 0xFF;
        if ((this->shotsPerRound == 0xFF) || (this->shotsPerRound == 0)) {
            this->shotsPerRound = 1;
        }
        EnDekunuts_SetupWait(this);
        Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_EN_DEKUNUTS, thisx->world.pos.x,
                           thisx->world.pos.y, thisx->world.pos.z, 0, thisx->world.rot.y, 0, DEKUNUTS_FLOWER);
    }
}

void EnDekunuts_Destroy(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;

    if (this->actor.params != DEKUNUTS_FLOWER) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void EnDekunuts_SetupWait(EnDekunuts* this) {
    Animation_PlayOnceSetSpeed(&this->skelAnime, &gDekuNutsUpAnim, 0.0f);
    this->animFlagAndTimer = Rand_S16Offset(100, 50);
    this->collider.dim.height = 5;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnDekunuts_Wait;
}

void EnDekunuts_SetupLookAround(EnDekunuts* this) {
    Animation_PlayLoop(&this->skelAnime, &gDekuNutsLookAroundAnim);
    this->animFlagAndTimer = 2;
    this->actionFunc = EnDekunuts_LookAround;
}

void EnDekunuts_SetupThrowNut(EnDekunuts* this) {
    Animation_PlayOnce(&this->skelAnime, &gDekuNutsSpitAnim);
    this->animFlagAndTimer = this->shotsPerRound;
    this->actionFunc = EnDekunuts_ThrowNut;
}

void EnDekunuts_SetupStand(EnDekunuts* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDekuNutsStandAnim, -3.0f);
    if (this->actionFunc == EnDekunuts_ThrowNut) {
        this->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        this->animFlagAndTimer = 1;
    }
    this->actionFunc = EnDekunuts_Stand;
}

void EnDekunuts_SetupBurrow(EnDekunuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuNutsBurrowAnim, -5.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = EnDekunuts_Burrow;
}

void EnDekunuts_SetupBeginRun(EnDekunuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuNutsUnburrowAnim, -3.0f);
    this->collider.dim.height = 37;
    this->actor.colChkInfo.mass = 0x32;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnDekunuts_BeginRun;
}

void EnDekunuts_SetupRun(EnDekunuts* this) {
    Animation_PlayLoop(&this->skelAnime, &gDekuNutsRunAnim);
    this->animFlagAndTimer = 2;
    this->playWalkSound = false;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = EnDekunuts_Run;
}

void EnDekunuts_SetupGasp(EnDekunuts* this) {
    Animation_PlayLoop(&this->skelAnime, &gDekuNutsGaspAnim);
    this->animFlagAndTimer = 3;
    this->actor.speedXZ = 0.0f;
    if (this->runAwayCount != 0) {
        this->runAwayCount--;
    }
    this->actionFunc = EnDekunuts_Gasp;
}

void EnDekunuts_SetupBeDamaged(EnDekunuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuNutsDamageAnim, -3.0f);
    if ((this->collider.info.acHitInfo->toucher.dmgFlags & 0x1F824) != 0) {
        this->actor.world.rot.y = this->collider.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_WorldYawTowardActor(&this->actor, this->collider.base.ac) + 0x8000;
    }
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnDekunuts_BeDamaged;
    this->actor.speedXZ = 10.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_CUTBODY);
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gDekuNutsDamageAnim));
}

void EnDekunuts_SetupBeStunned(EnDekunuts* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDekuNutsDamageAnim, -3.0f);
    this->animFlagAndTimer = 5;
    this->actionFunc = EnDekunuts_BeStunned;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0,
                         Animation_GetLastFrame(&gDekuNutsDamageAnim) * this->animFlagAndTimer);
}

void EnDekunuts_SetupDie(EnDekunuts* this) {
    Animation_PlayOnce(&this->skelAnime, &gDekuNutsDieAnim);
    this->actionFunc = EnDekunuts_Die;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DEAD);
    gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MAD_SCRUB]++;
}

void EnDekunuts_Wait(EnDekunuts* this, PlayState* play) {
    s32 hasSlowPlaybackSpeed = false;

    if (this->skelAnime.playSpeed < 0.5f) {
        hasSlowPlaybackSpeed = true;
    }
    if (hasSlowPlaybackSpeed && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
        this->collider.base.acFlags |= AC_ON;
    } else if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_UP);
    }

    this->collider.dim.height = ((CLAMP(this->skelAnime.curFrame, 9.0f, 12.0f) - 9.0f) * 9.0f) + 5.0f;
    if (!hasSlowPlaybackSpeed && (this->actor.xzDistToPlayer < 120.0f)) {
        EnDekunuts_SetupBurrow(this);
    } else if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.xzDistToPlayer < 120.0f) {
            EnDekunuts_SetupBurrow(this);
        } else if ((this->animFlagAndTimer == 0) && (this->actor.xzDistToPlayer > 320.0f)) {
            EnDekunuts_SetupLookAround(this);
        } else {
            EnDekunuts_SetupStand(this);
        }
    }
    if (hasSlowPlaybackSpeed &&
        ((this->actor.xzDistToPlayer > 160.0f) && (fabsf(this->actor.yDistToPlayer) < 120.0f)) &&
        ((this->animFlagAndTimer == 0) || (this->actor.xzDistToPlayer < 480.0f))) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void EnDekunuts_LookAround(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if ((this->actor.xzDistToPlayer < 120.0f) || (this->animFlagAndTimer == 0)) {
        EnDekunuts_SetupBurrow(this);
    }
}

void EnDekunuts_Stand(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (!(this->animFlagAndTimer & 0x1000)) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }
    if (this->animFlagAndTimer == 0x1000) {
        if ((this->actor.xzDistToPlayer > 480.0f) || (this->actor.xzDistToPlayer < 120.0f)) {
            EnDekunuts_SetupBurrow(this);
        } else {
            EnDekunuts_SetupThrowNut(this);
        }
    } else if (this->animFlagAndTimer == 0) {
        EnDekunuts_SetupThrowNut(this);
    }
}

void EnDekunuts_ThrowNut(EnDekunuts* this, PlayState* play) {
    Vec3f spawnPos;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDekunuts_SetupStand(this);
    } else if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        spawnPos.x = this->actor.world.pos.x + (Math_SinS(this->actor.shape.rot.y) * 23.0f);
        spawnPos.y = this->actor.world.pos.y + 12.0f;
        spawnPos.z = this->actor.world.pos.z + (Math_CosS(this->actor.shape.rot.y) * 23.0f);
        if (Actor_Spawn(&play->actorCtx, play, ACTOR_EN_NUTSBALL, spawnPos.x, spawnPos.y, spawnPos.z,
                        this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, 0, true) != NULL) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_THROW);
        }
    } else if ((this->animFlagAndTimer > 1) && Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gDekuNutsSpitAnim, -3.0f);
        if (this->animFlagAndTimer != 0) {
            this->animFlagAndTimer--;
        }
    }
}

void EnDekunuts_Burrow(EnDekunuts* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDekunuts_SetupWait(this);
    } else {
        this->collider.dim.height = ((3.0f - CLAMP(this->skelAnime.curFrame, 1.0f, 3.0f)) * 12.0f) + 5.0f;
    }
    if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
        this->collider.base.acFlags &= ~AC_ON;
    }
    Math_ApproachF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.5f, 3.0f);
    Math_ApproachF(&this->actor.world.pos.z, this->actor.home.pos.z, 0.5f, 3.0f);
}

void EnDekunuts_BeginRun(EnDekunuts* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->runDirection = this->actor.yawTowardsPlayer + 0x8000;
        this->runAwayCount = 3;
        EnDekunuts_SetupRun(this);
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
}

void EnDekunuts_Run(EnDekunuts* this, PlayState* play) {
    s16 diffRotInit;
    s16 diffRot;
    f32 phi_f0;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (this->playWalkSound) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_WALK);
        this->playWalkSound = false;
    } else {
        this->playWalkSound = true;
    }

    Math_StepToF(&this->actor.speedXZ, 7.5f, 1.0f);
    if (Math_SmoothStepToS(&this->actor.world.rot.y, this->runDirection, 1, 0xE38, 0xB6) == 0) {
        if (this->actor.bgCheckFlags & 0x20) {
            this->runDirection = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
        } else if (this->actor.bgCheckFlags & 8) {
            this->runDirection = this->actor.wallYaw;
        } else if (this->runAwayCount == 0) {
            diffRotInit = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
            diffRot = diffRotInit - this->actor.yawTowardsPlayer;
            if (ABS(diffRot) > 0x2000) {
                this->runDirection = diffRotInit;
            } else {
                phi_f0 = (diffRot >= 0.0f) ? 1.0f : -1.0f;
                this->runDirection = (phi_f0 * -0x2000) + this->actor.yawTowardsPlayer;
            }
        } else {
            this->runDirection = this->actor.yawTowardsPlayer + 0x8000;
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    if ((this->runAwayCount == 0) && Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < 20.0f &&
        fabsf(this->actor.world.pos.y - this->actor.home.pos.y) < 2.0f) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actor.speedXZ = 0.0f;
        EnDekunuts_SetupBurrow(this);
    } else if (this->animFlagAndTimer == 0) {
        EnDekunuts_SetupGasp(this);
    }
}

void EnDekunuts_Gasp(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (this->animFlagAndTimer == 0) {
        EnDekunuts_SetupRun(this);
    }
}

void EnDekunuts_BeDamaged(EnDekunuts* this, PlayState* play) {
    Math_StepToF(&this->actor.speedXZ, 0.0f, 1.0f);
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDekunuts_SetupDie(this);
    }
}

void EnDekunuts_BeStunned(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        if (this->animFlagAndTimer != 0) {
            this->animFlagAndTimer--;
        }
        if (this->animFlagAndTimer == 0) {
            EnDekunuts_SetupRun(this);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_FAINT);
        }
    }
}

void EnDekunuts_Die(EnDekunuts* this, PlayState* play) {
    static Vec3f effectVelAndAccel = { 0.0f, 0.0f, 0.0f };

    s32 pad;
    Vec3f effectPos;

    if (SkelAnime_Update(&this->skelAnime)) {
        effectPos.x = this->actor.world.pos.x;
        effectPos.y = this->actor.world.pos.y + 18.0f;
        effectPos.z = this->actor.world.pos.z;
        EffectSsDeadDb_Spawn(play, &effectPos, &effectVelAndAccel, &effectVelAndAccel, 200, 0, 255, 255, 255, 255,
                             150, 150, 150, 1, 13, 1);
        effectPos.y = this->actor.world.pos.y + 10.0f;
        EffectSsHahen_SpawnBurst(play, &effectPos, 3.0f, 0, 12, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x30);
        if (this->actor.child != NULL) {
            Actor_ChangeCategory(play, &play->actorCtx, this->actor.child, ACTORCAT_PROP);
        }
        Actor_Kill(&this->actor);
    }
}

void EnDekunuts_ColliderCheck(EnDekunuts* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.info, 1);
        if (this->actor.colChkInfo.mass == 0x32) {
            if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
                if (this->actor.colChkInfo.damageEffect != 1) {
                    if (this->actor.colChkInfo.damageEffect == 2) {
                        EffectSsFCircle_Spawn(play, &this->actor, &this->actor.world.pos, 40, 50);
                    }
                    EnDekunuts_SetupBeDamaged(this);
                    if (Actor_ApplyDamage(&this->actor) == 0) {
                        Enemy_StartFinishingBlow(play, &this->actor);
                    }
                } else if (this->actionFunc != EnDekunuts_BeStunned) {
                    EnDekunuts_SetupBeStunned(this);
                }
            }
        } else {
            EnDekunuts_SetupBeginRun(this);
        }
    } else if ((this->actor.colChkInfo.mass == MASS_IMMOVABLE) && (play->actorCtx.unk_02 != 0)) {
        EnDekunuts_SetupBeginRun(this);
    }
}

void EnDekunuts_Update(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    s32 pad;

    if (this->actor.params != DEKUNUTS_FLOWER) {
        EnDekunuts_ColliderCheck(this, play);
        this->actionFunc(this, play);
        Actor_MoveForward(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, this->collider.dim.radius, this->collider.dim.height,
                                0x1D);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        if (this->actionFunc == EnDekunuts_Wait) {
            Actor_SetFocus(&this->actor, this->skelAnime.curFrame);
        } else if (this->actionFunc == EnDekunuts_Burrow) {
            Actor_SetFocus(&this->actor,
                           20.0f - ((this->skelAnime.curFrame * 20.0f) / Animation_GetLastFrame(&gDekuNutsBurrowAnim)));
        } else {
            Actor_SetFocus(&this->actor, 20.0f);
        }
    }
}

s32 EnDekunuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    f32 x;
    f32 y;
    f32 z;
    f32 curFrame;

    if ((limbIndex == 7) && (this->actionFunc == EnDekunuts_ThrowNut)) {
        curFrame = this->skelAnime.curFrame;
        if (curFrame <= 6.0f) {
            x = 1.0f - (curFrame * 0.0833f);
            z = 1.0f + (curFrame * 0.1167f);
            y = 1.0f + (curFrame * 0.1167f);
        } else if (curFrame <= 7.0f) {
            curFrame -= 6.0f;
            x = 0.5f + curFrame;
            z = 1.7f - (curFrame * 0.7f);
            y = 1.7f - (curFrame * 0.7f);
        } else if (curFrame <= 10.0f) {
            x = 1.5f - ((curFrame - 7.0f) * 0.1667f);
            z = 1.0f;
            y = 1.0f;
        } else {
            return false;
        }
        Matrix_Scale(x, y, z, MTXMODE_APPLY);
    }
    return false;
}

void EnDekunuts_Draw(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;

    if (this->actor.params == DEKUNUTS_FLOWER) {
        Gfx_DrawDListOpa(play, gDekuNutsFlowerDL);
    } else {
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDekunuts_OverrideLimbDraw,
                          NULL, this);
    }
}

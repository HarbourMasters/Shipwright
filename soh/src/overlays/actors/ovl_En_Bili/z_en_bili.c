/*
 * File: z_en_bili.c
 * Overlay: ovl_En_Bili
 * Description: Biri (small jellyfish-like enemy)
 */

#include "z_en_bili.h"
#include "objects/object_bl/object_bl.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_ARROW_DRAGGABLE)

void EnBili_Init(Actor* thisx, PlayState* play);
void EnBili_Destroy(Actor* thisx, PlayState* play);
void EnBili_Update(Actor* thisx, PlayState* play);
void EnBili_Draw(Actor* thisx, PlayState* play);

void EnBili_SetupFloatIdle(EnBili* this);
void EnBili_SetupSpawnedFlyApart(EnBili* this);
void EnBili_FloatIdle(EnBili* this, PlayState* play);
void EnBili_SpawnedFlyApart(EnBili* this, PlayState* play);
void EnBili_DischargeLightning(EnBili* this, PlayState* play);
void EnBili_Climb(EnBili* this, PlayState* play);
void EnBili_ApproachPlayer(EnBili* this, PlayState* play);
void EnBili_SetNewHomeHeight(EnBili* this, PlayState* play);
void EnBili_Recoil(EnBili* this, PlayState* play);
void EnBili_Burnt(EnBili* this, PlayState* play);
void EnBili_Die(EnBili* this, PlayState* play);
void EnBili_Stunned(EnBili* this, PlayState* play);
void EnBili_Frozen(EnBili* this, PlayState* play);

const ActorInit En_Bili_InitVars = {
    ACTOR_EN_BILI,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BL,
    sizeof(EnBili),
    (ActorFunc)EnBili_Init,
    (ActorFunc)EnBili_Destroy,
    (ActorFunc)EnBili_Update,
    (ActorFunc)EnBili_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT8,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x03, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 9, 28, -20, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 9, 28, -20, 30 };

typedef enum {
    /* 0x0 */ BIRI_DMGEFF_NONE,
    /* 0x1 */ BIRI_DMGEFF_DEKUNUT,
    /* 0x2 */ BIRI_DMGEFF_FIRE,
    /* 0x3 */ BIRI_DMGEFF_ICE,
    /* 0xE */ BIRI_DMGEFF_SLINGSHOT = 0xE,
    /* 0xF */ BIRI_DMGEFF_SWORD
} BiriDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, BIRI_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(0, BIRI_DMGEFF_SLINGSHOT),
    /* Explosive     */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(1, BIRI_DMGEFF_NONE),
    /* Normal arrow  */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, BIRI_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, BIRI_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, BIRI_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, BIRI_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, BIRI_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, BIRI_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(4, BIRI_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, BIRI_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, BIRI_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x17, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_STOP),
};

void EnBili_Init(Actor* thisx, PlayState* play) {
    EnBili* this = (EnBili*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 17.0f);
    this->actor.shape.shadowAlpha = 155;
    SkelAnime_Init(play, &this->skelAnime, &gBiriSkel, &gBiriDefaultAnim, this->jointTable, this->morphTable,
                   EN_BILI_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->playFlySound = false;

    if (this->actor.params == EN_BILI_TYPE_NORMAL) {
        EnBili_SetupFloatIdle(this);
    } else {
        EnBili_SetupSpawnedFlyApart(this);
    }
}

void EnBili_Destroy(Actor* thisx, PlayState* play) {
    EnBili* this = (EnBili*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

// Setup Action Functions

void EnBili_SetupFloatIdle(EnBili* this) {
    this->actor.speedXZ = 0.7f;
    this->collider.info.bumper.effect = 1; // Shock?
    this->timer = 32;
    this->collider.base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = EnBili_FloatIdle;
    this->actor.home.pos.y = this->actor.world.pos.y;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
}

/**
 * Separates the Biri spawned by a dying EnVali.
 */
void EnBili_SetupSpawnedFlyApart(EnBili* this) {
    Animation_PlayLoop(&this->skelAnime, &gBiriDefaultAnim);
    this->timer = 25;
    this->actor.velocity.y = 6.0f;
    this->actor.gravity = -0.3f;
    this->collider.base.atFlags &= ~AT_ON;
    this->actionFunc = EnBili_SpawnedFlyApart;
    this->actor.speedXZ = 3.0f;
}

/**
 * Used for both touching player/player's shield and being hit with sword. What to do next is determined by params.
 */
void EnBili_SetupDischargeLightning(EnBili* this) {
    Animation_PlayLoop(&this->skelAnime, &gBiriDischargeLightningAnim);
    this->timer = 10;
    this->actionFunc = EnBili_DischargeLightning;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = -1.0f;
}

void EnBili_SetupClimb(EnBili* this) {
    Animation_PlayOnce(&this->skelAnime, &gBiriClimbAnim);
    this->collider.base.atFlags &= ~AT_ON;
    this->actionFunc = EnBili_Climb;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
}

void EnBili_SetupApproachPlayer(EnBili* this) {
    this->actor.speedXZ = 1.2f;
    this->actionFunc = EnBili_ApproachPlayer;
}

void EnBili_SetupSetNewHomeHeight(EnBili* this) {
    Animation_PlayLoop(&this->skelAnime, &gBiriDefaultAnim);
    this->timer = 96;
    this->actor.speedXZ = 0.9f;
    this->collider.base.atFlags |= AT_ON;
    this->actionFunc = EnBili_SetNewHomeHeight;
    this->actor.home.pos.y = this->actor.world.pos.y;
}

void EnBili_SetupRecoil(EnBili* this) {
    if (this->skelAnime.animation != &gBiriDefaultAnim) {
        Animation_PlayLoop(&this->skelAnime, &gBiriDefaultAnim);
    }

    this->actor.world.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->collider.base.ac->prevPos) + 0x8000;
    this->actor.world.rot.x = Actor_WorldPitchTowardPoint(&this->actor, &this->collider.base.ac->prevPos);
    this->actionFunc = EnBili_Recoil;
    this->actor.speedXZ = 5.0f;
}

/**
 * Used for both fire damage and generic damage
 */
void EnBili_SetupBurnt(EnBili* this) {
    if (this->actionFunc == EnBili_Climb) {
        Animation_PlayLoop(&this->skelAnime, &gBiriDefaultAnim);
    }

    this->timer = 20;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
    this->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xC8, 0x2000, 0x14);
    this->actionFunc = EnBili_Burnt;
}

void EnBili_SetupDie(EnBili* this) {
    this->timer = 18;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->actionFunc = EnBili_Die;
    this->actor.speedXZ = 0.0f;
    GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
}

/**
 * Falls to ground
 */
void EnBili_SetupStunned(EnBili* this) {
    this->timer = 80;
    this->collider.info.bumper.effect = 0;
    this->actor.gravity = -1.0f;
    this->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&this->actor, 0, 0x96, 0x2000, 0x50);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->collider.base.atFlags &= ~AT_ON;
    this->actionFunc = EnBili_Stunned;
}

void EnBili_SetupFrozen(EnBili* this, PlayState* play) {
    s32 i;
    Vec3f effectPos;

    if (!(this->actor.flags & ACTOR_FLAG_DRAGGED_BY_ARROW)) {
        this->actor.gravity = -1.0f;
    }

    this->actor.velocity.y = 0.0f;
    effectPos.y = this->actor.world.pos.y - 15.0f;

    for (i = 0; i < 8; i++) {

        effectPos.x = this->actor.world.pos.x + ((i & 1) ? 7.0f : -7.0f);
        effectPos.y += 2.5f;
        effectPos.z = this->actor.world.pos.z + ((i & 4) ? 7.0f : -7.0f);

        EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &effectPos, 150, 150, 150, 250, 235, 245, 255,
                                       (Rand_ZeroOne() * 0.2f) + 0.7f);
    }

    this->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&this->actor, 0, 0x96, 0x2000, 0xA);
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->timer = 300;
    this->actionFunc = EnBili_Frozen;
}

// Miscellaneous

/**
 * Changes the texture displayed on the oral arms limb using the current frame.
 */
void EnBili_UpdateTentaclesIndex(EnBili* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 temp; // Not strictly necessary, but avoids a few s16 casts

    if (this->actionFunc == EnBili_DischargeLightning) {
        temp = 3 - curFrame;
        this->tentaclesTexIndex = (ABS(temp) + 5) % 8;
    } else if (this->actionFunc == EnBili_Climb) {
        if (curFrame <= 9) {
            temp = curFrame >> 1;
            this->tentaclesTexIndex = CLAMP_MAX(temp, 3);
        } else if (curFrame <= 18) {
            temp = 17 - curFrame;
            this->tentaclesTexIndex = CLAMP_MIN(temp, 0) >> 1;
        } else if (curFrame <= 36) {
            this->tentaclesTexIndex = ((36 - curFrame) / 3) + 2;
        } else {
            this->tentaclesTexIndex = (40 - curFrame) >> 1;
        }
    } else {
        this->tentaclesTexIndex = curFrame >> 1;
    }
}

/**
 * Tracks Player height, with oscillation, and moves away from walls
 */
void EnBili_UpdateFloating(EnBili* this) {
    f32 playerHeight = this->actor.world.pos.y + this->actor.yDistToPlayer;
    f32 heightOffset = ((this->actionFunc == EnBili_SetNewHomeHeight) ? 100.0f : 40.0f);
    f32 baseHeight = CLAMP_MIN(this->actor.floorHeight, playerHeight);

    Math_StepToF(&this->actor.home.pos.y, baseHeight + heightOffset, 1.0f);
    this->actor.world.pos.y = this->actor.home.pos.y + (sinf(this->timer * (M_PI / 16)) * 3.0f);

    // Turn around if touching wall
    if (this->actor.bgCheckFlags & 8) {
        this->actor.world.rot.y = this->actor.wallYaw;
    }
}

// Action functions

void EnBili_FloatIdle(EnBili* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->timer % 4)) {
        this->actor.world.rot.y += Rand_CenteredFloat(1820.0f);
    }

    EnBili_UpdateFloating(this);

    if (this->timer == 0) {
        this->timer = 32;
    }

    if ((this->actor.xzDistToPlayer < 160.0f) && (fabsf(this->actor.yDistToPlayer) < 45.0f)) {
        EnBili_SetupApproachPlayer(this);
    }
}

void EnBili_SpawnedFlyApart(EnBili* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        EnBili_SetupFloatIdle(this);
    }
}

void EnBili_DischargeLightning(EnBili* this, PlayState* play) {
    static Color_RGBA8 primColor = { 255, 255, 255, 255 };
    static Color_RGBA8 envColor = { 200, 255, 255, 255 };
    s32 i;
    Vec3f effectPos;
    s16 effectYaw;

    for (i = 0; i < 4; i++) {
        if (!((this->timer + (i << 1)) % 4)) {
            effectYaw = (s16)Rand_CenteredFloat(12288.0f) + (i * 0x4000) + 0x2000;
            effectPos.x = Rand_CenteredFloat(5.0f) + this->actor.world.pos.x;
            effectPos.y = (Rand_ZeroOne() * 5.0f) + this->actor.world.pos.y + 2.5f;
            effectPos.z = Rand_CenteredFloat(5.0f) + this->actor.world.pos.z;
            EffectSsLightning_Spawn(play, &effectPos, &primColor, &envColor, 15, effectYaw, 6, 2);
        }
    }

    SkelAnime_Update(&this->skelAnime);
    func_8002F974(&this->actor, NA_SE_EN_BIRI_SPARK - SFX_FLAG);

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.velocity.y *= -1.0f;

    if ((this->timer == 0) && Animation_OnFrame(&this->skelAnime, 0.0f)) {
        if (this->actor.params == EN_BILI_TYPE_DYING) {
            EnBili_SetupDie(this);
        } else {
            EnBili_SetupClimb(this);
        }
    }
}

void EnBili_Climb(EnBili* this, PlayState* play) {
    s32 skelAnimeUpdate = SkelAnime_Update(&this->skelAnime);
    f32 curFrame = this->skelAnime.curFrame;

    if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BIRI_JUMP);
    }

    if (curFrame > 9.0f) {
        Math_ApproachF(&this->actor.world.pos.y, this->actor.world.pos.y + this->actor.yDistToPlayer + 100.0f, 0.5f,
                       5.0f);
    }

    if (skelAnimeUpdate) {
        EnBili_SetupSetNewHomeHeight(this);
    }
}

void EnBili_ApproachPlayer(EnBili* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 1820);

    if (this->timer != 0) {
        this->timer--;
    }

    EnBili_UpdateFloating(this);

    if (this->timer == 0) {
        this->timer = 32;
    }

    if (this->actor.xzDistToPlayer > 200.0f) {
        EnBili_SetupFloatIdle(this);
    }
}

void EnBili_SetNewHomeHeight(EnBili* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    Math_ScaledStepToS(&this->actor.world.rot.y, (s16)(this->actor.yawTowardsPlayer + 0x8000), 910);
    EnBili_UpdateFloating(this);

    if (this->timer == 0) {
        EnBili_SetupFloatIdle(this);
    }
}

void EnBili_Recoil(EnBili* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Math_StepToF(&this->actor.speedXZ, 0.0f, 0.3f)) {
        this->actor.world.rot.y += 0x8000;
        EnBili_SetupFloatIdle(this);
    }
}

void EnBili_Burnt(EnBili* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.flags & ACTOR_FLAG_DRAGGED_BY_ARROW) {
        this->actor.colorFilterTimer = 20;
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            EnBili_SetupDie(this);
        }
    }
}

void EnBili_Die(EnBili* this, PlayState* play) {
    static Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f effectAccel = { 0.0f, 0.0f, 0.0f };
    s16 effectScale;
    Vec3f effectPos;
    s32 i;

    if (this->actor.draw != NULL) {
        if (this->actor.flags & ACTOR_FLAG_DRAGGED_BY_ARROW) {
            return;
        }
        this->actor.draw = NULL;
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x50);
    }

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer != 0) {
        for (i = 0; i < 2; i++) {
            effectPos.x = ((Rand_ZeroOne() * 10.0f) + this->actor.world.pos.x) - 5.0f;
            effectPos.y = ((Rand_ZeroOne() * 5.0f) + this->actor.world.pos.y) - 2.5f;
            effectPos.z = ((Rand_ZeroOne() * 10.0f) + this->actor.world.pos.z) - 5.0f;

            effectVelocity.y = Rand_ZeroOne() + 1.0f;
            effectScale = Rand_S16Offset(40, 40);

            if (Rand_ZeroOne() < 0.7f) {
                EffectSsDtBubble_SpawnColorProfile(play, &effectPos, &effectVelocity, &effectAccel, effectScale,
                                                   25, 2, 1);
            } else {
                EffectSsDtBubble_SpawnColorProfile(play, &effectPos, &effectVelocity, &effectAccel, effectScale,
                                                   25, 0, 1);
            }
        }
    } else {
        Actor_Kill(&this->actor);
    }

    if (this->timer == 14) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_BIRI_BUBLE);
    }
}

void EnBili_Stunned(EnBili* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
    }

    if (this->timer == 0) {
        EnBili_SetupFloatIdle(this);
    }
}

void EnBili_Frozen(EnBili* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->actor.flags & ACTOR_FLAG_DRAGGED_BY_ARROW)) {
        this->actor.gravity = -1.0f;
    }

    if ((this->actor.bgCheckFlags & 1) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
        this->actor.colorFilterTimer = 0;
        EnBili_SetupDie(this);
    } else {
        this->actor.colorFilterTimer = 10;
    }
}

void EnBili_UpdateDamage(EnBili* this, PlayState* play) {
    u8 damageEffect;

    if ((this->actor.colChkInfo.health != 0) && (this->collider.base.acFlags & AC_HIT)) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.info, 1);

        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (Actor_ApplyDamage(&this->actor) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BIRI_DEAD);
                Enemy_StartFinishingBlow(play, &this->actor);
                this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
            }

            damageEffect = this->actor.colChkInfo.damageEffect;

            if (damageEffect == BIRI_DMGEFF_DEKUNUT) {
                if (this->actionFunc != EnBili_Stunned) {
                    EnBili_SetupStunned(this);
                }
            } else if (damageEffect == BIRI_DMGEFF_SWORD) {
                if (this->actionFunc != EnBili_Stunned) {
                    Actor_SetColorFilter(&this->actor, 0x4000, 0xC8, 0x2000, 0xA);

                    if (this->actor.colChkInfo.health == 0) {
                        this->actor.params = EN_BILI_TYPE_DYING;
                    }
                    EnBili_SetupDischargeLightning(this);
                } else {
                    EnBili_SetupBurnt(this);
                }
            } else if (damageEffect == BIRI_DMGEFF_FIRE) {
                EnBili_SetupBurnt(this);
                this->timer = 2;
            } else if (damageEffect == BIRI_DMGEFF_ICE) {
                EnBili_SetupFrozen(this, play);
            } else if (damageEffect == BIRI_DMGEFF_SLINGSHOT) {
                EnBili_SetupRecoil(this);
            } else {
                EnBili_SetupBurnt(this);
            }

            if (this->collider.info.acHitInfo->toucher.dmgFlags & 0x1F820) { // DMG_ARROW
                this->actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
            }
        }
    }
}

void EnBili_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBili* this = (EnBili*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        EnBili_SetupDischargeLightning(this);
    }

    EnBili_UpdateDamage(this, play);
    this->actionFunc(this, play);

    if (this->actionFunc != EnBili_Die) {
        EnBili_UpdateTentaclesIndex(this);
        if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
            if ((this->actionFunc == EnBili_FloatIdle) || (this->actionFunc == EnBili_SetNewHomeHeight) ||
                (this->actionFunc == EnBili_ApproachPlayer) || (this->actionFunc == EnBili_Recoil)) {
                if (this->playFlySound) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BIRI_FLY);
                    this->playFlySound = false;
                } else {
                    this->playFlySound = true;
                }
            }
        }
        if (this->actionFunc == EnBili_Recoil) {
            func_8002D97C(&this->actor);
        } else {
            Actor_MoveForward(&this->actor);
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 5.0f, this->collider.dim.radius, this->collider.dim.height, 7);
        Collider_UpdateCylinder(&this->actor, &this->collider);

        if (this->collider.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        Actor_SetFocus(&this->actor, 0.0f);
    }
}

// Draw and associated functions

void EnBili_PulseLimb3(EnBili* this, f32 frame, Vec3f* arg2) {
    f32 cos;
    f32 sin;

    if (this->actionFunc == EnBili_DischargeLightning) {
        arg2->y = 1.0f - (sinf((M_PI * 0.16667f) * frame) * 0.26f);
    } else if (this->actionFunc == EnBili_Climb) {
        if (frame <= 8.0f) {
            arg2->y = (cosf((M_PI * 0.125f) * frame) * 0.15f) + 0.85f;
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (M_PI * 0.1f));
            arg2->y = 1.0f - (0.3f * cos);
            arg2->x = (0.2f * cos) + 0.8f;
        } else {
            cos = cosf((frame - 18.0f) * (M_PI * 0.0227f));
            arg2->y = (0.31f * cos) + 1.0f;
            arg2->x = 1.0f - (0.4f * cos);
        }

        arg2->z = arg2->x;
    } else if (this->actionFunc == EnBili_Stunned) {
        sin = sinf((M_PI * 0.1f) * this->timer) * 0.08f;
        arg2->x -= sin;
        arg2->y += sin;
        arg2->z -= sin;
    } else {
        arg2->y = (cosf((M_PI * 0.125f) * frame) * 0.13f) + 0.87f;
    }
}

void EnBili_PulseLimb2(EnBili* this, f32 frame, Vec3f* arg2) {
    f32 cos;
    f32 sin;

    if (this->actionFunc == EnBili_DischargeLightning) {
        arg2->y = (sinf((M_PI * 0.16667f) * frame) * 0.2f) + 1.0f;
    } else if (this->actionFunc == EnBili_Climb) {
        if (frame <= 8.0f) {
            arg2->x = 1.125f - (cosf((M_PI * 0.125f) * frame) * 0.125f);
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (M_PI * 0.1f));
            arg2->x = (0.275f * cos) + 0.975f;
            arg2->y = 1.25f - (0.25f * cos);
        } else {
            cos = cosf((frame - 18.0f) * (M_PI * 0.0227f));
            arg2->x = 1.0f - (0.3f * cos);
            arg2->y = (0.48f * cos) + 1.0f;
        }
        arg2->z = arg2->x;
    } else if (this->actionFunc == EnBili_Stunned) {
        sin = sinf((M_PI * 0.1f) * this->timer) * 0.08f;
        arg2->x += sin;
        arg2->y -= sin;
        arg2->z += sin;
    } else {
        arg2->y = 1.1f - (cosf((M_PI * 0.125f) * frame) * 0.1f);
    }
}

void EnBili_PulseLimb4(EnBili* this, f32 frame, Vec3f* arg2) {
    f32 cos;

    if (this->actionFunc == EnBili_Climb) {
        if (frame <= 8.0f) {
            cos = cosf((M_PI * 0.125f) * frame);
            arg2->x = 1.125f - (0.125f * cos);
            arg2->y = (0.3f * cos) + 0.7f;
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (M_PI * 0.1f));
            arg2->x = (0.325f * cos) + 0.925f;
            arg2->y = 0.95f - (0.55f * cos);
        } else {
            cos = cosf((frame - 18.0f) * (M_PI * 0.0227f));
            arg2->x = 1.0f - (0.4f * cos);
            arg2->y = (0.52f * cos) + 1.0f;
        }
        arg2->z = arg2->x;
    }
}

s32 EnBili_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                            Gfx** gfx) {
    EnBili* this = (EnBili*)thisx;
    Vec3f limbScale = { 1.0f, 1.0f, 1.0f };
    f32 curFrame = this->skelAnime.curFrame;

    if (limbIndex == EN_BILI_LIMB_OUTER_HOOD) {
        EnBili_PulseLimb3(this, curFrame, &limbScale);
    } else if (limbIndex == EN_BILI_LIMB_INNER_HOOD) {
        EnBili_PulseLimb2(this, curFrame, &limbScale);
    } else if (limbIndex == EN_BILI_LIMB_TENTACLES) {
        EnBili_PulseLimb4(this, curFrame, &limbScale);
        rot->y = (Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y) + 0x8000;
    }

    Matrix_Scale(limbScale.x, limbScale.y, limbScale.z, MTXMODE_APPLY);
    return false;
}

static void* sTentaclesTextures[] = {
    gBiriTentacles0Tex, gBiriTentacles1Tex, gBiriTentacles2Tex, gBiriTentacles3Tex,
    gBiriTentacles4Tex, gBiriTentacles5Tex, gBiriTentacles6Tex, gBiriTentacles7Tex,
};

#include "overlays/ovl_En_Bili/ovl_En_Bili.h"

void EnBili_Draw(Actor* thisx, PlayState* play) {
    EnBili* this = (EnBili*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    this->tentaclesTexIndex = CLAMP_MAX(this->tentaclesTexIndex, 7);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTentaclesTextures[this->tentaclesTexIndex]));

    if ((this->actionFunc == EnBili_DischargeLightning) && ((this->timer & 1) != 0)) {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_809C16F0);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_809C1700);
    }

    POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   EnBili_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);
    CLOSE_DISPS(play->state.gfxCtx);
}

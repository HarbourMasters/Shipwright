/*
 * File: z_en_vali.c
 * Overlay: ovl_En_Vali
 * Description: Bari (Big Jellyfish)
 */

#include "z_en_vali.h"
#include "objects/object_vali/object_vali.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_12)

void EnVali_Init(Actor* thisx, PlayState* play);
void EnVali_Destroy(Actor* thisx, PlayState* play);
void EnVali_Update(Actor* thisx, PlayState* play);
void EnVali_Draw(Actor* thisx, PlayState* play);

void EnVali_SetupLurk(EnVali* this);
void EnVali_SetupDropAppear(EnVali* this);

void EnVali_Lurk(EnVali* this, PlayState* play);
void EnVali_DropAppear(EnVali* this, PlayState* play);
void EnVali_FloatIdle(EnVali* this, PlayState* play);
void EnVali_Attacked(EnVali* this, PlayState* play);
void EnVali_Retaliate(EnVali* this, PlayState* play);
void EnVali_MoveArmsDown(EnVali* this, PlayState* play);
void EnVali_Burnt(EnVali* this, PlayState* play);
void EnVali_DivideAndDie(EnVali* this, PlayState* play);
void EnVali_Stunned(EnVali* this, PlayState* play);
void EnVali_Frozen(EnVali* this, PlayState* play);
void EnVali_ReturnToLurk(EnVali* this, PlayState* play);

const ActorInit En_Vali_InitVars = {
    ACTOR_EN_VALI,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_VALI,
    sizeof(EnVali),
    (ActorFunc)EnVali_Init,
    (ActorFunc)EnVali_Destroy,
    (ActorFunc)EnVali_Update,
    (ActorFunc)EnVali_Draw,
    NULL,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x07, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
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
        { 0xFFCFFFFF, 0x07, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 17, 35, -15, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 18, 32, MASS_HEAVY };

typedef enum {
    /* 0x0 */ BARI_DMGEFF_NONE,
    /* 0x1 */ BARI_DMGEFF_STUN,
    /* 0x2 */ BARI_DMGEFF_FIRE,
    /* 0x3 */ BARI_DMGEFF_ICE,
    /* 0xE */ BARI_DMGEFF_SLINGSHOT = 0xE,
    /* 0xF */ BARI_DMGEFF_SWORD
} BariDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, BARI_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(0, BARI_DMGEFF_SLINGSHOT),
    /* Explosive     */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(0, BARI_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, BARI_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, BARI_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, BARI_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, BARI_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, BARI_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, BARI_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(4, BARI_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, BARI_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, BARI_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, BARI_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, BARI_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, BARI_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x18, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5000, ICHAIN_STOP),
};

void EnVali_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVali* this = (EnVali*)thisx;
    s32 bgId;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 27.0f);
    this->actor.shape.shadowAlpha = 155;
    SkelAnime_Init(play, &this->skelAnime, &gBariSkel, &gBariLurkingAnim, this->jointTable, this->morphTable,
                   EN_VALI_LIMB_MAX);

    Collider_InitQuad(play, &this->leftArmCollider);
    Collider_SetQuad(play, &this->leftArmCollider, &this->actor, &sQuadInit);
    Collider_InitQuad(play, &this->rightArmCollider);
    Collider_SetQuad(play, &this->rightArmCollider, &this->actor, &sQuadInit);
    Collider_InitCylinder(play, &this->bodyCollider);
    Collider_SetCylinder(play, &this->bodyCollider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    EnVali_SetupLurk(this);

    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.floorHeight = BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                          &this->actor, &this->actor.world.pos);
    this->actor.params = BARI_TYPE_NORMAL;

    if (this->actor.floorHeight == BGCHECK_Y_MIN) {
        Actor_Kill(&this->actor);
    }
}

void EnVali_Destroy(Actor* thisx, PlayState* play) {
    EnVali* this = (EnVali*)thisx;

    Collider_DestroyQuad(play, &this->leftArmCollider);
    Collider_DestroyQuad(play, &this->rightArmCollider);
    Collider_DestroyCylinder(play, &this->bodyCollider);
}

void EnVali_SetupLurk(EnVali* this) {
    Animation_PlayLoop(&this->skelAnime, &gBariLurkingAnim);
    this->actor.draw = NULL;
    this->bodyCollider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnVali_Lurk;
}

void EnVali_SetupDropAppear(EnVali* this) {
    this->actor.draw = EnVali_Draw;
    this->actor.flags |= ACTOR_FLAG_0;
    this->actor.velocity.y = 1.0f;
    this->actionFunc = EnVali_DropAppear;
}

void EnVali_SetupFloatIdle(EnVali* this) {
    Animation_MorphToLoop(&this->skelAnime, &gBariWaitingAnim, -3.0f);
    this->leftArmCollider.dim.quad[2] = this->leftArmCollider.dim.quad[3] = this->rightArmCollider.dim.quad[2] =
        this->rightArmCollider.dim.quad[3] = this->leftArmCollider.dim.quad[0] = this->leftArmCollider.dim.quad[1] =
            this->rightArmCollider.dim.quad[0] = this->rightArmCollider.dim.quad[1] = this->actor.world.pos;

    this->leftArmCollider.dim.quad[2].y = this->leftArmCollider.dim.quad[3].y = this->rightArmCollider.dim.quad[2].y =
        this->rightArmCollider.dim.quad[3].y = this->leftArmCollider.dim.quad[0].y =
            this->leftArmCollider.dim.quad[1].y = this->rightArmCollider.dim.quad[0].y =
                this->rightArmCollider.dim.quad[1].y = this->actor.world.pos.y - 10.0f;

    this->actor.flags &= ~ACTOR_FLAG_4;
    this->bodyCollider.base.acFlags |= AC_ON;
    this->slingshotReactionTimer = 0;
    this->floatHomeHeight = this->actor.world.pos.y;
    this->actionFunc = EnVali_FloatIdle;
}

/**
 * Used for both touching player/player's shield and being hit with sword. What to do next is determined by params.
 */
void EnVali_SetupAttacked(EnVali* this) {
    this->lightningTimer = 20;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->bodyCollider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnVali_Attacked;
}

void EnVali_SetupRetaliate(EnVali* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBariRetaliatingAnim, -5.0f);
    Actor_SetColorFilter(&this->actor, 0x4000, 150, 0x2000, 30);
    this->actor.params = BARI_TYPE_NORMAL;
    this->bodyCollider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnVali_Retaliate;
}

void EnVali_SetupMoveArmsDown(EnVali* this) {
    Animation_PlayOnce(&this->skelAnime, &gBariMovingArmsDownAnim);
    this->actionFunc = EnVali_MoveArmsDown;
}

void EnVali_SetupBurnt(EnVali* this) {
    this->timer = 2;
    this->bodyCollider.base.acFlags &= ~AC_ON;
    Actor_SetColorFilter(&this->actor, 0x4000, 150, 0x2000, 30);
    this->actionFunc = EnVali_Burnt;
}

void EnVali_SetupDivideAndDie(EnVali* this, PlayState* play) {
    s32 i;

    for (i = 0; i < 3; i++) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BILI, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, 0);

        this->actor.world.rot.y += 0x10000 / 3;
    }

    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x50);
    this->timer = Rand_S16Offset(10, 10);
    this->bodyCollider.base.acFlags &= ~AC_ON;
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_BARI_SPLIT);
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.draw = NULL;
    this->actionFunc = EnVali_DivideAndDie;
    gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BARI]++;
}

void EnVali_SetupStunned(EnVali* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBariWaitingAnim, 10.0f);
    this->timer = 80;
    this->actor.velocity.y = 0.0f;
    Actor_SetColorFilter(&this->actor, 0, 255, 0x2000, 80);
    this->bodyCollider.info.bumper.effect = 0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actor.velocity.y = 1.0f;
    this->actionFunc = EnVali_Stunned;
}

void EnVali_SetupFrozen(EnVali* this) {
    this->actor.velocity.y = 0.0f;
    Actor_SetColorFilter(&this->actor, 0, 255, 0x2000, 36);
    this->bodyCollider.base.acFlags &= ~AC_ON;
    this->timer = 36;
    this->actionFunc = EnVali_Frozen;
}

void EnVali_SetupReturnToLurk(EnVali* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBariLurkingAnim, 10.0f);
    this->actor.flags |= ACTOR_FLAG_4;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actionFunc = EnVali_ReturnToLurk;
}

void EnVali_DischargeLightning(EnVali* this, PlayState* play) {
    static Color_RGBA8 primColor = { 255, 255, 255, 255 };
    static Color_RGBA8 envColor = { 200, 255, 255, 255 };
    Vec3f pos;
    s32 i;
    f32 cos;
    f32 sin;
    s16 yaw;

    for (i = 0; i < 4; i++) {
        cos = -Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));
        sin = Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));
        if (!((this->lightningTimer + (i << 1)) % 4)) {
            yaw = (s16)Rand_CenteredFloat(12288.0f) + (i * 0x4000) + 0x2000;
            pos.x = this->actor.world.pos.x + (Math_SinS(yaw) * 12.0f * cos);
            pos.y = this->actor.world.pos.y - (Math_CosS(yaw) * 12.0f) + 10.0f;
            pos.z = this->actor.world.pos.z + (Math_SinS(yaw) * 12.0f * sin);

            EffectSsLightning_Spawn(play, &pos, &primColor, &envColor, 17, yaw, 6, 2);
        }
    }

    func_8002F974(&this->actor, NA_SE_EN_BIRI_SPARK - SFX_FLAG);
}

void EnVali_Lurk(EnVali* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 150.0f) {
        EnVali_SetupDropAppear(this);
    }
}

void EnVali_DropAppear(EnVali* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.velocity.y *= 1.5f;
    this->actor.velocity.y = CLAMP_MAX(this->actor.velocity.y, 40.0f);

    if (Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, this->actor.velocity.y)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        EnVali_SetupFloatIdle(this);
    }
}

void EnVali_FloatIdle(EnVali* this, PlayState* play) {
    s32 curFrame;

    SkelAnime_Update(&this->skelAnime);

    if (this->slingshotReactionTimer != 0) {
        this->slingshotReactionTimer--;
    }

    curFrame = this->skelAnime.curFrame;

    Math_StepToF(&this->floatHomeHeight, this->actor.floorHeight + 40.0f, 1.2f);
    this->actor.world.pos.y = this->floatHomeHeight - (sinf(curFrame * M_PI * 0.0125f) * 8.0f);

    if (this->slingshotReactionTimer) {
        this->actor.shape.rot.y += 0x800;

        if (((this->slingshotReactionTimer % 6) == 0) && (curFrame > 15) && (curFrame <= 55)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BARI_ROLL);
        }
    } else if ((curFrame == 16) || (curFrame == 30) || (curFrame == 42) || (curFrame == 55)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BARI_ROLL);
    }

    curFrame = ((curFrame > 40) ? (80 - curFrame) : curFrame);

    this->actor.shape.rot.y += (s16)((curFrame + 4) * 0.4f * (0x10000 / 360.0f));
    if (this->actor.xzDistToPlayer > 250.0f) {
        EnVali_SetupReturnToLurk(this);
    }
}

void EnVali_Attacked(EnVali* this, PlayState* play) {
    if (this->lightningTimer != 0) {
        this->lightningTimer--;
    }

    EnVali_DischargeLightning(this, play);

    if (this->lightningTimer == 0) {
        this->actor.flags |= ACTOR_FLAG_0;
        this->bodyCollider.base.acFlags |= AC_ON;
        if (this->actor.params == BARI_TYPE_SWORD_DAMAGE) {
            EnVali_SetupRetaliate(this);
        } else {
            this->actionFunc = EnVali_FloatIdle;
        }
    } else if ((this->lightningTimer % 2) != 0) {
        this->actor.world.pos.y += 1.0f;
    } else {
        this->actor.world.pos.y -= 1.0f;
    }
}

void EnVali_Retaliate(EnVali* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.colChkInfo.health != 0) {
            EnVali_SetupMoveArmsDown(this);
        } else {
            EnVali_SetupDivideAndDie(this, play);
        }
    }
}

void EnVali_MoveArmsDown(EnVali* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnVali_SetupFloatIdle(this);
    }
}

void EnVali_Burnt(EnVali* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        EnVali_SetupDivideAndDie(this, play);
    }
}

void EnVali_DivideAndDie(EnVali* this, PlayState* play) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s16 scale;
    Vec3f pos;
    s32 i;

    if (this->timer != 0) {
        this->timer--;
    }

    for (i = 0; i < 2; i++) {
        pos.x = this->actor.world.pos.x + Rand_CenteredFloat(20.0f);
        pos.y = this->actor.world.pos.y + Rand_CenteredFloat(8.0f);
        pos.z = this->actor.world.pos.z + Rand_CenteredFloat(20.0f);
        velocity.y = (Rand_ZeroOne() + 1.0f);
        scale = Rand_S16Offset(40, 40);

        if (Rand_ZeroOne() < 0.7f) {
            EffectSsDtBubble_SpawnColorProfile(play, &pos, &velocity, &accel, scale, 25, 2, 1);
        } else {
            EffectSsDtBubble_SpawnColorProfile(play, &pos, &velocity, &accel, scale, 25, 0, 1);
        }
    }

    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnVali_Stunned(EnVali* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->actor.velocity.y != 0.0f) {
        if (Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, this->actor.velocity.y)) {
            this->actor.velocity.y = 0.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        } else {
            this->actor.velocity.y += 1.0f;
        }
    }

    if (this->timer == 0) {
        this->bodyCollider.info.bumper.effect = 1; // Shock?
        EnVali_SetupFloatIdle(this);
    }
}

void EnVali_Frozen(EnVali* this, PlayState* play) {
    Vec3f pos;
    s32 temp_v0;
    s32 temp_v1;

    if (this->timer != 0) {
        this->timer--;
    }

    temp_v1 = this->timer - 20;
    this->actor.colorFilterTimer = 36;

    if (temp_v1 > 0) {
        temp_v0 = temp_v1 >> 1;

        if ((this->timer % 2) != 0) {
            pos.y = this->actor.world.pos.y - 20.0f + (-temp_v0 * 5 + 40);
            pos.x = this->actor.world.pos.x + ((temp_v0 & 2) ? 12.0f : -12.0f);
            pos.z = this->actor.world.pos.z + ((temp_v0 & 1) ? 12.0f : -12.0f);

            EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &pos, 150, 150, 150, 250, 235, 245, 255,
                                           (Rand_ZeroOne() * 0.2f) + 1.3f);
        }
    } else if (this->timer == 0) {
        this->actor.velocity.y += 1.0f;
        if (Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, this->actor.velocity.y)) {
            EnVali_SetupDivideAndDie(this, play);
            this->actor.colorFilterTimer = 0;
        }
    }
}

void EnVali_ReturnToLurk(EnVali* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f, 15.0f, 0.1f) < 0.01f) {
        EnVali_SetupLurk(this);
    }
}

void EnVali_UpdateDamage(EnVali* this, PlayState* play) {
    if (this->bodyCollider.base.acFlags & AC_HIT) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->bodyCollider.info, 1);

        if ((this->actor.colChkInfo.damageEffect != BARI_DMGEFF_NONE) || (this->actor.colChkInfo.damage != 0)) {
            if (Actor_ApplyDamage(&this->actor) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BARI_DEAD);
                Enemy_StartFinishingBlow(play, &this->actor);
                this->actor.flags &= ~ACTOR_FLAG_0;
            } else if ((this->actor.colChkInfo.damageEffect != BARI_DMGEFF_STUN) &&
                       (this->actor.colChkInfo.damageEffect != BARI_DMGEFF_SLINGSHOT)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BARI_DAMAGE);
            }

            if (this->actor.colChkInfo.damageEffect == BARI_DMGEFF_STUN) {
                if (this->actionFunc != EnVali_Stunned) {
                    EnVali_SetupStunned(this);
                }
            } else if (this->actor.colChkInfo.damageEffect == BARI_DMGEFF_SWORD) {
                if (this->actionFunc != EnVali_Stunned) {
                    Actor_SetColorFilter(&this->actor, 0x4000, 150, 0x2000, 30);
                    this->actor.params = BARI_TYPE_SWORD_DAMAGE;
                    EnVali_SetupAttacked(this);
                } else {
                    EnVali_SetupRetaliate(this);
                }
            } else if (this->actor.colChkInfo.damageEffect == BARI_DMGEFF_FIRE) {
                EnVali_SetupBurnt(this);
            } else if (this->actor.colChkInfo.damageEffect == BARI_DMGEFF_ICE) {
                EnVali_SetupFrozen(this);
            } else if (this->actor.colChkInfo.damageEffect == BARI_DMGEFF_SLINGSHOT) {
                if (this->slingshotReactionTimer == 0) {
                    this->slingshotReactionTimer = 20;
                }
            } else {
                EnVali_SetupRetaliate(this);
            }
        }
    }
}

void EnVali_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVali* this = (EnVali*)thisx;

    if ((this->bodyCollider.base.atFlags & AT_HIT) || (this->leftArmCollider.base.atFlags & AT_HIT) ||
        (this->rightArmCollider.base.atFlags & AT_HIT)) {
        this->leftArmCollider.base.atFlags &= ~AT_HIT;
        this->rightArmCollider.base.atFlags &= ~AT_HIT;
        this->bodyCollider.base.atFlags &= ~AT_HIT;
        EnVali_SetupAttacked(this);
    }

    EnVali_UpdateDamage(this, play);
    this->actionFunc(this, play);

    if ((this->actionFunc != EnVali_DivideAndDie) && (this->actionFunc != EnVali_Lurk)) {
        Collider_UpdateCylinder(&this->actor, &this->bodyCollider);

        if (this->actionFunc == EnVali_FloatIdle) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->leftArmCollider.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->rightArmCollider.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if (this->bodyCollider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
        Actor_SetFocus(&this->actor, 0.0f);
    }
}

// Draw and associated functions

void EnVali_PulseOutside(EnVali* this, f32 curFrame, Vec3f* scale) {
    f32 scaleChange;

    if (this->actionFunc == EnVali_Attacked) {
        s32 scalePhase = 20 - (this->lightningTimer % 20);

        if (scalePhase >= 10) {
            scalePhase -= 10;
        }

        scale->y -= 0.2f * sinf((M_PI / 10) * scalePhase);
    } else if (this->actionFunc == EnVali_Retaliate) {
        scaleChange = sinf((M_PI / 10) * curFrame);
        scale->y -= 0.24f * scaleChange;
        scale->x -= 0.13f * scaleChange;
        scale->z = scale->x;
    } else if (this->actionFunc == EnVali_MoveArmsDown) {
        scaleChange = cosf((M_PI / 50) * curFrame);
        scale->y -= 0.24f * scaleChange;
        scale->x -= 0.13f * scaleChange;
        scale->z = scale->x;
    } else if (this->actionFunc == EnVali_Stunned) {
        scaleChange = sinf((M_PI / 10) * this->timer) * 0.08f;
        scale->x += scaleChange;
        scale->y -= scaleChange;
        scale->z += scaleChange;
    } else {
        if (curFrame >= 40.0f) {
            curFrame -= 40.0f;
        }

        scale->y -= 0.2f * sinf((M_PI / 40) * curFrame);
    }
}

void EnVali_PulseInsides(EnVali* this, f32 curFrame, Vec3f* scale) {
    f32 scaleChange;

    if (this->actionFunc == EnVali_Attacked) {
        s32 scalePhase = 20 - (this->lightningTimer % 20);

        if (scalePhase >= 10) {
            scalePhase -= 10;
        }

        scale->y -= 0.13f * sinf((M_PI / 10) * scalePhase);
    } else if (this->actionFunc == EnVali_Retaliate) {
        scaleChange = sinf((M_PI / 10) * curFrame);
        scale->y -= 0.18f * scaleChange;
        scale->x -= 0.1f * scaleChange;
        scale->z = scale->x;
    } else if (this->actionFunc == EnVali_MoveArmsDown) {
        scaleChange = cosf((M_PI / 50) * curFrame);
        scale->y -= 0.18f * scaleChange;
        scale->x -= 0.1f * scaleChange;
        scale->z = scale->x;
    } else if (this->actionFunc == EnVali_Stunned) {
        scaleChange = sinf((M_PI / 10) * this->timer) * 0.08f;
        scale->x -= scaleChange;
        scale->y += scaleChange;
        scale->z -= scaleChange;
    } else {
        if (curFrame >= 40.0f) {
            curFrame -= 40.0f;
        }

        scale->y -= 0.13f * sinf((M_PI / 40) * curFrame);
    }
}

s32 EnVali_SetArmLength(EnVali* this, f32 curFrame) {
    f32 targetArmScale;

    if (this->actionFunc == EnVali_FloatIdle) {
        if (curFrame <= 10.0f) {
            targetArmScale = curFrame * 0.05f + 1.0f;
        } else if (curFrame > 70.0f) {
            targetArmScale = (80.0f - curFrame) * 0.05f + 1.0f;
        } else {
            targetArmScale = 1.5f;
        }
    } else if (this->actionFunc == EnVali_Retaliate) {
        targetArmScale = 1.0f - sinf((M_PI / 10) * curFrame) * 0.35f;
    } else if (this->actionFunc == EnVali_MoveArmsDown) {
        targetArmScale = 1.0f - cosf((M_PI / 50) * curFrame) * 0.35f;
    } else if ((this->actionFunc == EnVali_Attacked) || (this->actionFunc == EnVali_Frozen)) {
        targetArmScale = this->armScale;
    } else {
        targetArmScale = 1.0f;
    }

    Math_StepToF(&this->armScale, targetArmScale, 0.1f);

    if (this->armScale == 1.0f) {
        return false;
    } else {
        return true;
    }
}

s32 EnVali_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                            Gfx** gfx) {
    EnVali* this = (EnVali*)thisx;
    f32 curFrame;

    if ((limbIndex == EN_VALI_LIMB_NUCLEUS) || (limbIndex == EN_VALI_LIMB_OUTER_HOOD) ||
        (limbIndex == EN_VALI_LIMB_INNER_HOOD)) {
        *dList = NULL;
        return false;
    } else {
        curFrame = this->skelAnime.curFrame;

        if ((limbIndex == EN_VALI_LIMB_LEFT_ARM_BASE) || (limbIndex == EN_VALI_LIMB_RIGHT_ARM_BASE)) {
            if (EnVali_SetArmLength(this, curFrame)) {
                Matrix_Scale(this->armScale, 1.0f, 1.0f, MTXMODE_APPLY);
            }
        }

        return false;
    }
}

void EnVali_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f D_80B28970 = { 3000.0f, 0.0f, 0.0f };
    static Vec3f D_80B2897C = { -1000.0f, 0.0f, 0.0f };
    Vec3f sp3C;
    Vec3f sp30;
    EnVali* this = (EnVali*)thisx;

    if (this->actionFunc == EnVali_FloatIdle) {
        if ((limbIndex == EN_VALI_LIMB_LEFT_FOREARM_BASE) || (limbIndex == EN_VALI_LIMB_RIGHT_FOREARM_BASE)) {
            Matrix_MultVec3f(&D_80B28970, &sp3C);
            Matrix_MultVec3f(&D_80B2897C, &sp30);

            if (limbIndex == EN_VALI_LIMB_LEFT_FOREARM_BASE) {
                Collider_SetQuadVertices(&this->leftArmCollider, &sp30, &sp3C, &this->leftArmCollider.dim.quad[0],
                                         &this->leftArmCollider.dim.quad[1]);
            } else {
                Collider_SetQuadVertices(&this->rightArmCollider, &sp30, &sp3C, &this->rightArmCollider.dim.quad[0],
                                         &this->rightArmCollider.dim.quad[1]);
            }
        }
    }
}

void EnVali_DrawBody(EnVali* this, PlayState* play) {
    MtxF mtx;
    f32 cos;
    f32 sin;
    f32 curFrame;
    Vec3f scale = { 1.0f, 1.0f, 1.0f };

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Get(&mtx);
    curFrame = this->skelAnime.curFrame;
    EnVali_PulseInsides(this, curFrame, &scale);
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariInnerHoodDL);

    Matrix_Put(&mtx);
    Matrix_RotateY(-this->actor.shape.rot.y * (M_PI / 32768.0f), MTXMODE_APPLY);

    cos = Math_CosS(this->actor.shape.rot.y);
    sin = Math_SinS(this->actor.shape.rot.y);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariNucleusDL);

    Matrix_Translate((506.0f * cos) + (372.0f * sin), 1114.0f, (372.0f * cos) - (506.0f * sin), MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariNucleusDL);

    Matrix_Translate((-964.0f * cos) - (804.0f * sin), -108.0f, (-804.0f * cos) + (964.0f * sin), MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariNucleusDL);

    Matrix_Put(&mtx);

    scale.x = scale.y = scale.z = 1.0f;

    EnVali_PulseOutside(this, curFrame, &scale);
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariOuterHoodDL);

    Matrix_Put(&mtx);

    CLOSE_DISPS(play->state.gfxCtx);
}

static Gfx D_80B28998[] = {
    gsDPSetCombineLERP(1, TEXEL0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0, COMBINED, 0, PRIMITIVE, 0, TEXEL1, 0,
                       PRIM_LOD_FRAC, COMBINED),
    gsSPEndDisplayList(),
};

static Gfx D_80B289A8[] = {
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0, COMBINED, 0, PRIMITIVE, 0, TEXEL1, 0,
                       PRIM_LOD_FRAC, COMBINED),
    gsSPEndDisplayList(),
};

void EnVali_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVali* this = (EnVali*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TexScroll(play->state.gfxCtx, 0, (127 - (play->gameplayFrames * 12)) % 128, 32, 32));

    if ((this->lightningTimer % 2) != 0) {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_80B28998);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_80B289A8);
    }

    EnVali_DrawBody(this, play);

    POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   EnVali_OverrideLimbDraw, EnVali_PostLimbDraw, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

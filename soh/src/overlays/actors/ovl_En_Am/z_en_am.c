/*
 * File: z_en_am.c
 * Overlay: ovl_En_Am
 * Description: Armos
 */

#include "z_en_am.h"
#include "objects/object_am/object_am.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_26)

void EnAm_Init(Actor* thisx, PlayState* play);
void EnAm_Destroy(Actor* thisx, PlayState* play);
void EnAm_Update(Actor* thisx, PlayState* play);
void EnAm_Draw(Actor* thisx, PlayState* play);

void EnAm_SetupStatue(EnAm* this);
void EnAm_SetupSleep(EnAm* this);
void EnAm_Statue(EnAm* this, PlayState* play);
void EnAm_Sleep(EnAm* this, PlayState* play);
void EnAm_Lunge(EnAm* this, PlayState* play);
void EnAm_RotateToHome(EnAm* this, PlayState* play);
void EnAm_MoveToHome(EnAm* this, PlayState* play);
void EnAm_RotateToInit(EnAm* this, PlayState* play);
void EnAm_Cooldown(EnAm* this, PlayState* play);
void EnAm_Ricochet(EnAm* this, PlayState* play);
void EnAm_Stunned(EnAm* this, PlayState* play);
void EnAm_RecoilFromDamage(EnAm* this, PlayState* play);

typedef enum {
    /* 00 */ AM_BEHAVIOR_NONE,
    /* 01 */ AM_BEHAVIOR_DAMAGED,
    /* 03 */ AM_BEHAVIOR_DO_NOTHING = 3,
    /* 05 */ AM_BEHAVIOR_5 = 5, // checked but never set
    /* 06 */ AM_BEHAVIOR_STUNNED,
    /* 07 */ AM_BEHAVIOR_GO_HOME,
    /* 08 */ AM_BEHAVIOR_RICOCHET,
    /* 10 */ AM_BEHAVIOR_AGGRO = 10
} ArmosBehavior;

const ActorInit En_Am_InitVars = {
    ACTOR_EN_AM,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_AM,
    sizeof(EnAm),
    (ActorFunc)EnAm_Init,
    (ActorFunc)EnAm_Destroy,
    (ActorFunc)EnAm_Update,
    (ActorFunc)EnAm_Draw,
    NULL,
};

static ColliderCylinderInit sHurtCylinderInit = {
    {
        COLTYPE_HIT5,
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
    { 15, 70, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sBlockCylinderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00400106, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 15, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 00 */ AM_DMGEFF_NONE, // used by anything that cant kill the armos
    /* 01 */ AM_DMGEFF_NUT,
    /* 06 */ AM_DMGEFF_STUN = 6, // doesnt include deku nuts
    /* 13 */ AM_DMGEFF_ICE = 13,
    /* 14 */ AM_DMGEFF_MAGIC_FIRE_LIGHT,
    /* 15 */ AM_DMGEFF_KILL // any damage source that can kill the armos (and isnt a special case)
} ArmosDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, AM_DMGEFF_NUT),
    /* Deku stick    */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, AM_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Boomerang     */ DMG_ENTRY(0, AM_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Hammer swing  */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Hookshot      */ DMG_ENTRY(0, AM_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, AM_DMGEFF_NONE),
    /* Master sword  */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Giant's Knife */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Fire arrow    */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Ice arrow     */ DMG_ENTRY(4, AM_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Unk arrow 1   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(0, AM_DMGEFF_MAGIC_FIRE_LIGHT),
    /* Ice magic     */ DMG_ENTRY(3, AM_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, AM_DMGEFF_MAGIC_FIRE_LIGHT),
    /* Shield        */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, AM_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Master spin   */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Kokiri jump   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, AM_DMGEFF_KILL),
    /* Master jump   */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Unknown 1     */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Unknown 2     */ DMG_ENTRY(0, AM_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x13, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5300, ICHAIN_STOP),
};

void EnAm_SetupAction(EnAm* this, EnAmActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Checks bgCheckFlags in the direction of current yaw at the specified distance.
 *
 * Returns true if the armos would land on the ground in the resulting position.
 *
 * If it won't land on the ground, it will return true anyway if the floor the armos will be on
 * is no more than 20 units lower than the home position. This prevents the armos from going down steep slopes.
 */
s32 EnAm_CanMove(EnAm* this, PlayState* play, f32 distance, s16 yaw) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    // save current position and bgCheckFlags to restore later
    curPos = this->dyna.actor.world.pos;
    curBgCheckFlags = this->dyna.actor.bgCheckFlags;

    sin = Math_SinS(yaw) * distance;
    cos = Math_CosS(yaw) * distance;

    this->dyna.actor.world.pos.x += sin;
    this->dyna.actor.world.pos.z += cos;

    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, 4);
    this->dyna.actor.world.pos = curPos;
    ret = this->dyna.actor.bgCheckFlags & 1;

    if (!ret && (this->dyna.actor.floorHeight >= (this->dyna.actor.home.pos.y - 20.0f))) {
        ret = true;
    }

    this->dyna.actor.bgCheckFlags = curBgCheckFlags;

    return ret;
}

void EnAm_Init(Actor* thisx, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    s32 pad;
    EnAm* this = (EnAm*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    ActorShape_Init(&this->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
    SkelAnime_Init(play, &this->skelAnime, &gArmosSkel, &gArmosRicochetAnim, this->jointTable, this->morphTable,
                   14);
    Actor_SetScale(&this->dyna.actor, 0.01f);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    Collider_InitCylinder(play, &this->hurtCollider);
    Collider_InitCylinder(play, &this->blockCollider);
    Collider_SetCylinder(play, &this->hurtCollider, &this->dyna.actor, &sHurtCylinderInit);

    if (this->dyna.actor.params == ARMOS_STATUE) {
        this->dyna.actor.colChkInfo.health = 1;
        Collider_SetCylinder(play, &this->blockCollider, &this->dyna.actor, &sHurtCylinderInit);
        this->hurtCollider.base.ocFlags1 = (OC1_ON | OC1_NO_PUSH | OC1_TYPE_1 | OC1_TYPE_2);
        this->blockCollider.base.ocFlags1 = (OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER);
        CollisionHeader_GetVirtual(&gArmosCol, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        Actor_ChangeCategory(play, &play->actorCtx, &this->dyna.actor, ACTORCAT_BG);
        EnAm_SetupStatue(this);
    } else {
        Collider_SetCylinder(play, &this->blockCollider, &this->dyna.actor, &sBlockCylinderInit);
        Collider_InitQuad(play, &this->hitCollider);
        Collider_SetQuad(play, &this->hitCollider, &this->dyna.actor, &sQuadInit);
        this->dyna.actor.colChkInfo.health = 1;
        this->dyna.actor.colChkInfo.damageTable = &sDamageTable;
        EnAm_SetupSleep(this);
        this->unk_258 = 0;
    }

    this->dyna.actor.colChkInfo.mass = MASS_HEAVY;
}

void EnAm_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAm* this = (EnAm*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->hurtCollider);
    Collider_DestroyCylinder(play, &this->blockCollider);
    //! @bug Quad collider is not destroyed (though destroy doesnt really do anything anyway)
}

void EnAm_SpawnEffects(EnAm* this, PlayState* play) {
    static Vec3f velocity = { 0.0f, -1.5f, 0.0f };
    static Vec3f accel = { 0.0f, -0.2f, 0.0f };
    s32 i;
    Vec3f pos;
    Color_RGBA8 primColor = { 100, 100, 100, 0 };
    Color_RGBA8 envColor = { 40, 40, 40, 0 };
    s32 pad;

    for (i = 4; i > 0; i--) {
        pos.x = this->dyna.actor.world.pos.x + ((Rand_ZeroOne() - 0.5f) * 65.0f);
        pos.y = (this->dyna.actor.world.pos.y + 40.0f) + ((Rand_ZeroOne() - 0.5f) * 10.0f);
        pos.z = this->dyna.actor.world.pos.z + ((Rand_ZeroOne() - 0.5f) * 65.0f);

        EffectSsKiraKira_SpawnSmall(play, &pos, &velocity, &accel, &primColor, &envColor);
    }

    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_AMOS_WALK);
    Actor_SpawnFloorDustRing(play, &this->dyna.actor, &this->dyna.actor.world.pos, 4.0f, 3, 8.0f, 300, 15, false);
}

void EnAm_SetupSleep(EnAm* this) {
    f32 lastFrame = Animation_GetLastFrame(&gArmosRicochetAnim);

    Animation_Change(&this->skelAnime, &gArmosRicochetAnim, 0.0f, lastFrame, lastFrame, ANIMMODE_LOOP, 0.0f);
    this->behavior = AM_BEHAVIOR_DO_NOTHING;
    this->dyna.actor.speedXZ = 0.0f;
    this->unk_258 = (this->textureBlend == 255) ? 0 : 1;
    EnAm_SetupAction(this, EnAm_Sleep);
}

void EnAm_SetupStatue(EnAm* this) {
    f32 lastFrame = Animation_GetLastFrame(&gArmosRicochetAnim);

    Animation_Change(&this->skelAnime, &gArmosRicochetAnim, 0.0f, lastFrame, lastFrame, ANIMMODE_LOOP, 0.0f);
    this->dyna.actor.flags &= ~ACTOR_FLAG_0;
    this->behavior = AM_BEHAVIOR_DO_NOTHING;
    this->dyna.actor.speedXZ = 0.0f;
    EnAm_SetupAction(this, EnAm_Statue);
}

void EnAm_SetupLunge(EnAm* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gArmosHopAnim, 4.0f);
    this->unk_258 = 3;
    this->behavior = AM_BEHAVIOR_AGGRO;
    this->dyna.actor.speedXZ = 0.0f;
    this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y;
    EnAm_SetupAction(this, EnAm_Lunge);
}

void EnAm_SetupCooldown(EnAm* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gArmosHopAnim, 4.0f);
    this->unk_258 = 3;
    this->cooldownTimer = 40;
    this->behavior = AM_BEHAVIOR_AGGRO;
    this->dyna.actor.speedXZ = 0.0f;
    this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y;
    EnAm_SetupAction(this, EnAm_Cooldown);
}

void EnAm_SetupMoveToHome(EnAm* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gArmosHopAnim, 4.0f);
    this->behavior = AM_BEHAVIOR_GO_HOME;
    this->unk_258 = 1;
    this->dyna.actor.speedXZ = 0.0f;
    EnAm_SetupAction(this, EnAm_MoveToHome);
}

void EnAm_SetupRotateToInit(EnAm* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gArmosHopAnim, 4.0f);
    this->behavior = AM_BEHAVIOR_GO_HOME;
    this->unk_258 = 1;
    this->dyna.actor.speedXZ = 0.0f;
    EnAm_SetupAction(this, EnAm_RotateToInit);
}

void EnAm_SetupRotateToHome(EnAm* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gArmosHopAnim, 4.0f);
    this->behavior = AM_BEHAVIOR_GO_HOME;
    this->dyna.actor.speedXZ = 0.0f;
    this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y;
    EnAm_SetupAction(this, EnAm_RotateToHome);
}

void EnAm_SetupRecoilFromDamage(EnAm* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gArmosDamagedAnim, 1.0f, 4.0f,
                     Animation_GetLastFrame(&gArmosDamagedAnim) - 6.0f, ANIMMODE_ONCE, 0.0f);
    this->behavior = AM_BEHAVIOR_DAMAGED;
    this->dyna.actor.world.rot.y = this->dyna.actor.yawTowardsPlayer;
    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_AMOS_DAMAGE);

    if (EnAm_CanMove(this, play, -6.0f, this->dyna.actor.world.rot.y)) {
        this->dyna.actor.speedXZ = -6.0f;
    }

    this->dyna.actor.colorFilterTimer = 0;
    Enemy_StartFinishingBlow(play, &this->dyna.actor);
    EnAm_SetupAction(this, EnAm_RecoilFromDamage);
}

void EnAm_SetupRicochet(EnAm* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gArmosRicochetAnim, 1.0f, 0.0f, 8.0f, ANIMMODE_ONCE, 0.0f);
    this->dyna.actor.world.rot.y = this->dyna.actor.yawTowardsPlayer;

    if (EnAm_CanMove(this, play, -6.0f, this->dyna.actor.world.rot.y)) {
        this->dyna.actor.speedXZ = -6.0f;
    }

    this->unk_264 = 0;
    this->unk_258 = 0;
    this->cooldownTimer = 5;
    this->behavior = AM_BEHAVIOR_RICOCHET;
    EnAm_SetupAction(this, EnAm_Ricochet);
}

void EnAm_Sleep(EnAm* this, PlayState* play) {
    f32 cos;
    s32 pad;
    f32 rand;
    f32 sin;
    Player* player = GET_PLAYER(play);

    if ((this->unk_258 != 0) ||
        ((this->hurtCollider.base.ocFlags1 & OC1_HIT) && (this->hurtCollider.base.oc == &player->actor)) ||
        (this->hurtCollider.base.acFlags & AC_HIT)) {
        this->hurtCollider.base.acFlags &= ~AC_HIT;

        if (this->textureBlend == 0) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_AMOS_WAVE);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_AMOS_VOICE);
            Actor_SetColorFilter(&this->dyna.actor, 0x4000, 255, 0, 8);
        }

        if (this->textureBlend >= 240) {
            this->attackTimer = 200;
            this->textureBlend = 255;
            this->dyna.actor.flags |= ACTOR_FLAG_0;
            this->dyna.actor.shape.yOffset = 0.0f;
            EnAm_SetupLunge(this);
        } else {
            // shake randomly about the origin while waking up
            rand = (Rand_ZeroOne() - 0.5f) * 10.0f;

            cos = Math_CosS(this->dyna.actor.shape.rot.y) * rand;
            sin = Math_SinS(this->dyna.actor.shape.rot.y) * rand;

            this->dyna.actor.world.pos.x = this->shakeOrigin.x + cos;
            this->dyna.actor.world.pos.z = this->shakeOrigin.z + sin;

            this->textureBlend += 20;
            this->unk_258 = 1;
        }
    } else {
        if (this->textureBlend > 10) {
            this->textureBlend -= 10;
        } else {
            this->textureBlend = 0;
            this->dyna.actor.flags &= ~ACTOR_FLAG_0;

            if (this->dyna.bgId < 0) {
                this->unk_264 = 0;
            }

            this->dyna.actor.speedXZ += this->dyna.unk_150;
            this->shakeOrigin = this->dyna.actor.world.pos;
            this->dyna.actor.world.rot.y = this->dyna.unk_158;
            this->dyna.actor.speedXZ = CLAMP(this->dyna.actor.speedXZ, -2.5f, 2.5f);
            Math_SmoothStepToF(&this->dyna.actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);

            if (this->dyna.actor.speedXZ != 0.0f) {
                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
            }

            this->dyna.unk_154 = 0.0f;
            this->dyna.unk_150 = 0.0f;
        }
    }
}

/**
 * Spin toward the direction of the home position to start moving back to it.
 */
void EnAm_RotateToHome(EnAm* this, PlayState* play) {
    s16 yawToHome = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);

    if (this->skelAnime.curFrame == 8.0f) {
        Math_SmoothStepToS(&this->dyna.actor.world.rot.y, yawToHome, 1, 0x1F40, 0);
        this->dyna.actor.velocity.y = 12.0f;
    } else if (this->skelAnime.curFrame > 11.0f) {
        if (!(this->dyna.actor.bgCheckFlags & 1)) {
            this->skelAnime.curFrame = 11;
        } else {
            EnAm_SpawnEffects(this, play);

            if (this->dyna.actor.world.rot.y == yawToHome) {
                this->unk_258 = 0;
            }

            this->dyna.actor.velocity.y = 0.0f;
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        }
    }

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_258 == 0) {
        EnAm_SetupMoveToHome(this);
    }

    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
}

/**
 * After reaching the home position, spin back to the starting rotation.
 */
void EnAm_RotateToInit(EnAm* this, PlayState* play) {
    if (this->skelAnime.curFrame == 8.0f) {
        if ((this->dyna.actor.world.pos.x == this->dyna.actor.home.pos.x) &&
            (this->dyna.actor.world.pos.z == this->dyna.actor.home.pos.z)) {
            Math_SmoothStepToS(&this->dyna.actor.world.rot.y, this->dyna.actor.home.rot.y, 1, 0x1F40, 0);
        }
        this->unk_258 = 2;
        this->dyna.actor.velocity.y = 12.0f;
    } else if (this->skelAnime.curFrame > 11.0f) {
        if (!(this->dyna.actor.bgCheckFlags & 1)) {
            this->skelAnime.curFrame = 11;
        } else {
            this->unk_258 = 1;
            EnAm_SpawnEffects(this, play);

            if (this->dyna.actor.home.rot.y == this->dyna.actor.world.rot.y) {
                this->unk_258 = 0;
            }

            this->dyna.actor.velocity.y = 0.0f;
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        }
    }

    if (this->unk_258 == 2) {
        Math_SmoothStepToF(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x, 1.0f, 8.0f, 0.0f);
        Math_SmoothStepToF(&this->dyna.actor.world.pos.z, this->dyna.actor.home.pos.z, 1.0f, 8.0f, 0.0f);
    }

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_258 == 0) {
        EnAm_SetupSleep(this);
    }

    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
}

void EnAm_MoveToHome(EnAm* this, PlayState* play) {
    s16 yawToHome = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);

    if (this->skelAnime.curFrame == 8.0f) {
        this->dyna.actor.velocity.y = 12.0f;
        this->dyna.actor.speedXZ = 6.0f;
    } else if (this->skelAnime.curFrame > 11.0f) {
        if (!(this->dyna.actor.bgCheckFlags & 1)) {
            this->skelAnime.curFrame = 11;
        } else {
            Math_SmoothStepToS(&this->dyna.actor.world.rot.y, yawToHome, 1, 0xBB8, 0);

            if (this->dyna.actor.bgCheckFlags & 2) {
                this->unk_258--;
            }

            this->dyna.actor.velocity.y = 0.0f;
            this->dyna.actor.speedXZ = 0.0f;
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
            EnAm_SpawnEffects(this, play);

            if (Actor_WorldDistXYZToPoint(&this->dyna.actor, &this->dyna.actor.home.pos) < 80.0f) {
                EnAm_SetupRotateToInit(this);
            }
        }
    }

    // turn away from a wall if touching one
    if ((this->dyna.actor.speedXZ != 0.0f) && (this->dyna.actor.bgCheckFlags & 8)) {
        this->dyna.actor.world.rot.y = this->dyna.actor.wallYaw;
        Actor_MoveForward(&this->dyna.actor);
    }

    SkelAnime_Update(&this->skelAnime);

    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
}

void EnAm_RecoilFromDamage(EnAm* this, PlayState* play) {
    if (this->dyna.actor.speedXZ < 0.0f) {
        this->dyna.actor.speedXZ += 0.5f;
    }

    if ((this->dyna.actor.velocity.y <= 0.0f) && !EnAm_CanMove(this, play, -8.0f, this->dyna.actor.world.rot.y)) {
        this->dyna.actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        EnAm_SetupLunge(this);
        this->deathTimer = 64;
        this->panicSpinRot = 0;
    }
}

/**
 * After doing 3 lunges, wait for 2 seconds before attacking again.
 * Turn toward the player before lunging again.
 */
void EnAm_Cooldown(EnAm* this, PlayState* play) {
    s16 yawDiff = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.world.rot.y;

    yawDiff = ABS(yawDiff);

    if (this->cooldownTimer != 0) {
        this->cooldownTimer--;
    } else {
        if (this->skelAnime.curFrame == 8.0f) {
            Math_SmoothStepToS(&this->dyna.actor.world.rot.y, this->dyna.actor.yawTowardsPlayer, 1, 0x1F40, 0);
            this->dyna.actor.velocity.y = 12.0f;
        } else if (this->skelAnime.curFrame > 11.0f) {
            if (!(this->dyna.actor.bgCheckFlags & 1)) {
                this->skelAnime.curFrame = 11;
            } else {
                if (yawDiff < 3500) {
                    this->unk_258 = 0;
                }
                this->dyna.actor.velocity.y = 0.0f;
                this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
                EnAm_SpawnEffects(this, play);
            }
        }

        SkelAnime_Update(&this->skelAnime);

        if (this->unk_258 == 0) {
            EnAm_SetupLunge(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_AMOS_VOICE);
        }

        this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
    }
}

/**
 * Lunge toward the player in an attempt to deal damage. Hop 3 times.
 * Used for both normal attacks and the death sequence.
 */
void EnAm_Lunge(EnAm* this, PlayState* play) {
    if (this->deathTimer < 52) {
        if (this->skelAnime.curFrame == 8.0f) {
            this->dyna.actor.velocity.y = 12.0f;

            if (EnAm_CanMove(this, play, 80.0f, this->dyna.actor.world.rot.y)) {
                this->dyna.actor.speedXZ = 6.0f;
            } else {
                this->dyna.actor.speedXZ = 0.0f;
            }

            this->unk_264 = 1;
            this->hitCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        } else if (this->skelAnime.curFrame > 11.0f) {
            if (!(this->dyna.actor.bgCheckFlags & 1)) {
                this->skelAnime.curFrame = 11;
            } else {
                Math_SmoothStepToS(&this->dyna.actor.world.rot.y, this->dyna.actor.yawTowardsPlayer, 1, 0x1770, 0);

                if (this->dyna.actor.bgCheckFlags & 2) {
                    this->unk_258--;
                }

                this->dyna.actor.velocity.y = 0.0f;
                this->dyna.actor.speedXZ = 0.0f;
                this->unk_264 = 0;
                this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
                EnAm_SpawnEffects(this, play);

                if (((Actor_WorldDistXZToPoint(&this->dyna.actor, &this->dyna.actor.home.pos) > 240.0f) ||
                     (this->attackTimer == 0)) &&
                    (this->deathTimer == 0)) {
                    EnAm_SetupRotateToHome(this);
                }
            }
        }

        // turn and move away from a wall if contact is made with one
        if ((this->dyna.actor.speedXZ != 0.0f) && (this->dyna.actor.bgCheckFlags & 8)) {
            this->dyna.actor.world.rot.y =
                (this->dyna.actor.wallYaw - this->dyna.actor.world.rot.y) + this->dyna.actor.wallYaw;
            Actor_MoveForward(&this->dyna.actor);
            this->dyna.actor.bgCheckFlags &= ~8;
        }

        SkelAnime_Update(&this->skelAnime);

        if ((this->unk_258 == 0) && (this->deathTimer == 0)) {
            EnAm_SetupCooldown(this);
        }

        if (this->deathTimer == 0) {
            this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
        } else {
            if (this->panicSpinRot < 8000) {
                this->panicSpinRot += 800;
            }
            this->dyna.actor.shape.rot.y += this->panicSpinRot;
        }
    }
}

void EnAm_Statue(EnAm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp158f = this->dyna.unk_158;
    s16 moveDir = 0;
    s32 blockSpeed = CVar_GetS32("gFasterBlockPush", 0);

    if (this->unk_258 == 0) {
        if (this->dyna.unk_150 != 0.0f) {
            this->unk_258 = 0x8000 - (blockSpeed * 0x1000);
        }
    } else {
        this->unk_258 -= 0x800;
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);

        if (this->dyna.unk_150 < 0.0f) {
            temp158f = this->dyna.unk_158 + 0x8000;
        }

        if (this->hurtCollider.base.ocFlags1 & OC1_HIT) {
            moveDir = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &this->hurtCollider.base.oc->world.pos) - temp158f;
        }

        if ((this->dyna.unk_150 == 0.0f) || (this->unk_258 == 0) || !(this->dyna.actor.bgCheckFlags & 1) ||
            !func_800435D8(play, &this->dyna, 0x14,
                           (Math_SinS(this->unk_258) * (this->dyna.unk_150 * 0.5f)) + 40.0f, 0xA) ||
            ((this->hurtCollider.base.ocFlags1 & OC1_HIT) && (ABS(moveDir) <= 0x2000))) {

            this->unk_258 = 0;
            player->stateFlags2 &= ~0x151;
            player->actor.speedXZ = 0.0f;
            this->dyna.unk_150 = this->dyna.unk_154 = 0.0f;
        }

        this->dyna.actor.world.rot.y = this->dyna.unk_158;
        this->dyna.actor.speedXZ = Math_SinS(this->unk_258 * 8 / (8 - blockSpeed)) * (this->dyna.unk_150 * 0.5f) +
            (blockSpeed == 5 ? blockSpeed * (this->dyna.unk_150 * 0.5f) * 0.25681 : 
                (blockSpeed == 4 ? blockSpeed * (this->dyna.unk_150 * 0.5f) * 0.18305 :
                    (blockSpeed == 3 ? blockSpeed * (this->dyna.unk_150 * 0.5f) * 0.14222 :
                        (blockSpeed == 2 ? blockSpeed * (this->dyna.unk_150 * 0.5f) * 0.11625 :
                            (blockSpeed == 1 ? blockSpeed * (this->dyna.unk_150 * 0.5f) * 0.09828 : 0)))));
    }

    if (this->dyna.actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    }

    this->dyna.unk_150 = this->dyna.unk_154 = 0.0f;
}

void EnAm_SetupStunned(EnAm* this, PlayState* play) {
    // animation is set but SkelAnime_Update is not called in the action
    // likely copy pasted from EnAm_SetupRecoilFromDamage
    Animation_Change(&this->skelAnime, &gArmosDamagedAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gArmosDamagedAnim),
                     ANIMMODE_ONCE, 0.0f);

    this->dyna.actor.world.rot.y = this->dyna.actor.yawTowardsPlayer;

    if (EnAm_CanMove(this, play, -6.0f, this->dyna.actor.world.rot.y)) {
        this->dyna.actor.speedXZ = -6.0f;
    }

    Actor_SetColorFilter(&this->dyna.actor, 0, 120, 0, 100);

    if (this->damageEffect == AM_DMGEFF_ICE) {
        this->iceTimer = 48;
    }

    this->behavior = AM_BEHAVIOR_STUNNED;
    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnAm_SetupAction(this, EnAm_Stunned);
}

void EnAm_Stunned(EnAm* this, PlayState* play) {
    Math_SmoothStepToS(&this->dyna.actor.shape.rot.y, this->dyna.actor.world.rot.y, 1, 0xFA0, 0);

    if (this->dyna.actor.speedXZ < 0.0f) {
        this->dyna.actor.speedXZ += 0.5f;
    }

    if ((this->dyna.actor.velocity.y <= 0.0f) && !EnAm_CanMove(this, play, -9.0f, this->dyna.actor.world.rot.y)) {
        this->dyna.actor.speedXZ = 0.0f;
    }

    if (this->dyna.actor.colorFilterTimer == 0) {
        if (this->dyna.actor.colChkInfo.health != 0) {
            EnAm_SetupLunge(this);
        } else {
            EnAm_SetupRecoilFromDamage(this, play);
        }
    }
}

void EnAm_Ricochet(EnAm* this, PlayState* play) {
    if (this->dyna.actor.speedXZ < 0.0f) {
        this->dyna.actor.speedXZ += 0.5f;
    }

    if ((this->dyna.actor.velocity.y <= 0.0f) &&
        !EnAm_CanMove(this, play, this->dyna.actor.speedXZ * 1.5f, this->dyna.actor.world.rot.y)) {
        this->dyna.actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        this->dyna.actor.speedXZ = 0.0f;
        EnAm_SetupLunge(this);
    }
}

void EnAm_TransformSwordHitbox(Actor* thisx, PlayState* play) {
    static Vec3f D_809B0074 = { 2500.0f, 7000.0f, 0.0f };
    static Vec3f D_809B0080 = { -2500.0f, 0.0f, 0.0f };
    static Vec3f D_809B008C = { 2500.0f, 7000.0f, 4000.0f };
    static Vec3f D_809B0098 = { -2500.0f, 0.0f, 4000.0f };
    EnAm* this = (EnAm*)thisx;

    Matrix_MultVec3f(&D_809B0074, &this->hitCollider.dim.quad[1]);
    Matrix_MultVec3f(&D_809B0080, &this->hitCollider.dim.quad[0]);
    Matrix_MultVec3f(&D_809B008C, &this->hitCollider.dim.quad[3]);
    Matrix_MultVec3f(&D_809B0098, &this->hitCollider.dim.quad[2]);

    Collider_SetQuadVertices(&this->hitCollider, &this->hitCollider.dim.quad[0], &this->hitCollider.dim.quad[1],
                             &this->hitCollider.dim.quad[2], &this->hitCollider.dim.quad[3]);
}

void EnAm_UpdateDamage(EnAm* this, PlayState* play) {
    s32 pad;
    Vec3f sparkPos;

    if (this->deathTimer == 0) {
        if (this->blockCollider.base.acFlags & AC_BOUNCED) {
            this->blockCollider.base.acFlags &= ~(AC_HIT | AC_BOUNCED);
            this->hurtCollider.base.acFlags &= ~AC_HIT;

            if (this->behavior >= AM_BEHAVIOR_5) {
                EnAm_SetupRicochet(this, play);
            }
        } else if ((this->hurtCollider.base.acFlags & AC_HIT) && (this->behavior >= AM_BEHAVIOR_5)) {
            this->hurtCollider.base.acFlags &= ~AC_HIT;

            if (this->dyna.actor.colChkInfo.damageEffect != AM_DMGEFF_MAGIC_FIRE_LIGHT) {
                this->unk_264 = 0;
                this->damageEffect = this->dyna.actor.colChkInfo.damageEffect;
                Actor_SetDropFlag(&this->dyna.actor, &this->hurtCollider.info, 0);

                if ((this->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_NUT) ||
                    (this->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_STUN) ||
                    (this->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_ICE)) {
                    if (this->behavior != AM_BEHAVIOR_STUNNED) {
                        EnAm_SetupStunned(this, play);

                        if (this->dyna.actor.colChkInfo.damage != 0) {
                            this->dyna.actor.colChkInfo.health = 0;
                        }
                    } else if (this->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_STUN) {
                        sparkPos = this->dyna.actor.world.pos;
                        sparkPos.y += 50.0f;
                        CollisionCheck_SpawnShieldParticlesMetal(play, &sparkPos);
                    }
                } else if ((this->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_KILL) ||
                           (this->behavior == AM_BEHAVIOR_STUNNED)) {
                    this->dyna.actor.colChkInfo.health = 0;

                    EnAm_SetupRecoilFromDamage(this, play);
                } else {
                    EnAm_SetupRicochet(this, play);
                }
            }
        }
    }
}

void EnAm_Update(Actor* thisx, PlayState* play) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 dustPrimColor = { 150, 150, 150, 255 };
    static Color_RGBA8 dustEnvColor = { 100, 100, 100, 150 };
    s32 pad;
    EnAm* this = (EnAm*)thisx;
    EnBom* bomb;
    Vec3f dustPos;
    s32 i;
    f32 dustPosScale;
    s32 pad1;

    if (this->dyna.actor.params != ARMOS_STATUE) {
        EnAm_UpdateDamage(this, play);
    }

    if (this->dyna.actor.colChkInfo.damageEffect != AM_DMGEFF_MAGIC_FIRE_LIGHT) {
        if (this->attackTimer != 0) {
            this->attackTimer--;
        }

        this->actionFunc(this, play);

        if (this->deathTimer != 0) {
            this->deathTimer--;

            if (this->deathTimer == 0) {
                dustPosScale = play->gameplayFrames * 10;

                EnAm_SpawnEffects(this, play);
                bomb =
                    (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->dyna.actor.world.pos.x,
                                        this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0, 0, 2, BOMB_BODY);
                if (bomb != NULL) {
                    bomb->timer = 0;
                }

                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_AMOS_DEAD);
                Item_DropCollectibleRandom(play, &this->dyna.actor, &this->dyna.actor.world.pos, 0xA0);

                for (i = 9; i >= 0; i--) {
                    dustPos.x = (sinf(dustPosScale) * 7.0f) + this->dyna.actor.world.pos.x;
                    dustPos.y = (Rand_CenteredFloat(10.0f) * 6.0f) + (this->dyna.actor.world.pos.y + 40.0f);
                    dustPos.z = (cosf(dustPosScale) * 7.0f) + this->dyna.actor.world.pos.z;

                    func_8002836C(play, &dustPos, &zeroVec, &zeroVec, &dustPrimColor, &dustEnvColor, 200, 45, 12);
                    dustPosScale += 60.0f;
                }

                gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_ARMOS]++;

                Actor_Kill(&this->dyna.actor);
                return;
            }

            if ((this->deathTimer % 4) == 0) {
                Actor_SetColorFilter(&this->dyna.actor, 0x4000, 255, 0, 4);
            }
        }

        Actor_MoveForward(&this->dyna.actor);
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 20.0f, 28.0f, 80.0f, 0x1D);
    }

    Collider_UpdateCylinder(&this->dyna.actor, &this->hurtCollider);
    Collider_UpdateCylinder(&this->dyna.actor, &this->blockCollider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->hurtCollider.base);

    if (this->dyna.actor.params != ARMOS_STATUE) {
        Actor_SetFocus(&this->dyna.actor, this->dyna.actor.scale.x * 4500.0f);

        if (this->dyna.actor.colorFilterTimer == 0) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->blockCollider.base);
        }

        CollisionCheck_SetAC(play, &play->colChkCtx, &this->hurtCollider.base);

        if ((this->behavior >= 4) && (this->unk_264 > 0)) {
            if (!(this->hitCollider.base.atFlags & AT_BOUNCED)) {
                if (this->hitCollider.base.atFlags & AT_HIT) {
                    Player* player = GET_PLAYER(play);

                    if (this->hitCollider.base.at == &player->actor) {
                        Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                    }
                }
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->hitCollider.base);
            } else {
                this->hitCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
                this->hitCollider.base.at = NULL;
                EnAm_SetupRicochet(this, play);
            }
        }
    } else {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->blockCollider.base);
    }
}

static Vec3f sUnused1 = { 1100.0f, -700.0f, 0.0f };
static Vec3f sUnused2 = { 0.0f, 0.0f, 0.0f };

void EnAm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnAm* this = (EnAm*)thisx;

    if ((limbIndex == 1) && (this->unk_264 != 0)) {
        EnAm_TransformSwordHitbox(&this->dyna.actor, play);
    }
}

static Vec3f sIcePosOffsets[] = {
    { 20.0f, 40.0f, 0.0f },   { 10.0f, 60.0f, 10.0f },   { -10.0f, 60.0f, 10.0f }, { -20.0f, 40.0f, 0.0f },
    { 10.0f, 60.0f, -10.0f }, { -10.0f, 60.0f, -10.0f }, { 0.0f, 40.0f, -20.0f },  { 10.0f, 20.0f, 10.0f },
    { 10.0f, 20.0f, -10.0f }, { 0.0f, 40.0f, 20.0f },    { -10.0f, 20.0f, 10.0f }, { -10.0f, 20.0f, -10.0f },
};

void EnAm_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Vec3f sp68;
    EnAm* this = (EnAm*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->textureBlend);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, EnAm_PostLimbDraw, this);

    if (this->iceTimer != 0) {
        this->dyna.actor.colorFilterTimer++;
        this->iceTimer--;

        if ((this->iceTimer % 4) == 0) {
            s32 index;

            index = this->iceTimer >> 2;

            sp68.x = this->dyna.actor.world.pos.x + sIcePosOffsets[index].x;
            sp68.y = this->dyna.actor.world.pos.y + sIcePosOffsets[index].y;
            sp68.z = this->dyna.actor.world.pos.z + sIcePosOffsets[index].z;

            EffectSsEnIce_SpawnFlyingVec3f(play, &this->dyna.actor, &sp68, 150, 150, 150, 250, 235, 245, 255,
                                           1.4f);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

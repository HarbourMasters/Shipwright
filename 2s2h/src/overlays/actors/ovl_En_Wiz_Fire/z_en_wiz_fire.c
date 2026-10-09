/*
 * File: z_en_wiz_fire.c
 * Overlay: ovl_En_Wiz_Fire
 * Description: Wizrobe Fire/Ice Attack
 */

#include "z_en_wiz_fire.h"
#include "overlays/actors/ovl_En_Wiz/z_en_wiz.h"
#include "objects/object_wiz/object_wiz.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_LOCK_ON_DISABLED)

void EnWizFire_Init(Actor* thisx, PlayState* play);
void EnWizFire_Destroy(Actor* thisx, PlayState* play);
void EnWizFire_Update(Actor* thisx, PlayState* play2);
void EnWizFire_Draw(Actor* thisx, PlayState* play2);

void EnWiz_SetupMoveMagicProjectile(EnWizFire* this, PlayState* play);
void EnWiz_MoveMagicProjectile(EnWizFire* this, PlayState* play);
void EnWiz_SetupSmallFlame(EnWizFire* this, PlayState* play);
void EnWiz_SmallFlame(EnWizFire* this, PlayState* play);
void EnWiz_Pool(EnWizFire* this, PlayState* play);
void EnWiz_KillMagicProjectile(EnWizFire* this, PlayState* play);
void EnWizFire_DrawSmallFlame(Actor* thisx, PlayState* play);
void EnWizFire_InitializeEffect(EnWizFire* this, Vec3f* pos, Vec3f* accel);
void EnWizFire_UpdateEffects(EnWizFire* this, PlayState* play);
void EnWizFire_DrawEffects(EnWizFire* this, PlayState* play);

typedef enum {
    /* 0 */ EN_WIZ_FIRE_ACTION_MOVE_MAGIC_PROJECTILE,
    /* 1 */ EN_WIZ_FIRE_ACTION_SMALL_FLAME,
    /* 2 */ EN_WIZ_FIRE_ACTION_POOL,
    /* 3 */ EN_WIZ_FIRE_ACTION_KILL_MAGIC_PROJECTILE
} EnWizFireAction;

static s32 sPoolHitByIceArrow = false;

ActorProfile En_Wiz_Fire_Profile = {
    /**/ ACTOR_EN_WIZ_FIRE,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_WIZ,
    /**/ sizeof(EnWizFire),
    /**/ EnWizFire_Init,
    /**/ EnWizFire_Destroy,
    /**/ EnWizFire_Update,
    /**/ EnWizFire_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x09, 0x10 },
        { 0x01001202, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

void EnWizFire_Init(Actor* thisx, PlayState* play) {
    EnWizFire* this = (EnWizFire*)thisx;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->type = EN_WIZ_FIRE_GET_TYPE(&this->actor);
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->wallCheckTimer = 10;
    this->alpha = 255.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    if (!Player_HasMirrorShieldEquipped(play)) {
        this->collider.elem.atDmgInfo.dmgFlags = 0x20000000;
    }

    switch (this->type) {
        case EN_WIZ_FIRE_TYPE_ICE_MAGIC_PROJECTILE:
            this->isIceType = true;
            this->collider.elem.atDmgInfo.damage = 8;
            this->collider.elem.atDmgInfo.effect = 2;
            this->collider.elem.acDmgInfo.dmgFlags = (0x1000000 | 0x800 | 0x200 | 0x2);
            this->type = EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE;
            // fallthrough
        case EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE:
            if (this->type == EN_WIZ_FIRE_TYPE_ICE_MAGIC_PROJECTILE) {
                this->type = EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE;
                this->collider.elem.atDmgInfo.damage = 8;
            }
            // fallthrough
        case EN_WIZ_FIRE_TYPE_ARCING_MAGIC_PROJECTILE:
        case EN_WIZ_FIRE_TYPE_REFLECTED_MAGIC_PROJECTILE:
            this->actionFunc = EnWiz_SetupMoveMagicProjectile;
            break;

        case EN_WIZ_FIRE_TYPE_SMALL_FLAME:
            this->actor.draw = EnWizFire_DrawSmallFlame;
            this->smallFlameScroll = Rand_S16Offset(0, 10000);
            this->action = EN_WIZ_FIRE_ACTION_SMALL_FLAME;
            this->collider.elem.atDmgInfo.damage = 2;
            this->actionFunc = EnWiz_SetupSmallFlame;
            break;

        default:
            break;
    }
}

void EnWizFire_Destroy(Actor* thisx, PlayState* play) {
    EnWizFire* this = (EnWizFire*)thisx;

    if (this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) {
        play->envCtx.adjLightSettings.fogColor[2] = 0;
        play->envCtx.adjLightSettings.fogColor[1] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.fogColor[0] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.light2Color[2] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.light2Color[1] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.light2Color[0] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.light1Color[2] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.light1Color[1] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.light1Color[0] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.ambientColor[2] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.ambientColor[1] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.ambientColor[0] = play->envCtx.adjLightSettings.fogColor[2];
        play->envCtx.adjLightSettings.fogNear = play->envCtx.adjLightSettings.fogColor[2];
    }

    Collider_DestroyCylinder(play, &this->collider);
}

void EnWiz_SetupMoveMagicProjectile(EnWizFire* this, PlayState* play) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->magicProjectilePos); i++) {
        this->magicProjectilePos[i] = this->actor.world.pos;
    }

    this->lowestUsedIndex = 0;

    Matrix_Push();
    Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->actor.world.rot.x, MTXMODE_APPLY);

    if (this->type != EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) {
        velocity.z = Rand_CenteredFloat(2.0f) + 8.0f;
    } else {
        velocity.z = 12.0f;
    }

    Matrix_MultVec3f(&velocity, &this->actor.velocity);
    Matrix_Pop();

    this->actor.world.rot.x = this->actor.world.rot.y = this->actor.world.rot.z = 0;
    this->timer = 50;

    if (this->type != EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) {
        this->actor.velocity.y = 10.0f;
        this->actor.gravity = -1.0f;
        this->targetScale = 0.01f;
    } else {
        this->targetScale = 0.02f;
        this->timer = 100;
    }

    this->action = EN_WIZ_FIRE_ACTION_MOVE_MAGIC_PROJECTILE;
    this->actionFunc = EnWiz_MoveMagicProjectile;
}

/**
 * Moves all types of magic projectiles around, including arcing ones created by the
 * Fire Wizrobe's attack or ones reflected by the Mirror Shield.
 */
void EnWiz_MoveMagicProjectile(EnWizFire* this, PlayState* play) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };

    this->actor.world.rot.z += 5000;

    if (this->type != EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) {
        this->targetScale = 0.01f;
    } else {
        this->targetScale = 0.02f;
    }

    if ((this->timer == 0) && (this->scale < 0.001f)) {
        Math_Vec3f_Copy(&this->actor.velocity, &gZeroVec3f);
        this->action = EN_WIZ_FIRE_ACTION_KILL_MAGIC_PROJECTILE;
        this->increaseLowestUsedIndexTimer = 0;
        this->actionFunc = EnWiz_KillMagicProjectile;
        return;
    }

    if (this->timer == 0) {
        this->targetScale = 0.0f;
    }

    Math_ApproachF(&this->scale, this->targetScale, 0.2f, 0.01f);

    if (this->wallCheckTimer == 0) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) &&
            (this->timer != 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
            sPoolHitByIceArrow = false;
            this->timer = 0;
            this->targetScale = 0.0f;
        }
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->poolTimer == 0)) {
        s32 i;
        s16 arcingProjectileRotY;
        s32 pad;

        if (this->type == EN_WIZ_FIRE_TYPE_ARCING_MAGIC_PROJECTILE) {
            this->increaseLowestUsedIndexTimer = 10;

            Matrix_Push();
            Matrix_RotateYS(TRUNCF_BINANG(Rand_CenteredFloat(0x100)) + this->actor.world.rot.y, MTXMODE_NEW);
            velocity.z = Rand_CenteredFloat(2.0f) + 8.0f;
            Matrix_MultVec3f(&velocity, &this->actor.velocity);
            Matrix_Pop();

            this->actor.velocity.y = 6.0f;
            this->actor.gravity = -0.7f;

            if (!this->hasSpawnedSmallFlame) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WIZ_FIRE, this->actor.world.pos.x,
                            this->actor.world.pos.y - 10.0f, this->actor.world.pos.z, 0, 0, 0,
                            EN_WIZ_FIRE_TYPE_SMALL_FLAME);
                this->hasSpawnedSmallFlame = true;
            }

            this->timer = 0;
            this->scale = 0.0f;
            Math_Vec3f_Copy(&this->actor.velocity, &gZeroVec3f);
            this->action = EN_WIZ_FIRE_ACTION_KILL_MAGIC_PROJECTILE;
            this->increaseLowestUsedIndexTimer = 0;
            this->actionFunc = EnWiz_KillMagicProjectile;
            return;
        }

        if ((this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) && (this->timer != 0)) {
            if (this->actor.floorBgId == BGCHECK_SCENE) {
                this->poolTimer = 100;
                if (!this->isIceType) {
                    arcingProjectileRotY = 0;

                    for (i = 0; i < 5; i++) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WIZ_FIRE, this->actor.world.pos.x,
                                    this->actor.world.pos.y, this->actor.world.pos.z, 0, arcingProjectileRotY, 0,
                                    EN_WIZ_FIRE_TYPE_ARCING_MAGIC_PROJECTILE);
                        arcingProjectileRotY += BINANG_ADD((s32)Rand_CenteredFloat(0x1000), 0x10000 / 5);
                    }

                    Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
                    this->poolTimer = Rand_S16Offset(70, 30);
                    if (this->poolTimer != 0) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_EXP - SFX_FLAG);
                    }
                } else if (this->poolTimer != 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
                }

                Math_Vec3f_Copy(&this->actor.velocity, &gZeroVec3f);
                this->timer = 0;
                this->action = EN_WIZ_FIRE_ACTION_POOL;
                this->scale = 0.0f;
                this->actionFunc = EnWiz_Pool;
            }

            return;
        }
    }

    if ((this->type != EN_WIZ_FIRE_TYPE_REFLECTED_MAGIC_PROJECTILE) && (this->timer != 0)) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags == 0x1000) {
                this->timer = 0;
                this->hitByIceArrow = true;
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EV_ICE_MELT);
            }
        }

        if (Player_HasMirrorShieldEquipped(play) && (this->collider.base.atFlags & AT_BOUNCED)) {
            Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_REFLECT_MG);
            this->collider.base.atFlags &= ~(AT_TYPE_ENEMY | AT_BOUNCED | AT_HIT);
            this->collider.base.atFlags |= AT_TYPE_PLAYER;
            this->collider.elem.atDmgInfo.dmgFlags = 0x20;
            this->collider.elem.atDmgInfo.damage = 2;
            this->timer = 100;
            this->type = EN_WIZ_FIRE_TYPE_REFLECTED_MAGIC_PROJECTILE;
            this->actor.velocity.x *= -1.0f;
            this->actor.velocity.y *= -0.5f;
            this->actor.velocity.z *= -1.0f;

            if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_WIZ) &&
                (this->actor.parent->update != NULL)) {
                ((EnWiz*)this->actor.parent)->hasActiveProjectile = false;
            }
        }
    }
}

void EnWiz_SetupSmallFlame(EnWizFire* this, PlayState* play) {
    this->targetScale = 0.02f;
    this->timer = Rand_S16Offset(50, 50);
    this->scaleMod.x = Rand_CenteredFloat(1.0f) * 0.007f;
    this->scaleMod.y = Rand_CenteredFloat(1.0f) * 0.005f;
    this->scaleMod.z = Rand_CenteredFloat(1.0f) * 0.007f;
    this->actionFunc = EnWiz_SmallFlame;
}

/**
 * Manages the small flame that is created when the arcing projectiles from the Fire Wizrobe's
 * attack hit the floor.
 */
void EnWiz_SmallFlame(EnWizFire* this, PlayState* play) {
    if (this->timer > 10) {
        Math_ApproachF(&this->scale, this->targetScale, 0.3f, 0.01f);
    } else {
        Math_ApproachF(&this->scale, 2.0f * this->targetScale, 0.2f, 0.002f);
        Math_ApproachZeroF(&this->alpha, 1.0f, 35.0f);
        if ((this->timer == 0) && (this->alpha < 2.0f)) {
            Actor_Kill(&this->actor);
        }
        return;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        if (this->timer != 0) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->timer > 10) {
                this->timer -= 10;
            }

            if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags == 0x1000) {
                this->timer = 0;
                this->hitByIceArrow = true;
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EV_ICE_MELT);
            }
        }
    }

    if (this->timer != 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
    }
}

/**
 * Manages the pool of fire or ice that is created when a magic projectile hits the floor.
 */
void EnWiz_Pool(EnWizFire* this, PlayState* play) {
    s32 pad;

    if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_WIZ) &&
        (this->actor.parent->update != NULL) && (this->actor.parent->colChkInfo.health == 0)) {
        this->poolTimer = 0;
        this->playerHitByIceProjectile = true;
    }

    this->lowestUsedIndex++;

    if (this->lowestUsedIndex > 10) {
        this->lowestUsedIndex = 10;
    }

    if (this->poolTimer != 0) {
        Math_ApproachF(&this->blendScale, 60.0f, 0.5f, 10.0f);
        if (this->isIceType == true) {
            Vec3f accel = { 0.0f, 0.0f, 0.0f };
            Vec3f pos;

            accel.x = Rand_CenteredFloat(3.0f) / 10.0f;
            accel.y = 0.23f;
            accel.z = Rand_CenteredFloat(3.0f) / 10.0f;

            Math_Vec3f_Copy(&pos, &this->actor.world.pos);
            pos.x += Rand_CenteredFloat(150.0f);
            pos.z += Rand_CenteredFloat(150.0f);

            Math_ApproachF(&this->poolScale, 0.022f, 0.3f, 0.01f);
            this->collider.dim.radius = this->poolScale * 4300.0f;
            this->collider.dim.height = 30;
            this->collider.dim.yShift = 15;
            EnWizFire_InitializeEffect(this, &pos, &accel);
            Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
            return;
        }

        Math_ApproachF(&this->poolScale, 0.02f, 0.3f, 0.002f);
        Math_ApproachF(&this->fireSmokeScale, 0.02f, 0.3f, 0.002f);
        Math_ApproachF(&this->bigFlameScale, 0.02f, 0.3f, 0.2f);
        this->collider.dim.radius = this->poolScale * 4000.0f;
        this->collider.dim.height = this->bigFlameScale * 1850.0f;
        this->collider.dim.yShift = -15;

        if (this->collider.dim.height < 2) {
            this->collider.dim.height = 2;
        }

        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (!sPoolHitByIceArrow && (this->collider.elem.acHitElem->atDmgInfo.dmgFlags == 0x1000)) {
                sPoolHitByIceArrow = true;
                this->hitByIceArrow = true;
                this->poolTimer = 0;
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EV_ICE_MELT);
            }
        }

        this->actor.world.pos.y = this->actor.floorHeight + 10.0f;
        Actor_SetFocus(&this->actor, 0.0f);
        return;
    }

    Math_ApproachZeroF(&this->blendScale, 0.2f, 3.0f);

    if (this->isIceType == true) {
        Math_ApproachZeroF(&this->poolScale, 0.046f, 0.001f);
        Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);

        if (!this->shouldPoolFadeOut) {
            if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_WIZ) && (this->poolScale < 0.05f)) {
                EnWiz* wiz = (EnWiz*)this->actor.parent;

                this->collider.dim.yShift = -15;
                this->shouldPoolFadeOut = true;
                wiz->hasActiveProjectile = false;
            }
        }

        if (this->shouldPoolFadeOut && (this->poolScale < 0.05f)) {
            Math_ApproachZeroF(&this->alpha, 1.0f, 5.0f);
        }

        if ((this->poolScale < 0.001f) && (this->blendScaleFrac < 0.001f)) {
            sPoolHitByIceArrow = false;
            Actor_Kill(&this->actor);
        }

        return;
    }

    Math_ApproachZeroF(&this->bigFlameScale, 0.1f, 0.01f);

    if (this->bigFlameScale < 0.01f) {
        Math_ApproachZeroF(&this->alpha, 1.0f, 10.0f);
        if ((this->alpha < 10.0f) && (this->blendScaleFrac < 0.001f)) {
            sPoolHitByIceArrow = false;
            if ((this->actor.parent != NULL) && (this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) &&
                (this->actor.parent->id == ACTOR_EN_WIZ) && (this->actor.parent->update != NULL)) {
                EnWiz* wiz = (EnWiz*)this->actor.parent;

                wiz->hasActiveProjectile = false;
            }

            Actor_Kill(&this->actor);
        }
    }
}

void EnWiz_KillMagicProjectile(EnWizFire* this, PlayState* play) {
    if (this->increaseLowestUsedIndexTimer == 0) {
        this->increaseLowestUsedIndexTimer = 2;
        this->lowestUsedIndex++;
        if (this->lowestUsedIndex >= 6) {
            if ((this->actor.parent != NULL) && (this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) &&
                (this->actor.parent->id == ACTOR_EN_WIZ)) {
                EnWiz* wiz = (EnWiz*)this->actor.parent;

                sPoolHitByIceArrow = false;
                if (wiz->actor.update != NULL) {
                    wiz->hasActiveProjectile = false;
                }
            }

            Actor_Kill(&this->actor);
        }
    }
}

void EnWizFire_Update(Actor* thisx, PlayState* play2) {
    // These are AmbientColor, DiffuseColor1, DiffuseColor2, and fogColor
    static Color_RGB8 sLightSettingsColors[] = {
        { 100, 40, 40 }, { 180, 120, 80 },  { 155, 80, 80 },   { 125, 20, 0 },    // Fire
        { 0, 0, 0 },     { 200, 250, 250 }, { 100, 250, 250 }, { 225, 255, 235 }, // Ice
    };
    PlayState* play = play2;
    EnWizFire* this = (EnWizFire*)thisx;
    Player* player = GET_PLAYER(play);
    s32 j;
    s16 randomScale;
    s16 index;

    Actor_SetScale(&this->actor, this->scale);
    EnWizFire_UpdateEffects(this, play);
    this->blendScaleFrac = this->blendScale / 60.0f;

    if (this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) {
        Actor* wiz = this->actor.parent;

        if ((wiz != NULL) && (wiz->id == ACTOR_EN_WIZ) && (wiz->update != NULL) &&
            (((EnWiz*)wiz)->type != EN_WIZ_TYPE_FIRE_NO_BGM)) {
            f32 fogNear;

            index = this->isIceType * 4;
            fogNear = 970.0f;
            if (!this->isIceType) {
                fogNear = 968.0f;
            }

            play->envCtx.adjLightSettings.fogNear =
                TRUNCF_BINANG((fogNear - play->envCtx.lightSettings.fogNear) * this->blendScaleFrac);

            play->envCtx.adjLightSettings.ambientColor[0] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].r - play->envCtx.lightSettings.ambientColor[0]) *
                              this->blendScaleFrac);
            play->envCtx.adjLightSettings.ambientColor[1] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].g - play->envCtx.lightSettings.ambientColor[1]) *
                              this->blendScaleFrac);
            play->envCtx.adjLightSettings.ambientColor[2] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].b - play->envCtx.lightSettings.ambientColor[2]) *
                              this->blendScaleFrac);

            index++;
            play->envCtx.adjLightSettings.light1Color[0] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].r - play->envCtx.lightSettings.light1Color[0]) *
                              this->blendScaleFrac);
            play->envCtx.adjLightSettings.light1Color[1] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].g - play->envCtx.lightSettings.light1Color[1]) *
                              this->blendScaleFrac);
            play->envCtx.adjLightSettings.light1Color[2] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].b - play->envCtx.lightSettings.light1Color[2]) *
                              this->blendScaleFrac);

            index++;
            play->envCtx.adjLightSettings.light2Color[0] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].r - play->envCtx.lightSettings.light2Color[0]) *
                              this->blendScaleFrac);
            play->envCtx.adjLightSettings.light2Color[1] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].g - play->envCtx.lightSettings.light2Color[1]) *
                              this->blendScaleFrac);
            play->envCtx.adjLightSettings.light2Color[2] =
                TRUNCF_BINANG(((f32)sLightSettingsColors[index].b - play->envCtx.lightSettings.light2Color[2]) *
                              this->blendScaleFrac);

            index++;
            play->envCtx.adjLightSettings.fogColor[0] = TRUNCF_BINANG(
                ((f32)sLightSettingsColors[index].r - play->envCtx.lightSettings.fogColor[0]) * this->blendScaleFrac);
            play->envCtx.adjLightSettings.fogColor[1] = TRUNCF_BINANG(
                ((f32)sLightSettingsColors[index].g - play->envCtx.lightSettings.fogColor[1]) * this->blendScaleFrac);
            play->envCtx.adjLightSettings.fogColor[2] = TRUNCF_BINANG(
                ((f32)sLightSettingsColors[index].b - play->envCtx.lightSettings.fogColor[2]) * this->blendScaleFrac);
        }
    }

    this->smallFlameScroll++;

    this->actionFunc(this, play);

    this->actor.shape.yOffset = 10.0f;
    Actor_UpdatePos(&this->actor);

    this->magicProjectilePos[0] = this->actor.world.pos;

    for (j = ARRAY_COUNT(this->magicProjectilePos) - 2; j >= 0; j--) {
        this->magicProjectilePos[j + 1] = this->magicProjectilePos[j];
    }

    this->actor.velocity.y += this->actor.gravity;

    DECR(this->wallCheckTimer);
    DECR(this->timer);
    DECR(this->steamSpawnTimer);
    DECR(this->poolTimer);

    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 5.0f, 10.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);

    if ((this->hitByIceArrow || sPoolHitByIceArrow) && (this->steamSpawnTimer == 0)) {
        Vec3f accel;
        Vec3f velocity;
        Vec3f pos;
        f32 scaleStep;
        s32 i;

        accel.x = 0.0f;
        accel.y = 1.0f;
        accel.z = 0.0f;
        velocity.x = 0.0f;
        velocity.y = 1.0f;
        velocity.z = 0.0f;

        scaleStep = Rand_S16Offset(20, 10);
        if (this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) {
            scaleStep = Rand_S16Offset(40, 20);
        }

        this->steamSpawnTimer = Rand_S16Offset(2, 2);

        for (i = 0; i < 2; i++) {
            static Color_RGBA8 sSteamPrimColor = { 250, 250, 250, 255 };
            static Color_RGBA8 sSteamEnvColor = { 180, 180, 180, 255 };

            randomScale = Rand_S16Offset(20, 20);
            pos.x = ((f32)((Rand_ZeroOne() < 0.5f) ? -1 : 1) * randomScale) + this->actor.world.pos.x;
            pos.y = (Rand_ZeroOne() * 20.0f) + this->actor.floorHeight;
            randomScale = Rand_S16Offset(20, 20);
            pos.z = ((f32)((Rand_ZeroOne() < .5f) ? -1 : 1) * randomScale) + this->actor.world.pos.z;
            func_800B0DE0(play, &pos, &velocity, &accel, &sSteamPrimColor, &sSteamEnvColor, Rand_S16Offset(350, 100),
                          scaleStep);
        }
    }

    if (this->action < EN_WIZ_FIRE_ACTION_POOL) {
        this->collider.dim.radius = (this->scale * 15.0f) + 25.0f;
        this->collider.dim.height = (this->scale * 15.0f) + 25.0f;
        this->collider.dim.yShift = (this->scale * -0.75f) - 5.0f;
    }

    if (this->type == EN_WIZ_FIRE_TYPE_SMALL_FLAME) {
        this->collider.dim.radius = 10;
        this->collider.dim.height = this->scale * 5000.0f;
        this->collider.dim.yShift = 0;
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_LAUGH2);
            if (player->invincibilityTimer > 0) {
                player->invincibilityTimer += 40;
                if (this->isIceType) {
                    player->invincibilityTimer += 50;
                    this->playerHitByIceProjectile = true;
                }
            }
        }
    }

    if ((player->stateFlags2 & PLAYER_STATE2_4000) && (player->av2.actionVar2 < 90)) {
        player->av2.actionVar2 = 90;
    }

    if (!this->hitByIceArrow && !sPoolHitByIceArrow &&
        ((this->type != EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) || (this->alpha > 200.0f))) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        if (player->invincibilityTimer == 0) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void EnWizFire_DrawIcePool(EnWizFire* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) && (this->action == EN_WIZ_FIRE_ACTION_POOL)) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(this->poolScale, this->poolScale, this->poolScale, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 40, 30, 80);
        gSPDisplayList(POLY_XLU_DISP++, gWizrobeIcePoolDL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 215, 215, 215, (s8)this->alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, -play->state.frames & 0x7F, -play->state.frames & 0x7F, 32,
                                      64, 1, play->state.frames & 0xFF, play->state.frames & 0xFF, 16, 16, -1, -1, 1,
                                      1));

        Matrix_RotateYS(0, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gWizrobeIcePoolShineDL);
    }

    EnWizFire_DrawEffects(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnWizFire_DrawFirePoolAndFlame(EnWizFire* this, PlayState* play2) {
    PlayState* play = play2;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->type == EN_WIZ_FIRE_TYPE_MAGIC_PROJECTILE) && (this->action == EN_WIZ_FIRE_ACTION_POOL)) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        Matrix_Push();
        Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(this->poolScale, this->poolScale, this->poolScale, MTXMODE_APPLY);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, -play->state.frames % 128, 0, 0x20, 0x20, 1,
                                      (play->state.frames * 2) % 128, 0, 0x20, 0x20, -1, 0, 2, 0));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 40, 0, (s8)this->alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 245, 255, 128);
        gSPDisplayList(POLY_XLU_DISP++, gWizrobeFirePoolDL);

        Matrix_Pop();

        Matrix_Push();
        Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, play->state.frames % 128, (-play->state.frames * 6) % 256,
                                      0x20, 0x40, 1, (play->state.frames * 2) % 128, (-play->state.frames * 6) % 256,
                                      0x20, 0x40, 1, -6, 2, -6));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 80, 0, 0, (s8)this->alpha);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 100);

        Matrix_Scale(this->fireSmokeScale, this->fireSmokeScale, this->fireSmokeScale, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gWizrobeFireSmokeDL);

        Matrix_Pop();

        Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (-play->state.frames * 3) % 128, 0, 0x20, 0x20, 1, 0,
                                      (-play->state.frames * 10) % 256, 0x20, 0x40, -3, 0, 0, -10));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 50, 0, 255);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 235, 240, 128);

        Matrix_Scale(this->bigFlameScale, this->bigFlameScale, this->bigFlameScale, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gWizrobeBigFlameDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnWizFire_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnWizFire* this = (EnWizFire*)thisx;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_Push();

    for (i = ARRAY_COUNT(this->magicProjectilePos) - 1; i >= this->lowestUsedIndex; i--) {
        f32 scale = this->actor.scale.x - (i * -0.0019f);

        if (scale > 0.0f) {
            if (!this->isIceType) {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255 - (i * 25), 0, 255 - (i * 25));
                gDPSetEnvColor(POLY_XLU_DISP++, 255 - (i * 25), 0, 0, 0);
            } else {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255 - (i * 7), 255 - (i * 3), 255 - (i * 3), 255 - (i * 25));
                gDPSetEnvColor(POLY_XLU_DISP++, 220, 255, 235, 0);
            }

            Matrix_Translate(this->magicProjectilePos[i].x, this->magicProjectilePos[i].y + this->actor.shape.yOffset,
                             this->magicProjectilePos[i].z, MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_RotateZS(this->actor.world.rot.z, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gWizrobeMagicProjectileDL);
        }
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);

    if (!this->isIceType) {
        EnWizFire_DrawFirePoolAndFlame(this, play);
    } else {
        EnWizFire_DrawIcePool(this, play);
    }
}

void EnWizFire_DrawSmallFlame(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWizFire* this = (EnWizFire*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight + 20.0f, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(this->scale + this->scaleMod.x, this->scale + this->scaleMod.y, this->scale + this->scaleMod.z,
                 MTXMODE_APPLY);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 32, 1, this->smallFlameScroll & 0x7F,
                                  (-this->smallFlameScroll * 10) & 0x7F, 32, 32, 0, 0, 1, -10));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 100, 50, 0, (s8)this->alpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 235, 245, 255);

    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gWizrobeSmallFlameDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnWizFire_InitializeEffect(EnWizFire* this, Vec3f* pos, Vec3f* accel) {
    s16 i;
    EnWizFireEffect* effect = &this->effects[0];

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isEnabled) {
            effect->isEnabled = true;
            Math_Vec3f_Copy(&effect->velocity, &gZeroVec3f);
            effect->smokeScroll = Rand_ZeroFloat(100.0f);
            effect->pos = *pos;
            effect->accel = *accel;
            effect->scale = (Rand_ZeroFloat(5.0f) + 20.0f) * 0.001f;
            effect->shouldDecreaseAlpha = 0;
            effect->alpha = 0;
            break;
        }
    }
}

void EnWizFire_UpdateEffects(EnWizFire* this, PlayState* play) {
    s32 i;
    EnWizFireEffect* effect = &this->effects[0];

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            effect->smokeScroll++;

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if (!effect->shouldDecreaseAlpha) {
                effect->alpha += 10;
                if (effect->alpha >= 100) {
                    effect->shouldDecreaseAlpha = true;
                }
            } else {
                effect->alpha -= 8;
                if (effect->alpha <= 0) {
                    effect->alpha = 0;
                    effect->isEnabled = false;
                }
            }
        }
    }
}

void EnWizFire_DrawEffects(EnWizFire* this, PlayState* play) {
    s16 i;
    u8 materialFlag;
    EnWizFireEffect* effect = &this->effects[0];
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    materialFlag = false;
    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            if (!materialFlag) {
                gSPDisplayList(POLY_XLU_DISP++, gWizrobeIceSmokeMaterialDL);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, effect->alpha);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (effect->smokeScroll * 3) & 0x7F,
                                          (effect->smokeScroll * 0xF) & 0xFF, 32, 64, 1, 0, 0, 32, 32, 3, 0xF, 0, 0));

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
            gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BACK | G_FOG);
            gSPDisplayList(POLY_XLU_DISP++, gWizrobeIceSmokeModelDL);
        }
    }

    CLOSE_DISPS(gfxCtx);
}

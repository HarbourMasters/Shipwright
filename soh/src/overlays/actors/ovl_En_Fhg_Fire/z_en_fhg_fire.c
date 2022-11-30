/*
 * File: z_en_fhg_fire.c
 * Overlay: ovl_En_Fhg_Fire
 * Description: Phantom Ganon's Lighting Attack
 */

#include "z_en_fhg_fire.h"
#include "objects/object_fhg/object_fhg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
#include "overlays/actors/ovl_En_fHG/z_en_fhg.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /*  0 */ STRIKE_INIT,
    /* 10 */ STRIKE_BURST = 10,
    /* 11 */ STRIKE_TRAILS
} StrikeMode;

typedef enum {
    /* 0 */ TRAIL_INIT,
    /* 1 */ TRAIL_APPEAR,
    /* 2 */ TRAIL_DISSIPATE
} TrailMode;

typedef enum {
    /* 0 */ BALL_FIZZLE,
    /* 1 */ BALL_BURST,
    /* 2 */ BALL_IMPACT
} BallKillMode;

void EnFhgFire_Init(Actor* thisx, PlayState* play);
void EnFhgFire_Destroy(Actor* thisx, PlayState* play);
void EnFhgFire_Update(Actor* thisx, PlayState* play);
void EnFhgFire_Draw(Actor* thisx, PlayState* play);

void EnFhgFire_LightningStrike(EnFhgFire* this, PlayState* play);
void EnFhgFire_LightningTrail(EnFhgFire* this, PlayState* play);
void EnFhgFire_LightningShock(EnFhgFire* this, PlayState* play);
void EnFhgFire_LightningBurst(EnFhgFire* this, PlayState* play);
void EnFhgFire_SpearLight(EnFhgFire* this, PlayState* play);
void EnFhgFire_EnergyBall(EnFhgFire* this, PlayState* play);
void EnFhgFire_PhantomWarp(EnFhgFire* this, PlayState* play);

const ActorInit En_Fhg_Fire_InitVars = {
    0,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_FHG,
    sizeof(EnFhgFire),
    (ActorFunc)EnFhgFire_Init,
    (ActorFunc)EnFhgFire_Destroy,
    (ActorFunc)EnFhgFire_Update,
    (ActorFunc)EnFhgFire_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK6,
        { 0x00100700, 0x03, 0x20 },
        { 0x0D900700, 0x00, 0x00 },
        TOUCH_ON,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 30, 10, { 0, 0, 0 } },
};

void EnFhgFire_SetUpdate(EnFhgFire* this, EnFhgFireUpdateFunc updateFunc) {
    this->updateFunc = updateFunc;
}

void EnFhgFire_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFhgFire* this = (EnFhgFire*)thisx;
    Player* player = GET_PLAYER(play);

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    if ((this->actor.params == FHGFIRE_LIGHTNING_SHOCK) || (this->actor.params == FHGFIRE_LIGHTNING_BURST) ||
        (this->actor.params == FHGFIRE_ENERGY_BALL)) {
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    }
    this->fwork[FHGFIRE_ALPHA] = 200.0f;
    Actor_SetScale(&this->actor, 0.0f);

    if (this->actor.params == FHGFIRE_LIGHTNING_STRIKE) {
        EnFhgFire_SetUpdate(this, EnFhgFire_LightningStrike);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_THUNDER);
    } else if (this->actor.params >= FHGFIRE_LIGHTNING_TRAIL) {
        EnFhgFire_SetUpdate(this, EnFhgFire_LightningTrail);
        this->actor.shape.rot = this->actor.world.rot;
    }
    if (this->actor.params == FHGFIRE_LIGHTNING_SHOCK) {
        this->actor.draw = NULL;
        EnFhgFire_SetUpdate(this, EnFhgFire_LightningShock);
        this->actor.speedXZ = 30.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_SPARK);
    } else if (this->actor.params == FHGFIRE_LIGHTNING_BURST) {
        EnFhgFire_SetUpdate(this, EnFhgFire_LightningBurst);
        this->fwork[FHGFIRE_ALPHA] = 255.0f;
        this->work[FHGFIRE_TIMER] = 32;
        this->work[FHGFIRE_FX_TIMER] = 50;
        this->lensFlareTimer = 10;

        this->fwork[FHGFIRE_BURST_SCALE] = this->actor.world.rot.x / 100.0f;
        this->collider.dim.radius = this->actor.world.rot.x * 0.13f;
        this->collider.dim.height = this->actor.world.rot.x * 0.13f;
        this->collider.dim.yShift = 0;
    } else if (this->actor.params == FHGFIRE_SPEAR_LIGHT) {
        osSyncPrintf("yari hikari ct 1\n"); // "light spear"
        EnFhgFire_SetUpdate(this, EnFhgFire_SpearLight);
        osSyncPrintf("yari hikari ct 2\n");
        this->work[FHGFIRE_TIMER] = this->actor.world.rot.x;
        this->work[FHGFIRE_FIRE_MODE] = this->actor.world.rot.y;
    } else if ((this->actor.params == FHGFIRE_WARP_EMERGE) || (this->actor.params == FHGFIRE_WARP_RETREAT) ||
               (this->actor.params == FHGFIRE_WARP_DEATH)) {
        Actor_SetScale(&this->actor, 7.0f);
        EnFhgFire_SetUpdate(this, EnFhgFire_PhantomWarp);
        if (this->actor.params == FHGFIRE_WARP_DEATH) {
            this->work[FHGFIRE_TIMER] = 440;
            this->actor.scale.z = 1.0f;
        } else {
            this->work[FHGFIRE_TIMER] = 76;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_FANTOM_WARP_S);
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_FANTOM_WARP_S2);
        }
    } else if (this->actor.params == FHGFIRE_ENERGY_BALL) {
        f32 dxL;
        f32 dyL;
        f32 dzL;
        f32 dxzL;

        this->actor.speedXZ = (this->actor.world.rot.x == 0) ? 8.0f : 3.0f;
        EnFhgFire_SetUpdate(this, EnFhgFire_EnergyBall);

        this->work[FHGFIRE_TIMER] = 70;
        this->work[FHGFIRE_FX_TIMER] = 2;

        dxL = player->actor.world.pos.x - this->actor.world.pos.x;
        dyL = player->actor.world.pos.y + 30.0f - this->actor.world.pos.y;
        dzL = player->actor.world.pos.z - this->actor.world.pos.z;
        this->actor.world.rot.y = Math_FAtan2F(dxL, dzL) * (0x8000 / M_PI);
        dxzL = sqrtf(SQ(dxL) + SQ(dzL));
        this->actor.world.rot.x = Math_FAtan2F(dyL, dxzL) * (0x8000 / M_PI);
        this->collider.dim.radius = 40;
        this->collider.dim.height = 50;
        this->collider.dim.yShift = -25;
        this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 255, 255, 255);
    }
}

void EnFhgFire_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFhgFire* this = (EnFhgFire*)thisx;

    if ((this->actor.params == FHGFIRE_LIGHTNING_SHOCK) || (this->actor.params == FHGFIRE_LIGHTNING_BURST) ||
        (this->actor.params == FHGFIRE_ENERGY_BALL)) {
        Collider_DestroyCylinder(play, &this->collider);
    }

    if (this->actor.params == FHGFIRE_ENERGY_BALL) {
        LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    }
}

void EnFhgFire_LightningStrike(EnFhgFire* this, PlayState* play) {
    Camera* camera = Play_GetCamera(play, 0);
    s16 i;

    switch (this->work[FHGFIRE_FIRE_MODE]) {
        case STRIKE_INIT:
            this->work[FHGFIRE_FIRE_MODE] = STRIKE_BURST;
            this->work[FHGFIRE_TIMER] = 7;
            break;
        case STRIKE_BURST:
            this->actor.shape.rot.y =
                Camera_GetInputDirYaw(camera) + 0x8000 * (this->work[FHGFIRE_VARIANCE_TIMER] & 0xFF);
            Math_ApproachF(&this->fwork[FHGFIRE_SCALE], 1.0f, 1.0f, 0.2f);

            if (this->work[FHGFIRE_TIMER] == 0) {
                this->work[FHGFIRE_FIRE_MODE] = STRIKE_TRAILS;
                this->actor.shape.rot.z += 0x8000;
                this->work[FHGFIRE_TIMER] = 37;
                this->actor.world.pos.y -= 200.0f;

                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE,
                                   this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 500, 0, 0,
                                   FHGFIRE_LIGHTNING_BURST);
                {
                    Vec3f sp7C;
                    Vec3f sp70 = { 0.0f, -1.0f, 0.0f };

                    for (i = 0; i < 35; i++) {
                        sp7C.x = Rand_CenteredFloat(30.f);
                        sp7C.y = Rand_ZeroFloat(5.0f) + 3.0f;
                        sp7C.z = Rand_CenteredFloat(30.f);
                        sp70.y = -0.2f;
                        EffectSsFhgFlash_SpawnLightBall(play, &this->actor.world.pos, &sp7C, &sp70,
                                                        (s16)(Rand_ZeroOne() * 100.0f) + 240, FHGFLASH_LIGHTBALL_GREEN);
                    }
                }
                func_80033E88(&this->actor, play, 4, 10);
            }

            break;
        case STRIKE_TRAILS:
            this->actor.shape.rot.y =
                Camera_GetInputDirYaw(camera) + (this->work[FHGFIRE_VARIANCE_TIMER] & 0xFF) * 0x8000;

            Math_ApproachF(&this->fwork[FHGFIRE_SCALE], 0.0f, 1.0f, 0.2f);
            if (this->work[FHGFIRE_TIMER] == 30) {
                s16 randY = (Rand_ZeroOne() < 0.5f) ? 0x1000 : 0;

                for (i = 0; i < 8; i++) {
                    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE,
                                       this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0,
                                       (i * 0x2000) + randY, 0x4000, FHGFIRE_LIGHTNING_TRAIL + i);
                }

                for (i = 0; i < 8; i++) {
                    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE,
                                       this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0,
                                       (i * 0x2000) + randY, 0, FHGFIRE_LIGHTNING_SHOCK);
                }
            }

            if (this->work[FHGFIRE_TIMER] == 0) {
                Actor_Kill(&this->actor);
            }
    }

    Actor_SetScale(&this->actor, this->fwork[FHGFIRE_SCALE]);
}

void EnFhgFire_LightningTrail(EnFhgFire* this, PlayState* play) {
    osSyncPrintf("FF MOVE 1\n");
    this->actor.shape.rot.x += (s16)(Rand_ZeroOne() * 4000.0f) + 0x4000;

    switch (this->work[FHGFIRE_FIRE_MODE]) {
        case TRAIL_INIT:
            this->work[FHGFIRE_FIRE_MODE] = TRAIL_APPEAR;
            this->work[FHGFIRE_TIMER] = (s16)(Rand_ZeroOne() * 7.0f) + 7;
        case TRAIL_APPEAR:
            Math_ApproachF(&this->fwork[FHGFIRE_SCALE], 1.7f, 1.0f, 0.34f);

            if (this->work[FHGFIRE_TIMER] == 0) {
                this->work[FHGFIRE_FIRE_MODE] = TRAIL_DISSIPATE;
                this->work[FHGFIRE_TIMER] = 10;
                this->actor.world.pos.z += Math_SinS(this->actor.shape.rot.y) * -200.0f * this->fwork[FHGFIRE_SCALE];
                this->actor.world.pos.x += Math_CosS(this->actor.shape.rot.y) * 200.0f * this->fwork[FHGFIRE_SCALE];
                this->actor.shape.rot.y += 0x8000;
            }
            break;
        case TRAIL_DISSIPATE:
            Math_ApproachZeroF(&this->fwork[FHGFIRE_SCALE], 1.0f, 0.34f);
            if (this->work[FHGFIRE_TIMER] == 0) {
                Actor_Kill(&this->actor);
            }
            break;
    }

    Actor_SetScale(&this->actor, this->fwork[FHGFIRE_SCALE]);
    if (this->actor.scale.x > 1.0f) {
        this->actor.scale.x = 1.0f;
    }

    osSyncPrintf("FF MOVE 2\n");
}

void EnFhgFire_LightningShock(EnFhgFire* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f pos;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
    }

    if (Rand_ZeroOne() < 0.5f) {
        pos = this->actor.world.pos;
        pos.y -= 20.0f;
        EffectSsFhgFlash_SpawnShock(play, &this->actor, &pos, 200, FHGFLASH_SHOCK_NO_ACTOR);
    }

    Actor_MoveForward(&this->actor);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    if (player->invincibilityTimer == 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f, 1);
    if (this->actor.bgCheckFlags & 8) {
        Actor_Kill(&this->actor);
    }
}

void EnFhgFire_LightningBurst(EnFhgFire* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    play->envCtx.fillScreen = true;
    this->actor.shape.rot.y += 0x1000;

    if (this->work[FHGFIRE_FX_TIMER] == 49) {
        play->envCtx.unk_BF = 1;
        play->envCtx.unk_D6 = 0xFF;
    }
    if (this->work[FHGFIRE_FX_TIMER] == 31) {
        play->envCtx.unk_BF = 0x00;
        play->envCtx.unk_D6 = 0x14;
    }
    if (this->work[FHGFIRE_FX_TIMER] >= 48) {
        play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] =
            play->envCtx.screenFillColor[2] = 255;

        if ((this->work[FHGFIRE_TIMER] % 2) != 0) {
            play->envCtx.screenFillColor[3] = 70;
        } else {
            play->envCtx.screenFillColor[3] = 0;
        }
    } else {
        play->envCtx.screenFillColor[3] = 0;
    }

    if (this->work[FHGFIRE_TIMER] <= 20) {
        Math_ApproachZeroF(&this->fwork[FHGFIRE_ALPHA], 1.0f, 45.0f);
        Math_ApproachZeroF(&this->fwork[FHGFIRE_SCALE], 1.0f, 0.5f);
    } else {
        Math_ApproachF(&this->fwork[FHGFIRE_SCALE], this->fwork[FHGFIRE_BURST_SCALE], 0.5f, 3.0f);
    }

    Actor_SetScale(&this->actor, this->fwork[FHGFIRE_SCALE]);
    if (this->fwork[FHGFIRE_BURST_SCALE] > 3.0f) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        if (player->invincibilityTimer == 0) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
    }

    if (this->work[FHGFIRE_TIMER] == 0) {
        Actor_Kill(&this->actor);
        play->envCtx.fillScreen = false;
    }

    if (this->lensFlareTimer != 0) {
        this->lensFlareTimer--;
        this->lensFlareOn = true;
        Math_ApproachF(&this->lensFlareScale, 40.0f, 0.3f, 10.0f);
    } else {
        Math_ApproachZeroF(&this->lensFlareScale, 1.0f, 5.0f);
        if (this->lensFlareScale == 0.0f) {
            this->lensFlareOn = false;
        }
    }

    gCustomLensFlareOn = this->lensFlareOn;
    gCustomLensFlarePos = this->actor.world.pos;
    gLensFlareScale = this->lensFlareScale;
    gLensFlareColorIntensity = 10.0f;
    gLensFlareScreenFillAlpha = 0;
}

void EnFhgFire_SpearLight(EnFhgFire* this, PlayState* play) {
    BossGanondrof* bossGnd;
    s16 i;

    osSyncPrintf("yari hikari 1\n");
    bossGnd = (BossGanondrof*)this->actor.parent;
    if ((this->work[FHGFIRE_VARIANCE_TIMER] % 2) != 0) {
        Actor_SetScale(&this->actor, 6.0f);
    } else {
        Actor_SetScale(&this->actor, 5.25f);
    }

    this->actor.world.pos = bossGnd->spearTip;
    this->actor.shape.rot.z += (s16)(Rand_ZeroOne() * 0x4E20) + 0x4000;

    osSyncPrintf("yari hikari 2\n");
    if (this->work[FHGFIRE_FIRE_MODE] == FHGFIRE_LIGHT_GREEN) {
        Vec3f ballPos;
        Vec3f ballVel = { 0.0f, 0.0f, 0.0f };
        Vec3f ballAccel = { 0.0f, 0.0f, 0.0f };

        osSyncPrintf("FLASH !!\n");

        for (i = 0; i < 2; i++) {
            ballPos.x = Rand_CenteredFloat(20.0f) + this->actor.world.pos.x;
            ballPos.y = Rand_CenteredFloat(20.0f) + this->actor.world.pos.y;
            ballPos.z = Rand_CenteredFloat(20.0f) + this->actor.world.pos.z;
            ballAccel.y = -0.08f;

            EffectSsFhgFlash_SpawnLightBall(play, &ballPos, &ballVel, &ballAccel,
                                            (s16)(Rand_ZeroOne() * 80.0f) + 150, FHGFLASH_LIGHTBALL_GREEN);
        }
    }

    if (this->work[FHGFIRE_TIMER] == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnFhgFire_EnergyBall(EnFhgFire* this, PlayState* play) {
    f32 dxL;
    f32 dyL;
    f32 dzL;
    f32 dxzL;
    f32 dxPG;
    f32 dyPG;
    f32 dzPG;
    u8 killMode = BALL_FIZZLE;
    u8 canBottleReflect1;
    Player* player = GET_PLAYER(play);

    if (this->work[FHGFIRE_KILL_TIMER] != 0) {
        this->work[FHGFIRE_KILL_TIMER]--;
        if (this->work[FHGFIRE_KILL_TIMER] == 0) {
            Actor_Kill(&this->actor);
            return;
        }
    } else {
        s32 canBottleReflect2;
        BossGanondrof* bossGnd = (BossGanondrof*)this->actor.parent;

        dxPG = bossGnd->targetPos.x - this->actor.world.pos.x;
        dyPG = bossGnd->targetPos.y - this->actor.world.pos.y;
        dzPG = bossGnd->targetPos.z - this->actor.world.pos.z;
        dxL = player->actor.world.pos.x - this->actor.world.pos.x;
        dyL = player->actor.world.pos.y + 40.0f - this->actor.world.pos.y;
        dzL = player->actor.world.pos.z - this->actor.world.pos.z;
        func_8002D908(&this->actor);
        func_8002D7EC(&this->actor);
        if (this->work[FHGFIRE_VARIANCE_TIMER] & 1) {
            Actor_SetScale(&this->actor, 6.0f);
        } else {
            Actor_SetScale(&this->actor, 5.25f);
        }
        this->actor.shape.rot.z += (s16)(Rand_ZeroOne() * 0x4E20) + 0x4000;
        {
            u8 lightBallColor1 = FHGFLASH_LIGHTBALL_GREEN;
            s16 i1;
            Vec3f spD4;
            Vec3f spC8 = { 0.0f, 0.0f, 0.0f };
            Vec3f spBC = { 0.0f, 0.0f, 0.0f };

            if (this->work[FHGFIRE_FIRE_MODE] >= FHGFIRE_LIGHT_BLUE) {
                lightBallColor1 = FHGFLASH_LIGHTBALL_LIGHTBLUE;
            }
            for (i1 = 0; i1 < 3; i1++) {
                spD4.x = Rand_CenteredFloat(20.0f) + this->actor.world.pos.x;
                spD4.y = Rand_CenteredFloat(20.0f) + this->actor.world.pos.y;
                spD4.z = Rand_CenteredFloat(20.0f) + this->actor.world.pos.z;
                spBC.y = -0.08f;
                EffectSsFhgFlash_SpawnLightBall(play, &spD4, &spC8, &spBC, (s16)(Rand_ZeroOne() * 80.0f) + 150,
                                                lightBallColor1);
            }
        }
        switch (this->work[FHGFIRE_FIRE_MODE]) {
            case FHGFIRE_LIGHT_GREEN:
                canBottleReflect1 =
                    ((player->stateFlags1 & PLAYER_STATE1_SWINGING_BOTTLE) &&
                     (ABS((s16)(player->actor.shape.rot.y - (s16)(bossGnd->actor.yawTowardsPlayer + 0x8000))) <
                      0x2000) &&
                     (sqrtf(SQ(dxL) + SQ(dyL) + SQ(dzL)) <= 25.0f))
                        ? true
                        : false;
                if ((this->collider.base.acFlags & AC_HIT) || canBottleReflect1) {
                    ColliderInfo* hurtbox = this->collider.info.acHitInfo;
                    s16 i2;
                    Vec3f spA8;
                    Vec3f sp9C = { 0.0f, -0.5f, 0.0f };
                    s16 angleModX;
                    s16 angleModY;

                    for (i2 = 0; i2 < 30; i2++) {
                        spA8.x = Rand_CenteredFloat(20.0f);
                        spA8.y = Rand_CenteredFloat(20.0f);
                        spA8.z = Rand_CenteredFloat(20.0f);
                        EffectSsFhgFlash_SpawnLightBall(play, &this->actor.world.pos, &spA8, &sp9C,
                                                        (s16)(Rand_ZeroOne() * 25.0f) + 50, FHGFLASH_LIGHTBALL_GREEN);
                    }
                    canBottleReflect2 = canBottleReflect1;
                    if (!canBottleReflect2 && (hurtbox->toucher.dmgFlags & 0x00100000)) {
                        killMode = BALL_IMPACT;
                        Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_REFLECT_MG, &player->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_800AA000(this->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
                    } else {
                        if (bossGnd->flyMode == GND_FLY_NEUTRAL) {
                            angleModX = Rand_CenteredFloat(0x2000);
                            angleModY = Rand_CenteredFloat(0x2000);
                            this->actor.speedXZ = 15.0f;
                        } else {
                            angleModX = 0;
                            angleModY = 0;
                            this->work[FHGFIRE_RETURN_COUNT]++;
                            if ((this->work[FHGFIRE_RETURN_COUNT] > 3) && (Rand_ZeroOne() < 0.5f)) {
                                this->work[FHGFIRE_RETURN_COUNT] = 100;
                            }

                            if (!canBottleReflect2 && (player->meleeWeaponAnimation >= 24)) {
                                this->actor.speedXZ = 20.0f;
                                this->work[FHGFIRE_RETURN_COUNT] = 4;
                            } else {
                                this->actor.speedXZ += 1.0f;
                            }
                        }
                        this->actor.world.rot.y = (s16)(Math_FAtan2F(dxPG, dzPG) * (0x8000 / M_PI)) + angleModY;
                        this->actor.world.rot.x =
                            (s16)(Math_FAtan2F(dyPG, sqrtf((dxPG * dxPG) + (dzPG * dzPG))) * (0x8000 / M_PI)) +
                            angleModX;
                        this->work[FHGFIRE_FIRE_MODE] = FHGFIRE_LIGHT_BLUE;
                        this->work[FHGFIRE_FX_TIMER] = 2;
                        Audio_PlaySoundGeneral(NA_SE_IT_SWORD_REFLECT_MG, &player->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_800AA000(this->actor.xyzDistToPlayerSq, 0xB4, 0x14, 0x64);
                    }
                } else if (sqrtf(SQ(dxL) + SQ(dyL) + SQ(dzL)) <= 25.0f) {
                    killMode = BALL_BURST;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
                    if ((bossGnd->flyMode >= GND_FLY_VOLLEY) && (this->work[FHGFIRE_RETURN_COUNT] >= 2)) {
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_LAUGH);
                    }
                    func_8002F698(play, &this->actor, 3.0f, this->actor.world.rot.y, 0.0f, 3, 0x10);
                }
                break;
            case FHGFIRE_LIGHT_BLUE:
                if ((bossGnd->flyMode == GND_FLY_RETURN) && (this->work[FHGFIRE_RETURN_COUNT] < 100)) {
                    this->actor.world.rot.y = Math_FAtan2F(dxPG, dzPG) * (0x8000 / M_PI);
                    if ((sqrtf(SQ(dxPG) + SQ(dzPG)) < (150.0f + (this->actor.speedXZ * 8.0f)))) {
                        this->work[FHGFIRE_FIRE_MODE] = FHGFIRE_LIGHT_REFLECT;
                        bossGnd->returnSuccess = true;
                        this->work[FHGFIRE_TIMER] = 8;
                    }
                } else {
                    if (this->work[FHGFIRE_RETURN_COUNT] >= 100) {
                        if ((sqrtf(SQ(dxPG) + SQ(dyPG) + SQ(dzPG)) < 100.0f)) {
                            bossGnd->returnSuccess = true;
                        }
                        this->actor.world.rot.y = Math_FAtan2F(dxPG, dzPG) * (0x8000 / M_PI);
                        this->actor.world.rot.x = Math_FAtan2F(dyPG, sqrtf(SQ(dxPG) + SQ(dzPG))) * (0x8000 / M_PI);
                    }
                    if ((fabsf(dxPG) < 30.0f) && (fabsf(dzPG) < 30.0f) && (fabsf(dyPG) < 45.0f)) {
                        killMode = BALL_IMPACT;
                        bossGnd->returnCount = this->work[FHGFIRE_RETURN_COUNT] + 1;
                        Audio_PlaySoundGeneral(NA_SE_EN_FANTOM_HIT_THUNDER, &bossGnd->actor.projectedPos, 4,
                                               &D_801333E0, &D_801333E0, &D_801333E8);
                        Audio_PlaySoundGeneral(NA_SE_EN_FANTOM_DAMAGE, &bossGnd->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                    }
                }
                break;
            case FHGFIRE_LIGHT_REFLECT:
                if (this->work[FHGFIRE_TIMER] == 0) {
                    s16 i3;
                    Vec3f sp88;
                    Vec3f sp7C = { 0.0f, -0.5f, 0.0f };

                    for (i3 = 0; i3 < 30; i3++) {
                        sp88.x = Rand_CenteredFloat(20.0f);
                        sp88.y = Rand_CenteredFloat(20.0f);
                        sp88.z = Rand_CenteredFloat(20.0f);
                        EffectSsFhgFlash_SpawnLightBall(play, &this->actor.world.pos, &sp88, &sp7C,
                                                        (s16)(Rand_ZeroOne() * 40.0f) + 80, FHGFLASH_LIGHTBALL_GREEN);
                    }
                    this->actor.world.rot.y = Math_FAtan2F(dxL, dzL) * (0x8000 / M_PI);
                    dxzL = sqrtf(SQ(dxL) + SQ(dzL));
                    this->actor.world.rot.x = Math_FAtan2F(dyL, dxzL) * (0x8000 / M_PI);
                    this->work[FHGFIRE_FIRE_MODE] = FHGFIRE_LIGHT_GREEN;
                    Audio_PlayActorSound2(&this->actor, NA_SE_IT_SWORD_REFLECT_MG);
                    this->actor.speedXZ += 2.0f;
                }
                break;
        }

        osSyncPrintf("F_FIRE_MODE %d\n", this->work[FHGFIRE_FIRE_MODE]);
        osSyncPrintf("fly_mode    %d\n", bossGnd->flyMode);
        if (this->work[FHGFIRE_FX_TIMER] == 0) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f, 7);
            if ((this->actor.bgCheckFlags & 0x19) || killMode) {
                u8 lightBallColor2 = FHGFLASH_LIGHTBALL_GREEN;
                s16 i4;
                Vec3f sp6C;
                Vec3f sp60 = { 0.0f, -1.0f, 0.0f };

                if (this->work[FHGFIRE_FIRE_MODE] > FHGFIRE_LIGHT_GREEN) {
                    lightBallColor2 = FHGFLASH_LIGHTBALL_LIGHTBLUE;
                }
                for (i4 = 0; i4 < 30; i4++) {
                    sp6C.x = Rand_CenteredFloat(20.0f);
                    sp6C.y = Rand_CenteredFloat(20.0f);
                    sp6C.z = Rand_CenteredFloat(20.0f);
                    sp60.y = -0.1f;
                    EffectSsFhgFlash_SpawnLightBall(play, &this->actor.world.pos, &sp6C, &sp60,
                                                    (s16)(Rand_ZeroOne() * 50.0f) + 100, lightBallColor2);
                }
                if (killMode == BALL_BURST) {
                    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE,
                                       this->actor.world.pos.x, player->actor.world.pos.y + 20.0f,
                                       this->actor.world.pos.z, 0xC8, 0, 0, FHGFIRE_LIGHTNING_BURST);
                }
                bossGnd->flyMode = GND_FLY_NEUTRAL;
                this->work[FHGFIRE_KILL_TIMER] = 30;
                this->actor.draw = NULL;
                if (killMode == BALL_FIZZLE) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_THUNDER_GND);
                }
                return;
            } else {
                Collider_UpdateCylinder(&this->actor, &this->collider);
                osSyncPrintf("BEFORE setAC   %d\n", this->collider.base.shape);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
                osSyncPrintf("AFTER  setAC\n");
            }
        }
        Lights_PointNoGlowSetInfo(&this->lightInfo, (s16)this->actor.world.pos.x, (s16)this->actor.world.pos.y,
                                  (s16)this->actor.world.pos.z, 255, 255, 255, 200);
        if (this->actor.speedXZ > 20.0f) {
            this->actor.speedXZ = 20.0f;
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_FIRE - SFX_FLAG);
        // "Why ah ah ah ah"
        osSyncPrintf("なぜだああああああああ      %d\n", this->work[FHGFIRE_VARIANCE_TIMER]);
    }
}

void EnFhgFire_PhantomWarp(EnFhgFire* this, PlayState* play) {
    EnfHG* horse = (EnfHG*)this->actor.parent;
    f32 scrollDirection;

    this->fwork[FHGFIRE_WARP_TEX_1_X] += 25.0f * this->fwork[FHGFIRE_WARP_TEX_SPEED];
    this->fwork[FHGFIRE_WARP_TEX_1_Y] -= 40.0f * this->fwork[FHGFIRE_WARP_TEX_SPEED];
    this->fwork[FHGFIRE_WARP_TEX_2_X] += 5.0f * this->fwork[FHGFIRE_WARP_TEX_SPEED];
    this->fwork[FHGFIRE_WARP_TEX_2_Y] -= 30.0f * this->fwork[FHGFIRE_WARP_TEX_SPEED];

    if (this->actor.params == FHGFIRE_WARP_DEATH) {
        if (this->work[FHGFIRE_TIMER] > 70) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_FANTOM_WARP_L - SFX_FLAG);
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_FANTOM_WARP_L2 - SFX_FLAG);
        }

        if (this->work[FHGFIRE_TIMER] == 70) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_FANTOM_WARP_S);
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_FANTOM_WARP_S2);
        }
    }

    if (this->work[FHGFIRE_TIMER] > 50) {
        scrollDirection = 1.0f;
        if (this->actor.params > FHGFIRE_WARP_EMERGE) {
            scrollDirection = -1.0f;
        }
        Math_ApproachF(&this->fwork[FHGFIRE_WARP_TEX_SPEED], scrollDirection, 1.0f, 0.04f);
        Math_ApproachF(&this->fwork[FHGFIRE_WARP_ALPHA], 255.0f, 1.0f, 10.2f);
    } else if (this->work[FHGFIRE_TIMER] <= 25) {
        Math_ApproachZeroF(&this->fwork[FHGFIRE_WARP_TEX_SPEED], 1.0f, 0.04f);
        Math_ApproachZeroF(&this->fwork[FHGFIRE_WARP_ALPHA], 1.0f, 10.2f);
    }

    osSyncPrintf("EFC 1\n");
    if ((this->work[FHGFIRE_TIMER] == 0) || ((this->actor.params == FHGFIRE_WARP_EMERGE) && horse->fhgFireKillWarp)) {
        Actor_Kill(&this->actor);
    }
    osSyncPrintf("EFC 2\n");
}

void EnFhgFire_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFhgFire* this = (EnFhgFire*)thisx;

    this->work[FHGFIRE_VARIANCE_TIMER]++;

    if (this->work[FHGFIRE_TIMER] != 0) {
        this->work[FHGFIRE_TIMER]--;
    }
    if (this->work[FHGFIRE_FX_TIMER] != 0) {
        this->work[FHGFIRE_FX_TIMER]--;
    }

    this->updateFunc(this, play);
}

static void* sDustTextures[] = {
    gDust1Tex, gDust2Tex, gDust3Tex, gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex,
};

void EnFhgFire_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFhgFire* this = (EnFhgFire*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actor.params == FHGFIRE_LIGHTNING_BURST) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->fwork[FHGFIRE_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 165, 255, 75, 0);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gPhantomLightningBlastDL));
    } else if ((this->actor.params == FHGFIRE_SPEAR_LIGHT) || (this->actor.params == FHGFIRE_ENERGY_BALL)) {
        osSyncPrintf("yari hikari draw 1\n");
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->fwork[FHGFIRE_ALPHA]);

        if (this->work[FHGFIRE_FIRE_MODE] > FHGFIRE_LIGHT_GREEN) {
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 0);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 165, 255, 75, 0);
        }
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_RotateZ((this->actor.shape.rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPhantomEnergyBallDL);
    } else if ((this->actor.params == FHGFIRE_WARP_EMERGE) || (this->actor.params == FHGFIRE_WARP_RETREAT) ||
               (this->actor.params == FHGFIRE_WARP_DEATH)) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (u8)this->fwork[FHGFIRE_WARP_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 90, 50, 95, (s8)(this->fwork[FHGFIRE_WARP_ALPHA] * 0.5f));
        gDPPipeSync(POLY_XLU_DISP++);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, (s16)this->fwork[FHGFIRE_WARP_TEX_1_X],
                                    (s16)this->fwork[FHGFIRE_WARP_TEX_1_Y], 0x40, 0x40, 1,
                                    (s16)this->fwork[FHGFIRE_WARP_TEX_2_X], (s16)this->fwork[FHGFIRE_WARP_TEX_2_Y],
                                    0x40, 0x40));
        gSPDisplayList(POLY_XLU_DISP++, gPhantomWarpDL);
    } else {
        osSyncPrintf("FF DRAW 1\n");
        Matrix_Translate(0.0f, -100.0f, 0.0f, MTXMODE_APPLY);
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->fwork[FHGFIRE_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 30, 0);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPhantomLightningDL);
        osSyncPrintf("FF DRAW 2\n");
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/*
 * File: z_en_bom.c
 * Overlay: ovl_En_Bom
 * Description: Bomb
 */

#include "z_en_bom.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include <stdlib.h>

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void EnBom_Init(Actor* thisx, PlayState* play);
void EnBom_Destroy(Actor* thisx, PlayState* play);
void EnBom_Update(Actor* thisx, PlayState* play);
void EnBom_Draw(Actor* thisx, PlayState* play);

void EnBom_Move(EnBom* this, PlayState* play);
void EnBom_WaitForRelease(EnBom* this, PlayState* play);

const ActorInit En_Bom_InitVars = {
    ACTOR_EN_BOM,
    ACTORCAT_EXPLOSIVE,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnBom),
    (ActorFunc)EnBom_Init,
    (ActorFunc)EnBom_Destroy,
    (ActorFunc)EnBom_Update,
    (ActorFunc)EnBom_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x0003F828, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 6, 11, 14, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000008, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 0 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_STOP),
};

void EnBom_SetupAction(EnBom* this, EnBomActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnBom_Init(Actor* thisx, PlayState* play) {
    EnBom* this = (EnBom*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisx->shape, 700.0f, ActorShadow_DrawCircle, 16.0f);
    thisx->colChkInfo.mass = 200;
    thisx->colChkInfo.cylRadius = 5;
    thisx->colChkInfo.cylHeight = 10;

    if (!GameInteractor_GetRandomBombFuseTimerActive()) {
        this->timer = 70;
    } else {
        // Set random fuse timer with a minimum of 10. Do the sound and scale immediately,
        // otherwise the bomb is invisible until the timer hits the "normal" amount.
        uint32_t randomTimer = (rand() % 150) + 10;
        this->timer = randomTimer;
        Audio_PlayActorSound2(thisx, NA_SE_PL_TAKE_OUT_SHIELD);
        Actor_SetScale(thisx, 0.01f);
    }

    if (CVarGetFloat("gBombTimerMultiplier", 1.0f) != 1.0f) {
        this->timer = (s32)(70 * CVarGetFloat("gBombTimerMultiplier", 1.0f));
        // Do the sound and scale immediately if GameInteractor hasn't already.
        if (!GameInteractor_GetRandomBombFuseTimerActive()) {
            Audio_PlayActorSound2(thisx, NA_SE_PL_TAKE_OUT_SHIELD);
            Actor_SetScale(thisx, 0.01f);
        }
    }

    this->flashSpeedScale = 7;
    Collider_InitCylinder(play, &this->bombCollider);
    Collider_InitJntSph(play, &this->explosionCollider);
    Collider_SetCylinder(play, &this->bombCollider, thisx, &sCylinderInit);
    Collider_SetJntSph(play, &this->explosionCollider, thisx, &sJntSphInit, &this->explosionColliderItems[0]);
    this->explosionColliderItems[0].info.toucher.damage += (thisx->shape.rot.z & 0xFF00) >> 8;
    if (CVarGetInteger("gNutsExplodeBombs", 0)) {
        this->bombCollider.info.bumper.dmgFlags |= 1;
    }

    thisx->shape.rot.z &= 0xFF;
    if (thisx->shape.rot.z & 0x80) {
        thisx->shape.rot.z |= 0xFF00;
    }

    EnBom_SetupAction(this, EnBom_Move);
}

void EnBom_Destroy(Actor* thisx, PlayState* play) {
    EnBom* this = (EnBom*)thisx;

    Collider_DestroyJntSph(play, &this->explosionCollider);
    Collider_DestroyCylinder(play, &this->bombCollider);
}

void EnBom_Move(EnBom* this, PlayState* play) {
    // if bomb has a parent actor, the bomb hasnt been released yet
    if (Actor_HasParent(&this->actor, play)) {
        EnBom_SetupAction(this, EnBom_WaitForRelease);
        this->actor.room = -1;
        return;
    }

    if ((this->actor.velocity.y > 0.0f) && (this->actor.bgCheckFlags & 0x10)) {
        this->actor.velocity.y = -this->actor.velocity.y;
    }

    // rebound bomb off the wall it hits
    if ((this->actor.speedXZ != 0.0f) && (this->actor.bgCheckFlags & 8)) {
        if (ABS((s16)(this->actor.wallYaw - this->actor.world.rot.y)) > 0x4000) {
            this->actor.world.rot.y = ((this->actor.wallYaw - this->actor.world.rot.y) + this->actor.wallYaw) - 0x8000;
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_BOMB_BOUND);
        Actor_MoveForward(&this->actor);
        this->actor.speedXZ *= 0.7f;
        this->actor.bgCheckFlags &= ~8;
    }

    if (!(this->actor.bgCheckFlags & 1)) {
        Math_StepToF(&this->actor.speedXZ, 0.0f, 0.08f);
    } else {
        Math_StepToF(&this->actor.speedXZ, 0.0f, 1.0f);
        if ((this->actor.bgCheckFlags & 2) && (this->actor.velocity.y < -3.0f)) {
            func_8002F850(play, &this->actor);
            this->actor.velocity.y *= -0.3f;
            this->actor.bgCheckFlags &= ~2;
        } else if (this->timer >= 4) {
            func_8002F580(&this->actor, play);
        }
    }

    Actor_MoveForward(&this->actor);
}

void EnBom_WaitForRelease(EnBom* this, PlayState* play) {
    // if parent is NULL bomb has been released
    if (Actor_HasNoParent(&this->actor, play)) {
        EnBom_SetupAction(this, EnBom_Move);
        EnBom_Move(this, play);
    }
}

void EnBom_Explode(EnBom* this, PlayState* play) {
    Player* player;

    if (this->explosionCollider.elements[0].dim.modelSphere.radius == 0) {
        this->actor.flags |= ACTOR_FLAG_DRAW_WHILE_CULLED;
        func_800AA000(this->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
    }

    if (CVarGetInteger("gStaticExplosionRadius", 0)) {
        //72 is the maximum radius of an OoT bomb explosion
        this->explosionCollider.elements[0].dim.worldSphere.radius = 72;
    } else {
        this->explosionCollider.elements[0].dim.worldSphere.radius += this->actor.shape.rot.z + 8;
    }
        

    if (this->actor.params == BOMB_EXPLOSION) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->explosionCollider.base);
    }

    if (play->envCtx.adjLight1Color[0] != 0) {
        play->envCtx.adjLight1Color[0] -= 25;
    }

    if (play->envCtx.adjLight1Color[1] != 0) {
        play->envCtx.adjLight1Color[1] -= 25;
    }

    if (play->envCtx.adjLight1Color[2] != 0) {
        play->envCtx.adjLight1Color[2] -= 25;
    }

    if (play->envCtx.adjAmbientColor[0] != 0) {
        play->envCtx.adjAmbientColor[0] -= 25;
    }

    if (play->envCtx.adjAmbientColor[1] != 0) {
        play->envCtx.adjAmbientColor[1] -= 25;
    }

    if (play->envCtx.adjAmbientColor[2] != 0) {
        play->envCtx.adjAmbientColor[2] -= 25;
    }

    if (this->timer == 0) {
        player = GET_PLAYER(play);

        if ((player->stateFlags1 & 0x800) && (player->heldActor == &this->actor)) {
            player->actor.child = NULL;
            player->heldActor = NULL;
            player->interactRangeActor = NULL;
            player->stateFlags1 &= ~0x800;
        }

        Actor_Kill(&this->actor);
    }
}

void EnBom_Update(Actor* thisx, PlayState* play2) {
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f bomb2Accel = { 0.0f, 0.1f, 0.0f };
    Vec3f effAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f dustAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 dustColor = { 255, 255, 255, 255 };
    s32 pad;
    PlayState* play = play2;
    EnBom* this = (EnBom*)thisx;

    thisx->gravity = -1.2f;

    if (this->timer != 0) {
        this->timer--;
    }

    // With random bomb fuse timer or gBombTimerMultiplier, sound effect and scaling is already done on init.
    if (this->timer == 67 && !GameInteractor_GetRandomBombFuseTimerActive() && CVarGetFloat("gBombTimerMultiplier", 1.0f) == 1.0f) {
        Audio_PlayActorSound2(thisx, NA_SE_PL_TAKE_OUT_SHIELD);
        Actor_SetScale(thisx, 0.01f);
    }

    if ((thisx->xzDistToPlayer >= 20.0f) || (ABS(thisx->yDistToPlayer) >= 80.0f)) {
        this->bumpOn = true;
    }

    this->actionFunc(this, play);

    Actor_UpdateBgCheckInfo(play, thisx, 5.0f, 10.0f, 15.0f, 0x1F);

    if (thisx->params == BOMB_BODY) {
        float timerMultiplier = CVarGetFloat("gBombTimerMultiplier", 1.0f);
        if (this->timer < (timerMultiplier == 1.0f ? 63 : (s32)(70 * timerMultiplier - 7))) {
            dustAccel.y = 0.2f;

            // spawn spark effect on even frames
            effPos = thisx->world.pos;
            effPos.y += 17.0f;
            if ((play->gameplayFrames % 2) == 0) {
                EffectSsGSpk_SpawnFuse(play, thisx, &effPos, &effVelocity, &effAccel);
            }

            Audio_PlayActorSound2(thisx, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);

            effPos.y += 3.0f;
            func_8002829C(play, &effPos, &effVelocity, &dustAccel, &dustColor, &dustColor, 50, 5);
        }

        if ((this->bombCollider.base.acFlags & AC_HIT) || ((this->bombCollider.base.ocFlags1 & OC1_HIT) &&
                                                           (this->bombCollider.base.oc->category == ACTORCAT_ENEMY))) {
            this->timer = 0;
            thisx->shape.rot.z = 0;
        } else {
            // if a lit stick touches the bomb, set timer to 100
            // these bombs never have a timer over 70, so this isnt used
            if ((this->timer > 100) && Player_IsBurningStickInRange(play, &thisx->world.pos, 30.0f, 50.0f)) {
                this->timer = 100;
            }
        }

        dustAccel.y = 0.2f;
        effPos = thisx->world.pos;
        effPos.y += 10.0f;

        // double bomb flash speed and adjust red color at certain times during the countdown
        if ((this->timer == 3) || (this->timer == 20) || (this->timer == 40)) {
            thisx->shape.rot.z = 0;
            this->flashSpeedScale >>= 1;
        }

        if ((this->timer < 100) && ((this->timer & (this->flashSpeedScale + 1)) != 0)) {
            Math_SmoothStepToF(&this->flashIntensity, 140.0f, 1.0f, 140.0f / this->flashSpeedScale, 0.0f);
        } else {
            Math_SmoothStepToF(&this->flashIntensity, 0.0f, 1.0f, 140.0f / this->flashSpeedScale, 0.0f);
        }

        if (this->timer < 3) {
            Actor_SetScale(thisx, thisx->scale.x + 0.002f);
        }

        if (this->timer == 0) {
            effPos = thisx->world.pos;

            effPos.y += 10.0f;
            if (Actor_HasParent(thisx, play)) {
                effPos.y += 30.0f;
            }

            EffectSsBomb2_SpawnLayered(play, &effPos, &effVelocity, &bomb2Accel, 100,
                                       (thisx->shape.rot.z * 6) + 19);

            effPos.y = thisx->floorHeight;
            if (thisx->floorHeight > BGCHECK_Y_MIN) {
                EffectSsBlast_SpawnWhiteShockwave(play, &effPos, &effVelocity, &effAccel);
            }

            Audio_PlayActorSound2(thisx, NA_SE_IT_BOMB_EXPLOSION);

            play->envCtx.adjLight1Color[0] = play->envCtx.adjLight1Color[1] =
                play->envCtx.adjLight1Color[2] = 250;

            play->envCtx.adjAmbientColor[0] = play->envCtx.adjAmbientColor[1] =
                play->envCtx.adjAmbientColor[2] = 250;

            Camera_AddQuake(&play->mainCamera, 2, 0xB, 8);
            thisx->params = BOMB_EXPLOSION;
            this->timer = 10;
            thisx->flags |= ACTOR_FLAG_DRAW_WHILE_CULLED;
            EnBom_SetupAction(this, EnBom_Explode);
        }
    }

    Actor_SetFocus(thisx, 20.0f);

    if (thisx->params <= BOMB_BODY) {
        Collider_UpdateCylinder(thisx, &this->bombCollider);

        // if link is not holding the bomb anymore and bump conditions are met, subscribe to OC
        if (!Actor_HasParent(thisx, play) && this->bumpOn) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->bombCollider.base);
        }

        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bombCollider.base);
    }

    if ((thisx->scale.x >= 0.01f) && (thisx->params != BOMB_EXPLOSION)) {
        if (thisx->yDistToWater >= 20.0f) {
            EffectSsDeadSound_SpawnStationary(play, &thisx->projectedPos, NA_SE_IT_BOMB_UNEXPLOSION, true,
                                              DEADSOUND_REPEAT_MODE_OFF, 10);
            Actor_Kill(thisx);
            return;
        }
        if (thisx->bgCheckFlags & 0x40) {
            thisx->bgCheckFlags &= ~0x40;
            Audio_PlayActorSound2(thisx, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

void EnBom_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBom* this = (EnBom*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (thisx->params == BOMB_BODY) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        func_8002EBCC(thisx, play, 0);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gBombCapDL);
        Matrix_RotateZYX(0x4000, 0, 0, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->flashIntensity, 0, 40, 255);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, (s16)this->flashIntensity, 0, 40, 255);
        gSPDisplayList(POLY_OPA_DISP++, gBombBodyDL);
        Collider_UpdateSpheres(0, &this->explosionCollider);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

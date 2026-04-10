/*
 * File: z_en_syateki_niw.c
 * Overlay: ovl_En_Syateki_Niw
 * Description: Hopping Cucco
 */

#include "z_en_syateki_niw.h"
#include "objects/object_niw/object_niw.h"
#include "vt.h"
#include "soh/frame_interpolation.h"
#include "soh/ResourceManagerHelpers.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void EnSyatekiNiw_Init(Actor* thisx, PlayState* play);
void EnSyatekiNiw_Destroy(Actor* thisx, PlayState* play);
void EnSyatekiNiw_Update(Actor* thisx, PlayState* play);
void EnSyatekiNiw_Draw(Actor* thisx, PlayState* play);

void func_80B11DEC(EnSyatekiNiw* this, PlayState* play);
void func_80B132A8(EnSyatekiNiw* this, PlayState* play);
void func_80B129EC(EnSyatekiNiw* this, PlayState* play);
void func_80B13464(EnSyatekiNiw* this, PlayState* play);
void func_80B123A8(EnSyatekiNiw* this, PlayState* play);
void func_80B11E78(EnSyatekiNiw* this, PlayState* play);
void func_80B12460(EnSyatekiNiw* this, PlayState* play);
void func_80B128D8(EnSyatekiNiw* this, PlayState* play);

void func_80B131B8(EnSyatekiNiw* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4);

const ActorInit En_Syateki_Niw_InitVars = {
    ACTOR_EN_SYATEKI_NIW,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_NIW,
    sizeof(EnSyatekiNiw),
    (ActorFunc)EnSyatekiNiw_Init,
    (ActorFunc)EnSyatekiNiw_Destroy,
    (ActorFunc)EnSyatekiNiw_Update,
    (ActorFunc)EnSyatekiNiw_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 10, 20, 4, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 1, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void EnSyatekiNiw_Init(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gCuccoSkel, &gCuccoAnim, this->jointTable, this->morphTable, 16);

    this->minigameType = this->actor.params;
    if (this->minigameType < 0) {
        this->minigameType = 0;
    }

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    if (this->minigameType == 0) {
        osSyncPrintf("\n\n");
        // "Archery range chicken"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 射的場鶏 ☆☆☆☆☆ \n" VT_RST);
        Actor_SetScale(&this->actor, 0.01f);
    } else {
        osSyncPrintf("\n\n");
        // "Bomb chicken"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ ボムにわ！ ☆☆☆☆☆ \n" VT_RST);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        Actor_SetScale(&this->actor, 0.01f);
    }

    this->initPos = this->actor.world.pos;
    this->targetPos = this->actor.world.pos;
    this->actionFunc = func_80B11DEC;
}

void EnSyatekiNiw_Destroy(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void func_80B11A94(EnSyatekiNiw* this, PlayState* play, s16 arg2) {
    if (this->peckTimer == 0) {
        if (arg2 == 0) {
            this->headRotXTarget = 0.0f;
        } else {
            this->headRotXTarget = -10000.0f;
        }

        this->headRotXState += 1;
        this->peckTimer = 3;
        if (!(this->headRotXState & 1)) {
            this->headRotXTarget = 0.0f;
            if (arg2 == 0) {
                this->peckTimer = Rand_ZeroFloat(30.0f);
            }
        }
    }

    if (this->flapTimer == 0) {
        this->wingsRotState++;
        this->wingsRotState &= 1;
        switch (arg2) {
            case 0:
                this->leftWingRotXTarget = 0.0f;
                this->rightWingRotXTarget = 0.0f;
                break;

            case 1:
                this->flapTimer = 3;
                this->leftWingRotXTarget = 7000.0f;
                this->rightWingRotXTarget = 7000.0f;
                if (this->wingsRotState == 0) {
                    this->leftWingRotXTarget = 0.0f;
                    this->rightWingRotXTarget = 0.0f;
                }
                break;

            case 2:
                this->flapTimer = 2;
                this->rightWingRotXTarget = this->leftWingRotXTarget = -10000.0f;
                this->leftWingRotYTarget = this->rightWingRotYTarget = 25000.0f;
                this->leftWingRotZTarget = this->rightWingRotZTarget = 6000.0f;
                if (this->wingsRotState == 0) {
                    this->rightWingRotYTarget = 8000.0f;
                    this->leftWingRotYTarget = 8000.0f;
                }
                break;

            case 3:
                this->flapTimer = 2;
                this->rightWingRotYTarget = 10000.0f;
                this->leftWingRotYTarget = 10000.0f;
                if (this->wingsRotState == 0) {
                    this->rightWingRotYTarget = 3000.0f;
                    this->leftWingRotYTarget = 3000.0f;
                }
                break;

            case 4:
                this->peckTimer = this->timer1 = 5;
                break;

            case 5:
                this->flapTimer = 5;
                this->rightWingRotYTarget = 14000.0f;
                this->leftWingRotYTarget = 14000.0f;
                if (this->wingsRotState == 0) {
                    this->rightWingRotYTarget = 10000.0f;
                    this->leftWingRotYTarget = 10000.0f;
                }
                break;
        }
    }

    if (this->headRotXTarget != this->headRot.x) {
        Math_ApproachF(&this->headRot.x, this->headRotXTarget, 0.5f, 4000.0f);
    }

    if (this->leftWingRotXTarget != this->leftWingRot.x) {
        Math_ApproachF(&this->leftWingRot.x, this->leftWingRotXTarget, 0.8f, 7000.0f);
    }

    if (this->leftWingRotYTarget != this->leftWingRot.y) {
        Math_ApproachF(&this->leftWingRot.y, this->leftWingRotYTarget, 0.8f, 7000.0f);
    }

    if (this->leftWingRotZTarget != this->leftWingRot.z) {
        Math_ApproachF(&this->leftWingRot.z, this->leftWingRotZTarget, 0.8f, 7000.0f);
    }

    if (this->rightWingRotXTarget != this->rightWingRot.x) {
        Math_ApproachF(&this->rightWingRot.x, this->rightWingRotXTarget, 0.8f, 7000.0f);
    }

    if (this->rightWingRotYTarget != this->rightWingRot.y) {
        Math_ApproachF(&this->rightWingRot.y, this->rightWingRotYTarget, 0.8f, 7000.0f);
    }

    if (this->rightWingRotZTarget != this->rightWingRot.z) {
        Math_ApproachF(&this->rightWingRot.z, this->rightWingRotZTarget, 0.8f, 7000.0f);
    }
}

void func_80B11DEC(EnSyatekiNiw* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    if (this->minigameType != 0) {
        Actor_SetScale(&this->actor, this->scale);
    }

    this->actionFunc = func_80B11E78;
}

void func_80B11E78(EnSyatekiNiw* this, PlayState* play) {
    Vec3f dustVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, 0.2f, 0.0f };
    Color_RGBA8 dustPrimColor = { 0, 0, 0, 255 };
    Color_RGBA8 dustEnvColor = { 0, 0, 0, 255 };
    Vec3f dustPos;
    f32 tmpf2;
    f32 sp4C;
    f32 sp50;
    f32 tmpf1;
    s16 sp4A;

    if ((this->archeryState != 0) && (this->minigameType == 0) && (this->actor.bgCheckFlags & 1)) {
        this->archeryState = 0;
        this->actionFunc = func_80B123A8;
        return;
    }

    sp4A = 0;
    if ((this->movementTimer == 0) && (this->hopTimer == 0)) {
        this->targetPosTimer++;
        if (this->targetPosTimer >= 8) {
            this->movementTimer = Rand_ZeroFloat(30.0f);
            this->targetPosTimer = Rand_ZeroFloat(3.99f);

            switch (this->minigameType) {
                case 0:
                    sp50 = Rand_CenteredFloat(100.0f);
                    if (sp50 < 0.0f) {
                        sp50 -= 100.0f;
                    } else {
                        sp50 += 100.0f;
                    }

                    sp4C = Rand_CenteredFloat(100.0f);
                    if (sp4C < 0.0f) {
                        sp4C -= 100.0f;
                    } else {
                        sp4C += 100.0f;
                    }

                    this->targetPos.x = this->initPos.x + sp50;
                    this->targetPos.z = this->initPos.z + sp4C;

                    if (this->targetPos.x < -150.0f) {
                        this->targetPos.x = -150.0f;
                    }

                    if (this->targetPos.x > 150.0f) {
                        this->targetPos.x = 150.0f;
                    }

                    if (this->targetPos.z < -60.0f) {
                        this->targetPos.z = -60.0f;
                    }

                    if (this->targetPos.z > -40.0f) {
                        this->targetPos.z = -40.0f;
                    }
                    break;

                case 1:
                    sp50 = Rand_CenteredFloat(50.0f);
                    if (sp50 < 0.0f) {
                        sp50 -= 50.0f;
                    } else {
                        sp50 += 50.0f;
                    }

                    sp4C = Rand_CenteredFloat(30.0f);
                    if (sp4C < 0.0f) {
                        sp4C -= 30.0f;
                    } else {
                        sp4C += 30.0f;
                    }

                    this->targetPos.x = this->initPos.x + sp50;
                    this->targetPos.z = this->initPos.z + sp4C;
                    break;
            }
        } else {
            this->hopTimer = 4;
            if (this->actor.bgCheckFlags & 1) {
                this->actor.velocity.y = 2.5f;
                if ((Rand_ZeroFloat(10.0f) < 1.0f) && (this->minigameType == 0)) {
                    this->hopTimer = 0xC;
                    this->actor.velocity.y = 10.0f;
                }
            }
        }
    }
    if (this->hopTimer != 0) {
        sp4A = 1;
        Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 1.0f, this->posRotStep.y);
        Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 1.0f, this->posRotStep.y);
        Math_ApproachF(&this->posRotStep.y, 3.0f, 1.0f, 0.3f);
        tmpf1 = this->targetPos.x - this->actor.world.pos.x;
        tmpf2 = this->targetPos.z - this->actor.world.pos.z;

        if (fabsf(tmpf1) < 10.0f) {
            tmpf1 = 0;
        }

        if (fabsf(tmpf2) < 10.0f) {
            tmpf2 = 0.0f;
        }

        if ((tmpf1 == 0.0f) && (tmpf2 == 0.0f)) {
            this->hopTimer = 0;
            this->targetPosTimer = 7;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, Math_FAtan2F(tmpf1, tmpf2) * (0x8000 / M_PI), 3,
                           this->posRotStep.z, 0);
        Math_ApproachF(&this->posRotStep.z, 10000.0f, 1.0f, 1000.0f);
    }

    if (this->sootTimer == 0) {
        func_80B11A94(this, play, sp4A);
        return;
    }

    if ((play->gameplayFrames % 4) == 0) {
        dustVelocity.y = Rand_CenteredFloat(5.0f);
        dustAccel.y = 0.2f;
        dustPos = this->actor.world.pos;
        func_8002836C(play, &dustPos, &dustVelocity, &dustAccel, &dustPrimColor, &dustEnvColor, 600, 40, 30);
    }
}

void func_80B123A8(EnSyatekiNiw* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    this->rightWingRotZTarget = 6000.0f;
    this->unkArcheryFloat = -10000.0f;
    this->rightWingRot.z = 6000.0f;
    this->rightWingRot.y = 10000.0f;
    this->actionFunc = func_80B12460;
    this->leftWingRot.z = 6000.0f;
    this->leftWingRotZTarget = 6000.0f;
    this->rightWingRot.x = -10000.0f;
    this->rightWingRotXTarget = -10000.0f;
    this->leftWingRot.y = -10000.0f;
    this->leftWingRot.x = -10000.0f;
    this->leftWingRotXTarget = -10000.0f;
}

void func_80B12460(EnSyatekiNiw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 phi_f16 = 0.0f;

    player->actor.freezeTimer = 10;
    switch (this->archeryState) {
        case 0:
            this->archeryAnimationType = 2;
            this->posRotStep.y = 0.0f;
            this->archeryState = 1;
            break;

        case 1:
            this->actor.speedXZ = 2.0f;
            if (this->hopTimer == 0) {
                this->hopTimer = 3;
                this->actor.velocity.y = 3.5f;
            }

            if (this->archeryTimer == 0) {
                this->rotYFlip++;
                this->rotYFlip &= 1;
                this->archeryTimer = 5;
            }

            phi_f16 = (this->rotYFlip == 0) ? 5000.0f : -5000.0f;
            if (this->actor.world.pos.z > 100.0f) {
                this->actor.speedXZ = 2.0f;
                this->actor.gravity = -0.3f;
                this->actor.velocity.y = 5.0f;
                this->archeryState = 2;
            }
            break;

        case 2:
            if ((player->actor.world.pos.z - 40.0f) < this->actor.world.pos.z) {
                this->actor.speedXZ = 0.0f;
            }

            if ((this->actor.bgCheckFlags & 1) && (this->actor.world.pos.z > 110.0f)) {
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = 0.0f;
                this->leftWingRotZTarget = 0.0f;
                this->rightWingRotZTarget = 0.0f;
                this->rightWingRotYTarget = 0.0f;
                this->leftWingRotYTarget = 0.0f;
                this->unkArcheryFloat = 0.0f;
                this->actor.speedXZ = 0.5f;
                this->peckTimer = this->timer1 = 0;
                this->headRotXState = this->unk_290 = 0;
                this->archeryAnimationType = 1;
                this->archeryState = 3;
            }
            break;

        case 3:
            if ((player->actor.world.pos.z - 50.0f) < this->actor.world.pos.z) {
                this->actor.speedXZ = 0.0f;
                this->cluckTimer = 0x3C;
                this->archeryTimer = 0x14;
                this->headRotXTarget = 10000.0f;
                this->archeryState = 4;
            }
            break;

        case 4:
            if (this->archeryTimer == 0) {
                this->archeryAnimationType = 4;
                this->headRotXTarget = 5000.0f;
                this->leftWingRotXTarget = 0.0f;
                this->rightWingRotXTarget = 0.0f;
                this->leftWingRotZTarget = 0.0f;
                this->rightWingRotZTarget = 0.0f;
                this->leftWingRotYTarget = 14000.0f;
                this->rightWingRotYTarget = 14000.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
                this->peckTimer = this->timer1 = this->archeryTimer = 0x1E;
                this->archeryState = 5;
            }
            break;

        case 5:
            if (this->archeryTimer == 1) {
                this->flapTimer = 0;
                this->archeryAnimationType = 5;
                this->timer1 = this->flapTimer;
                this->peckTimer = this->flapTimer;
                this->actor.speedXZ = 1.0f;
            }

            if ((this->archeryTimer == 0) && ((player->actor.world.pos.z - 30.0f) < this->actor.world.pos.z)) {
                Audio_PlaySoundGeneral(NA_SE_VO_LI_DOWN, &this->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                this->movementTimer = 0x14;
                this->archeryState = 6;
                this->actor.speedXZ = 0.0f;
            }
            break;

        case 6:
            if (this->movementTimer == 1) {
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->nextEntranceIndex = gSaveContext.entranceIndex;
                play->shootingGalleryStatus = 0;
                player->actor.freezeTimer = 20;
                this->movementTimer = 0x14;
                this->actionFunc = func_80B128D8;
            }
            break;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y,
                       (s16)(Math_FAtan2F(player->actor.world.pos.x - this->actor.world.pos.x,
                                          player->actor.world.pos.z - this->actor.world.pos.z) *
                             (0x8000 / M_PI)) +
                           phi_f16,
                       5, this->posRotStep.y, 0);
    Math_ApproachF(&this->posRotStep.y, 3000.0f, 1.0f, 500.0f);
    if (this->archeryAnimationType == 2) {
        this->timer1 = 10;
        this->peckTimer = this->timer1;
    }

    func_80B11A94(this, play, this->archeryAnimationType);
}

void func_80B128D8(EnSyatekiNiw* this, PlayState* play) {
    if (this->movementTimer == 1) {
        gSaveContext.timerState = TIMER_STATE_OFF;
    }
}

void func_80B128F8(EnSyatekiNiw* this, PlayState* play) {
    s16 sp26;
    s16 sp24;

    Actor_SetFocus(&this->actor, this->focusYOffset);
    Actor_GetScreenPos(play, &this->actor, &sp26, &sp24);
    if ((this->actor.projectedPos.z > 200.0f) && (this->actor.projectedPos.z < 800.0f) && (sp26 > 0) &&
        (sp26 < SCREEN_WIDTH) && (sp24 > 0) && (sp24 < SCREEN_HEIGHT)) {
        this->actor.speedXZ = 5.0f;
        this->rotYFlip = Rand_ZeroFloat(1.99f);
        this->removeStateYaw = Rand_CenteredFloat(8000.0f) + -10000.0f;
        this->cluckTimer = 0x1E;
        this->movementTimer = 0x64;
        this->actionFunc = func_80B129EC;
    }
}

void func_80B129EC(EnSyatekiNiw* this, PlayState* play) {
    s32 pad;
    f32 phi_f2;
    s16 sp2E;
    s16 sp2C;
    f32 tmpf2;

    Actor_SetFocus(&this->actor, this->focusYOffset);
    Actor_GetScreenPos(play, &this->actor, &sp2E, &sp2C);
    if ((this->movementTimer == 0) || (this->actor.projectedPos.z < -70.0f) || (sp2E < 0) || (sp2E > SCREEN_WIDTH) ||
        (sp2C < 0) || (sp2C > SCREEN_HEIGHT)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->spawnFeathers = 1;
    if (this->hopTimer == 0) {
        this->rotYFlip++;
        this->rotYFlip &= 1;
        this->hopTimer = (s16)Rand_CenteredFloat(4.0f) + 5;
        if ((Rand_ZeroFloat(5.0f) < 1.0f) && (this->actor.bgCheckFlags & 1)) {
            this->actor.velocity.y = 4.0f;
        }
    }

    phi_f2 = (this->rotYFlip == 0) ? 5000.0f : -5000.0f;
    tmpf2 = this->removeStateYaw + phi_f2;
    Math_SmoothStepToS(&this->actor.world.rot.y, tmpf2, 3, this->posRotStep.y, 0);
    Math_ApproachF(&this->posRotStep.y, 3000.0f, 1.0f, 500.0f);
    func_80B11A94(this, play, 2);
}

void func_80B12BA4(EnSyatekiNiw* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        switch (this->minigameType) {
            case 0:
                if (this->archeryState == 0) {
                    this->cluckTimer = 0x1E;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
                    this->archeryState = 1;
                    this->spawnFeathers = 1;
                    this->actionFunc = func_80B123A8;
                    this->actor.gravity = -3.0f;
                }
                break;

            case 1:
                this->cluckTimer = 0x1E;
                this->unkAlleyHitByte = 1;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
                this->sootTimer = 100;
                this->spawnFeathers = 1;
                this->movementTimer = this->sootTimer;
                break;
        }
    }
}

void EnSyatekiNiw_Update(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    s32 pad;
    s16 i;
    Vec3f sp90 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp84 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;

    func_80B132A8(this, play);
    this->lifetime++;
    if (this->peckTimer != 0) {
        this->peckTimer--;
    }

    if (this->flapTimer != 0) {
        this->flapTimer--;
    }

    if (this->archeryTimer != 0) {
        this->archeryTimer--;
    }

    if (this->hopTimer != 0) {
        this->hopTimer--;
    }

    if (this->movementTimer != 0) {
        this->movementTimer--;
    }

    if (this->cluckTimer != 0) {
        this->cluckTimer--;
    }

    if (this->sootTimer != 0) {
        this->sootTimer--;
    }

    this->actor.shape.rot = this->actor.world.rot;
    this->actor.shape.shadowScale = 15.0f;

    this->actionFunc(this, play);
    Actor_MoveXZGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f, 0x1D);

    if (this->spawnFeathers != 0) {
        for (i = 0; i < 20; i++) {
            sp78.x = Rand_CenteredFloat(10.0f) + this->actor.world.pos.x;
            sp78.y = Rand_CenteredFloat(10.0f) + (this->actor.world.pos.y + 20.0f);
            sp78.z = Rand_CenteredFloat(10.0f) + this->actor.world.pos.z;
            sp6C.x = Rand_CenteredFloat(3.0f);
            sp6C.y = (Rand_ZeroFloat(2.0f) * 0.5f) + 2.0f;
            sp6C.z = Rand_CenteredFloat(3.0f);
            sp60.z = sp60.x = 0.0f;
            sp60.y = -0.15f;
            func_80B131B8(this, &sp78, &sp6C, &sp60, Rand_ZeroFloat(8.0f) + 8.0f);
        }

        this->spawnFeathers = 0;
    }

    func_80B12BA4(this, play);
    if (this->cluckTimer == 0) {
        if (this->actionFunc == func_80B11E78) {
            this->cluckTimer = 0x12C;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_N);
        } else {
            this->cluckTimer = 0x1E;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
        }
    }

    i = 0;
    switch (this->minigameType) {
        case 0:
            if (play->shootingGalleryStatus != 0) {
                i = 1;
            }
            break;

        case 1:
            i = 1;
            break;
    }

    if (i != 0) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 SyatekiNiw_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    Vec3f sp0 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 13) {
        rot->y += (s16)this->headRot.x;
    }

    if (limbIndex == 11) {
        rot->x += (s16)this->rightWingRot.z;
        rot->y += (s16)this->rightWingRot.y;
        rot->z += (s16)this->rightWingRot.x;
    }

    if (limbIndex == 7) {
        rot->x += (s16)this->leftWingRot.z;
        rot->y += (s16)this->leftWingRot.y;
        rot->z += (s16)this->leftWingRot.x;
    }

    return false;
}

void EnSyatekiNiw_Draw(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    Color_RGBA8 sp30 = { 0, 0, 0, 255 };

    if (this->actionFunc != func_80B128F8) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        if (this->sootTimer != 0) {
            func_80026230(play, &sp30, 0, 0x14);
        }

        SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, SyatekiNiw_OverrideLimbDraw, NULL, this);
        func_80026608(play);
        func_80B13464(this, play);
    }
}

void func_80B131B8(EnSyatekiNiw* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4) {
    s16 i;
    EnSyatekiNiw_1* ptr = &this->effects[0];

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->state == 0) {
            ptr->epoch++;
            ptr->state = 1;
            ptr->pos = *arg1;
            ptr->vel = *arg2;
            ptr->accel = *arg3;
            ptr->timer = 0;
            ptr->scale = (arg4 / 1000.0f);
            ptr->lifespan = (s16)Rand_ZeroFloat(20.0f) + 0x28;
            ptr->rotPulse = Rand_ZeroFloat(1000.0f);
            return;
        }
    }
}

void func_80B132A8(EnSyatekiNiw* this, PlayState* play) {
    s16 i;
    EnSyatekiNiw_1* ptr = &this->effects[0];

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->state != 0) {
            ptr->pos.x += ptr->vel.x;
            ptr->pos.y += ptr->vel.y;
            ptr->pos.z += ptr->vel.z;
            ptr->timer++;
            ptr->vel.x += ptr->accel.x;
            ptr->vel.y += ptr->accel.y;
            ptr->vel.z += ptr->accel.z;
            if (ptr->state == 1) {
                ptr->rotPulse++;
                Math_ApproachF(&ptr->vel.x, 0.0f, 1.0f, 0.05f);
                Math_ApproachF(&ptr->vel.z, 0.0f, 1.0f, 0.05f);
                if (ptr->vel.y < -0.5f) {
                    ptr->vel.y = 0.5f;
                }

                ptr->rot = (Math_SinS(ptr->rotPulse * 3000) * M_PI) * 0.2f;
                if (ptr->lifespan < ptr->timer) {
                    ptr->state = 0;
                }
            }
        }
    }
}

void func_80B13464(EnSyatekiNiw* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    EnSyatekiNiw_1* ptr = &this->effects[0];
    u8 flag = 0;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->state == 1) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gCuccoEffectFeatherMaterialDL);
                flag++;
            }

            FrameInterpolation_RecordOpenChild(ptr, ptr->epoch);
            Matrix_Translate(ptr->pos.x, ptr->pos.y, ptr->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(ptr->scale, ptr->scale, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZ(ptr->rot, MTXMODE_APPLY);
            Matrix_Translate(0.0f, -1000.0f, 0.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gCuccoEffectFeatherModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

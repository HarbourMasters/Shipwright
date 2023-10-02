/*
 * File: z_en_tr.c
 * Overlay: ovl_En_Tr
 * Description: Koume and Kotake
 */

#include "z_en_tr.h"
#include "objects/object_tr/object_tr.h"
#include <assert.h>

#define FLAGS ACTOR_FLAG_UPDATE_WHILE_CULLED

void EnTr_Init(Actor* thisx, PlayState* play);
void EnTr_Destroy(Actor* thisx, PlayState* play);
void EnTr_Update(Actor* thisx, PlayState* play);
void EnTr_Draw(Actor* thisx, PlayState* play);

void EnTr_DoNothing(EnTr* this, PlayState* play);
void EnTr_ShrinkVanish(EnTr* this, PlayState* play);
void EnTr_WaitToReappear(EnTr* this, PlayState* play);
void EnTr_ChooseAction1(EnTr* this, PlayState* play);

void EnTr_UpdateRotation(EnTr* this, PlayState* play, s32 actionIndex);
void func_80B24038(EnTr* this, PlayState* play, s32 actionIndex);
void EnTr_SetStartPosRot(EnTr* this, PlayState* play, s32 actionIndex);

const ActorInit En_Tr_InitVars = {
    ACTOR_EN_TR,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TR,
    sizeof(EnTr),
    (ActorFunc)EnTr_Init,
    (ActorFunc)EnTr_Destroy,
    (ActorFunc)EnTr_Update,
    (ActorFunc)EnTr_Draw,
    NULL,
};

// The first elements of these animation arrays are for Koume, the second for Kotake

static AnimationHeader* unused[] = {
    &gKotakeKoumeStandingBroomOverRightShoulderAnim,
    &gKotakeKoumeStandingBroomOverLeftShoulderAnim,
};

static AnimationHeader* D_80B24368[] = {
    &gKotakeKoumeLookOverLeftShoulderAnim,
    &gKotakeKoumeLookOverRightShoulderAnim,
};

static AnimationHeader* D_80B24370[] = {
    &gKotakeKoumeLookingOverLeftShoulderAnim,
    &gKotakeKoumeLookingOverRightShoulderAnim,
};

static AnimationHeader* D_80B24378[] = {
    &gKotakeKoumeFlyAnim,
    &gKotakeKoumeFlyAnim,
};

static AnimationHeader* D_80B24380[] = {
    &gKotakeKoumeCastMagicAnim,
    &gKotakeKoumeCastMagicAnim,
};

static f32 D_80B24388[] = { 0.0f, 20.0f, -30.0f, 20.0f, -20.0f, -20.0f, 30.0f };

static f32 D_80B243A4[] = { 0.0f, 30.0f, 0.0f, -30.0f, 30.0f, -30.0f, 0.0f };

// Has to be 1-dimensional to match
static Color_RGBA8 D_80B243C0[4] = {
    { 255, 200, 0, 255 },
    { 255, 0, 0, 255 },
    { 255, 255, 255, 255 },
    { 0, 0, 255, 255 },
};

static void* sEyeTextures[] = {
    gKotakeKoumeEyeOpenTex,
    gKotakeKoumeEyeHalfTex,
    gKotakeKoumeEyeClosedTex,
};

void EnTr_SetupAction(EnTr* this, EnTrActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnTr_Init(Actor* thisx, PlayState* play) {
    EnTr* this = (EnTr*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    EnTr_SetupAction(this, EnTr_DoNothing);
    this->unk_2D4 = 0; // Set and not used
    this->actor.child = NULL;
    Actor_SetScale(&this->actor, 0.01f);

    switch (this->actor.params) {
        case TR_KOUME:
            SkelAnime_InitFlex(play, &this->skelAnime, &gKoumeSkel, &gKotakeKoumeStandingBroomOverRightShoulderAnim,
                               this->jointTable, this->morphTable, 27);
            Animation_PlayOnce(&this->skelAnime, &gKotakeKoumeStandingBroomOverRightShoulderAnim);
            this->animation = NULL;
            EnTr_SetupAction(this, EnTr_ChooseAction1);
            this->actionIndex = 3;
            break;

        case TR_KOTAKE:
            SkelAnime_InitFlex(play, &this->skelAnime, &gKotakeSkel, &gKotakeKoumeStandingBroomOverLeftShoulderAnim,
                               this->jointTable, this->morphTable, 27);
            Animation_PlayOnce(&this->skelAnime, &gKotakeKoumeStandingBroomOverLeftShoulderAnim);
            this->animation = NULL;
            EnTr_SetupAction(this, EnTr_ChooseAction1);
            this->actionIndex = 2;
            break;

        default:
            assert(0);
            break;
    }
}

void EnTr_Destroy(Actor* thisx, PlayState* play) {
    EnTr* this = (EnTr*)thisx;

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnTr_CrySpellcast(EnTr* this, PlayState* play) {
    if (this->timer == 11) {
        // Both cry in the title screen cutscene, but only Kotake in the in-game cutscene
        if ((this->actor.params != TR_KOUME) || (gSaveContext.sceneSetupIndex == 6)) {
            Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_SHOOT_VOICE, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }

    if (this->timer > 0) {
        this->timer--;
    } else if (this->actor.child != NULL) {
        this->actor.child = NULL;
    }
    func_8002F974(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
}

void EnTr_DoNothing(EnTr* this, PlayState* play) {
}

void EnTr_ChooseAction2(EnTr* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.npcActions[this->actionIndex] != NULL) {
            switch (play->csCtx.npcActions[this->actionIndex]->action) {

                case 4:
                    Actor_SetScale(&this->actor, 0.01f);
                    EnTr_SetupAction(this, EnTr_ShrinkVanish);
                    this->timer = 24;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DEAD2);
                    break;

                case 6:
                    Animation_Change(&this->skelAnime, D_80B24380[this->actor.params], 1.0f, 0.0f,
                                     Animation_GetLastFrame(D_80B24380[this->actor.params]), ANIMMODE_ONCE, -5.0f);
                    EnTr_SetupAction(this, EnTr_CrySpellcast);
                    this->animation = D_80B24378[this->actor.params];
                    this->timer = 39;
                    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K,
                                       this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0,
                                       0, this->actor.params + 9);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_MASIC1);
                    break;

                default:
                    func_80B24038(this, play, this->actionIndex);
                    EnTr_UpdateRotation(this, play, this->actionIndex);
                    break;
            }
            func_8002F974(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
        }
    }
}

void EnTr_FlyKidnapCutscene(EnTr* this, PlayState* play) {
    Vec3f originalPos = this->actor.world.pos;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.npcActions[this->actionIndex] != NULL) {
            if (play->csCtx.npcActions[this->actionIndex]->action == 8) {
                func_80B24038(this, play, this->actionIndex);
                this->actor.world.rot.y = Math_Atan2S(this->actor.velocity.z, this->actor.velocity.x);
                Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 10, 0x400, 0x100);
                this->actor.world.rot.y = this->actor.shape.rot.y;
            } else {
                EnTr_SetStartPosRot(this, play, this->actionIndex);
                this->actor.world.pos.x += Math_SinS(this->timer) * 150.0f;
                this->actor.world.pos.y += -100.0f;
                this->actor.world.pos.z += Math_CosS(this->timer) * 150.0f;

                this->actor.shape.rot.y = (s16)(this->timer) + 0x4000;
                this->timer += 0x400;

                this->actor.velocity.x = this->actor.world.pos.x - originalPos.x;
                this->actor.velocity.y = this->actor.world.pos.y - originalPos.y;
                this->actor.velocity.z = this->actor.world.pos.z - originalPos.z;
            }

            if (play->csCtx.frames < 670) {
                func_8002F974(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
            }
        }
    }
}

void func_80B23254(EnTr* this, PlayState* play, s32 arg2, f32 arg3, f32 scale) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    Vec3f sp58;
    Color_RGBA8* primColor;
    Color_RGBA8* envColor;
    Vec3f cameraEye = GET_ACTIVE_CAM(play)->eye;
    s16 yaw = Math_Vec3f_Yaw(&cameraEye, &this->actor.world.pos);
    s16 reversePitch = -Math_Vec3f_Pitch(&cameraEye, &this->actor.world.pos);
    f32 sp3C;

    accel.x = accel.z = 0.0f;
    sp3C = Math_SinS(yaw);
    velocity.x = Math_CosS(reversePitch) * (arg3 * sp3C);
    velocity.y = Math_SinS(reversePitch) * arg3;
    sp3C = Math_CosS(yaw);
    velocity.z = Math_CosS(reversePitch) * (arg3 * sp3C);
    accel.y = 0.5f;

    primColor = &D_80B243C0[2 * this->actor.params];
    envColor = &D_80B243C0[2 * this->actor.params + 1];

    sp58 = this->actor.world.pos;
    sp58.x -= velocity.x * 10.0f;
    sp58.y -= velocity.y * 10.0f;
    sp58.z -= velocity.z * 10.0f;

    pos.x = sp58.x + ((D_80B24388[arg2] * scale) * Math_CosS(yaw));
    pos.y = sp58.y + (D_80B243A4[arg2] * scale);
    pos.z = sp58.z - ((D_80B24388[arg2] * scale) * Math_SinS(yaw));
    func_8002829C(play, &pos, &velocity, &accel, primColor, envColor, (s32)(800.0f * scale), (s32)(80.0f * scale));
}

void EnTr_ShrinkVanish(EnTr* this, PlayState* play) {
    if (this->timer >= 17) {
        this->actor.shape.rot.y = (this->actor.shape.rot.y - (this->timer * 0x28F)) + 0x3D68;
    } else {
        if (this->timer >= 5) {
            Actor_SetScale(&this->actor, this->actor.scale.x * 0.9f);
            this->actor.shape.rot.y = (this->actor.shape.rot.y - (this->timer * 0x28F)) + 0x3D68;
        } else if (this->timer > 0) {
            s32 temp_hi = (this->timer * 2) % 7;

            func_80B23254(this, play, temp_hi, 5.0f, 0.2f);
            func_80B23254(this, play, (temp_hi + 1) % 7, 5.0f, 0.2f);
            Actor_SetScale(&this->actor, this->actor.scale.x * 0.9f);
            this->actor.shape.rot.y = (this->actor.shape.rot.y - (this->timer * 0x28F)) + 0x3D68;
        } else {
            EnTr_SetupAction(this, EnTr_WaitToReappear);
            this->actor.draw = NULL;
        }
    }

    if (this->timer == 4) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_DOWN);
    }

    if (this->timer > 0) {
        this->timer--;
    }
}

void EnTr_Reappear(EnTr* this, PlayState* play) {
    if (this->timer >= 31) {
        s32 temp_hi = (this->timer * 2) % 7;

        func_80B23254(this, play, temp_hi, 5.0f, 1.0f);
        func_80B23254(this, play, (temp_hi + 1) % 7, 5.0f, 1.0f);
    } else if (this->timer == 30) {
        this->actor.draw = EnTr_Draw;
        this->actor.shape.rot.y += this->timer * 0x1A6;
    } else if (this->timer > 0) {
        this->actor.shape.rot.y += this->timer * 0x1A6;
        Actor_SetScale(&this->actor, (this->actor.scale.x * 0.8f) + 0.002f);
    } else {
        EnTr_SetupAction(this, EnTr_ChooseAction2);
        Actor_SetScale(&this->actor, 0.01f);
    }

    if (this->timer > 0) {
        this->timer--;
    }
    func_8002F974(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
}

void EnTr_WaitToReappear(EnTr* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.npcActions[this->actionIndex] != NULL) &&
            ((play->csCtx.npcActions[this->actionIndex]->action == 3) ||
             (play->csCtx.npcActions[this->actionIndex]->action == 5))) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_TWINROBA_TRANSFORM);
            this->timer = 34;
            EnTr_SetStartPosRot(this, play, this->actionIndex);
            EnTr_SetupAction(this, EnTr_Reappear);
            Animation_PlayLoop(&this->skelAnime, &gKotakeKoumeFlyAnim);
            this->animation = NULL;
            Actor_SetScale(&this->actor, 0.003f);
        }
    }
}

void EnTr_TakeOff(EnTr* this, PlayState* play) {
    f32 lastFrame = Animation_GetLastFrame(D_80B24378[this->actor.params]);

    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.npcActions[this->actionIndex] != NULL) &&
            (play->csCtx.npcActions[this->actionIndex]->action == 3)) {
            Animation_Change(&this->skelAnime, D_80B24378[this->actor.params], 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP,
                             -10.0f);
            this->animation = NULL;
            EnTr_SetupAction(this, EnTr_ChooseAction2);
        }
    }
}

void EnTr_TurnLookOverShoulder(EnTr* this, PlayState* play) {
    f32 lastFrame = Animation_GetLastFrame(D_80B24368[this->actor.params]);

    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.npcActions[this->actionIndex] != NULL) &&
            (play->csCtx.npcActions[this->actionIndex]->action == 2)) {
            Animation_Change(&this->skelAnime, D_80B24368[this->actor.params], 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE,
                             -4.0f);
            this->animation = D_80B24370[this->actor.params];
            EnTr_SetupAction(this, EnTr_TakeOff);
        }
    }
}

void EnTr_ChooseAction1(EnTr* this, PlayState* play) {
    u32 frames = play->gameplayFrames;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.npcActions[this->actionIndex] != NULL) {
            switch (play->csCtx.npcActions[this->actionIndex]->action) {
                case 1:
                    EnTr_SetStartPosRot(this, play, this->actionIndex);
                    EnTr_SetupAction(this, EnTr_TurnLookOverShoulder);
                    break;

                case 3:
                    EnTr_SetStartPosRot(this, play, this->actionIndex);
                    EnTr_SetupAction(this, EnTr_ChooseAction2);
                    Animation_PlayLoop(&this->skelAnime, &gKotakeKoumeFlyAnim);
                    this->animation = NULL;
                    break;

                case 4:
                    EnTr_SetupAction(this, EnTr_WaitToReappear);
                    this->actor.draw = NULL;
                    break;

                case 7:
                    EnTr_SetupAction(this, EnTr_FlyKidnapCutscene);
                    Animation_PlayLoop(&this->skelAnime, &gKotakeKoumeFlyAnim);
                    this->animation = NULL;
                    this->timer =
                        ((this->actor.params != TR_KOUME) ? ((u8)frames * 0x400) + 0x8000 : (u8)frames * 0x400);
                    break;
            }
        }
    }
}

void EnTr_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTr* this = (EnTr*)thisx;

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 5);
    this->actionFunc(this, play);

    if (SkelAnime_Update(&this->skelAnime) != 0) {
        if (this->animation != NULL) {
            if ((this->animation == &gKotakeKoumeLookingOverLeftShoulderAnim) || (this->animation == &gKotakeKoumeLookingOverRightShoulderAnim)) {
                if (this->actor.params != TR_KOUME) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_TWINROBA_LAUGH2);
                } else {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_TWINROBA_LAUGH);
                }
                Animation_PlayLoop(&this->skelAnime, this->animation);
            } else if (this->animation == &gKotakeKoumeFlyAnim) {
                EnTr_SetupAction(this, EnTr_ChooseAction2);
                Animation_Change(&this->skelAnime, &gKotakeKoumeFlyAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gKotakeKoumeFlyAnim), ANIMMODE_LOOP, -5.0f);
            } else {
                Animation_PlayLoop(&this->skelAnime, this->animation);
            }
            this->animation = NULL;
        } else {
            this->skelAnime.curFrame = 0.0f;
        }
    }
    Actor_SetFocus(&this->actor, 0.0f);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }
    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

s32 EnTr_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Vec3f src = { 2300.0f, 0.0f, -600.0f };
    Vec3f dest = { 0.0f, 0.0f, 0.0f };
    EnTr* this = (EnTr*)thisx;
    Actor* child = this->actor.child;

    if ((child != NULL) && (limbIndex == 19)) {
        Matrix_MultVec3f(&src, &dest);
        dest.x -= (10.0f * Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))));
        dest.z -= (10.0f * Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))));
        child->world.pos = dest;
    }
    return 0;
}

void EnTr_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTr* this = (EnTr*)thisx;

    if ((play->csCtx.state == CS_STATE_IDLE) || (play->csCtx.npcActions[this->actionIndex] == 0)) {
        this->actor.shape.shadowDraw = NULL;
    } else {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;

        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL_37Opa(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeIndex]));
        func_8002EBCC(&this->actor, play, 0);
        SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnTr_OverrideLimbDraw, NULL, this);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

f32 func_80B23FDC(PlayState* play, s32 actionIndex) {
    f32 phi_f2 = Environment_LerpWeight(play->csCtx.npcActions[actionIndex]->endFrame,
                                        play->csCtx.npcActions[actionIndex]->startFrame, play->csCtx.frames);
    phi_f2 = CLAMP_MAX(phi_f2, 1.0f);
    return phi_f2;
}

void func_80B24038(EnTr* this, PlayState* play, s32 actionIndex) {
    Vec3f startPos;
    Vec3f endPos;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 phi_f12;

    startPos.x = play->csCtx.npcActions[actionIndex]->startPos.x;
    startPos.y = play->csCtx.npcActions[actionIndex]->startPos.y;
    startPos.z = play->csCtx.npcActions[actionIndex]->startPos.z;

    endPos.x = play->csCtx.npcActions[actionIndex]->endPos.x;
    endPos.y = play->csCtx.npcActions[actionIndex]->endPos.y;
    endPos.z = play->csCtx.npcActions[actionIndex]->endPos.z;

    temp_f0 = func_80B23FDC(play, actionIndex);

    startPos.x = ((endPos.x - startPos.x) * temp_f0) + startPos.x;
    startPos.y = ((endPos.y - startPos.y) * temp_f0) + startPos.y;
    startPos.z = ((endPos.z - startPos.z) * temp_f0) + startPos.z;

    endPos.x = (startPos.x - this->actor.world.pos.x) * 0.1f;
    endPos.y = (startPos.y - this->actor.world.pos.y) * 0.1f;
    endPos.z = (startPos.z - this->actor.world.pos.z) * 0.1f;

    temp_f0_2 = sqrtf(SQ(endPos.x) + SQ(endPos.y) + SQ(endPos.z));
    phi_f12 = CLAMP(temp_f0_2, 0.0f, 20.0f);

    if ((temp_f0_2 != phi_f12) && (temp_f0_2 != 0.0f)) {
        endPos.x *= phi_f12 / temp_f0_2;
        endPos.y *= phi_f12 / temp_f0_2;
        endPos.z *= phi_f12 / temp_f0_2;
    }

    Math_StepToF(&this->actor.velocity.x, endPos.x, 1.0f);
    Math_StepToF(&this->actor.velocity.y, endPos.y, 1.0f);
    Math_StepToF(&this->actor.velocity.z, endPos.z, 1.0f);
    func_8002D7EC(&this->actor);
}

void EnTr_UpdateRotation(EnTr* this, PlayState* play, s32 actionIndex) {
    s16 rotY = play->csCtx.npcActions[actionIndex]->rot.y;
    s32 rotDiff = this->actor.world.rot.y - rotY;
    s32 rotSign;

    if (rotDiff < 0) {
        rotDiff = -rotDiff;
        rotSign = 1;
    } else {
        rotSign = -1;
    }

    if (rotDiff >= 0x8000) {
        rotSign = -rotSign;
        rotDiff = 0x10000 - rotDiff;
    }

    rotDiff *= 0.1f;

    this->actor.world.rot.y += rotDiff * rotSign;
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnTr_SetStartPosRot(EnTr* this, PlayState* play, s32 actionIndex) {
    Vec3f startPos;

    startPos.x = play->csCtx.npcActions[actionIndex]->startPos.x;
    startPos.y = play->csCtx.npcActions[actionIndex]->startPos.y;
    startPos.z = play->csCtx.npcActions[actionIndex]->startPos.z;

    this->actor.world.pos = startPos;
    this->actor.world.rot.y = this->actor.shape.rot.y = play->csCtx.npcActions[actionIndex]->rot.y;
}

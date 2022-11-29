/*
 * File: z_en_diving_game.c
 * Overlay: ovl_En_Diving_Game
 * Description: Diving minigame
 */

#include "z_en_diving_game.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "objects/object_zo/object_zo.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnDivingGame_Init(Actor* thisx, PlayState* play);
void EnDivingGame_Destroy(Actor* thisx, PlayState* play);
void EnDivingGame_Update(Actor* thisx, PlayState* play);
void EnDivingGame_Draw(Actor* thisx, PlayState* play);

void func_809EDCB0(EnDivingGame* this, PlayState* play);
void EnDivingGame_Talk(EnDivingGame* this, PlayState* play);
void EnDivingGame_HandlePlayChoice(EnDivingGame* this, PlayState* play);
void func_809EE048(EnDivingGame* this, PlayState* play);
void func_809EE0FC(EnDivingGame* this, PlayState* play);
void func_809EE194(EnDivingGame* this, PlayState* play);
void EnDivingGame_SetupRupeeThrow(EnDivingGame* this, PlayState* play);
void EnDivingGame_RupeeThrow(EnDivingGame* this, PlayState* play);
void EnDivingGame_SetupUnderwaterViewCs(EnDivingGame* this, PlayState* play);
void func_809EE780(EnDivingGame* this, PlayState* play);
void func_809EE800(EnDivingGame* this, PlayState* play);
void func_809EE8F0(EnDivingGame* this, PlayState* play);
void func_809EE96C(EnDivingGame* this, PlayState* play);
void func_809EEA00(EnDivingGame* this, PlayState* play);
void func_809EEA90(EnDivingGame* this, PlayState* play);
void func_809EEAF8(EnDivingGame* this, PlayState* play);

const ActorInit En_Diving_Game_InitVars = {
    ACTOR_EN_DIVING_GAME,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZO,
    sizeof(EnDivingGame),
    (ActorFunc)EnDivingGame_Init,
    (ActorFunc)EnDivingGame_Destroy,
    (ActorFunc)EnDivingGame_Update,
    (ActorFunc)EnDivingGame_Draw,
    NULL,
};

// used to ensure there's only one instance of this actor.
static u8 sHasSpawned = false;

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 10, 10, 0, { 0, 0, 0 } },
};

static void* sEyeTextures[] = {
    gZoraEyeOpenTex,
    gZoraEyeHalfTex,
    gZoraEyeClosedTex,
};

void EnDivingGame_Init(Actor* thisx, PlayState* play) {
    EnDivingGame* this = (EnDivingGame*)thisx;

    this->actor.gravity = -3.0f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gZoraSkel, &gZoraIdleAnim, this->jointTable, this->morphTable, 20);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 素もぐりＧＯ ☆☆☆☆☆ \n" VT_RST);
    this->actor.room = -1;
    this->actor.scale.x = 0.01f;
    this->actor.scale.y = 0.012999999f;
    this->actor.scale.z = 0.0139999995f;
    if (sHasSpawned) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ もういてる原 ☆☆☆☆☆ \n" VT_RST);
        this->unk_31F = 1;
        Actor_Kill(&this->actor);
    } else {
        sHasSpawned = true;
        this->actor.targetMode = 0;
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actionFunc = func_809EDCB0;
    }
}

void EnDivingGame_Destroy(Actor* thisx, PlayState* play) {
    EnDivingGame* this = (EnDivingGame*)thisx;

    if (this->unk_31F == 0) {
        sHasSpawned = false;
        gSaveContext.timer1State = 0;
    }
    Collider_DestroyCylinder(play, &this->collider);
}

void EnDivingGame_SpawnRuppy(EnDivingGame* this, PlayState* play) {
    EnExRuppy* rupee;
    Vec3f rupeePos;

    rupeePos.x = (Rand_ZeroOne() - 0.5f) * 30.0f + this->actor.world.pos.x;
    rupeePos.y = (Rand_ZeroOne() - 0.5f) * 20.0f + (this->actor.world.pos.y + 30.0f);
    rupeePos.z = (Rand_ZeroOne() - 0.5f) * 20.0f + this->actor.world.pos.z;
    rupee = (EnExRuppy*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_RUPPY, rupeePos.x,
                                           rupeePos.y, rupeePos.z, 0, (s16)Rand_CenteredFloat(3500.0f) - 1000,
                                           this->rupeesLeftToThrow, 0);
    if (rupee != NULL) {
        rupee->actor.speedXZ = 12.0f;
        rupee->actor.velocity.y = 6.0f;
    }
}

s32 EnDivingGame_HasMinigameFinished(EnDivingGame* this, PlayState* play) {
    if (gSaveContext.timer1State == 10 && !Play_InCsMode(play)) {
        // Failed.
        gSaveContext.timer1State = 0;
        func_800F5B58();
        func_80078884(NA_SE_SY_FOUND);
        this->actor.textId = 0x71AD;
        Message_StartTextbox(play, this->actor.textId, NULL);
        this->unk_292 = TEXT_STATE_EVENT;
        this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
        func_8002DF54(play, NULL, 8);
        this->actionFunc = func_809EE048;
        return true;
    } else {
        s32 rupeesNeeded = 5;

        if (gSaveContext.eventChkInf[3] & 0x100) {
            rupeesNeeded = 10;
        }
        if (this->grabbedRupeesCounter >= rupeesNeeded) {
            // Won.
            gSaveContext.timer1State = 0;
            this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
            if (!(gSaveContext.eventChkInf[3] & 0x100)) {
                this->actor.textId = 0x4055;
            } else {
                this->actor.textId = 0x405D;
                if (this->extraWinCount < 100) {
                    this->extraWinCount++;
                }
            }
            Message_StartTextbox(play, this->actor.textId, NULL);
            this->unk_292 = TEXT_STATE_EVENT;
            func_800F5B58();
            Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
            func_8002DF54(play, NULL, 8);
            if (!(gSaveContext.eventChkInf[3] & 0x100)) {
                this->actionFunc = func_809EE96C;
            } else {
                this->actionFunc = func_809EE048;
            }
            return true;
        }
    }
    return false;
}

// EnDivingGame_FinishMinigame ? // Reset probably
void func_809EDCB0(EnDivingGame* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gZoraIdleAnim);

    Animation_Change(&this->skelAnime, &gZoraIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->notPlayingMinigame = true;
    this->actionFunc = EnDivingGame_Talk;
}

void EnDivingGame_Talk(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->state != ENDIVINGGAME_STATE_PLAYING || !EnDivingGame_HasMinigameFinished(this, play)) {
        if (Actor_ProcessTalkRequest(&this->actor, play)) {
            if (this->unk_292 != TEXT_STATE_DONE) {
                switch (this->state) {
                    case ENDIVINGGAME_STATE_NOTPLAYING:
                        func_8002DF54(play, NULL, 8);
                        this->actionFunc = EnDivingGame_HandlePlayChoice;
                        break;
                    case ENDIVINGGAME_STATE_AWARDPRIZE:
                        this->actionFunc = func_809EEA00;
                        break;
                    case ENDIVINGGAME_STATE_PLAYING:
                        this->actionFunc = func_809EE8F0;
                        break;
                }
            }
        } else {
            if (Text_GetFaceReaction(play, 0x1D) != 0) {
                this->actor.textId = Text_GetFaceReaction(play, 0x1D);
                this->unk_292 = TEXT_STATE_DONE;
            } else {
                switch (this->state) {
                    case ENDIVINGGAME_STATE_NOTPLAYING:
                        this->unk_292 = TEXT_STATE_CHOICE;
                        if (!(gSaveContext.eventChkInf[3] & 0x100)) {
                            this->actor.textId = 0x4053;
                            this->phase = ENDIVINGGAME_PHASE_1;
                        } else {
                            this->actor.textId = 0x405C;
                            this->phase = ENDIVINGGAME_PHASE_2;
                        }
                        break;
                    case ENDIVINGGAME_STATE_AWARDPRIZE:
                        this->actor.textId = 0x4056;
                        this->unk_292 = TEXT_STATE_EVENT;
                        break;
                    case ENDIVINGGAME_STATE_PLAYING:
                        this->actor.textId = 0x405B;
                        this->unk_292 = TEXT_STATE_EVENT;
                        break;
                }
            }
            func_8002F2CC(&this->actor, play, 80.0f);
        }
    }
}

void EnDivingGame_HandlePlayChoice(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_292 == Message_GetState(&play->msgCtx) &&
        Message_ShouldAdvance(play)) { // Did player selected an answer?
        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                if (gSaveContext.rupees >= 20) {
                    Rupees_ChangeBy(-20);
                    this->actor.textId = 0x4054;
                } else {
                    this->actor.textId = 0x85;
                    this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
                }
                break;
            case 1: // No
                this->actor.textId = 0x2D;
                this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
                break;
        }
        if (!(gSaveContext.eventChkInf[3] & 0x100) || this->actor.textId == 0x85 || this->actor.textId == 0x2D) {
            Message_ContinueTextbox(play, this->actor.textId);
            this->unk_292 = TEXT_STATE_EVENT;
            this->actionFunc = func_809EE048;
        } else {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
            func_8002DF54(play, NULL, 8);
            this->actionFunc = func_809EE0FC;
        }
    }
}

// Waits for the message to close
void func_809EE048(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_292 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {
        if (this->phase == ENDIVINGGAME_PHASE_ENDED) {
            Message_CloseTextbox(play);
            func_8002DF54(play, NULL, 7);
            this->actionFunc = func_809EDCB0;
        } else {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
            func_8002DF54(play, NULL, 8);
            this->actionFunc = func_809EE0FC;
        }
    }
}

// another "start minigame" step
void func_809EE0FC(EnDivingGame* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gZoraThrowRupeesAnim);

    Animation_Change(&this->skelAnime, &gZoraThrowRupeesAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_ONCE, -10.0f);
    this->notPlayingMinigame = false;
    this->actionFunc = func_809EE194;
}

// Wait a bit before start throwing the rupees.
void func_809EE194(EnDivingGame* this, PlayState* play) {
    f32 currentFrame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (currentFrame >= 15.0f) {
        this->actionFunc = EnDivingGame_SetupRupeeThrow;
    }
}

void EnDivingGame_SetupRupeeThrow(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->subCamId = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, 0, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, this->subCamId, CAM_STAT_ACTIVE);
    this->spawnRuppyTimer = 10;
    this->unk_2F4.x = -210.0f;
    this->unk_2F4.y = -80.0f;
    this->unk_2F4.z = -1020.0f;
    this->unk_2D0.x = -280.0f;
    this->unk_2D0.y = -20.0f;
    this->unk_2D0.z = -240.0f;
    if (!(gSaveContext.eventChkInf[3] & 0x100)) {
        this->rupeesLeftToThrow = 5;
    } else {
        this->rupeesLeftToThrow = 10;
    }
    this->unk_2DC.x = this->unk_2DC.y = this->unk_2DC.z = this->unk_300.x = this->unk_300.y = this->unk_300.z = 0.1f;
    this->camLookAt.x = play->view.lookAt.x;
    this->camLookAt.y = play->view.lookAt.y;
    this->camLookAt.z = play->view.lookAt.z;
    this->camEye.x = play->view.eye.x;
    this->camEye.y = play->view.eye.y + 80.0f;
    this->camEye.z = play->view.eye.z + 250.0f;
    this->unk_2E8.x = fabsf(this->camEye.x - this->unk_2D0.x) * 0.04f;
    this->unk_2E8.y = fabsf(this->camEye.y - this->unk_2D0.y) * 0.04f;
    this->unk_2E8.z = fabsf(this->camEye.z - this->unk_2D0.z) * 0.04f;
    this->unk_30C.x = fabsf(this->camLookAt.x - this->unk_2F4.x) * 0.04f;
    this->unk_30C.y = fabsf(this->camLookAt.y - this->unk_2F4.y) * 0.04f;
    this->unk_30C.z = fabsf(this->camLookAt.z - this->unk_2F4.z) * 0.04f;
    Play_CameraSetAtEye(play, this->subCamId, &this->camLookAt, &this->camEye);
    Play_CameraSetFov(play, this->subCamId, play->mainCamera.fov);
    this->csCameraTimer = 60;
    this->actionFunc = EnDivingGame_RupeeThrow;
    this->unk_318 = 0.0f;
}

// Throws rupee when this->spawnRuppyTimer == 0
void EnDivingGame_RupeeThrow(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (func_800C0DB4(play, &this->actor.projectedPos)) {
        Audio_SetExtraFilter(0);
    }
    if (this->subCamId != 0) {
        Math_ApproachF(&this->camEye.x, this->unk_2D0.x, this->unk_2DC.x, this->unk_2E8.x * this->unk_318);
        Math_ApproachF(&this->camEye.z, this->unk_2D0.z, this->unk_2DC.z, this->unk_2E8.z * this->unk_318);
        Math_ApproachF(&this->camLookAt.x, this->unk_2F4.x, this->unk_300.x, this->unk_30C.x * this->unk_318);
        Math_ApproachF(&this->camLookAt.y, this->unk_2F4.y, this->unk_300.y, this->unk_30C.y * this->unk_318);
        Math_ApproachF(&this->camLookAt.z, this->unk_2F4.z, this->unk_300.z, this->unk_30C.z * this->unk_318);
        Math_ApproachF(&this->unk_318, 1.0f, 1.0f, 0.02f);
    }
    Play_CameraSetAtEye(play, this->subCamId, &this->camLookAt, &this->camEye);
    if (!this->allRupeesThrown && this->spawnRuppyTimer == 0) {
        this->spawnRuppyTimer = 5;
        EnDivingGame_SpawnRuppy(this, play);
        this->rupeesLeftToThrow--;
        if (!(gSaveContext.eventChkInf[3] & 0x100)) {
            this->unk_296 = 30;
        } else {
            this->unk_296 = 5;
        }
        if (this->rupeesLeftToThrow <= 0) {
            this->rupeesLeftToThrow = 0;
            this->allRupeesThrown = true;
        }
    }
    if (this->csCameraTimer == 0 ||
        ((fabsf(this->camEye.x - this->unk_2D0.x) < 2.0f) && (fabsf(this->camEye.y - this->unk_2D0.y) < 2.0f) &&
         (fabsf(this->camEye.z - this->unk_2D0.z) < 2.0f) && (fabsf(this->camLookAt.x - this->unk_2F4.x) < 2.0f) &&
         (fabsf(this->camLookAt.y - this->unk_2F4.y) < 2.0f) && (fabsf(this->camLookAt.z - this->unk_2F4.z) < 2.0f))) {
        if (this->unk_2A2 != 0) {
            this->csCameraTimer = 70;
            this->unk_2A2 = 2;
            this->actionFunc = func_809EE780;
        } else {
            this->actionFunc = EnDivingGame_SetupUnderwaterViewCs;
        }
    }
}

// Called just before changing the camera to focus the underwater rupees.
void EnDivingGame_SetupUnderwaterViewCs(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_296 == 0) {
        this->unk_2A2 = 1;
        this->csCameraTimer = 100;
        this->actionFunc = EnDivingGame_RupeeThrow;
        this->camLookAt.x = this->unk_2F4.x = -210.0f;
        this->camLookAt.y = this->unk_2F4.y = -80.0f;
        this->camLookAt.z = this->unk_2F4.z = -1020.0f;
        this->camEye.x = this->unk_2D0.x = -280.0f;
        this->camEye.y = this->unk_2D0.y = -20.0f;
        this->camEye.z = this->unk_2D0.z = -240.0f;
    }
}

// EnDivingGame_SayStartAndWait ?
void func_809EE780(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->csCameraTimer == 0) {
        Play_ClearCamera(play, this->subCamId);
        Play_ChangeCameraStatus(play, 0, CAM_STAT_ACTIVE);
        this->actor.textId = 0x405A;
        Message_ContinueTextbox(play, this->actor.textId);
        this->unk_292 = TEXT_STATE_EVENT;
        this->actionFunc = func_809EE800;
    }
}

// EnDivingGame_TalkDuringMinigame
void func_809EE800(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_292 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (!(gSaveContext.eventChkInf[3] & 0x100)) {
            func_80088B34(BREG(2) + 50);
        } else {
            func_80088B34(BREG(2) + 50);
        }
        func_800F5ACC(NA_BGM_TIMED_MINI_GAME);
        func_8002DF54(play, NULL, 7);
        this->actor.textId = 0x405B;
        this->unk_292 = TEXT_STATE_EVENT;
        this->state = ENDIVINGGAME_STATE_PLAYING;
        this->actionFunc = EnDivingGame_Talk;
    }
}

void func_809EE8F0(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((this->unk_292 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play))) {
        Message_CloseTextbox(play);
        this->actionFunc = EnDivingGame_Talk;
    } else {
        EnDivingGame_HasMinigameFinished(this, play);
    }
}

// EnDivingGame_SayCongratsAndWait ? // EnDivingGame_PlayerWonPhase1
void func_809EE96C(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((this->unk_292 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play))) {
        Message_CloseTextbox(play);
        func_8002DF54(play, NULL, 7);
        this->actor.textId = 0x4056;
        this->unk_292 = TEXT_STATE_EVENT;
        this->state = ENDIVINGGAME_STATE_AWARDPRIZE;
        this->actionFunc = EnDivingGame_Talk;
    }
}

void func_809EEA00(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((this->unk_292 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play))) {
        Message_CloseTextbox(play);
        this->actor.parent = NULL;
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_SCALE_SILVER, 90.0f, 10.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_ZD_DIVING_MINIGAME, GI_SCALE_SILVER);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 90.0f, 10.0f);
        }
        this->actionFunc = func_809EEA90;
    }
}

void func_809EEA90(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_HasParent(&this->actor, play)) {
        this->actionFunc = func_809EEAF8;
    } else {
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_SCALE_SILVER, 90.0f, 10.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_ZD_DIVING_MINIGAME, GI_SCALE_SILVER);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 90.0f, 10.0f);
        }
    }
}

// Award the scale?
void func_809EEAF8(EnDivingGame* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(play)) {
        // "Successful completion"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        this->allRupeesThrown = this->state = this->phase = this->unk_2A2 = this->grabbedRupeesCounter = 0;
        gSaveContext.eventChkInf[3] |= 0x100;
        this->actionFunc = func_809EDCB0;
    }
}

void EnDivingGame_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDivingGame* this = (EnDivingGame*)thisx;
    Player* player = GET_PLAYER(play);
    Vec3f pos;

    if (this->csCameraTimer != 0) {
        this->csCameraTimer--;
    }
    if (this->unk_296 != 0) {
        this->unk_296--;
    }
    if (this->eyeTimer != 0) {
        this->eyeTimer--;
    }
    if (this->spawnRuppyTimer != 0) {
        this->spawnRuppyTimer--;
    }

    if (gSaveContext.timer1Value == 10) {
        func_800F5918();
    }
    if (this->eyeTimer == 0) {
        this->eyeTimer = 2;
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            this->eyeTexIndex = 0;
            this->eyeTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 80.0f);
    this->unk_324.unk_18 = player->actor.world.pos;
    this->unk_324.unk_18.y = player->actor.world.pos.y;
    func_80034A14(&this->actor, &this->unk_324, 2, 4);
    this->vec_284 = this->unk_324.unk_08;
    this->vec_28A = this->unk_324.unk_0E;
    if ((play->gameplayFrames % 16) == 0) {
        pos = this->actor.world.pos;
        pos.y += 20.0f;
        EffectSsGRipple_Spawn(play, &pos, 100, 500, 30);
    }
    this->unk_290++;
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f, 29);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

Gfx* EnDivingGame_EmptyDList(GraphicsContext* gfxCtx) {
    Gfx* displayList = Graph_Alloc(gfxCtx, sizeof(Gfx));

    gSPEndDisplayList(displayList);
    return displayList;
}

s32 EnDivingGame_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    EnDivingGame* this = (EnDivingGame*)thisx;
    s32 pad;

    if (limbIndex == 6) {
        rot->x += this->vec_28A.y;
    }

    if (limbIndex == 15) {
        rot->x += this->vec_284.y;
        rot->z += this->vec_284.z;
    }

    if (this->notPlayingMinigame && (limbIndex == 8 || limbIndex == 9 || limbIndex == 12)) {
        rot->y += Math_SinS((play->state.frames * (limbIndex * 50 + 0x814))) * 200.0f;
        rot->z += Math_CosS((play->state.frames * (limbIndex * 50 + 0x940))) * 200.0f;
    }

    return 0;
}

void EnDivingGame_Draw(Actor* thisx, PlayState* play) {
    EnDivingGame* this = (EnDivingGame*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, EnDivingGame_EmptyDList(play->state.gfxCtx));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeTexIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDivingGame_OverrideLimbDraw, NULL, this);
    CLOSE_DISPS(play->state.gfxCtx);
}

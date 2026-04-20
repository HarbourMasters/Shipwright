/*
 * File: z_en_ta.c
 * Overlay: ovl_En_Ta
 * Description: Talon
 */

#include "z_en_ta.h"
#include "vt.h"
#include "objects/object_ta/object_ta.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnTa_Init(Actor* thisx, PlayState* play);
void EnTa_Destroy(Actor* thisx, PlayState* play);
void EnTa_Update(Actor* thisx, PlayState* play);
void EnTa_Draw(Actor* thisx, PlayState* play);

void EnTa_IdleAsleepInCastle(EnTa* this, PlayState* play);
void EnTa_IdleAsleepInLonLonHouse(EnTa* this, PlayState* play);
void EnTa_IdleAsleepInKakariko(EnTa* this, PlayState* play);
void EnTa_IdleAwakeInCastle(EnTa* this, PlayState* play);
void EnTa_IdleAwakeInKakariko(EnTa* this, PlayState* play);
void EnTa_IdleAtRanch(EnTa* this, PlayState* play);
void EnTa_RunCuccoGame(EnTa* this, PlayState* play);
void EnTa_IdleSittingInLonLonHouse(EnTa* this, PlayState* play);
void EnTa_IdleAfterCuccoGameFinished(EnTa* this, PlayState* play);
void EnTa_BlinkWaitUntilNext(EnTa* this);
void EnTa_BlinkAdvanceState(EnTa* this);
void EnTa_AnimRepeatCurrent(EnTa* this);
void EnTa_AnimSleeping(EnTa* this);
void EnTa_AnimSitSleeping(EnTa* this);
void EnTa_AnimRunToEnd(EnTa* this);

const ActorInit En_Ta_InitVars = {
    ACTOR_EN_TA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TA,
    sizeof(EnTa),
    (ActorFunc)EnTa_Init,
    (ActorFunc)EnTa_Destroy,
    (ActorFunc)EnTa_Update,
    (ActorFunc)EnTa_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000004, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

void EnTa_SetupAction(EnTa* this, EnTaActionFunc arg1, EnTaUnkFunc arg2) {
    this->actionFunc = arg1;
    this->animFunc = arg2;
}

void EnTa_SetTextForTalkInLonLonHouse(EnTa* this, PlayState* play) {
    u16 faceReaction = Text_GetFaceReaction(play, 24);

    if (gSaveContext.eventInf[0] & 0x400) {
        if (gSaveContext.eventInf[0] & 0x100) {
            if (Flags_GetItemGetInf(ITEMGETINF_TALON_BOTTLE)) {
                this->actor.textId = 0x2088;
            } else {
                this->actor.textId = 0x2086;
            }
        } else {
            this->actor.textId = 0x2085;
        }
        gSaveContext.eventInf[0] &= ~0x100;
    } else if (faceReaction == 0) {
        if (Flags_GetInfTable(INFTABLE_TALKED_TO_TALON_IN_RANCH_HOUSE)) {
            if (Flags_GetItemGetInf(ITEMGETINF_TALON_BOTTLE)) {
                this->actor.textId = 0x208B;
            } else {
                this->actor.textId = 0x207F;
            }
        } else {
            this->actor.textId = 0x207E;
        }
    } else {
        this->actor.textId = faceReaction;
    }
}

void EnTa_Init(Actor* thisx, PlayState* play2) {
    EnTa* this = (EnTa*)thisx;
    PlayState* play = play2;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gTalonSkel, &gTalonStandAnim, this->jointTable, this->morphTable, 17);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->stateFlags = 0;
    this->rapidBlinks = 0;
    this->nodOffTimer = 0;
    this->blinkTimer = 20;
    this->blinkFunc = EnTa_BlinkWaitUntilNext;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.targetMode = 6;
    this->actor.velocity.y = -4.0f;
    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;

    switch (this->actor.params) {
        case 1:
            osSyncPrintf(VT_FGCOL(CYAN) " 追放タロン \n" VT_RST);
            if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO)) {
                Actor_Kill(&this->actor);
            } else if (!LINK_IS_ADULT) {
                Actor_Kill(&this->actor);
            } else if (Flags_GetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO)) {
                EnTa_SetupAction(this, EnTa_IdleAwakeInKakariko, EnTa_AnimRepeatCurrent);
                this->eyeIndex = 0;
                Animation_PlayOnce(&this->skelAnime, &gTalonStandAnim);
                this->currentAnimation = &gTalonStandAnim;
            } else {
                EnTa_SetupAction(this, EnTa_IdleAsleepInKakariko, EnTa_AnimSleeping);
                this->eyeIndex = 2;
                Animation_PlayOnce(&this->skelAnime, &gTalonSleepAnim);
                this->currentAnimation = &gTalonSleepAnim;
                this->actor.shape.shadowScale = 54.0f;
            }
            break;
        case 2:
            osSyncPrintf(VT_FGCOL(CYAN) " 出戻りタロン \n" VT_RST);
            if (!Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO)) {
                Actor_Kill(&this->actor);
            } else if (!LINK_IS_ADULT) {
                Actor_Kill(&this->actor);
            } else if (play->sceneNum == SCENE_STABLE && !IS_DAY) {
                Actor_Kill(&this->actor);
                osSyncPrintf(VT_FGCOL(CYAN) " 夜はいない \n" VT_RST);
            } else {
                EnTa_SetupAction(this, EnTa_IdleAtRanch, EnTa_AnimRepeatCurrent);
                this->eyeIndex = 0;
                Animation_PlayOnce(&this->skelAnime, &gTalonStandAnim);
                this->currentAnimation = &gTalonStandAnim;
            }
            break;
        default:
            osSyncPrintf(VT_FGCOL(CYAN) " その他のタロン \n" VT_RST);
            if (play->sceneNum == SCENE_HYRULE_CASTLE) {
                if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                    Actor_Kill(&this->actor);
                } else if (Flags_GetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_CASTLE)) {
                    EnTa_SetupAction(this, EnTa_IdleAwakeInCastle, EnTa_AnimRepeatCurrent);
                    this->eyeIndex = 0;
                    Animation_PlayOnce(&this->skelAnime, &gTalonStandAnim);
                    this->currentAnimation = &gTalonStandAnim;
                } else {
                    EnTa_SetupAction(this, EnTa_IdleAsleepInCastle, EnTa_AnimSleeping);
                    this->eyeIndex = 2;
                    Animation_PlayOnce(&this->skelAnime, &gTalonSleepAnim);
                    this->currentAnimation = &gTalonSleepAnim;
                    this->actor.shape.shadowScale = 54.0f;
                }
            } else if (play->sceneNum == SCENE_LON_LON_BUILDINGS) {
                osSyncPrintf(VT_FGCOL(CYAN) " ロンロン牧場の倉庫 の タロン\n" VT_RST);
                if (!Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                    Actor_Kill(&this->actor);
                } else if (LINK_IS_ADULT) {
                    Actor_Kill(&this->actor);
                } else {
                    if (IS_DAY) {
                        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                        this->superCuccoTimers[0] = this->superCuccoTimers[1] = this->superCuccoTimers[2] = 7;
                        this->superCuccos[0] = (EnNiw*)Actor_Spawn(
                            &play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x + 5.0f,
                            this->actor.world.pos.y + 3.0f, this->actor.world.pos.z + 26.0f, 0, 0, 0, 0xD);
                        this->superCuccos[1] = (EnNiw*)Actor_Spawn(
                            &play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x - 20.0f,
                            this->actor.world.pos.y + 40.0f, this->actor.world.pos.z - 30.0f, 0, 0, 0, 0xD);
                        this->superCuccos[2] = (EnNiw*)Actor_Spawn(
                            &play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x + 20.0f,
                            this->actor.world.pos.y + 40.0f, this->actor.world.pos.z - 30.0f, 0, 0, 0, 0xD);
                        EnTa_SetTextForTalkInLonLonHouse(this, play);

                        if (gSaveContext.eventInf[0] & 0x400) {
                            EnTa_SetupAction(this, EnTa_IdleAfterCuccoGameFinished, EnTa_AnimRunToEnd);
                            Animation_Change(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                                             Animation_GetLastFrame(&gTalonSitWakeUpAnim) - 1.0f,
                                             Animation_GetLastFrame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 0.0f);
                            gSaveContext.eventInf[0] &= ~0x400;
                        } else {
                            EnTa_SetupAction(this, EnTa_IdleSittingInLonLonHouse, EnTa_AnimSitSleeping);
                            this->eyeIndex = 0;
                            Animation_PlayOnce(&this->skelAnime, &gTalonSitSleepingAnim);
                            this->currentAnimation = &gTalonSitSleepingAnim;
                        }
                    } else {
                        EnTa_SetupAction(this, EnTa_IdleAsleepInLonLonHouse, EnTa_AnimSleeping);
                        this->eyeIndex = 2;
                        Animation_PlayOnce(&this->skelAnime, &gTalonSleepAnim);
                        this->currentAnimation = &gTalonSleepAnim;
                        this->actor.shape.shadowScale = 54.0f;
                    }
                }
            } else {
                EnTa_SetupAction(this, EnTa_IdleAsleepInCastle, EnTa_AnimSleeping);
                this->eyeIndex = 2;
                Animation_PlayOnce(&this->skelAnime, &gTalonSleepAnim);
                this->currentAnimation = &gTalonSleepAnim;
                this->actor.shape.shadowScale = 54.0f;
            }
            break;
    }
}

void EnTa_DecreaseShadowSize(EnTa* this) {
    if (this->actor.shape.shadowScale > 36.0f) {
        this->actor.shape.shadowScale -= 0.8f;
    }
}

void EnTa_Destroy(Actor* thisx, PlayState* play) {
    EnTa* this = (EnTa*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    if (this->actor.params != 1 && this->actor.params != 2 && play->sceneNum == SCENE_LON_LON_BUILDINGS) {
        gSaveContext.timerState = TIMER_STATE_OFF;
    }

    if (this->stateFlags & 0x200) {
        func_800F5B58();
    }

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

s32 EnTa_RequestTalk(EnTa* this, PlayState* play, u16 textId) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        return true;
    }

    this->actor.textId = textId;

    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) &&
        (this->actor.xzDistToPlayer < 100.0f)) {
        this->stateFlags |= 1;
        func_8002F2CC(&this->actor, play, 100.0f);
    }
    return false;
}

void EnTa_SleepTalkInKakariko(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupAction(this, EnTa_IdleAsleepInKakariko, EnTa_AnimSleeping);
    }
}

void EnTa_SleepTalkInLonLonHouse(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupAction(this, EnTa_IdleAsleepInLonLonHouse, EnTa_AnimSleeping);
    }
}

void EnTa_SetupAwake(EnTa* this) {
    if (!LINK_IS_ADULT) {
        EnTa_SetupAction(this, EnTa_IdleAwakeInCastle, EnTa_AnimRepeatCurrent);
        Flags_SetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_CASTLE);
    } else {
        EnTa_SetupAction(this, EnTa_IdleAwakeInKakariko, EnTa_AnimRepeatCurrent);
        Flags_SetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO);
    }
}

void EnTa_TalkWakingUp2(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupAwake(this);
    }
    EnTa_DecreaseShadowSize(this);
    this->stateFlags |= 0x4;
}

void EnTa_TalkWakingUp1(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupAwake(this);
        this->blinkTimer = 1;
        this->blinkFunc = EnTa_BlinkAdvanceState;
    }

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) {
        this->eyeIndex = 1;
        EnTa_SetupAction(this, EnTa_TalkWakingUp2, EnTa_AnimRepeatCurrent);
    }
    EnTa_DecreaseShadowSize(this);
    this->stateFlags |= 4;
}

void EnTa_WakeUp(EnTa* this, PlayState* play) {
    this->stateFlags |= 4;

    if (this->timer == 0) {
        EnTa_SetupAction(this, EnTa_TalkWakingUp1, EnTa_AnimRepeatCurrent);
        this->rapidBlinks = 3;
        this->timer = 60;
        Animation_PlayOnce(&this->skelAnime, &gTalonWakeUpAnim);
        this->currentAnimation = &gTalonStandAnim;
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_TA_SURPRISE);
    }
}

void EnTa_SleepTalkInCastle(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupAction(this, EnTa_IdleAsleepInCastle, EnTa_AnimSleeping);
    }
}

void EnTa_IdleAsleepInCastle(EnTa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        s32 exchangeItemId = func_8002F368(play);

        switch (exchangeItemId) {
            case EXCH_ITEM_CHICKEN:
                player->actor.textId = 0x702B;
                EnTa_SetupAction(this, EnTa_WakeUp, EnTa_AnimRepeatCurrent);
                this->timer = 40;
                break;
            default:
                if (exchangeItemId != EXCH_ITEM_NONE) {
                    player->actor.textId = 0x702A;
                }
                EnTa_SetupAction(this, EnTa_SleepTalkInCastle, EnTa_AnimSleeping);
                break;
        }
    } else {
        this->actor.textId = 0x702A;
        func_8002F298(&this->actor, play, 100.0f, 3);
    }
}

void EnTa_IdleAsleepInLonLonHouse(EnTa* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        EnTa_SetupAction(this, EnTa_SleepTalkInLonLonHouse, EnTa_AnimSleeping);
    }
    this->actor.textId = 0x204B;
    func_8002F2CC(&this->actor, play, 100.0f);
}

void EnTa_IdleAsleepInKakariko(EnTa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        s32 exchangeItemId = func_8002F368(play);

        switch (exchangeItemId) {
            case EXCH_ITEM_POCKET_CUCCO:
                player->actor.textId = 0x702B;
                EnTa_SetupAction(this, EnTa_WakeUp, EnTa_AnimRepeatCurrent);
                this->timer = 40;
                break;
            default:
                if (exchangeItemId != EXCH_ITEM_NONE) {
                    player->actor.textId = 0x5015;
                }
                EnTa_SetupAction(this, EnTa_SleepTalkInKakariko, EnTa_AnimSleeping);
                break;
        }
    } else {
        this->actor.textId = 0x5015;
        func_8002F298(&this->actor, play, 100.0f, 6);
    }
}

void EnTa_RunWithAccelerationAndSfx(EnTa* this, PlayState* play) {
    s32 framesMod12 = (s32)play->state.frames % 12;

    if (framesMod12 == 0 || framesMod12 == 6) {
        Audio_PlayActorSound2(&this->actor, NA_SE_PL_WALK_GROUND);
    }
    if (this->actor.speedXZ < 6.0f) {
        this->actor.speedXZ += 0.4f;
    }
    Actor_MoveXZGravity(&this->actor);
}

void EnTa_RunAwayRunOutOfGate(EnTa* this, PlayState* play) {
    func_80033480(play, &this->actor.world.pos, 50.0f, 2, 250, 20, 1);
    EnTa_RunWithAccelerationAndSfx(this, play);

    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnTa_RunAwayTurnTowardsGate(EnTa* this, PlayState* play) {
    this->actor.world.rot.y += 0xC00;
    this->actor.shape.rot.y += 0xC00;

    if (this->timer == 0) {
        EnTa_SetupAction(this, EnTa_RunAwayRunOutOfGate, EnTa_AnimRepeatCurrent);
        this->timer = 60;
    }
}

void EnTa_RunAwayRunWest(EnTa* this, PlayState* play) {
    func_80033480(play, &this->actor.world.pos, 50.0f, 2, 250, 20, 1);
    EnTa_RunWithAccelerationAndSfx(this, play);

    if (this->timer == 0) {
        EnTa_SetupAction(this, EnTa_RunAwayTurnTowardsGate, EnTa_AnimRepeatCurrent);
        this->timer = 5;
    }
}

void EnTa_RunAwayTurnWest(EnTa* this, PlayState* play) {
    this->actor.world.rot.y -= 0xD00;
    this->actor.shape.rot.y -= 0xD00;

    if (this->timer == 0) {
        EnTa_SetupAction(this, EnTa_RunAwayRunWest, EnTa_AnimRepeatCurrent);
        this->timer = 65;
    }
}

void EnTa_RunAwayRunSouth(EnTa* this, PlayState* play) {
    func_80033480(play, &this->actor.world.pos, 50.0f, 2, 250, 20, 1);
    EnTa_RunWithAccelerationAndSfx(this, play);

    if (this->timer == 20) {
        Message_CloseTextbox(play);
    }
    if (this->timer == 0) {
        this->timer = 5;
        EnTa_SetupAction(this, EnTa_RunAwayTurnWest, EnTa_AnimRepeatCurrent);
    }
}

void EnTa_RunAwayStart(EnTa* this, PlayState* play) {
    this->actor.world.rot.y -= 0xC00;
    this->actor.shape.rot.y -= 0xC00;

    if (this->timer == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_TA_CRY_1);
        EnTa_SetupAction(this, EnTa_RunAwayRunSouth, EnTa_AnimRepeatCurrent);
        this->timer = 65;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

void EnTa_TalkAwakeInCastle(EnTa* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) {
        s16 csCamIdx = OnePointCutscene_Init(play, 4175, -99, &this->actor, MAIN_CAM);
        EnTa_SetupAction(this, EnTa_RunAwayStart, EnTa_AnimRepeatCurrent);
        this->timer = 5;
        Flags_SetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE);
        if (GameInteractor_Should(VB_PLAY_ONEPOINT_ACTOR_CS, true, this)) {
            OnePointCutscene_EndCutscene(play, csCamIdx);
        }
        Animation_PlayOnce(&this->skelAnime, &gTalonRunTransitionAnim);
        this->currentAnimation = &gTalonRunAnim;
    }
    this->stateFlags |= 1;
}

void EnTa_IdleAwakeInCastle(EnTa* this, PlayState* play) {
    if (EnTa_RequestTalk(this, play, 0x702C)) {
        EnTa_SetupAction(this, EnTa_TalkAwakeInCastle, EnTa_AnimRepeatCurrent);
    }
    EnTa_DecreaseShadowSize(this);
}

void EnTa_TalkAwakeInKakariko(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupAction(this, EnTa_IdleAwakeInKakariko, EnTa_AnimRepeatCurrent);
    }
    this->stateFlags |= 1;
}

void EnTa_IdleAwakeInKakariko(EnTa* this, PlayState* play) {
    if (Flags_GetEventChkInf(EVENTCHKINF_EPONA_OBTAINED)) {
        if (EnTa_RequestTalk(this, play, 0x5017)) {
            EnTa_SetupAction(this, EnTa_TalkAwakeInKakariko, EnTa_AnimRepeatCurrent);
            Flags_SetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO);
        }
    } else if (EnTa_RequestTalk(this, play, 0x5016)) {
        EnTa_SetupAction(this, EnTa_TalkAwakeInKakariko, EnTa_AnimRepeatCurrent);
    }
    EnTa_DecreaseShadowSize(this);
}

void EnTa_TalkAtRanch(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupAction(this, EnTa_IdleAtRanch, EnTa_AnimRepeatCurrent);
    }
    this->stateFlags |= 1;
}

void EnTa_IdleAtRanch(EnTa* this, PlayState* play) {
    if (EnTa_RequestTalk(this, play, 0x2055)) {
        EnTa_SetupAction(this, EnTa_TalkAtRanch, EnTa_AnimRepeatCurrent);
    }
}

s32 EnTa_CheckCanBuyMilk(void) {
    if (gSaveContext.rupees < 30) {
        return 0;
    } else if (!Inventory_HasEmptyBottle()) {
        return 1;
    } else {
        return 2;
    }
}

void EnTa_CreateFloorCamera(EnTa* this, PlayState* play) {
    Vec3f b;
    Vec3f a;

    this->subCamId = Play_CreateSubCamera(play);
    this->returnToCamId = play->activeCamera;
    Play_ChangeCameraStatus(play, this->returnToCamId, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, this->subCamId, CAM_STAT_ACTIVE);

    b.x = 1053.0f;
    b.y = 11.0f;
    b.z = 22.0f;

    a.x = 1053.0f;
    a.y = 45.0f;
    a.z = -40.0f;

    Play_CameraSetAtEye(play, this->subCamId, &a, &b);
}

void EnTa_RemoveFloorCamera(EnTa* this, PlayState* play) {
    Play_ChangeCameraStatus(play, this->returnToCamId, CAM_STAT_ACTIVE);
    Play_ClearCamera(play, this->subCamId);
}

void EnTa_SetupActionWithSleepAnimation(EnTa* this, EnTaActionFunc arg1) {
    EnTa_SetupAction(this, arg1, EnTa_AnimSitSleeping);
    this->eyeIndex = 2;
    Animation_Change(&this->skelAnime, &gTalonSitSleepingAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gTalonSitSleepingAnim), ANIMMODE_ONCE, -5.0f);
    this->nodOffTimer = 0;
    this->currentAnimation = &gTalonSitSleepingAnim;
}

void EnTa_SetupActionWithWakeUpAnimation(EnTa* this, EnTaActionFunc arg1) {
    this->eyeIndex = 1;
    EnTa_SetupAction(this, arg1, EnTa_AnimRunToEnd);
    this->stateFlags &= ~0x10;
    Animation_Change(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gTalonSitWakeUpAnim),
                     ANIMMODE_ONCE, -5.0f);
}

void EnTa_TalkNotEnoughRupees(EnTa* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        EnTa_SetupActionWithSleepAnimation(this, EnTa_IdleSittingInLonLonHouse);
        EnTa_SetTextForTalkInLonLonHouse(this, play);
    }
    this->stateFlags |= 1;
}

s32 EnTa_IsPlayerHoldingSuperCucco(EnTa* this, PlayState* play, s32 idx) {
    Player* player = GET_PLAYER(play);
    Actor* interactRangeActor;

    if (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        interactRangeActor = player->interactRangeActor;
        if (interactRangeActor != NULL && interactRangeActor->id == ACTOR_EN_NIW &&
            interactRangeActor == &this->superCuccos[idx]->actor) {
            return true;
        }
    }
    return false;
}

void EnTa_TalkFoundSuperCucco(EnTa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        s32 unk_2CA;

        Animation_Change(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim) - 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 10.0f);
        this->stateFlags &= ~0x10;
        Message_CloseTextbox(play);
        unk_2CA = this->lastFoundSuperCuccoIdx;
        this->actionFunc = EnTa_RunCuccoGame;
        this->superCuccos[unk_2CA]->actor.gravity = 0.1f;
        this->superCuccos[unk_2CA]->actor.velocity.y = 0.0f;
        this->superCuccos[unk_2CA]->actor.speedXZ = 0.0f;
        this->superCuccos[unk_2CA]->actor.parent = NULL;

        if (player->interactRangeActor == &this->superCuccos[unk_2CA]->actor) {
            player->interactRangeActor = NULL;
        }
        if (player->heldActor == &this->superCuccos[unk_2CA]->actor) {
            player->heldActor = NULL;
        }
        player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        this->superCuccos[unk_2CA] = NULL;
    }
    this->stateFlags |= 1;
}

void EnTa_IdleFoundSuperCucco(EnTa* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = EnTa_TalkFoundSuperCucco;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        func_8002F2CC(&this->actor, play, 1000.0f);
    }
    this->stateFlags |= 1;
}

s32 EnTa_GetSuperCuccosCount(EnTa* this, PlayState* play) {
    s32 count;
    s32 i;

    for (count = 0, i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
        if (this->superCuccos[i] != NULL) {
            count++;
        }
    }
    return count;
}

void EnTa_AnimateHandsUpDown(EnTa* this) {
    if (this->stateFlags & 0x10) {
        if (this->stateFlags & 0x100) {
            Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 17.0f, 22.0f, ANIMMODE_ONCE, 0.0f);
            this->stateFlags &= ~0x100;
        } else {
            Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, -1.0f, 21.0f, 16.0f, ANIMMODE_ONCE, 3.0f);
            this->stateFlags |= 0x100;
        }
        this->stateFlags &= ~0x10;
    }
}

void EnTa_TransitionToPostCuccoGame(EnTa* this, PlayState* play) {
    EnTa_AnimateHandsUpDown(this);

    if (this->timer == 0) {
        if (this->stateFlags & 0x80) {
            this->stateFlags &= ~0x80;
            EnTa_RemoveFloorCamera(this, play);
        }
    }
}

void EnTa_TalkCuccoGameEnd(EnTa* this, PlayState* play) {
    EnTa_AnimateHandsUpDown(this);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->nextEntranceIndex = ENTR_LON_LON_BUILDINGS_2;

        if (gSaveContext.eventInf[0] & 0x100) {
            play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        } else {
            play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
        }

        play->transitionTrigger = TRANS_TRIGGER_START;
        gSaveContext.eventInf[0] |= 0x400;
        this->actionFunc = EnTa_TransitionToPostCuccoGame;
        this->timer = 22;
    }
}

void EnTa_RunCuccoGame(EnTa* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
        if (this->superCuccos[i] != NULL) {
            if (this->superCuccos[i]->actor.gravity > -2.0f) {
                this->superCuccos[i]->actor.gravity -= 0.03f;
            }

            if (!GameInteractor_Should(VB_PREVENT_STRENGTH, !EnTa_IsPlayerHoldingSuperCucco(this, play, i))) {
                if (this->superCuccoTimers[i] > 0) {
                    this->superCuccoTimers[i]--;
                } else {
                    this->lastFoundSuperCuccoIdx = i;
                    Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
                    this->stateFlags &= ~0x10;

                    switch (EnTa_GetSuperCuccosCount(this, play)) {
                        case 1:
                            gSaveContext.timerState = TIMER_STATE_OFF;
                            Player_SetCsActionWithHaltedActors(play, &this->actor, 1);

                            Message_StartTextbox(play, 0x2084, &this->actor);
                            this->actionFunc = EnTa_TalkCuccoGameEnd;
                            Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE,
                                             -10.0f);
                            this->stateFlags &= ~0x10;
                            this->stateFlags &= ~0x100;
                            gSaveContext.eventInf[0] |= 0x100;
                            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
                            this->stateFlags &= ~0x200;
                            Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
                            return;
                        case 2:
                            this->actor.textId = 0x2083;
                            Audio_PlayActorSound2(&this->actor, NA_SE_VO_TA_CRY_1);
                            break;
                        case 3:
                            this->actor.textId = 0x2082;
                            Audio_PlayActorSound2(&this->actor, NA_SE_VO_TA_SURPRISE);
                            break;
                    }
                    this->actionFunc = EnTa_IdleFoundSuperCucco;
                    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                    func_8002F2CC(&this->actor, play, 1000.0f);
                    return;
                }
            } else {
                this->superCuccoTimers[i] = 7;
            }
        }
    }

    if (gSaveContext.timerSeconds == 10) {
        func_800F5918();
    }

    if (gSaveContext.timerSeconds == 0 && !Play_InCsMode(play)) {
        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
        this->stateFlags &= ~0x200;
        Sfx_PlaySfxCentered(NA_SE_SY_FOUND);
        gSaveContext.timerState = TIMER_STATE_OFF;
        Player_SetCsActionWithHaltedActors(play, &this->actor, 1);
        Message_StartTextbox(play, 0x2081, &this->actor);
        this->actionFunc = EnTa_TalkCuccoGameEnd;
        EnTa_CreateFloorCamera(this, play);
        gSaveContext.eventInf[0] &= ~0x100;
        this->stateFlags |= 0x80;
        Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
        this->stateFlags &= ~0x10;
        this->stateFlags &= ~0x100;
    }

    this->stateFlags |= 1;
}

void EnTa_ThrowSuperCuccos(EnTa* this, PlayState* play) {
    s32 i;

    if (this->timer > 35) {
        for (i = 1; i < ARRAY_COUNT(this->superCuccos); i++) {
            if (this->superCuccos[i] != NULL) {
                Math_SmoothStepToS(&this->superCuccos[i]->actor.world.rot.y, i * -10000 - 3000, 2, 0x800, 0x100);
                this->superCuccos[i]->actor.shape.rot.y = this->superCuccos[i]->actor.world.rot.y;
            }
        }
    } else if (this->timer == 35) {
        for (i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
            this->superCuccoTimers[i] = (s32)(Rand_CenteredFloat(6.0f) + 10.0f);

            if (this->superCuccos[i] != NULL) {
                EnNiw* niw = this->superCuccos[i];

                niw->unk_308 = 1;
                niw->actor.gravity = 0.0f;
            }
        }
    } else {
        for (i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
            if (this->timer < 35 - this->superCuccoTimers[i]) {
                if (this->superCuccos[i] != NULL) {
                    if (this->superCuccos[i]->actor.gravity > -2.0f) {
                        this->superCuccos[i]->actor.gravity -= 0.03f;
                    }
                }
            }
        }
    }

    if (this->timer == 0) {
        EnTa_SetupAction(this, EnTa_RunCuccoGame, EnTa_AnimRunToEnd);
        this->stateFlags &= ~0x10;
        Animation_Change(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim) - 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 10.0f);
        Player_SetCsActionWithHaltedActors(play, &this->actor, 7);
    }
}

void EnTa_StartingCuccoGame3(EnTa* this, PlayState* play) {
    if (this->timer == 0 && this->stateFlags & 0x20) {
        EnTa_SetupAction(this, EnTa_ThrowSuperCuccos, EnTa_AnimRunToEnd);
        this->stateFlags &= ~0x10;
        Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 1.0f,
                         Animation_GetLastFrame(&gTalonSitHandsUpAnim), ANIMMODE_ONCE, 0.0f);
        this->timer = 50;
        Interface_SetTimer(0x1E);
        func_800F5ACC(NA_BGM_TIMED_MINI_GAME);
        this->stateFlags |= 0x200;
        Message_CloseTextbox(play);
        Player_SetCsActionWithHaltedActors(play, &this->actor, 1);
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->stateFlags |= 0x20;
    }

    this->stateFlags |= 1;
}

void EnTa_StartingCuccoGame2(EnTa* this, PlayState* play) {
    if (this->stateFlags & 0x10) {
        EnTa_SetupAction(this, EnTa_StartingCuccoGame3, EnTa_AnimRunToEnd);
        this->stateFlags &= ~0x10;
        Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 0.0f, 1.0f, ANIMMODE_ONCE, 0.0f);
        this->timer = 5;
    }
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->stateFlags |= 0x20;
    }
    this->stateFlags |= 1;
}

void EnTa_StartingCuccoGame1(EnTa* this, PlayState* play) {
    if (this->stateFlags & 0x10) {
        EnTa_SetupAction(this, EnTa_StartingCuccoGame2, EnTa_AnimRunToEnd);
        this->stateFlags &= ~0x10;
        Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, -1.0f, 29.0f, 0.0f, ANIMMODE_ONCE, 10.0f);
    }
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->stateFlags |= 0x20;
    }
    this->stateFlags |= 1;
}

void EnTa_StartCuccoGame(EnTa* this, PlayState* play) {
    EnTa_SetupAction(this, EnTa_StartingCuccoGame1, EnTa_AnimRunToEnd);
    this->stateFlags &= ~0x10;
    Animation_Change(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
    Message_ContinueTextbox(play, 0x2080);
    this->stateFlags &= ~0x20;
}

void EnTa_TalkGeneralInLonLonHouse(EnTa* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnTa_SetupActionWithSleepAnimation(this, EnTa_IdleSittingInLonLonHouse);
        EnTa_SetTextForTalkInLonLonHouse(this, play);
    }
    this->stateFlags |= 1;
}

void EnTa_GiveItemInLonLonHouse(EnTa* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play) ||
        !GameInteractor_Should(VB_GIVE_ITEM_FROM_TALONS_CHICKENS, true, &this->actor)) {
        this->actor.parent = NULL;
        this->actionFunc = EnTa_TalkGeneralInLonLonHouse;
        if (!(this->stateFlags & 0x2)) {
            Flags_SetItemGetInf(ITEMGETINF_TALON_BOTTLE);
        }
        this->stateFlags &= ~0x2;
    } else if (this->stateFlags & 2) {
        Actor_OfferGetItem(&this->actor, play, GI_MILK, 10000.0f, 50.0f);
    } else {
        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_TALONS_CHICKENS, true, &this->actor)) {
            Actor_OfferGetItem(&this->actor, play, GI_MILK_BOTTLE, 10000.0f, 50.0f);
        }
    }
    this->stateFlags |= 1;
}

void EnTa_TalkAfterCuccoGameFirstWon(EnTa* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->stateFlags &= ~0x2;
        EnTa_SetupAction(this, EnTa_GiveItemInLonLonHouse, EnTa_AnimRunToEnd);
        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_TALONS_CHICKENS, true, &this->actor)) {
            Actor_OfferGetItem(&this->actor, play, GI_MILK_BOTTLE, 10000.0f, 50.0f);
        }
    }
}

void EnTa_WaitBuyMilkOrPlayCuccoGameResponse(EnTa* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                switch (EnTa_CheckCanBuyMilk()) {
                    case 0:
                        Message_ContinueTextbox(play, 0x85);
                        EnTa_SetupAction(this, EnTa_TalkNotEnoughRupees, EnTa_AnimRunToEnd);
                        break;
                    case 1:
                        Message_ContinueTextbox(play, 0x208A);
                        EnTa_SetupAction(this, EnTa_TalkGeneralInLonLonHouse, EnTa_AnimRunToEnd);
                        break;
                    case 2:
                        this->stateFlags |= 2;
                        EnTa_SetupAction(this, EnTa_GiveItemInLonLonHouse, EnTa_AnimRunToEnd);
                        Rupees_ChangeBy(-30);
                        GetItemEntry itemEntry = ItemTable_Retrieve(GI_MILK);
                        gSaveContext.ship.pendingSale = itemEntry.itemId;
                        gSaveContext.ship.pendingSaleMod = itemEntry.modIndex;
                        Actor_OfferGetItem(&this->actor, play, GI_MILK, 10000.0f, 50.0f);
                        break;
                }
                break;
            case 1:
                if (gSaveContext.rupees < 10) {
                    Message_ContinueTextbox(play, 0x85);
                    EnTa_SetupAction(this, EnTa_TalkNotEnoughRupees, EnTa_AnimRunToEnd);
                } else {
                    Rupees_ChangeBy(-10);
                    EnTa_StartCuccoGame(this, play);
                }
                break;
            case 2:
                EnTa_SetupActionWithSleepAnimation(this, EnTa_IdleSittingInLonLonHouse);
                EnTa_SetTextForTalkInLonLonHouse(this, play);
                break;
        }
    }

    if (this->stateFlags & 0x10) {
        this->stateFlags |= 1;
    }
}

void EnTa_WaitForPlayCuccoGameResponse(EnTa* this, PlayState* play) {
    s32 price;

    if (this->actor.textId == 0x2085) {
        price = 5;
    } else {
        price = 10;
    }

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.rupees < price) {
                    Message_ContinueTextbox(play, 0x85);
                    EnTa_SetupAction(this, EnTa_TalkNotEnoughRupees, EnTa_AnimRunToEnd);
                } else {
                    Rupees_ChangeBy(-price);
                    EnTa_StartCuccoGame(this, play);
                }
                break;
            case 1:
                EnTa_SetupActionWithSleepAnimation(this, EnTa_IdleSittingInLonLonHouse);
                EnTa_SetTextForTalkInLonLonHouse(this, play);
                break;
        }
    }

    if (this->stateFlags & 0x10) {
        this->stateFlags |= 1;
    }
}

void EnTa_WaitForMarryMalonResponse(EnTa* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, 0x2087);
        EnTa_SetupAction(this, EnTa_TalkAfterCuccoGameFirstWon, EnTa_AnimRunToEnd);
    }

    if (this->stateFlags & 0x10) {
        this->stateFlags |= 1;
    }
}

void EnTa_ContinueTalkInLonLonHouse(EnTa* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Flags_SetInfTable(INFTABLE_TALKED_TO_TALON_IN_RANCH_HOUSE);
        if (Flags_GetItemGetInf(ITEMGETINF_TALON_BOTTLE)) {
            Message_ContinueTextbox(play, 0x208B);
            EnTa_SetupAction(this, EnTa_WaitBuyMilkOrPlayCuccoGameResponse, EnTa_AnimRunToEnd);
        } else {
            Message_ContinueTextbox(play, 0x207F);
            EnTa_SetupAction(this, EnTa_WaitForPlayCuccoGameResponse, EnTa_AnimRunToEnd);
        }
    }

    if (this->stateFlags & 0x10) {
        this->stateFlags |= 1;
    }
}

void EnTa_TalkAfterCuccoGameWon(EnTa* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (Inventory_HasEmptyBottle()) {
            Message_CloseTextbox(play);
            this->stateFlags |= 2;
            EnTa_SetupAction(this, EnTa_GiveItemInLonLonHouse, EnTa_AnimRunToEnd);
            Actor_OfferGetItem(&this->actor, play, GI_MILK, 10000.0f, 50.0f);
        } else {
            Message_ContinueTextbox(play, 0x208A);
            EnTa_SetupAction(this, EnTa_TalkGeneralInLonLonHouse, EnTa_AnimRunToEnd);
        }
    }
}

void EnTa_IdleSittingInLonLonHouse(EnTa* this, PlayState* play) {
    u16 faceReaction = Text_GetFaceReaction(play, 0x18);

    EnTa_SetTextForTalkInLonLonHouse(this, play);

    if (EnTa_RequestTalk(this, play, this->actor.textId)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_TA_SURPRISE);

        if (faceReaction != 0) {
            EnTa_SetupActionWithWakeUpAnimation(this, EnTa_TalkGeneralInLonLonHouse);
        } else {
            Flags_SetInfTable(INFTABLE_TALKED_TO_TALON_IN_RANCH_HOUSE);

            switch (this->actor.textId) {
                case 0x207E:
                case 0x207F:
                    EnTa_SetupActionWithWakeUpAnimation(this, EnTa_WaitForPlayCuccoGameResponse);
                    break;
                case 0x208B:
                    EnTa_SetupActionWithWakeUpAnimation(this, EnTa_WaitBuyMilkOrPlayCuccoGameResponse);
                    break;
                default:
                    EnTa_SetupActionWithWakeUpAnimation(this, EnTa_ContinueTalkInLonLonHouse);
                    break;
            }
        }
    }
    this->stateFlags &= ~1;
}

void EnTa_IdleAfterCuccoGameFinished(EnTa* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        switch (this->actor.textId) {
            case 0x2085:
                this->actionFunc = EnTa_WaitForPlayCuccoGameResponse;
                break;
            case 0x2086:
                this->actionFunc = EnTa_WaitForMarryMalonResponse;
                break;
            case 0x2088:
                this->actionFunc = EnTa_TalkAfterCuccoGameWon;
                break;
        }
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        func_8002F2CC(&this->actor, play, 1000.0f);
    }
    this->stateFlags |= 1;
}

void EnTa_BlinkWaitUntilNext(EnTa* this) {
    s16 temp_v0 = this->blinkTimer - 1;

    if (temp_v0 != 0) {
        this->blinkTimer = temp_v0;
    } else {
        this->blinkFunc = EnTa_BlinkAdvanceState;
    }
}

void EnTa_BlinkAdvanceState(EnTa* this) {
    s16 blinkTimer = this->blinkTimer - 1;

    if (blinkTimer != 0) {
        this->blinkTimer = blinkTimer;
    } else {
        s16 nextEyeIndex = this->eyeIndex + 1;
        s16 blinkTimer = 3;

        if (nextEyeIndex >= blinkTimer) {
            this->eyeIndex = 0;
            if (this->rapidBlinks > 0) {
                this->rapidBlinks--;
                blinkTimer = 1;
            } else {
                blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
            }
            this->blinkTimer = blinkTimer;
            this->blinkFunc = EnTa_BlinkWaitUntilNext;
        } else {
            this->eyeIndex = nextEyeIndex;
            this->blinkTimer = 1;
        }
    }
}

void EnTa_AnimRepeatCurrent(EnTa* this) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayOnce(&this->skelAnime, this->currentAnimation);
    }
}

void EnTa_AnimSleeping(EnTa* this) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayOnce(&this->skelAnime, this->currentAnimation);
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_TA_SLEEP);
    }
    this->stateFlags |= 0xC;
}

void EnTa_AnimSitSleeping(EnTa* this) {
    if (this->nodOffTimer > 0) {
        this->nodOffTimer--;
    } else {
        if (SkelAnime_Update(&this->skelAnime)) {
            Animation_PlayOnce(&this->skelAnime, this->currentAnimation);
            this->nodOffTimer = Rand_ZeroFloat(100.0f) + 100.0f;
        }

        if (this->skelAnime.curFrame < 96.0f && this->skelAnime.curFrame >= 53.0f) {
            this->eyeIndex = 1;
        } else {
            this->eyeIndex = 2;
        }
        this->stateFlags |= 8;
    }
    this->stateFlags |= 4;
}

void EnTa_AnimRunToEnd(EnTa* this) {
    if (!(this->stateFlags & 0x10)) {
        if (SkelAnime_Update(&this->skelAnime)) {
            this->stateFlags |= 0x10;
        }
        this->stateFlags |= 8;
    }
}

void EnTa_Update(Actor* thisx, PlayState* play) {
    EnTa* this = (EnTa*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveXZGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    this->animFunc(this);
    this->actionFunc(this, play);

    if (!(this->stateFlags & 4)) {
        this->blinkFunc(this);
    }

    if (this->stateFlags & 1) {
        func_80038290(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 6200, 100);
    }

    this->stateFlags &= ~0x5;

    if (this->timer > 0) {
        this->timer--;
    }
}

s32 EnTa_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTa* this = (EnTa*)thisx;

    switch (limbIndex) {
        case 8:
            rot->x += this->torsoRot.y;
            rot->y -= this->torsoRot.x;
            break;
        case 15:
            rot->x += this->headRot.y;
            rot->z += this->headRot.x;
            break;
    }

    if (this->stateFlags & 0x8) {
        this->stateFlags &= ~0x8;
    } else if ((limbIndex == 8) || (limbIndex == 10) || (limbIndex == 13)) {
        s32 limbIdx50 = limbIndex * 50;

        rot->y += Math_SinS(play->state.frames * (limbIdx50 + 0x814)) * 200.0f;
        rot->z += Math_CosS(play->state.frames * (limbIdx50 + 0x940)) * 200.0f;
    }

    return false;
}

void EnTa_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80B16E7C = {
        1100.0f,
        1000.0f,
        0.0f,
    };
    EnTa* this = (EnTa*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&D_80B16E7C, &this->actor.focus.pos);
    }
}

void EnTa_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = {
        gTalonEyeOpenTex,
        gTalonEyeHalfTex,
        gTalonEyeClosedTex,
    };
    EnTa* this = (EnTa*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_37Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x9, SEGMENTED_TO_VIRTUAL(gTalonHeadSkinTex));

    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnTa_OverrideLimbDraw, EnTa_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}

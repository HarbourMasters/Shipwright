/*
 * File: z_en_ginko_man.c
 * Overlay: ovl_En_Ginko_Man
 * Description: Bank Teller
 */

#include "z_en_ginko_man.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnGinkoMan_Init(Actor* thisx, PlayState* play);
void EnGinkoMan_Destroy(Actor* thisx, PlayState* play);
void EnGinkoMan_Update(Actor* thisx, PlayState* play);
void EnGinkoMan_Draw(Actor* thisx, PlayState* play);

void EnGinkoMan_SetupIdle(EnGinkoMan* this);
void EnGinkoMan_SetupDialogue(EnGinkoMan* this);
void EnGinkoMan_SetupBankAward(EnGinkoMan* this);
void EnGinkoMan_SetupBankAward2(EnGinkoMan* this);
void EnGinkoMan_SetupStamp(EnGinkoMan* this);

void EnGinkoMan_Idle(EnGinkoMan* this, PlayState* play);
void EnGinkoMan_BankAward(EnGinkoMan* this, PlayState* play);
void EnGinkoMan_DepositDialogue(EnGinkoMan* this, PlayState* play);
void EnGinkoMan_BankAward2(EnGinkoMan* this, PlayState* play);
void EnGinkoMan_Stamp(EnGinkoMan* this, PlayState* play);
void EnGinkoMan_Dialogue(EnGinkoMan* this, PlayState* play);
void EnGinkoMan_SwitchAnimation(EnGinkoMan* this, PlayState* play);

ActorProfile En_Ginko_Man_Profile = {
    /**/ ACTOR_EN_GINKO_MAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BOJ,
    /**/ sizeof(EnGinkoMan),
    /**/ EnGinkoMan_Init,
    /**/ EnGinkoMan_Destroy,
    /**/ EnGinkoMan_Update,
    /**/ EnGinkoMan_Draw,
};

typedef enum {
    /* 0 */ GINKO_ANIM_LEGSMACKING,
    /* 1 */ GINKO_ANIM_SITTING,
    /* 2 */ GINKO_ANIM_REACHING,
    /* 3 */ GINKO_ANIM_AMAZED,
    /* 4 */ GINKO_ANIM_ADVERTISING, // looking around for customers
    /* 5 */ GINKO_ANIM_MAX
} GinkoAnimation;

static AnimationInfo sAnimationInfo[GINKO_ANIM_MAX] = {
    { &object_boj_Anim_0008C0, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f }, // GINKO_ANIM_LEGSMACKING
    { &object_boj_Anim_0043F0, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f }, // GINKO_ANIM_SITTING
    { &object_boj_Anim_004F40, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f }, // GINKO_ANIM_REACHING
    { &object_boj_Anim_000AC4, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f }, // GINKO_ANIM_AMAZED
    { &object_boj_Anim_004A7C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f }, // GINKO_ANIM_ADVERTISING
};

void EnGinkoMan_Init(Actor* thisx, PlayState* play) {
    EnGinkoMan* this = (EnGinkoMan*)thisx;

    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->actor.cullingVolumeDistance = 400.0f;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.colChkInfo.cylRadius = 100;
    this->curTextId = 0;
    this->isNewAccount = false;
    this->isStampChecked = false;
    this->choiceDepositWithdrawl = GINKOMAN_CHOICE_RESET;
    this->serviceFee = 0;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_boj_Skel_00C240, &object_boj_Anim_0043F0, this->jointTable,
                       this->morphTable, OBJECT_BOJ_LIMB_MAX);
    EnGinkoMan_SetupIdle(this);
}

void EnGinkoMan_Destroy(Actor* thisx, PlayState* play) {
}

void EnGinkoMan_SetupIdle(EnGinkoMan* this) {
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
    this->actionFunc = EnGinkoMan_Idle;
}

void EnGinkoMan_Idle(EnGinkoMan* this, PlayState* play) {
    s32 yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    EnGinkoMan_SwitchAnimation(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (GameInteractor_Should(VB_CONTINUE_BANKER_DIALOGUE, true, this)) {
            if (HS_GET_BANK_RUPEES() == 0) {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_LEGSMACKING);
                Message_StartTextbox(play, 0x44C, &this->actor);
                this->curTextId = 0x44C; // would you like to make an account
            } else {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                if ((CURRENT_DAY == 3) && (gSaveContext.save.isNight == true)) {
                    Message_StartTextbox(play, 0x467, &this->actor);
                    this->curTextId = 0x467;
                } else {
                    Message_StartTextbox(play, 0x466, &this->actor);
                    this->curTextId = 0x466;
                }
            }
        }
        EnGinkoMan_SetupDialogue(this);
    } else if (ABS_ALT(yaw) < 0x1555) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

// action func: non-input dialogue
void EnGinkoMan_DepositDialogue(EnGinkoMan* this, PlayState* play) {
    if (!Message_ShouldAdvance(play)) {
        return;
    }

    if (!GameInteractor_Should(VB_CONTINUE_BANKER_DIALOGUE, true, this)) {}

    switch (this->curTextId) {
        case 0x44C:
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE)) {
                Message_StartTextbox(play, 0x44E, &this->actor);
                this->curTextId = 0x44E;
            } else {
                Message_StartTextbox(play, 0x44D, &this->actor);
                this->curTextId = 0x44D;
            }
            break;

        case 0x44D:
            Message_StartTextbox(play, 0x44E, &this->actor);
            this->curTextId = 0x44E;
            break;

        case 0x44F:
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_LEGSMACKING);
            Message_StartTextbox(play, 0x450, &this->actor);
            this->curTextId = 0x450;
            break;

        case 0x453: // you deposited a tiny amount
        case 0x454: // you deposited a normal amount
        case 0x455: // you deposited a lot
            if (this->isNewAccount == true) {
                this->isNewAccount = false;
                if (this->curTextId != 0x453) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                }

                Message_StartTextbox(play, 0x461, &this->actor);
                this->curTextId = 0x461;
            } else {
                if (this->curTextId == 0x453) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_LEGSMACKING);
                }

                play->msgCtx.rupeesTotal = HS_GET_BANK_RUPEES();
                Message_StartTextbox(play, 0x45A, &this->actor);
                this->curTextId = 0x45A;
            }
            break;

        case 0x456:
        case 0x459:
            Message_StartTextbox(play, 0x44E, &this->actor);
            this->curTextId = 0x44E;
            break;

        case 0x45A:
            if (GameInteractor_Should(VB_PASS_FIRST_BANK_THRESHOLD,
                                      (HS_GET_BANK_RUPEES() >= 200) && (this->previousBankValue < 200) &&
                                          !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_40),
                                      this)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_59_40);
                Message_StartTextbox(play, 0x45B, &this->actor);
                this->curTextId = 0x45B;
            } else if (GameInteractor_Should(VB_PASS_INTEREST_BANK_THRESHOLD,
                                             (HS_GET_BANK_RUPEES() >= 1000) && (this->previousBankValue < 1000),
                                             this) &&
                       !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_80)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_59_80);
                Message_StartTextbox(play, 0x45C, &this->actor);
                this->curTextId = 0x45C;
            } else if (GameInteractor_Should(VB_PASS_SECOND_BANK_THRESHOLD, HS_GET_BANK_RUPEES() >= 5000, this)) {
                if (GameInteractor_Should(VB_PASS_SECOND_BANK_THRESHOLD_ALT,
                                          (this->previousBankValue < 5000) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_60_01),
                                          this)) {
                    SET_WEEKEVENTREG(WEEKEVENTREG_60_01);
                    Message_StartTextbox(play, 0x45D, &this->actor);
                    this->curTextId = 0x45D;
                } else if (this->previousBankValue < (s16)HS_GET_BANK_RUPEES()) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                    Message_StartTextbox(play, 0x45E, &this->actor);
                    this->curTextId = 0x45E;
                } else {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_LEGSMACKING);
                    Message_StartTextbox(play, 0x460, &this->actor);
                    this->curTextId = 0x460;
                }
            } else {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_LEGSMACKING);
                Message_StartTextbox(play, 0x460, &this->actor);
                this->curTextId = 0x460;
            }
            break;

        case 0x45B: // given 200 reward
        case 0x45C: // given 1000 reward
        case 0x45D: // given 5000 reward
            this->isStampChecked = false;
            Message_CloseTextbox(play);
            EnGinkoMan_SetupBankAward(this);
            EnGinkoMan_BankAward(this, play);
            break;

        case 0x461:
            Message_StartTextbox(play, 0x462, &this->actor);
            this->curTextId = 0x462;
            break;

        case 0x462:
            Message_StartTextbox(play, 0x463, &this->actor);
            this->curTextId = 0x463;
            break;

        case 0x463:
            Message_StartTextbox(play, 0x464, &this->actor);
            this->curTextId = 0x464;
            break;

        case 0x464:
            play->msgCtx.msgMode = MSGMODE_PAUSED;
            EnGinkoMan_SetupStamp(this); // stamp player
            break;

        case 0x465:
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_LEGSMACKING);
            play->msgCtx.rupeesTotal = HS_GET_BANK_RUPEES();
            Message_StartTextbox(play, 0x45A, &this->actor);
            this->curTextId = 0x45A;
            break;

        case 0x466:
        case 0x467:
            Message_StartTextbox(play, 0x468, &this->actor);
            this->curTextId = 0x468;
            break;

        case 0x469:
            EnGinkoMan_SetupStamp(this); // stamp player
            break;

        case 0x46A:
        case 0x46C:
        case 0x47E:
            if (this->choiceDepositWithdrawl == GINKOMAN_CHOICE_DEPOSIT) {
                if (HS_GET_BANK_RUPEES() >= 5000) {
                    Message_StartTextbox(play, 0x45F, &this->actor);
                    this->curTextId = 0x45F;
                } else if (gSaveContext.save.saveInfo.playerData.rupees == 0) {
                    Message_StartTextbox(play, 0x458, &this->actor);
                    this->curTextId = 0x458;
                } else {
                    Message_StartTextbox(play, 0x479, &this->actor);
                    this->curTextId = 0x479;
                }
            } else if ((CURRENT_DAY == 3) && (gSaveContext.save.isNight == true)) {
                Message_StartTextbox(play, 0x46D, &this->actor);

                this->curTextId = 0x46D;
            } else { // GINKOMAN_CHOICE_WITHDRAWL
                Message_StartTextbox(play, 0x46B, &this->actor);
                this->curTextId = 0x46B;
            }

            this->choiceDepositWithdrawl = GINKOMAN_CHOICE_RESET;
            break;

        case 0x46B:
            Message_StartTextbox(play, 0x46E, &this->actor);
            this->curTextId = 0x46E;
            break;

        case 0x46D:
            Message_StartTextbox(play, 0x46B, &this->actor);
            this->curTextId = 0x46B;
            break;

        case 0x470:
            if (Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->isStampChecked = false;
                EnGinkoMan_SetupIdle(this); // change to waiting for approach
            }
            break;

        case 0x476:
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
            // fallthrough
        case 0x475:
        case 0x47C:
        case 0x47D:
            Message_StartTextbox(play, 0x468, &this->actor);
            this->curTextId = 0x468;
            break;

        case 0x472:
        case 0x473:
        case 0x474:
            if (HS_GET_BANK_RUPEES() == 0) {
                Message_StartTextbox(play, 0x478, &this->actor);
                this->curTextId = 0x478;
            } else {
                play->msgCtx.rupeesTotal = HS_GET_BANK_RUPEES();
                Message_StartTextbox(play, 0x45A, &this->actor);
                this->curTextId = 0x45A;
            }
            break;

        case 0x477:
            Message_StartTextbox(play, 0x471, &this->actor);
            this->curTextId = 0x471;
            this->serviceFee = play->msgCtx.unk1206C;
            break;

        case 0x479:
            Message_StartTextbox(play, 0x44F, &this->actor);
            this->curTextId = 0x44F;
            break;

        default:
            break;
    }
}

void EnGinkoMan_WaitForDialogueInput(EnGinkoMan* this, PlayState* play) {
    if (!Message_ShouldAdvance(play)) {
        return;
    }

    if (!GameInteractor_Should(VB_CONTINUE_BANKER_DIALOGUE, true, this)) {}

    switch (this->curTextId) {
        case 0x44E:
            if (play->msgCtx.choiceIndex == GINKOMAN_CHOICE_YES) {
                if (HS_GET_BANK_RUPEES() >= 5000) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Message_StartTextbox(play, 0x45F, &this->actor);
                    this->curTextId = 0x45F; // bank full, cannot accept more
                } else {
                    if (gSaveContext.save.saveInfo.playerData.rupees > 0) {
                        Audio_PlaySfx_MessageDecide();
                        Message_StartTextbox(play, 0x44F, &this->actor);
                        this->curTextId = 0x44F;
                    } else {
                        Audio_PlaySfx(NA_SE_SY_ERROR);
                        Message_StartTextbox(play, 0x458, &this->actor);
                        this->curTextId = 0x458; // you haven't even gotten a single rup
                    }
                }
            } else { // GINKOMAN_CHOICE_NO
                Audio_PlaySfx_MessageCancel();
                Message_StartTextbox(play, 0x451, &this->actor);
                this->curTextId = 0x451;
            }
            break;

        case 0x452:
            if (play->msgCtx.choiceIndex == GINKOMAN_CHOICE_YES) {
                if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.rupeesSelected) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                    Message_StartTextbox(play, 0x459, &this->actor);
                    this->curTextId = 0x459;
                } else {
                    Audio_PlaySfx_MessageDecide();
                    if (play->msgCtx.rupeesSelected >= 100) {
                        Message_StartTextbox(play, 0x455, &this->actor);
                        this->curTextId = 0x455;
                    } else if (play->msgCtx.rupeesSelected >= 10) {
                        Message_StartTextbox(play, 0x454, &this->actor);
                        this->curTextId = 0x454;
                    } else {
                        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                        Message_StartTextbox(play, 0x453, &this->actor);
                        this->curTextId = 0x453;
                    }

                    if (HS_GET_BANK_RUPEES() == 0) {
                        this->isNewAccount = true;
                    }

                    Rupees_ChangeBy(-play->msgCtx.rupeesSelected);
                    this->previousBankValue = HS_GET_BANK_RUPEES();
                    HS_SET_BANK_RUPEES(HS_GET_BANK_RUPEES() + play->msgCtx.rupeesSelected);
                }
            } else { // GINKOMAN_CHOICE_NO
                Audio_PlaySfx_MessageCancel();
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                if (HS_GET_BANK_RUPEES() == 0) {
                    Message_StartTextbox(play, 0x456, &this->actor);
                    this->curTextId = 0x456;
                } else {
                    Message_StartTextbox(play, 0x47D, &this->actor);
                    this->curTextId = 0x47D;
                }
            }
            break;

        case 0x468:
            if (play->msgCtx.choiceIndex == GINKOMAN_CHOICE_CANCEL) {
                Audio_PlaySfx_MessageCancel();
                Message_StartTextbox(play, 0x470, &this->actor);
                this->curTextId = 0x470;
            } else {
                Audio_PlaySfx_MessageDecide();
                this->choiceDepositWithdrawl = play->msgCtx.choiceIndex;
                if (!this->isStampChecked) {
                    this->isStampChecked = true;
                    Message_StartTextbox(play, 0x469, &this->actor);
                    this->curTextId = 0x469;
                } else {
                    Message_StartTextbox(play, 0x47E, &this->actor);
                    this->curTextId = 0x47E;
                }
            }
            break;

        case 0x471:
            if (play->msgCtx.choiceIndex == GINKOMAN_CHOICE_YES) {
                if ((s32)HS_GET_BANK_RUPEES() < (play->msgCtx.rupeesSelected + this->serviceFee)) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_LEGSMACKING);
                    Message_StartTextbox(play, 0x476, &this->actor);
                    this->curTextId = 0x476;
                } else if (CUR_CAPACITY(UPG_WALLET) <
                           (play->msgCtx.rupeesSelected + gSaveContext.save.saveInfo.playerData.rupees)) {
                    // check if wallet is big enough
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Message_StartTextbox(play, 0x475, &this->actor);
                    this->curTextId = 0x475;
                } else {
                    Audio_PlaySfx_MessageDecide();
                    if (play->msgCtx.rupeesSelected >= 100) {
                        Message_StartTextbox(play, 0x474, &this->actor);
                        this->curTextId = 0x474;
                    } else if (play->msgCtx.rupeesSelected >= 10) {
                        Message_StartTextbox(play, 0x473, &this->actor);
                        this->curTextId = 0x473;
                    } else {
                        Message_StartTextbox(play, 0x472, &this->actor);
                        this->curTextId = 0x472;
                    }

                    this->previousBankValue = HS_GET_BANK_RUPEES();
                    HS_SET_BANK_RUPEES(HS_GET_BANK_RUPEES() - play->msgCtx.rupeesSelected - this->serviceFee);
                    Rupees_ChangeBy(play->msgCtx.rupeesSelected);
                }
            } else {
                Audio_PlaySfx_MessageCancel();
                Message_StartTextbox(play, 0x47C, &this->actor);
                this->curTextId = 0x47C;
            }
            break;

        default:
            break;
    }
}

void EnGinkoMan_WaitForRupeeCount(EnGinkoMan* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->curTextId) {
            case 0x450:
                if (play->msgCtx.rupeesSelected == 0) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                    Message_StartTextbox(play, 0x457, &this->actor);
                    this->curTextId = 0x457;
                } else {
                    Message_StartTextbox(play, 0x452, &this->actor);
                    this->curTextId = 0x452;
                }
                break;

            case 0x46E:
                if (play->msgCtx.rupeesSelected == 0) {
                    Message_StartTextbox(play, 0x46F, &this->actor);
                    this->curTextId = 0x46F;
                } else if (gSaveContext.save.isNight == true) {
                    Message_StartTextbox(play, 0x477, &this->actor);
                    this->curTextId = 0x477;
                } else {
                    Message_StartTextbox(play, 0x471, &this->actor);
                    this->curTextId = 0x471;
                    this->serviceFee = 0;
                }
                break;

            default:
                break;
        }
    }
}

void EnGinkoMan_SetupDialogue(EnGinkoMan* this) {
    this->actionFunc = EnGinkoMan_Dialogue;
}

void EnGinkoMan_Dialogue(EnGinkoMan* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            EnGinkoMan_SetupIdle(this);
            break;

        case TEXT_STATE_CHOICE:
            EnGinkoMan_WaitForDialogueInput(this, play);
            break;

        case TEXT_STATE_EVENT:
            EnGinkoMan_DepositDialogue(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                this->isStampChecked = false;
                EnGinkoMan_SetupIdle(this);
            }
            break;

        case TEXT_STATE_INPUT_RUPEES:
            EnGinkoMan_WaitForRupeeCount(this, play);
            break;

        case TEXT_STATE_NONE:
        default:
            break;
    }

    if ((this->skelAnime.animation == &object_boj_Anim_0008C0) &&
        Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BANK_MAN_HAND_HIT);
    }
}

void EnGinkoMan_SetupBankAward(EnGinkoMan* this) {
    this->actionFunc = EnGinkoMan_BankAward;
}

void EnGinkoMan_BankAward(EnGinkoMan* this, PlayState* play) {
    if (!GameInteractor_Should(VB_BANKER_GIVE_REWARD, true, this)) {
        return;
    }

    if (Actor_HasParent(&this->actor, play)) {
        // Parent is the player when starting to receive the award
        this->actor.parent = NULL;
        EnGinkoMan_SetupBankAward2(this);
    } else if (this->curTextId == 0x45B) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE)) {
            Actor_OfferGetItem(&this->actor, play, GI_WALLET_ADULT + CUR_UPG_VALUE(UPG_WALLET), 500.0f, 100.0f);
        } else {
            Actor_OfferGetItem(&this->actor, play, GI_RUPEE_BLUE, 500.0f, 100.0f);
        }
    } else if (this->curTextId == 0x45C) {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_BLUE, 500.0f, 100.0f);
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_BLUE, 500.0f, 100.0f);
    }
}

// called when bank has a parent actor
void EnGinkoMan_SetupBankAward2(EnGinkoMan* this) {
    this->actionFunc = EnGinkoMan_BankAward2;
}

// separate function to handle bank rewards... called while the player is receiving the award
void EnGinkoMan_BankAward2(EnGinkoMan* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE) && (this->curTextId == 0x45B)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE);
            Message_StartTextbox(play, 0x47A, &this->actor);
            this->curTextId = 0x47A; // Message after receiving reward for depositing 200 rupees.
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
            Message_StartTextbox(play, 0x47B, &this->actor);
            this->curTextId = 0x47B; // Message after receiving reward for depositing 1000 rupees.
        }

        EnGinkoMan_SetupDialogue(this);
    } else if (this->curTextId == 0x45D) { // saved up 5000 rupees for HP
        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE);
            }
            EnGinkoMan_SetupIdle(this);
        }
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 500.0f, PLAYER_IA_MINUS1);
    }
}

void EnGinkoMan_SetupStamp(EnGinkoMan* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_REACHING);
    this->actionFunc = EnGinkoMan_Stamp;
}

void EnGinkoMan_Stamp(EnGinkoMan* this, PlayState* play) {
    if ((this->curTextId == 0x464) && Animation_OnFrame(&this->skelAnime, 10.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_HANKO); // "stamp"
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        switch (this->curTextId) {
            case 0x464:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                Message_StartTextbox(play, 0x465, &this->actor);
                this->curTextId = 0x465;
                break;

            case 0x469:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_SITTING);
                play->msgCtx.rupeesTotal = HS_GET_BANK_RUPEES();
                if ((CURRENT_DAY == 3) && (gSaveContext.save.isNight == true)) {
                    Message_StartTextbox(play, 0x46C, &this->actor);
                    this->curTextId = 0x46C;
                } else {
                    Message_StartTextbox(play, 0x46A, &this->actor);
                    this->curTextId = 0x46A;
                }
                break;

            default:
                break;
        }

        EnGinkoMan_SetupDialogue(this);
    }
}

void EnGinkoMan_SwitchAnimation(EnGinkoMan* this, PlayState* play) {
    if (this->actor.xzDistToPlayer > 160.0f) {
        if (this->animTimer == 0) {
            if (this->skelAnime.animation != &object_boj_Anim_004A7C) {
                this->animTimer = 40;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_ADVERTISING);
            }
        }
    } else if ((this->animTimer == 0) && (this->skelAnime.animation != &object_boj_Anim_000AC4)) {
        this->animTimer = 40;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, GINKO_ANIM_AMAZED);
    }

    DECR(this->animTimer);
}

void EnGinkoMan_FacePlayer(EnGinkoMan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->skelAnime.animation != &object_boj_Anim_004A7C) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    } else {
        Actor_TrackNone(&this->headRot, &this->torsoRot);
    }
}

void EnGinkoMan_Update(Actor* thisx, PlayState* play) {
    EnGinkoMan* this = (EnGinkoMan*)thisx;

    this->actionFunc(this, play);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 30.0f;
    EnGinkoMan_FacePlayer(this, play);
}

s32 EnGinkoMan_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnGinkoMan* this = (EnGinkoMan*)thisx;

    if (limbIndex == OBJECT_BOJ_LIMB_0F) {
        *dList = object_boj_DL_00B1D8;
    }

    if (limbIndex == OBJECT_BOJ_LIMB_0F) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    } else if (limbIndex == OBJECT_BOJ_LIMB_08) {
        Matrix_RotateXS(-this->torsoRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->torsoRot.x, MTXMODE_APPLY);
    }

    return false;
}

void EnGinkoMan_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnGinkoMan_Draw(Actor* thisx, PlayState* play) {
    EnGinkoMan* this = (EnGinkoMan*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_EnvColor(play->state.gfxCtx, 50, 80, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 50, 80, 0, 0));
    gDPPipeSync(POLY_OPA_DISP++);

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnGinkoMan_OverrideLimbDraw, EnGinkoMan_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

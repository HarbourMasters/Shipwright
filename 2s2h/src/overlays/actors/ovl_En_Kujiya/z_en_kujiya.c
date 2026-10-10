/*
 * File: z_en_kujiya.c
 * Overlay: ovl_En_Kujiya
 * Description: Clock Town - Lottery Shop
 *
 * (kuji = lottery, ya = shop)
 */

#include "z_en_kujiya.h"
#include "objects/object_kujiya/object_kujiya.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnKujiya_Init(Actor* thisx, PlayState* play);
void EnKujiya_Destroy(Actor* thisx, PlayState* play);
void EnKujiya_Update(Actor* thisx, PlayState* play);
void EnKujiya_Draw(Actor* thisx, PlayState* play);

void EnKujiya_SetupWait(EnKujiya* this);
void EnKujiya_Wait(EnKujiya* this, PlayState* play);
void EnKujiya_SetupTalk(EnKujiya* this);
void EnKujiya_Talk(EnKujiya* this, PlayState* play);
void EnKujiya_SetupGivePrize(EnKujiya* this);
void EnKujiya_GivePrize(EnKujiya* this, PlayState* play);
void EnKujiya_SetupFinishGivePrize(EnKujiya* this);
void EnKujiya_FinishGivePrize(EnKujiya* this, PlayState* play);
s32 EnKujiya_CheckBoughtTicket(void);
void EnKujiya_SetBoughtTicket(void);
void EnKujiya_UnsetBoughtTicket(void);
void EnKujiya_SetupTurnToOpen(EnKujiya* this);
void EnKujiya_TurnToOpen(EnKujiya* this, PlayState* play);
void EnKujiya_SetupTurnToClosed(EnKujiya* this);
void EnKujiya_TurnToClosed(EnKujiya* this, PlayState* play);

ActorProfile En_Kujiya_Profile = {
    /**/ ACTOR_EN_KUJIYA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KUJIYA,
    /**/ sizeof(EnKujiya),
    /**/ EnKujiya_Init,
    /**/ EnKujiya_Destroy,
    /**/ EnKujiya_Update,
    /**/ EnKujiya_Draw,
};

#define CHECK_LOTTERY_NUMBERS()                                                      \
    (((u32)((void)0, gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][0]) == \
      ((HS_GET_LOTTERY_CODE_GUESS() & 0xF00) >> 8)) &&                               \
     ((u32)((void)0, gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][1]) == \
      ((HS_GET_LOTTERY_CODE_GUESS() & 0xF0) >> 4)) &&                                \
     ((u32)((void)0, gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][2]) == \
      (HS_GET_LOTTERY_CODE_GUESS() & 0xF)))

void EnKujiya_Init(Actor* thisx, PlayState* play) {
    EnKujiya* this = (EnKujiya*)thisx;

    Actor_SetScale(&this->actor, 0.1f);

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 30.0f;

    if (EnKujiya_CheckBoughtTicket() && (CURRENT_TIME >= CLOCK_TIME(6, 0)) && (CURRENT_TIME < CLOCK_TIME(18, 0))) {
        this->actor.shape.rot.y = 0;
    } else {
        this->actor.shape.rot.y = 0x7555;
    }
    EnKujiya_SetupWait(this);
}

void EnKujiya_Destroy(Actor* thisx, PlayState* play) {
}

void EnKujiya_SetupWait(EnKujiya* this) {
    this->actionFunc = EnKujiya_Wait;
}

void EnKujiya_Wait(EnKujiya* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if ((CURRENT_TIME >= CLOCK_TIME(6, 0)) && (CURRENT_TIME < CLOCK_TIME(18, 0))) {
            if (EnKujiya_CheckBoughtTicket()) {
                Message_StartTextbox(play, 0x2B61, &this->actor);
                this->textId = 0x2B61; // Come back tomorrow
            } else {
                Message_StartTextbox(play, 0x2B5C, &this->actor);
                this->textId = 0x2B5C; // Pick 3 numbers
            }
        } else if (EnKujiya_CheckBoughtTicket()) {
            Message_StartTextbox(play, 0x2B64, &this->actor);
            this->textId = 0x2B64; // Announce winning numbers
        } else {
            Message_StartTextbox(play, 0x2B63, &this->actor);
            this->textId = 0x2B63; // Exchanging winning tickets
        }

        EnKujiya_SetupTalk(this);
    } else if ((CURRENT_TIME >= CLOCK_TIME(18, 0)) && EnKujiya_CheckBoughtTicket() && (this->actor.shape.rot.y == 0)) {
        EnKujiya_SetupTurnToOpen(this);
    } else if (this->actor.xzDistToPlayer < 100.0f) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void EnKujiya_HandlePlayerChoice(EnKujiya* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) { // Buy
            if (gSaveContext.save.saveInfo.playerData.rupees < 10) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                Message_StartTextbox(play, 0x2B62, &this->actor);
                this->textId = 0x2B62; // Not enough Rupees
            } else {
                Audio_PlaySfx_MessageDecide();
                Rupees_ChangeBy(-10);
                Message_StartTextbox(play, 0x2B5F, &this->actor);
                this->textId = 0x2B5F; // Enter number
            }
        } else { // Don't buy
            Audio_PlaySfx_MessageCancel();
            Message_StartTextbox(play, 0x2B5E, &this->actor);
            this->textId = 0x2B5E; // Too bad
        }
    }
}

void EnKujiya_ChooseNextDialogue(EnKujiya* this, PlayState* play) {
    // Build suspense
    if (this->textId == 0x2B65) { // Your numbers vs winning numbers
        if (this->timer != 0) {
            this->timer--;
            return;
        }
    }

    if (Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x2B5C:
                Message_StartTextbox(play, 0x2B5D, &this->actor);
                this->textId = 0x2B5D; // Buy or not
                break;

            case 0x2B60:
                EnKujiya_SetBoughtTicket();
                Message_CloseTextbox(play);
                EnKujiya_SetupTurnToClosed(this);
                break;

            case 0x2B64:
                EnKujiya_UnsetBoughtTicket();
                this->timer = 20;
                Message_StartTextbox(play, 0x2B65, &this->actor);
                this->textId = 0x2B65; // Your numbers vs winning numbers
                break;

            case 0x2B65:
                if (CHECK_LOTTERY_NUMBERS()) {
                    Message_StartTextbox(play, 0x2B66, &this->actor);
                    this->textId = 0x2B66; // Won 50 Rupees
                } else {
                    Message_StartTextbox(play, 0x2B67, &this->actor);
                    this->textId = 0x2B67; // Lost, come back tomorrow
                }
                break;

            case 0x2B66:
                Message_CloseTextbox(play);
                if (GameInteractor_Should(VB_GIVE_LOTTERY_WINNINGS, true, this)) {
                    EnKujiya_SetupGivePrize(this);
                    EnKujiya_GivePrize(this, play);
                }
                break;

            default:
                break;
        }
    }
}

void EnKujiya_SetupTalk(EnKujiya* this) {
    this->actionFunc = EnKujiya_Talk;
}

void EnKujiya_Talk(EnKujiya* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            break;

        case TEXT_STATE_CHOICE:
            EnKujiya_HandlePlayerChoice(this, play);
            break;

        case TEXT_STATE_EVENT:
            EnKujiya_ChooseNextDialogue(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                EnKujiya_SetupWait(this);
            }
            break;

        case TEXT_STATE_INPUT_LOTTERY_CODE:
            if (Message_ShouldAdvance(play)) {
                Inventory_SaveLotteryCodeGuess(play);
                Message_StartTextbox(play, 0x2B60, &this->actor);
                this->textId = 0x2B60; // Will announce winning numbers after 6
            }
            break;

        default:
            break;
    }
}

void EnKujiya_SetupGivePrize(EnKujiya* this) {
    this->actionFunc = EnKujiya_GivePrize;
}

void EnKujiya_GivePrize(EnKujiya* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        EnKujiya_SetupFinishGivePrize(this);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
    }
}

void EnKujiya_SetupFinishGivePrize(EnKujiya* this) {
    this->actionFunc = EnKujiya_FinishGivePrize;
}

void EnKujiya_FinishGivePrize(EnKujiya* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        EnKujiya_SetupWait(this);
    }
}

s32 EnKujiya_CheckBoughtTicket(void) {
    switch (CURRENT_DAY) {
        case 1:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_33_10)) {
                return true;
            }
            break;

        case 2:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_33_20)) {
                return true;
            }
            break;

        case 3:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_33_40)) {
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}

void EnKujiya_SetBoughtTicket(void) {
    switch (CURRENT_DAY) {
        case 1:
            SET_WEEKEVENTREG(WEEKEVENTREG_33_10);
            break;

        case 2:
            SET_WEEKEVENTREG(WEEKEVENTREG_33_20);
            break;

        case 3:
            SET_WEEKEVENTREG(WEEKEVENTREG_33_40);
            break;

        default:
            break;
    }
}

/**
 * Used when player turns in a ticket
 */
void EnKujiya_UnsetBoughtTicket(void) {
    switch (CURRENT_DAY) {
        case 1:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_33_10);
            break;

        case 2:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_33_20);
            break;

        case 3:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_33_40);
            break;

        default:
            break;
    }
}

void EnKujiya_SetupTurnToOpen(EnKujiya* this) {
    this->timer = 0;
    this->actionFunc = EnKujiya_TurnToOpen;
}

void EnKujiya_TurnToOpen(EnKujiya* this, PlayState* play) {
    if (this->actor.csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }

    if (!Math_SmoothStepToS(&this->actor.shape.rot.y, 0x7555, 0xA, 0x16C, 0x16C)) {
        if (this->timer > 20) {
            if (CutsceneManager_GetCurrentCsId() == this->actor.csId) {
                CutsceneManager_Stop(this->actor.csId);
            }
            EnKujiya_SetupWait(this);
        } else {
            this->timer++;
        }
    } else {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_WINDMILL_LEVEL - SFX_FLAG);
    }
}

void EnKujiya_SetupTurnToClosed(EnKujiya* this) {
    this->timer = 0;
    this->actionFunc = EnKujiya_TurnToClosed;
}

void EnKujiya_TurnToClosed(EnKujiya* this, PlayState* play) {
    if (this->actor.csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }

    if (!Math_SmoothStepToS(&this->actor.shape.rot.y, 0, 0xA, 0x16C, 0x16C)) {
        if (this->timer > 20) {
            if (CutsceneManager_GetCurrentCsId() == this->actor.csId) {
                CutsceneManager_Stop(this->actor.csId);
            }
            EnKujiya_SetupWait(this);
        } else {
            this->timer++;
        }
    } else {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_WINDMILL_LEVEL - SFX_FLAG);
    }
}

void EnKujiya_Update(Actor* thisx, PlayState* play) {
    EnKujiya* this = (EnKujiya*)thisx;

    this->actionFunc(this, play);
}

void EnKujiya_Draw(Actor* thisx, PlayState* play) {
    EnKujiya* this = (EnKujiya*)thisx;

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gLotteryShopTexAnim));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopCylinderDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopBackSignDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopOpenBoxDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopScrollingEffectsDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopRedRupeesDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopGreenRupeesDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopBlueRupeesDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopTargetArrowAndSignDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopScrollingArrowDL);
    gSPDisplayList(POLY_OPA_DISP++, gLotteryShopMaskDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

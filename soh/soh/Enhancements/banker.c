#include <stdlib.h>
#include "z64.h"
#include "macros.h"
#include "libultraship/libultra/gbi.h"
#include "../soh/assets/textures/parameter_static/parameter_static.h"
#include "variables.h"
#include "custom-message/CustomMessageTypes.h"
#include "functions.h"
#include "soh/OTRGlobals.h"


#include "luslog.h"

#define DIGIT_WIDTH 8
#define DIGIT_HEIGHT 16
#define HUNDREDS_OFFSET -16
#define TENS_OFFSET 8
#define ONES_OFFSET 8
#define BLINK_DURATION 10
#define COOLDOWN_FRAMES 3
#define FADE_DURATION 5

static s16 bankerValue = 0;
static s16 selectedDigit = 0;
static s16 blinkTimer = 0;
static bool isGivingHeart = false;
static bool isGivingCharm = false; 
static bool canContinue = false;
static bool prevTextboxHeart = false;
/*static*/ bool prevTextboxCharm = false; //Temporarily global until GI_PIRATE_CHARM is implemented
static Actor* bankerActor = NULL;
static s16 OptionChoice = -1;
static s16 fadeTimer = 0;

extern const char* digitTextures[];

Color_RGBA8 highlightColor = { 255, 255, 0, 255 };

void UpdateBankerOverlay(PlayState* play, Gfx** gfx, s16 value, s16 selectedDigit) {
    s16 posX = 160 - 76;
    s16 posY = 180 - 20;
    s16 digits[3] = {value / 100, (value % 100) / 10, value % 10};
    s16 offsets[3] = {HUNDREDS_OFFSET, TENS_OFFSET, ONES_OFFSET};
    if (fadeTimer < FADE_DURATION) {
        fadeTimer++;
    }
    for (int i = 0; i < 3; i++) {
        posX += offsets[i];
        s16 digit = digits[i];
        bool isSelected = selectedDigit == 2 - i;
        gDPPipeSync((*gfx)++);
        gDPLoadTextureBlock((*gfx)++, ((u8*)digitTextures[digit]), G_IM_FMT_I, G_IM_SIZ_8b, DIGIT_WIDTH, DIGIT_HEIGHT, 0,
                            G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        u8 fadeAlpha = fadeTimer < FADE_DURATION ? (255 * fadeTimer) / FADE_DURATION : 255;
        Color_RGBA8 primColor = isSelected && blinkTimer < BLINK_DURATION ? 
                                (Color_RGBA8){highlightColor.r, highlightColor.g, highlightColor.b, fadeAlpha} : 
                                (Color_RGBA8){255, 255, 255, fadeAlpha};
        gDPSetPrimColor((*gfx)++, 0, 0, primColor.r, primColor.g, primColor.b, primColor.a);
        gDPSetCombineMode((*gfx)++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetRenderMode((*gfx)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gSPTextureRectangle((*gfx)++, posX << 2, posY << 2, (posX + DIGIT_WIDTH) << 2, (posY + DIGIT_HEIGHT) << 2,
                            G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }
}

s16 CalculateSelectedValue(s16 value, s16 selectedDigit, bool increase, s16 min, s16 max) {
    s16 digitValues[3];
    digitValues[0] = value % 10;
    digitValues[1] = (value / 10) % 10;
    digitValues[2] = value / 100;
    if (increase) {
        digitValues[selectedDigit] = (digitValues[selectedDigit] + 1) % 10;
    } else {
        digitValues[selectedDigit] = (digitValues[selectedDigit] + 9) % 10;
    }
    s16 newValue = digitValues[2] * 100 + digitValues[1] * 10 + digitValues[0];
    return (newValue < min) ? min : (newValue > max) ? max : newValue;
}

void ProcessInput(PlayState* play, s16* value, s16* selectedDigit) {
    static s16 inputCooldownTimer = 0;
    Input* input = &play->state.input[0];
    bool playSound = false;
    if (play->msgCtx.textId != TEXT_BANKER_WITHDRAWAL_AMOUNT && play->msgCtx.textId != TEXT_BANKER_DEPOSIT_AMOUNT) {
        return;
    }
    if (inputCooldownTimer > 0) {
        inputCooldownTimer--;
        return;
    }    if (abs(input->rel.stick_y) > 30) {
        *value = CalculateSelectedValue(*value, *selectedDigit, input->rel.stick_y > 0, 0, 999);
        bankerValue = *value;
        playSound = true;
    }
    if (abs(input->rel.stick_x) > 30) {
        *selectedDigit = (input->rel.stick_x < 0) ? (*selectedDigit + 1) % 3 : (*selectedDigit - 1 + 3) % 3;
        playSound = true;
    }
    if (playSound) {
        Audio_PlaySoundGeneral(NA_SE_SY_RUPY_COUNT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    inputCooldownTimer = playSound ? COOLDOWN_FRAMES : 0;
}

static void HandleBankerInteraction(PlayState* play, MessageContext* msgCtx) {
    uint16_t currentTextId = msgCtx->textId;

    if (msgCtx->msgMode != MSGMODE_TEXT_DONE && msgCtx->msgMode != MSGMODE_TEXT_CLOSING) {
        return;
    }

    if (!Message_ShouldAdvance(play)) {
        return;
    }

    s16 nextTextId;

    switch (currentTextId) {
        case TEXT_BEGGAR_VANILLA:
            canContinue = false;
            Actor* playerActor = &GET_PLAYER(play)->actor;
            Actor* foundActor = Actor_FindNearby(play, playerActor, ACTOR_EN_HY, ACTORCAT_NPC, 80.0f);
            if (foundActor != NULL) {
                bankerActor = foundActor;
            }
            Message_ContinueTextbox(play, TEXT_BANKER_OPTIONS);
            break;

        case TEXT_BANKER_OPTIONS:
            if (msgCtx->choiceIndex == 0 || msgCtx->choiceIndex == 1) {
                OptionChoice = msgCtx->choiceIndex;
                if (gSaveContext.hasFee == 0) {
                    Message_ContinueTextbox(play, TEXT_BANKER_TRANSACTION_FEE);
                } else {
                    Message_ContinueTextbox(play, TEXT_BANKER_BALANCE);
                }
            } else if (msgCtx->choiceIndex == 2) {
                Message_CloseTextbox(play);
            }
            fadeTimer = 0;
            break;

        case TEXT_BANKER_BALANCE:
            if (gSaveContext.playerBalance >= 200 && !gSaveContext.hasWarpTransfer) {
                gSaveContext.hasWarpTransfer = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_WARP_TRANSFER_INTRO);
            } else if (gSaveContext.playerBalance >= 1000 && !gSaveContext.hasFee) {
                gSaveContext.hasFee = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_FEE);
            } else if (gSaveContext.playerBalance >= 5000 && !gSaveContext.hasPieceOfHeart) {
                gSaveContext.hasPieceOfHeart = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_PIECE_OF_HEART);
            } else {
                nextTextId = (OptionChoice == 0) ? TEXT_BANKER_DEPOSIT_AMOUNT : TEXT_BANKER_WITHDRAWAL_AMOUNT;
                Message_StartTextbox(play, nextTextId, bankerActor);
            }
            break;

        case TEXT_BANKER_WITHDRAWAL_AMOUNT:
            if (bankerValue == 0) {
                Message_ContinueTextbox(play, TEXT_BANKER_ERROR_ZERO_AMOUNT);
            } else if (bankerValue + (gSaveContext.hasFee ? 0 : gSaveContext.rupeesFee) > gSaveContext.playerBalance) {
                Message_ContinueTextbox(play, TEXT_BANKER_ERROR_INSUFFICIENT_BALANCE);
            } else if (bankerValue + gSaveContext.rupees > CUR_CAPACITY(UPG_WALLET)) {
                Message_ContinueTextbox(play, TEXT_BANKER_ERROR_WALLET_FULL);
            } else {
                Rupees_ChangeBy(bankerValue);
                gSaveContext.playerBalance -= bankerValue + (gSaveContext.hasFee ? 0 : gSaveContext.rupeesFee);
                Message_ContinueTextbox(play, TEXT_BANKER_WITHDRAWAL_CONFIRM);
            }
            break;

        case TEXT_BANKER_DEPOSIT_AMOUNT:
            if (bankerValue == 0) {
                Message_ContinueTextbox(play, TEXT_BANKER_ERROR_ZERO_AMOUNT);
            } else if ((gSaveContext.playerBalance + bankerValue) > 5000) {
                Message_ContinueTextbox(play, TEXT_BANKER_ERROR_MAX_BALANCE);
            } else if (bankerValue > gSaveContext.rupees) {
                Message_ContinueTextbox(play, TEXT_BANKER_ERROR_INSUFFICIENT_BALANCE);
            } else if (gSaveContext.hasFee == 0 && (gSaveContext.playerBalance + bankerValue - gSaveContext.rupeesFee) < 1) {
                Message_ContinueTextbox(play, TEXT_BANKER_ERROR_DEPOSIT_NOT_WORTHWHILE);
            } else {
                Rupees_ChangeBy(-bankerValue);
                gSaveContext.playerBalance += bankerValue - (gSaveContext.hasFee ? 0 : gSaveContext.rupeesFee);
                Message_ContinueTextbox(play, TEXT_BANKER_DEPOSIT_CONFIRM);
            }
            break;

        case TEXT_BANKER_WITHDRAWAL_CONFIRM:
        case TEXT_BANKER_DEPOSIT_CONFIRM:
            bankerValue = 0;
            canContinue = true;
            if (gSaveContext.playerBalance >= 200 && !gSaveContext.hasWarpTransfer) {
                gSaveContext.hasWarpTransfer = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_WARP_TRANSFER_INTRO);
            } else if (gSaveContext.playerBalance >= 1000 && !gSaveContext.hasFee) {
                gSaveContext.hasFee = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_FEE);
            } else if (gSaveContext.playerBalance >= 5000 && !gSaveContext.hasPieceOfHeart) {
                gSaveContext.hasPieceOfHeart = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_PIECE_OF_HEART);
            }
            break;

        case TEXT_BANKER_REWARD_WARP_TRANSFER_INTRO:
            Message_ContinueTextbox(play, TEXT_BANKER_REWARD_WARP_TRANSFER_ITEM);
            break;

        case TEXT_BANKER_REWARD_WARP_TRANSFER_ITEM:
            Message_CloseTextbox(play);
            isGivingCharm = true;
            break;
        //TEXT_BLUE_RUPEE is a placeholder for TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_1. GI_PIRATE_CHARM has not been implemented yet.
        case TEXT_BLUE_RUPEE: 
            Message_CloseTextbox(play);
            prevTextboxCharm = true;
            break;

        case TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_2:
            Message_ContinueTextbox(play, TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_3);
            break;

        case TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_3:
            if (gSaveContext.playerBalance >= 1000 && !gSaveContext.hasFee) {
                gSaveContext.hasFee = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_FEE);
            } else if (canContinue) {
                canContinue = false;
            } else {
                nextTextId = (OptionChoice == 0) ? TEXT_BANKER_DEPOSIT_AMOUNT : TEXT_BANKER_WITHDRAWAL_AMOUNT;
                Message_StartTextbox(play, nextTextId, bankerActor);
            }
            break;

        case TEXT_BANKER_REWARD_FEE:
            if (gSaveContext.playerBalance >= 5000 && !gSaveContext.hasPieceOfHeart) {
                gSaveContext.hasPieceOfHeart = 1;
                Message_ContinueTextbox(play, TEXT_BANKER_REWARD_PIECE_OF_HEART);
            } else if (canContinue) {
                canContinue = false;
            } else {
                nextTextId = (OptionChoice == 0) ? TEXT_BANKER_DEPOSIT_AMOUNT : TEXT_BANKER_WITHDRAWAL_AMOUNT;
                Message_StartTextbox(play, nextTextId, bankerActor);
            }
            break;

        case TEXT_BANKER_REWARD_PIECE_OF_HEART:
            Message_CloseTextbox(play);
            isGivingHeart = true;
            break;

        case TEXT_HEART_PIECE:
            Message_CloseTextbox(play);
            prevTextboxHeart = true;
            break;

        case TEXT_BANKER_ERROR_ZERO_AMOUNT:
        case TEXT_BANKER_ERROR_INSUFFICIENT_BALANCE:
        case TEXT_BANKER_ERROR_WALLET_FULL:
        case TEXT_BANKER_ERROR_MAX_BALANCE:
        case TEXT_BANKER_ERROR_DEPOSIT_NOT_WORTHWHILE:
            Message_ContinueTextbox(play, TEXT_BANKER_OPTIONS);
            break;

        case TEXT_BANKER_TRANSACTION_FEE:
            if (msgCtx->choiceIndex == 0) {
                Message_ContinueTextbox(play, TEXT_BANKER_BALANCE);
            } else if (msgCtx->choiceIndex == 1) {
                Message_CloseTextbox(play);
            }
            break;

        default:
            break;
    }
}

void DrawBankerOverlay(PlayState* play, Gfx** gfx) {
    UpdateBankerOverlay(play, gfx, bankerValue, selectedDigit);
}

void BankerMain(PlayState* play, GraphicsContext* gfxCtx) {
    static s16 prevMsgMode = MSGMODE_NONE;
    bool isMsgModeActive = play->msgCtx.msgMode != MSGMODE_NONE;
    bool wasMsgModeActive = prevMsgMode != MSGMODE_NONE;
    if (!isMsgModeActive && !wasMsgModeActive) {
        return;
    }
    if (isMsgModeActive) {
        ProcessInput(play, &bankerValue, &selectedDigit);
        blinkTimer = (blinkTimer + 1) % (BLINK_DURATION * 2);
        bool isBankerTextId = play->msgCtx.textId == TEXT_BANKER_WITHDRAWAL_AMOUNT || play->msgCtx.textId == TEXT_BANKER_DEPOSIT_AMOUNT;
        if (isBankerTextId) {
            Gfx** gfx = &gfxCtx->overlay.p;
        }
    }
    if (wasMsgModeActive && !isMsgModeActive) {
        if (isGivingHeart) {
            isGivingHeart = GiveItemEntryWithoutActor(play, ItemTable_Retrieve(GI_HEART_PIECE)) ? false : true;
        } else if (isGivingCharm) {
            //GI_RUPEE_BLUE is a placeholder for GI_PIRATE_CHARM, which has not been implemented yet.
            isGivingCharm = GiveItemEntryWithoutActor(play, ItemTable_Retrieve(GI_RUPEE_BLUE)) ? false : true;
        } else if (!canContinue && prevTextboxHeart) {
            s16 nextTextId = OptionChoice == 0 ? TEXT_BANKER_DEPOSIT_AMOUNT : TEXT_BANKER_WITHDRAWAL_AMOUNT;
            bankerActor->textId = nextTextId;
            Player_StartTalking(play, bankerActor);
            Message_StartTextbox(play, nextTextId, bankerActor);
            prevTextboxHeart = false;
            canContinue = true;
        } else if (prevTextboxCharm) {
            s16 charmTextID = TEXT_BANKER_REWARD_WARP_TRANSFER_LORE_2;
            bankerActor->textId = charmTextID;
            Player_StartTalking(play, bankerActor);
            Message_StartTextbox(play, charmTextID, bankerActor);
            prevTextboxCharm = false;
        }
    }
    prevMsgMode = play->msgCtx.msgMode;
    if (isMsgModeActive) {
        HandleBankerInteraction(play, &play->msgCtx);
    }
}
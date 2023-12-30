#include "z64.h"
#include "macros.h"
#include "libultraship/libultra/gbi.h"
#include "../soh/assets/textures/parameter_static/parameter_static.h"
#include "variables.h"
#include "custom-message/CustomMessageTypes.h"

#include "luslog.h"

#define DIGIT_WIDTH 8
#define DIGIT_HEIGHT 16
#define HUNDREDS_POSITION_OFFSET -16
#define TENS_POSITION_OFFSET 8
#define ONES_POSITION_OFFSET 8
#define BLINK_DURATION 10
#define INPUT_COOLDOWN_FRAMES 3

static s16 gBankerValue = 0;
static s16 gBankerSelectedDigit = 0;
static s16 gBlinkTimer = 0;
static s16 gTempWithdrawalAmount = 0;

extern const char* digitTextures[];
Color_RGBA8 highlightColor = { 255, 255, 0, 255 };

// This function is responsible for drawing a single digit on the screen.
void DrawDigit(PlayState* play, Gfx** gfx, s16 digit, s16 posX, s16 posY, bool isSelected) {
    gDPPipeSync((*gfx)++);

    gDPLoadTextureBlock((*gfx)++, ((u8*)digitTextures[digit]), G_IM_FMT_I, G_IM_SIZ_8b, DIGIT_WIDTH, DIGIT_HEIGHT, 0,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Color_RGBA8 primColor = isSelected && gBlinkTimer < BLINK_DURATION ? highlightColor : (Color_RGBA8){255, 255, 255, 255};
    gDPSetPrimColor((*gfx)++, 0, 0, primColor.r, primColor.g, primColor.b, primColor.a);

    gDPSetCombineMode((*gfx)++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    gDPSetRenderMode((*gfx)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

    gSPTextureRectangle((*gfx)++, posX << 2, posY << 2, (posX + DIGIT_WIDTH) << 2, (posY + DIGIT_HEIGHT) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
}

// This function updates the digit overlay on the screen.
void UpdateDigitOverlay(PlayState* play, Gfx** gfx, s16 value, s16 selectedDigit) {
    s16 posX = 160 - 76;
    s16 posY = 180 - 20;

    posX += HUNDREDS_POSITION_OFFSET;
    DrawDigit(play, gfx, value / 100, posX, posY, selectedDigit == 2);

    posX += TENS_POSITION_OFFSET;
    DrawDigit(play, gfx, (value % 100) / 10, posX, posY, selectedDigit == 1);

    posX += ONES_POSITION_OFFSET;
    DrawDigit(play, gfx, value % 10, posX, posY, selectedDigit == 0);
}

// This function calculates the new value based on the selected digit and whether the value should increase or decrease.
s16 CalculateNewValue(s16 value, s16 selectedDigit, bool increase) {
    s16 digitValues[3];
    digitValues[0] = value % 10;
    digitValues[1] = (value / 10) % 10;
    digitValues[2] = value / 100;

    if (increase) {
        digitValues[selectedDigit] = (digitValues[selectedDigit] + 1) % 10;
    } else {
        digitValues[selectedDigit] = (digitValues[selectedDigit] + 9) % 10;
    }

    return digitValues[2] * 100 + digitValues[1] * 10 + digitValues[0];
}

// This function ensures that the value stays within the specified range.
s16 ClampValue(s16 value, s16 min, s16 max) {
    return (value < min) ? min : (value > max) ? max : value;
}

// This function processes the vertical stick movement and updates the value accordingly.
bool ProcessVerticalStickMovement(Input* input, s16* value, s16* selectedDigit) {
    if (abs(input->rel.stick_y) > 30) {
        *value = CalculateNewValue(*value, *selectedDigit, input->rel.stick_y > 0);
        *value = ClampValue(*value, 0, 999);
        gBankerValue = *value;
        return true;
    }
    return false;
}

// This function adjusts the selected digit based on the horizontal stick movement.
s16 AdjustDigit(s16 stick_x, s16 selectedDigit) {
    return (stick_x < 0) ? (selectedDigit + 1) % 3 : (selectedDigit - 1 + 3) % 3;
}

// This function processes the horizontal stick movement and updates the selected digit accordingly.
bool ProcessHorizontalStickMovement(Input* input, s16* selectedDigit) {
    if (abs(input->rel.stick_x) > 30) {
        *selectedDigit = AdjustDigit(input->rel.stick_x, *selectedDigit);
        return true;
    }
    return false;
}

// This function plays a sound effect if the playSound parameter is true.
void PlaySoundEffect(bool playSound) {
    if (playSound) {
        Audio_PlaySoundGeneral(NA_SE_SY_RUPY_COUNT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

// This function processes the input and updates the value and selected digit accordingly.
void ProcessInput(PlayState* play, s16* value, s16* selectedDigit) {
    static s16 inputCooldownTimer = 0;
    Input* input = &play->state.input[0];
    bool playSound = false;

    if (play->msgCtx.textId != TEXT_BANKER_WITHDRAWAL_AMOUNT && play->msgCtx.textId != TEXT_BANKER_DEPOSIT_AMOUNT) {
        return;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Message_CloseTextbox(play);
        return;
    }

    if (inputCooldownTimer > 0) {
        inputCooldownTimer--;
        return;
    }

    playSound = ProcessVerticalStickMovement(input, value, selectedDigit);

    playSound |= ProcessHorizontalStickMovement(input, selectedDigit);

    PlaySoundEffect(playSound);

    inputCooldownTimer = playSound ? INPUT_COOLDOWN_FRAMES : 0;
}

// This function handles the banker withdrawal amount.
void HandleBankerWithdrawalAmount(PlayState* play, MessageContext* msgCtx) {
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A)) {
        if (gBankerValue + gSaveContext.rupees <= CUR_CAPACITY(UPG_WALLET)) {
            Rupees_ChangeBy(gBankerValue);

            gSaveContext.playerBalance -= gBankerValue;
        }
    }
}

// This function handles the banker deposit amount.
void HandleBankerDepositAmount(PlayState* play, MessageContext* msgCtx) {
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A)) {
        if (gBankerValue <= gSaveContext.rupees) {
            Rupees_ChangeBy(-gBankerValue);

            gSaveContext.playerBalance += gBankerValue;
        }
    }
}

// This function handles the banker options.
static void HandleBankerOptions(PlayState* play, MessageContext* msgCtx) {
    if (Message_ShouldAdvance(play)) {
        switch (msgCtx->choiceIndex) {
            case 0:
            case 1:
                Message_ContinueTextbox(play, TEXT_BANKER_BALANCE, NULL);
                break;
            case 2:
                Message_CloseTextbox(play);
                break;
            default:
                break;
        }
    }
}

// This function handles the banker balance.
static void HandleBankerBalance(PlayState* play, MessageContext* msgCtx) {
    if (Message_ShouldAdvance(play)) {
        s16 nextTextId = (msgCtx->choiceIndex == 0) ? TEXT_BANKER_DEPOSIT_AMOUNT : TEXT_BANKER_WITHDRAWAL_AMOUNT;
        Message_ContinueTextbox(play, nextTextId);
    }
}

// This function handles the banker withdrawal.
static void HandleBankerWithdrawal(PlayState* play, MessageContext* msgCtx) {
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A)) {
        if (gBankerValue + gSaveContext.rupees <= CUR_CAPACITY(UPG_WALLET)) {
            HandleBankerWithdrawalAmount(play, msgCtx);
            Message_ContinueTextbox(play, TEXT_BANKER_WITHDRAWAL_CONFIRM);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
}

// This function handles the banker deposit.
static void HandleBankerDeposit(PlayState* play, MessageContext* msgCtx) {
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A)) {
        if (gBankerValue <= gSaveContext.rupees) {
            HandleBankerDepositAmount(play, msgCtx);
            Message_ContinueTextbox(play, TEXT_BANKER_DEPOSIT_CONFIRM);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
}

// This function handles the banker confirmation.
static void HandleBankerConfirmation(PlayState* play, MessageContext* msgCtx, s16 textId) {
    if (Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
    }
}

// This function handles the banker textbox.
void HandleBankerTextbox(PlayState* play, MessageContext* msgCtx) {
    switch (msgCtx->textId) {
        case TEXT_BEGGAR_VANILLA:
            if (Message_ShouldAdvance(play)) {
                Message_ContinueTextbox(play, TEXT_BANKER_OPTIONS);
            }
            break;
        case TEXT_BANKER_OPTIONS:
            HandleBankerOptions(play, msgCtx);
            break;
        case TEXT_BANKER_BALANCE:
            HandleBankerBalance(play, msgCtx);
            break;
        case TEXT_BANKER_WITHDRAWAL_AMOUNT:
            HandleBankerWithdrawal(play, msgCtx);
            break;
        case TEXT_BANKER_DEPOSIT_AMOUNT:
            HandleBankerDeposit(play, msgCtx);
            break;
        case TEXT_BANKER_WITHDRAWAL_CONFIRM:
        case TEXT_BANKER_DEPOSIT_CONFIRM:
            HandleBankerConfirmation(play, msgCtx, msgCtx->textId);
            break;
        default:
            break;
    }
}

// This function draws the banker digit overlay.
void DrawBankerDigitOverlay(PlayState* play, Gfx** gfx) {
    UpdateDigitOverlay(play, gfx, gBankerValue, gBankerSelectedDigit);
}

// This is the main function for the banker.
void BankerMain(PlayState* play, GraphicsContext* gfxCtx) {
    ProcessInput(play, &gBankerValue, &gBankerSelectedDigit);

    gBlinkTimer = (gBlinkTimer + 1) % (BLINK_DURATION * 2);

    if (play->msgCtx.textId == TEXT_BANKER_WITHDRAWAL_AMOUNT || play->msgCtx.textId == TEXT_BANKER_DEPOSIT_AMOUNT) {
        Gfx** gfx = &gfxCtx->overlay.p;
        UpdateDigitOverlay(play, gfx, gBankerValue, gBankerSelectedDigit);
    }

    HandleBankerTextbox(play, &play->msgCtx);
}
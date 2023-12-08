// banker.c
#include "z64.h"
#include "macros.h"
#include "libultraship/libultra/gbi.h"
#include "../soh/assets/textures/parameter_static/parameter_static.h"
#include "variables.h"
#include "custom-message/CustomMessageTypes.h"

#include "luslog.h"

// Constants
#define DIGIT_WIDTH 8
#define DIGIT_HEIGHT 16
#define HUNDREDS_POSITION_OFFSET -16
#define TENS_POSITION_OFFSET 8
#define ONES_POSITION_OFFSET 8
#define BLINK_DURATION 10
#define INPUT_COOLDOWN_FRAMES 3

// Global variables
static s16 gBankerValue = 0;
static s16 gBankerSelectedDigit = 0;
static s16 gBlinkTimer = 0;
static s16 gTempWithdrawalAmount = 0;

// External variables
extern const char* digitTextures[];
Color_RGBA8 highlightColor = { 255, 255, 0, 255 };

// Function prototypes
void DrawDigit(PlayState* play, Gfx** gfx, s16 digit, s16 posX, s16 posY, bool isSelected);
void UpdateDigitOverlay(PlayState* play, Gfx** gfx, s16 value, s16 selectedDigit);
void ProcessInput(PlayState* play, s16* value, s16* selectedDigit);
void HandleBankerOptions(PlayState* play, MessageContext* msgCtx);
void HandleBankerWithdrawalAmount(PlayState* play, MessageContext* msgCtx);

// Function to draw a single digit
void DrawDigit(PlayState* play, Gfx** gfx, s16 digit, s16 posX, s16 posY, bool isSelected) {
    gDPPipeSync((*gfx)++);

    // Load the digit texture
    gDPLoadTextureBlock((*gfx)++, ((u8*)digitTextures[digit]), G_IM_FMT_I, G_IM_SIZ_8b, DIGIT_WIDTH, DIGIT_HEIGHT, 0,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    // If the digit is selected and the blink timer is in the "on" phase of the blink cycle, set the highlight color
    if (isSelected && gBlinkTimer < BLINK_DURATION) {
        gDPSetPrimColor((*gfx)++, 0, 0, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
    } else {
        // Set the primitive color to white with full alpha for non-selected digits or "off" phase of the blink
        gDPSetPrimColor((*gfx)++, 0, 0, 255, 255, 255, 255);
    }

    // Set the combine mode to blend the texture with the primitive color using alpha
    gDPSetCombineMode((*gfx)++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    // Set the render mode for blending with alpha
    gDPSetRenderMode((*gfx)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

    // Draw the digit texture rectangle
    gSPTextureRectangle((*gfx)++, posX << 2, posY << 2, (posX + DIGIT_WIDTH) << 2, (posY + DIGIT_HEIGHT) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
}

// Function to update and draw the 3-digit overlay
void UpdateDigitOverlay(PlayState* play, Gfx** gfx, s16 value, s16 selectedDigit) {
    s16 posX = 160 - 76; // X position of the rightmost digit (ones place)
    s16 posY = 180 - 20; // Y position of the digits

    // Draw hundreds place
    posX += HUNDREDS_POSITION_OFFSET;
    DrawDigit(play, gfx, value / 100, posX, posY, selectedDigit == 2);

    // Draw tens place
    posX += TENS_POSITION_OFFSET;
    DrawDigit(play, gfx, (value % 100) / 10, posX, posY, selectedDigit == 1);

    // Draw ones place
    posX += ONES_POSITION_OFFSET;
    DrawDigit(play, gfx, value % 10, posX, posY, selectedDigit == 0);
}

// Helper function to calculate the new value based on the digit position and direction
s16 CalculateNewValue(s16 value, s16 selectedDigit, bool increase) {
    s16 digitValues[3];
    digitValues[0] = value % 10;        // ones
    digitValues[1] = (value / 10) % 10; // tens
    digitValues[2] = value / 100;       // hundreds

    // Increase or decrease the selected digit
    if (increase) {
        digitValues[selectedDigit] = (digitValues[selectedDigit] + 1) % 10;
    } else {
        digitValues[selectedDigit] = (digitValues[selectedDigit] + 9) % 10; // Corrected to wrap around properly
    }

    // Recalculate the total value
    return digitValues[2] * 100 + digitValues[1] * 10 + digitValues[0];
}

void ProcessInput(PlayState* play, s16* value, s16* selectedDigit) {
    static s16 inputCooldownTimer = 0; // Cooldown timer for input
    Input* input = &play->state.input[0];
    bool playSound = false;

    // Only process input if the TEXT_BANKER_WITHDRAWAL_AMOUNT textbox is present
    if (play->msgCtx.textId == TEXT_BANKER_WITHDRAWAL_AMOUNT) {
        // Check if the B button was pressed
        if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
            // Close the textbox
            Message_CloseTextbox(play);
            return;
        }
        // Check if the cooldown timer has elapsed
        if (inputCooldownTimer <= 0) {
            // Process vertical stick movement (up/down)
            if (input->rel.stick_y > 30 || input->rel.stick_y < -30) {
                // Adjust the selected digit's value
                *value = CalculateNewValue(*value, *selectedDigit, input->rel.stick_y > 0);
                *value = (*value < 0) ? 0 : (*value > 999) ? 999 : *value; // Clamp the value between 0 and 999
                gBankerValue = *value; // Update gBankerValue with the selected withdrawal amount
                playSound = true;
                // Reset the cooldown timer
                inputCooldownTimer = INPUT_COOLDOWN_FRAMES;
            }

            // Process horizontal stick movement (left/right)
            if (input->rel.stick_x > 30 || input->rel.stick_x < -30) {
                // Adjust the selected digit
                if (input->rel.stick_x < 0) { // Stick moved to the left
                    *selectedDigit = (*selectedDigit + 1) % 3;
                } else { // Stick moved to the right
                    *selectedDigit = (*selectedDigit - 1 + 3) % 3;
                }
                playSound = true;
                // Reset the cooldown timer
                inputCooldownTimer = INPUT_COOLDOWN_FRAMES;
            }
        } else {
            // Decrement the cooldown timer
            inputCooldownTimer--;
        }

        // Play the rupee sound effect if a digit was incremented or decremented, or if the selected digit changed
        if (playSound) {
            Audio_PlaySoundGeneral(NA_SE_SY_RUPY_COUNT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
}

void HandleBankerWithdrawalAmount(PlayState* play, MessageContext* msgCtx) {
    // Check if the A button was pressed to confirm the withdrawal amount
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A)) {
        // Check if the player's wallet can carry the withdrawal amount
        if (gBankerValue + gSaveContext.rupees <= CUR_CAPACITY(UPG_WALLET)) {
            // Use Rupees_ChangeBy to add the withdrawal amount to the player's wallet
            Rupees_ChangeBy(gBankerValue);

            // Deduct the withdrawal amount from the player's balance
            gSaveContext.playerBalance -= gBankerValue;
        }
    }
}

void HandleBankerTextbox(PlayState* play, MessageContext* msgCtx) {
    Input* input = &play->state.input[0];

    if (msgCtx->textId == TEXT_BEGGAR_VANILLA && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, TEXT_BANKER_OPTIONS);
    } else if (msgCtx->textId == TEXT_BANKER_OPTIONS && Message_ShouldAdvance(play)) {
        switch (msgCtx->choiceIndex) {
            case 0: // Sell Something
                Message_ContinueTextbox(play, TEXT_BEGGAR_VANILLA, NULL);
                break;
            case 1: // Withdrawal Rupees
                Message_ContinueTextbox(play, TEXT_BANKER_BALANCE, NULL);
                break;
            case 2: // No
                Message_CloseTextbox(play);
                break;
            default:
                break;
        }
    } else if (msgCtx->textId == TEXT_BANKER_BALANCE && Message_ShouldAdvance(play)) {
        Message_ContinueTextbox(play, TEXT_BANKER_WITHDRAWAL_AMOUNT);
    } else if (msgCtx->textId == TEXT_BANKER_WITHDRAWAL_AMOUNT) {
        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
            if (gBankerValue + gSaveContext.rupees <= CUR_CAPACITY(UPG_WALLET)) {
                HandleBankerWithdrawalAmount(play, msgCtx);
                Message_ContinueTextbox(play, TEXT_BANKER_WITHDRAWAL_CONFIRM);
            } else {
                // Play the error sound
                Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
    } else if (msgCtx->textId == TEXT_BANKER_WITHDRAWAL_CONFIRM && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
    }
}

void DrawBankerDigitOverlay(PlayState* play, Gfx** gfx) {
    // Call UpdateDigitOverlay to draw the overlay with the current value and selected digit
    UpdateDigitOverlay(play, gfx, gBankerValue, gBankerSelectedDigit);
}

// Main function to initialize and update the overlay
void BankerMain(PlayState* play, GraphicsContext* gfxCtx) {
    ProcessInput(play, &gBankerValue, &gBankerSelectedDigit);

    // Increment the blink timer and reset if it exceeds the duration
    gBlinkTimer = (gBlinkTimer + 1) % (BLINK_DURATION * 2);

    // Check if the current textbox is TEXT_BANKER_WITHDRAWAL
    if (play->msgCtx.textId == TEXT_BANKER_WITHDRAWAL_AMOUNT) {
        // Update the digit overlay only if the correct textbox
        Gfx** gfx = &gfxCtx->overlay.p;
        UpdateDigitOverlay(play, gfx, gBankerValue, gBankerSelectedDigit);
    }

    // Handle the banker textbox logic
    HandleBankerTextbox(play, &play->msgCtx);
}
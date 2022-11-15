#include "global.h"
#include "message_data_static.h"
#include "vt.h"

#include <string.h>

#include "textures/parameter_static/parameter_static.h"
#include "textures/message_static/message_static.h"
#include "textures/message_texture_static/message_texture_static.h"

s16 sTextFade = false; // original name: key_off_flag ?

u8 D_8014B2F4 = 0;

s16 sOcarinaNoteBufPos = 0;

s16 sOcarinaNoteBufLen = 0;

u8 sTextboxSkipped = false;

u16 sNextTextId = 0;

s16 sTextIsCredits = false;

UNK_TYPE D_8014B30C = 0;

s16 sLastPlayedSong = 0xFF; // last played song?

s16 sHasSunsSong = false;

s16 sMessageHasSetSfx = false;

u16 sOcarinaSongBitFlags = 0; // ocarina bit flags

MessageTableEntry* sNesMessageEntryTablePtr;
MessageTableEntry* sGerMessageEntryTablePtr;
MessageTableEntry* sFraMessageEntryTablePtr;
MessageTableEntry* sStaffMessageEntryTablePtr;

char* _message_0xFFFC_nes;

s16 sTextboxBackgroundForePrimColors[][3] = {
    { 255, 255, 255 }, { 50, 20, 0 },     { 255, 60, 0 },    { 255, 255, 255 },
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
};

s16 sTextboxBackgroundBackPrimColors[][3] = {
    { 0, 0, 0 },
    { 220, 150, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
};

s16 sTextboxBackgroundYOffsets[] = {
    1,
    2,
};

// original name: onpu_buff
u8 sOcarinaNoteBuf[12] = { 0 };

s16 sOcarinaNotesAlphaValues[9] = { 0 };

// Maps the ocarina song order to the quest item order
s16 gOcarinaSongItemMap[] = {
    OCARINA_SONG_MINUET,   OCARINA_SONG_BOLERO,  OCARINA_SONG_SERENADE, OCARINA_SONG_REQUIEM,
    OCARINA_SONG_NOCTURNE, OCARINA_SONG_PRELUDE, OCARINA_SONG_LULLABY,  OCARINA_SONG_EPONAS,
    OCARINA_SONG_SARIAS,   OCARINA_SONG_SUNS,    OCARINA_SONG_TIME,     OCARINA_SONG_STORMS,
};

s32 sCharTexSize;
s32 sCharTexScale;

Color_RGB8 sOcarinaNoteABtnEnv;
Color_RGB8 sOcarinaNoteCBtnEnv;
Color_RGB8 sOcarinaNoteCBtnPrim;

Color_RGB8 sOcarinaNoteD5Prim;
Color_RGB8 sOcarinaNoteB4Prim;
Color_RGB8 sOcarinaNoteA4Prim;
Color_RGB8 sOcarinaNoteF4Prim;
Color_RGB8 sOcarinaNoteD4Prim;

// If the "separate" bool is set, use the "gCCC<cDirection>BtnPrim CVars
// to set the button color. Otherwise, use sOcarinaNoteCBtnPrim.
void Message_SetCustomOrGeneralCColor(Color_RGB8* color, bool separate, char cDirection) {
    if (!separate) {
        *color = sOcarinaNoteCBtnPrim;
        return;
    }

    // C direction is '*' @ idx 4
    char cVar[] = "gCCC*BtnPrim";
    cVar[4] = cDirection;
    *color = CVar_GetRGB(cVar, (Color_RGB8){ .r = 255, .g = 255, .b = 50});
}

typedef struct {
    bool separateC;
    bool dpad;
    bool rightStick;
} CustomNoteOptions;

// Set the non-null button color corresponding to the button map
//
// If the assigned button is A, a C button, Start, or the D-pad, then it'll be
// colored to the corresponding CVar. Otherwise, its color will be equal to
// sOcarinaNoteCBtnPrim.
void Message_SetCustomOcarinaNoteColor(Color_RGB8* color, s32 btnMap, CustomNoteOptions* flags) {
    switch (btnMap) {
        case BTN_A:
            *color = CVar_GetRGB("gCCABtnPrim", (Color_RGB8){ .r = 80, .g = 255, .b = 150 });
            break;
        case BTN_CUP:
            Message_SetCustomOrGeneralCColor(color, flags->separateC, 'U');
            break;
        case BTN_CDOWN:
            Message_SetCustomOrGeneralCColor(color, flags->separateC, 'D');
            break;
        case BTN_CLEFT:
            Message_SetCustomOrGeneralCColor(color, flags->separateC, 'L');
            break;
        case BTN_CRIGHT:
            Message_SetCustomOrGeneralCColor(color, flags->separateC, 'R');
            break;
        case BTN_START:
            *color = CVar_GetRGB("gCCStartBtnPrim", (Color_RGB8){ .r = 200, .g = 0, .b = 0 });
            break;
        case BTN_DUP:
        case BTN_DDOWN:
        case BTN_DLEFT:
        case BTN_DRIGHT:
            *color = CVar_GetRGB("gCCDpadPrim", (Color_RGB8){ .r = 255, .g = 255, .b = 255 });
            color->r *= 103 / 255;
            color->g *= 103 / 255;
            color->b *= 103 / 255;
            break;
        case 0:
            if (flags->dpad && !flags->rightStick) {
                // D pad is dark gray even when set to white, so emulate that.
                *color = CVar_GetRGB("gCCDpadPrim", (Color_RGB8){ .r = 255, .g = 255, .b = 255 });
                color->r *= 103 / 255;
                color->g *= 103 / 255;
                color->b *= 103 / 255;
                break;
            }  // else fall through
        default:
            *color = sOcarinaNoteCBtnPrim;
            break;
    }
}

void Message_ResetOcarinaNoteState(void) {
    R_OCARINA_NOTES_YPOS(0) = 189;
    R_OCARINA_NOTES_YPOS(1) = 184;
    R_OCARINA_NOTES_YPOS(2) = 179;
    R_OCARINA_NOTES_YPOS(3) = 174;
    R_OCARINA_NOTES_YPOS(4) = 169;
    sOcarinaNoteBuf[0] = 0xFF;
    sOcarinaNotesAlphaValues[0] = sOcarinaNotesAlphaValues[1] = sOcarinaNotesAlphaValues[2] =
        sOcarinaNotesAlphaValues[3] = sOcarinaNotesAlphaValues[4] = sOcarinaNotesAlphaValues[5] =
            sOcarinaNotesAlphaValues[6] = sOcarinaNotesAlphaValues[7] = sOcarinaNotesAlphaValues[8] = 0;
    sOcarinaNoteABtnEnv = (Color_RGB8){ .r = 10, .g = 10, .b = 10 };
    sOcarinaNoteCBtnEnv = (Color_RGB8){ .r = 10, .g = 10, .b = 10 };

    if (CVar_GetS32("gHudColors", 1) != 2) {  // N64/GameCube
        if (CVar_GetS32("gHudColors", 1) == 0) {
            sOcarinaNoteD4Prim = (Color_RGB8){ .r = 80, .g = 150, .b = 255 };
        } else if (CVar_GetS32("gHudColors", 1) == 1) {
            sOcarinaNoteD4Prim = (Color_RGB8){ .r = 80, .g = 255, .b = 150 };
        }

        sOcarinaNoteCBtnPrim = (Color_RGB8){ .r = 255, .g = 255, .b = 50 };
        sOcarinaNoteD5Prim = sOcarinaNoteCBtnPrim;
        sOcarinaNoteB4Prim = sOcarinaNoteCBtnPrim;
        sOcarinaNoteA4Prim = sOcarinaNoteCBtnPrim;
        sOcarinaNoteF4Prim = sOcarinaNoteCBtnPrim;
    } else {  // Custom
        sOcarinaNoteCBtnPrim = CVar_GetRGB("gCCCBtnPrim", (Color_RGB8){ .r = 255, .g = 255, .b = 50 });

        CustomNoteOptions options = (CustomNoteOptions){
            .separateC = CVar_GetS32("gCCparated", 0),
            .dpad = CVar_GetS32("gDpadOcarina", 0),
            .rightStick = CVar_GetS32("gDpadRStick", 0),
        };

        if (CVar_GetS32("gCustomOcarinaControls", 0)) {
            Message_SetCustomOcarinaNoteColor(&sOcarinaNoteD5Prim, CVar_GetS32("gOcarinaD5BtnMap", BTN_CUP), &options);
            Message_SetCustomOcarinaNoteColor(&sOcarinaNoteB4Prim, CVar_GetS32("gOcarinaB4BtnMap", BTN_CLEFT), &options);
            Message_SetCustomOcarinaNoteColor(&sOcarinaNoteA4Prim, CVar_GetS32("gOcarinaA4BtnMap", BTN_CRIGHT), &options);
            Message_SetCustomOcarinaNoteColor(&sOcarinaNoteF4Prim, CVar_GetS32("gOcarinaF4BtnMap", BTN_CDOWN), &options);
            Message_SetCustomOcarinaNoteColor(&sOcarinaNoteD4Prim, CVar_GetS32("gOcarinaD4BtnMap", BTN_A), &options);
        } else {
            Message_SetCustomOrGeneralCColor(&sOcarinaNoteD5Prim, options.separateC, 'U');
            Message_SetCustomOrGeneralCColor(&sOcarinaNoteB4Prim, options.separateC, 'L');
            Message_SetCustomOrGeneralCColor(&sOcarinaNoteA4Prim, options.separateC, 'R');
            Message_SetCustomOrGeneralCColor(&sOcarinaNoteF4Prim, options.separateC, 'D');
            sOcarinaNoteD4Prim = CVar_GetRGB("gCCABtnPrim", (Color_RGB8){ .r = 80, .g = 255, .b = 150 });
        }
    }
}

void Message_UpdateOcarinaGame(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    play->msgCtx.msgMode++;

    if (play->msgCtx.msgMode == MSGMODE_MEMORY_GAME_PLAYER_PLAYING) {
        Audio_OcaSetInstrument(1);
        msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
        msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
        func_800ECC04((1 << OCARINA_SONG_MEMORY_GAME) + 0x8000);
        msgCtx->textDrawPos = msgCtx->decodedTextLen;
    } else if (msgCtx->msgMode == MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_PLAYING) {
        Audio_OcaSetInstrument(6);
        msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
        msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
        Audio_OcaSetSongPlayback(OCARINA_SONG_MEMORY_GAME + 1, 1);
        msgCtx->stateTimer = 2;
    }
    Message_ResetOcarinaNoteState();
}

u8 Message_ShouldAdvance(PlayState* play) {
    Input* input = &play->state.input[0];

    bool isB_Held = CVar_GetS32("gSkipText", 0) != 0 ? CHECK_BTN_ALL(input->cur.button, BTN_B)
                                                     : CHECK_BTN_ALL(input->press.button, BTN_B);

    if (CHECK_BTN_ALL(input->press.button, BTN_A) || isB_Held || CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
        Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_PASS, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    return CHECK_BTN_ALL(input->press.button, BTN_A) || isB_Held || CHECK_BTN_ALL(input->press.button, BTN_CUP);
}

u8 Message_ShouldAdvanceSilent(PlayState* play) {
    Input* input = &play->state.input[0];

    bool isB_Held = CVar_GetS32("gSkipText", 0) != 0 ? CHECK_BTN_ALL(input->cur.button, BTN_B)
                                                     : CHECK_BTN_ALL(input->press.button, BTN_B);

    return CHECK_BTN_ALL(input->press.button, BTN_A) || isB_Held || CHECK_BTN_ALL(input->press.button, BTN_CUP);
}

/**
 * Closes any currently displayed textbox immediately, without waiting for
 * input from the player.
 */
void Message_CloseTextbox(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (msgCtx->msgLength != 0) {
        msgCtx->stateTimer = 2;
        msgCtx->msgMode = MSGMODE_TEXT_CLOSING;
        msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
        Audio_PlaySoundGeneral(0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void Message_HandleChoiceSelection(PlayState* play, u8 numChoices) {
    static s16 sAnalogStickHeld = false;
    MessageContext* msgCtx = &play->msgCtx;
    Input* input = &play->state.input[0];
    bool dpad = CVar_GetS32("gDpadText", 0);

    if ((input->rel.stick_y >= 30 && !sAnalogStickHeld) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
        sAnalogStickHeld = true;
        msgCtx->choiceIndex--;
        if (msgCtx->choiceIndex > 128) {
            msgCtx->choiceIndex = 0;
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else if ((input->rel.stick_y <= -30 && !sAnalogStickHeld) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DDOWN))) {
        sAnalogStickHeld = true;
        msgCtx->choiceIndex++;
        if (msgCtx->choiceIndex > numChoices) {
            msgCtx->choiceIndex = numChoices;
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else if (ABS(input->rel.stick_y) < 30) {
        sAnalogStickHeld = false;
    }
    msgCtx->textPosX = R_TEXT_CHOICE_XPOS;
    msgCtx->textPosY =
        (numChoices == 1) ? R_TEXT_CHOICE_YPOS(msgCtx->choiceIndex + 1) : R_TEXT_CHOICE_YPOS(msgCtx->choiceIndex);
}

void Message_DrawTextChar(PlayState* play, void* textureImage, Gfx** p) {
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx = *p;
    s16 x = msgCtx->textPosX;
    s16 y = msgCtx->textPosY;

    gDPPipeSync(gfx++);

    sCharTexSize = (R_TEXT_CHAR_SCALE / 100.0f) * 16.0f;
    sCharTexScale = 1024.0f / (R_TEXT_CHAR_SCALE / 100.0f);

    gDPLoadTextureBlock_4b(gfx++, textureImage, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH, FONT_CHAR_TEX_HEIGHT, 0,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    // Draw drop shadow
    if (msgCtx->textBoxType != TEXTBOX_TYPE_NONE_NO_SHADOW) {
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, msgCtx->textColorAlpha);
        gSPTextureRectangle(gfx++, (x + R_TEXT_DROP_SHADOW_OFFSET) << 2, (y + R_TEXT_DROP_SHADOW_OFFSET) << 2,
                            (x + R_TEXT_DROP_SHADOW_OFFSET + sCharTexSize) << 2,
                            (y + R_TEXT_DROP_SHADOW_OFFSET + sCharTexSize) << 2, G_TX_RENDERTILE, 0, 0, sCharTexScale,
                            sCharTexScale);
    }

    gDPPipeSync(gfx++);
    gDPSetPrimColor(gfx++, 0, 0, msgCtx->textColorR, msgCtx->textColorG, msgCtx->textColorB, msgCtx->textColorAlpha);
    gSPTextureRectangle(gfx++, x << 2, y << 2, (x + sCharTexSize) << 2, (y + sCharTexSize) << 2, G_TX_RENDERTILE, 0, 0,
                        sCharTexScale, sCharTexScale);
    *p = gfx;
}

// resizes textboxes when opening them
void Message_GrowTextbox(MessageContext* msgCtx) {
    static f32 sWidthCoefficients[] = {
        1.2f, 1.5f, 1.8f, 2.0f, 2.1f, 2.2f, 2.1f, 2.0f,
    };
    static f32 sHeightCoefficients[] = {
        0.6f, 0.75f, 0.9f, 1.0f, 1.05f, 1.1f, 1.05f, 1.0f,
    };
    f32 width =
        R_TEXTBOX_WIDTH_TARGET * (sWidthCoefficients[msgCtx->stateTimer] + sWidthCoefficients[msgCtx->stateTimer]);
    f32 height = R_TEXTBOX_HEIGHT_TARGET * sHeightCoefficients[msgCtx->stateTimer];
    f32 texWidth =
        R_TEXTBOX_TEXWIDTH_TARGET / (sWidthCoefficients[msgCtx->stateTimer] + sWidthCoefficients[msgCtx->stateTimer]);
    f32 texHeight = R_TEXTBOX_TEXHEIGHT_TARGET / sHeightCoefficients[msgCtx->stateTimer];

    // Adjust y pos
    R_TEXTBOX_Y = R_TEXTBOX_Y_TARGET +
                  (R_TEXTBOX_Y_TARGET - (s16)(R_TEXTBOX_Y_TARGET * sHeightCoefficients[msgCtx->stateTimer] + 0.5f)) / 2;

    msgCtx->textboxColorAlphaCurrent += msgCtx->textboxColorAlphaTarget / 8;
    msgCtx->stateTimer++;
    if (msgCtx->stateTimer == 8) {
        // Reached the end
        R_TEXTBOX_X = R_TEXTBOX_X_TARGET;
        R_TEXTBOX_Y = R_TEXTBOX_Y_TARGET;
        msgCtx->msgMode = MSGMODE_TEXT_STARTING;
        msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;
    }
    // Adjust width and height
    R_TEXTBOX_WIDTH = (s16)(width + 0.5f) / 2;
    R_TEXTBOX_HEIGHT = (s16)(height + 0.5f);
    // Adjust texture coordinates
    R_TEXTBOX_TEXWIDTH = texWidth + 0.5f;
    R_TEXTBOX_TEXHEIGHT = texHeight + 0.5f;
    // Adjust x pos
    R_TEXTBOX_X = (R_TEXTBOX_X_TARGET + R_TEXTBOX_WIDTH_TARGET) - (R_TEXTBOX_WIDTH / 2);
}

void Message_FindMessage(PlayState* play, u16 textId) {
    const char* foundSeg;
    const char* nextSeg;
    MessageTableEntry* messageTableEntry = sNesMessageEntryTablePtr;
    const char** languageSegmentTable;
    Font* font;
    const char* seg;
    u16 bufferId = textId;
    // Use the better owl message if better owl is enabled
    if (CVar_GetS32("gBetterOwl", 0) != 0 && (bufferId == 0x2066 || bufferId == 0x607B ||
        bufferId == 0x10C2 || bufferId == 0x10C6 || bufferId == 0x206A))
    {
        bufferId = 0x71B3;
    }

    if (gSaveContext.language == LANGUAGE_GER)
        messageTableEntry = sGerMessageEntryTablePtr;
    else if (gSaveContext.language == LANGUAGE_FRA)
        messageTableEntry = sFraMessageEntryTablePtr;

    // If PAL languages are not present in the OTR file, default to English
    if (messageTableEntry == NULL)
        messageTableEntry = sNesMessageEntryTablePtr;

    seg = messageTableEntry->segment;

    while (messageTableEntry->textId != 0xFFFF) {
        font = &play->msgCtx.font;

        if (messageTableEntry->textId == bufferId) {
            foundSeg = messageTableEntry->segment;
            font->charTexBuf[0] = messageTableEntry->typePos;

            nextSeg = messageTableEntry->segment;
            font->msgOffset = messageTableEntry->segment;
            font->msgLength = messageTableEntry->msgSize;

            // "Message found!!!"
            osSyncPrintf(" メッセージが,見つかった！！！ = %x  "
                         "(data=%x) (data0=%x) (data1=%x) (data2=%x) (data3=%x)\n",
                         bufferId, font->msgOffset, font->msgLength, foundSeg, seg, nextSeg);
            return;
        }
        messageTableEntry++;
    }

    // "Message not found!!!"
    osSyncPrintf(" メッセージが,見つからなかった！！！ = %x\n", bufferId);
    font = &play->msgCtx.font;
    messageTableEntry = sNesMessageEntryTablePtr;

    foundSeg = messageTableEntry->segment;
    font->charTexBuf[0] = messageTableEntry->typePos;
    messageTableEntry++;
    nextSeg = messageTableEntry->segment;
    font->msgOffset = foundSeg - seg;
    font->msgLength = nextSeg - foundSeg;
}

void Message_FindCreditsMessage(PlayState* play, u16 textId) {
    const char* foundSeg;
    const char* nextSeg;
    const char* seg;
    MessageTableEntry* messageTableEntry = sStaffMessageEntryTablePtr;
    Font* font;

    seg = messageTableEntry->segment;
    while (messageTableEntry->textId != 0xFFFF) {
        font = &play->msgCtx.font;

        if (messageTableEntry->textId == textId) {
            foundSeg = messageTableEntry->segment;
            font->charTexBuf[0] = messageTableEntry->typePos;
            nextSeg = messageTableEntry->segment;
            font->msgOffset = messageTableEntry->segment;
            font->msgLength = messageTableEntry->msgSize;
            // "Message found!!!"
            osSyncPrintf(" メッセージが,見つかった！！！ = %x  (data=%x) (data0=%x) (data1=%x) (data2=%x) (data3=%x)\n",
                         textId, font->msgOffset, font->msgLength, foundSeg, seg, nextSeg);
            return;
        }
        messageTableEntry++;
    }
}

void Message_SetTextColor(MessageContext* msgCtx, u16 colorParameter) {
    switch (colorParameter) {
        case MSGCOL_RED:
            if (msgCtx->textBoxType == TEXTBOX_TYPE_WOODEN) {
                msgCtx->textColorR = 255;
                msgCtx->textColorG = 120;
                msgCtx->textColorB = 0;
            } else {
                msgCtx->textColorR = 255;
                msgCtx->textColorG = 60;
                msgCtx->textColorB = 60;
            }
            break;
        case MSGCOL_ADJUSTABLE:
            if (msgCtx->textBoxType == TEXTBOX_TYPE_WOODEN) {
                msgCtx->textColorR = R_TEXT_ADJUST_COLOR_1_R;
                msgCtx->textColorG = R_TEXT_ADJUST_COLOR_1_G;
                msgCtx->textColorB = R_TEXT_ADJUST_COLOR_1_B;
            } else {
                msgCtx->textColorR = R_TEXT_ADJUST_COLOR_2_R;
                msgCtx->textColorG = R_TEXT_ADJUST_COLOR_2_G;
                msgCtx->textColorB = R_TEXT_ADJUST_COLOR_2_B;
            }
            break;
        case MSGCOL_BLUE:
            if (msgCtx->textBoxType == TEXTBOX_TYPE_WOODEN) {
                msgCtx->textColorR = 80;
                msgCtx->textColorG = 110;
                msgCtx->textColorB = 255;
            } else {
                msgCtx->textColorR = 80;
                msgCtx->textColorG = 90;
                msgCtx->textColorB = 255;
            }
            break;
        case MSGCOL_LIGHTBLUE:
            if (msgCtx->textBoxType == TEXTBOX_TYPE_WOODEN) {
                msgCtx->textColorR = 90;
                msgCtx->textColorG = 180;
                msgCtx->textColorB = 255;
            } else if (msgCtx->textBoxType == TEXTBOX_TYPE_NONE_NO_SHADOW) {
                msgCtx->textColorR = 80;
                msgCtx->textColorG = 150;
                msgCtx->textColorB = 180;
            } else {
                msgCtx->textColorR = 100;
                msgCtx->textColorG = 180;
                msgCtx->textColorB = 255;
            }
            break;
        case MSGCOL_PURPLE:
            if (msgCtx->textBoxType == TEXTBOX_TYPE_WOODEN) {
                msgCtx->textColorR = 210;
                msgCtx->textColorG = 100;
                msgCtx->textColorB = 255;
            } else {
                msgCtx->textColorR = 255;
                msgCtx->textColorG = 150;
                msgCtx->textColorB = 180;
            }
            break;
        case MSGCOL_YELLOW:
            if (msgCtx->textBoxType == TEXTBOX_TYPE_WOODEN) {
                msgCtx->textColorR = 255;
                msgCtx->textColorG = 255;
                msgCtx->textColorB = 30;
            } else {
                msgCtx->textColorR = 225;
                msgCtx->textColorG = 255;
                msgCtx->textColorB = 50;
            }
            break;
        case MSGCOL_BLACK:
            msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 0;
            break;
        case MSGCOL_DEFAULT:
        default:
            if (msgCtx->textBoxType == TEXTBOX_TYPE_NONE_NO_SHADOW) {
                msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 0;
            } else {
                msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 255;
            }
            break;
    }
}

void Message_DrawTextboxIcon(PlayState* play, Gfx** p, s16 x, s16 y) {
    static s16 sIconPrimColors[][3] = {
        { 0, 200, 80 },
        { 50, 255, 130 },
    };
    static s16 sIconEnvColors[][3] = {
        { 0, 0, 0 },
        { 0, 255, 130 },
    };
    static s16 sIconPrimR = 0;
    static s16 sIconPrimG = 200;
    static s16 sIconPrimB = 80;
    static s16 sIconFlashTimer = 12;
    static s16 sIconFlashColorIdx = 0;
    static s16 sIconEnvR = 0;
    static s16 sIconEnvG = 0;
    static s16 sIconEnvB = 0;
    if (CVar_GetS32("gHudColors", 1) == 0) {
        sIconPrimColors[0][0] = 4;
        sIconPrimColors[0][1] = 84;
        sIconPrimColors[0][2] = 204;
        sIconPrimColors[1][0] = 45;
        sIconPrimColors[1][1] = 125;
        sIconPrimColors[1][2] = 255;
        sIconEnvColors[0][0] = 0;
        sIconEnvColors[0][1] = 0;
        sIconEnvColors[0][2] = 0;
        sIconEnvColors[1][0] = 0;
        sIconEnvColors[1][1] = 70;
        sIconEnvColors[1][2] = 255;
    } else if (CVar_GetS32("gHudColors", 1) == 1) {
        sIconPrimColors[0][0] = 4;
        sIconPrimColors[0][1] = 200;
        sIconPrimColors[0][2] = 80;
        sIconPrimColors[1][0] = 50;
        sIconPrimColors[1][1] = 255;
        sIconPrimColors[1][2] = 130;
        sIconEnvColors[0][0] = 0;
        sIconEnvColors[0][1] = 0;
        sIconEnvColors[0][2] = 0;
        sIconEnvColors[1][0] = 0;
        sIconEnvColors[1][1] = 255;
        sIconEnvColors[1][2] = 130;
    } else if (CVar_GetS32("gHudColors", 1) == 2) {
        Color_RGB8 aBtnColor = CVar_GetRGB("gCCABtnPrim", (Color_RGB8){ .r = 0, .g = 200, .b = 80 });
        sIconPrimColors[0][0] = (aBtnColor.r/255)*95;
        sIconPrimColors[0][1] = (aBtnColor.g/255)*95;
        sIconPrimColors[0][2] = (aBtnColor.b/255)*95;
        sIconPrimColors[1][0] = aBtnColor.r;
        sIconPrimColors[1][1] = aBtnColor.g;
        sIconPrimColors[1][2] = aBtnColor.b;
        sIconEnvColors[0][0] = 0;
        sIconEnvColors[0][1] = 0;
        sIconEnvColors[0][2] = 0;
        sIconEnvColors[1][0] = 10;
        sIconEnvColors[1][1] = 10;
        sIconEnvColors[1][2] = 10;
    }
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    Gfx* gfx = *p;
    s16 primR;
    s16 primG;
    s16 primB;
    s16 envR;
    s16 envG;
    s16 envB;
    u8* iconTexture = font->iconBuf;
    gSPInvalidateTexCache(gfx++, iconTexture);

    if (sTextIsCredits) {
        return;
    }

    primR = (ABS(sIconPrimR - sIconPrimColors[sIconFlashColorIdx][0])) / sIconFlashTimer;
    primG = (ABS(sIconPrimG - sIconPrimColors[sIconFlashColorIdx][1])) / sIconFlashTimer;
    primB = (ABS(sIconPrimB - sIconPrimColors[sIconFlashColorIdx][2])) / sIconFlashTimer;

    if (sIconPrimR >= sIconPrimColors[sIconFlashColorIdx][0]) {
        sIconPrimR -= primR;
    } else {
        sIconPrimR += primR;
    }

    if (sIconPrimG >= sIconPrimColors[sIconFlashColorIdx][1]) {
        sIconPrimG -= primG;
    } else {
        sIconPrimG += primG;
    }

    if (sIconPrimB >= sIconPrimColors[sIconFlashColorIdx][2]) {
        sIconPrimB -= primB;
    } else {
        sIconPrimB += primB;
    }

    envR = (ABS(sIconEnvR - sIconEnvColors[sIconFlashColorIdx][0])) / sIconFlashTimer;
    envG = (ABS(sIconEnvG - sIconEnvColors[sIconFlashColorIdx][1])) / sIconFlashTimer;
    envB = (ABS(sIconEnvB - sIconEnvColors[sIconFlashColorIdx][2])) / sIconFlashTimer;

    if (sIconEnvR >= sIconEnvColors[sIconFlashColorIdx][0]) {
        sIconEnvR -= envR;
    } else {
        sIconEnvR += envR;
    }

    if (sIconEnvG >= sIconEnvColors[sIconFlashColorIdx][1]) {
        sIconEnvG -= envG;
    } else {
        sIconEnvG += envG;
    }

    if (sIconEnvB >= sIconEnvColors[sIconFlashColorIdx][2]) {
        sIconEnvB -= envB;
    } else {
        sIconEnvB += envB;
    }

    sIconFlashTimer--;

    if (sIconFlashTimer == 0) {
        sIconPrimR = sIconPrimColors[sIconFlashColorIdx][0];
        sIconPrimG = sIconPrimColors[sIconFlashColorIdx][1];
        sIconPrimB = sIconPrimColors[sIconFlashColorIdx][2];
        sIconEnvR = sIconEnvColors[sIconFlashColorIdx][0];
        sIconEnvG = sIconEnvColors[sIconFlashColorIdx][1];
        sIconEnvB = sIconEnvColors[sIconFlashColorIdx][2];
        sIconFlashTimer = 12;
        sIconFlashColorIdx ^= 1;
    }

    gDPPipeSync(gfx++);

    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPSetPrimColor(gfx++, 0, 0, sIconPrimR, sIconPrimG, sIconPrimB, 255);
    gDPSetEnvColor(gfx++, sIconEnvR, sIconEnvG, sIconEnvB, 255);

    gDPLoadTextureBlock_4b(gfx++, iconTexture, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH, FONT_CHAR_TEX_HEIGHT, 0,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    sCharTexSize = 16.0f * ((f32)R_TEXT_CHAR_SCALE / 100.0f);
    sCharTexScale = 1024.0f / ((f32)R_TEXT_CHAR_SCALE / 100.0f);

    gSPTextureRectangle(gfx++, x << 2, y << 2, (x + sCharTexSize) << 2, (y + sCharTexSize) << 2, G_TX_RENDERTILE, 0, 0,
                        sCharTexScale, sCharTexScale);

    msgCtx->stateTimer++;

    *p = gfx;
}

#define MESSAGE_SPACE_WIDTH 6
f32 sFontWidths[144] = {
    8.0f,  // ' '
    8.0f,  // '!'
    6.0f,  // '"'
    9.0f,  // '#'
    9.0f,  // '$'
    14.0f, // '%'
    12.0f, // '&'
    3.0f,  // '''
    7.0f,  // '('
    7.0f,  // ')'
    7.0f,  // '*'
    9.0f,  // '+'
    4.0f,  // ','
    6.0f,  // '-'
    4.0f,  // '.'
    9.0f,  // '/'
    10.0f, // '0'
    5.0f,  // '1'
    9.0f,  // '2'
    9.0f,  // '3'
    10.0f, // '4'
    9.0f,  // '5'
    9.0f,  // '6'
    9.0f,  // '7'
    9.0f,  // '8'
    9.0f,  // '9'
    6.0f,  // ':'
    6.0f,  // ';'
    9.0f,  // '<'
    11.0f, // '='
    9.0f,  // '>'
    11.0f, // '?'
    13.0f, // '@'
    12.0f, // 'A'
    9.0f,  // 'B'
    11.0f, // 'C'
    11.0f, // 'D'
    8.0f,  // 'E'
    8.0f,  // 'F'
    12.0f, // 'G'
    10.0f, // 'H'
    4.0f,  // 'I'
    8.0f,  // 'J'
    10.0f, // 'K'
    8.0f,  // 'L'
    13.0f, // 'M'
    11.0f, // 'N'
    13.0f, // 'O'
    9.0f,  // 'P'
    13.0f, // 'Q'
    10.0f, // 'R'
    10.0f, // 'S'
    9.0f,  // 'T'
    10.0f, // 'U'
    11.0f, // 'V'
    15.0f, // 'W'
    11.0f, // 'X'
    10.0f, // 'Y'
    10.0f, // 'Z'
    7.0f,  // '['
    10.0f, // '\'
    7.0f,  // ']'
    10.0f, // '^'
    9.0f,  // '_'
    5.0f,  // '`'
    8.0f,  // 'a'
    9.0f,  // 'b'
    8.0f,  // 'c'
    9.0f,  // 'd'
    9.0f,  // 'e'
    6.0f,  // 'f'
    9.0f,  // 'g'
    8.0f,  // 'h'
    4.0f,  // 'i'
    6.0f,  // 'j'
    8.0f,  // 'k'
    4.0f,  // 'l'
    12.0f, // 'm'
    9.0f,  // 'n'
    9.0f,  // 'o'
    9.0f,  // 'p'
    9.0f,  // 'q'
    7.0f,  // 'r'
    8.0f,  // 's'
    7.0f,  // 't'
    8.0f,  // 'u'
    9.0f,  // 'v'
    12.0f, // 'w'
    8.0f,  // 'x'
    9.0f,  // 'y'
    8.0f,  // 'z'
    7.0f,  // '{'
    5.0f,  // '|'
    7.0f,  // '}'
    10.0f, // '~'
    10.0f, // '‾'
    12.0f, // 'À'
    6.0f,  // 'î'
    12.0f, // 'Â'
    12.0f, // 'Ä'
    11.0f, // 'Ç'
    8.0f,  // 'È'
    8.0f,  // 'É'
    8.0f,  // 'Ê'
    6.0f,  // 'Ë'
    6.0f,  // 'Ï'
    13.0f, // 'Ô'
    13.0f, // 'Ö'
    10.0f, // 'Ù'
    10.0f, // 'Û'
    10.0f, // 'Ü'
    9.0f,  // 'ß'
    8.0f,  // 'à'
    8.0f,  // 'á'
    8.0f,  // 'â'
    8.0f,  // 'ä'
    8.0f,  // 'ç'
    9.0f,  // 'è'
    9.0f,  // 'é'
    9.0f,  // 'ê'
    9.0f,  // 'ë'
    6.0f,  // 'ï'
    9.0f,  // 'ô'
    9.0f,  // 'ö'
    9.0f,  // 'ù'
    9.0f,  // 'û'
    9.0f,  // 'ü'
    14.0f, // '[A]'
    14.0f, // '[B]'
    14.0f, // '[C]'
    14.0f, // '[L]'
    14.0f, // '[R]'
    14.0f, // '[Z]'
    14.0f, // '[C-Up]'
    14.0f, // '[C-Down]'
    14.0f, // '[C-Left]'
    14.0f, // '[C-Right]'
    14.0f, // '▼'
    14.0f, // '[Control-Pad]'
    14.0f, // '[D-Pad]'
    14.0f, // ?
    14.0f, // ?
    14.0f, // ?
    14.0f, // ?
};

u16 Message_DrawItemIcon(PlayState* play, u16 itemId, Gfx** p, u16 i) {
    s32 pad;
    Gfx* gfx = *p;
    MessageContext* msgCtx = &play->msgCtx;

    // clang-format off
    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) { Audio_PlaySoundGeneral(0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8); }
    // clang-format on

    gDPPipeSync(gfx++);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, msgCtx->textColorAlpha);

    // Invalidate icon texture as it may have changed from the last time a text box had an icon
    gSPInvalidateTexCache(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE);

    if (itemId >= ITEM_MEDALLION_FOREST) {
        gDPLoadTextureBlock(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_RGBA, G_IM_SIZ_32b,
                            24, 24, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
    } else {
        gDPLoadTextureBlock(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_RGBA, G_IM_SIZ_32b,
                            32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
    }
    gSPTextureRectangle(gfx++, (msgCtx->textPosX + R_TEXTBOX_ICON_XPOS) << 2, R_TEXTBOX_ICON_YPOS << 2,
                        (msgCtx->textPosX + R_TEXTBOX_ICON_XPOS + R_TEXTBOX_ICON_SIZE) << 2,
                        (R_TEXTBOX_ICON_YPOS + R_TEXTBOX_ICON_SIZE) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    gDPPipeSync(gfx++);
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);

    msgCtx->textPosX += 32;

    i++;
    *p = gfx;

    return i;
}

void Message_HandleOcarina(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (play->msgCtx.msgMode == MSGMODE_TEXT_DISPLAYING) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_01;

        if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_LONG_RECORDING) {
            msgCtx->msgMode = MSGMODE_SCARECROW_LONG_RECORDING_START;
            // "Recording Start / Recording Start / Recording Start / Recording Start -> "
            osSyncPrintf("録音開始 録音開始 録音開始 録音開始  -> ");
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_LONG_PLAYBACK) {
            // "Recording Playback / Recording Playback / Recording Playback / Recording Playback -> "
            osSyncPrintf("録音再生 録音再生 録音再生 録音再生  -> ");
            Audio_OcaSetInstrument(1);
            Audio_OcaSetInstrument(1);
            msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
            sOcarinaNoteBufPos = sOcarinaNoteBufLen = 0;
            msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos;
            Message_ResetOcarinaNoteState();
            msgCtx->stateTimer = 3;
            msgCtx->msgMode = MSGMODE_SCARECROW_LONG_PLAYBACK;
            Audio_OcaSetSongPlayback(OCARINA_SONG_SCARECROW_LONG + 1, 1);
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_RECORDING) {
            msgCtx->msgMode = MSGMODE_SCARECROW_RECORDING_START;
            // "8 Note Recording Start / 8 Note Recording Start / 8 Note Recording Start -> "
            osSyncPrintf("８音録音開始 ８音録音開始 ８音録音開始  -> ");
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_PLAYBACK) {
            // "8 Note Playback / 8 Note Playback / 8 Note Playback -> "
            osSyncPrintf("８音再生 ８音再生 ８音再生  -> ");
            Audio_OcaSetInstrument(1);
            Audio_OcaSetInstrument(1);
            msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
            sOcarinaNoteBufPos = sOcarinaNoteBufLen = 0;
            msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos;
            Message_ResetOcarinaNoteState();
            msgCtx->stateTimer = 3;
            msgCtx->msgMode = MSGMODE_SCARECROW_PLAYBACK;
            Audio_OcaSetSongPlayback(OCARINA_SONG_SCARECROW + 1, 1);
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_MEMORY_GAME) {
            msgCtx->msgMode = MSGMODE_MEMORY_GAME_START;
            // "Musical Round Start / Musical Round Start / Musical Round Start / Musical Round Start -> "
            osSyncPrintf("輪唱開始 輪唱開始 輪唱開始 輪唱開始  -> ");
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_FROGS) {
            msgCtx->msgMode = MSGMODE_FROGS_START;
            // "Frog Chorus / Frog Chorus -> "
            osSyncPrintf("カエルの合唱 カエルの合唱  -> ");
        } else {
            // "Ocarina（%d）"
            osSyncPrintf("オカリナ（%d） ", msgCtx->ocarinaAction);
            if (msgCtx->ocarinaAction == OCARINA_ACTION_UNK_0 || msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY ||
                msgCtx->ocarinaAction >= OCARINA_ACTION_CHECK_SARIA) {
                msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
                osSyncPrintf("000000000000  -> ");
            } else if (msgCtx->ocarinaAction >= OCARINA_ACTION_TEACH_MINUET &&
                       msgCtx->ocarinaAction <= OCARINA_ACTION_TEACH_STORMS) {
                msgCtx->msgMode = MSGMODE_SONG_DEMONSTRATION_STARTING;
                osSyncPrintf("111111111111  -> ");
            } else {
                msgCtx->msgMode = MSGMODE_SONG_PLAYBACK_STARTING;
                osSyncPrintf("222222222222  -> ");
            }
        }
        osSyncPrintf("msg_mode=%d\n", msgCtx->msgMode);
    }
}

/**
 * Draws the text contents of a textbox, up to the current point that has
 * been scrolled to so far.
 */
void Message_DrawText(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    u16 lookAheadCharacter;
    u8 character;
    u16 j;
    u16 i;
    u16 sfxHi;
    u16 charTexIdx;
    Font* font = &play->msgCtx.font;
    Gfx* gfx = *gfxP;

    play->msgCtx.textPosX = R_TEXT_INIT_XPOS;

    if (sTextIsCredits == false) {
        msgCtx->textPosY = R_TEXT_INIT_YPOS;
    } else {
        msgCtx->textPosY = YREG(1);
    }

    if (msgCtx->textBoxType == TEXTBOX_TYPE_NONE_NO_SHADOW) {
        msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 0;
    } else {
        msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 255;
    }

    msgCtx->unk_E3D0 = 0;
    charTexIdx = 0;

    for (i = 0; i < msgCtx->textDrawPos; i++) {
        character = msgCtx->msgBufDecoded[i];

        switch (character) {
            case MESSAGE_NEWLINE:
                msgCtx->textPosX = R_TEXT_INIT_XPOS;
                if (msgCtx->choiceNum == 1 || msgCtx->choiceNum == 3) {
                    msgCtx->textPosX += 32;
                }
                if (msgCtx->choiceNum == 2 && msgCtx->textPosY != R_TEXT_INIT_YPOS) {
                    msgCtx->textPosX += 32;
                }
                msgCtx->textPosY += R_TEXT_LINE_SPACING;
                break;
            case MESSAGE_COLOR:
                Message_SetTextColor(msgCtx, msgCtx->msgBufDecoded[++i] & 0xF);
                break;
            case ' ':
                msgCtx->textPosX += MESSAGE_SPACE_WIDTH;
                break;
            case MESSAGE_BOX_BREAK:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    if (!sTextboxSkipped) {
                        Audio_PlaySoundGeneral(0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        msgCtx->msgMode = MSGMODE_TEXT_AWAIT_NEXT;
                        Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_TRIANGLE);
                    } else {
                        msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
                        msgCtx->textUnskippable = false;
                        msgCtx->msgBufPos++;
                    }
                }
                *gfxP = gfx;
                return;
            case MESSAGE_SHIFT:
                msgCtx->textPosX += msgCtx->msgBufDecoded[++i];
                break;
            case MESSAGE_TEXTID:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_HAS_NEXT;
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Audio_PlaySoundGeneral(0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_TRIANGLE);
                }
                *gfxP = gfx;
                return;
            case MESSAGE_QUICKTEXT_ENABLE:
                if (i + 1 == msgCtx->textDrawPos && (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING ||
                                                     (msgCtx->msgMode >= MSGMODE_OCARINA_STARTING &&
                                                      msgCtx->msgMode < MSGMODE_SCARECROW_LONG_RECORDING_START))) {
                    j = i;
                    while (true) {
                        lookAheadCharacter = msgCtx->msgBufDecoded[j];
                        if (lookAheadCharacter == MESSAGE_SHIFT) {
                            j += 2;
                        } else if ((lookAheadCharacter != MESSAGE_QUICKTEXT_DISABLE) &&
                                   (lookAheadCharacter != MESSAGE_PERSISTENT) &&
                                   (lookAheadCharacter != MESSAGE_EVENT) &&
                                   (lookAheadCharacter != MESSAGE_BOX_BREAK_DELAYED) &&
                                   (lookAheadCharacter != MESSAGE_AWAIT_BUTTON_PRESS) &&
                                   (lookAheadCharacter != MESSAGE_BOX_BREAK) && (lookAheadCharacter != MESSAGE_END)) {
                            j++;
                        } else {
                            break;
                        }
                    }
                    i = j - 1;
                    msgCtx->textDrawPos = i + 1;

                    if (character) {}
                }
            case MESSAGE_QUICKTEXT_DISABLE:
                break;
            case MESSAGE_AWAIT_BUTTON_PRESS:
                if (i + 1 == msgCtx->textDrawPos) {
                    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                        msgCtx->msgMode = MSGMODE_TEXT_AWAIT_INPUT;
                        Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_TRIANGLE);
                    }
                    *gfxP = gfx;
                    return;
                }
                break;
            case MESSAGE_BOX_BREAK_DELAYED:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->stateTimer = msgCtx->msgBufDecoded[++i];
                    msgCtx->msgMode = MSGMODE_TEXT_DELAYED_BREAK;
                }
                *gfxP = gfx;
                return;
            case MESSAGE_FADE2:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADING;
                    // "Timer"
                    osSyncPrintf("タイマー (%x) (%x)", msgCtx->msgBufDecoded[i + 1], msgCtx->msgBufDecoded[i + 2]);
                    msgCtx->stateTimer = msgCtx->msgBufDecoded[++i] << 8;
                    msgCtx->stateTimer |= msgCtx->msgBufDecoded[++i];
                    // "Total wct"
                    osSyncPrintf("合計wct=%x(%d)\n", msgCtx->stateTimer, msgCtx->stateTimer);
                }
                *gfxP = gfx;
                return;
            case MESSAGE_SFX:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING && !sMessageHasSetSfx) {
                    sMessageHasSetSfx = true;
                    // "Sound (SE)"
                    osSyncPrintf("サウンド（ＳＥ）\n");
                    sfxHi = msgCtx->msgBufDecoded[i + 1] << 8;
                    Audio_PlaySoundGeneral(sfxHi | msgCtx->msgBufDecoded[i + 2], &D_801333D4, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                }
                i += 2;
                break;
            case MESSAGE_ITEM_ICON:
                i = Message_DrawItemIcon(play, msgCtx->msgBufDecoded[i + 1], &gfx, i);
                break;
            case MESSAGE_BACKGROUND:
                // clang-format off
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) { Audio_PlaySoundGeneral(0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8); }
                // clang-format on
                gDPPipeSync(gfx++);
                gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                gDPSetPrimColor(gfx++, 0, 0, sTextboxBackgroundBackPrimColors[msgCtx->textboxBackgroundBackColorIdx][0],
                                sTextboxBackgroundBackPrimColors[msgCtx->textboxBackgroundBackColorIdx][1],
                                sTextboxBackgroundBackPrimColors[msgCtx->textboxBackgroundBackColorIdx][2],
                                msgCtx->textColorAlpha);

                gDPLoadTextureBlock_4b(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_I, 96, 48,
                                       0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                       G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(
                    gfx++, (msgCtx->textPosX + 1) << 2,
                    (R_TEXTBOX_BG_YPOS + sTextboxBackgroundYOffsets[msgCtx->textboxBackgroundYOffsetIdx]) << 2,
                    (msgCtx->textPosX + 96 + 1) << 2,
                    (R_TEXTBOX_BG_YPOS + sTextboxBackgroundYOffsets[msgCtx->textboxBackgroundYOffsetIdx] + 48) << 2,
                    G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                gDPLoadTextureBlock_4b(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE + 0x900, G_IM_FMT_I,
                                       96, 48, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                       G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(
                    gfx++, (msgCtx->textPosX + 96 + 1) << 2,
                    (R_TEXTBOX_BG_YPOS + sTextboxBackgroundYOffsets[msgCtx->textboxBackgroundYOffsetIdx]) << 2,
                    (msgCtx->textPosX + 96 + 1 + 96 + 1) << 2,
                    (R_TEXTBOX_BG_YPOS + sTextboxBackgroundYOffsets[msgCtx->textboxBackgroundYOffsetIdx] + 48) << 2,
                    G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                gDPPipeSync(gfx++);
                gDPSetPrimColor(gfx++, 0, 0, sTextboxBackgroundForePrimColors[msgCtx->textboxBackgroundForeColorIdx][0],
                                sTextboxBackgroundForePrimColors[msgCtx->textboxBackgroundForeColorIdx][1],
                                sTextboxBackgroundForePrimColors[msgCtx->textboxBackgroundForeColorIdx][2],
                                msgCtx->textColorAlpha);

                gDPLoadTextureBlock_4b(gfx++, ((uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE), G_IM_FMT_I, 96,
                                       48, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                       G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, msgCtx->textPosX << 2, R_TEXTBOX_BG_YPOS << 2, (msgCtx->textPosX + 96) << 2,
                                    (R_TEXTBOX_BG_YPOS + 48) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                gDPLoadTextureBlock_4b(gfx++, ((uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE + 0x900),
                                       G_IM_FMT_I, 96, 48, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                       G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, (msgCtx->textPosX + 96) << 2, R_TEXTBOX_BG_YPOS << 2,
                                    (msgCtx->textPosX + 192) << 2, (R_TEXTBOX_BG_YPOS + 48) << 2, G_TX_RENDERTILE, 0, 0,
                                    1 << 10, 1 << 10);

                gDPPipeSync(gfx++);
                gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                                  PRIMITIVE, 0);

                msgCtx->textPosX += 32;
                break;
            case MESSAGE_TEXT_SPEED:
                msgCtx->textDelay = msgCtx->msgBufDecoded[++i];
                break;
            case MESSAGE_UNSKIPPABLE:
                msgCtx->textUnskippable = CVar_GetS32("gSkipText", 0) != 1;
                break;
            case MESSAGE_TWO_CHOICE:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_2_CHOICE;
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->choiceTextId = msgCtx->textId;
                    msgCtx->stateTimer = 4;
                    msgCtx->choiceIndex = 0;
                    Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_ARROW);
                }
                break;
            case MESSAGE_THREE_CHOICE:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_3_CHOICE;
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->choiceTextId = msgCtx->textId;
                    msgCtx->stateTimer = 4;
                    msgCtx->choiceIndex = 0;
                    Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_ARROW);
                }
                break;
            case MESSAGE_END:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_DEFAULT) {
                        Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_END, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_SQUARE);
                        if (play->csCtx.state == 0) {
                            Interface_SetDoAction(play, DO_ACTION_RETURN);
                        }
                    }
                }
                *gfxP = gfx;
                return;
            case MESSAGE_OCARINA:
                msgCtx->textDrawPos = i + 1;
                if (i + 1 == msgCtx->textDrawPos) {
                    Message_HandleOcarina(play);
                    *gfxP = gfx;
                    return;
                }
                break;
            case MESSAGE_FADE:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADING;
                    msgCtx->stateTimer = msgCtx->msgBufDecoded[++i];
                    Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_SQUARE);
                    if (play->csCtx.state == 0) {
                        Interface_SetDoAction(play, DO_ACTION_RETURN);
                    }
                }
                *gfxP = gfx;
                return;
            case MESSAGE_PERSISTENT:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Audio_PlaySoundGeneral(0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_PERSISTENT;
                }
                *gfxP = gfx;
                return;
            case MESSAGE_EVENT:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_EVENT;
                    Font_LoadMessageBoxIcon(font, TEXTBOX_ICON_TRIANGLE);
                    Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_END, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                }
                *gfxP = gfx;
                return;
            default:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING && i + 1 == msgCtx->textDrawPos &&
                    msgCtx->textDelayTimer == msgCtx->textDelay) {
                    Audio_PlaySoundGeneral(0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                }
                Message_DrawTextChar(play, &font->charTexBuf[charTexIdx], &gfx);
                charTexIdx += FONT_CHAR_TEX_SIZE;

                msgCtx->textPosX += (s32)(sFontWidths[character - ' '] * (R_TEXT_CHAR_SCALE / 100.0f));
                break;
        }
    }
    if (msgCtx->textDelayTimer == 0) {
        msgCtx->textDrawPos = i + CVar_GetS32("gTextSpeed", 2);
        msgCtx->textDelayTimer = msgCtx->textDelay;
    } else {
        msgCtx->textDelayTimer--;
    }
    *gfxP = gfx;
}

void Message_LoadItemIcon(PlayState* play, u16 itemId, s16 y) {
    static s16 sIconItem32XOffsets[] = { 74, 74, 74 };
    static s16 sIconItem24XOffsets[] = { 72, 72, 72 };
    MessageContext* msgCtx = &play->msgCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (itemId == ITEM_DUNGEON_MAP) {
        interfaceCtx->mapPalette[30] = 0xFF;
        interfaceCtx->mapPalette[31] = 0xFF;
    }
    if (itemId < ITEM_MEDALLION_FOREST) {
        R_TEXTBOX_ICON_XPOS = R_TEXT_INIT_XPOS - sIconItem32XOffsets[gSaveContext.language];
        R_TEXTBOX_ICON_YPOS = y + 6;
        R_TEXTBOX_ICON_SIZE = 32;
        memcpy((uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE,
               ResourceMgr_LoadTexByName(gItemIcons[itemId]), ResourceMgr_LoadTexSizeByName(gItemIcons[itemId]));
        // "Item 32-0"
        osSyncPrintf("アイテム32-0\n");
    } else {
        R_TEXTBOX_ICON_XPOS = R_TEXT_INIT_XPOS - sIconItem24XOffsets[gSaveContext.language];
        R_TEXTBOX_ICON_YPOS = y + 10;
        R_TEXTBOX_ICON_SIZE = 24;
        memcpy((uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE,
               ResourceMgr_LoadTexByName(gItemIcons[itemId]), ResourceMgr_LoadTexSizeByName(gItemIcons[itemId]));
        // "Item 24"
        osSyncPrintf("アイテム24＝%d (%d) {%d}\n", itemId, itemId - ITEM_KOKIRI_EMERALD, 84);
    }
    msgCtx->msgBufPos++;
    msgCtx->choiceNum = 1;
}

void Message_Decode(PlayState* play) {
    u8 temp_s2;
    u8 phi_s1;
    u16 phi_s0_3;
    s32 loadChar;
    s32 charTexIdx = 0;
    s16 playerNameLen;
    s16 decodedBufPos = 0;
    s16 numLines = 0;
    s16 i;
    s16 digits[4];
    f32 timeInSeconds;
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &play->msgCtx.font;

    if ((msgCtx->msgMode >= MSGMODE_OCARINA_STARTING && msgCtx->msgMode <= MSGMODE_OCARINA_AWAIT_INPUT) || msgCtx->textBoxType == TEXTBOX_TYPE_OCARINA) {
        // TODO: Figure out what specific textures to invalidate to prevent the ocarina textboxes from flashing
        gSPInvalidateTexCache(play->state.gfxCtx->polyOpa.p++, NULL);
    } else {
        for (u32 i = 0; i < FONT_CHAR_TEX_SIZE * 120; i += FONT_CHAR_TEX_SIZE) {
            if (&font->charTexBuf[i] != NULL) {
                gSPInvalidateTexCache(play->state.gfxCtx->polyOpa.p++, &font->charTexBuf[i]);
            }
        }
    }

    play->msgCtx.textDelayTimer = 0;
    play->msgCtx.textUnskippable = play->msgCtx.textDelay = play->msgCtx.textDelayTimer = 0;
    sTextFade = false;

    while (true) {
        phi_s1 = temp_s2 = msgCtx->msgBufDecoded[decodedBufPos] = font->msgBuf[msgCtx->msgBufPos];

        if (temp_s2 == MESSAGE_BOX_BREAK || temp_s2 == MESSAGE_TEXTID || temp_s2 == MESSAGE_BOX_BREAK_DELAYED ||
            temp_s2 == MESSAGE_EVENT || temp_s2 == MESSAGE_END) {
            // Textbox decoding ends with any of the above text control characters
            msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
            msgCtx->textDrawPos = 1;
            R_TEXT_INIT_YPOS = R_TEXTBOX_Y + 8;
            osSyncPrintf("ＪＪ＝%d\n", numLines);
            if (msgCtx->textBoxType != TEXTBOX_TYPE_NONE_BOTTOM) {
                if (numLines == 0) {
                    R_TEXT_INIT_YPOS = (u16)(R_TEXTBOX_Y + 26);
                } else if (numLines == 1) {
                    R_TEXT_INIT_YPOS = (u16)(R_TEXTBOX_Y + 20);
                } else if (numLines == 2) {
                    R_TEXT_INIT_YPOS = (u16)(R_TEXTBOX_Y + 16);
                }
            }
            if (phi_s1 == MESSAGE_TEXTID) {
                osSyncPrintf("NZ_NEXTMSG=%x, %x, %x\n", font->msgBuf[msgCtx->msgBufPos],
                             font->msgBuf[msgCtx->msgBufPos + 1], font->msgBuf[msgCtx->msgBufPos + 2]);
                temp_s2 = msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[msgCtx->msgBufPos + 1];
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[msgCtx->msgBufPos + 2];
                phi_s0_3 = temp_s2 << 8;
                sNextTextId = msgCtx->msgBufDecoded[decodedBufPos] | phi_s0_3;
            }
            if (phi_s1 == MESSAGE_BOX_BREAK_DELAYED) {
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[msgCtx->msgBufPos + 1];
                msgCtx->msgBufPos += 2;
            }
            msgCtx->decodedTextLen = decodedBufPos;
            if (sTextboxSkipped) {
                msgCtx->textDrawPos = msgCtx->decodedTextLen;
            }
            break;
        } else if (temp_s2 == MESSAGE_NAME) {
            // Substitute the player name control character for the file's player name.
            for (playerNameLen = ARRAY_COUNT(gSaveContext.playerName); playerNameLen > 0; playerNameLen--) {
                if (gSaveContext.playerName[playerNameLen - 1] != 0x3E) {
                    break;
                }
            }
            // "Name"
            osSyncPrintf("\n名前 ＝ ");
            for (i = 0; i < playerNameLen; i++) {
                phi_s1 = gSaveContext.playerName[i];
                if (phi_s1 == 0x3E) {
                    phi_s1 = ' ';
                } else if (phi_s1 == 0x40) {
                    phi_s1 = '.';
                } else if (phi_s1 == 0x3F) {
                    phi_s1 = '-';
                } else if (phi_s1 < 0xA) {
                    phi_s1 += 0;
                    phi_s1 += '0';
                } else if (phi_s1 < 0x24) {
                    phi_s1 += 0;
                    phi_s1 += '7';
                } else if (phi_s1 < 0x3E) {
                    phi_s1 += 0;
                    phi_s1 += '=';
                }
                if (phi_s1 != ' ') {
                    Font_LoadChar(font, phi_s1 - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                }
                osSyncPrintf("%x ", phi_s1);
                msgCtx->msgBufDecoded[decodedBufPos] = phi_s1;
                decodedBufPos++;
            }
            decodedBufPos--;
        } else if (temp_s2 == MESSAGE_MARATHON_TIME || temp_s2 == MESSAGE_RACE_TIME) {
            // Convert the values of the appropriate timer to digits and add the
            //  digits to the decoded buffer in place of the control character.
            // "EVENT timer"
            osSyncPrintf("\nＥＶＥＮＴタイマー ＝ ");
            digits[0] = digits[1] = digits[2] = 0;
            if (temp_s2 == MESSAGE_RACE_TIME) {
                digits[3] = gSaveContext.timer1Value;
            } else {
                digits[3] = gSaveContext.timer2Value;
            }

            while (digits[3] >= 60) {
                digits[1]++;
                if (digits[1] >= 10) {
                    digits[0]++;
                    digits[1] -= 10;
                }
                digits[3] -= 60;
            }
            while (digits[3] >= 10) {
                digits[2]++;
                digits[3] -= 10;
            }

            for (i = 0; i < 4; i++) {
                Font_LoadChar(font, digits[i] + '0' - ' ', charTexIdx);
                charTexIdx += FONT_CHAR_TEX_SIZE;
                msgCtx->msgBufDecoded[decodedBufPos] = digits[i] + '0';
                decodedBufPos++;
                if (i == 1) {
                    Font_LoadChar(font, '"' - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    msgCtx->msgBufDecoded[decodedBufPos] = '"';
                    decodedBufPos++;
                } else if (i == 3) {
                    Font_LoadChar(font, '"' - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    msgCtx->msgBufDecoded[decodedBufPos] = '"';
                }
            }
        } else if (temp_s2 == MESSAGE_POINTS) {
            // Convert the values of the current minigame score to digits and
            //  add the digits to the decoded buffer in place of the control character.
            // "Yabusame score"
            osSyncPrintf("\n流鏑馬スコア ＝ %d\n", gSaveContext.minigameScore);
            digits[0] = digits[1] = digits[2] = 0;
            digits[3] = gSaveContext.minigameScore;

            while (digits[3] >= 1000) {
                digits[0]++;
                digits[3] -= 1000;
            }
            while (digits[3] >= 100) {
                digits[1]++;
                digits[3] -= 100;
            }
            while (digits[3] >= 10) {
                digits[2]++;
                digits[3] -= 10;
            }

            loadChar = false;
            for (i = 0; i < 4; i++) {
                if (i == 3 || digits[i] != 0) {
                    loadChar = true;
                }
                if (loadChar) {
                    Font_LoadChar(font, digits[i] + '0' - ' ', charTexIdx);
                    msgCtx->msgBufDecoded[decodedBufPos] = digits[i] + '0';
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    decodedBufPos++;
                }
            }
            decodedBufPos--;
        } else if (temp_s2 == MESSAGE_TOKENS) {
            // Convert the current number of collected gold skulltula tokens to digits and
            //  add the digits to the decoded buffer in place of the control character.
            // "Total number of gold stars"
            osSyncPrintf("\n金スタ合計数 ＝ %d", gSaveContext.inventory.gsTokens);
            digits[0] = digits[1] = 0;
            digits[2] = gSaveContext.inventory.gsTokens;

            while (digits[2] >= 100) {
                digits[0]++;
                digits[2] -= 100;
            }
            while (digits[2] >= 10) {
                digits[1]++;
                digits[2] -= 10;
            }

            loadChar = false;
            for (i = 0; i < 3; i++) {
                if (i == 2 || digits[i] != 0) {
                    loadChar = true;
                }
                if (loadChar) {
                    Font_LoadChar(font, digits[i] + '0' - ' ', charTexIdx);
                    msgCtx->msgBufDecoded[decodedBufPos] = digits[i] + '0';
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    osSyncPrintf("%x(%x) ", digits[i] + '0' - ' ', digits[i]);
                    decodedBufPos++;
                }
            }
            decodedBufPos--;
        } else if (temp_s2 == MESSAGE_FISH_INFO) {
            // "Fishing hole fish size"
            osSyncPrintf("\n釣り堀魚サイズ ＝ ");
            digits[0] = 0;
            digits[1] = gSaveContext.minigameScore;

            while (digits[1] >= 10) {
                digits[0]++;
                digits[1] -= 10;
            }

            for (i = 0; i < 2; i++) {
                if (i == 1 || digits[i] != 0) {
                    Font_LoadChar(font, digits[i] + '0' - ' ', charTexIdx);
                    msgCtx->msgBufDecoded[decodedBufPos] = digits[i] + '0';
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    osSyncPrintf("%x(%x) ", digits[i] + '0' - ' ', digits[i]);
                    decodedBufPos++;
                }
            }
            decodedBufPos--;
        } else if (temp_s2 == MESSAGE_HIGHSCORE) {
            phi_s0_3 = HIGH_SCORE((u8)font->msgBuf[++msgCtx->msgBufPos]);
            // "Highscore"
            osSyncPrintf("ランキング＝%d\n", font->msgBuf[msgCtx->msgBufPos]);
            if ((font->msgBuf[msgCtx->msgBufPos] & 0xFF) == 2) {
                if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
                    phi_s0_3 &= 0x7F;
                } else {
                    osSyncPrintf("HI_SCORE( kanfont->mbuff.nes_mes_buf[message->rdp] & 0xff000000 ) = %x\n",
                                 HIGH_SCORE(font->msgBufWide[msgCtx->msgBufPos] & 0xFF000000));
                    phi_s0_3 = ((HIGH_SCORE((u8)font->msgBuf[msgCtx->msgBufPos]) & 0xFF000000) >> 0x18) & 0x7F;
                }
                phi_s0_3 = SQ((f32)phi_s0_3) * 0.0036f + 0.5f;
                osSyncPrintf("score=%d\n", phi_s0_3);
            }
            switch (font->msgBuf[msgCtx->msgBufPos] & 0xFF) {
                case HS_HBA:
                case HS_POE_POINTS:
                case HS_FISHING:
                    digits[0] = digits[1] = digits[2] = 0;
                    digits[3] = phi_s0_3;

                    while (digits[3] >= 1000) {
                        digits[0]++;
                        digits[3] -= 1000;
                    }
                    while (digits[3] >= 100) {
                        digits[1]++;
                        digits[3] -= 100;
                    }
                    while (digits[3] >= 10) {
                        digits[2]++;
                        digits[3] -= 10;
                    }
                    if (temp_s2) {}

                    loadChar = false;
                    for (i = 0; i < 4; i++) {
                        if (i == 3 || digits[i] != 0) {
                            loadChar = true;
                        }
                        if (loadChar) {
                            Font_LoadChar(font, digits[i] + '0' - ' ', charTexIdx);
                            msgCtx->msgBufDecoded[decodedBufPos] = digits[i] + '0';
                            charTexIdx += FONT_CHAR_TEX_SIZE;
                            decodedBufPos++;
                        }
                    }
                    decodedBufPos--;
                    break;
                case HS_UNK_05:
                    break;
                case HS_HORSE_RACE:
                case HS_MARATHON:
                case HS_DAMPE_RACE:
                    digits[0] = digits[1] = digits[2] = 0;
                    digits[3] = phi_s0_3;

                    while (digits[3] >= 60) {
                        digits[1]++;
                        if (digits[1] >= 10) {
                            digits[0]++;
                            digits[1] -= 10;
                        }
                        digits[3] -= 60;
                    }
                    while (digits[3] >= 10) {
                        digits[2]++;
                        digits[3] -= 10;
                    }

                    for (i = 0; i < 4; i++) {
                        Font_LoadChar(font, digits[i] + '0' - ' ', charTexIdx);
                        charTexIdx += FONT_CHAR_TEX_SIZE;
                        msgCtx->msgBufDecoded[decodedBufPos] = digits[i] + '0';
                        decodedBufPos++;
                        if (i == 1) {
                            Font_LoadChar(font, '"' - ' ', charTexIdx);
                            charTexIdx += FONT_CHAR_TEX_SIZE;
                            msgCtx->msgBufDecoded[decodedBufPos] = '"';
                            decodedBufPos++;
                        } else if (i == 3) {
                            Font_LoadChar(font, '"' - ' ', charTexIdx);
                            charTexIdx += FONT_CHAR_TEX_SIZE;
                            msgCtx->msgBufDecoded[decodedBufPos] = '"';
                        }
                    }
                    break;
            }
        } else if (temp_s2 == MESSAGE_TIME) {
            // "Zelda time"
            osSyncPrintf("\nゼルダ時間 ＝ ");
            digits[0] = 0;
            timeInSeconds = gSaveContext.dayTime * (24.0f * 60.0f / 0x10000);

            digits[1] = timeInSeconds / 60.0f;
            while (digits[1] >= 10) {
                digits[0]++;
                digits[1] -= 10;
            }
            digits[2] = 0;
            digits[3] = (s16)timeInSeconds % 60;
            while (digits[3] >= 10) {
                digits[2]++;
                digits[3] -= 10;
            }

            for (i = 0; i < 4; i++) {
                Font_LoadChar(font, digits[i] + '0' - ' ', charTexIdx);
                charTexIdx += FONT_CHAR_TEX_SIZE;
                msgCtx->msgBufDecoded[decodedBufPos] = digits[i] + '0';
                decodedBufPos++;
                if (i == 1) {
                    Font_LoadChar(font, ':' - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    msgCtx->msgBufDecoded[decodedBufPos] = ':';
                    decodedBufPos++;
                }
            }
            decodedBufPos--;
        } else if (temp_s2 == MESSAGE_ITEM_ICON) {
            msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[msgCtx->msgBufPos + 1];
            osSyncPrintf("ITEM_NO=(%d) (%d)\n", msgCtx->msgBufDecoded[decodedBufPos],
                         font->msgBuf[msgCtx->msgBufPos + 1]);
            Message_LoadItemIcon(play, font->msgBuf[msgCtx->msgBufPos + 1], R_TEXTBOX_Y + 10);
        } else if (temp_s2 == MESSAGE_BACKGROUND) {
            msgCtx->textboxBackgroundIdx = font->msgBuf[msgCtx->msgBufPos + 1] * 2;
            msgCtx->textboxBackgroundForeColorIdx = (font->msgBuf[msgCtx->msgBufPos + 2] & 0xF0) >> 4;
            msgCtx->textboxBackgroundBackColorIdx = font->msgBuf[msgCtx->msgBufPos + 2] & 0xF;
            msgCtx->textboxBackgroundYOffsetIdx = (font->msgBuf[msgCtx->msgBufPos + 3] & 0xF0) >> 4;
            msgCtx->textboxBackgroundUnkArg = font->msgBuf[msgCtx->msgBufPos + 3] & 0xF;

            memcpy((uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE,
                   ResourceMgr_LoadTexByName(gRedMessageXLeftTex), ResourceMgr_LoadTexSizeByName(gRedMessageXLeftTex));
            memcpy((uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE + 0x900,
                   ResourceMgr_LoadTexByName(gRedMessageXRightTex), ResourceMgr_LoadTexSizeByName(gRedMessageXRightTex));

            msgCtx->msgBufPos += 3;
            R_TEXTBOX_BG_YPOS = R_TEXTBOX_Y + 8;
            numLines = 2;
            R_TEXT_INIT_XPOS = 50;
        } else if (temp_s2 == MESSAGE_COLOR) {
            msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[++msgCtx->msgBufPos];
        } else if (temp_s2 == MESSAGE_NEWLINE) {
            numLines++;
        } else if (temp_s2 != MESSAGE_QUICKTEXT_ENABLE && temp_s2 != MESSAGE_QUICKTEXT_DISABLE &&
                   temp_s2 != MESSAGE_AWAIT_BUTTON_PRESS && temp_s2 != MESSAGE_OCARINA &&
                   temp_s2 != MESSAGE_PERSISTENT && temp_s2 != MESSAGE_UNSKIPPABLE) {
            if (temp_s2 == MESSAGE_FADE) {
                sTextFade = true;
                osSyncPrintf("NZ_TIMER_END (key_off_flag=%d)\n", sTextFade);
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[++msgCtx->msgBufPos];
            } else if (temp_s2 == MESSAGE_FADE2) {
                sTextFade = true;
                osSyncPrintf("NZ_BGM (key_off_flag=%d)\n", sTextFade);
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[++msgCtx->msgBufPos];
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[++msgCtx->msgBufPos];
            } else if (temp_s2 == MESSAGE_SHIFT || temp_s2 == MESSAGE_TEXT_SPEED) {
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[++msgCtx->msgBufPos] & 0xFF;
            } else if (temp_s2 == MESSAGE_SFX) {
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[++msgCtx->msgBufPos];
                msgCtx->msgBufDecoded[++decodedBufPos] = font->msgBuf[++msgCtx->msgBufPos];
            } else if (temp_s2 == MESSAGE_TWO_CHOICE) {
                msgCtx->choiceNum = 2;
            } else if (temp_s2 == MESSAGE_THREE_CHOICE) {
                msgCtx->choiceNum = 3;
            } else if (temp_s2 != ' ') {
                Font_LoadChar(font, temp_s2 - ' ', charTexIdx);
                charTexIdx += FONT_CHAR_TEX_SIZE;
            }
        }
        decodedBufPos++;
        msgCtx->msgBufPos++;
    }
}

extern const char* msgStaticTbl[];

void Message_OpenText(PlayState* play, u16 textId) {
    static s16 messageStaticIndices[] = { 0, 1, 3, 2 };
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    s16 textBoxType;

    if (msgCtx->msgMode == MSGMODE_NONE) {
        gSaveContext.unk_13EE = gSaveContext.unk_13EA;
    }
    if (YREG(15) == 0x10) {
        Interface_ChangeAlpha(5);
    }

    sMessageHasSetSfx = D_8014B2F4 = sTextboxSkipped = sTextIsCredits = 0;
    if ((msgCtx->msgMode >= MSGMODE_OCARINA_STARTING && msgCtx->msgMode <= MSGMODE_OCARINA_AWAIT_INPUT) ||
        msgCtx->textBoxType == TEXTBOX_TYPE_OCARINA) {
        // TODO: Figure out what specific textures to invalidate to prevent the ocarina textboxes from flashing
        gSPInvalidateTexCache(play->state.gfxCtx->polyOpa.p++, NULL);
    } else {
        for (u32 i = 0; i < FONT_CHAR_TEX_SIZE * 120; i += FONT_CHAR_TEX_SIZE) {
            if (&font->charTexBuf[i] != NULL) {
                gSPInvalidateTexCache(play->state.gfxCtx->polyOpa.p++, &font->charTexBuf[i]);
            }
        }
    }

    if (textId >= 0x0500 && textId < 0x0600) { // text ids 0500 to 0600 are reserved for credits
        sTextIsCredits = true;
        R_TEXT_CHAR_SCALE = 85;
        R_TEXT_LINE_SPACING = 6;
        R_TEXT_INIT_XPOS = 20;
        YREG(1) = 48;
    } else {
        R_TEXT_CHAR_SCALE = 75;
        R_TEXT_LINE_SPACING = 12;
        R_TEXT_INIT_XPOS = 65;
    }
    if (textId == 0xC2 || textId == 0xFA) {
        // Increments text id based on piece of heart count, assumes the piece of heart text is all
        // in order and that you don't have more than the intended amount of heart pieces.
        textId += (gSaveContext.inventory.questItems & 0xF0000000 & 0xF0000000) >> 0x1C;
    } else if (!gSaveContext.n64ddFlag && (msgCtx->textId == 0xC && CHECK_OWNED_EQUIP(EQUIP_SWORD, 2))) {
        textId = 0xB; // Traded Giant's Knife for Biggoron Sword
    } else if (!gSaveContext.n64ddFlag && (msgCtx->textId == 0xB4 && (gSaveContext.eventChkInf[9] & 0x40))) {
        textId = 0xB5; // Destroyed Gold Skulltula
    }
    // Ocarina Staff + Dialog
    if (textId == 0x4077 || // Pierre?
        textId == 0x407A || // Pierre?
        textId == 0x2061 || // Learning Epona's Song
        textId == 0x5035 || // Guru-Guru in Windmill
        textId == 0x40AC) { // Ocarina Frog Minigame
        Interface_ChangeAlpha(1);
    }
    msgCtx->textId = textId;

    if (textId == 0x2030) { // Talking to Ingo as adult in Lon Lon Ranch for the first time before freeing Epona
        osSyncPrintf(VT_FGCOL(YELLOW));
        osSyncPrintf("？？？？？？？？？？？？？？？？  z_message.c  ？？？？？？？？？？？？？？？？？？\n");
        osSyncPrintf(VT_RST);
        gSaveContext.eventInf[0] = gSaveContext.eventInf[1] = gSaveContext.eventInf[2] = gSaveContext.eventInf[3] = 0;
    }

    // RANDOTODO: Use this for ice trap messages
    if (CustomMessage_RetrieveIfExists(play)) {
        osSyncPrintf("Found custom message");
    } else if (sTextIsCredits) {
        Message_FindCreditsMessage(play, textId);
        msgCtx->msgLength = font->msgLength;
        char* src = (uintptr_t)font->msgOffset;
        memcpy(font->msgBuf, src, font->msgLength);

        // OTRTODO
        //DmaMgr_SendRequest1(font->msgBuf, (uintptr_t)(_staff_message_data_staticSegmentRomStart + 4 + font->msgOffset),
                            //font->msgLength, __FILE__, __LINE__);

    } else if (CVar_GetS32("gAskToEquip", 0) &&
               (((LINK_IS_ADULT || CVar_GetS32("gTimelessEquipment", 0)) &&
                 // 0C = Biggoron, 4B = Giant's, 4E = Mirror Shield, 50-51 = Tunics
                 (textId == 0x0C || textId == 0x4B || textId == 0x4E ||
                  textId == 0x50 || textId == 0x51)) ||
                ((!LINK_IS_ADULT || CVar_GetS32("gTimelessEquipment", 0)) &&
                // 4C = Deku Shield, A4 = Kokiri Sword
                 (textId == 0x4C || textId == 0xA4)) ||
                // 4D == Hylian Shield
                textId == 0x4D)) {
        Message_FindMessage(play, textId);
        msgCtx->msgLength = font->msgLength = GetEquipNowMessage(font->msgBuf, font->msgOffset, sizeof(font->msgBuf));
    } else {
        Message_FindMessage(play, textId);
        msgCtx->msgLength = font->msgLength;
        char* src = (uintptr_t)font->msgOffset;
        memcpy(font->msgBuf, src, font->msgLength);
    }

    msgCtx->textBoxProperties = font->charTexBuf[0];
    msgCtx->textBoxType = msgCtx->textBoxProperties >> 4;
    msgCtx->textBoxPos = msgCtx->textBoxProperties & 0xF;
    textBoxType = msgCtx->textBoxType;
    // "Text Box Type"
    osSyncPrintf("吹き出し種類＝%d\n", msgCtx->textBoxType);
    if (textBoxType < TEXTBOX_TYPE_NONE_BOTTOM) {
        const char* textureName = msgStaticTbl[messageStaticIndices[textBoxType]];
        memcpy(msgCtx->textboxSegment, ResourceMgr_LoadTexByName(textureName), MESSAGE_STATIC_TEX_SIZE);
        if (textBoxType == TEXTBOX_TYPE_BLACK) {
            msgCtx->textboxColorRed = 0;
            msgCtx->textboxColorGreen = 0;
            msgCtx->textboxColorBlue = 0;
        } else if (textBoxType == TEXTBOX_TYPE_WOODEN) {
            msgCtx->textboxColorRed = 70;
            msgCtx->textboxColorGreen = 50;
            msgCtx->textboxColorBlue = 30;
        } else if (textBoxType == TEXTBOX_TYPE_BLUE) {
            msgCtx->textboxColorRed = 0;
            msgCtx->textboxColorGreen = 10;
            msgCtx->textboxColorBlue = 50;
        } else {
            msgCtx->textboxColorRed = 255;
            msgCtx->textboxColorGreen = 0;
            msgCtx->textboxColorBlue = 0;
        }
        if (textBoxType == TEXTBOX_TYPE_WOODEN) {
            msgCtx->textboxColorAlphaTarget = 230;
        } else if (textBoxType == TEXTBOX_TYPE_OCARINA) {
            msgCtx->textboxColorAlphaTarget = 180;
        } else {
            msgCtx->textboxColorAlphaTarget = 170;
        }
        msgCtx->textboxColorAlphaCurrent = 0;
    }
    msgCtx->choiceNum = msgCtx->textUnskippable = msgCtx->textboxEndType = 0;
    msgCtx->msgBufPos = msgCtx->unk_E3D0 = msgCtx->textDrawPos = 0;
}

void Message_StartTextbox(PlayState* play, u16 textId, Actor* actor) {
    MessageContext* msgCtx = &play->msgCtx;

    osSyncPrintf(VT_FGCOL(BLUE));
    // "Message"
    osSyncPrintf("めっせーじ＝%x(%d)\n", textId, actor);
    osSyncPrintf(VT_RST);

    msgCtx->ocarinaAction = 0xFFFF;
    // we need the talkActor for gossip stones in rando
    // so we need to switch the order of these lines
    if (gSaveContext.n64ddFlag && textId == 0x2053) {
        msgCtx->talkActor = actor;
        Message_OpenText(play, textId);
    } else {
        Message_OpenText(play, textId);
        msgCtx->talkActor = actor;
    }
    msgCtx->msgMode = MSGMODE_TEXT_START;
    msgCtx->stateTimer = 0;
    msgCtx->textDelayTimer = 0;
    play->msgCtx.ocarinaMode = OCARINA_MODE_00;
}

void Message_ContinueTextbox(PlayState* play, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;

    osSyncPrintf(VT_FGCOL(GREEN));
    // "Message"
    osSyncPrintf("めっせーじ＝%x  message->msg_data\n", textId, msgCtx->msgLength);
    osSyncPrintf(VT_RST);

    msgCtx->msgLength = 0;
    Message_OpenText(play, textId);
    msgCtx->msgMode = MSGMODE_NONE;
    msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;
    msgCtx->msgMode = MSGMODE_TEXT_CONTINUING;
    msgCtx->stateTimer = 3;
    msgCtx->textboxEndType = msgCtx->msgBufPos = msgCtx->unk_E3D0 = msgCtx->textDrawPos = msgCtx->textDelayTimer = 0;
    msgCtx->textColorAlpha = 255;

    if (YREG(31) == 0 && play->interfaceCtx.unk_1FA == 0) {
        Interface_SetDoAction(play, DO_ACTION_NEXT);
    }
    msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;
}

void Message_StartOcarina(PlayState* play, u16 ocarinaActionId) {
    static u16 sOcarinaSongFlagsMap[] = {
        (1 << OCARINA_SONG_MINUET),    (1 << OCARINA_SONG_BOLERO),   (1 << OCARINA_SONG_SERENADE),
        (1 << OCARINA_SONG_REQUIEM),   (1 << OCARINA_SONG_NOCTURNE), (1 << OCARINA_SONG_PRELUDE),
        (1 << OCARINA_SONG_LULLABY),   (1 << OCARINA_SONG_EPONAS),   (1 << OCARINA_SONG_SARIAS),
        (1 << OCARINA_SONG_SUNS),      (1 << OCARINA_SONG_TIME),     (1 << OCARINA_SONG_STORMS),
        (1 << OCARINA_SONG_SCARECROW),
    };
    MessageContext* msgCtx = &play->msgCtx;
    s32 textId;
    s16 j;
    s16 i;
    s16 noStop;
    s32 k;

    osSyncPrintf(VT_FGCOL(GREEN));

    for (i = sOcarinaSongBitFlags = 0; i < (QUEST_KOKIRI_EMERALD - QUEST_SONG_MINUET); i++) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_MINUET + i)) {
            osSyncPrintf("ocarina_check_bit[%d]=%x\n", i, sOcarinaSongFlagsMap[i]);
            sOcarinaSongBitFlags |= sOcarinaSongFlagsMap[i];
        }
    }
    if (gSaveContext.scarecrowSpawnSongSet) {
        sOcarinaSongBitFlags |= (1 << OCARINA_SONG_SCARECROW);
    }
    osSyncPrintf("ocarina_bit = %x\n", sOcarinaSongBitFlags);
    osSyncPrintf(VT_RST);

    sHasSunsSong = CHECK_QUEST_ITEM(QUEST_SONG_SUN);
    msgCtx->ocarinaStaff = Audio_OcaGetRecordingStaff();
    msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
    sOcarinaNoteBufLen = 0;
    Message_ResetOcarinaNoteState();
    sLastPlayedSong = msgCtx->unk_E3F2 = msgCtx->lastOcaNoteIdx = 0xFF;

    // "Ocarina Number"
    osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ オカリナ番号＝%d(%d) ☆☆☆☆☆\n" VT_RST, ocarinaActionId, 2);
    noStop = false;
    if (ocarinaActionId >= 0x893) {
        Message_OpenText(play, ocarinaActionId); // You played the [song name]
        textId = ocarinaActionId + 0x86E;
    } else if (ocarinaActionId == OCARINA_ACTION_MEMORY_GAME) {
        msgCtx->ocarinaAction = ocarinaActionId;
        Message_OpenText(play, 0x86D); // Play using [A] and [C].
        textId = ocarinaActionId + 0x86E;
    } else if (ocarinaActionId == OCARINA_ACTION_FREE_PLAY || ocarinaActionId >= OCARINA_ACTION_CHECK_SARIA) {
        osSyncPrintf("ocarina_set 000000000000000000  = %d\n", ocarinaActionId);
        msgCtx->ocarinaAction = ocarinaActionId;
        if (ocarinaActionId >= OCARINA_ACTION_CHECK_SARIA && ocarinaActionId <= OCARINA_ACTION_CHECK_STORMS) {
            Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        if (ocarinaActionId == OCARINA_ACTION_SCARECROW_PLAYBACK) {
            Message_OpenText(play, 0x86F); // Ocarina
            textId = ocarinaActionId + 0x86E;
        } else {
            Message_OpenText(play, 0x86E); // Play using [A] and [C]; [B] to Stop.
            textId = ocarinaActionId + 0x86E;
        }
    } else {
        msgCtx->ocarinaAction = ocarinaActionId;
        noStop = true;
        if (ocarinaActionId >= OCARINA_ACTION_PLAYBACK_MINUET) {
            osSyncPrintf("222222222\n");
            Message_OpenText(play, 0x86D); // Play using [A] and [C].
            textId = ocarinaActionId + 0x86E;
        } else {
            osSyncPrintf("333333333\n");
            textId = ocarinaActionId + 0x86E;
            Message_OpenText(play, textId); // Play using [A] and [C]; [B] to Stop.
        }
    }
    msgCtx->talkActor = NULL;
    // "Ocarina Mode"
    osSyncPrintf("オカリナモード = %d  (%x)\n", msgCtx->ocarinaAction, textId);
    msgCtx->textDelayTimer = 0;
    play->msgCtx.ocarinaMode = OCARINA_MODE_00;
    R_TEXTBOX_X = 34;
    R_TEXTBOX_Y = 142;
    R_TEXTBOX_TEXWIDTH = 512;
    R_TEXTBOX_TEXHEIGHT = 512;
    R_TEXTBOX_WIDTH = 256;
    R_TEXTBOX_HEIGHT = 64;
    msgCtx->stateTimer = 0;
    if (YREG(12) != 0) {
        msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
    } else {
        msgCtx->stateTimer = 2;
        msgCtx->msgMode = MSGMODE_TEXT_CONTINUING;
    }
    msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;
    if (noStop == false) {
        Interface_LoadActionLabelB(play, DO_ACTION_STOP);
        noStop = gSaveContext.unk_13EA;
        Interface_ChangeAlpha(0xA);
        gSaveContext.unk_13EA = noStop;
    }
    // "Music Performance Start"
    osSyncPrintf("演奏開始\n");
    if (ocarinaActionId == OCARINA_ACTION_FREE_PLAY || ocarinaActionId == OCARINA_ACTION_CHECK_NOWARP) {
        msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
        msgCtx->textBoxType = 0x63;
    } else if (ocarinaActionId == OCARINA_ACTION_FROGS) {
        msgCtx->msgMode = MSGMODE_FROGS_START;
        msgCtx->textBoxType = TEXTBOX_TYPE_BLUE;
    } else if (ocarinaActionId == OCARINA_ACTION_MEMORY_GAME) {
        Interface_ChangeAlpha(1);
        Message_Decode(play);
        msgCtx->msgMode = MSGMODE_MEMORY_GAME_START;
    } else if (ocarinaActionId == OCARINA_ACTION_SCARECROW_LONG_PLAYBACK) {
        // "?????Recording Playback / Recording Playback / Recording Playback / Recording Playback -> "
        osSyncPrintf("?????録音再生 録音再生 録音再生 録音再生  -> ");
        Audio_OcaSetInstrument(1);
        Audio_OcaSetInstrument(1);
        msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
        sOcarinaNoteBufPos = sOcarinaNoteBufLen = 0;
        msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos;
        Message_ResetOcarinaNoteState();
        msgCtx->stateTimer = 3;
        msgCtx->msgMode = MSGMODE_SCARECROW_LONG_PLAYBACK;
        Audio_OcaSetSongPlayback(OCARINA_SONG_SCARECROW_LONG + 1, 1);
        gSaveContext.unk_13EA = 0;
        Interface_ChangeAlpha(1);
    }
    for (k = 0, j = 0; j < 48; j++, k += 0x80) {
        func_8006EE50(&play->msgCtx.font, 0x8140, k);
    }
}

void func_8010BD58(PlayState* play, u16 ocarinaActionId) {
    play->msgCtx.unk_E40E = 0;
    Message_StartOcarina(play, ocarinaActionId);
}

void func_8010BD88(PlayState* play, u16 ocarinaActionId) {
    play->msgCtx.unk_E40E = 1;
    Message_StartOcarina(play, ocarinaActionId);
}

u8 Message_GetState(MessageContext* msgCtx) {
    u8 state;

    if (msgCtx->msgLength == 0) {
        state = TEXT_STATE_NONE;
    } else if (msgCtx->msgMode == MSGMODE_TEXT_DONE) {
        if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_HAS_NEXT) {
            state = TEXT_STATE_DONE_HAS_NEXT;
        } else if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_2_CHOICE ||
                   msgCtx->textboxEndType == TEXTBOX_ENDTYPE_3_CHOICE) {
            state = TEXT_STATE_CHOICE;
        } else if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_EVENT ||
                   msgCtx->textboxEndType == TEXTBOX_ENDTYPE_PERSISTENT) {
            state = TEXT_STATE_EVENT;
        } else if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADING) {
            state = TEXT_STATE_DONE_FADING;
        } else {
            state = TEXT_STATE_DONE;
        }
    } else if (msgCtx->msgMode == MSGMODE_TEXT_AWAIT_NEXT) {
        state = TEXT_STATE_AWAITING_NEXT;
    } else if (msgCtx->msgMode == MSGMODE_SONG_DEMONSTRATION_DONE) {
        state = TEXT_STATE_SONG_DEMO_DONE;
    } else if (msgCtx->ocarinaMode == OCARINA_MODE_03) {
        state = TEXT_STATE_8;
    } else if (msgCtx->msgMode == MSGMODE_OCARINA_AWAIT_INPUT) {
        state = TEXT_STATE_9;
    } else if (msgCtx->msgMode == MSGMODE_TEXT_CLOSING && msgCtx->stateTimer == 1) {
        state = TEXT_STATE_CLOSING;
    } else {
        state = TEXT_STATE_DONE_FADING;
    }
    return state;
}

void Message_DrawTextBox(PlayState* play, Gfx** p) {
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx = *p;

    gSPInvalidateTexCache(gfx++, msgCtx->textboxSegment);
    gDPPipeSync(gfx++);
    gDPSetPrimColor(gfx++, 0, 0, msgCtx->textboxColorRed, msgCtx->textboxColorGreen, msgCtx->textboxColorBlue,
                    msgCtx->textboxColorAlphaCurrent);

    if (!(msgCtx->textBoxType) || msgCtx->textBoxType == TEXTBOX_TYPE_BLUE) {
        gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment, G_IM_FMT_I, 128, 64, 0, G_TX_MIRROR, G_TX_NOMIRROR, 7, 0,
                               G_TX_NOLOD, G_TX_NOLOD);
    } else {
        if (msgCtx->textBoxType == TEXTBOX_TYPE_OCARINA) {
            gDPSetEnvColor(gfx++, 0, 0, 0, 255);
        } else {
            gDPSetEnvColor(gfx++, 50, 20, 0, 255);
        }

        gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment, G_IM_FMT_IA, 128, 64, 0, G_TX_MIRROR, G_TX_MIRROR, 7, 0,
                               G_TX_NOLOD, G_TX_NOLOD);
    }

    gSPTextureRectangle(gfx++, R_TEXTBOX_X << 2, R_TEXTBOX_Y << 2, (R_TEXTBOX_X + R_TEXTBOX_WIDTH) << 2,
                        (R_TEXTBOX_Y + R_TEXTBOX_HEIGHT) << 2, G_TX_RENDERTILE, 0, 0, R_TEXTBOX_TEXWIDTH << 1,
                        R_TEXTBOX_TEXHEIGHT << 1);

    // Draw treble clef
    if (msgCtx->textBoxType == TEXTBOX_TYPE_OCARINA) {
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 255, 100, 0, 255);
        gDPLoadTextureBlock_4b(gfx++, gOcarinaTrebleClefTex, G_IM_FMT_I, 16, 32, 0, G_TX_MIRROR, G_TX_MIRROR,
                               G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gfx++, R_TEXTBOX_CLEF_XPOS << 2, R_TEXTBOX_CLEF_YPOS << 2, (R_TEXTBOX_CLEF_XPOS + 16) << 2,
                            (R_TEXTBOX_CLEF_YPOS + 32) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    *p = gfx;
}

void Message_SetView(View* view) {
    SET_FULLSCREEN_VIEWPORT(view);
    func_800AB2C4(view);
}

/**
 * Draws the textbox in full and updates ocarina state
 */
void Message_DrawMain(PlayState* play, Gfx** p) {
    static s16 sOcarinaEffectActorIds[] = {
        ACTOR_OCEFF_WIPE3, ACTOR_OCEFF_WIPE2, ACTOR_OCEFF_WIPE,  ACTOR_OCEFF_SPOT,
        ACTOR_OCEFF_WIPE,  ACTOR_OCEFF_STORM, ACTOR_OCEFF_WIPE4,
    };
    static s16 sOcarinaEffectActorParams[] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0000, 0x0000 };
    static void* sOcarinaNoteTextures[] = {
        gOcarinaATex, gOcarinaCDownTex, gOcarinaCRightTex, gOcarinaCLeftTex, gOcarinaCUpTex,
    };
    s16 sOcarinaNoteAPrimColors[2][3];
    s16 sOcarinaNoteAPrimColors_GCN[][3] = {
        { 80, 255, 150 },
        { 100, 255, 200 },
    };
    s16 sOcarinaNoteAPrimColors_N64[][3] = {
        { 80, 255, 150 },
        { 100, 255, 200 },
    };
    Color_RGB8 ABtnColor = CVar_GetRGB("gCCABtnPrim", (Color_RGB8){ .r = 80, .g = 255, .b = 150});
    s16 ABtnR = ABtnColor.r;
    s16 ABtnG = ABtnColor.g;
    s16 ABtnB = ABtnColor.b;
    s16 ABtnR_2 = ABtnColor.r+20;
    s16 ABtnG_2 = ABtnColor.g+20;
    s16 ABtnB_2 = ABtnColor.b+20;
    if(ABtnR_2 > 255){ABtnR_2=255;};
    if(ABtnG_2 > 255){ABtnG_2=255;};
    if(ABtnB_2 > 255){ABtnB_2=255;};
    s16 sOcarinaNoteAPrimColors_CUSTOM[][3] = {
        { ABtnG, ABtnG, ABtnB },
        { ABtnR_2, ABtnG_2, ABtnB_2 },
    };
    static s16 sOcarinaNoteAEnvColors[][3] = {
        { 10, 10, 10 },
        { 50, 255, 50 },
    };
    s16 sOcarinaNoteCPrimColors[2][3];
    s16 sOcarinaNoteCPrimColors_GCN[][3] = {
        { 255, 255, 50 },
        { 255, 255, 180 },
    };

	Color_RGB8 CBtnDefaultColor = {255, 255, 50};
    Color_RGB8 CBtnColor = CVar_GetRGB("gCCCBtnPrim", CBtnDefaultColor);
    s16 CBtnR = CBtnColor.r;
    s16 CBtnG = CBtnColor.g;
    s16 CBtnB = CBtnColor.b;
    s16 CBtnR_2 = CBtnColor.r+20;
    s16 CBtnG_2 = CBtnColor.g+20;
    s16 CBtnB_2 = CBtnColor.b+20;
    Color_RGB8 CUpColor = CVar_GetRGB("gCCCUBtnPrim", CBtnDefaultColor);
    s16 CBtnRU = CUpColor.r;
    s16 CBtnGU = CUpColor.g;
    s16 CBtnBU = CUpColor.b;
    Color_RGB8 CLeftColor = CVar_GetRGB("gCCCLBtnPrim", CBtnDefaultColor);
    s16 CBtnRL = CLeftColor.r;
    s16 CBtnGL = CLeftColor.g;
    s16 CBtnBL = CLeftColor.b;
    Color_RGB8 CDownColor = CVar_GetRGB("gCCCDBtnPrim", CBtnDefaultColor);
    s16 CBtnRD = CDownColor.r;
    s16 CBtnGD = CDownColor.g;
    s16 CBtnBD = CDownColor.b;
    Color_RGB8 CRightColor = CVar_GetRGB("gCCCRBtnPrim", CBtnDefaultColor);
    s16 CBtnRR = CRightColor.r;
    s16 CBtnGR = CRightColor.g;
    s16 CBtnBR = CRightColor.b;
    if(CBtnR_2 > 255){CBtnR_2=255;};
    if(CBtnG_2 > 255){CBtnG_2=255;};
    if(CBtnB_2 > 255){CBtnB_2=255;};
    s16 sOcarinaNoteCPrimColors_CUSTOM[][3] = {
        { CBtnR, CBtnG, CBtnB },    //Unified
        { CBtnR_2, CBtnG_2, CBtnB_2 },
        { CBtnRL, CBtnGL, CBtnBL }, //Left
        { CBtnRD, CBtnGD, CBtnBD }, //Down
        { CBtnRR, CBtnGR, CBtnBR }, //Right
        { CBtnRU, CBtnGU, CBtnBU }, //Up
    };
    static s16 sOcarinaNoteCEnvColors[][3] = {
        { 10, 10, 10 },
        { 110, 110, 50 },
    };
    if (CVar_GetS32("gHudColors", 1) == 0) { //N64
        sOcarinaNoteAPrimColors < sOcarinaNoteAPrimColors_N64;
        sOcarinaNoteCPrimColors < sOcarinaNoteCPrimColors_GCN; //GCN and N64 share same C buttons color.
    } else if (CVar_GetS32("gHudColors", 1) == 1) { //GCN
        sOcarinaNoteAPrimColors < sOcarinaNoteAPrimColors_GCN;
        sOcarinaNoteCPrimColors < sOcarinaNoteCPrimColors_GCN;
    } else if (CVar_GetS32("gHudColors", 1) == 2) { //Custom
        sOcarinaNoteAPrimColors < sOcarinaNoteAPrimColors_CUSTOM;
        sOcarinaNoteCPrimColors < sOcarinaNoteCPrimColors_CUSTOM;
    }
    static s16 sOcarinaNoteFlashTimer = 12;
    static s16 sOcarinaNoteFlashColorIdx = 1;
    static s16 sOcarinaSongFanfares[] = {
        NA_BGM_OCA_MINUET,   NA_BGM_OCA_BOLERO, NA_BGM_OCA_SERENADE, NA_BGM_OCA_REQUIEM,
        NA_BGM_OCA_NOCTURNE, NA_BGM_OCA_LIGHT,  NA_BGM_OCA_SARIA,    NA_BGM_OCA_EPONA,
        NA_BGM_OCA_ZELDA,    NA_BGM_OCA_SUNS,   NA_BGM_OCA_TIME,     NA_BGM_OCA_STORM,
    };
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    MessageContext* msgCtx = &play->msgCtx;
    u16 noteBufPos;
    Player* player = GET_PLAYER(play);
    s32 pad;
    Gfx* gfx = *p;
    s16 r;
    s16 g;
    s16 b;
    s16 ru;//Red Up
    s16 gu;//Green Up
    s16 bu;//Blue Up
    s16 rl;//Red Left
    s16 gl;//green Left
    s16 bl;//Blue Left
    s16 rd;//And so on
    s16 gd;
    s16 bd;
    s16 rr;
    s16 gr;
    s16 br;
    u16 i;
    u16 notePosX;
    u16 pad1;
    u16 j;

    gSPSegment(gfx++, 0x02, play->interfaceCtx.parameterSegment);
    gSPSegment(gfx++, 0x07, msgCtx->textboxSegment);

    if (msgCtx->msgLength != 0) {
        if (msgCtx->ocarinaAction != OCARINA_ACTION_FROGS && msgCtx->msgMode != MSGMODE_SONG_PLAYED_ACT &&
            msgCtx->msgMode >= MSGMODE_TEXT_BOX_GROWING && msgCtx->msgMode < MSGMODE_TEXT_CLOSING &&
            msgCtx->textBoxType < TEXTBOX_TYPE_NONE_BOTTOM) {
            Message_SetView(&msgCtx->view);
            func_8009457C(&gfx);
            Message_DrawTextBox(play, &gfx);
        }

        func_8009457C(&gfx);

        gDPSetAlphaCompare(gfx++, G_AC_NONE);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);

            bool isB_Held = CVar_GetS32("gSkipText", 0) != 0 ? CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_B)
                                                         : CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B);

        switch (msgCtx->msgMode) {
            case MSGMODE_TEXT_START:
            case MSGMODE_TEXT_BOX_GROWING:
            case MSGMODE_TEXT_STARTING:
            case MSGMODE_TEXT_NEXT_MSG:
                break;
            case MSGMODE_TEXT_CONTINUING:
                if (msgCtx->stateTimer == 1) {
                    for (j = 0, i = 0; i < 48; i++, j += 0x80) {
                        func_8006EE50(&play->msgCtx.font, 0x8140, j);
                    }
                    Message_DrawText(play, &gfx);
                }
                break;
            case MSGMODE_TEXT_DISPLAYING:
            case MSGMODE_TEXT_DELAYED_BREAK:
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_TEXT_AWAIT_INPUT:
            case MSGMODE_TEXT_AWAIT_NEXT:
                Message_DrawText(play, &gfx);
                Message_DrawTextboxIcon(play, &gfx, R_TEXTBOX_END_XPOS, R_TEXTBOX_END_YPOS);
                break;
            case MSGMODE_OCARINA_STARTING:
            case MSGMODE_SONG_DEMONSTRATION_STARTING:
            case MSGMODE_SONG_PLAYBACK_STARTING:
                Audio_OcaSetInstrument(1);
                msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                play->msgCtx.ocarinaMode = OCARINA_MODE_01;
                Message_ResetOcarinaNoteState();
                sOcarinaNoteFlashTimer = 3;
                sOcarinaNoteFlashColorIdx = 1;
                if (msgCtx->msgMode == MSGMODE_OCARINA_STARTING) {
                    if (msgCtx->ocarinaAction == OCARINA_ACTION_UNK_0 ||
                        msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY ||
                        msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_RECORDING ||
                        msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOWARP ||
                        msgCtx->ocarinaAction >= OCARINA_ACTION_CHECK_SARIA) {
                        if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY ||
                            msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOWARP) {
                            func_800ECC04(sOcarinaSongBitFlags + 0xC000);
                        } else {
                            // "On Stage Performance"
                            osSyncPrintf("台上演奏\n");
                            func_800ECC04(sOcarinaSongBitFlags);
                        }
                    } else {
                        osSyncPrintf("Na_StartOcarinaSinglePlayCheck2( message->ocarina_no );\n");
                        func_800ECC04((1 << (msgCtx->ocarinaAction % 32)) + 0x8000);
                    }
                    msgCtx->msgMode = MSGMODE_OCARINA_PLAYING;
                } else if (msgCtx->msgMode == MSGMODE_SONG_DEMONSTRATION_STARTING) {
                    msgCtx->stateTimer = 20;
                    msgCtx->msgMode = MSGMODE_SONG_DEMONSTRATION_SELECT_INSTRUMENT;
                } else {
                    func_800ECC04((1 << ((msgCtx->ocarinaAction + 0x11) % 32)) + 0x8000);
                    // "Performance Check"
                    osSyncPrintf("演奏チェック=%d\n", msgCtx->ocarinaAction - OCARINA_ACTION_PLAYBACK_MINUET);
                    msgCtx->msgMode = MSGMODE_SONG_PLAYBACK;
                }
                if (msgCtx->ocarinaAction != OCARINA_ACTION_FREE_PLAY &&
                    msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOWARP) {
                    Message_DrawText(play, &gfx);
                }
                break;
            case MSGMODE_OCARINA_PLAYING:
                msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                if (msgCtx->ocarinaStaff->pos) {
                    osSyncPrintf("locate=%d  onpu_pt=%d\n", msgCtx->ocarinaStaff->pos, sOcarinaNoteBufPos);
                    if (msgCtx->ocarinaStaff->pos == 1 && sOcarinaNoteBufPos == 8) {
                        sOcarinaNoteBufPos = 0;
                    }
                    if (sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                        msgCtx->lastOcaNoteIdx = sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos - 1] =
                            msgCtx->ocarinaStaff->noteIdx;
                        sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos] = OCARINA_NOTE_INVALID;
                        sOcarinaNoteBufPos++;
                    }
                }
                msgCtx->lastPlayedSong = msgCtx->ocarinaStaff->state;
                if (msgCtx->ocarinaStaff->state < OCARINA_SONG_MEMORY_GAME) {
                    if (msgCtx->ocarinaStaff->state == OCARINA_SONG_SCARECROW ||
                        CHECK_QUEST_ITEM(QUEST_SONG_MINUET + gOcarinaSongItemMap[msgCtx->ocarinaStaff->state])) {
                        sLastPlayedSong = msgCtx->unk_E3F2 = msgCtx->lastPlayedSong = msgCtx->ocarinaStaff->state;
                        msgCtx->msgMode = MSGMODE_OCARINA_CORRECT_PLAYBACK;
                        msgCtx->stateTimer = 20;
                        if (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOWARP) {
                            if (msgCtx->ocarinaStaff->state < OCARINA_SONG_SARIAS ||
                                msgCtx->ocarinaStaff->state == OCARINA_SONG_SCARECROW) {
                                Audio_OcaSetInstrument(0);
                                Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                       &D_801333E8);
                                msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
                            } else {
                                // "Ocarina_Flog Correct Example Performance"
                                osSyncPrintf("Ocarina_Flog 正解模範演奏=%x\n", msgCtx->lastPlayedSong);
                                Message_ContinueTextbox(play, 0x86F); // Ocarina
                                msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                                msgCtx->textBoxType = TEXTBOX_TYPE_OCARINA;
                                msgCtx->stateTimer = 10;
                                Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0,
                                                       &D_801333E0, &D_801333E8);
                                Interface_ChangeAlpha(1);
                            }
                        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_SCARECROW) {
                            if (msgCtx->ocarinaStaff->state < OCARINA_SONG_SCARECROW) {
                                Audio_OcaSetInstrument(0);
                                Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                       &D_801333E8);
                                msgCtx->stateTimer = 10;
                                msgCtx->msgMode = MSGMODE_OCARINA_FAIL;
                            } else {
                                // "Ocarina_Flog Correct Example Performance"
                                osSyncPrintf("Ocarina_Flog 正解模範演奏=%x\n", msgCtx->lastPlayedSong);
                                Message_ContinueTextbox(play, 0x86F); // Ocarina
                                msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                                msgCtx->textBoxType = TEXTBOX_TYPE_OCARINA;
                                msgCtx->stateTimer = 10;
                                Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0,
                                                       &D_801333E0, &D_801333E8);
                                Interface_ChangeAlpha(1);
                            }
                        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) {
                            // "Ocarina_Free Correct Example Performance"
                            osSyncPrintf("Ocarina_Free 正解模範演奏=%x\n", msgCtx->lastPlayedSong);
                            Message_ContinueTextbox(play, 0x86F); // Ocarina
                            msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                            msgCtx->textBoxType = TEXTBOX_TYPE_OCARINA;
                            msgCtx->stateTimer = 10;
                            Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        } else {
                            Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }
                        Interface_ChangeAlpha(1);
                    } else {
                        Audio_OcaSetInstrument(0);
                        Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
                    }
                } else if (msgCtx->ocarinaStaff->state == 0xFF) {
                    Audio_OcaSetInstrument(0);
                    Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    msgCtx->stateTimer = 10;
                    msgCtx->msgMode = MSGMODE_OCARINA_FAIL;
                } else if (isB_Held) {
                    Audio_OcaSetInstrument(0);
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    Message_CloseTextbox(play);
                }
                if (msgCtx->ocarinaAction != OCARINA_ACTION_FREE_PLAY &&
                    msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOWARP) {
                    Message_DrawText(play, &gfx);
                }
                break;
            case MSGMODE_OCARINA_CORRECT_PLAYBACK:
            case MSGMODE_SONG_PLAYBACK_SUCCESS:
            case MSGMODE_SCARECROW_RECORDING_DONE:
                r = ABS(sOcarinaNoteD4Prim.r - sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][0]) /
                    sOcarinaNoteFlashTimer;
                g = ABS(sOcarinaNoteD4Prim.g - sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][1]) /
                    sOcarinaNoteFlashTimer;
                b = ABS(sOcarinaNoteD4Prim.b - sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][2]) /
                    sOcarinaNoteFlashTimer;

                if (sOcarinaNoteD4Prim.r >= sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][0]) {
                    sOcarinaNoteD4Prim.r -= r;
                } else {
                    sOcarinaNoteD4Prim.r += r;
                }
                if (sOcarinaNoteD4Prim.g >= sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][1]) {
                    sOcarinaNoteD4Prim.g -= g;
                } else {
                    sOcarinaNoteD4Prim.g += g;
                }
                if (sOcarinaNoteD4Prim.b >= sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][2]) {
                    sOcarinaNoteD4Prim.b -= b;
                } else {
                    sOcarinaNoteD4Prim.b += b;
                }

                r = ABS(sOcarinaNoteABtnEnv.r - sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][0]) /
                    sOcarinaNoteFlashTimer;
                g = ABS(sOcarinaNoteABtnEnv.g - sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][1]) /
                    sOcarinaNoteFlashTimer;
                b = ABS(sOcarinaNoteABtnEnv.b - sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][2]) /
                    sOcarinaNoteFlashTimer;

                if (sOcarinaNoteCBtnEnv.r >= sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][0]) {
                    sOcarinaNoteABtnEnv.r -= r;
                } else {
                    sOcarinaNoteABtnEnv.r += r;
                }
                if (sOcarinaNoteCBtnEnv.g >= sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][1]) {
                    sOcarinaNoteABtnEnv.g -= g;
                } else {
                    sOcarinaNoteABtnEnv.g += g;
                }
                if (sOcarinaNoteCBtnEnv.b >= sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][2]) {
                    sOcarinaNoteABtnEnv.b -= b;
                } else {
                    sOcarinaNoteABtnEnv.b += b;
                }

                r = ABS(sOcarinaNoteCBtnPrim.r - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][0]) /
                    sOcarinaNoteFlashTimer;
                g = ABS(sOcarinaNoteCBtnPrim.g - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][1]) /
                    sOcarinaNoteFlashTimer;
                b = ABS(sOcarinaNoteCBtnPrim.b - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][2]) /
                    sOcarinaNoteFlashTimer;

                ru = ABS(sOcarinaNoteD5Prim.r - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][0]) /
                    sOcarinaNoteFlashTimer;
                gu = ABS(sOcarinaNoteD5Prim.g - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][1]) /
                    sOcarinaNoteFlashTimer;
                bu = ABS(sOcarinaNoteD5Prim.b - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][2]) /
                    sOcarinaNoteFlashTimer;
                rl = ABS(sOcarinaNoteB4Prim.r - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][0]) /
                    sOcarinaNoteFlashTimer;
                gl = ABS(sOcarinaNoteB4Prim.g - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][1]) /
                    sOcarinaNoteFlashTimer;
                bl = ABS(sOcarinaNoteB4Prim.b - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][2]) /
                    sOcarinaNoteFlashTimer;
                rd = ABS(sOcarinaNoteF4Prim.r - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][0]) /
                    sOcarinaNoteFlashTimer;
                gd = ABS(sOcarinaNoteF4Prim.g - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][1]) /
                    sOcarinaNoteFlashTimer;
                bd = ABS(sOcarinaNoteF4Prim.b - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][2]) /
                    sOcarinaNoteFlashTimer;
                rr = ABS(sOcarinaNoteA4Prim.r - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][0]) /
                    sOcarinaNoteFlashTimer;
                gr = ABS(sOcarinaNoteA4Prim.g - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][1]) /
                    sOcarinaNoteFlashTimer;
                br = ABS(sOcarinaNoteA4Prim.b - sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][2]) /
                    sOcarinaNoteFlashTimer;

                if (sOcarinaNoteCBtnPrim.r >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][0]) {
                    sOcarinaNoteCBtnPrim.r -= r;
                } else {
                    sOcarinaNoteCBtnPrim.r += r;
                }
                if (sOcarinaNoteCBtnPrim.g >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][1]) {
                    sOcarinaNoteCBtnPrim.g -= g;
                } else {
                    sOcarinaNoteCBtnPrim.g += g;
                }
                if (sOcarinaNoteCBtnPrim.b >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][2]) {
                    sOcarinaNoteCBtnPrim.b -= b;
                } else {
                    sOcarinaNoteCBtnPrim.b += b;
                }

                if (sOcarinaNoteD5Prim.r >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][0]) {
                    sOcarinaNoteD5Prim.r -= ru;
                } else {
                    sOcarinaNoteD5Prim.r += ru;
                }
                if (sOcarinaNoteD5Prim.g >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][1]) {
                    sOcarinaNoteD5Prim.g -= gu;
                } else {
                    sOcarinaNoteD5Prim.g += gu;
                }
                if (sOcarinaNoteD5Prim.b >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][2]) {
                    sOcarinaNoteD5Prim.b -= bu;
                } else {
                    sOcarinaNoteD5Prim.b += bu;
                }

                if (sOcarinaNoteB4Prim.r >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][0]) {
                    sOcarinaNoteB4Prim.r -= rl;
                } else {
                    sOcarinaNoteB4Prim.r += rl;
                }
                if (sOcarinaNoteB4Prim.g >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][1]) {
                    sOcarinaNoteB4Prim.g -= gl;
                } else {
                    sOcarinaNoteB4Prim.g += gl;
                }
                if (sOcarinaNoteB4Prim.b >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][2]) {
                    sOcarinaNoteB4Prim.b -= bl;
                } else {
                    sOcarinaNoteB4Prim.b += bl;
                }

                if (sOcarinaNoteF4Prim.r >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][0]) {
                    sOcarinaNoteF4Prim.r -= rd;
                } else {
                    sOcarinaNoteF4Prim.r += rd;
                }
                if (sOcarinaNoteF4Prim.g >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][1]) {
                    sOcarinaNoteF4Prim.g -= gd;
                } else {
                    sOcarinaNoteF4Prim.g += gd;
                }
                if (sOcarinaNoteF4Prim.b >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][2]) {
                    sOcarinaNoteF4Prim.b -= bd;
                } else {
                    sOcarinaNoteF4Prim.b += bd;
                }

                if (sOcarinaNoteA4Prim.r >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][0]) {
                    sOcarinaNoteA4Prim.r -= rr;
                } else {
                    sOcarinaNoteA4Prim.r += rr;
                }
                if (sOcarinaNoteA4Prim.g >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][1]) {
                    sOcarinaNoteA4Prim.g -= gr;
                } else {
                    sOcarinaNoteA4Prim.g += gr;
                }
                if (sOcarinaNoteA4Prim.b >= sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][2]) {
                    sOcarinaNoteA4Prim.b -= br;
                } else {
                    sOcarinaNoteA4Prim.b += br;
                }

                r = ABS(sOcarinaNoteCBtnEnv.r - sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][0]) /
                    sOcarinaNoteFlashTimer;
                g = ABS(sOcarinaNoteCBtnEnv.g - sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][1]) /
                    sOcarinaNoteFlashTimer;
                b = ABS(sOcarinaNoteCBtnEnv.b - sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][2]) /
                    sOcarinaNoteFlashTimer;

                if (sOcarinaNoteCBtnEnv.r >= sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][0]) {
                    sOcarinaNoteCBtnEnv.r -= r;
                } else {
                    sOcarinaNoteCBtnEnv.r += r;
                }
                if (sOcarinaNoteCBtnEnv.g >= sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][1]) {
                    sOcarinaNoteCBtnEnv.g -= g;
                } else {
                    sOcarinaNoteCBtnEnv.g += g;
                }
                if (sOcarinaNoteCBtnEnv.b >= sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][2]) {
                    sOcarinaNoteCBtnEnv.b -= b;
                } else {
                    sOcarinaNoteCBtnEnv.b += b;
                }

                sOcarinaNoteFlashTimer--;
                if (sOcarinaNoteFlashTimer == 0) {
                    sOcarinaNoteD4Prim.r = sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][0];
                    sOcarinaNoteD4Prim.g = sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][1];
                    sOcarinaNoteD4Prim.b = sOcarinaNoteAPrimColors[sOcarinaNoteFlashColorIdx][2];
                    sOcarinaNoteABtnEnv.r = sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][0];
                    sOcarinaNoteABtnEnv.g = sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][1];
                    sOcarinaNoteABtnEnv.b = sOcarinaNoteAEnvColors[sOcarinaNoteFlashColorIdx][2];
                    sOcarinaNoteCBtnPrim.r = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][0];
                    sOcarinaNoteCBtnPrim.g = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][1];
                    sOcarinaNoteCBtnPrim.b = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx][2];
                    sOcarinaNoteD5Prim.r = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][0];
                    sOcarinaNoteD5Prim.g = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][1];
                    sOcarinaNoteD5Prim.b = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+5][2];
                    sOcarinaNoteB4Prim.r = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][0];
                    sOcarinaNoteB4Prim.g = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][1];
                    sOcarinaNoteB4Prim.b = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+2][2];
                    sOcarinaNoteF4Prim.r = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][0];
                    sOcarinaNoteF4Prim.g = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][1];
                    sOcarinaNoteF4Prim.b = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+3][2];
                    sOcarinaNoteA4Prim.r = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][0];
                    sOcarinaNoteA4Prim.g = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][1];
                    sOcarinaNoteA4Prim.b = sOcarinaNoteCPrimColors[sOcarinaNoteFlashColorIdx+4][2];
                    sOcarinaNoteCBtnEnv.r = sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][0];
                    sOcarinaNoteCBtnEnv.g = sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][1];
                    sOcarinaNoteCBtnEnv.b = sOcarinaNoteCEnvColors[sOcarinaNoteFlashColorIdx][2];
                    sOcarinaNoteFlashTimer = 3;
                    sOcarinaNoteFlashColorIdx ^= 1;
                }

                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    Audio_OcaSetInstrument(0);
                    if (msgCtx->msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) {
                        // "Correct Example Performance"
                        osSyncPrintf("正解模範演奏=%x\n", msgCtx->lastPlayedSong);
                        Message_ContinueTextbox(play, 0x86F); // Ocarina
                        msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                        msgCtx->textBoxType = TEXTBOX_TYPE_OCARINA;
                        msgCtx->stateTimer = 1;
                    } else if (msgCtx->msgMode == MSGMODE_SONG_PLAYBACK_SUCCESS) {
                        if (msgCtx->lastPlayedSong >= OCARINA_SONG_SARIAS) {
                            Message_ContinueTextbox(play, 0x86F); // Ocarina
                            msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                            msgCtx->textBoxType = TEXTBOX_TYPE_OCARINA;
                            msgCtx->stateTimer = 1;
                        } else {
                            Message_CloseTextbox(play);
                            play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                        }
                    } else {
                        Message_CloseTextbox(play);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_03;
                    }
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_OCARINA_FAIL:
            case MSGMODE_SONG_PLAYBACK_FAIL:
                Message_DrawText(play, &gfx);
            case MSGMODE_OCARINA_FAIL_NO_TEXT:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    R_OCARINA_NOTES_YPOS_OFFSET = 1;
                    if (msgCtx->msgMode == MSGMODE_SONG_PLAYBACK_FAIL) {
                        // "kokokokokoko"
                        osSyncPrintf("ここここここ\n");
                        Message_ContinueTextbox(play, 0x88B); // red X background
                        Message_Decode(play);
                        msgCtx->msgMode = MSGMODE_SONG_PLAYBACK_NOTES_DROP;
                    } else {
                        msgCtx->msgMode = MSGMODE_OCARINA_NOTES_DROP;
                    }
                    // "Cancel"
                    osSyncPrintf("キャンセル\n");
                }
                break;
            case MSGMODE_OCARINA_NOTES_DROP:
            case MSGMODE_SONG_PLAYBACK_NOTES_DROP:
                for (i = 0; i < 5; i++) {
                    R_OCARINA_NOTES_YPOS(i) += R_OCARINA_NOTES_YPOS_OFFSET;
                }
                R_OCARINA_NOTES_YPOS_OFFSET += R_OCARINA_NOTES_YPOS_OFFSET;
                if (R_OCARINA_NOTES_YPOS_OFFSET >= 550) {
                    sOcarinaNoteBuf[0] = OCARINA_NOTE_INVALID;
                    sOcarinaNotesAlphaValues[0] = sOcarinaNotesAlphaValues[1] = sOcarinaNotesAlphaValues[2] =
                        sOcarinaNotesAlphaValues[3] = sOcarinaNotesAlphaValues[4] = sOcarinaNotesAlphaValues[5] =
                            sOcarinaNotesAlphaValues[6] = sOcarinaNotesAlphaValues[7] = sOcarinaNotesAlphaValues[8] = 0;
                    if (msgCtx->msgMode == MSGMODE_SONG_PLAYBACK_NOTES_DROP) {
                        msgCtx->msgMode = MSGMODE_OCARINA_AWAIT_INPUT;
                    } else {
                        msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
                    }
                }
                break;
            case MSGMODE_SONG_PLAYED:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    Audio_OcaSetInstrument(0);
                    osSyncPrintf(VT_FGCOL(GREEN));
                    osSyncPrintf("Na_StopOcarinaMode();\n");
                    osSyncPrintf("Na_StopOcarinaMode();\n");
                    osSyncPrintf("Na_StopOcarinaMode();\n");
                    osSyncPrintf(VT_RST);
                    Message_Decode(play);
                    msgCtx->msgMode = MSGMODE_SETUP_DISPLAY_SONG_PLAYED;
                    msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                    msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                    Message_ResetOcarinaNoteState();
                    if (msgCtx->lastPlayedSong >= OCARINA_SONG_SARIAS &&
                        msgCtx->lastPlayedSong < OCARINA_SONG_MEMORY_GAME) {
                        Actor_Spawn(&play->actorCtx, play,
                                    sOcarinaEffectActorIds[msgCtx->lastPlayedSong - OCARINA_SONG_SARIAS],
                                    player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0,
                                    0, 0, sOcarinaEffectActorParams[msgCtx->lastPlayedSong - OCARINA_SONG_SARIAS]);
                    }
                }
                break;
            case MSGMODE_SETUP_DISPLAY_SONG_PLAYED:
                if (CVar_GetS32("gFastOcarinaPlayback", 0) == 0 ||
                    play->msgCtx.lastPlayedSong == OCARINA_SONG_TIME ||
                    play->msgCtx.lastPlayedSong == OCARINA_SONG_STORMS ||
                    play->msgCtx.lastPlayedSong == OCARINA_SONG_SUNS) {
                    Message_DrawText(play, &gfx);
                    Audio_OcaSetInstrument(1);
                    Audio_OcaSetInstrument(1);
                    Audio_OcaSetSongPlayback(msgCtx->lastPlayedSong + 1, 1);
                } else {
                    Audio_OcaSetInstrument(1);
                    Audio_OcaSetInstrument(1);
                }
                if (msgCtx->lastPlayedSong != OCARINA_SONG_SCARECROW) {
                    Audio_PlayFanfare(sOcarinaSongFanfares[msgCtx->lastPlayedSong]);
                    Audio_SetSoundBanksMute(0x20);
                }
                play->msgCtx.ocarinaMode = OCARINA_MODE_01;
                if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) {
                    msgCtx->ocarinaAction = OCARINA_ACTION_FREE_PLAY_DONE;
                }
                if (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOWARP) {
                    msgCtx->ocarinaAction = OCARINA_ACTION_CHECK_NOWARP_DONE;
                }
                sOcarinaNoteBufPos = 0;
                msgCtx->msgMode = MSGMODE_DISPLAY_SONG_PLAYED;
                break;
            case MSGMODE_SONG_DEMONSTRATION_SELECT_INSTRUMENT:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    // "ocarina_no=%d Song Chosen=%d"
                    osSyncPrintf("ocarina_no=%d  選曲=%d\n", msgCtx->ocarinaAction, 0x16);
                    if (msgCtx->ocarinaAction < OCARINA_ACTION_TEACH_SARIA) {
                        Audio_OcaSetInstrument(4);
                    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_TEACH_EPONA) {
                        Audio_OcaSetInstrument(2);
                    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_TEACH_LULLABY) {
                        Audio_OcaSetInstrument(3);
                    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_TEACH_STORMS) {
                        Audio_OcaSetInstrument(5);
                    } else {
                        Audio_OcaSetInstrument(1);
                    }
                    // "Example Performance"
                    osSyncPrintf("模範演奏=%x\n", msgCtx->ocarinaAction - OCARINA_ACTION_TEACH_MINUET);
                    Audio_OcaSetSongPlayback(msgCtx->ocarinaAction - OCARINA_ACTION_TEACH_MINUET + 1, 2);
                    sOcarinaNoteBufPos = 0;
                    msgCtx->msgMode = MSGMODE_SONG_DEMONSTRATION;
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_DISPLAY_SONG_PLAYED_TEXT_BEGIN:
                Message_ContinueTextbox(play, msgCtx->lastPlayedSong + 0x893); // You played [song name]
                Message_Decode(play);
                msgCtx->msgMode = MSGMODE_DISPLAY_SONG_PLAYED_TEXT;

                if (CVar_GetS32("gFastOcarinaPlayback", 0) == 0 || play->msgCtx.lastPlayedSong == OCARINA_SONG_TIME
                    || play->msgCtx.lastPlayedSong == OCARINA_SONG_STORMS ||
                    play->msgCtx.lastPlayedSong == OCARINA_SONG_SUNS) {
                    msgCtx->stateTimer = 20;
                } else {
                    msgCtx->stateTimer = 1;
                }

                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_DISPLAY_SONG_PLAYED_TEXT:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    msgCtx->msgMode = MSGMODE_SONG_PLAYED_ACT_BEGIN;
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_SONG_PLAYED_ACT_BEGIN:
                Audio_OcaSetInstrument(0);
                Message_ResetOcarinaNoteState();
                msgCtx->msgMode = MSGMODE_SONG_PLAYED_ACT;
                msgCtx->stateTimer = 2;
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_SONG_PLAYED_ACT:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    if (msgCtx->lastPlayedSong < OCARINA_SONG_SARIAS &&
                        (msgCtx->ocarinaAction < OCARINA_ACTION_PLAYBACK_MINUET ||
                         msgCtx->ocarinaAction >= OCARINA_ACTION_PLAYBACK_SARIA)) {
                        if (msgCtx->disableWarpSongs || (interfaceCtx->restrictions.warpSongs == 3 && !gSaveContext.n64ddFlag)) {
                            Message_StartTextbox(play, 0x88C, NULL); // "You can't warp here!"
                            play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                        } else if ((gSaveContext.eventInf[0] & 0xF) != 1) {
                            Message_StartTextbox(play, msgCtx->lastPlayedSong + 0x88D,
                                                 NULL); // "Warp to [place name]?"
                            play->msgCtx.ocarinaMode = OCARINA_MODE_01;
                        } else {
                            Message_CloseTextbox(play);
                        }
                    } else {
                        Message_CloseTextbox(play);
                        if (msgCtx->lastPlayedSong == OCARINA_SONG_EPONAS) {
                            DREG(53) = 1;
                        }
                        osSyncPrintf(VT_FGCOL(YELLOW));
                        osSyncPrintf("☆☆☆ocarina=%d   message->ocarina_no=%d  ", msgCtx->lastPlayedSong,
                                     msgCtx->ocarinaAction);
                        if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY_DONE) {
                            play->msgCtx.ocarinaMode = OCARINA_MODE_01;
                            if (msgCtx->lastPlayedSong == OCARINA_SONG_SCARECROW) {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_0B;
                            }
                        } else if (msgCtx->ocarinaAction >= OCARINA_ACTION_CHECK_MINUET) {
                            osSyncPrintf(VT_FGCOL(YELLOW));
                            osSyncPrintf("Ocarina_PC_Wind=%d(%d) ☆☆☆   ", OCARINA_ACTION_CHECK_MINUET,
                                         msgCtx->ocarinaAction - OCARINA_ACTION_CHECK_MINUET);
                            if (msgCtx->lastPlayedSong + OCARINA_ACTION_CHECK_MINUET == msgCtx->ocarinaAction) {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_03;
                            } else {
                                play->msgCtx.ocarinaMode = msgCtx->lastPlayedSong - 1;
                            }
                        } else {
                            osSyncPrintf(VT_FGCOL(GREEN));
                            osSyncPrintf("Ocarina_C_Wind=%d(%d) ☆☆☆   ", OCARINA_ACTION_PLAYBACK_MINUET,
                                         msgCtx->ocarinaAction - OCARINA_ACTION_PLAYBACK_MINUET);
                            if (msgCtx->lastPlayedSong + OCARINA_ACTION_PLAYBACK_MINUET == msgCtx->ocarinaAction) {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_03;
                            } else {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                            }
                        }
                        osSyncPrintf(VT_RST);
                        osSyncPrintf("→  OCARINA_MODE=%d\n", play->msgCtx.ocarinaMode);
                    }
                }
                break;
            case MSGMODE_DISPLAY_SONG_PLAYED:
            case MSGMODE_SONG_DEMONSTRATION:
                msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
                if (msgCtx->ocarinaStaff->state == 0) {
                    if (msgCtx->msgMode == MSGMODE_DISPLAY_SONG_PLAYED) {
                        msgCtx->msgMode = MSGMODE_DISPLAY_SONG_PLAYED_TEXT_BEGIN;
                    } else {
                        msgCtx->msgMode = MSGMODE_SONG_DEMONSTRATION_DONE;
                    }
                    osSyncPrintf("onpu_buff[%d]=%x\n", msgCtx->ocarinaStaff->pos,
                                 sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos]);
                } else {
                    if (sOcarinaNoteBufPos != 0 && msgCtx->ocarinaStaff->pos == 1) {
                        sOcarinaNoteBufPos = 0;
                    }
                    if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                        msgCtx->lastOcaNoteIdx = sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos - 1] =
                            msgCtx->ocarinaStaff->noteIdx;
                        sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos] = OCARINA_NOTE_INVALID;
                        sOcarinaNoteBufPos++;
                    }
                }
            case MSGMODE_SONG_DEMONSTRATION_DONE:
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_SONG_PLAYBACK:
                msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos - 1] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos] = OCARINA_NOTE_INVALID;
                    sOcarinaNoteBufPos++;
                }
                if (msgCtx->ocarinaStaff->state < OCARINA_SONG_MEMORY_GAME) {
                    osSyncPrintf("M_OCARINA20 : ocarina_no=%x    status=%x\n", msgCtx->ocarinaAction,
                                 msgCtx->ocarinaStaff->state);
                    msgCtx->lastPlayedSong = msgCtx->ocarinaStaff->state;
                    msgCtx->msgMode = MSGMODE_SONG_PLAYBACK_SUCCESS;

                    if (!gSaveContext.n64ddFlag) {
                        Item_Give(play, ITEM_SONG_MINUET + gOcarinaSongItemMap[msgCtx->ocarinaStaff->state]);
                    }

                    osSyncPrintf(VT_FGCOL(YELLOW));
                    // "z_message.c Song Acquired"
                    osSyncPrintf("z_message.c 取得メロディ＝%d\n", ITEM_SONG_MINUET + msgCtx->ocarinaStaff->state);
                    osSyncPrintf(VT_RST);
                    msgCtx->stateTimer = 20;
                    Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                } else if (msgCtx->ocarinaStaff->state == 0xFF) {
                    Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    msgCtx->stateTimer = 10;
                    msgCtx->msgMode = MSGMODE_SONG_PLAYBACK_FAIL;
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_OCARINA_AWAIT_INPUT:
                Message_DrawText(play, &gfx);
                if (Message_ShouldAdvance(play)) {
                    func_8010BD58(play, msgCtx->ocarinaAction);
                }
                break;
            case MSGMODE_SCARECROW_LONG_RECORDING_START:
                // "Scarecrow Recording Initialization"
                osSyncPrintf("案山子録音 初期化\n");
                Audio_OcaSetRecordingState(1);
                Audio_OcaSetInstrument(1);
                msgCtx->ocarinaStaff = Audio_OcaGetRecordingStaff();
                msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                sOcarinaNoteBufLen = 0;
                Message_ResetOcarinaNoteState();
                msgCtx->msgMode = MSGMODE_SCARECROW_LONG_RECORDING_ONGOING;
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_SCARECROW_LONG_RECORDING_ONGOING:
                msgCtx->ocarinaStaff = Audio_OcaGetRecordingStaff();
                osSyncPrintf("\nonpu_pt=%d, locate=%d", sOcarinaNoteBufPos, msgCtx->ocarinaStaff->pos);
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    if (sOcarinaNoteBufLen >= 8) {
                        for (noteBufPos = sOcarinaNoteBufLen - 8, i = 0; i < 8; i++, noteBufPos++) {
                            sOcarinaNoteBuf[noteBufPos] = sOcarinaNoteBuf[noteBufPos + 1];
                        }
                        sOcarinaNoteBufLen--;
                    }
                    // "Button Entered"
                    osSyncPrintf("    入力ボタン【%d】=%d", sOcarinaNoteBufLen, msgCtx->ocarinaStaff->noteIdx);
                    msgCtx->lastOcaNoteIdx = sOcarinaNoteBuf[sOcarinaNoteBufLen] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBufLen++;
                    sOcarinaNoteBuf[sOcarinaNoteBufLen] = OCARINA_NOTE_INVALID;
                    sOcarinaNoteBufPos++;
                    if (msgCtx->ocarinaStaff->pos == 8) {
                        sOcarinaNoteBufPos = 0;
                    }
                }
                if (msgCtx->ocarinaStaff->state == 0 || isB_Held) {
                    if (sOcarinaNoteBufLen != 0) {
                        // "Recording complete！！！！！！！！！"
                        osSyncPrintf("録音終了！！！！！！！！！  message->info->status=%d \n",
                                     msgCtx->ocarinaStaff->state);
                        gSaveContext.scarecrowCustomSongSet = true;
                    }
                    Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    osSyncPrintf("aaaaaaaaaaaaaa\n");
                    Audio_OcaSetRecordingState(0);
                    msgCtx->stateTimer = 10;
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    Message_CloseTextbox(play);
                    // "Recording complete！！！！！！！！！Recording Complete"
                    osSyncPrintf("録音終了！！！！！！！！！録音終了\n");
                    osSyncPrintf(VT_FGCOL(YELLOW));
                    osSyncPrintf("\n====================================================================\n");
                    memcpy(gSaveContext.scarecrowCustomSong, gScarecrowCustomSongPtr,
                            sizeof(gSaveContext.scarecrowCustomSong));
                    for (i = 0; i < ARRAY_COUNT(gSaveContext.scarecrowCustomSong); i++) {
                        osSyncPrintf("%d, ", gSaveContext.scarecrowCustomSong[i]);
                    }
                    osSyncPrintf(VT_RST);
                    osSyncPrintf("\n====================================================================\n");
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_SCARECROW_LONG_PLAYBACK:
            case MSGMODE_SCARECROW_PLAYBACK:
                msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    if (sOcarinaNoteBufLen >= 8) {
                        for (noteBufPos = sOcarinaNoteBufLen - 8, i = 0; i < 8; i++, noteBufPos++) {
                            sOcarinaNoteBuf[noteBufPos] = sOcarinaNoteBuf[noteBufPos + 1];
                        }
                        sOcarinaNoteBufLen--;
                    }
                    sOcarinaNoteBuf[sOcarinaNoteBufLen] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBufLen++;
                    sOcarinaNoteBuf[sOcarinaNoteBufLen] = OCARINA_NOTE_INVALID;
                    sOcarinaNoteBufPos++;
                    if (msgCtx->ocarinaStaff->pos == 8) {
                        sOcarinaNoteBufLen = sOcarinaNoteBufPos = 0;
                    }
                }
                osSyncPrintf("status=%d (%d)\n", msgCtx->ocarinaStaff->state, 0);
                if (msgCtx->stateTimer == 0) {
                    if (msgCtx->ocarinaStaff->state == 0) {
                        osSyncPrintf("bbbbbbbbbbb\n");
                        Audio_OcaSetInstrument(0);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_0F;
                        Message_CloseTextbox(play);
                    }
                } else {
                    msgCtx->stateTimer--;
                }
                break;
            case MSGMODE_SCARECROW_RECORDING_START:
                Audio_OcaSetRecordingState(2);
                Audio_OcaSetInstrument(1);
                msgCtx->msgMode = MSGMODE_SCARECROW_RECORDING_ONGOING;
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_SCARECROW_RECORDING_ONGOING:
                msgCtx->ocarinaStaff = Audio_OcaGetRecordingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    msgCtx->lastOcaNoteIdx = sOcarinaNoteBuf[sOcarinaNoteBufPos] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBufPos++;
                    sOcarinaNoteBuf[sOcarinaNoteBufPos] = OCARINA_NOTE_INVALID;
                }
                if (msgCtx->ocarinaStaff->state == 0) {
                    // "8 Note Recording ＯＫ！"
                    osSyncPrintf("８音録音ＯＫ！\n");
                    msgCtx->stateTimer = 20;
                    gSaveContext.scarecrowSpawnSongSet = true;
                    msgCtx->msgMode = MSGMODE_SCARECROW_RECORDING_DONE;
                    Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    osSyncPrintf(VT_FGCOL(YELLOW));
                    osSyncPrintf("\n====================================================================\n");
                    memcpy(gSaveContext.scarecrowSpawnSong, gScarecrowSpawnSongPtr,
                            sizeof(gSaveContext.scarecrowSpawnSong));
                    for (i = 0; i < ARRAY_COUNT(gSaveContext.scarecrowSpawnSong); i++) {
                        osSyncPrintf("%d, ", gSaveContext.scarecrowSpawnSong[i]);
                    }
                    osSyncPrintf(VT_RST);
                    osSyncPrintf("\n====================================================================\n");
                } else if (msgCtx->ocarinaStaff->state == 0xFF || isB_Held) {
                    // "Played an existing song！！！"
                    osSyncPrintf("すでに存在する曲吹いた！！！ \n");
                    Audio_OcaSetRecordingState(0);
                    Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    Message_CloseTextbox(play);
                    msgCtx->msgMode = MSGMODE_SCARECROW_RECORDING_FAILED;
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_SCARECROW_RECORDING_FAILED:
                osSyncPrintf("cccccccccccc\n");
                Audio_OcaSetInstrument(0);
                Message_StartTextbox(play, 0x40AD, NULL); // Bonooru doesn't remember your song
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                break;
            case MSGMODE_MEMORY_GAME_START:
                Audio_OcaSetInstrument(1);
                Audio_OcaSetInstrument(6);
                Audio_OcaMemoryGameStart(gSaveContext.ocarinaGameRoundNum);
                msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
                msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                Message_ResetOcarinaNoteState();
                Audio_OcaSetSongPlayback(OCARINA_SONG_MEMORY_GAME + 1, 1);
                msgCtx->msgMode = MSGMODE_MEMORY_GAME_LEFT_SKULLKID_PLAYING;
                msgCtx->stateTimer = 2;
                break;
            case MSGMODE_MEMORY_GAME_LEFT_SKULLKID_PLAYING:
            case MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_PLAYING:
                Audio_PlaySoundGeneral(NA_SE_SY_METRONOME_LV - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos - 1] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos] = OCARINA_NOTE_INVALID;
                    sOcarinaNoteBufPos++;
                }
                if (msgCtx->stateTimer == 0) {
                    if (msgCtx->ocarinaStaff->state == 0) {
                        if (msgCtx->msgMode == MSGMODE_MEMORY_GAME_LEFT_SKULLKID_PLAYING) {
                            Audio_PlaySoundGeneral(NA_SE_SY_METRONOME, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        } else {
                            Audio_PlaySoundGeneral(NA_SE_SY_METRONOME_2, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }
                        msgCtx->msgMode++;
                    }
                } else {
                    msgCtx->stateTimer--;
                }
                break;
            case MSGMODE_MEMORY_GAME_LEFT_SKULLKID_WAIT:
            case MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_WAIT:
                msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos - 1] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos] = OCARINA_NOTE_INVALID;
                    sOcarinaNoteBufPos++;
                }
                break;
            case MSGMODE_MEMORY_GAME_PLAYER_PLAYING:
                Audio_PlaySoundGeneral(NA_SE_SY_METRONOME_LV - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos - 1] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos] = OCARINA_NOTE_INVALID;
                    sOcarinaNoteBufPos++;
                }
                if (msgCtx->ocarinaStaff->state == 0xFF) {
                    // "Musical round failed！！！！！！！！！"
                    osSyncPrintf("輪唱失敗！！！！！！！！！\n");
                    Audio_OcaSetInstrument(0);
                    Audio_PlaySoundGeneral(NA_SE_SY_OCARINA_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    msgCtx->stateTimer = 10;
                    play->msgCtx.ocarinaMode = OCARINA_MODE_03;
                } else if (msgCtx->ocarinaStaff->state == 0xD) {
                    // "Musical round succeeded！！！！！！！！！"
                    osSyncPrintf("輪唱成功！！！！！！！！！\n");
                    Audio_PlaySoundGeneral(NA_SE_SY_GET_ITEM, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    msgCtx->msgMode = MSGMODE_MEMORY_GAME_ROUND_SUCCESS;
                    msgCtx->stateTimer = 30;
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_MEMORY_GAME_ROUND_SUCCESS:
                msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos - 1] = msgCtx->ocarinaStaff->noteIdx;
                    sOcarinaNoteBuf[msgCtx->ocarinaStaff->pos] = OCARINA_NOTE_INVALID;
                    sOcarinaNoteBufPos++;
                }
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    if (Audio_OcaMemoryGameGenNote() != 1) {
                        Audio_PlaySoundGeneral(NA_SE_SY_METRONOME, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                        msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                        Message_ResetOcarinaNoteState();
                        msgCtx->msgMode = MSGMODE_MEMORY_GAME_START_NEXT_ROUND;
                    } else {
                        play->msgCtx.ocarinaMode = OCARINA_MODE_0F;
                    }
                }
                Message_DrawText(play, &gfx);
                break;
            case MSGMODE_MEMORY_GAME_START_NEXT_ROUND:
                if (!Audio_IsSfxPlaying(NA_SE_SY_METRONOME)) {
                    msgCtx->ocarinaStaff = Audio_OcaGetDisplayingStaff();
                    msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                    Message_ResetOcarinaNoteState();
                    Audio_OcaSetSongPlayback(OCARINA_SONG_MEMORY_GAME + 1, 1);
                }
                break;
            case MSGMODE_FROGS_START:
                Audio_OcaSetInstrument(1);
                msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                play->msgCtx.ocarinaMode = OCARINA_MODE_01;
                Message_ResetOcarinaNoteState();
                func_800ECC04(sOcarinaSongBitFlags + 0xC000);
                msgCtx->msgMode = MSGMODE_FROGS_PLAYING;
                break;
            case MSGMODE_FROGS_PLAYING:
                msgCtx->ocarinaStaff = Audio_OcaGetPlayingStaff();
                if (msgCtx->ocarinaStaff->pos && sOcarinaNoteBufPos == msgCtx->ocarinaStaff->pos - 1) {
                    msgCtx->lastOcaNoteIdx = msgCtx->ocarinaStaff->noteIdx;
                    msgCtx->ocarinaStaff->pos = sOcarinaNoteBufPos = 0;
                    Message_ResetOcarinaNoteState();
                    msgCtx->msgMode = MSGMODE_FROGS_WAITING;
                }
            case MSGMODE_FROGS_WAITING:
                break;
            case MSGMODE_TEXT_DONE:
                Message_DrawText(play, &gfx);

                switch (msgCtx->textboxEndType) {
                    case TEXTBOX_ENDTYPE_2_CHOICE:
                        Message_HandleChoiceSelection(play, 1);
                        Message_DrawTextboxIcon(play, &gfx, msgCtx->textPosX, msgCtx->textPosY);
                        break;
                    case TEXTBOX_ENDTYPE_3_CHOICE:
                        Message_HandleChoiceSelection(play, 2);
                        Message_DrawTextboxIcon(play, &gfx, msgCtx->textPosX, msgCtx->textPosY);
                        break;
                    case TEXTBOX_ENDTYPE_PERSISTENT:
                        if (msgCtx->textId >= 0x6D && msgCtx->textId < 0x73) {
                            msgCtx->stateTimer++;
                            if (msgCtx->stateTimer >= 31) {
                                msgCtx->stateTimer = 2;
                                msgCtx->msgMode = MSGMODE_TEXT_CLOSING;
                            }
                        }
                        break;
                    case TEXTBOX_ENDTYPE_EVENT:
                    default:
                        Message_DrawTextboxIcon(play, &gfx, R_TEXTBOX_END_XPOS, R_TEXTBOX_END_YPOS);
                    case TEXTBOX_ENDTYPE_FADING:
                        break;
                }
                break;
            case MSGMODE_TEXT_CLOSING:
            case MSGMODE_PAUSED:
                break;
            case MSGMODE_UNK_20:
            default:
                msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
                break;
        }

        if (msgCtx->msgMode >= MSGMODE_OCARINA_PLAYING && msgCtx->msgMode < MSGMODE_TEXT_AWAIT_NEXT &&
            msgCtx->ocarinaAction != OCARINA_ACTION_FREE_PLAY && msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOWARP) {
            func_8009457C(&gfx);

            gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                              ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            if (msgCtx->msgMode == MSGMODE_SONG_PLAYBACK) {
                g = msgCtx->ocarinaAction - OCARINA_ACTION_PLAYBACK_MINUET;
                r = gOcarinaSongNotes[g].len;
                for (notePosX = R_OCARINA_NOTES_XPOS, i = 0; i < r; i++, notePosX += R_OCARINA_NOTES_XPOS_OFFSET) {
                    gDPPipeSync(gfx++);
                    gDPSetPrimColor(gfx++, 0, 0, 150, 150, 150, 150);
                    gDPSetEnvColor(gfx++, 10, 10, 10, 0);

                    gDPLoadTextureBlock(gfx++, sOcarinaNoteTextures[gOcarinaSongNotes[g].notesIdx[i]], G_IM_FMT_IA,
                                        G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSPTextureRectangle(
                        gfx++, notePosX << 2, R_OCARINA_NOTES_YPOS(gOcarinaSongNotes[g].notesIdx[i]) << 2,
                        (notePosX + 16) << 2, (R_OCARINA_NOTES_YPOS(gOcarinaSongNotes[g].notesIdx[i]) + 16) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                }
            }

            if (msgCtx->msgMode != MSGMODE_SCARECROW_LONG_RECORDING_START &&
                msgCtx->msgMode != MSGMODE_MEMORY_GAME_START) {
                for (notePosX = R_OCARINA_NOTES_XPOS, i = 0; i < 8; i++, notePosX += R_OCARINA_NOTES_XPOS_OFFSET) {
                    if (sOcarinaNoteBuf[i] == OCARINA_NOTE_INVALID) {
                        break;
                    }

                    if (sOcarinaNotesAlphaValues[i] != 255) {
                        sOcarinaNotesAlphaValues[i] += VREG(50);
                        if (sOcarinaNotesAlphaValues[i] >= 255) {
                            sOcarinaNotesAlphaValues[i] = 255;
                        }
                    }

                    gDPPipeSync(gfx++);

                    // Since I don't know what exactly these Env vars are used for, I elected keep their usage
                    // consistent with the note played, rather than having AEnv be used for whatever note A happens to
                    // play at the moment and CEnv for everything else, even with custom controls enabled.
                    if (sOcarinaNoteBuf[i] == OCARINA_NOTE_D4) {
                        gDPSetPrimColor(gfx++, 0, 0, sOcarinaNoteD4Prim.r, sOcarinaNoteD4Prim.g, sOcarinaNoteD4Prim.b, sOcarinaNotesAlphaValues[i]);
                        gDPSetEnvColor(gfx++, sOcarinaNoteABtnEnv.r, sOcarinaNoteABtnEnv.g, sOcarinaNoteABtnEnv.b, 0);
                    } else {
                        if (sOcarinaNoteBuf[i] == OCARINA_NOTE_D5) {
                            gDPSetPrimColor(gfx++, 0, 0, sOcarinaNoteD5Prim.r, sOcarinaNoteD5Prim.g, sOcarinaNoteD5Prim.b, sOcarinaNotesAlphaValues[i]);
                        } else if (sOcarinaNoteBuf[i] == OCARINA_NOTE_B4) {
                            gDPSetPrimColor(gfx++, 0, 0, sOcarinaNoteB4Prim.r, sOcarinaNoteB4Prim.g, sOcarinaNoteB4Prim.b, sOcarinaNotesAlphaValues[i]);
                        } else if (sOcarinaNoteBuf[i] == OCARINA_NOTE_A4) {
                            gDPSetPrimColor(gfx++, 0, 0, sOcarinaNoteA4Prim.r, sOcarinaNoteA4Prim.g, sOcarinaNoteA4Prim.b, sOcarinaNotesAlphaValues[i]);
                        } else if (sOcarinaNoteBuf[i] == OCARINA_NOTE_F4) {
                            gDPSetPrimColor(gfx++, 0, 0, sOcarinaNoteF4Prim.r, sOcarinaNoteF4Prim.g, sOcarinaNoteF4Prim.b, sOcarinaNotesAlphaValues[i]);
                        }
                        gDPSetEnvColor(gfx++, sOcarinaNoteCBtnEnv.r, sOcarinaNoteCBtnEnv.g, sOcarinaNoteCBtnEnv.b, 0);
                    }

                    gDPLoadTextureBlock(gfx++, sOcarinaNoteTextures[sOcarinaNoteBuf[i]], G_IM_FMT_IA, G_IM_SIZ_8b, 16,
                                        16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSPTextureRectangle(gfx++, notePosX << 2, R_OCARINA_NOTES_YPOS(sOcarinaNoteBuf[i]) << 2,
                                        (notePosX + 16) << 2, (R_OCARINA_NOTES_YPOS(sOcarinaNoteBuf[i]) + 16) << 2,
                                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                }
            }
        }
    }
    *p = gfx;
}

/**
 * If the s16 variable pointed to by `var` changes in value, a black bar and white box
 * are briefly drawn onto the screen. It can only watch one variable per build due to
 * the last value being saved in a static variable.
 */
void Message_DrawDebugVariableChanged(s16* var, GraphicsContext* gfxCtx) {
    if (!CVar_GetS32("gDebugEnabled", 0)) { return; }

    static s16 sVarLastValue = 0;
    static s16 sFillTimer = 0;
    s32 pad;

    OPEN_DISPS(gfxCtx);

    if (sVarLastValue != *var) {
        sVarLastValue = *var;
        sFillTimer = 30;
    }
    if (sFillTimer != 0) {
        sFillTimer--;
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCycleType(POLY_OPA_DISP++, G_CYC_FILL);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_NOOP, G_RM_NOOP2);
        gDPSetFillColor(POLY_OPA_DISP++, GPACK_RGBA5551(0, 0, 0, 1) << 0x10 | GPACK_RGBA5551(0, 0, 0, 1));
        gDPFillRectangle(POLY_OPA_DISP++, 0, 110, SCREEN_WIDTH - 1, 150); // 40x319 black bar
        gDPPipeSync(POLY_OPA_DISP++);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCycleType(POLY_OPA_DISP++, G_CYC_FILL);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_NOOP, G_RM_NOOP2);
        gDPSetFillColor(POLY_OPA_DISP++, GPACK_RGBA5551(255, 255, 255, 1) << 0x10 | GPACK_RGBA5551(255, 255, 255, 1));
        gDPFillRectangle(POLY_OPA_DISP++, 40, 120, 60, 140); // 20x20 white box
        gDPPipeSync(POLY_OPA_DISP++);
    }
    CLOSE_DISPS(gfxCtx);
}

void Message_DrawDebugText(PlayState* play, Gfx** p) {
    s32 pad;
    GfxPrint printer;
    s32 pad1;

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, *p);
    GfxPrint_SetPos(&printer, 6, 26);
    GfxPrint_SetColor(&printer, 255, 60, 0, 255);
    GfxPrint_Printf(&printer, "%s", "MESSAGE");
    GfxPrint_SetPos(&printer, 14, 26);
    GfxPrint_Printf(&printer, "%s", "=");
    GfxPrint_SetPos(&printer, 16, 26);
    GfxPrint_Printf(&printer, "%x", play->msgCtx.textId);
    *p = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
}

void Message_Draw(PlayState* play) {
    Gfx* plusOne;
    Gfx* polyOpaP;
    s16 watchVar;

    OPEN_DISPS(play->state.gfxCtx);

    watchVar = gSaveContext.scarecrowCustomSongSet;
    Message_DrawDebugVariableChanged(&watchVar, play->state.gfxCtx);
    if (BREG(0) != 0 && play->msgCtx.textId != 0) {
        plusOne = Graph_GfxPlusOne(polyOpaP = POLY_OPA_DISP);
        gSPDisplayList(OVERLAY_DISP++, plusOne);
        Message_DrawDebugText(play, &plusOne);
        gSPEndDisplayList(plusOne++);
        Graph_BranchDlist(polyOpaP, plusOne);
        POLY_OPA_DISP = plusOne;
    }
    plusOne = Graph_GfxPlusOne(polyOpaP = POLY_OPA_DISP);
    gSPDisplayList(OVERLAY_DISP++, plusOne);
    Message_DrawMain(play, &plusOne);
    gSPEndDisplayList(plusOne++);
    Graph_BranchDlist(polyOpaP, plusOne);
    POLY_OPA_DISP = plusOne;
    CLOSE_DISPS(play->state.gfxCtx);
}

void Message_Update(PlayState* play) {
    static s16 sTextboxXPositions[] = {
        34, 34, 34, 34, 34, 34,
    };
    static s16 sTextboxLowerYPositions[] = {
        142, 142, 142, 142, 174, 142,
    };
    static s16 sTextboxUpperYPositions[] = {
        38, 38, 38, 38, 174, 38,
    };
    static s16 sTextboxMidYPositions[] = {
        90, 90, 90, 90, 174, 90,
    };
    static s16 sTextboxEndIconYOffset[] = {
        59, 59, 59, 59, 34, 59,
    };
    static s16 D_80153D3C[] = {
        // additional unreferenced data
        0x0400, 0x0400, 0x0200, 0x0000, 0x1038, 0x0008, 0x200A, 0x088B, 0x0007, 0x0009, 0x000A, 0x107E, 0x2008, 0x2007,
        0x0015, 0x0016, 0x0017, 0x0003, 0x0000, 0x270B, 0x00C8, 0x012C, 0x012D, 0xFFDA, 0x0014, 0x0016, 0x0014, 0x0016,
    };
    static u8 D_80153D74 = 0;
    static u16 D_80153D78 = 0;
    MessageContext* msgCtx = &play->msgCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    Input* input = &play->state.input[0];
    s16 var;
    s16 focusScreenPosX;
    s16 averageY = 0;
    s16 playerFocusScreenPosY;
    s16 actorFocusScreenPosY;

    if (BREG(0) != 0) {
        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN) && CHECK_BTN_ALL(input->cur.button, BTN_L)) {
            osSyncPrintf("msgno=%d\n", D_80153D78);
            Message_StartTextbox(play, R_MESSAGE_DEBUGGER_TEXTID, NULL);
            D_80153D78 = (D_80153D78 + 1) % 10;
        }
        if (R_MESSAGE_DEBUGGER_SELECT != 0) {
            while (R_MESSAGE_DEBUGGER_TEXTID != 0x8000) {
                MessageTableEntry* entry = &sNesMessageEntryTablePtr[0];

                while (entry->textId != 0xFFFD) {
                    if (entry->textId == R_MESSAGE_DEBUGGER_TEXTID) {
                        // "The message was found! !! !!"
                        osSyncPrintf(" メッセージが,見つかった！！！ = %x\n", R_MESSAGE_DEBUGGER_TEXTID);
                        Message_StartTextbox(play, R_MESSAGE_DEBUGGER_TEXTID, NULL);
                        R_MESSAGE_DEBUGGER_TEXTID++;
                        R_MESSAGE_DEBUGGER_SELECT = 0;
                        return;
                    }
                    entry++;
                }
                R_MESSAGE_DEBUGGER_TEXTID++;
            }
        }
    }

    if (msgCtx->msgLength == 0) {
        return;
    }

    bool isB_Held = CVar_GetS32("gSkipText", 0) != 0 ? CHECK_BTN_ALL(input->cur.button, BTN_B) && !sTextboxSkipped
                                                     : CHECK_BTN_ALL(input->press.button, BTN_B);

    switch (msgCtx->msgMode) {
        case MSGMODE_TEXT_START:
            D_8014B2F4++;

            var = false;
            if (YREG(15) == 0x40) {
                if (D_8014B2F4 >= 4) {
                    var = true;
                }
            } else if (YREG(15) != 0 || play->sceneNum == SCENE_HAIRAL_NIWA) {
                var = true;
            } else if (D_8014B2F4 >= 4 || msgCtx->talkActor == NULL) {
                var = true;
            }

            if (var) {
                if (msgCtx->talkActor != NULL) {
                    Actor_GetScreenPos(play, &GET_PLAYER(play)->actor, &focusScreenPosX,
                                       &playerFocusScreenPosY);
                    Actor_GetScreenPos(play, msgCtx->talkActor, &focusScreenPosX, &actorFocusScreenPosY);

                    if (playerFocusScreenPosY >= actorFocusScreenPosY) {
                        averageY = ((playerFocusScreenPosY - actorFocusScreenPosY) / 2) + actorFocusScreenPosY;
                    } else {
                        averageY = ((actorFocusScreenPosY - playerFocusScreenPosY) / 2) + playerFocusScreenPosY;
                    }
                    osSyncPrintf("dxpos=%d   dypos=%d  dypos1  dypos2=%d\n", focusScreenPosX, averageY,
                                 playerFocusScreenPosY, actorFocusScreenPosY);
                } else {
                    R_TEXTBOX_X = R_TEXTBOX_X_TARGET;
                    R_TEXTBOX_Y = R_TEXTBOX_Y_TARGET;
                }

                var = msgCtx->textBoxType;

                if (!msgCtx->textBoxPos) { // variable position
                    if (YREG(15) != 0 || play->sceneNum == SCENE_HAIRAL_NIWA) {
                        if (averageY < XREG(92)) {
                            R_TEXTBOX_Y_TARGET = sTextboxLowerYPositions[var];
                        } else {
                            R_TEXTBOX_Y_TARGET = sTextboxUpperYPositions[var];
                        }
                    } else if (play->sceneNum == SCENE_MARKET_DAY || play->sceneNum == SCENE_MARKET_NIGHT ||
                               play->sceneNum == SCENE_MARKET_RUINS) {
                        if (averageY < XREG(93)) {
                            R_TEXTBOX_Y_TARGET = sTextboxLowerYPositions[var];
                        } else {
                            R_TEXTBOX_Y_TARGET = sTextboxUpperYPositions[var];
                        }
                    } else {
                        if (averageY < XREG(94)) {
                            R_TEXTBOX_Y_TARGET = sTextboxLowerYPositions[var];
                        } else {
                            R_TEXTBOX_Y_TARGET = sTextboxUpperYPositions[var];
                        }
                    }
                } else {
                    if (msgCtx->textBoxPos == TEXTBOX_POS_TOP) {
                        R_TEXTBOX_Y_TARGET = sTextboxUpperYPositions[var];
                    } else if (msgCtx->textBoxPos == TEXTBOX_POS_BOTTOM) {
                        R_TEXTBOX_Y_TARGET = sTextboxLowerYPositions[var];
                    } else {
                        R_TEXTBOX_Y_TARGET = sTextboxMidYPositions[var];
                    }
                }

                R_TEXTBOX_X_TARGET = sTextboxXPositions[var];
                R_TEXTBOX_END_YPOS = sTextboxEndIconYOffset[var] + R_TEXTBOX_Y_TARGET;
                R_TEXT_CHOICE_YPOS(0) = R_TEXTBOX_Y_TARGET + 20;
                R_TEXT_CHOICE_YPOS(1) = R_TEXTBOX_Y_TARGET + 32;
                R_TEXT_CHOICE_YPOS(2) = R_TEXTBOX_Y_TARGET + 44;
                osSyncPrintf("message->msg_disp_type=%x\n", msgCtx->textBoxProperties & 0xF0);
                if (msgCtx->textBoxType == TEXTBOX_TYPE_NONE_BOTTOM ||
                    msgCtx->textBoxType == TEXTBOX_TYPE_NONE_NO_SHADOW) {
                    msgCtx->msgMode = MSGMODE_TEXT_STARTING;
                    R_TEXTBOX_X = R_TEXTBOX_X_TARGET;
                    R_TEXTBOX_Y = R_TEXTBOX_Y_TARGET;
                    R_TEXTBOX_WIDTH = 256;
                    R_TEXTBOX_HEIGHT = 64;
                    R_TEXTBOX_TEXWIDTH = 512;
                    R_TEXTBOX_TEXHEIGHT = 512;
                } else {
                    Message_GrowTextbox(msgCtx);
                    Audio_PlaySoundIfNotInCutscene(0);
                    msgCtx->stateTimer = 0;
                    msgCtx->msgMode = MSGMODE_TEXT_BOX_GROWING;
                }
            }
            break;
        case MSGMODE_TEXT_BOX_GROWING:
            Message_GrowTextbox(msgCtx);
            break;
        case MSGMODE_TEXT_STARTING:
            msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
            if (YREG(31) == 0) {
                Interface_SetDoAction(play, DO_ACTION_NEXT);
            }
            break;
        case MSGMODE_TEXT_NEXT_MSG:
            Message_Decode(play);
            if (sTextFade) {
                Interface_ChangeAlpha(1);
            }
            if (D_80153D74 != 0) {
                msgCtx->textDrawPos = msgCtx->decodedTextLen;
                D_80153D74 = 0;
            }
            break;
        case MSGMODE_TEXT_CONTINUING:
            msgCtx->stateTimer--;
            if (msgCtx->stateTimer == 0) {
                Message_Decode(play);
            }
            break;
        case MSGMODE_TEXT_DISPLAYING:
            if (msgCtx->textBoxType != TEXTBOX_TYPE_NONE_BOTTOM && YREG(31) == 0 && isB_Held &&
                !msgCtx->textUnskippable) {
                sTextboxSkipped = true;
                msgCtx->textDrawPos = msgCtx->decodedTextLen;
            }
            break;
        case MSGMODE_TEXT_AWAIT_INPUT:
            if (YREG(31) == 0 && Message_ShouldAdvance(play)) {
                msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
                msgCtx->textDrawPos++;
            }
            break;
        case MSGMODE_TEXT_DELAYED_BREAK:
            msgCtx->stateTimer--;
            if (msgCtx->stateTimer == 0) {
                msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
            }
            break;
        case MSGMODE_TEXT_AWAIT_NEXT:
            if (Message_ShouldAdvance(play)) {
                msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
                msgCtx->textUnskippable = false;
                msgCtx->msgBufPos++;
            }
            break;
        case MSGMODE_TEXT_DONE:
            if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADING) {
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    Message_CloseTextbox(play);
                }
            } else if (msgCtx->textboxEndType != TEXTBOX_ENDTYPE_PERSISTENT &&
                       msgCtx->textboxEndType != TEXTBOX_ENDTYPE_EVENT && YREG(31) == 0) {
                if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_2_CHOICE &&
                    play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
                    if (Message_ShouldAdvance(play)) {
                        osSyncPrintf("OCARINA_MODE=%d -> ", play->msgCtx.ocarinaMode);
                        play->msgCtx.ocarinaMode = (msgCtx->choiceIndex == 0) ? OCARINA_MODE_02 : OCARINA_MODE_04;
                        osSyncPrintf("InRaceSeq=%d(%d) OCARINA_MODE=%d  -->  ", gSaveContext.eventInf[0] & 0xF, 1,
                                     play->msgCtx.ocarinaMode);
                        Message_CloseTextbox(play);
                        osSyncPrintf("OCARINA_MODE=%d\n", play->msgCtx.ocarinaMode);
                    }
                } else if (Message_ShouldAdvanceSilent(play)) {
                    osSyncPrintf("select=%d\n", msgCtx->textboxEndType);
                    if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_HAS_NEXT) {
                        Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_PASS, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        Message_ContinueTextbox(play, sNextTextId);
                    } else {
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        Message_CloseTextbox(play);
                    }
                }
            }
            break;
        case MSGMODE_TEXT_CLOSING:
            msgCtx->stateTimer--;
            if (msgCtx->stateTimer != 0) {
                break;
            }
            if ((msgCtx->textId >= 0xC2 && msgCtx->textId < 0xC7) ||
                (msgCtx->textId >= 0xFA && msgCtx->textId < 0xFE)) {
                gSaveContext.healthAccumulator = 0x140; // Refill 20 hearts
            }
            if (msgCtx->textId == 0x301F || msgCtx->textId == 0xA || msgCtx->textId == 0xC || msgCtx->textId == 0xCF ||
                msgCtx->textId == 0x21C || msgCtx->textId == 9 || msgCtx->textId == 0x4078 ||
                msgCtx->textId == 0x2015 || msgCtx->textId == 0x3040) {
                gSaveContext.unk_13EE = 0x32;
            }
            if (play->csCtx.state == 0) {
                osSyncPrintf(VT_FGCOL(GREEN));
                osSyncPrintf("day_time=%x  active_camera=%d  ", gSaveContext.cutsceneIndex, play->activeCamera);

                if (msgCtx->textId != 0x2061 && msgCtx->textId != 0x2025 && msgCtx->textId != 0x208C &&
                    ((msgCtx->textId < 0x88D || msgCtx->textId >= 0x893) || msgCtx->choiceIndex != 0) &&
                    (msgCtx->textId != 0x3055 && gSaveContext.cutsceneIndex < 0xFFF0)) {
                    osSyncPrintf("=== day_time=%x ", ((void)0, gSaveContext.cutsceneIndex));
                    if (play->activeCamera == MAIN_CAM) {
                        if (gSaveContext.unk_13EE == 0 || gSaveContext.unk_13EE == 1 || gSaveContext.unk_13EE == 2) {
                            gSaveContext.unk_13EE = 0x32;
                        }
                        gSaveContext.unk_13EA = 0;
                        Interface_ChangeAlpha(gSaveContext.unk_13EE);
                    }
                }
            }
            osSyncPrintf(VT_RST);
            msgCtx->msgLength = 0;
            msgCtx->msgMode = MSGMODE_NONE;
            interfaceCtx->unk_1FA = interfaceCtx->unk_1FC = 0;
            msgCtx->textId = msgCtx->stateTimer = 0;

            if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_PERSISTENT) {
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
                play->msgCtx.ocarinaMode = OCARINA_MODE_02;
            } else {
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
            }
            if ((s32)(gSaveContext.inventory.questItems & 0xF0000000) == 0x40000000) {
                gSaveContext.inventory.questItems ^= 0x40000000;
                gSaveContext.healthCapacity += 0x10;
                gSaveContext.health += 0x10;
            }
            if (msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOWARP_DONE) {
                if (sLastPlayedSong == OCARINA_SONG_SARIAS) {
                    //! @bug The last played song is not unset often, and if something interrupts the message system
                    //       before it reaches this point after playing Saria's song, the song will be "stored".
                    //       Later, if the ocarina has not been played and another textbox is closed, this handling
                    //       for Saria's song will be carried out.
                    player->naviTextId = -0xE0;
                    player->naviActor->flags |= 0x10000;
                }
                if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY_DONE &&
                    (play->msgCtx.ocarinaMode == OCARINA_MODE_01 ||
                     play->msgCtx.ocarinaMode == OCARINA_MODE_0B)) {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    if (msgCtx->unk_E3F2 == OCARINA_SONG_SUNS) {
                        play->msgCtx.ocarinaMode = OCARINA_MODE_01;
                    }
                }
            }
            sLastPlayedSong = 0xFF;
            osSyncPrintf("OCARINA_MODE=%d   chk_ocarina_no=%d\n", play->msgCtx.ocarinaMode, msgCtx->unk_E3F2);
            break;
        case MSGMODE_PAUSED:
            break;
        default:
            msgCtx->lastOcaNoteIdx = OCARINA_NOTE_INVALID;
            break;
    }
}

void Message_SetTables(void) {
    OTRMessage_Init();

    // OTRTODO
    //sNesMessageEntryTablePtr = sNesMessageEntryTable;
    //sGerMessageEntryTablePtr = sGerMessageEntryTable;
    //sFraMessageEntryTablePtr = sFraMessageEntryTable;
    //sStaffMessageEntryTablePtr = sStaffMessageEntryTable;
}

// Appears to be file padding
UNK_TYPE D_80153D7C = 0x00000000;

// This should be part of z_game_over.c, but cannot be moved there as the entire
// late_rodata section of this file is in the way
s16 gGameOverTimer = 0;

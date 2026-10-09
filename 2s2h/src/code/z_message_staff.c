#include "global.h"
#include "message_data_fmt_staff.h"
#include "message_data_static.h"
#include "assets/interface/message_texture_static/message_texture_static.h"

void Message_FindCreditsMessage(PlayState* play, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    MessageTableEntry* msgEntry = msgCtx->messageTableCredits;
    const char* segment = msgEntry->segment;
    const char* foundSegment;
    const char* nextSegment;

    while (msgEntry->textId != 0xFFFF) {
        if (msgEntry->textId == textId) {
            // 2S2H [Port] Just assign the msgEntry, we don't need to change the messageStart and messageEnd
            font->messageStart = msgEntry;
            font->charBuf[font->unk_11D88][0] = msgEntry->typePos;
            break;
        }
        msgEntry++;
    }
}

f32 sCreditsFontWidths[144] = {
    8.0f,  8.0f,  6.0f,  9.0f,  9.0f,  14.0f, 12.0f, 3.0f,  7.0f,  7.0f,  7.0f,  9.0f,  4.0f,  6.0f,  4.0f,  9.0f,
    10.0f, 5.0f,  9.0f,  9.0f,  10.0f, 9.0f,  9.0f,  9.0f,  9.0f,  9.0f,  6.0f,  6.0f,  9.0f,  11.0f, 9.0f,  11.0f,
    13.0f, 12.0f, 9.0f,  11.0f, 11.0f, 8.0f,  8.0f,  12.0f, 10.0f, 4.0f,  8.0f,  10.0f, 8.0f,  13.0f, 11.0f, 13.0f,
    9.0f,  13.0f, 10.0f, 10.0f, 9.0f,  10.0f, 11.0f, 15.0f, 11.0f, 10.0f, 10.0f, 7.0f,  10.0f, 7.0f,  10.0f, 9.0f,
    5.0f,  8.0f,  9.0f,  8.0f,  9.0f,  9.0f,  6.0f,  9.0f,  8.0f,  4.0f,  6.0f,  8.0f,  4.0f,  12.0f, 9.0f,  9.0f,
    9.0f,  9.0f,  7.0f,  8.0f,  7.0f,  8.0f,  9.0f,  12.0f, 8.0f,  9.0f,  8.0f,  7.0f,  5.0f,  7.0f,  10.0f, 10.0f,
    12.0f, 12.0f, 12.0f, 12.0f, 11.0f, 8.0f,  8.0f,  8.0f,  6.0f,  6.0f,  13.0f, 13.0f, 10.0f, 10.0f, 10.0f, 9.0f,
    8.0f,  8.0f,  8.0f,  8.0f,  8.0f,  9.0f,  9.0f,  9.0f,  9.0f,  6.0f,  9.0f,  9.0f,  9.0f,  9.0f,  9.0f,  14.0f,
    14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f, 14.0f,
};

void Message_DrawTextCredits(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    u16 pad;
    u16 j;
    u16 i;
    u16 sfxHi;
    char character;
    u16 charTexIndex;
    Font* font = &msgCtx->font;
    Gfx* gfx = *gfxP;

    msgCtx->textPosX = msgCtx->unk11FF8;

    if (!msgCtx->textIsCredits) {
        msgCtx->textPosY = msgCtx->unk11FFA;
    } else {
        msgCtx->textPosY = 48;
    }

    if (msgCtx->textBoxType == TEXTBOX_TYPE_5) {
        msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 0;
    } else {
        msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 255;
    }

    charTexIndex = 0;

    for (i = 0; i < msgCtx->textDrawPos; i++) {
        character = msgCtx->decodedBuffer.schar[i];

        switch (character) {
            case MESSAGE_NEWLINE:
                msgCtx->textPosX = msgCtx->unk11FF8;
                if ((msgCtx->choiceNum == 1) || (msgCtx->choiceNum == 3)) {
                    msgCtx->textPosX += 32;
                }
                if ((msgCtx->choiceNum == 2) && (msgCtx->textPosY != msgCtx->unk11FFA)) {
                    msgCtx->textPosX += 32;
                }
                msgCtx->textPosY += msgCtx->unk11FFC;
                break;

            case MESSAGE_COLOR:
                break;

            case ' ':
                msgCtx->textPosX += 6;
                break;

            case MESSAGE_BOX_BREAK:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    if (!msgCtx->textboxSkipped) {
                        Audio_PlaySfx(NA_SE_NONE);
                        msgCtx->msgMode = MSGMODE_TEXT_AWAIT_NEXT;
                        Font_LoadMessageBoxEndIcon(font, 0);
                    } else {
                        msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
                        msgCtx->textUnskippable = false;
                        msgCtx->msgBufPos++;
                    }
                }
                *gfxP = gfx;
                return;

            case MESSAGE_SHIFT:
                msgCtx->textPosX += msgCtx->decodedBuffer.schar[++i];
                break;

            case MESSAGE_TEXTID:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_NEXT;
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Audio_PlaySfx(NA_SE_NONE);
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    Font_LoadMessageBoxEndIcon(font, 0);
                }
                *gfxP = gfx;
                return;

            case MESSAGE_QUICKTEXT_ENABLE:
                if (((i + 1) == msgCtx->textDrawPos) &&
                    ((msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) ||
                     ((msgCtx->msgMode >= MSGMODE_OCARINA_STARTING) && (msgCtx->msgMode <= MSGMODE_26)))) {
                    j = i;
                    while (true) {
                        character = msgCtx->decodedBuffer.schar[j];
                        if (character == 6) {
                            j += 2;
                            continue;
                        }
                        if ((character != MESSAGE_QUICKTEXT_DISABLE) && (character != MESSAGE_PERSISTENT) &&
                            (character != MESSAGE_EVENT) && (character != MESSAGE_BOX_BREAK_DELAYED) &&
                            (character != MESSAGE_AWAIT_BUTTON_PRESS) && (character != MESSAGE_BOX_BREAK) &&
                            (character != MESSAGE_END)) {
                            j++;
                            continue;
                        }
                        break;
                    }
                    i = j - 1;
                    msgCtx->textDrawPos = i + 1;
                }
                // fallthrough
            case MESSAGE_QUICKTEXT_DISABLE:
                break;

            case MESSAGE_AWAIT_BUTTON_PRESS:
                if ((i + 1) == msgCtx->textDrawPos) {
                    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                        msgCtx->msgMode = MSGMODE_TEXT_AWAIT_INPUT;
                        Font_LoadMessageBoxEndIcon(font, 0);
                    }
                    *gfxP = gfx;
                    return;
                }
                break;

            case MESSAGE_BOX_BREAK_DELAYED:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->stateTimer = msgCtx->decodedBuffer.schar[++i];
                    msgCtx->msgMode = MSGMODE_TEXT_DELAYED_BREAK;
                }
                *gfxP = gfx;
                return;

            case MESSAGE_FADE2:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADE_NORMAL;
                    msgCtx->stateTimer = msgCtx->decodedBuffer.schar[++i] << 8;
                    msgCtx->stateTimer |= msgCtx->decodedBuffer.schar[++i];
                }
                *gfxP = gfx;
                return;

            case MESSAGE_SFX:
                if ((msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) && !msgCtx->messageHasSetSfx) {
                    msgCtx->messageHasSetSfx = true;
                    sfxHi = msgCtx->decodedBuffer.schar[i + 1];
                    sfxHi = sfxHi << 8;
                    Audio_PlaySfx(sfxHi | msgCtx->decodedBuffer.schar[i + 2]);
                }
                i += 2;
                break;

            case MESSAGE_ITEM_ICON:
                break;

            case MESSAGE_BACKGROUND:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Audio_PlaySfx(NA_SE_NONE);
                }
                gDPPipeSync(gfx++);
                gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, msgCtx->textColorAlpha);

                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_1], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, (msgCtx->textPosX + 1) << 2, (msgCtx->unk12012 + 1) << 2,
                                    (msgCtx->textPosX + 97) << 2, (msgCtx->unk12012 + 49) << 2, G_TX_RENDERTILE, 0, 0,
                                    1 << 10, 1 << 10);

                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_2], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, (msgCtx->textPosX + 97) << 2, (msgCtx->unk12012 + 1) << 2,
                                    (msgCtx->textPosX + 194) << 2, (msgCtx->unk12012 + 49) << 2, 0, 0, 0, 1 << 10,
                                    1 << 10);

                gDPPipeSync(gfx++);
                gDPSetPrimColor(gfx++, 0, 0, 255, 60, 0, msgCtx->textColorAlpha);
                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_1], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, msgCtx->textPosX << 2, msgCtx->unk12012 << 2, (msgCtx->textPosX + 96) << 2,
                                    (msgCtx->unk12012 + 48) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_2], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, (msgCtx->textPosX + 96) << 2, msgCtx->unk12012 << 2,
                                    (msgCtx->textPosX + 192) << 2, (msgCtx->unk12012 + 48) << 2, G_TX_RENDERTILE, 0, 0,
                                    1 << 10, 1 << 10);
                gDPPipeSync(gfx++);
                gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                                  PRIMITIVE, 0);
                msgCtx->textPosX += 32;
                break;

            case MESSAGE_TEXT_SPEED:
                msgCtx->textDelay = msgCtx->decodedBuffer.schar[++i];
                break;

            case MESSAGE_UNSKIPPABLE:
                msgCtx->textUnskippable = true;
                break;

            case MESSAGE_TWO_CHOICE:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_TWO_CHOICE;
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->choiceTextId = msgCtx->currentTextId;
                    msgCtx->stateTimer = 4;
                    msgCtx->choiceIndex = 0;
                    Font_LoadMessageBoxEndIcon(font, 2);
                }
                break;

            case MESSAGE_THREE_CHOICE:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_THREE_CHOICE;
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->choiceTextId = msgCtx->currentTextId;
                    msgCtx->stateTimer = 4;
                    msgCtx->choiceIndex = 0;
                    Font_LoadMessageBoxEndIcon(font, 2);
                }
                break;

            case MESSAGE_END:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_DEFAULT) {
                        Audio_PlaySfx(NA_SE_SY_MESSAGE_END);
                        Font_LoadMessageBoxEndIcon(font, 1);
                        if (play->csCtx.state == CS_STATE_IDLE) {
                            Interface_SetAButtonDoAction(play, DO_ACTION_RETURN);
                        }
                    }
                }
                *gfxP = gfx;
                return;

            case MESSAGE_OCARINA:
                if ((i + 1) == msgCtx->textDrawPos) {
                    Message_HandleOcarina(play);
                    *gfxP = gfx;
                    return;
                }
                break;

            case MESSAGE_FADE:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADE_NORMAL;
                    msgCtx->stateTimer = msgCtx->decodedBuffer.schar[++i];
                    Font_LoadMessageBoxEndIcon(font, 1);
                    if (play->csCtx.state == CS_STATE_IDLE) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_RETURN);
                    }
                }
                *gfxP = gfx;
                return;

            case MESSAGE_PERSISTENT:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Audio_PlaySfx(NA_SE_NONE);
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_PERSISTENT;
                }
                *gfxP = gfx;
                return;

            case MESSAGE_EVENT:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_EVENT;
                    Font_LoadMessageBoxEndIcon(font, 0);
                    Audio_PlaySfx(NA_SE_SY_MESSAGE_END);
                }
                *gfxP = gfx;
                return;

            default:
                if ((msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) && ((i + 1) == msgCtx->textDrawPos) &&
                    (msgCtx->textDelayTimer == msgCtx->textDelay)) {
                    Audio_PlaySfx(NA_SE_NONE);
                }
                Message_DrawTextChar(play, &font->charBuf[font->unk_11D88][charTexIndex], &gfx);
                charTexIndex += FONT_CHAR_TEX_SIZE;

                msgCtx->textPosX += (s32)(sCreditsFontWidths[character - ' '] * msgCtx->textCharScale);
                break;
        }
    }

    if (msgCtx->textDelayTimer == 0) {
        msgCtx->textDrawPos = i + 1;
        msgCtx->textDelayTimer = msgCtx->textDelay;
    } else {
        msgCtx->textDelayTimer--;
    }

    *gfxP = gfx;
}

void Message_DecodeCredits(PlayState* play) {
    u16 curChar;
    s16 loadChar;
    s16 i;
    s16 playerNameLen;
    s16 decodedBufPos = 0;
    s16 charOffset;
    f32 timeInSeconds;
    s16 numLines = 0;
    s16 value;
    s16 digits[4];
    s32 charTexIdx = 0;
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &play->msgCtx.font;

    msgCtx->nextTextId = 0xFFFF;

    while (true) {
        curChar = font->msgBuf.schar[msgCtx->msgBufPos];
        msgCtx->decodedBuffer.schar[decodedBufPos] = font->msgBuf.schar[msgCtx->msgBufPos];

        if ((curChar == MESSAGE_BOX_BREAK) || (curChar == MESSAGE_TEXTID) || (curChar == MESSAGE_BOX_BREAK_DELAYED) ||
            (curChar == MESSAGE_EVENT) || (curChar == MESSAGE_END)) {
            // Textbox decoding ends with any of the above text control characters
            msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
            msgCtx->textDrawPos = 1;
            msgCtx->unk11FFA = msgCtx->textboxY + 8;

            if (msgCtx->textBoxType != TEXTBOX_TYPE_4) {
                if (numLines == 0) {
                    msgCtx->unk11FFA = msgCtx->textboxY + 26;
                } else if (numLines == 1) {
                    msgCtx->unk11FFA = msgCtx->textboxY + 20;
                } else if (numLines == 2) {
                    msgCtx->unk11FFA = msgCtx->textboxY + 16;
                }
            }

            if (curChar == MESSAGE_TEXTID) {
                value = font->msgBuf.schar[msgCtx->msgBufPos + 1];
                msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[msgCtx->msgBufPos + 1];
                msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[msgCtx->msgBufPos + 2];
                value <<= 8;
                msgCtx->nextTextId = msgCtx->decodedBuffer.schar[decodedBufPos] | value;
            }

            if (curChar == MESSAGE_BOX_BREAK_DELAYED) {
                msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[msgCtx->msgBufPos + 1];
                msgCtx->msgBufPos += 2;
            }

            msgCtx->decodedTextLen = decodedBufPos;

            if (msgCtx->textboxSkipped) {
                msgCtx->textDrawPos = msgCtx->decodedTextLen;
            }
            break;
        } else if (curChar == MESSAGE_NAME) {
            // Substitute the player name control character for the file's player name.
            for (playerNameLen = ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); playerNameLen > 0;
                 playerNameLen--) {
                if (gSaveContext.save.saveInfo.playerData.playerName[playerNameLen - 1] != 0x3E) {
                    break;
                }
            }

            for (i = 0; i < playerNameLen; i++) {
                curChar = gSaveContext.save.saveInfo.playerData.playerName[i];
                if (curChar == 62) {
                    curChar = ' ';
                } else if (curChar == 64) {
                    curChar = '.';
                } else if (curChar == 63) {
                    curChar = '-';
                } else if (curChar < 10) {
                    charOffset = curChar;
                    curChar = '0' + charOffset;
                } else if (curChar < 36) {
                    charOffset = curChar;
                    curChar = 'A' - 10 + charOffset;
                } else if (curChar < 62) {
                    charOffset = curChar;
                    curChar = 'a' - 36 + charOffset;
                }

                if (curChar != ' ') {
                    Font_LoadCharNES(play, curChar - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                }
                msgCtx->decodedBuffer.schar[decodedBufPos] = curChar;
                decodedBufPos++;
            }
            decodedBufPos--;
        } else if ((curChar == MESSAGE_MARATHON_TIME) || (curChar == MESSAGE_RACE_TIME)) {
            digits[0] = digits[1] = digits[2] = 0;
            if (curChar == 0x17) {
                digits[3] = gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2];
            } else {
                digits[3] = gSaveContext.timerCurTimes[TIMER_ID_GORON_RACE_UNUSED];
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
                Font_LoadCharNES(play, digits[i] + '0' - ' ', charTexIdx);
                charTexIdx += FONT_CHAR_TEX_SIZE;
                msgCtx->decodedBuffer.schar[decodedBufPos] = digits[i] + '0';
                decodedBufPos++;
                if (i == 1) {
                    Font_LoadCharNES(play, '"' - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.schar[decodedBufPos] = '"';
                    decodedBufPos++;
                } else if (i == 3) {
                    Font_LoadCharNES(play, '"' - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.schar[decodedBufPos] = '"';
                }
            }

        } else if (curChar == MESSAGE_POINTS) {
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
                if ((i == 3) || (digits[i] != 0)) {
                    loadChar = true;
                }
                if (loadChar) {
                    Font_LoadCharNES(play, digits[i] + '0' - ' ', charTexIdx);
                    msgCtx->decodedBuffer.schar[decodedBufPos] = digits[i] + '0';
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    decodedBufPos++;
                }
            }
            decodedBufPos--;
        } else if (curChar != MESSAGE_TOKENS) {
            if (curChar == MESSAGE_FISH_INFO) {
                digits[0] = 0;
                digits[1] = gSaveContext.minigameScore;

                while (digits[1] >= 10) {
                    digits[0]++;
                    digits[1] -= 10;
                }

                for (i = 0; i < 2; i++) {
                    if ((i == 1) || (digits[i] != 0)) {
                        Font_LoadCharNES(play, digits[i] + '0' - ' ', charTexIdx);
                        charTexIdx += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.schar[decodedBufPos] = digits[i] + '0';
                        decodedBufPos++;
                    }
                }
                decodedBufPos--;
            } else if (curChar == MESSAGE_HIGHSCORE) {
                value = HIGH_SCORE((u8)font->msgBuf.schar[++msgCtx->msgBufPos]);
                if ((font->msgBuf.schar[msgCtx->msgBufPos] & 0xFF) == HS_FISHING) {
                    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
                        value &= 0x7F;
                    } else {
                        value =
                            (s16)((HIGH_SCORE((u8)font->msgBuf.schar[msgCtx->msgBufPos]) & 0xFF000000) >> 0x18) & 0x7F;
                    }
                    value = SQ((f32)value) * 0.0036f + 0.5f;
                }
                switch (font->msgBuf.schar[msgCtx->msgBufPos] & 0xFF) {
                    case HS_BANK_RUPEES:
                    case HS_UNK_1:
                    case HS_FISHING:
                        digits[0] = digits[1] = digits[2] = 0;
                        digits[3] = value;

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
                            if ((i == 3) || (digits[i] != 0)) {
                                loadChar = true;
                            }
                            if (loadChar) {
                                Font_LoadCharNES(play, digits[i] + '0' - ' ', charTexIdx);
                                msgCtx->decodedBuffer.schar[decodedBufPos] = digits[i] + '0';
                                charTexIdx += FONT_CHAR_TEX_SIZE;
                                decodedBufPos++;
                            }
                        }
                        decodedBufPos--;
                        break;

                    case HS_BOAT_ARCHERY:
                    case HS_HORSE_BACK_BALLOON:
                    case HS_SHOOTING_GALLERY:
                        digits[0] = digits[1] = digits[2] = 0;
                        digits[3] = value;

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
                            Font_LoadCharNES(play, digits[i] + '0' - ' ', charTexIdx);
                            charTexIdx += FONT_CHAR_TEX_SIZE;
                            msgCtx->decodedBuffer.schar[decodedBufPos] = digits[i] + '0';
                            decodedBufPos++;
                            if (i == 1) {
                                Font_LoadCharNES(play, '"' - ' ', charTexIdx);
                                charTexIdx += FONT_CHAR_TEX_SIZE;
                                msgCtx->decodedBuffer.schar[decodedBufPos] = '"';
                                decodedBufPos++;
                            } else if (i == 3) {
                                Font_LoadCharNES(play, '"' - ' ', charTexIdx);
                                charTexIdx += FONT_CHAR_TEX_SIZE;
                                msgCtx->decodedBuffer.schar[decodedBufPos] = '"';
                            }
                        }
                        break;

                    default:
                        break;
                }
            } else if (curChar == MESSAGE_TIME) {
                digits[0] = 0;
                timeInSeconds = TIME_TO_MINUTES_F(CURRENT_TIME);

                digits[1] = timeInSeconds / 60.0f;
                while (digits[1] >= 10) {
                    digits[0]++;
                    digits[1] -= 10;
                }

                digits[2] = 0;
                digits[3] = (s32)timeInSeconds % 60;
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                for (i = 0; i < 4; i++) {
                    Font_LoadCharNES(play, digits[i] + '0' - ' ', charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.schar[decodedBufPos] = digits[i] + '0';
                    decodedBufPos++;
                    if (i == 1) {
                        Font_LoadCharNES(play, ':' - ' ', charTexIdx);
                        charTexIdx += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.schar[decodedBufPos] = ':';
                        decodedBufPos++;
                    }
                }

                decodedBufPos--;
            } else if (curChar == MESSAGE_ITEM_ICON) {
                msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[msgCtx->msgBufPos + 1];
                Message_LoadItemIcon(play, font->msgBuf.schar[msgCtx->msgBufPos + 1], msgCtx->textboxY + 10);
            } else if (curChar == MESSAGE_BACKGROUND) {
                // #region 2S2H [Port]
                // DmaMgr_SendRequest0(msgCtx->textboxSegment + 0x1000,
                //                     (uintptr_t)SEGMENT_ROM_START(message_texture_static) + 0x900, 0x900);
                // DmaMgr_SendRequest0(msgCtx->textboxSegment + 0x1900,
                //                     (uintptr_t)SEGMENT_ROM_START(message_texture_static) + 0x900, 0x900);
                msgCtx->textboxSegment[TEXTBOX_SEG_BG_1] = gMessageXLeftTex;
                msgCtx->textboxSegment[TEXTBOX_SEG_BG_2] = gMessageXRightTex;
                // #endregion
                msgCtx->msgBufPos += 3;
                msgCtx->unk12012 = msgCtx->textboxY + 8;
                numLines = 2;
            } else if (curChar == MESSAGE_COLOR) {
                msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[++msgCtx->msgBufPos];
            } else if (curChar == MESSAGE_NEWLINE) {
                numLines++;
            } else if ((curChar != MESSAGE_QUICKTEXT_ENABLE) && (curChar != MESSAGE_QUICKTEXT_DISABLE) &&
                       (curChar != MESSAGE_AWAIT_BUTTON_PRESS) && (curChar != MESSAGE_OCARINA) &&
                       (curChar != MESSAGE_PERSISTENT) && (curChar != MESSAGE_UNSKIPPABLE)) {
                if (curChar == MESSAGE_FADE) {
                    msgCtx->textFade = true;
                    msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[++msgCtx->msgBufPos];
                } else if (curChar == MESSAGE_FADE2) {
                    msgCtx->textFade = true;
                    msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[++msgCtx->msgBufPos];
                    msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[++msgCtx->msgBufPos];
                } else if ((curChar == MESSAGE_SHIFT) || (curChar == MESSAGE_TEXT_SPEED)) {
                    msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[++msgCtx->msgBufPos] & 0xFF;
                } else if (curChar == MESSAGE_SFX) {
                    msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[++msgCtx->msgBufPos];
                    msgCtx->decodedBuffer.schar[++decodedBufPos] = font->msgBuf.schar[++msgCtx->msgBufPos];
                } else if (curChar == MESSAGE_TWO_CHOICE) {
                    msgCtx->choiceNum = 2;
                } else if (curChar == MESSAGE_THREE_CHOICE) {
                    msgCtx->choiceNum = 3;
                } else if (curChar != ' ') {
                    Font_LoadCharNES(play, curChar, charTexIdx);
                    charTexIdx += FONT_CHAR_TEX_SIZE;
                }
            }
        }

        decodedBufPos++;
        msgCtx->msgBufPos++;
    }
}

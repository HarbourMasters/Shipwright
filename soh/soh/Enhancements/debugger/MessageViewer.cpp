#include "MessageViewer.h"

#include <soh/UIWidgets.hpp>
#include <textures/message_static/message_static.h>

#include "../custom-message/CustomMessageManager.h"
#include "functions.h"
#include "macros.h"
#include "message_data_static.h"
#include "variables.h"
#include "soh/util.h"

extern "C" u8 sMessageHasSetSfx;

void MessageViewer::InitElement() {
    CustomMessageManager::Instance->AddCustomMessageTable(TABLE_ID);
    mTableIdBuf = static_cast<char*>(calloc(MAX_STRING_SIZE, sizeof(char)));
    mTextIdBuf = static_cast<char*>(calloc(MAX_STRING_SIZE, sizeof(char)));
    mCustomMessageBuf = static_cast<char*>(calloc(MAX_STRING_SIZE, sizeof(char)));
}

void MessageViewer::DrawElement() {
    ImGui::Text("Table ID");
    ImGui::SameLine();
    ImGui::InputText("##TableID", mTableIdBuf, MAX_STRING_SIZE, ImGuiInputTextFlags_CallbackCharFilter, UIWidgets::TextFilters::FilterAlphaNum);
    UIWidgets::InsertHelpHoverText("Leave blank for vanilla table");
    ImGui::Text("Text ID");
    ImGui::SameLine();
    switch (mTextIdBase) {
        case DECIMAL:
            ImGui::InputText("##TextID", mTextIdBuf, MAX_STRING_SIZE, ImGuiInputTextFlags_CharsDecimal);
            UIWidgets::InsertHelpHoverText("Decimal Text ID of the message to load. Decimal digits only (0-9).");
            break;
        case HEXADECIMAL:
        default:
            ImGui::InputText("##TextID", mTextIdBuf, MAX_STRING_SIZE, ImGuiInputTextFlags_CharsHexadecimal);
            UIWidgets::InsertHelpHoverText("Hexadecimal Text ID of the message to load. Hexadecimal digits only (0-9/A-F).");
            break;
    }
    if (ImGui::RadioButton("Hexadecimal", &mTextIdBase, HEXADECIMAL)) {
        memset(mTextIdBuf, 0, sizeof(char) * MAX_STRING_SIZE);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Decimal", &mTextIdBase, DECIMAL)) {
        memset(mTextIdBuf, 0, sizeof(char) * MAX_STRING_SIZE);
    }
    ImGui::Text("Language");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##Language", mLanguages[mLanguage])) {
        // ReSharper disable CppDFAUnreachableCode
        for (size_t i = 0; i < mLanguages.size(); i++) {
            if (strlen(mLanguages[i]) > 0) {
                if (ImGui::Selectable(mLanguages[i], i == mLanguage)) {
                    mLanguage = i;
                }
            }
        }
        ImGui::EndCombo();
    }
    UIWidgets::InsertHelpHoverText("Which language to load from the selected text ID");
    if (ImGui::Button("Display Message##ExistingMessage")) {
        mDisplayExistingMessageClicked = true;
    }
    ImGui::Text("Custom Message");
    UIWidgets::InsertHelpHoverText("Enter a string using Custom Message Syntax to preview it in-game. "
                                   "Any newline (\\n) characters inserted by the Enter key will be stripped "
                                   "from the output.");
    ImGui::InputTextMultiline("##CustomMessage", mCustomMessageBuf, MAX_STRING_SIZE);
    if (ImGui::Button("Display Message##CustomMessage")) {
        mDisplayCustomMessageClicked = true;
    }
    // ReSharper restore CppDFAUnreachableCode
}

void MessageViewer::UpdateElement() {
    if (mDisplayExistingMessageClicked) {
        mTableId = std::string(mTableIdBuf);
        switch (mTextIdBase) {
            case DECIMAL:
                mTextId = std::stoi(std::string(mTextIdBuf), nullptr, 10);
                break;
            case HEXADECIMAL:
            default:
                mTextId = std::stoi(std::string(mTextIdBuf), nullptr, 16);
                break;
        }
        DisplayExistingMessage();
        mDisplayExistingMessageClicked = false;
    }
    if (mDisplayCustomMessageClicked) {
        mCustomMessageString = std::string(mCustomMessageBuf);
        std::erase(mCustomMessageString, '\n');
        DisplayCustomMessage();
        mDisplayCustomMessageClicked = false;
    }
}

void MessageViewer::DisplayExistingMessage() const {
    MessageDebug_StartTextBox(mTableId.c_str(), mTextId, mLanguage);
}

void MessageViewer::DisplayCustomMessage() const {
    MessageDebug_DisplayCustomMessage(mCustomMessageString.c_str());
}

extern "C" MessageTableEntry* sNesMessageEntryTablePtr;
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;
extern "C" MessageTableEntry* sStaffMessageEntryTablePtr;

void FindMessage(PlayState* play, const uint16_t textId, const uint8_t language) {
    const char* foundSeg;
    const char* nextSeg;
    MessageTableEntry* messageTableEntry = sNesMessageEntryTablePtr;
    Font* font;
    u16 bufferId = textId;
    // Use the better owl message if better owl is enabled
    if (CVarGetInteger(CVAR_ENHANCEMENT("BetterOwl"), 0) != 0 && (bufferId == 0x2066 || bufferId == 0x607B ||
        bufferId == 0x10C2 || bufferId == 0x10C6 || bufferId == 0x206A))
    {
        bufferId = 0x71B3;
    }

    if (language == LANGUAGE_GER)
        messageTableEntry = sGerMessageEntryTablePtr;
    else if (language == LANGUAGE_FRA)
        messageTableEntry = sFraMessageEntryTablePtr;

    // If PAL languages are not present in the OTR file, default to English
    if (messageTableEntry == nullptr)
        messageTableEntry = sNesMessageEntryTablePtr;

    const char* seg = messageTableEntry->segment;

    while (messageTableEntry->textId != 0xFFFF) {
        font = &play->msgCtx.font;

        if (messageTableEntry->textId == bufferId) {
            foundSeg = messageTableEntry->segment;
            font->charTexBuf[0] = messageTableEntry->typePos;

            nextSeg = messageTableEntry->segment;
            font->msgOffset = reinterpret_cast<uintptr_t>(messageTableEntry->segment);
            font->msgLength = messageTableEntry->msgSize;
            return;
        }
        messageTableEntry++;
    }

    font = &play->msgCtx.font;
    messageTableEntry = sNesMessageEntryTablePtr;

    foundSeg = messageTableEntry->segment;
    font->charTexBuf[0] = messageTableEntry->typePos;
    messageTableEntry++;
    nextSeg = messageTableEntry->segment;
    font->msgOffset = foundSeg - seg;
    font->msgLength = nextSeg - foundSeg;
}

static const char* msgStaticTbl[] =
{
    gDefaultMessageBackgroundTex,
    gSignMessageBackgroundTex,
    gNoteStaffMessageBackgroundTex,
    gFadingMessageBackgroundTex,
    gMessageContinueTriangleTex,
    gMessageEndSquareTex,
    gMessageArrowTex
};

void MessageDebug_StartTextBox(const char* tableId, uint16_t textId, uint8_t language) {
    PlayState* play = gPlayState;
    static int16_t messageStaticIndices[] = { 0, 1, 3, 2 };
    const auto player = GET_PLAYER(gPlayState);
    player->actor.flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
    MessageContext* msgCtx = &play->msgCtx;
    msgCtx->ocarinaAction = 0xFFFF;
    Font* font = &msgCtx->font;
    sMessageHasSetSfx = 0;
    for (u32 i = 0; i < FONT_CHAR_TEX_SIZE * 120; i += FONT_CHAR_TEX_SIZE) {
        if (&font->charTexBuf[i] != nullptr) {
            gSPInvalidateTexCache(play->state.gfxCtx->polyOpa.p++, reinterpret_cast<uintptr_t>(&font->charTexBuf[i]));
        }
    }
    R_TEXT_CHAR_SCALE = 75;
    R_TEXT_LINE_SPACING = 12;
    R_TEXT_INIT_XPOS = 65;
    char* buffer = font->msgBuf;
    msgCtx->textId = textId;
    if (strlen(tableId) == 0) {
        FindMessage(play, textId, language);
        msgCtx->msgLength = static_cast<int32_t>(font->msgLength);
        const uintptr_t src = font->msgOffset;
        memcpy(font->msgBuf, reinterpret_cast<void const *>(src), font->msgLength);
    } else {
        constexpr int maxBufferSize = sizeof(font->msgBuf);
        const CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(tableId, textId);
        font->charTexBuf[0] = (messageEntry.GetTextBoxType() << 4) | messageEntry.GetTextBoxPosition();
        switch (language) {
            case LANGUAGE_FRA:
                font->msgLength = SohUtils::CopyStringToCharBuffer(buffer, messageEntry.GetFrench(), maxBufferSize);
                break;
            case LANGUAGE_GER:
                font->msgLength = SohUtils::CopyStringToCharBuffer(buffer, messageEntry.GetGerman(), maxBufferSize);
                break;
            case LANGUAGE_ENG:
            default:
                font->msgLength = SohUtils::CopyStringToCharBuffer(buffer, messageEntry.GetEnglish(), maxBufferSize);
                break;
        }
        msgCtx->msgLength = static_cast<int32_t>(font->msgLength);
    }
    msgCtx->textBoxProperties = font->charTexBuf[0];
    msgCtx->textBoxType = msgCtx->textBoxProperties >> 4;
    msgCtx->textBoxPos = msgCtx->textBoxProperties & 0xF;
    const int16_t textBoxType = msgCtx->textBoxType;
    // "Text Box Type"
    osSyncPrintf("吹き出し種類＝%d\n", msgCtx->textBoxType);
    if (textBoxType < TEXTBOX_TYPE_NONE_BOTTOM) {
        const char* textureName = msgStaticTbl[messageStaticIndices[textBoxType]];
        memcpy(msgCtx->textboxSegment, textureName, strlen(textureName) + 1);
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
    msgCtx->talkActor = &player->actor;
    msgCtx->msgMode = MSGMODE_TEXT_START;
    msgCtx->stateTimer = 0;
    msgCtx->textDelayTimer = 0;
    msgCtx->ocarinaMode = OCARINA_MODE_00;
}

void MessageDebug_DisplayCustomMessage(const char* customMessage) {
    CustomMessageManager::Instance->ClearMessageTable(MessageViewer::TABLE_ID);
    CustomMessageManager::Instance->CreateMessage(MessageViewer::TABLE_ID, 0,
        CustomMessage(customMessage, customMessage, customMessage));
    MessageDebug_StartTextBox(MessageViewer::TABLE_ID, 0, 0);
}



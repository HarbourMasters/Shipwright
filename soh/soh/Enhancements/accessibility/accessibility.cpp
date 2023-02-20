#include "soh/Enhancements/game-interactor/GameInteractor.h"

#include <OtrFile.h>
#include <libultraship/bridge.h>
#include <libultraship/classes.h>
#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>

#include "soh/OTRGlobals.h"
#include "message_data_static.h"
#include "overlays/gamestates/ovl_file_choose/file_choose.h"

extern "C" {
extern PlayState* gPlayState;
}

typedef enum {
    /* 0x00 */ TEXT_BANK_SCENES,
    /* 0x01 */ TEXT_BANK_MISC,
    /* 0x02 */ TEXT_BANK_KALEIDO,
    /* 0x03 */ TEXT_BANK_FILECHOOSE,
} TextBank;

nlohmann::json sceneMap = nullptr;
nlohmann::json miscMap = nullptr;
nlohmann::json kaleidoMap = nullptr;
nlohmann::json fileChooseMap = nullptr;

// MARK: - Helpers

std::string GetParameritizedText(std::string key, TextBank bank, const char* arg) {
    switch (bank) {
        case TEXT_BANK_SCENES: {
            // TODO: Remove after everything is cataloged
            SohImGui::GetGameOverlay()->TextDrawNotification(12.0, true, "scene_json index: %s", key.c_str());
            return sceneMap[key].get<std::string>();
            break;
        }
        case TEXT_BANK_MISC: {
            auto value = miscMap[key].get<std::string>();
            
            std::string searchString = "$0";
            size_t index = value.find(searchString);
            
            if (index != std::string::npos) {
                ASSERT(arg != nullptr);
                value.replace(index, searchString.size(), std::string(arg));
                return value;
            } else {
                return value;
            }
            
            break;
        }
        case TEXT_BANK_KALEIDO: {
            // TODO: Remove after everything is cataloged
            SohImGui::GetGameOverlay()->TextDrawNotification(12.0, true, "kaleido_json index: %s", key.c_str());
            auto value = kaleidoMap[key].get<std::string>();
            
            std::string searchString = "$0";
            size_t index = value.find(searchString);
            
            if (index != std::string::npos) {
                ASSERT(arg != nullptr);
                value.replace(index, searchString.size(), std::string(arg));
                return value;
            } else {
                return value;
            }
            
            break;
        }
        case TEXT_BANK_FILECHOOSE: {
            return fileChooseMap[key].get<std::string>();
            break;
        }
    }
}

const char* GetLanguageCode() {
    switch (CVarGetInteger("gLanguages", 0)) {
        case LANGUAGE_FRA:
            return "fr-FR";
            break;
        case LANGUAGE_GER:
            return "de-DE";
            break;
    }
    
    return "en-US";
}

// MARK: - Boss Title Cards

const char* NameForSceneId(int16_t sceneId) {
    auto key = std::to_string(sceneId);
    auto name = GetParameritizedText(key, TEXT_BANK_SCENES, nullptr);
    return strdup(name.c_str());
}

static const char* titleCardText;

void RegisterOnSceneInitHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        titleCardText = NameForSceneId(sceneNum);
    });
}

void RegisterOnPresentTitleCardHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPresentTitleCard>([]() {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        SpeechSynthesizerSpeak(titleCardText, GetLanguageCode());
    });
}

// MARK: - Interface Updates

void RegisterOnInterfaceUpdateHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnInterfaceUpdate>([]() {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        static uint32_t prevTimer = 0;
        static char ttsAnnounceBuf[32];
        
        uint32_t timer = 0;
        if (gSaveContext.timer1State != 0) {
            timer = gSaveContext.timer1Value;
        } else if (gSaveContext.timer2State != 0) {
            timer = gSaveContext.timer2Value;
        }
        
        if (timer > 0) {
            if (timer > prevTimer || (timer % 30 == 0 && prevTimer != timer)) {
                uint32_t minutes = timer / 60;
                uint32_t seconds = timer % 60;
                char* announceBuf = ttsAnnounceBuf;
                char arg[8]; // at least big enough where no s8 string will overflow
                if (minutes > 0) {
                    snprintf(arg, sizeof(arg), "%d", minutes);
                    auto translation = GetParameritizedText((minutes > 1) ? "minutes_plural" : "minutes_singular", TEXT_BANK_MISC, arg);
                    announceBuf += snprintf(announceBuf, sizeof(ttsAnnounceBuf), "%s ", translation.c_str());
                }
                if (seconds > 0) {
                    snprintf(arg, sizeof(arg), "%d", seconds);
                    auto translation = GetParameritizedText((seconds > 1) ? "seconds_plural" : "seconds_singular", TEXT_BANK_MISC, arg);
                    announceBuf += snprintf(announceBuf, sizeof(ttsAnnounceBuf), "%s", translation.c_str());
                }
                ASSERT(announceBuf < ttsAnnounceBuf + sizeof(ttsAnnounceBuf));
                SpeechSynthesizerSpeak(strdup(ttsAnnounceBuf), GetLanguageCode());
                prevTimer = timer;
            }
        }
        
        prevTimer = timer;
        
        if (!GameInteractor::IsSaveLoaded()) return;
        
        static int16_t lostHealth = 0;
        static int16_t prevHealth = 0;
        
        if (gSaveContext.health - prevHealth < 0) {
            lostHealth += prevHealth - gSaveContext.health;
        }
        
        if (gPlayState->state.frames % 7 == 0) {
            if (lostHealth >= 16) {
                Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                lostHealth -= 16;
            }
        }
        
        prevHealth = gSaveContext.health;
    });
}


void RegisterOnKaleidoscopeUpdateHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnKaleidoscopeUpdate>([](int16_t inDungeonScene) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        static uint16_t prevCursorIndex = 0;
        static uint16_t prevCursorSpecialPos = 0;
        static uint16_t prevCursorPoint[5] = { 0 };
        
        PauseContext* pauseCtx = &gPlayState->pauseCtx;
        Input* input = &gPlayState->state.input[0];
        
        if (pauseCtx->state != 6) {
            //reset cursor index to so it is announced when pause is reopened
            prevCursorIndex = -1;
            return;
        }
        
        if ((pauseCtx->debugState != 1) && (pauseCtx->debugState != 2)) {
            char arg[8];
            if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                snprintf(arg, sizeof(arg), "%d", gSaveContext.health);
                auto translation = GetParameritizedText("health", TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                snprintf(arg, sizeof(arg), "%d", gSaveContext.magic);
                auto translation = GetParameritizedText("magic", TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                snprintf(arg, sizeof(arg), "%d", gSaveContext.rupees);
                auto translation = GetParameritizedText("rupees", TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                //TODO: announce timer?
            }
        }
        
        uint16_t cursorIndex = (pauseCtx->pageIndex == PAUSE_MAP && !inDungeonScene) ? PAUSE_WORLD_MAP : pauseCtx->pageIndex;
        if (prevCursorIndex == cursorIndex &&
            prevCursorSpecialPos == pauseCtx->cursorSpecialPos &&
            prevCursorPoint[cursorIndex] == pauseCtx->cursorPoint[cursorIndex]) {
            return;
        }
        
        prevCursorSpecialPos = pauseCtx->cursorSpecialPos;
        
        if (pauseCtx->cursorSpecialPos > 0) {
            return;
        }
        
        switch (pauseCtx->pageIndex) {
            case PAUSE_ITEM:
            {
                char arg[8]; // at least big enough where no s8 string will overflow
                switch (pauseCtx->cursorItem[PAUSE_ITEM]) {
                    case ITEM_STICK:
                    case ITEM_NUT:
                    case ITEM_BOMB:
                    case ITEM_BOMBCHU:
                    case ITEM_SLINGSHOT:
                    case ITEM_BOW:
                        snprintf(arg, sizeof(arg), "%d", AMMO(pauseCtx->cursorItem[PAUSE_ITEM]));
                        break;
                    case ITEM_BEAN:
                        snprintf(arg, sizeof(arg), "%d", 0);
                        break;
                    default:
                        arg[0] = '\0';
                }
                
                if (pauseCtx->cursorItem[PAUSE_ITEM] == 999) {
                    return;
                }
                
                std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_ITEM]);
                auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            case PAUSE_MAP:
                if (inDungeonScene) {
                    if (pauseCtx->cursorItem[PAUSE_MAP] != PAUSE_ITEM_NONE) {
                        std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_MAP]);
                        auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, nullptr);
                        SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                    }
                } else {
                    std::string key = std::to_string(0x0100 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP]);
                    auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, nullptr);
                    SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                    SPDLOG_INFO("Item: {}", key);
                }
                break;
            case PAUSE_QUEST:
            {
                char arg[8]; // at least big enough where no s8 string will overflow
                switch (pauseCtx->cursorItem[PAUSE_QUEST]) {
                    case ITEM_SKULL_TOKEN:
                        snprintf(arg, sizeof(arg), "%d", gSaveContext.inventory.gsTokens);
                        break;
                    case ITEM_HEART_CONTAINER:
                        snprintf(arg, sizeof(arg), "%d", ((gSaveContext.inventory.questItems & 0xF) & 0xF) >> 0x1C);
                        break;
                    default:
                        arg[0] = '\0';
                }
                
                if (pauseCtx->cursorItem[PAUSE_QUEST] == 999) {
                    return;
                }
                
                std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_QUEST]);
                auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            case PAUSE_EQUIP:
            {
                std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_EQUIP]);
                auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, nullptr);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            default:
                break;
        }
        
        prevCursorIndex = cursorIndex;
        memcpy(prevCursorPoint, pauseCtx->cursorPoint, sizeof(prevCursorPoint));
    });
}

void RegisterOnUpdateMainMenuSelection() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPresentFileSelect>([]() {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        auto translation = GetParameritizedText("file1", TEXT_BANK_FILECHOOSE, nullptr);
        SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
    });
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileSelectSelection>([](uint16_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_BTN_MAIN_FILE_1: {
                auto translation = GetParameritizedText("file1", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_FILE_2: {
                auto translation = GetParameritizedText("file2", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_FILE_3: {
                auto translation = GetParameritizedText("file3", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_OPTIONS: {
                auto translation = GetParameritizedText("options", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_COPY: {
                auto translation = GetParameritizedText("copy", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_ERASE: {
                auto translation = GetParameritizedText("erase", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizerSpeak(strdup(translation.c_str()), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });
}

// MARK: - Dialog Messages

static uint8_t ttsHasMessage;
static uint8_t ttsHasNewMessage;
static char ttsMessageBuf[256];
static int8_t ttsCurrentHighlightedChoice;

char8_t const* remap(uint8_t character) {
    switch (character) {
        case 0x80: return u8"À";
        case 0x81: return u8"î";
        case 0x82: return u8"Â";
        case 0x83: return u8"Ä";
        case 0x84: return u8"Ç";
        case 0x85: return u8"È";
        case 0x86: return u8"É";
        case 0x87: return u8"Ê";
        case 0x88: return u8"Ë";
        case 0x89: return u8"Ï";
        case 0x8A: return u8"Ô";
        case 0x8B: return u8"Ö";
        case 0x8C: return u8"Ù";
        case 0x8D: return u8"Û";
        case 0x8E: return u8"Ü";
        case 0x8F: return u8"ß";
        case 0x90: return u8"à";
        case 0x91: return u8"á";
        case 0x92: return u8"â";
        case 0x93: return u8"ä";
        case 0x94: return u8"ç";
        case 0x95: return u8"è";
        case 0x96: return u8"é";
        case 0x97: return u8"ê";
        case 0x98: return u8"ë";
        case 0x99: return u8"ï";
        case 0x9A: return u8"ô";
        case 0x9B: return u8"ö";
        case 0x9C: return u8"ù";
        case 0x9D: return u8"û";
        case 0x9E: return u8"ü";
        case 0x9F: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_a", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA0: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_b", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA1: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_c", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA2: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_l", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA3: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_r", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA4: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_z", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA5: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_c_up", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA6: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_c_down", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA7: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_c_left", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xA8: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_button_c_right", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xAA: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_analog_stick", TEXT_BANK_MISC, nullptr).c_str()));
        case 0xAB: return reinterpret_cast<const char8_t*>(strdup(GetParameritizedText("input_d_pad", TEXT_BANK_MISC, nullptr).c_str()));
        default: return u8"";
    }
}

void Message_TTS_Decode(uint8_t* sourceBuf, char* destBuf, uint16_t startOfset, uint16_t size) {
    uint32_t destWriteIndex = 0;
    uint8_t isListingChoices = 0;
    
    for (uint16_t i = 0; i < size; i++) {
        uint8_t cchar = sourceBuf[i + startOfset];
        
        if (cchar < ' ') {
            switch (cchar) {
                case MESSAGE_NEWLINE:
                    destBuf[destWriteIndex++] = (isListingChoices) ? '\n' : ' ';
                    break;
                case MESSAGE_THREE_CHOICE:
                case MESSAGE_TWO_CHOICE:
                    destBuf[destWriteIndex++] = '\n';
                    isListingChoices = 1;
                    break;
                case MESSAGE_COLOR:
                case MESSAGE_SHIFT:
                case MESSAGE_TEXT_SPEED:
                case MESSAGE_BOX_BREAK_DELAYED:
                case MESSAGE_FADE:
                case MESSAGE_ITEM_ICON:
                    i++;
                    break;
                case MESSAGE_FADE2:
                case MESSAGE_SFX:
                case MESSAGE_TEXTID:
                    i += 2;
                    break;
                default:
                    break;
            }
        } else {
            if (cchar <= 0x80) {
                destBuf[destWriteIndex++] = cchar;
            } else {
                auto rchar = remap(cchar);
                while (*rchar) {
                    destBuf[destWriteIndex++] = *rchar++;
                }
            }
        }
    }
    
    destBuf[destWriteIndex] = 0;
}

void RegisterOnDialogMessageHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnDialogMessage>([]() {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        MessageContext *msgCtx = &gPlayState->msgCtx;
        
        if (msgCtx->msgMode == MSGMODE_TEXT_NEXT_MSG || msgCtx->msgMode == MSGMODE_DISPLAY_SONG_PLAYED_TEXT_BEGIN || (msgCtx->msgMode == MSGMODE_TEXT_CONTINUING && msgCtx->stateTimer == 1)) {
            ttsHasNewMessage = 1;
        } else if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING || msgCtx->msgMode == MSGMODE_TEXT_AWAIT_NEXT || msgCtx->msgMode == MSGMODE_TEXT_DONE || msgCtx->msgMode == MSGMODE_TEXT_DELAYED_BREAK
                   || msgCtx->msgMode == MSGMODE_OCARINA_STARTING || msgCtx->msgMode == MSGMODE_OCARINA_PLAYING
                   || msgCtx->msgMode == MSGMODE_DISPLAY_SONG_PLAYED_TEXT || msgCtx->msgMode == MSGMODE_DISPLAY_SONG_PLAYED_TEXT || msgCtx->msgMode == MSGMODE_SONG_PLAYED_ACT_BEGIN || msgCtx->msgMode == MSGMODE_SONG_PLAYED_ACT || msgCtx->msgMode == MSGMODE_SONG_PLAYBACK_STARTING || msgCtx->msgMode == MSGMODE_SONG_PLAYBACK || msgCtx->msgMode == MSGMODE_SONG_DEMONSTRATION_STARTING || msgCtx->msgMode == MSGMODE_SONG_DEMONSTRATION_SELECT_INSTRUMENT || msgCtx->msgMode == MSGMODE_SONG_DEMONSTRATION
        ) {
            if (ttsHasNewMessage) {
                ttsHasMessage = 1;
                ttsHasNewMessage = 0;
                ttsCurrentHighlightedChoice = 0;
                
                uint16_t size = msgCtx->decodedTextLen;
                Message_TTS_Decode(msgCtx->msgBufDecoded, ttsMessageBuf, 0, size);
                SpeechSynthesizerSpeak(ttsMessageBuf, GetLanguageCode());
            } else if (msgCtx->msgMode == MSGMODE_TEXT_DONE && msgCtx->choiceNum > 0 && msgCtx->choiceIndex != ttsCurrentHighlightedChoice) {
                ttsCurrentHighlightedChoice = msgCtx->choiceIndex;
                uint16_t startOffset = 0;
                while (startOffset < msgCtx->decodedTextLen) {
                    if (msgCtx->msgBufDecoded[startOffset] == MESSAGE_TWO_CHOICE || msgCtx->msgBufDecoded[startOffset] == MESSAGE_THREE_CHOICE) {
                        startOffset++;
                        break;
                    }
                    startOffset++;
                }
                
                uint16_t endOffset = 0;
                if (startOffset < msgCtx->decodedTextLen) {
                    uint8_t i = msgCtx->choiceIndex;
                    while (i-- > 0) {
                        while (startOffset < msgCtx->decodedTextLen) {
                            if (msgCtx->msgBufDecoded[startOffset] == MESSAGE_NEWLINE) {
                                startOffset++;
                                break;
                            }
                            startOffset++;
                        }
                    }
                    
                    endOffset = startOffset;
                    while (endOffset < msgCtx->decodedTextLen) {
                        if (msgCtx->msgBufDecoded[endOffset] == MESSAGE_NEWLINE) {
                            break;
                        }
                        endOffset++;
                    }
                    
                    if (startOffset < msgCtx->decodedTextLen && startOffset != endOffset) {
                        uint16_t size = endOffset - startOffset;
                        Message_TTS_Decode(msgCtx->msgBufDecoded, ttsMessageBuf, startOffset, size);
                        SpeechSynthesizerSpeak(ttsMessageBuf, GetLanguageCode());
                    }
                }
            }
        } else if (ttsHasMessage) {
            ttsHasMessage = 0;
            ttsHasNewMessage = 0;
            
            if (msgCtx->decodedTextLen < 3 || (msgCtx->msgBufDecoded[msgCtx->decodedTextLen - 2] != MESSAGE_FADE && msgCtx->msgBufDecoded[msgCtx->decodedTextLen - 3] != MESSAGE_FADE2)) {
                SpeechSynthesizerSpeakEnglish(""); // cancel current speech (except for faded out messages)
            }
        }
    });
}

// MARK: - Main Registration

void InitAccessibilityTexts() {
    std::string languageSuffix = "_eng.json";
    switch (CVarGetInteger("gLanguages", 0)) {
        case LANGUAGE_FRA:
            languageSuffix = "_fra.json";
            break;
        case LANGUAGE_GER:
            languageSuffix = "_ger.json";
            break;
    }

    auto sceneFile = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("accessibility/texts/scenes" + languageSuffix);
    if (sceneFile != nullptr) {
        sceneMap = nlohmann::json::parse(sceneFile->Buffer, nullptr, true, true);
    }
    
    auto miscFile = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("accessibility/texts/misc" + languageSuffix);
    if (miscFile != nullptr) {
        miscMap = nlohmann::json::parse(miscFile->Buffer, nullptr, true, true);
    }
    
    auto kaleidoFile = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("accessibility/texts/kaleidoscope" + languageSuffix);
    if (kaleidoFile != nullptr) {
        kaleidoMap = nlohmann::json::parse(kaleidoFile->Buffer, nullptr, true, true);
    }
    
    auto fileChooseFile = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("accessibility/texts/filechoose" + languageSuffix);
    if (fileChooseFile != nullptr) {
        fileChooseMap = nlohmann::json::parse(fileChooseFile->Buffer, nullptr, true, true);
    }
}

void RegisterOnSetGameLanguageHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSetGameLanguage>([]() {
        InitAccessibilityTexts();
    });
}

void RegisterAccessibilityModHooks() {
    RegisterOnSetGameLanguageHook();
    RegisterOnDialogMessageHook();
    RegisterOnSceneInitHook();
    RegisterOnPresentTitleCardHook();
    RegisterOnInterfaceUpdateHook();
    RegisterOnKaleidoscopeUpdateHook();
    RegisterOnUpdateMainMenuSelection();
}

void InitAccessibility() {
    InitAccessibilityTexts();
    RegisterAccessibilityModHooks();
}

#include "soh/Enhancements/game-interactor/GameInteractor.h"

#include <OtrFile.h>
#include <libultraship/bridge.h>
#include <libultraship/classes.h>
#include <nlohmann/json.hpp>

#include "soh/OTRGlobals.h"
#include "message_data_static.h"

extern "C" {
extern PlayState* gPlayState;
}

typedef enum {
    /* 0x00 */ TEXT_BANK_SCENES,
    /* 0x01 */ TEXT_BANK_UNITS,
} TextBank;

nlohmann::json sceneMap = nullptr;
nlohmann::json unitsMap = nullptr;

std::string GetParameritizedText(std::string key, TextBank bank, const char* arg) {
    switch (bank) {
        case TEXT_BANK_SCENES: {
            return sceneMap[key].get<std::string>();
            break;
        }
        case TEXT_BANK_UNITS: {
            auto value = unitsMap[key].get<std::string>();
            
            std::string searchString = "$0";
            size_t index = value.find(searchString);
            
            if (index != std::string::npos) {
                value.replace(index, searchString.size(), std::string(arg));
                return value;
            } else {
                return value;
            }
            
            break;
        }
    }
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
        titleCardText = NameForSceneId(sceneNum);
    });
}

void RegisterOnPresentTitleCardHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPresentTitleCard>([]() {
        SpeechSynthesizerSpeak(titleCardText);
    });
}

// MARK: - Interface Updates

void RegisterOnInterfaceUpdateHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnInterfaceUpdate>([]() {
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
                    auto translation = GetParameritizedText((minutes > 1) ? "minutes_plural" : "minutes_singular", TEXT_BANK_UNITS, arg);
                    announceBuf += snprintf(announceBuf, sizeof(ttsAnnounceBuf), "%s ", translation.c_str());
                }
                if (seconds > 0) {
                    snprintf(arg, sizeof(arg), "%d", seconds);
                    auto translation = GetParameritizedText((seconds > 1) ? "seconds_plural" : "seconds_singular", TEXT_BANK_UNITS, arg);
                    announceBuf += snprintf(announceBuf, sizeof(ttsAnnounceBuf), "%s", translation.c_str());
                }
                ASSERT(announceBuf < ttsAnnounceBuf + sizeof(ttsAnnounceBuf));
                SpeechSynthesizerSpeak(strdup(ttsAnnounceBuf));
                prevTimer = timer;
            }
        }
        
        prevTimer = timer;
        
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

// MARK: - Dialog Messages

static uint8_t ttsHasMessage;
static uint8_t ttsHasNewMessage;
static char ttsMessageBuf[256];
static int8_t ttsCurrentHighlightedChoice;

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
            destBuf[destWriteIndex++] = cchar;
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
                SpeechSynthesizerSpeak(ttsMessageBuf);
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
                        SpeechSynthesizerSpeak(ttsMessageBuf);
                    }
                }
            }
        } else if (ttsHasMessage) {
            ttsHasMessage = 0;
            ttsHasNewMessage = 0;
            
            if (msgCtx->decodedTextLen < 3 || (msgCtx->msgBufDecoded[msgCtx->decodedTextLen - 2] != MESSAGE_FADE && msgCtx->msgBufDecoded[msgCtx->decodedTextLen - 3] != MESSAGE_FADE2)) {
                SpeechSynthesizerSpeak(""); // cancel current speech (except for faded out messages)
            }
        }
    });
}


// MARK: - Main Registration

void InitAccessibilityTexts() {
    std::string languageSuffix = "_eng.json";
    switch (gSaveContext.language) {
        case LANGUAGE_FRA:
            languageSuffix = "_fra.json";
            break;
        case LANGUAGE_GER:
            languageSuffix = "_ger.json";
            break;
    }

    auto sceneFile = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("accessibility/texts/scenes" + languageSuffix);
    if (sceneFile == nullptr || sceneMap != nullptr) {
        return;
    }
    sceneMap = nlohmann::json::parse(sceneFile->Buffer);
    
    auto unitsFile = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("accessibility/texts/units" + languageSuffix);
    if (unitsFile == nullptr || unitsMap != nullptr) {
        return;
    }
    unitsMap = nlohmann::json::parse(unitsFile->Buffer);
}

void RegisterAccessibilityModHooks() {
    RegisterOnDialogMessageHook();
    RegisterOnSceneInitHook();
    RegisterOnPresentTitleCardHook();
    RegisterOnInterfaceUpdateHook();
}

void InitAccessibility() {
    InitAccessibilityTexts();
    RegisterAccessibilityModHooks();
}

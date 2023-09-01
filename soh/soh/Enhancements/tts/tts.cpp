#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/speechsynthesizer/SpeechSynthesizer.h"

#include <cassert>
#include <File.h>
#include <libultraship/classes.h>
#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>

#include "soh/OTRGlobals.h"
#include "message_data_static.h"
#include "overlays/gamestates/ovl_file_choose/file_choose.h"
#include "soh/Enhancements/boss-rush/BossRush.h"

extern "C" {
extern MapData* gMapData;
extern SaveContext gSaveContext;
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
            return sceneMap[key].get<std::string>();
            break;
        }
        case TEXT_BANK_MISC: {
            auto value = miscMap[key].get<std::string>();
            
            std::string searchString = "$0";
            size_t index = value.find(searchString);
            
            if (index != std::string::npos) {
                assert(arg != nullptr);
                value.replace(index, searchString.size(), std::string(arg));
                return value;
            } else {
                return value;
            }
            
            break;
        }
        case TEXT_BANK_KALEIDO: {
            auto value = kaleidoMap[key].get<std::string>();
            
            std::string searchString = "$0";
            size_t index = value.find(searchString);
            
            if (index != std::string::npos) {
                assert(arg != nullptr);
                value.replace(index, searchString.size(), std::string(arg));
                return value;
            } else {
                return value;
            }
            
            break;
        }
        case TEXT_BANK_FILECHOOSE: {
            auto value = fileChooseMap[key].get<std::string>();

            std::string searchString = "$0";
            size_t index = value.find(searchString);

            if (index != std::string::npos) {
                assert(arg != nullptr);
                value.replace(index, searchString.size(), std::string(arg));
                return value;
            } else {
                return value;
            }

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

std::string NameForSceneId(int16_t sceneId) {
    auto key = std::to_string(sceneId);
    auto name = GetParameritizedText(key, TEXT_BANK_SCENES, nullptr);
    return name;
}

static std::string titleCardText;

void RegisterOnSceneInitHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        titleCardText = NameForSceneId(sceneNum);
    });
}

void RegisterOnPresentTitleCardHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPresentTitleCard>([]() {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        SpeechSynthesizer::Instance->Speak(titleCardText.c_str(), GetLanguageCode());
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
                assert(announceBuf < ttsAnnounceBuf + sizeof(ttsAnnounceBuf));
                SpeechSynthesizer::Instance->Speak(ttsAnnounceBuf, GetLanguageCode());
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

        static int16_t prevCursorIndex = 0;
        static uint16_t prevCursorSpecialPos = 0;
        static uint16_t prevCursorPoint[5] = { 0 };
        static int16_t prevPromptChoice = -1;
        static int16_t prevSubState = -1;
        static int16_t prevState = -1;

        PauseContext* pauseCtx = &gPlayState->pauseCtx;
        Input* input = &gPlayState->state.input[0];

        // Save game prompt
        if (pauseCtx->state == 7) {
            if (pauseCtx->unk_1EC == 1) {
                // prompt
                if (prevPromptChoice != pauseCtx->promptChoice) {
                    auto prompt = GetParameritizedText(pauseCtx->promptChoice == 0 ? "yes" : "no", TEXT_BANK_MISC, nullptr);
                    if (prevPromptChoice == -1) {
                        auto translation = GetParameritizedText("save_prompt", TEXT_BANK_KALEIDO, nullptr);
                        SpeechSynthesizer::Instance->Speak((translation + " - " + prompt).c_str(), GetLanguageCode());
                    } else {
                        SpeechSynthesizer::Instance->Speak(prompt.c_str(), GetLanguageCode());
                    }

                    prevPromptChoice = pauseCtx->promptChoice;
                }
            } else if (pauseCtx->unk_1EC == 4 && prevSubState != 4) {
                // Saved
                auto translation = GetParameritizedText("game_saved", TEXT_BANK_KALEIDO, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
            }
            prevSubState = pauseCtx->unk_1EC;
            prevState = pauseCtx->state;
            return;
        }

        // Announce page when
        // Kaleido pages are rotating and page halfway rotated
        // Or Kaleido was just opened
        if ((pauseCtx->unk_1E4 == 1 && pauseCtx->unk_1EA == 32) || (pauseCtx->state == 4 && prevState != 4)) {
            uint16_t modeNextPageMap[] = {
                PAUSE_MAP, PAUSE_EQUIP, PAUSE_QUEST, PAUSE_ITEM, PAUSE_EQUIP, PAUSE_MAP, PAUSE_ITEM, PAUSE_QUEST,
            };
            uint16_t nextPage = modeNextPageMap[pauseCtx->mode];

            switch (nextPage) {
                case PAUSE_ITEM: {
                    auto translation = GetParameritizedText("item_menu", TEXT_BANK_KALEIDO, nullptr);
                    SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                    break;
                }
                case PAUSE_MAP: {
                    std::string map;
                    if (inDungeonScene) {
                        std::string key = std::to_string(gSaveContext.mapIndex);
                        map = GetParameritizedText(key, TEXT_BANK_SCENES, nullptr);
                    } else {
                        map = GetParameritizedText("overworld", TEXT_BANK_KALEIDO, nullptr);
                    }
                    auto translation = GetParameritizedText("map_menu", TEXT_BANK_KALEIDO, map.c_str());
                    SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                    break;
                }
                case PAUSE_QUEST: {
                    auto translation = GetParameritizedText("quest_menu", TEXT_BANK_KALEIDO, nullptr);
                    SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                    break;
                }
                case PAUSE_EQUIP: {
                    auto translation = GetParameritizedText("equip_menu", TEXT_BANK_KALEIDO, nullptr);
                    SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                    break;
                }
            }
            prevState = pauseCtx->state;
            return;
        }

        prevState = pauseCtx->state;

        if (pauseCtx->state != 6) {
            // Reset cursor index and values so it is announced when pause is reopened
            prevCursorIndex = -1;
            prevPromptChoice = -1;
            prevSubState = -1;
            return;
        }

        if ((pauseCtx->debugState != 1) && (pauseCtx->debugState != 2)) {
            char arg[8];
            if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                // Normalize hearts to fractional count similar to z_lifemeter
                int curHeartFraction = gSaveContext.health % 16;
                int fullHearts = gSaveContext.health / 16;
                float fraction = ceilf((float)curHeartFraction / 5) * 0.25;
                float health = (float)fullHearts + fraction;
                snprintf(arg, sizeof(arg), "%g", health);
                auto translation = GetParameritizedText("health", TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT) && gSaveContext.magicCapacity != 0) {
                // Normalize magic to percentage
                float magicLevel = ((float)gSaveContext.magic / gSaveContext.magicCapacity) * 100;
                snprintf(arg, sizeof(arg), "%.0f%%", magicLevel);
                auto translation = GetParameritizedText("magic", TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                snprintf(arg, sizeof(arg), "%d", gSaveContext.rupees);
                auto translation = GetParameritizedText("rupees", TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
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

        std::string buttonNames[] = {
            "input_button_c_left",
            "input_button_c_down",
            "input_button_c_right",
            "input_d_pad_up",
            "input_d_pad_down",
            "input_d_pad_left",
            "input_d_pad_right",
        };
        int8_t assignedTo = -1;
        
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
                    case ITEM_BEAN:
                        snprintf(arg, sizeof(arg), "%d", AMMO(pauseCtx->cursorItem[PAUSE_ITEM]));
                        break;
                    default:
                        arg[0] = '\0';
                }

                if (pauseCtx->cursorItem[PAUSE_ITEM] == PAUSE_ITEM_NONE ||
                    pauseCtx->cursorItem[PAUSE_ITEM] == ITEM_NONE) {
                    prevCursorIndex = -1;
                    return;
                }

                std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_ITEM]);
                std::string itemTranslation = GetParameritizedText(key, TEXT_BANK_KALEIDO, arg);

                // Check if item is assigned to a button
                for (size_t i = 0; i < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); i++) {
                    if (gSaveContext.equips.buttonItems[i + 1] == pauseCtx->cursorItem[PAUSE_ITEM]) {
                        assignedTo = i;
                        break;
                    }
                }

                if (assignedTo != -1) {
                    auto button = GetParameritizedText(buttonNames[assignedTo], TEXT_BANK_MISC, nullptr);
                    auto translation = GetParameritizedText("assigned_to", TEXT_BANK_KALEIDO, button.c_str());
                    SpeechSynthesizer::Instance->Speak((itemTranslation + " - " + translation).c_str(), GetLanguageCode());
                } else {
                    SpeechSynthesizer::Instance->Speak(itemTranslation.c_str(), GetLanguageCode());
                }
                break;
            }
            case PAUSE_MAP:
                if (inDungeonScene) {
                    // Dungeon map items
                    if (pauseCtx->cursorItem[PAUSE_MAP] != PAUSE_ITEM_NONE) {
                        std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_MAP]);
                        auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, nullptr);
                        SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                    } else {
                        // Dungeon map floor numbers
                        char arg[8];
                        int cursorPoint = pauseCtx->cursorPoint[PAUSE_MAP];

                        // Cursor is on a dungeon floor position
                        if (cursorPoint >= 3 && cursorPoint < 11) {
                            int floorID = gMapData->floorID[gPlayState->interfaceCtx.unk_25A][pauseCtx->dungeonMapSlot - 3];
                            // Normalize so F1 == 0, and negative numbers are basement levels
                            int normalizedFloor = (floorID * -1) + 8;
                            if (normalizedFloor >= 0) {
                                snprintf(arg, sizeof(arg), "%d", normalizedFloor + 1);
                                auto translation = GetParameritizedText("floor", TEXT_BANK_KALEIDO, arg);
                                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                            } else {
                                snprintf(arg, sizeof(arg), "%d", normalizedFloor * -1);
                                auto translation = GetParameritizedText("basement", TEXT_BANK_KALEIDO, arg);
                                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                            }
                        }
                    }
                } else {
                    std::string key = std::to_string(0x0100 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP]);
                    auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, nullptr);
                    SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
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
                        snprintf(arg, sizeof(arg), "%d", (gSaveContext.inventory.questItems & 0xF0000000) >> 0x1C);
                        break;
                    default:
                        arg[0] = '\0';
                }

                if (pauseCtx->cursorItem[PAUSE_QUEST] == PAUSE_ITEM_NONE) {
                    prevCursorIndex = -1;
                    return;
                }

                std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_QUEST]);
                auto translation = GetParameritizedText(key, TEXT_BANK_KALEIDO, arg);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case PAUSE_EQUIP:
            {
                if (pauseCtx->namedItem == PAUSE_ITEM_NONE) {
                    prevCursorIndex = -1;
                    return;
                }

                std::string key = std::to_string(pauseCtx->cursorItem[PAUSE_EQUIP]);
                auto itemTranslation = GetParameritizedText(key, TEXT_BANK_KALEIDO, nullptr);
                uint8_t checkEquipItem = pauseCtx->namedItem;

                // BGS from kaleido reports as ITEM_HEART_PIECE_2 (122)
                // remap BGS and broken knife to be the BGS item for the current equip check
                if (checkEquipItem == ITEM_HEART_PIECE_2 || checkEquipItem == ITEM_SWORD_KNIFE) {
                    checkEquipItem = ITEM_SWORD_BGS;
                }

                // Check if equipment item is currently equipped or assigned to a button
                if (checkEquipItem >= ITEM_SWORD_KOKIRI && checkEquipItem <= ITEM_BOOTS_HOVER) {
                    uint8_t checkEquipType = (checkEquipItem - ITEM_SWORD_KOKIRI) / 3;
                    uint8_t checkEquipValue = ((checkEquipItem - ITEM_SWORD_KOKIRI) % 3) + 1;

                    if (CUR_EQUIP_VALUE(checkEquipType) == checkEquipValue) {
                        itemTranslation = GetParameritizedText("equipped", TEXT_BANK_KALEIDO, itemTranslation.c_str());
                    }

                    for (size_t i = 0; i < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); i++) {
                        if (gSaveContext.equips.buttonItems[i + 1] == checkEquipItem) {
                            assignedTo = i;
                            break;
                        }
                    }
                }

                if (assignedTo != -1) {
                    auto button = GetParameritizedText(buttonNames[assignedTo], TEXT_BANK_MISC, nullptr);
                    auto translation = GetParameritizedText("assigned_to", TEXT_BANK_KALEIDO, button.c_str());
                    SpeechSynthesizer::Instance->Speak((itemTranslation + " - " + translation).c_str(), GetLanguageCode());
                } else {
                    SpeechSynthesizer::Instance->Speak(itemTranslation.c_str(), GetLanguageCode());
                }
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
        SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
    });
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileSelectSelection>([](uint16_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_BTN_MAIN_FILE_1: {
                auto translation = GetParameritizedText("file1", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_FILE_2: {
                auto translation = GetParameritizedText("file2", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_FILE_3: {
                auto translation = GetParameritizedText("file3", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_OPTIONS: {
                auto translation = GetParameritizedText("options", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_COPY: {
                auto translation = GetParameritizedText("copy", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_MAIN_ERASE: {
                auto translation = GetParameritizedText("erase", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileSelectConfirmationSelection>([](uint16_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;

        switch (optionIndex) {
            case FS_BTN_CONFIRM_YES: {
                auto translation = GetParameritizedText("confirm", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_CONFIRM_QUIT: {
                auto translation = GetParameritizedText("quit", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileCopySelection>([](uint16_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_BTN_COPY_FILE_1: {
                auto translation = GetParameritizedText("file1", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_COPY_FILE_2: {
                auto translation = GetParameritizedText("file2", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_COPY_FILE_3: {
                auto translation = GetParameritizedText("file3", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_COPY_QUIT: {
                auto translation = GetParameritizedText("quit", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileCopyConfirmationSelection>([](uint16_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_BTN_CONFIRM_YES: {
                auto translation = GetParameritizedText("confirm", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_CONFIRM_QUIT: {
                auto translation = GetParameritizedText("quit", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileEraseSelection>([](uint16_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_BTN_ERASE_FILE_1: {
                auto translation = GetParameritizedText("file1", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_ERASE_FILE_2: {
                auto translation = GetParameritizedText("file2", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_ERASE_FILE_3: {
                auto translation = GetParameritizedText("file3", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_ERASE_QUIT: {
                auto translation = GetParameritizedText("quit", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileEraseConfirmationSelection>([](uint16_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_BTN_CONFIRM_YES: {
                auto translation = GetParameritizedText("confirm", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_BTN_CONFIRM_QUIT: {
                auto translation = GetParameritizedText("quit", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileAudioSelection>([](uint8_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_AUDIO_STEREO: {
                auto translation = GetParameritizedText("audio_stereo", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_AUDIO_MONO: {
                auto translation = GetParameritizedText("audio_mono", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_AUDIO_HEADSET: {
                auto translation = GetParameritizedText("audio_headset", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_AUDIO_SURROUND: {
                auto translation = GetParameritizedText("audio_surround", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });
    
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileTargetSelection>([](uint8_t optionIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;
        
        switch (optionIndex) {
            case FS_TARGET_SWITCH: {
                auto translation = GetParameritizedText("target_switch", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_TARGET_HOLD: {
                auto translation = GetParameritizedText("target_hold", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileQuestSelection>([](uint8_t questIndex) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;

        switch (questIndex) {
            case FS_QUEST_NORMAL: {
                auto translation = GetParameritizedText("quest_sel_vanilla", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_QUEST_MASTER: {
                auto translation = GetParameritizedText("quest_sel_mq", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_QUEST_RANDOMIZER: {
                auto translation = GetParameritizedText("quest_sel_randomizer", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            case FS_QUEST_BOSSRUSH: {
                auto translation = GetParameritizedText("quest_sel_boss_rush", TEXT_BANK_FILECHOOSE, nullptr);
                SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
                break;
            }
            default:
                break;
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileBossRushOptionSelection>([](uint8_t optionIndex, uint8_t optionValue) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;

        auto optionName = BossRush_GetSettingName(optionIndex, gSaveContext.language);
        auto optionValueName = BossRush_GetSettingChoiceName(optionIndex, optionValue, gSaveContext.language);
        auto translation = optionName + std::string(" - ") + optionValueName;
        SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnUpdateFileNameSelection>([](int16_t charCode) {
        if (!CVarGetInteger("gA11yTTS", 0)) return;

        char charVal[2];
        std::string translation;

        if (charCode < 10) { // Digits
            sprintf(charVal, "%c", charCode + 0x30);
        } else if (charCode >= 10 && charCode < 36) { // Uppercase letters
            sprintf(charVal, "%c", charCode + 0x37);
            translation = GetParameritizedText("capital_letter", TEXT_BANK_FILECHOOSE, charVal);
        } else if (charCode >= 36 && charCode < 62) { // Lowercase letters
            sprintf(charVal, "%c", charCode + 0x3D);
        } else if (charCode == 62) { // Space
            translation = GetParameritizedText("space", TEXT_BANK_FILECHOOSE, nullptr);
        } else if (charCode == 63) { // -
            translation = GetParameritizedText("hyphen", TEXT_BANK_FILECHOOSE, nullptr);
        } else if (charCode == 64) { // .
            translation = GetParameritizedText("period", TEXT_BANK_FILECHOOSE, nullptr);
        } else if (charCode == 0xF0 + FS_KBD_BTN_BACKSPACE) {
            translation = GetParameritizedText("backspace", TEXT_BANK_FILECHOOSE, nullptr);
        } else if (charCode == 0xF0 + FS_KBD_BTN_END) {
            translation = GetParameritizedText("end", TEXT_BANK_FILECHOOSE, nullptr);
        } else {
            sprintf(charVal, "%c", charCode);
        }

        if (translation.empty()) {
            SpeechSynthesizer::Instance->Speak(charVal, GetLanguageCode());
        } else {
            SpeechSynthesizer::Instance->Speak(translation.c_str(), GetLanguageCode());
        }
    });
}

// MARK: - Dialog Messages

static uint8_t ttsHasMessage;
static uint8_t ttsHasNewMessage;
static int8_t ttsCurrentHighlightedChoice;

std::string remap(uint8_t character) {
    switch (character) {
        case 0x80: return "À";
        case 0x81: return "î";
        case 0x82: return "Â";
        case 0x83: return "Ä";
        case 0x84: return "Ç";
        case 0x85: return "È";
        case 0x86: return "É";
        case 0x87: return "Ê";
        case 0x88: return "Ë";
        case 0x89: return "Ï";
        case 0x8A: return "Ô";
        case 0x8B: return "Ö";
        case 0x8C: return "Ù";
        case 0x8D: return "Û";
        case 0x8E: return "Ü";
        case 0x8F: return "ß";
        case 0x90: return "à";
        case 0x91: return "á";
        case 0x92: return "â";
        case 0x93: return "ä";
        case 0x94: return "ç";
        case 0x95: return "è";
        case 0x96: return "é";
        case 0x97: return "ê";
        case 0x98: return "ë";
        case 0x99: return "ï";
        case 0x9A: return "ô";
        case 0x9B: return "ö";
        case 0x9C: return "ù";
        case 0x9D: return "û";
        case 0x9E: return "ü";
        case 0x9F: return GetParameritizedText("input_button_a", TEXT_BANK_MISC, nullptr);
        case 0xA0: return GetParameritizedText("input_button_b", TEXT_BANK_MISC, nullptr);
        case 0xA1: return GetParameritizedText("input_button_c", TEXT_BANK_MISC, nullptr);
        case 0xA2: return GetParameritizedText("input_button_l", TEXT_BANK_MISC, nullptr);
        case 0xA3: return GetParameritizedText("input_button_r", TEXT_BANK_MISC, nullptr);
        case 0xA4: return GetParameritizedText("input_button_z", TEXT_BANK_MISC, nullptr);
        case 0xA5: return GetParameritizedText("input_button_c_up", TEXT_BANK_MISC, nullptr);
        case 0xA6: return GetParameritizedText("input_button_c_down", TEXT_BANK_MISC, nullptr);
        case 0xA7: return GetParameritizedText("input_button_c_left", TEXT_BANK_MISC, nullptr);
        case 0xA8: return GetParameritizedText("input_button_c_right", TEXT_BANK_MISC, nullptr);
        case 0xAA: return GetParameritizedText("input_analog_stick", TEXT_BANK_MISC, nullptr);
        case 0xAB: return GetParameritizedText("input_d_pad", TEXT_BANK_MISC, nullptr);
        default: return "";
    }
}

std::string Message_TTS_Decode(uint8_t* sourceBuf, uint16_t startOfset, uint16_t size) {
    std::string output;
    uint32_t destWriteIndex = 0;
    uint8_t isListingChoices = 0;
    
    for (uint16_t i = 0; i < size; i++) {
        uint8_t cchar = sourceBuf[i + startOfset];
        
        if (cchar < ' ') {
            switch (cchar) {
                case MESSAGE_NEWLINE:
                    output += (isListingChoices) ? '\n' : ' ';
                    break;
                case MESSAGE_THREE_CHOICE:
                case MESSAGE_TWO_CHOICE:
                    output += '\n';
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
                output += cchar;
            } else {
                output += remap(cchar);
            }
        }
    }
    
    return output;
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
                auto decodedMsg = Message_TTS_Decode(msgCtx->msgBufDecoded, 0, size);
                SpeechSynthesizer::Instance->Speak(decodedMsg.c_str(), GetLanguageCode());
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
                        auto decodedMsg = Message_TTS_Decode(msgCtx->msgBufDecoded, startOffset, size);
                        SpeechSynthesizer::Instance->Speak(decodedMsg.c_str(), GetLanguageCode());
                    }
                }
            }
        } else if (ttsHasMessage) {
            ttsHasMessage = 0;
            ttsHasNewMessage = 0;
            
            if (msgCtx->decodedTextLen < 3 || (msgCtx->msgBufDecoded[msgCtx->decodedTextLen - 2] != MESSAGE_FADE && msgCtx->msgBufDecoded[msgCtx->decodedTextLen - 3] != MESSAGE_FADE2)) {
                SpeechSynthesizer::Instance->Speak("", GetLanguageCode()); // cancel current speech (except for faded out messages)
            }
        }
    });
}

// MARK: - Main Registration

void InitTTSBank() {
    std::string languageSuffix = "_eng.json";
    switch (CVarGetInteger("gLanguages", 0)) {
        case LANGUAGE_FRA:
            languageSuffix = "_fra.json";
            break;
        case LANGUAGE_GER:
            languageSuffix = "_ger.json";
            break;
    }

    auto sceneFile = LUS::Context::GetInstance()->GetResourceManager()->LoadFile("accessibility/texts/scenes" + languageSuffix);
    if (sceneFile != nullptr) {
        sceneMap = nlohmann::json::parse(sceneFile->Buffer, nullptr, true, true);
    }
    
    auto miscFile = LUS::Context::GetInstance()->GetResourceManager()->LoadFile("accessibility/texts/misc" + languageSuffix);
    if (miscFile != nullptr) {
        miscMap = nlohmann::json::parse(miscFile->Buffer, nullptr, true, true);
    }
    
    auto kaleidoFile = LUS::Context::GetInstance()->GetResourceManager()->LoadFile("accessibility/texts/kaleidoscope" + languageSuffix);
    if (kaleidoFile != nullptr) {
        kaleidoMap = nlohmann::json::parse(kaleidoFile->Buffer, nullptr, true, true);
    }
    
    auto fileChooseFile = LUS::Context::GetInstance()->GetResourceManager()->LoadFile("accessibility/texts/filechoose" + languageSuffix);
    if (fileChooseFile != nullptr) {
        fileChooseMap = nlohmann::json::parse(fileChooseFile->Buffer, nullptr, true, true);
    }
}

void RegisterOnSetGameLanguageHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSetGameLanguage>([]() {
        InitTTSBank();
    });
}

void RegisterTTSModHooks() {
    RegisterOnSetGameLanguageHook();
    RegisterOnDialogMessageHook();
    RegisterOnSceneInitHook();
    RegisterOnPresentTitleCardHook();
    RegisterOnInterfaceUpdateHook();
    RegisterOnKaleidoscopeUpdateHook();
    RegisterOnUpdateMainMenuSelection();
}

void RegisterTTS() {
    InitTTSBank();
    RegisterTTSModHooks();
}

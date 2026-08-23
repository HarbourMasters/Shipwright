#include "debugSaveEditor.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/randomizer/item.h"
#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/Enhancements/randomizer/randomizerEnums/RandomizerGet.h"
#include "soh/Enhancements/randomizer/randomizerEnums/RandomizerInf.h"
#include "soh/util.h"
#include "soh/SohGui/ImGuiUtils.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SaveManager.h"
#include "soh/ResourceManagerHelpers.h"

#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#include <algorithm>
#include <array>
#include <bit>
#include <map>
#include <string>
#include <soh_assets.h>

#include <fast/Fast3dGui.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

#include "message_data_static.h"
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;
extern "C" MessageTableEntry* sJpnMessageEntryTablePtr;

// Maps entries in the GS flag array to the area name it represents
std::vector<const char*> gsMapping = {
    "Deku Tree",
    "Dodongo's Cavern",
    "Inside Jabu-Jabu's Belly",
    "Forest Temple",
    "Fire Temple",
    "Water Temple",
    "Spirit Temple",
    "Shadow Temple",
    "Bottom of the Well",
    "Ice Cavern",
    "Hyrule Field",
    "Lon Lon Ranch",
    "Kokiri Forest",
    "Lost Woods, Sacred Forest Meadow",
    "Castle Town and Ganon's Castle",
    "Death Mountain Trail, Goron City",
    "Kakariko Village",
    "Zora Fountain, River",
    "Lake Hylia",
    "Gerudo Valley",
    "Gerudo Fortress",
    "Desert Colossus, Haunted Wasteland",
};

const char* MAGIC_LEVEL_NAMES[3] = { "No Magic", "Single Magic", "Double Magic" };
constexpr int8_t MAGIC_LEVEL_MAX = 2;

const char* WALLET_LEVEL_NAMES[4] = { "Child Wallet (99)", "Adult Wallet (200)", "Giant Wallet (500)",
                                      "Tycoon Wallet (999)" };
constexpr int8_t WALLET_LEVEL_MAX = 4;

extern "C" u8 gAreaGsFlags[];

extern "C" u8 gAmmoItems[];

#define IMAGE_SIZE 40.0f

// Increment value for ImGui::InputScalar calls
static const ImU32 INPUT_ONE = 1;

// Fishing high score bit positions (gSaveContext.highScores[2])
namespace FishingBits {
constexpr uint32_t CHILD_SIZE_MASK = 0x0000007F;
constexpr uint32_t CHILD_CHEATED_MASK = 0x00000080;
constexpr uint32_t CHILD_PLAYED_MASK = 0x00000100;
constexpr uint32_t ADULT_PLAYED_MASK = 0x00000200;
constexpr uint32_t CHILD_PRIZE_MASK = 0x00000400;
constexpr uint32_t ADULT_PRIZE_MASK = 0x00000800;
constexpr uint32_t STOLE_HAT_MASK = 0x00001000;
constexpr uint32_t TIMES_PLAYED_SHIFT = 16;
constexpr uint32_t TIMES_PLAYED_MASK = 0x00FF0000;
constexpr uint32_t ADULT_SIZE_SHIFT = 24;
constexpr uint32_t ADULT_SIZE_MASK = 0x7F000000;
constexpr uint32_t ADULT_CHEATED_MASK = 0x80000000;
} // namespace FishingBits

using namespace UIWidgets;

// Helper function to get the correct display name for items, including rando items
static const char* GetItemDisplayName(int32_t item) {
    if (item == ITEM_NONE) {
        return "None";
    }
    if (item < 0 || item > ITEM_ROCS_FEATHER) {
        static char unknownName[32];
        snprintf(unknownName, sizeof(unknownName), "Unknown (0x%02X)", item);
        return unknownName;
    }
    return SohUtils::GetItemName(item).c_str();
}

IntSliderOptions intSliderOptionsBase;
ButtonOptions buttonOptionsBase;
CheckboxOptions checkboxOptionsBase;
ComboboxOptions comboboxOptionsBase;
static std::map<std::string, ImGuiTextFilter> flagTableFilters;

// Modification of gAmmoItems that replaces ITEM_NONE with the item in inventory slot it represents
u8 gAllAmmoItems[] = {
    ITEM_STICK,     ITEM_NUT,          ITEM_BOMB,    ITEM_BOW,      ITEM_ARROW_FIRE, ITEM_DINS_FIRE,
    ITEM_SLINGSHOT, ITEM_OCARINA_TIME, ITEM_BOMBCHU, ITEM_LONGSHOT, ITEM_ARROW_ICE,  ITEM_FARORES_WIND,
    ITEM_BOOMERANG, ITEM_LENS,         ITEM_BEAN,    ITEM_HAMMER,
};

// Encapsulates what is drawn by the passed-in function within a border
template <typename T> void DrawGroupWithBorder(T&& drawFunc, std::string section) {
    // First group encapsulates the inner portion and border
    ImGui::BeginChild(std::string("##" + section).c_str(), ImVec2(0, 0),
                      ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX |
                          ImGuiChildFlags_AutoResizeY);

    ImGui::BeginGroup();
    ImGui::AlignTextToFramePadding();
    drawFunc();
    ImGui::EndGroup();

    ImGui::EndChild();
}

// Maximum small keys obtainable per dungeon. Delegates to the rando DungeonInfo
// for vanilla/MQ small key counts (synced to the loaded save on load via SaveManager).
// Returns 0 for dungeons without small keys (e.g. Deku Tree, Ganon's Tower).
static int8_t GetMaxKeysForDungeon(int32_t dungeonIndex) {
    Rando::DungeonInfo* dungeon = Rando::Context::GetInstance()->GetDungeons()->GetDungeonFromScene(dungeonIndex);
    if (dungeon == nullptr) {
        return 0; // Not a tracked key dungeon — no small keys.
    }
    return static_cast<int8_t>(dungeon->GetSmallKeyCount());
}

// Check if a dungeon has a boss key
static bool DungeonHasBossKey(int32_t dungeonIndex) {
    switch (dungeonIndex) {
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_GANONS_TOWER: // Boss key opens door to Ganondorf
            return true;
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
        case SCENE_GERUDO_TRAINING_GROUND:
        case SCENE_INSIDE_GANONS_CASTLE:
        default:
            return false;
    }
}

// Helper to draw a button item selector box (exact same pattern as inventory picker)
// buttonIndex: index into buttonItems array (0=B, 1=C-Left, 2=C-Down, 3=C-Right, 4-7=D-pad)
// isBButton: true for B button (allows swords only), false for C/D-pad (button-usable items)
// restrictToValid: pointer to shared restrict flag (nullptr = use internal static)
static void DrawButtonItemSelector(const char* label, int buttonIndex, UIWidgets::Colors color, bool isBButton = false,
                                   const bool* restrictToValidPtr = nullptr) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    // Use provided restrictToValid or default to true (restricted mode)
    bool useRestriction = restrictToValidPtr ? *restrictToValidPtr : true;
    uint8_t* buttonItem = &gSaveContext.equips.buttonItems[buttonIndex];

    // Helper to check if item is a sword (for B button)
    auto isSword = [](int32_t item) -> bool {
        return item == ITEM_SWORD_KOKIRI || item == ITEM_SWORD_MASTER || item == ITEM_SWORD_BGS ||
               item == ITEM_SWORD_KNIFE;
    };

    // Helper to check if item is button-usable (for C/D-pad buttons)
    // Restricted to items from Stick (0x00) to Bow Arrow Light (0x3A)
    auto isButtonUsable = [](int32_t item) -> bool {
        if (item == ITEM_ROCS_FEATHER)
            return true;
        // All button-usable items: ITEM_STICK (0x00) to ITEM_BOW_ARROW_LIGHT (0x3A)
        if (item >= ITEM_STICK && item <= ITEM_BOW_ARROW_LIGHT)
            return true;
        return false;
    };

    // Helper to check if an item should be shown in the picker based on mode and button type
    auto shouldShowItem = [&](int32_t item) -> bool {
        if (item == ITEM_NONE)
            return false;
        if (item == ITEM_ROCS_FEATHER)
            return !isBButton; // Roc's Feather on C/D-pad only
        if (!useRestriction)
            return true; // Unrestricted: show everything
        if (isBButton)
            return isSword(item);    // B button restricted: swords only
        return isButtonUsable(item); // C/D-pad restricted: button-usable items only
    };

    uint8_t item = *buttonItem;
    // Use DarkGray for empty buttons, otherwise use the provided color
    if (item != ITEM_NONE) {
        PushStyleButton(color);
    } else {
        PushStyleButton(Colors::DarkGray);
    }

    if (item == ITEM_ROCS_FEATHER) {
        std::string rocId = std::string("RG_ROCS_FEATHER_btn_") + label;
        if (ImGui::ImageButton(rocId.c_str(), gui->GetTextureByName("RG_ROCS_FEATHER"), ImVec2(IMAGE_SIZE, IMAGE_SIZE),
                               ImVec2(0, 0), ImVec2(1, 1))) {
            ImGui::OpenPopup(label);
        }
    } else if (const auto mappedItem = itemMapping.find(item); item != ITEM_NONE && mappedItem != itemMapping.end()) {
        const ItemMapEntry& slotEntry = mappedItem->second;
        // Use label-based ID to avoid conflicts when same item is on multiple buttons
        std::string itemId = std::string("item_btn_") + label + "_" + slotEntry.name;
        if (ImGui::ImageButton(itemId.c_str(), gui->GetTextureByName(slotEntry.name), ImVec2(IMAGE_SIZE, IMAGE_SIZE),
                               ImVec2(0, 0), ImVec2(1, 1))) {
            ImGui::OpenPopup(label);
        }
    } else {
        // Use label-based ID for empty buttons to avoid conflicts
        std::string emptyId = std::string("##btnEmpty_") + label;
        if (ImGui::Button(emptyId.c_str(), ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
            ImGui::OpenPopup(label);
        }
    }
    PopStyleButton();

    // Tooltip with current item name
    if (item != ITEM_NONE) {
        Tooltip(GetItemDisplayName(item));
    } else {
        Tooltip("Empty");
    }

    // Popup selector (exact same pattern as inventory picker)
    if (ImGui::BeginPopup(label)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        PushStyleButton(Colors::DarkGray);
        std::string noneId = std::string("##btnNonePicker_") + label;
        if (ImGui::Button(noneId.c_str(), ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
            *buttonItem = ITEM_NONE;
            // Update cButtonSlots for C buttons
            if (buttonIndex >= 1 && buttonIndex <= 3) {
                gSaveContext.equips.cButtonSlots[buttonIndex - 1] = SLOT_NONE;
            }
            // Refresh HUD, pause menu, and player actor
            if (gPlayState != nullptr) {
                Interface_LoadItemIcon1(gPlayState, buttonIndex);
                // Update player actor equipment when B button changes
                if (buttonIndex == 0) {
                    Player* player = GET_PLAYER(gPlayState);
                    if (player != nullptr) {
                        player->currentSwordItemId = static_cast<s8>(ITEM_NONE);
                    }
                }
                // Refresh pause menu if open
                if (gPlayState->pauseCtx.state != 0) {
                    KaleidoScope_InitVertices(gPlayState, gPlayState->state.gfxCtx);
                }
            }
            ImGui::CloseCurrentPopup();
        }
        PopStyleButton();
        UIWidgets::Tooltip("None");

        std::vector<ItemMapEntry> possibleItems;
        // Use shouldShowItem helper to filter items based on mode and button type
        for (const auto& [itemId, entry] : itemMapping) {
            if (shouldShowItem(itemId)) {
                possibleItems.push_back(entry);
            }
        }

        for (size_t pickerIndex = 0; pickerIndex < possibleItems.size(); pickerIndex++) {
            if (((pickerIndex + 1) % 8) != 0) {
                ImGui::SameLine();
            }

            const ItemMapEntry& slotEntry = possibleItems[pickerIndex];
            PushStyleButton(Colors::DarkGray);
            std::string pickerItemId = std::string("item_picker_") + label + "_" + slotEntry.name;
            auto ret = ImGui::ImageButton(pickerItemId.c_str(), gui->GetTextureByName(slotEntry.name),
                                          ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
            PopStyleButton();
            if (ret) {
                *buttonItem = slotEntry.id;
                // Update cButtonSlots for C buttons to point to the correct inventory slot
                if (buttonIndex >= 1 && buttonIndex <= 3) {
                    gSaveContext.equips.cButtonSlots[buttonIndex - 1] = SLOT(slotEntry.id);
                }
                // Refresh HUD, pause menu, and player actor
                if (gPlayState != nullptr) {
                    Interface_LoadItemIcon1(gPlayState, buttonIndex);
                    // Update player actor equipment when B button changes
                    if (buttonIndex == 0) {
                        Player* player = GET_PLAYER(gPlayState);
                        if (player != nullptr) {
                            // Check item type and update corresponding equipment
                            if ((slotEntry.id >= ITEM_SWORD_KOKIRI && slotEntry.id <= ITEM_SWORD_BGS) ||
                                slotEntry.id == ITEM_SWORD_BROKEN) {
                                player->currentSwordItemId = slotEntry.id;
                            } else if (slotEntry.id >= ITEM_SHIELD_DEKU && slotEntry.id <= ITEM_SHIELD_MIRROR) {
                                if (slotEntry.id == ITEM_SHIELD_DEKU)
                                    player->currentShield = PLAYER_SHIELD_DEKU;
                                else if (slotEntry.id == ITEM_SHIELD_HYLIAN)
                                    player->currentShield = PLAYER_SHIELD_HYLIAN;
                                else if (slotEntry.id == ITEM_SHIELD_MIRROR)
                                    player->currentShield = PLAYER_SHIELD_MIRROR;
                            } else if (slotEntry.id >= ITEM_TUNIC_KOKIRI && slotEntry.id <= ITEM_TUNIC_ZORA) {
                                if (slotEntry.id == ITEM_TUNIC_KOKIRI)
                                    player->currentTunic = PLAYER_TUNIC_KOKIRI;
                                else if (slotEntry.id == ITEM_TUNIC_GORON)
                                    player->currentTunic = PLAYER_TUNIC_GORON;
                                else if (slotEntry.id == ITEM_TUNIC_ZORA)
                                    player->currentTunic = PLAYER_TUNIC_ZORA;
                            } else if (slotEntry.id >= ITEM_BOOTS_KOKIRI && slotEntry.id <= ITEM_BOOTS_HOVER) {
                                if (slotEntry.id == ITEM_BOOTS_KOKIRI)
                                    player->currentBoots = PLAYER_BOOTS_KOKIRI;
                                else if (slotEntry.id == ITEM_BOOTS_IRON)
                                    player->currentBoots = PLAYER_BOOTS_IRON;
                                else if (slotEntry.id == ITEM_BOOTS_HOVER)
                                    player->currentBoots = PLAYER_BOOTS_HOVER;
                            }
                        }
                    }
                    if (gPlayState->pauseCtx.state != 0) {
                        KaleidoScope_InitVertices(gPlayState, gPlayState->state.gfxCtx);
                    }
                }
                ImGui::CloseCurrentPopup();
            }
            UIWidgets::Tooltip(GetItemDisplayName(slotEntry.id));
        }

        // Add Roc's Feather at the end for C/D-pad buttons in restricted mode
        if (shouldShowItem(ITEM_ROCS_FEATHER)) {
            ImGui::SameLine();
            PushStyleButton(Colors::DarkGray);
            std::string rocPickerId = std::string("RG_ROCS_FEATHER_picker_") + label;
            auto retRoc = ImGui::ImageButton(rocPickerId.c_str(), gui->GetTextureByName("RG_ROCS_FEATHER"),
                                             ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
            PopStyleButton();
            if (retRoc) {
                *buttonItem = ITEM_ROCS_FEATHER;
                // Update cButtonSlots for C buttons (Roc's Feather uses Nayru's Love slot)
                if (buttonIndex >= 1 && buttonIndex <= 3) {
                    gSaveContext.equips.cButtonSlots[buttonIndex - 1] = SLOT_NAYRUS_LOVE;
                }
                // Refresh button icon on HUD and pause menu
                if (gPlayState != nullptr) {
                    Interface_LoadItemIcon1(gPlayState, buttonIndex);
                    if (gPlayState->pauseCtx.state != 0) {
                        KaleidoScope_InitVertices(gPlayState, gPlayState->state.gfxCtx);
                    }
                }
                ImGui::CloseCurrentPopup();
            }
            UIWidgets::Tooltip("Roc's Feather");
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
}

char z2ASCII(int code) {
    int ret;
    if (code < 10) { // Digits
        ret = code + 0x30;
    } else if (code >= 10 && code < 36) { // Uppercase letters
        ret = code + 0x37;
    } else if (code >= 36 && code < 62) { // Lowercase letters
        ret = code + 0x3D;
    } else if (code == 62) { // Space
        ret = code - 0x1E;
    } else if (code == 63 || code == 64) { // _ and .
        ret = code - 0x12;
    } else {
        ret = code;
    }
    return char(ret);
}

std::string decodeNTSCPlayerNameChar(int code) {
    const std::string charmap[] = {
        "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  // 10
        "あ", "い", "う", "え", "お", "か", "き", "く", "け", "こ", // 20
        "さ", "し", "す", "せ", "そ", "た", "ち", "つ", "て", "と", // 30
        "な", "に", "ぬ", "ね", "の", "は", "ひ", "ふ", "へ", "ほ", // 40
        "ま", "み", "む", "め", "も", "や", "ゆ", "よ", "ら", "り", // 50
        "る", "れ", "ろ", "わ", "を", "ん", "ぁ", "ぃ", "ぅ", "ぇ", // 60
        "ぉ", "っ", "ゃ", "ゅ", "ょ", "が", "ぎ", "ぐ", "げ", "ご", // 70
        "ざ", "じ", "ず", "ぜ", "ぞ", "だ", "ぢ", "づ", "で", "ど", // 80
        "ば", "び", "ぶ", "べ", "ぼ", "ぱ", "ぴ", "ぷ", "ぺ", "ぽ", // 90
        "ア", "イ", "ウ", "エ", "オ", "カ", "キ", "ク", "ケ", "コ", // 100
        "サ", "シ", "ス", "セ", "ソ", "タ", "チ", "ツ", "テ", "ト", // 110
        "ナ", "ニ", "ヌ", "ネ", "ノ", "ハ", "ヒ", "フ", "ヘ", "ホ", // 120
        "マ", "ミ", "ム", "メ", "モ", "ヤ", "ユ", "ヨ", "ラ", "リ", // 130
        "ル", "レ", "ロ", "ワ", "ヲ", "ン", "ァ", "ィ", "ゥ", "ェ", // 140
        "ォ", "ッ", "ャ", "ュ", "ョ", "ガ", "ギ", "グ", "ゲ", "ゴ", // 150
        "ザ", "ジ", "ズ", "ゼ", "ゾ", "ダ", "ヂ", "ヅ", "デ", "ド", // 160
        "バ", "ビ", "ブ", "ベ", "ボ", "パ", "ピ", "プ", "ペ", "ポ", // 170
        "ヴ",
    };
    std::string ret;

    if (code < 171) { // Digits and Japanese
        ret = charmap[code];
    } else if (code >= 171 && code < 197) { // Uppercase letters
        ret.assign(1, (char)(code - 171 + 65));
    } else if (code >= 197 && code < 223) { // Lowercase letters
        ret.assign(1, (char)(code - 197 + 97));
    } else if (code == 223) { // Space
        ret = " ";
    } else if (code == 228) { // -
        ret = "-";
    } else if (code == 234) { // .
        ret = ".";
    } else {
        ret = "?";
    }

    return ret;
}

enum MagicLevel { MAGIC_LEVEL_NONE, MAGIC_LEVEL_SINGLE, MAGIC_LEVEL_DOUBLE };

std::map<int8_t, const char*> magicLevelMap = {
    { MAGIC_LEVEL_NONE, "None" },
    { MAGIC_LEVEL_SINGLE, "Single" },
    { MAGIC_LEVEL_DOUBLE, "Double" },
};

enum AudioOutput {
    AUDIO_STEREO,
    AUDIO_MONO,
    AUDIO_HEADSET,
    AUDIO_SURROUND,
};

std::map<uint8_t, const char*> audioMap = {
    { AUDIO_STEREO, "Stereo" },
    { AUDIO_MONO, "Mono" },
    { AUDIO_HEADSET, "Headset" },
    { AUDIO_SURROUND, "Surround" },
};

enum ZTarget {
    Z_TARGET_SWITCH,
    Z_TARGET_HOLD,
};

std::map<uint8_t, const char*> zTargetMap = {
    { Z_TARGET_SWITCH, "Switch" },
    { Z_TARGET_HOLD, "Hold" },
};

std::map<int32_t, const char*> fileNumMap = {
    { 0, "File 1" },
    { 1, "File 2" },
    { 2, "File 3" },
};

std::map<uint8_t, const char*> filenameLanguageMap = {
    { NAME_LANGUAGE_PAL, "PAL" },
    { NAME_LANGUAGE_NTSC_JPN, "NTSC JPN" },
    { NAME_LANGUAGE_NTSC_ENG, "NTSC ENG" },
};

std::map<uint8_t, const char*> filenameLanguageMapNTSCOnly = {
    { NAME_LANGUAGE_NTSC_JPN, "NTSC JPN" },
    { NAME_LANGUAGE_NTSC_ENG, "NTSC ENG" },
};

void DrawGeneralTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("generalTab", ImVec2(0, 0), true);

    UIWidgets::BeginCardLayout(
        { .columnsPerRow = 2, .minColumnWidth = 350.0f, .fixedColumnWidths = { 450.0f, 450.0f } });

    UIWidgets::BeginCard("identityCard");

    // Show status message when not in game
    bool canEditName = true;
    if (gSaveContext.gameMode == GAMEMODE_TITLE_SCREEN) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Title Screen");
        canEditName = false;
    } else if (gSaveContext.gameMode == GAMEMODE_FILE_SELECT) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "File Select");
        canEditName = false;
    } else if (gPlayState == nullptr) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Game Inactive");
        canEditName = false;
    } else {
        // Show file number as text when in game
        const char* fileName =
            (gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2) ? fileNumMap[gSaveContext.fileNum] : "Debug";
        ImGui::Text("File: %s", fileName);
    }

    ImGui::Spacing();

    // Player name input - only editable when file is loaded
    static char playerNameInput[9] = "";
    static bool nameInputInitialized = false;

    if (!canEditName) {
        // Read-only display when no file is loaded
        ImGui::BeginDisabled();
        ImGui::InputText("##PlayerNameDisabled", playerNameInput, 9);
        ImGui::EndDisabled();
    } else {
        // Convert current name to ASCII for display
        if (!nameInputInitialized || ImGui::IsWindowFocused()) {
            for (int i = 0; i < 8; i++) {
                if (gSaveContext.ship.filenameLanguage == NAME_LANGUAGE_PAL) {
                    playerNameInput[i] = z2ASCII(gSaveContext.playerName[i]);
                } else {
                    const std::string decoded = decodeNTSCPlayerNameChar(gSaveContext.playerName[i]);
                    playerNameInput[i] =
                        (decoded.length() == 1 && decoded[0] >= 32 && decoded[0] <= 126) ? decoded[0] : ' ';
                }
            }
            playerNameInput[8] = '\0';
            nameInputInitialized = true;
        }

        PushStyleInput(THEME_COLOR);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
        if (ImGui::InputText("##PlayerNameInput", playerNameInput, 9)) {
            // Convert ASCII input back to game encoding
            for (int i = 0; i < 8; i++) {
                if (playerNameInput[i] == '\0') {
                    gSaveContext.playerName[i] = 223;
                } else {
                    gSaveContext.playerName[i] = playerNameInput[i];
                }
            }
            nameInputInitialized = false;
        }
        ImGui::PopStyleVar(2);
        PopStyleInput();
    }

    ImGui::Spacing();

    // Filename encoding
    const bool hasPAL = (sGerMessageEntryTablePtr != nullptr) && (sFraMessageEntryTablePtr != nullptr);
    const bool hasNTSC = (sJpnMessageEntryTablePtr != nullptr);
    if (hasPAL && hasNTSC) {
        Combobox("Player Name Language", &gSaveContext.ship.filenameLanguage, filenameLanguageMap,
                 comboboxOptionsBase.Tooltip("Encoding used for Player Name"));
    } else if (hasNTSC && (gSaveContext.ship.filenameLanguage != NAME_LANGUAGE_PAL)) {
        Combobox("Player Name Language", &gSaveContext.ship.filenameLanguage, filenameLanguageMapNTSCOnly,
                 comboboxOptionsBase.Tooltip("Encoding used for Player Name"));
    } else {
        // PAL only (read only)
        ImGui::BeginDisabled();
        Combobox("Player Name Language", &gSaveContext.ship.filenameLanguage, filenameLanguageMap,
                 comboboxOptionsBase.Tooltip("Encoding used for Player Name"));
        ImGui::EndDisabled();
    }
    UIWidgets::EndCard();

    UIWidgets::BeginCard("healthCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Health");
    ImGui::Spacing();

    if (Button("Max Health",
               ButtonOptions().Size(ImVec2(0, 0)).Color(Colors::Red).Tooltip("Set max health and double defense"))) {
        gSaveContext.isDoubleDefenseAcquired = 1;
        gSaveContext.inventory.defenseHearts = 20;
        gSaveContext.healthCapacity = gSaveContext.health = 20 * 16;
    }
    ImGui::SameLine();
    if (Button("Reset", ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR).Tooltip("Reset health to default"))) {
        gSaveContext.isDoubleDefenseAcquired = 0;
        gSaveContext.inventory.defenseHearts = 0;
        gSaveContext.healthCapacity = gSaveContext.health = 3 * 16;
    }
    ImGui::SameLine();
    bool isDoubleDefenseAcquired = gSaveContext.isDoubleDefenseAcquired != 0;
    if (Checkbox("DD", &isDoubleDefenseAcquired, CheckboxOptions().Color(Colors::DarkRed).Tooltip("Double Defense"))) {
        gSaveContext.isDoubleDefenseAcquired = isDoubleDefenseAcquired;
        gSaveContext.inventory.defenseHearts = isDoubleDefenseAcquired ? 20 : 0;
    }

    PushStyleSlider(Colors::DarkRed);
    static const int16_t HEART_COUNT_MIN = 3;
    static const int16_t HEART_COUNT_MAX = 20;
    static const int16_t S16_ZERO = 0;
    int16_t heartCount = (int16_t)gSaveContext.healthCapacity / 16;
    if (ImGui::SliderScalar("##heartCountSlider", ImGuiDataType_S16, &heartCount, &HEART_COUNT_MIN, &HEART_COUNT_MAX,
                            "Max Hearts: %d")) {
        gSaveContext.healthCapacity = heartCount * 16;
        if (gSaveContext.health > gSaveContext.healthCapacity) {
            gSaveContext.health = gSaveContext.healthCapacity;
        }
    }
    int16_t health = (int16_t)gSaveContext.health;
    int16_t healthMax = gSaveContext.healthCapacity;
    ImGui::SliderScalar("##healthSlider", ImGuiDataType_S16, &health, &S16_ZERO, &healthMax, "Health: %d");
    if (ImGui::IsItemEdited()) {
        gSaveContext.health = health;
    }
    PopStyleSlider();
    UIWidgets::EndCard();

    UIWidgets::BeginCard("magicCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Magic");
    ImGui::Spacing();

    if (Button("Max Magic", ButtonOptions().Size(ImVec2(0, 0)).Color(Colors::DarkGreen).Tooltip("Set max magic"))) {
        gSaveContext.magicCapacity = gSaveContext.magic = 0x60;
        gSaveContext.magicLevel = 2;
        gSaveContext.isMagicAcquired = true;
        gSaveContext.isDoubleMagicAcquired = true;
    }
    ImGui::SameLine();
    if (Button("Reset", ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR).Tooltip("Reset magic"))) {
        gSaveContext.magicCapacity = gSaveContext.magic = 0;
        gSaveContext.magicLevel = 0;
        gSaveContext.isMagicAcquired = false;
        gSaveContext.isDoubleMagicAcquired = false;
    }
    ImGui::SameLine();
    bool hasMagic = gSaveContext.isMagicAcquired;
    Checkbox("Has Magic", &hasMagic, CheckboxOptions().Color(Colors::DarkGreen).Tooltip("Magic acquired"));
    if (ImGui::IsItemEdited()) {
        gSaveContext.isMagicAcquired = hasMagic;
        if (!hasMagic) {
            gSaveContext.magicLevel = 0;
            gSaveContext.magicCapacity = gSaveContext.magic = 0;
        }
    }

    PushStyleSlider(Colors::DarkGreen);
    static const int8_t S8_ZERO = 0;
    static const int8_t MAGIC_LEVEL_MAX_VAR = MAGIC_LEVEL_MAX;
    int8_t magicLevel = gSaveContext.magicLevel;
    if (ImGui::SliderScalar("##magicLevelSlider", ImGuiDataType_S8, &magicLevel, &S8_ZERO, &MAGIC_LEVEL_MAX_VAR,
                            MAGIC_LEVEL_NAMES[gSaveContext.magicLevel])) {
        gSaveContext.magicLevel = magicLevel;
        gSaveContext.isMagicAcquired = gSaveContext.magicLevel > 0;
        gSaveContext.isDoubleMagicAcquired = gSaveContext.magicLevel == 2;
        gSaveContext.magicCapacity = gSaveContext.magicLevel * 0x30;
        if (gSaveContext.magic > gSaveContext.magicCapacity) {
            gSaveContext.magic = static_cast<s8>(gSaveContext.magicCapacity);
        }
    }
    int8_t magic = (int8_t)gSaveContext.magic;
    int8_t magicMax = static_cast<int8_t>(gSaveContext.magicCapacity);
    ImGui::SliderScalar("##magicSlider", ImGuiDataType_S8, &magic, &S8_ZERO, &magicMax, "Magic: %d");
    if (ImGui::IsItemEdited()) {
        gSaveContext.magic = magic;
    }
    PopStyleSlider();
    UIWidgets::EndCard();

    UIWidgets::BeginCard("rupeesCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Rupees");
    ImGui::Spacing();

    int8_t walletLevelMax = (IS_RANDO) ? WALLET_LEVEL_MAX : WALLET_LEVEL_MAX - 1;
    if (Button("Max Rupees",
               ButtonOptions().Size(ImVec2(0, 0)).Color(Colors::Green).Tooltip("Set max rupees and wallet"))) {
        Inventory_ChangeUpgrade(UPG_WALLET, walletLevelMax);
        gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
    }
    ImGui::SameLine();
    if (Button("Reset", ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR).Tooltip("Reset rupees and wallet"))) {
        gSaveContext.rupees = 0;
        Inventory_ChangeUpgrade(UPG_WALLET, 0);
    }

    PushStyleSlider(Colors::Green);
    static const int8_t U8_ZERO = 0;
    int8_t walletLevelLocalMax = walletLevelMax;
    int8_t walletLevel = CUR_UPG_VALUE(UPG_WALLET);
    if (ImGui::SliderScalar("##walletLevelSlider", ImGuiDataType_S8, &walletLevel, &U8_ZERO, &walletLevelLocalMax,
                            WALLET_LEVEL_NAMES[walletLevel])) {
        Inventory_ChangeUpgrade(UPG_WALLET, walletLevel);
        int16_t maxRupees = CUR_CAPACITY(UPG_WALLET);
        if (gSaveContext.rupees > maxRupees) {
            gSaveContext.rupees = maxRupees;
        }
    }
    int16_t maxRupees = CUR_CAPACITY(UPG_WALLET);
    int16_t rupees = (int16_t)gSaveContext.rupees;
    ImGui::SliderScalar("##rupeesSlider", ImGuiDataType_S16, &rupees, &S16_ZERO, &maxRupees, "Rupees: %d");
    if (ImGui::IsItemEdited()) {
        gSaveContext.rupees = rupees;
    }
    PopStyleSlider();
    UIWidgets::EndCard();

    UIWidgets::BeginCard("timeCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Time");
    ImGui::Spacing();

    SliderInt("Time of Day", (int32_t*)&gSaveContext.dayTime,
              IntSliderOptions().Min(0).Max(0xFFFF).Format("%d").Tooltip("Time of day"));

    if (Button("Dawn", ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR))) {
        gSaveContext.dayTime = 0x4000;
    }
    ImGui::SameLine();
    if (Button("Noon", ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR))) {
        gSaveContext.dayTime = 0x8000;
    }
    ImGui::SameLine();
    if (Button("Sunset", ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR))) {
        gSaveContext.dayTime = 0xC001;
    }
    ImGui::SameLine();
    if (Button("Midnight", ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR))) {
        gSaveContext.dayTime = 0;
    }

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##TotalDays", ImGuiDataType_S32, &gSaveContext.totalDays);
    Tooltip("Total number of days elapsed since the start of the game");
    PopStyleInput();
    ImGui::SameLine();
    ImGui::Text("Days");
    UIWidgets::EndCard();

    UIWidgets::BeginCard("equipmentCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Equipment");
    ImGui::Spacing();

    ImGui::Text("Deaths");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##Deaths", ImGuiDataType_U16, &gSaveContext.deaths);
    Tooltip("Total number of deaths");
    PopStyleInput();

    Checkbox("Has BGS", (bool*)&gSaveContext.bgsFlag,
             CheckboxOptions().Color(THEME_COLOR).Tooltip("Is Biggoron sword unlocked?"));

    ImGui::Text("Sword Health");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##SwordHealth", ImGuiDataType_U16, &gSaveContext.swordHealth);
    Tooltip("Giant's knife health. Default is 8. Must be >0 for Biggoron sword to work");
    PopStyleInput();

    ImGui::Text("BGS Days");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##BgsDayCount", ImGuiDataType_S32, &gSaveContext.bgsDayCount);
    Tooltip("Total number of days elapsed since receiving claim check from Biggoron");
    PopStyleInput();
    UIWidgets::EndCard();

    UIWidgets::BeginCard("locationCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Location");
    ImGui::Spacing();

    ImGui::Text("Entrance Index");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##EntranceIndex", ImGuiDataType_S32, &gSaveContext.entranceIndex);
    Tooltip("From which entrance did Link arrive?");
    PopStyleInput();

    ImGui::Text("Cutscene Index");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##CutsceneIndex", ImGuiDataType_S32, &gSaveContext.cutsceneIndex);
    Tooltip("Which cutscene is this?");
    PopStyleInput();
    UIWidgets::EndCard();

    UIWidgets::BeginCard("timersCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Timers");
    ImGui::Spacing();

    ImGui::Text("Navi Timer");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##NaviTimer", ImGuiDataType_U16, &gSaveContext.naviTimer);
    Tooltip("Navi wants to talk at 600 units, decides not to at 3000.");
    PopStyleInput();

    ImGui::Text("Timer State");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##TimerState", ImGuiDataType_S16, &gSaveContext.timerState);
    Tooltip("Heat timer, race timer, etc. Has white font");
    PopStyleInput();

    ImGui::Text("Timer Seconds");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##TimerSeconds", ImGuiDataType_S16, &gSaveContext.timerSeconds, &INPUT_ONE, NULL);
    Tooltip("Time, in seconds");
    PopStyleInput();

    ImGui::Text("Sub Timer State");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##SubTimerState", ImGuiDataType_S16, &gSaveContext.subTimerState);
    Tooltip("Trade timer, Ganon collapse timer, etc. Has yellow font");
    PopStyleInput();

    ImGui::Text("Sub Timer Seconds");
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("##SubTimerSeconds", ImGuiDataType_S16, &gSaveContext.subTimerSeconds, &INPUT_ONE, NULL);
    Tooltip("Time, in seconds");
    PopStyleInput();
    UIWidgets::EndCard();

    UIWidgets::BeginCard("settingsCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Settings");
    ImGui::Spacing();

    Combobox("Audio", &gSaveContext.audioSetting, audioMap, comboboxOptionsBase.Tooltip("Sound setting"));

    Checkbox("64 DD file?", (bool*)&gSaveContext.n64ddFlag,
             CheckboxOptions().Color(Colors::Orange).Tooltip("WARNING! If you save, your file may be locked!"));

    Combobox("Z Target Mode", &gSaveContext.zTargetSetting, zTargetMap,
             comboboxOptionsBase.Tooltip("Z-Targeting behavior"));

    // Triforce Pieces editor moved to the "Randomizer Specific" card in DrawEquipmentTab().
    UIWidgets::EndCard();

    UIWidgets::BeginCard("minigamesCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Minigames");
    ImGui::Spacing();

    static std::array<const char*, 7> minigameHS = { "Horseback Archery", "Big Poe Points",
                                                     "Fishing",           "Malon's Obstacle Course",
                                                     "Running Man Race",  "?",
                                                     "Dampe's Race" };

    for (int i = 0; i < 7; i++) {
        if (i == 2) {
            if (ImGui::TreeNode("Fishing")) {
                ImGui::Text("Child Size");
                u8 fishSize = gSaveContext.highScores[i] & FishingBits::CHILD_SIZE_MASK;
                PushStyleInput(THEME_COLOR);
                if (ImGui::InputScalar("##ChildSize", ImGuiDataType_U8, &fishSize)) {
                    gSaveContext.highScores[i] &= ~FishingBits::CHILD_SIZE_MASK;
                    gSaveContext.highScores[i] |= fishSize & FishingBits::CHILD_SIZE_MASK;
                }
                char fishMsg[64];
                std::snprintf(fishMsg, 64, "Weight: %2.0f lbs", ((SQ(fishSize) * .0036) + .5));
                Tooltip(fishMsg);
                PopStyleInput();

                bool FishBool = gSaveContext.highScores[i] & FishingBits::CHILD_CHEATED_MASK;
                if (Checkbox("Cheated (Child)", &FishBool,
                             CheckboxOptions().Color(THEME_COLOR).Tooltip("Used the Sinking lure"))) {
                    gSaveContext.highScores[i] &= ~FishingBits::CHILD_CHEATED_MASK;
                    gSaveContext.highScores[i] |= (FishingBits::CHILD_CHEATED_MASK * FishBool);
                }

                ImGui::Text("Adult Size");
                fishSize = (gSaveContext.highScores[i] & FishingBits::ADULT_SIZE_MASK) >> FishingBits::ADULT_SIZE_SHIFT;
                PushStyleInput(THEME_COLOR);
                if (ImGui::InputScalar("##AdultSize", ImGuiDataType_U8, &fishSize)) {
                    gSaveContext.highScores[i] &= ~FishingBits::ADULT_SIZE_MASK;
                    gSaveContext.highScores[i] |= (fishSize & FishingBits::CHILD_SIZE_MASK)
                                                  << FishingBits::ADULT_SIZE_SHIFT;
                }
                std::snprintf(fishMsg, 64, "Weight: %2.0f lbs", ((SQ(fishSize) * .0036) + .5));
                Tooltip(fishMsg);
                PopStyleInput();

                FishBool = gSaveContext.highScores[i] & FishingBits::ADULT_CHEATED_MASK;
                if (Checkbox("Cheated (Adult)", &FishBool,
                             CheckboxOptions().Color(THEME_COLOR).Tooltip("Used the Sinking lure"))) {
                    gSaveContext.highScores[i] &= ~FishingBits::ADULT_CHEATED_MASK;
                    gSaveContext.highScores[i] |= (FishingBits::ADULT_CHEATED_MASK * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & FishingBits::CHILD_PLAYED_MASK;
                if (Checkbox("Played (Child)", &FishBool,
                             CheckboxOptions().Color(THEME_COLOR).Tooltip("Played as child"))) {
                    gSaveContext.highScores[i] &= ~FishingBits::CHILD_PLAYED_MASK;
                    gSaveContext.highScores[i] |= (FishingBits::CHILD_PLAYED_MASK * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & FishingBits::ADULT_PLAYED_MASK;
                if (Checkbox("Played (Adult)", &FishBool,
                             CheckboxOptions().Color(THEME_COLOR).Tooltip("Played as adult"))) {
                    gSaveContext.highScores[i] &= ~FishingBits::ADULT_PLAYED_MASK;
                    gSaveContext.highScores[i] |= (FishingBits::ADULT_PLAYED_MASK * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & FishingBits::CHILD_PRIZE_MASK;
                if (Checkbox("Got Prize (Child)", &FishBool, CheckboxOptions().Color(THEME_COLOR))) {
                    gSaveContext.highScores[i] &= ~FishingBits::CHILD_PRIZE_MASK;
                    gSaveContext.highScores[i] |= (FishingBits::CHILD_PRIZE_MASK * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & FishingBits::ADULT_PRIZE_MASK;
                if (Checkbox("Got Prize (Adult)", &FishBool, CheckboxOptions().Color(THEME_COLOR))) {
                    gSaveContext.highScores[i] &= ~FishingBits::ADULT_PRIZE_MASK;
                    gSaveContext.highScores[i] |= (FishingBits::ADULT_PRIZE_MASK * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & FishingBits::STOLE_HAT_MASK;
                if (Checkbox("Stole Owner's Hat", &FishBool, CheckboxOptions().Color(THEME_COLOR))) {
                    gSaveContext.highScores[i] &= ~FishingBits::STOLE_HAT_MASK;
                    gSaveContext.highScores[i] |= (FishingBits::STOLE_HAT_MASK * FishBool);
                }
                ImGui::Text("Times Played");
                fishSize =
                    (gSaveContext.highScores[i] & FishingBits::TIMES_PLAYED_MASK) >> FishingBits::TIMES_PLAYED_SHIFT;
                PushStyleInput(THEME_COLOR);
                if (ImGui::InputScalar("##TimesPlayed", ImGuiDataType_U8, &fishSize)) {
                    gSaveContext.highScores[i] &= ~FishingBits::TIMES_PLAYED_MASK;
                    gSaveContext.highScores[i] |= (fishSize) << FishingBits::TIMES_PLAYED_SHIFT;
                }
                Tooltip("Determines weather and school size during dawn/dusk.");
                PopStyleInput();

                ImGui::TreePop();
            }
        } else if (i != 5) { // HS_UNK_05 is unused
            std::string minigameLbl = minigameHS[i];
            std::string minigameId = "##minigame" + std::to_string(i);
            ImGui::Text("%s", minigameLbl.c_str());
            PushStyleInput(THEME_COLOR);
            ImGui::InputScalar(minigameId.c_str(), ImGuiDataType_S32, &gSaveContext.highScores[i], &INPUT_ONE, NULL);
            PopStyleInput();
        }
    }

    UIWidgets::EndCard();
    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void DrawBGSItemFlag(uint8_t itemID) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    const ItemMapEntry& slotEntry = itemMapping[itemID];
    ImGui::Image(gui->GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1));
}

// Re-sync any C/D-pad button that mirrors an edited inventory slot (buttonItems[i] == items[cButtonSlots[i-1]]),
// so a raw item edit doesn't leave the button showing stale contents.
static void SyncButtonItemsForSlot(uint8_t slot) {
    for (size_t i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
        if (gSaveContext.equips.cButtonSlots[i - 1] == slot) {
            gSaveContext.equips.buttonItems[i] = gSaveContext.inventory.items[slot];
            if (gPlayState != nullptr) {
                Interface_LoadItemIcon1(gPlayState, static_cast<u16>(i));
            }
        }
    }
}

void DrawInventoryTab() {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("inventoryTab", ImVec2(0, 0), true);

    UIWidgets::BeginCardLayout(
        { .columnsPerRow = 2, .minColumnWidth = 350.0f, .fixedColumnWidths = { 450.0f, 450.0f } });

    // Items grid card (force to column 0)
    UIWidgets::BeginCard("inventoryGridCard", 0);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Items");
    ImGui::Spacing();

    static InventorySlot selectedIndex = SLOT_NONE;
    static const char* itemPopupPicker = "itemPopupPicker";
    static bool restrictToValid = true;

    // Check if D-pad is enabled for border coloring
    bool dpadEnabled = CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0);

    static bool syncButtons = true;
    Checkbox("Keep C/D-pad buttons in sync", &syncButtons,
             checkboxOptionsBase.Tooltip("Refresh a C or D-pad button when its inventory slot is edited. Disable to "
                                         "leave a slot and its button out of sync (e.g. to set up RBA)."));

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 6; x++) {
            static_assert(5 + 3 * 6 < sizeof(gSaveContext.inventory.items) / sizeof(gSaveContext.inventory.items[0]));
            InventorySlot index = static_cast<InventorySlot>(x + y * 6);

            ImGui::PushID(index);

            if (x != 0) {
                ImGui::SameLine();
            }

            uint8_t item = gSaveContext.inventory.items[index];

            // Check if this item is equipped and which slot for colored border
            enum EquippedSlot { None, BButton, CButton, DPad };
            EquippedSlot equippedSlot = EquippedSlot::None;

            // Check C button slots (cButtonSlots maps to inventory slot indices)
            for (int cBtn = 0; cBtn < 4; cBtn++) {
                if (gSaveContext.equips.cButtonSlots[cBtn] == index) {
                    equippedSlot = EquippedSlot::CButton;
                    break;
                }
            }
            // Check D-pad slots (buttonItems[4-7] - need to match the item, not slot)
            if (equippedSlot == EquippedSlot::None && dpadEnabled) {
                for (int dpadBtn = 4; dpadBtn <= 7; dpadBtn++) {
                    if (gSaveContext.equips.buttonItems[dpadBtn] == item) {
                        equippedSlot = EquippedSlot::DPad;
                        break;
                    }
                }
            }
            // For B button (buttonItems[0])
            if (equippedSlot == EquippedSlot::None && gSaveContext.equips.buttonItems[0] == item) {
                equippedSlot = EquippedSlot::BButton;
            }

            // Determine border color (needed before group for proper rendering)
            ImU32 borderColor = 0;
            bool drawBorder = equippedSlot != EquippedSlot::None;
            if (drawBorder) {
                if (equippedSlot == EquippedSlot::CButton) {
                    borderColor = IM_COL32(255, 165, 0, 255); // Orange for C-button
                } else if (equippedSlot == EquippedSlot::DPad) {
                    borderColor = IM_COL32(180, 180, 180, 255); // Light grey for D-pad
                } else {
                    // B button color (green for N64, red for GC)
                    bool isGcScheme = CVarGetInteger(CVAR_COSMETIC("DefaultColorScheme"), 0) == 1;
                    borderColor = isGcScheme ? IM_COL32(255, 0, 0, 255) : IM_COL32(0, 255, 0, 255);
                }
            }

            ImGui::BeginGroup();

            // Save cursor position for square border (around button only, not ammo)
            ImVec2 buttonPos = ImGui::GetCursorScreenPos();

            PushStyleButton(Colors::DarkGray);
            bool wasClicked = false;
            if (item == ITEM_ROCS_FEATHER) {
                wasClicked = ImGui::ImageButton("RG_ROCS_FEATHER", gui->GetTextureByName("RG_ROCS_FEATHER"),
                                                ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
            } else if (const auto mappedItem = itemMapping.find(item);
                       item != ITEM_NONE && mappedItem != itemMapping.end()) {
                const ItemMapEntry& slotEntry = mappedItem->second;
                wasClicked = ImGui::ImageButton(slotEntry.name.c_str(), gui->GetTextureByName(slotEntry.name),
                                                ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
            } else {
                wasClicked =
                    ImGui::Button("##itemNone", ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2);
            }
            PopStyleButton();

            // Draw square border around button only (not ammo)
            if (drawBorder) {
                ImVec2 buttonMin = buttonPos;
                ImVec2 buttonMax = ImVec2(buttonPos.x + IMAGE_SIZE + ImGui::GetStyle().FramePadding.x * 2,
                                          buttonPos.y + IMAGE_SIZE + ImGui::GetStyle().FramePadding.y * 2);
                ImGui::GetWindowDrawList()->AddRect(ImVec2(buttonMin.x - 2, buttonMin.y - 2),
                                                    ImVec2(buttonMax.x + 2, buttonMax.y + 2), borderColor, 0.0f, 0,
                                                    2.0f);
            }

            if (wasClicked) {
                selectedIndex = index;
                ImGui::OpenPopup(itemPopupPicker);
            }

            // Tooltip (after button, before popup)
            if (item != ITEM_NONE) {
                Tooltip(GetItemDisplayName(item));
            }

            // Show ammo input below items that have ammo
            if (item != ITEM_NONE && item != ITEM_ROCS_FEATHER) {
                // Check if this item has ammo by checking if it's in the ammo items list
                bool hasAmmo = false;
                for (uint32_t ammoIndex = 0; ammoIndex < 16; ammoIndex++) {
                    if (gAmmoItems[ammoIndex] == item) {
                        hasAmmo = true;
                        break;
                    }
                }
                if (hasAmmo) {
                    ImGui::PushItemWidth(IMAGE_SIZE);
                    PushStyleInput(THEME_COLOR);
                    ImGui::InputScalar("##ammoInput", ImGuiDataType_S8, &AMMO(item));
                    PopStyleInput();
                    ImGui::PopItemWidth();
                }
            }

            ImGui::EndGroup();

            // Item picker popup
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            if (ImGui::BeginPopup(itemPopupPicker)) {
                PushStyleButton(Colors::DarkGray);
                if (ImGui::Button("##itemNonePicker",
                                  ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
                    if (selectedIndex != SLOT_NONE) {
                        gSaveContext.inventory.items[selectedIndex] = ITEM_NONE;
                        if (syncButtons) {
                            SyncButtonItemsForSlot(selectedIndex);
                        }
                    }
                    ImGui::CloseCurrentPopup();
                }
                PopStyleButton();
                UIWidgets::Tooltip("None");

                std::vector<ItemMapEntry> possibleItems;
                bool addRocsFeather = false;
                if (restrictToValid) {
                    // Scan gItemSlots to find legal items for this slot. Bottles are a special case
                    for (int slotIndex = 0; slotIndex < 56; slotIndex++) {
                        int testIndex = (selectedIndex == SLOT_BOTTLE_1 || selectedIndex == SLOT_BOTTLE_2 ||
                                         selectedIndex == SLOT_BOTTLE_3 || selectedIndex == SLOT_BOTTLE_4)
                                            ? SLOT_BOTTLE_1
                                            : selectedIndex;
                        if (const auto mappedItem = itemMapping.find(slotIndex);
                            gItemSlots[slotIndex] == testIndex && mappedItem != itemMapping.end()) {
                            possibleItems.push_back(mappedItem->second);
                        }
                    }
                    // Add Roc's Feather to Nayru's Love slot in restricted mode
                    if (selectedIndex == SLOT_NAYRUS_LOVE) {
                        addRocsFeather = true;
                    }
                } else {
                    for (const auto& entry : itemMapping) {
                        possibleItems.push_back(entry.second);
                    }
                    // Add Roc's Feather to ALL slots in unrestricted mode
                    addRocsFeather = true;
                }

                for (size_t pickerIndex = 0; pickerIndex < possibleItems.size(); pickerIndex++) {
                    if (((pickerIndex + 1) % 8) != 0) {
                        ImGui::SameLine();
                    }
                    const ItemMapEntry& slotEntry = possibleItems[pickerIndex];
                    PushStyleButton(Colors::DarkGray);
                    auto ret = ImGui::ImageButton(slotEntry.name.c_str(), gui->GetTextureByName(slotEntry.name),
                                                  ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
                    PopStyleButton();
                    if (ret) {
                        gSaveContext.inventory.items[selectedIndex] = slotEntry.id;
                        if (syncButtons) {
                            SyncButtonItemsForSlot(selectedIndex);
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    UIWidgets::Tooltip(GetItemDisplayName(slotEntry.id));
                }

                // Add Roc's Feather at the end of the list in unrestricted mode
                if (addRocsFeather) {
                    ImGui::SameLine();
                    PushStyleButton(Colors::DarkGray);
                    auto retRoc = ImGui::ImageButton("RG_ROCS_FEATHER_PICKER", gui->GetTextureByName("RG_ROCS_FEATHER"),
                                                     ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
                    PopStyleButton();
                    if (retRoc) {
                        gSaveContext.inventory.items[selectedIndex] = ITEM_ROCS_FEATHER;
                        ImGui::CloseCurrentPopup();
                    }
                    UIWidgets::Tooltip("Roc's Feather");
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::PopID();
        }
    }

    UIWidgets::EndCard();

    // Action buttons card (force to column 0)
    UIWidgets::BeginCard("inventoryActionsCard", 0);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Actions");
    ImGui::Spacing();

    Checkbox("Restrict to valid items", &restrictToValid,
             CheckboxOptions()
                 .Color(THEME_COLOR)
                 .Tooltip("Restricts items and ammo to only what is possible to legally acquire in-game"));
    ImGui::Spacing();

    if (Button("Give All", ButtonOptions().Color(Colors::Green).Tooltip("Give all valid items for each slot"))) {
        for (int32_t y = 0; y < 4; y++) {
            for (int32_t x = 0; x < 6; x++) {
                int32_t index = x + y * 6;
                // Find the last valid item for this slot
                for (int slotIndex = 55; slotIndex >= 0; slotIndex--) {
                    if (gItemSlots[slotIndex] == index) {
                        gSaveContext.inventory.items[index] = slotIndex;
                        break;
                    }
                }
            }
        }
        // Also max out ammo
        for (uint32_t ammoIndex = 0; ammoIndex < 16; ammoIndex++) {
            uint8_t item = gAmmoItems[ammoIndex];
            if (item != ITEM_NONE && SLOT(item) >= 0) {
                gSaveContext.inventory.ammo[SLOT(item)] = 99; // Max ammo
            }
        }
    }

    ImGui::Spacing();

    if (Button("Reset", ButtonOptions().Color(Colors::Red).Tooltip("Remove all items"))) {
        for (int32_t i = 0; i < 24; i++) {
            gSaveContext.inventory.items[i] = ITEM_NONE;
        }
        // Also clear ammo
        for (uint32_t ammoIndex = 0; ammoIndex < 16; ammoIndex++) {
            uint8_t item = gAmmoItems[ammoIndex];
            if (item != ITEM_NONE && SLOT(item) >= 0) {
                gSaveContext.inventory.ammo[SLOT(item)] = 0;
            }
        }
    }

    UIWidgets::EndCard();

    // Button Items card (N64 C button layout - force to column 1)
    UIWidgets::BeginCard("buttonItemsCard", 1);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Button Items");
    ImGui::Spacing();

    // Determine button colors based on color scheme
    bool isGcScheme = CVarGetInteger(CVAR_COSMETIC("DefaultColorScheme"), 0) == 1;
    Colors bButtonColor = isGcScheme ? Colors::Red : Colors::Green;
    Colors cButtonsColor = Colors::Orange; // Yellow-orange for C buttons

    // Note: "Restrict to valid items" checkbox is in the Actions card and affects all buttons here

    // Static counter for unique placeholder IDs
    static int placeholderCounter = 0;

    // Helper to draw invisible placeholder (for C-buttons)
    auto DrawPlaceholder = [&]() {
        ImGui::PushID(placeholderCounter++);
        ImGui::InvisibleButton("##placeholder", ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2);
        ImGui::PopID();
    };

    // Helper to draw grey placeholder (for D-pad) - NOW INVISIBLE
    auto DrawGreyPlaceholder = [&]() {
        ImGui::PushID(placeholderCounter++);
        ImGui::InvisibleButton("##placeholder", ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2);
        ImGui::PopID();
    };

    placeholderCounter = 0; // Reset counter each frame

    // C-button diamond layout:
    // Row 1: B, C-Left, [placeholder], C-Right
    DrawButtonItemSelector("B Button##btnItems", 0, bButtonColor, true, &restrictToValid);
    ImGui::SameLine();
    DrawButtonItemSelector("C Left##btnItems", 1, cButtonsColor, false, &restrictToValid);
    ImGui::SameLine();
    DrawPlaceholder();
    ImGui::SameLine();
    DrawButtonItemSelector("C Right##btnItems", 3, cButtonsColor, false, &restrictToValid);

    // Row 2: [placeholder], [placeholder], C-Down
    DrawPlaceholder();
    ImGui::SameLine();
    DrawPlaceholder();
    ImGui::SameLine();
    DrawButtonItemSelector("C Down##btnItems", 2, cButtonsColor, false, &restrictToValid);

    UIWidgets::EndCard();

    // D-Pad Items card (D-Pad layout - force to column 1)
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0)) {
        UIWidgets::BeginCard("dpadItemsCard", 1);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "D-Pad Items");
        ImGui::Spacing();

        Colors dpadColor = Colors::LightGray; // Light grey for D-pad buttons (not placeholders)

        // D-pad cross layout:
        // Row 1: [invisible placeholder], D-Pad Up, [invisible placeholder]
        DrawGreyPlaceholder();
        ImGui::SameLine();
        DrawButtonItemSelector("D-pad Up##dpadItems", 4, dpadColor, false, &restrictToValid);
        ImGui::SameLine();
        DrawGreyPlaceholder();

        // Row 2: D-Pad Left, [invisible placeholder], D-Pad Right
        DrawButtonItemSelector("D-pad Left##dpadItems", 6, dpadColor, false, &restrictToValid);
        ImGui::SameLine();
        DrawGreyPlaceholder();
        ImGui::SameLine();
        DrawButtonItemSelector("D-pad Right##dpadItems", 7, dpadColor, false, &restrictToValid);

        // Row 3: [invisible placeholder], D-Pad Down
        DrawGreyPlaceholder();
        ImGui::SameLine();
        DrawButtonItemSelector("D-pad Down##dpadItems", 5, dpadColor, false, &restrictToValid);

        UIWidgets::EndCard();
    }

    // Trade quest items card (only shown in rando when trade shuffle is enabled)
    if (IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE)) {
        UIWidgets::BeginCard("tradeQuestCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Adult Trade Quest Items");
        ImGui::Spacing();
        for (int i = ITEM_POCKET_EGG; i <= ITEM_CLAIM_CHECK; i++) {
            DrawBGSItemFlag(i);
        }
        UIWidgets::EndCard();
    }

    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

// Draw flag array with tooltips for player state
template <typename T, typename N>
void DrawFlagArrayWithTooltips(const std::string& name, T& flags, Colors color, const N& names) {
    ImGui::PushID(name.c_str());
    int32_t bitCount = sizeof(T) * 8;
    for (int32_t flagIndex = 0; flagIndex < bitCount; flagIndex++) {
        if ((flagIndex % 8) != 0) {
            ImGui::SameLine();
        }
        ImGui::PushID(flagIndex);
        uint32_t bitMask = 1 << flagIndex;
        bool flag = (flags & bitMask) != 0;
        PushStyleCheckbox(color);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 3.0f));
        std::string id = spdlog::fmt_lib::format("##{}{}", name, flagIndex);
        if (ImGui::Checkbox(id.c_str(), &flag)) {
            if (flag) {
                flags |= bitMask;
            } else {
                flags &= ~bitMask;
            }
        }
        ImGui::PopStyleVar();
        PopStyleCheckbox();
        // Show tooltip with state name
        if (ImGui::IsItemHovered() && flagIndex < (int32_t)names.size()) {
            ImGui::BeginTooltip();
            ImGui::Text("Bit %d: %s", flagIndex, names[flagIndex].c_str());
            ImGui::EndTooltip();
        }
        ImGui::PopID();
    }
    ImGui::PopID();
}

// Draw a flag bitfield as a grid of checkboxes
void DrawFlagTableArray16(const FlagTable& flagTable, uint16_t row, uint16_t& flags) {
    ImGui::PushID((std::to_string(row) + flagTable.name).c_str());
    for (int32_t flagIndex = 15; flagIndex >= 0; flagIndex--) {
        ImGui::SameLine();
        ImGui::PushID(flagIndex);
        bool hasDescription = !!flagTable.flagDescriptions.contains(row * 16 + flagIndex);
        uint32_t bitMask = 1 << flagIndex;
        ImVec4 themeColor = ColorValues.at(THEME_COLOR);
        ImVec4 colorDark = { themeColor.x * 0.4f, themeColor.y * 0.4f, themeColor.z * 0.4f, themeColor.z };
        ImVec4& color = themeColor;
        if (!hasDescription) {
            color = colorDark;
        }
        PushStyleCheckbox(hasDescription ? themeColor : colorDark);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 3.0f));
        bool flag = (flags & bitMask) != 0;
        if (ImGui::Checkbox("##check", &flag)) {
            if (flag) {
                flags |= bitMask;
            } else {
                flags &= ~bitMask;
            }
        }
        ImGui::PopStyleVar();
        PopStyleCheckbox();
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            uint16_t index = row * 16 + flagIndex;
            if (hasDescription) {
                const char* desc = flagTable.flagDescriptions.at(index);
                ImGui::Text("0x%02X: %s", index, UIWidgets::WrappedText(desc, 60).c_str());
            } else {
                ImGui::Text("0x%02X: %s (Bit %d)", index, flagTable.name, flagIndex);
            }
            ImGui::EndTooltip();
        }
        ImGui::PopID();
    }

    ImGui::PopID();
}

// Helper to draw "Set All / Clear All" button pair with flag array
void DrawFlagButtons(const char* name, uint32_t& flags, UIWidgets::Colors color) {
    if (UIWidgets::Button(spdlog::fmt_lib::format("Set All##{}", name).c_str(),
                          UIWidgets::ButtonOptions().Size(ImVec2(0, 0)).Color(Colors::Green))) {
        flags = UINT32_MAX;
    }
    ImGui::SameLine();
    if (UIWidgets::Button(spdlog::fmt_lib::format("Clear All##{}", name).c_str(),
                          UIWidgets::ButtonOptions().Size(ImVec2(0, 0)).Color(Colors::Red))) {
        flags = 0;
    }
    DrawFlagArray32(name, flags, color);
}

// Helper to display active state flags
void DrawActiveStates(const std::string& label, uint32_t states, const std::vector<std::string>& names) {
    std::string active = "Active: ";
    bool hasAny = false;
    for (size_t i = 0; i < names.size() && i < 32; i++) {
        if ((states >> i) & 1) {
            if (hasAny)
                active += ", ";
            active += names[i];
            hasAny = true;
        }
    }
    if (!hasAny)
        active += "None";
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    ImGui::Text("%s", active.c_str());
    ImGui::PopStyleColor();
}

static uint16_t& GetFlagTableEntry(const FlagTable& flagTable, size_t row) {
    switch (flagTable.flagTableType) {
        case EVENT_CHECK_INF:
            return gSaveContext.eventChkInf[row];
        case ITEM_GET_INF:
            return gSaveContext.itemGetInf[row];
        case INF_TABLE:
            return gSaveContext.infTable[row];
        case EVENT_INF:
            return gSaveContext.eventInf[row];
        case RANDOMIZER_INF:
            return gSaveContext.ship.randomizerInf[row];
        default: // Shouldn't be hit
            assert(false);
            return gSaveContext.eventChkInf[row];
    }
}

static void DrawFlagTableSearchResults(const FlagTable& flagTable, ImGuiTextFilter& filter) {
    bool hasMatches = false;

    for (size_t row = 0; row < flagTable.size + 1; row++) {
        uint16_t& flags = GetFlagTableEntry(flagTable, row);

        for (int32_t flagIndex = 15; flagIndex >= 0; flagIndex--) {
            uint16_t index = static_cast<uint16_t>(row * 16 + flagIndex);
            auto descIt = flagTable.flagDescriptions.find(index);
            const char* desc = descIt != flagTable.flagDescriptions.end() ? descIt->second : "";
            std::string searchable = spdlog::fmt_lib::format("0x{:02X} {}", index, desc);
            if (!filter.PassFilter(searchable.c_str())) {
                continue;
            }

            hasMatches = true;

            ImGui::PushID(index);
            bool hasDescription = descIt != flagTable.flagDescriptions.end();
            uint32_t bitMask = 1 << flagIndex;
            ImVec4 themeColor = ColorValues.at(THEME_COLOR);
            ImVec4 colorDark = { themeColor.x * 0.4f, themeColor.y * 0.4f, themeColor.z * 0.4f, themeColor.z };
            PushStyleCheckbox(hasDescription ? themeColor : colorDark);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 3.0f));
            bool flag = (flags & bitMask) != 0;
            if (ImGui::Checkbox("##check", &flag)) {
                if (flag) {
                    flags |= bitMask;
                } else {
                    flags &= ~bitMask;
                }
            }
            ImGui::PopStyleVar();
            PopStyleCheckbox();

            // Show tooltip for all flag checkboxes in search results
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                if (hasDescription) {
                    ImGui::Text("0x%02X: %s", index, UIWidgets::WrappedText(desc, 60).c_str());
                } else {
                    ImGui::Text("0x%02X: %s (Bit %d)", index, flagTable.name, flagIndex);
                }
                ImGui::EndTooltip();
            }

            ImGui::SameLine();
            if (hasDescription) {
                ImGui::TextWrapped("0x%02X: %s", index, desc);
            } else {
                ImGui::Text("0x%02X", index);
            }

            ImGui::PopID();
        }
    }

    if (!hasMatches) {
        ImGui::Text("No flags match the current search.");
    }
}

void DrawFlagsTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("flagsTab", ImVec2(0, 0), true);

    // ========== COLLAPSIBLE FLAG TABLES FIRST ==========
    // Flag tables - use 2-column layout to leave space for other cards
    UIWidgets::BeginCardLayout({ .columnsPerRow = 2, .minColumnWidth = 450.0f });

    // Render tables in reverse order - last 4 (Event Check Inf, Inf, Event Inf, Randomizer Inf) first
    for (int i = (int)flagTables.size() - 1; i >= 0; i--) {
        const FlagTable& flagTable = flagTables[i];
        if (flagTable.flagTableType == RANDOMIZER_INF && !IS_RANDO && !IS_BOSS_RUSH) {
            continue;
        }

        UIWidgets::BeginCard(flagTable.name, 0);
        std::string treeNodeLabel = std::string(flagTable.name) + "##tree";
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode(treeNodeLabel.c_str())) {
            ImGui::PushID(flagTable.name);
            ImGuiTextFilter& flagFilter = flagTableFilters[flagTable.name];
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 16);
            PushStyleInput(THEME_COLOR);
            flagFilter.Draw();
            PopStyleInput();
            ImGui::Spacing();

            if (!flagFilter.IsActive()) {
                for (size_t j = 0; j < flagTable.size + 1; j++) {
                    DrawGroupWithBorder(
                        [&]() {
                            if (j == 0) {
                                for (int k = 0xF; k >= 0; k--) {
                                    ImGui::SameLine(static_cast<f32>(37.5 + ((0xF - k) * 33.8)));
                                    ImGui::Text("%X", k);
                                }
                            }

                            ImGui::Text("%s", spdlog::fmt_lib::format("{:<2X}", j).c_str());

                            switch (flagTable.flagTableType) {
                                case EVENT_CHECK_INF:
                                    DrawFlagTableArray16(flagTable, static_cast<uint16_t>(j),
                                                         gSaveContext.eventChkInf[j]);
                                    break;
                                case ITEM_GET_INF:
                                    DrawFlagTableArray16(flagTable, static_cast<uint16_t>(j),
                                                         gSaveContext.itemGetInf[j]);
                                    break;
                                case INF_TABLE:
                                    DrawFlagTableArray16(flagTable, static_cast<uint16_t>(j), gSaveContext.infTable[j]);
                                    break;
                                case EVENT_INF:
                                    DrawFlagTableArray16(flagTable, static_cast<uint16_t>(j), gSaveContext.eventInf[j]);
                                    break;
                                case RANDOMIZER_INF:
                                    DrawFlagTableArray16(flagTable, static_cast<uint16_t>(j),
                                                         gSaveContext.ship.randomizerInf[j]);
                                    break;
                            }
                        },
                        flagTable.name);
                }
            } else {
                DrawFlagTableSearchResults(flagTable, flagFilter);
            }

            // make some buttons to help with fishsanity debugging
            uint8_t fsMode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
            if (flagTable.flagTableType == RANDOMIZER_INF && fsMode != RO_FISHSANITY_OFF &&
                fsMode != RO_FISHSANITY_OVERWORLD) {
                if (ImGui::Button("Catch All (Child)")) {
                    for (int k = RAND_INF_CHILD_FISH_1; k <= RAND_INF_CHILD_LOACH_2; k++) {
                        Flags_SetRandomizerInf((RandomizerInf)k);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncatch All (Child)")) {
                    for (int k = RAND_INF_CHILD_FISH_1; k <= RAND_INF_CHILD_LOACH_2; k++) {
                        Flags_UnsetRandomizerInf((RandomizerInf)k);
                    }
                }

                if (ImGui::Button("Catch All (Adult)")) {
                    for (int k = RAND_INF_ADULT_FISH_1; k <= RAND_INF_ADULT_LOACH; k++) {
                        Flags_SetRandomizerInf((RandomizerInf)k);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncatch All (Adult)")) {
                    for (int k = RAND_INF_ADULT_FISH_1; k <= RAND_INF_ADULT_LOACH; k++) {
                        Flags_UnsetRandomizerInf((RandomizerInf)k);
                    }
                }
            }

            ImGui::PopID();
            ImGui::TreePop();
        }
        UIWidgets::EndCard();
    }

    UIWidgets::EndCardLayout();

    // ========== OTHER CARDS LAST ==========
    UIWidgets::BeginCardLayout(
        { .columnsPerRow = 2, .minColumnWidth = 350.0f, .fixedColumnWidths = { 450.0f, 450.0f } });

    // Current Scene Section
    UIWidgets::BeginCard("currentSceneCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Current Scene");
    ImGui::Spacing();
    if (gPlayState != nullptr) {
        ActorContext* act = &gPlayState->actorCtx;

        ImGui::Text("Switch");
        InsertHelpHoverText("Permanently-saved switch flags");
        DrawFlagButtons("swch", act->flags.swch, THEME_COLOR);

        ImGui::Spacing();
        ImGui::Text("Temp Switch");
        InsertHelpHoverText("Temporary switch flags. Unset on scene transitions");
        DrawFlagButtons("tempSwch", act->flags.tempSwch, THEME_COLOR);

        ImGui::Spacing();
        ImGui::Text("Clear");
        InsertHelpHoverText("Permanently-saved room-clear flags");
        DrawFlagButtons("clear", act->flags.clear, THEME_COLOR);

        ImGui::Spacing();
        ImGui::Text("Temp Clear");
        InsertHelpHoverText("Temporary room-clear flags. Unset on scene transitions");
        DrawFlagButtons("tempClear", act->flags.tempClear, THEME_COLOR);

        ImGui::Spacing();
        ImGui::Text("Collect");
        InsertHelpHoverText("Permanently-saved collect flags");
        DrawFlagButtons("collect", act->flags.collect, THEME_COLOR);

        ImGui::Spacing();
        ImGui::Text("Temp Collect");
        InsertHelpHoverText("Temporary collect flags. Unset on scene transitions");
        DrawFlagButtons("tempCollect", act->flags.tempCollect, THEME_COLOR);

        ImGui::Spacing();
        ImGui::Text("Chest");
        InsertHelpHoverText("Permanently-saved chest flags");
        DrawFlagButtons("chest", act->flags.chest, THEME_COLOR);

        ImGui::Spacing();
        ImGui::BeginGroup();

        if (Button(
                "Reload Flags",
                ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR).Tooltip("Load flags from saved scene flags"))) {
            act->flags.swch = gSaveContext.sceneFlags[gPlayState->sceneNum].swch;
            act->flags.clear = gSaveContext.sceneFlags[gPlayState->sceneNum].clear;
            act->flags.collect = gSaveContext.sceneFlags[gPlayState->sceneNum].collect;
            act->flags.chest = gSaveContext.sceneFlags[gPlayState->sceneNum].chest;
        }

        if (Button("Save Flags",
                   ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR).Tooltip("Save current scene flags"))) {
            gSaveContext.sceneFlags[gPlayState->sceneNum].swch = act->flags.swch;
            gSaveContext.sceneFlags[gPlayState->sceneNum].clear = act->flags.clear;
            gSaveContext.sceneFlags[gPlayState->sceneNum].collect = act->flags.collect;
            gSaveContext.sceneFlags[gPlayState->sceneNum].chest = act->flags.chest;
        }

        if (Button("Clear Flags",
                   ButtonOptions().Size(ImVec2(0, 0)).Color(Colors::Red).Tooltip("Clear current scene flags"))) {
            act->flags.swch = 0;
            act->flags.clear = 0;
            act->flags.collect = 0;
            act->flags.chest = 0;
        }

        ImGui::EndGroup();
    } else {
        ImGui::Text("Current game state does not have an active scene");
    }
    UIWidgets::EndCard();

    // Saved Scene Flags Card
    UIWidgets::BeginCard("savedSceneCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Saved Scene Flags");
    ImGui::Spacing();

    static uint32_t selectedSceneFlagMap = 0;
    if (selectedSceneFlagMap >= SCENE_ID_MAX) {
        selectedSceneFlagMap = 0;
    }
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Map");
    ImGui::SameLine();
    PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("##Map", SohUtils::GetSceneName(selectedSceneFlagMap).c_str())) {
        for (int32_t sceneIndex = 0; sceneIndex < SCENE_ID_MAX; sceneIndex++) {
            if (ImGui::Selectable(SohUtils::GetSceneName(sceneIndex).c_str())) {
                selectedSceneFlagMap = sceneIndex;
            }
        }
        ImGui::EndCombo();
    }
    PopStyleCombobox();

    // Don't show current scene button if there is no current scene
    if (gPlayState != nullptr) {
        ImGui::SameLine();
        if (Button("Current",
                   ButtonOptions().Size(ImVec2(0, 0)).Color(THEME_COLOR).Tooltip("Open flags for current scene"))) {
            if (gPlayState->sceneNum < SCENE_ID_MAX) {
                selectedSceneFlagMap = gPlayState->sceneNum;
            }
        }
    }

    ImGui::Spacing();
    ImGui::Text("Switch");
    DrawFlagArray32("Switch", gSaveContext.sceneFlags[selectedSceneFlagMap].swch, THEME_COLOR);

    ImGui::Spacing();
    ImGui::Text("Clear");
    DrawFlagArray32("Clear", gSaveContext.sceneFlags[selectedSceneFlagMap].clear, THEME_COLOR);

    ImGui::Spacing();
    ImGui::Text("Collect");
    DrawFlagArray32("Collect", gSaveContext.sceneFlags[selectedSceneFlagMap].collect, THEME_COLOR);

    ImGui::Spacing();
    ImGui::Text("Chest");
    DrawFlagArray32("Chest", gSaveContext.sceneFlags[selectedSceneFlagMap].chest, THEME_COLOR);

    ImGui::Spacing();
    ImGui::Text("Rooms");
    DrawFlagArray32("Rooms", gSaveContext.sceneFlags[selectedSceneFlagMap].rooms, THEME_COLOR);

    ImGui::Spacing();
    ImGui::Text("Floors");
    DrawFlagArray32("Floors", gSaveContext.sceneFlags[selectedSceneFlagMap].floors, THEME_COLOR);
    UIWidgets::EndCard();

    // Gold Skulltulas Card
    UIWidgets::BeginCard("gsCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Gold Skulltulas");
    ImGui::Spacing();

    PushStyleCombobox(THEME_COLOR);
    static size_t selectedGsMap = 0;
    if (selectedGsMap >= gsMapping.size()) {
        selectedGsMap = 0;
    }
    if (ImGui::BeginCombo("##GSMap", gsMapping[selectedGsMap])) {
        for (size_t index = 0; index < gsMapping.size(); index++) {
            if (ImGui::Selectable(gsMapping[index])) {
                selectedGsMap = index;
            }
        }
        ImGui::EndCombo();
    }
    PopStyleCombobox();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Flags");
    uint32_t currentFlags = GET_GS_FLAGS(selectedGsMap);
    uint32_t allFlags = gAreaGsFlags[selectedGsMap];
    uint32_t setMask = 1;
    // Iterate over bitfield and create a checkbox for each skulltula
    while (allFlags != 0) {
        bool isThisSet = (currentFlags & 0x1) == 0x1;

        ImGui::SameLine();
        ImGui::PushID(allFlags);
        PushStyleCheckbox(THEME_COLOR);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 3.0f));
        if (ImGui::Checkbox("##gs", &isThisSet)) {
            if (isThisSet) {
                SET_GS_FLAGS(selectedGsMap, setMask);
            } else {
                uint32_t currentFlagsBase = GET_GS_FLAGS(selectedGsMap);
                gSaveContext.gsFlags[selectedGsMap >> 2] &= ~gGsFlagsMasks[selectedGsMap & 3];
                SET_GS_FLAGS(selectedGsMap, currentFlagsBase & ~setMask);
            }
        }
        ImGui::PopStyleVar();
        PopStyleCheckbox();

        ImGui::PopID();

        allFlags >>= 1;
        currentFlags >>= 1;
        setMask <<= 1;
    }

    // If playing a Randomizer Save with Shuffle Skull Tokens on anything other than "Off" we don't want to keep
    // GS Token Count updated, since Gold Skulltulas killed will not correlate to GS Tokens Collected.
    if (!(IS_RANDO &&
          OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_TOKENS) != RO_TOKENSANITY_OFF)) {
        static bool keepGsCountUpdated = true;
        Checkbox("Keep GS Count Updated", &keepGsCountUpdated,
                 CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Automatically adjust the number of gold skulltula tokens acquired based on set flags."));
        int32_t gsCount = 0;
        if (keepGsCountUpdated) {
            for (int32_t gsFlagIndex = 0; gsFlagIndex < 6; gsFlagIndex++) {
                gsCount += std::popcount(static_cast<uint32_t>(gSaveContext.gsFlags[gsFlagIndex]));
            }
            gSaveContext.inventory.gsTokens = gsCount;
        }
    }
    UIWidgets::EndCard();

    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

// Draws a combo that lets you choose and upgrade value from a drop-down of text values
void DrawUpgrade(const std::string& categoryName, int32_t categoryId, const std::vector<std::string>& names) {
    ImGui::Text("%s", categoryName.c_str());
    ImGui::SameLine();
    ImGui::PushID(categoryName.c_str());
    PushStyleCombobox(THEME_COLOR);
    ImGui::AlignTextToFramePadding();
    auto value = (size_t)CUR_UPG_VALUE(categoryId);
    auto name = value < names.size() ? names[value].c_str() : "Glitched";
    if (ImGui::BeginCombo("##upgrade", name)) {
        for (size_t i = 0; i < names.size(); i++) {
            if (ImGui::Selectable(names[i].c_str())) {
                Inventory_ChangeUpgrade(categoryId, static_cast<s16>(i));
            }
        }

        ImGui::EndCombo();
    }
    PopStyleCombobox();
    ImGui::PopID();
    UIWidgets::Tooltip(categoryName.c_str());
}

// Draws a combo that lets you choose and upgrade value from a popup grid of icons
void DrawUpgradeIcon(const std::string& categoryName, int32_t categoryId, const std::vector<uint8_t>& items) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    static const char* upgradePopupPicker = "upgradePopupPicker";

    ImGui::PushID(categoryName.c_str());

    PushStyleButton(Colors::DarkGray);
    auto value = (size_t)CUR_UPG_VALUE(categoryId);
    uint8_t item = value < items.size() ? items[value] : (uint8_t)ITEM_NONE;
    const ItemMapEntry& slotEntry = itemMapping[item];
    if (ImGui::ImageButton(slotEntry.name.c_str(),
                           gui->GetTextureByName(item != ITEM_NONE ? slotEntry.name : itemMapping[items[1]].nameFaded),
                           ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1))) {
        ImGui::OpenPopup(upgradePopupPicker);
    }
    PopStyleButton();
    Tooltip(categoryName.c_str());

    if (ImGui::BeginPopup(upgradePopupPicker)) {
        for (size_t pickerIndex = 0; pickerIndex < items.size(); pickerIndex++) {
            if ((pickerIndex % 8) != 0) {
                ImGui::SameLine();
            }

            PushStyleButton(Colors::DarkGray);
            if (items[pickerIndex] == ITEM_NONE) {
                if (ImGui::Button("##upgradePopupPicker",
                                  ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
                    Inventory_ChangeUpgrade(categoryId, static_cast<s16>(pickerIndex));
                    ImGui::CloseCurrentPopup();
                }
                Tooltip("None");
            } else {
                const ItemMapEntry& slotEntry = itemMapping[items[pickerIndex]];
                auto ret = ImGui::ImageButton(slotEntry.name.c_str(), gui->GetTextureByName(slotEntry.name),
                                              ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
                if (ret) {
                    Inventory_ChangeUpgrade(categoryId, static_cast<s16>(pickerIndex));
                    ImGui::CloseCurrentPopup();
                }
                Tooltip(GetItemDisplayName(slotEntry.id));
            }
            PopStyleButton();
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void DrawEquipmentTab() {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("equipmentTab", ImVec2(0, 0), true);

    static const int8_t U8_ZERO = 0;

    UIWidgets::BeginCardLayout(
        { .columnsPerRow = 2, .minColumnWidth = 350.0f, .fixedColumnWidths = { 450.0f, 450.0f } });

    UIWidgets::BeginCard("equipmentCard", 0); // Force to column 0
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Equipment");
    ImGui::Spacing();

    // Get currently equipped items if in game
    int8_t currentSword = -1, currentShield = -1, currentTunic = -1, currentBoots = -1;
    if (gPlayState != nullptr) {
        Player* player = GET_PLAYER(gPlayState);
        currentSword = player->currentSwordItemId;
        currentShield = player->currentShield;
        currentTunic = player->currentTunic;
        currentBoots = player->currentBoots;
    }

    const std::vector<uint8_t> equipmentValues = {
        ITEM_SWORD_KOKIRI, ITEM_SWORD_MASTER,  ITEM_SWORD_BGS,     ITEM_SWORD_BROKEN,
        ITEM_SHIELD_DEKU,  ITEM_SHIELD_HYLIAN, ITEM_SHIELD_MIRROR, ITEM_NONE,
        ITEM_TUNIC_KOKIRI, ITEM_TUNIC_GORON,   ITEM_TUNIC_ZORA,    ITEM_NONE,
        ITEM_BOOTS_KOKIRI, ITEM_BOOTS_IRON,    ITEM_BOOTS_HOVER,   ITEM_NONE,
    };
    for (size_t i = 0; i < equipmentValues.size(); i++) {
        // Skip over unused 4th slots for shields, boots, and tunics
        if (equipmentValues[i] == ITEM_NONE) {
            continue;
        }
        if ((i % 4) != 0) {
            ImGui::SameLine();
        }

        ImGui::PushID(static_cast<int>(i));
        uint32_t bitMask = 1 << i;
        bool hasEquip = (bitMask & gSaveContext.inventory.equipment) != 0;
        const ItemMapEntry& entry = itemMapping[equipmentValues[i]];

        // Check if this item is currently equipped
        bool isEquipped = false;
        if (i < 4) {
            isEquipped = (currentSword == equipmentValues[i]);
        } else if (i < 8) {
            isEquipped = (currentShield == static_cast<int8_t>(PLAYER_SHIELD_DEKU + (i - 4)));
        } else if (i < 12) {
            isEquipped = (currentTunic == static_cast<int8_t>(PLAYER_TUNIC_KOKIRI + (i - 8)));
        } else {
            isEquipped = (currentBoots == static_cast<int8_t>(PLAYER_BOOTS_KOKIRI + (i - 12)));
        }

        PushStyleButton(Colors::DarkGray);
        auto ret =
            ImGui::ImageButton(entry.name.c_str(), gui->GetTextureByName(hasEquip ? entry.name : entry.nameFaded),
                               ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
        if (ret) {
            if (hasEquip) {
                gSaveContext.inventory.equipment &= ~bitMask;
            } else {
                gSaveContext.inventory.equipment |= bitMask;
            }
        }
        PopStyleButton();

        Tooltip(GetItemDisplayName(entry.id));

        // Draw border AFTER button (using actual rendered size)
        if (isEquipped) {
            ImVec2 itemMin = ImGui::GetItemRectMin();
            ImVec2 itemMax = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(ImVec2(itemMin.x - 2, itemMin.y - 2),
                                                ImVec2(itemMax.x + 2, itemMax.y + 2), IM_COL32(255, 255, 255, 255),
                                                0.0f, 0, 2.0f);
        }

        ImGui::PopID();
    }

    UIWidgets::EndCard();

    UIWidgets::BeginCard("upgradesCard", 0); // Force to column 0 (under Equipment)
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Upgrades");
    ImGui::Spacing();

    const std::vector<uint8_t> bulletBagValues = {
        ITEM_NONE,
        ITEM_BULLET_BAG_30,
        ITEM_BULLET_BAG_40,
        ITEM_BULLET_BAG_50,
    };
    DrawUpgradeIcon("Bullet Bag", UPG_BULLET_BAG, bulletBagValues);

    ImGui::SameLine();

    const std::vector<uint8_t> quiverValues = {
        ITEM_NONE,
        ITEM_QUIVER_30,
        ITEM_QUIVER_40,
        ITEM_QUIVER_50,
    };
    DrawUpgradeIcon("Quiver", UPG_QUIVER, quiverValues);

    ImGui::SameLine();

    const std::vector<uint8_t> bombBagValues = {
        ITEM_NONE,
        ITEM_BOMB_BAG_20,
        ITEM_BOMB_BAG_30,
        ITEM_BOMB_BAG_40,
    };
    DrawUpgradeIcon("Bomb Bag", UPG_BOMB_BAG, bombBagValues);

    ImGui::SameLine();

    const std::vector<uint8_t> scaleValues = {
        ITEM_NONE,
        ITEM_SCALE_SILVER,
        ITEM_SCALE_GOLDEN,
    };
    DrawUpgradeIcon("Scale", UPG_SCALE, scaleValues);

    ImGui::SameLine();

    const std::vector<uint8_t> strengthValues = {
        ITEM_NONE,
        ITEM_BRACELET,
        ITEM_GAUNTLETS_SILVER,
        ITEM_GAUNTLETS_GOLD,
    };
    DrawUpgradeIcon("Strength", UPG_STRENGTH, strengthValues);

    // There is no icon for child wallet, so default to a text list
    // Always include Tycoon wallet in case it exists in an old save file
    const std::vector<std::string> walletNames = {
        "Child (99)",
        "Adult (200)",
        "Giant (500)",
        "Tycoon (999)",
    };
    DrawUpgrade("Wallet", UPG_WALLET, walletNames);

    const char* STICK_LEVEL_NAMES[4] = { "None", "10 Sticks", "20 Sticks", "30 Sticks" };
    constexpr int8_t STICK_LEVEL_MAX = 3;
    int8_t stickLevel = CUR_UPG_VALUE(UPG_STICKS);
    ImGui::Text("Deku Stick Capacity");
    PushStyleSlider(THEME_COLOR);
    static const int8_t STICK_LEVEL_MAX_VAR = STICK_LEVEL_MAX;
    if (ImGui::SliderScalar("##stickLevelSlider", ImGuiDataType_S8, &stickLevel, &U8_ZERO, &STICK_LEVEL_MAX_VAR,
                            STICK_LEVEL_NAMES[stickLevel])) {
        Inventory_ChangeUpgrade(UPG_STICKS, stickLevel);
    }
    PopStyleSlider();
    Tooltip("Maximum number of Deku Sticks Link can carry");

    const char* NUT_LEVEL_NAMES[4] = { "None", "20 Nuts", "30 Nuts", "40 Nuts" };
    constexpr int8_t NUT_LEVEL_MAX = 3;
    int8_t nutLevel = CUR_UPG_VALUE(UPG_NUTS);
    ImGui::Text("Deku Nut Capacity");
    PushStyleSlider(THEME_COLOR);
    static const int8_t NUT_LEVEL_MAX_VAR = NUT_LEVEL_MAX;
    if (ImGui::SliderScalar("##nutLevelSlider", ImGuiDataType_S8, &nutLevel, &U8_ZERO, &NUT_LEVEL_MAX_VAR,
                            NUT_LEVEL_NAMES[nutLevel])) {
        Inventory_ChangeUpgrade(UPG_NUTS, nutLevel);
    }
    PopStyleSlider();
    Tooltip("Maximum number of Deku Nuts Link can carry");

    UIWidgets::EndCard();

    // "Randomizer Specific" — rando-only editors grouped under the upgrades. Bombchu Bag
    // Capacity and Triforce Pieces were moved here (from this card and the General settings
    // card respectively), and ability-shuffle toggles are added below. Only rendered when the
    // seed actually uses at least one of these features (no empty card on vanilla-ish saves).
    if (IS_RANDO) {
        auto& randomizer = *OTRGlobals::Instance->gRandomizer;
        bool bombchuProgressive = randomizer.GetRandoSettingValue(RSK_BOMBCHU_BAG) == RO_BOMBCHU_BAG_PROGRESSIVE;
        bool triforceHunt = randomizer.GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_TOTAL) > 0;
        bool anyAbilityShuffle =
            randomizer.GetRandoSettingValue(RSK_SHUFFLE_SWIM) || randomizer.GetRandoSettingValue(RSK_SHUFFLE_GRAB) ||
            randomizer.GetRandoSettingValue(RSK_SHUFFLE_CLIMB) || randomizer.GetRandoSettingValue(RSK_SHUFFLE_CRAWL) ||
            randomizer.GetRandoSettingValue(RSK_SHUFFLE_OPEN_CHEST) ||
            randomizer.GetRandoSettingValue(RSK_SHUFFLE_SPEAK) ||
            randomizer.GetRandoSettingValue(RSK_SHUFFLE_OCARINA_BUTTONS) ||
            randomizer.GetRandoSettingValue(RSK_ROCS_FEATHER);

        if (bombchuProgressive || triforceHunt || anyAbilityShuffle) {
            UIWidgets::BeginCard("randomizerSpecificCard", 0);
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Randomizer Specific");
            ImGui::Spacing();

            // (a) Bombchu Bag Capacity
            if (bombchuProgressive) {
                const std::vector<std::string> bombchuNames = { "None", "20", "30", "50" };
                ImGui::Text("%s", "Bombchu Bag Capacity");
                ImGui::SameLine();
                ImGui::PushID("Bombchu Bag Capacity");
                PushStyleCombobox(THEME_COLOR);
                ImGui::AlignTextToFramePadding();
                auto value = gSaveContext.ship.quest.data.randomizer.bombchuUpgradeLevel;
                auto name = value < bombchuNames.size() ? bombchuNames[value].c_str() : "Glitched";
                if (ImGui::BeginCombo("##upgrade", name)) {
                    for (size_t i = 0; i < bombchuNames.size(); i++) {
                        if (ImGui::Selectable(bombchuNames[i].c_str())) {
                            gSaveContext.ship.quest.data.randomizer.bombchuUpgradeLevel = static_cast<u8>(i);
                            if (i > 0) {
                                INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
                            } else {
                                INV_CONTENT(ITEM_BOMBCHU) = ITEM_NONE;
                            }
                        }
                    }
                    ImGui::EndCombo();
                }
                PopStyleCombobox();
                ImGui::PopID();
                Tooltip("Bombchu Bag Capacity");
            }

            // (b) Triforce Pieces (moved from the General settings card)
            if (triforceHunt) {
                ImGui::Text("Triforce Pieces");
                PushStyleInput(Colors::Purple);
                ImGui::InputScalar("##TriforcePieces", ImGuiDataType_U8,
                                   &gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected);
                Tooltip("Currently obtained Triforce Pieces. For Triforce Hunt.");
                PopStyleInput();
            }

            // (c) Ability-shuffle items: one labeled checkbox per item that's in the pool,
            // one per line, alphabetically by name. Visibility derives from RSK_SHUFFLE_*
            // (the pool isn't queryable at runtime); the checked state is the RAND_INF_ flag.
            {
                struct AbilityEntry {
                    std::string label;
                    RandomizerInf flag;
                };
                std::vector<AbilityEntry> abilities;
                auto add = [&](RandomizerGet rg, RandomizerInf flag) {
                    abilities.push_back({ Rando::StaticData::RetrieveItem(rg).GetName().english, flag });
                };

                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_SWIM))
                    add(RG_BRONZE_SCALE, RAND_INF_CAN_SWIM);
                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_GRAB))
                    add(RG_POWER_BRACELET, RAND_INF_CAN_GRAB);
                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_CLIMB))
                    add(RG_CLIMB, RAND_INF_CAN_CLIMB);
                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_CRAWL))
                    add(RG_CRAWL, RAND_INF_CAN_CRAWL);
                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_OPEN_CHEST)) {
                    add(RG_OPEN_CHEST, RAND_INF_CAN_OPEN_CHEST);
                    if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_OPEN_CHEST) == RO_OPEN_CHEST_PROGRESSIVE) {
                        abilities.push_back({ "Large Chest", RAND_INF_CAN_OPEN_LARGE_CHEST }); // no RG_ item
                    }
                }
                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_SPEAK)) {
                    for (int i = 0; i <= (RG_SPEAK_ZORA - RG_SPEAK_DEKU); i++) {
                        add(static_cast<RandomizerGet>(RG_SPEAK_DEKU + i),
                            static_cast<RandomizerInf>(RAND_INF_CAN_SPEAK_DEKU + i));
                    }
                }
                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_OCARINA_BUTTONS)) {
                    for (int i = 0; i <= (RG_OCARINA_C_RIGHT_BUTTON - RG_OCARINA_A_BUTTON); i++) {
                        add(static_cast<RandomizerGet>(RG_OCARINA_A_BUTTON + i),
                            static_cast<RandomizerInf>(RAND_INF_HAS_OCARINA_A + i));
                    }
                }
                if (randomizer.GetRandoSettingValue(RSK_ROCS_FEATHER))
                    add(RG_ROCS_FEATHER, RAND_INF_OBTAINED_ROCS_FEATHER);
                if (randomizer.GetRandoSettingValue(RSK_SHUFFLE_FISHING_POLE))
                    add(RG_FISHING_POLE, RAND_INF_FISHING_POLE_FOUND);

                std::sort(abilities.begin(), abilities.end(),
                          [](const AbilityEntry& a, const AbilityEntry& b) { return a.label < b.label; });

                for (const auto& entry : abilities) {
                    ImGui::PushID(static_cast<int>(entry.flag));
                    bool has = Flags_GetRandomizerInf(entry.flag) != 0;
                    if (Checkbox(entry.label.c_str(), &has, CheckboxOptions().Color(THEME_COLOR))) {
                        if (has) {
                            Flags_SetRandomizerInf(entry.flag);
                        } else {
                            Flags_UnsetRandomizerInf(entry.flag);
                        }
                    }
                    ImGui::PopID();
                }
            }

            UIWidgets::EndCard();
        }
    }

    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

// Draws a toggleable icon for a quest item that is faded when disabled
void DrawQuestItemButton(uint32_t item) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    const QuestMapEntry& entry = questMapping[item];
    uint32_t bitMask = 1 << entry.id;
    bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
    PushStyleButton(Colors::DarkGray);
    auto ret =
        ImGui::ImageButton(entry.name.c_str(), gui->GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                           ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
    if (ret) {
        if (hasQuestItem) {
            gSaveContext.inventory.questItems &= ~bitMask;
        } else {
            gSaveContext.inventory.questItems |= bitMask;
        }
    }
    PopStyleButton();
    Tooltip(SohUtils::GetQuestItemName(entry.id).c_str());
}

// Draws a toggleable icon for a dungeon item that is faded when disabled
void DrawDungeonItemButton(uint32_t item, uint32_t scene) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    const ItemMapEntry& entry = itemMapping[item];
    uint32_t bitMask = 1 << (entry.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[scene]) != 0;
    PushStyleButton(Colors::DarkGray);
    auto ret = ImGui::ImageButton(entry.name.c_str(), gui->GetTextureByName(hasItem ? entry.name : entry.nameFaded),
                                  ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1));
    if (ret) {
        if (hasItem) {
            gSaveContext.inventory.dungeonItems[scene] &= ~bitMask;
        } else {
            gSaveContext.inventory.dungeonItems[scene] |= bitMask;
        }
    }
    PopStyleButton();
    Tooltip(GetItemDisplayName(entry.id));
}

void DrawQuestStatusTab() {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("questStatusTab", ImVec2(0, 0), true);

    UIWidgets::BeginCardLayout(
        { .columnsPerRow = 2, .minColumnWidth = 350.0f, .fixedColumnWidths = { 450.0f, 450.0f } });

    UIWidgets::BeginCard("medallionsStonesCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Medallions & Stones");
    ImGui::Spacing();

    for (int32_t i = QUEST_MEDALLION_FOREST; i < QUEST_MEDALLION_LIGHT + 1; i++) {
        if (i != QUEST_MEDALLION_FOREST) {
            ImGui::SameLine();
        }
        DrawQuestItemButton(i);
    }

    for (int32_t i = QUEST_KOKIRI_EMERALD; i < QUEST_ZORA_SAPPHIRE + 1; i++) {
        if (i != QUEST_KOKIRI_EMERALD) {
            ImGui::SameLine();
        }
        DrawQuestItemButton(i);
    }

    // Put Stone of Agony and Gerudo Card on the same line with a little space between them
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2);

    ImGui::SameLine();
    DrawQuestItemButton(QUEST_STONE_OF_AGONY);

    ImGui::SameLine();
    DrawQuestItemButton(QUEST_GERUDO_CARD);
    UIWidgets::EndCard();

    UIWidgets::BeginCard("songsCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Songs");
    ImGui::Spacing();

    for (const auto& [quest, entry] : songMapping) {
        if ((entry.id != QUEST_SONG_MINUET) && (entry.id != QUEST_SONG_LULLABY)) {
            ImGui::SameLine();
        }

        uint32_t bitMask = 1 << entry.id;
        bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
        PushStyleButton(Colors::DarkGray);
        auto ret =
            ImGui::ImageButton(entry.name.c_str(), gui->GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                               ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
        if (ret) {
            if (hasQuestItem) {
                gSaveContext.inventory.questItems &= ~bitMask;
            } else {
                gSaveContext.inventory.questItems |= bitMask;
            }
        }
        PopStyleButton();
        Tooltip(SohUtils::GetQuestItemName(entry.id).c_str());
    }

    UIWidgets::EndCard();

    UIWidgets::BeginCard("gsTokensCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Gold Skulltula Tokens");
    ImGui::Spacing();

    static const int16_t GS_MIN = 0;
    static const int16_t GS_MAX = 100;
    int16_t gsCount = gSaveContext.inventory.gsTokens;
    PushStyleSlider(THEME_COLOR);
    if (ImGui::SliderScalar("##gsCountSlider", ImGuiDataType_S16, &gsCount, &GS_MIN, &GS_MAX, "Count: %d")) {
        gSaveContext.inventory.gsTokens = gsCount;
    }
    PopStyleSlider();
    Tooltip("Number of gold skulltula tokens acquired");

    uint32_t bitMask = 1 << QUEST_SKULL_TOKEN;
    bool gsUnlocked = (bitMask & gSaveContext.inventory.questItems) != 0;
    if (Checkbox("GS unlocked", &gsUnlocked, CheckboxOptions().Color(THEME_COLOR))) {
        if (gsUnlocked) {
            gSaveContext.inventory.questItems |= bitMask;
        } else {
            gSaveContext.inventory.questItems &= ~bitMask;
        }
    }
    Tooltip("If unlocked, enables showing the gold skulltula count in the quest status menu");
    UIWidgets::EndCard();

    UIWidgets::BeginCard("pohCard");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Pieces of Heart");
    ImGui::Spacing();

    static const char* pohNames[4] = { "0", "1", "2", "3" };
    int32_t pohCount = (gSaveContext.inventory.questItems & 0xF0000000) >> 28;
    PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("##pohCombo", pohNames[pohCount])) {
        for (int32_t i = 0; i < 4; i++) {
            if (ImGui::Selectable(pohNames[i], pohCount == i)) {
                gSaveContext.inventory.questItems &= ~0xF0000000;
                gSaveContext.inventory.questItems |= (i << 28);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text("Pieces of Heart");
    Tooltip("The number of pieces of heart acquired towards the next heart container");
    PopStyleCombobox();
    UIWidgets::EndCard();

    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void DrawDungeonItemsTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("dungeonItemsTab", ImVec2(0, 0), true);

    UIWidgets::BeginCardLayout(
        { .columnsPerRow = 2, .minColumnWidth = 250.0f, .fixedColumnWidths = { 280.0f, 280.0f } });

    // All dungeons from Deku Tree to Ganon's Castle (skip boss scenes)
    for (int32_t dungeonIndex = SCENE_DEKU_TREE; dungeonIndex <= SCENE_INSIDE_GANONS_CASTLE; dungeonIndex++) {
        // Skip boss scenes and invalid scene IDs (0x0C doesn't exist)
        // Note: Ice Cavern has map/compass, just no boss key
        if (dungeonIndex == SCENE_DEKU_TREE_BOSS || dungeonIndex == SCENE_DODONGOS_CAVERN_BOSS ||
            dungeonIndex == SCENE_JABU_JABU_BOSS || dungeonIndex == SCENE_FOREST_TEMPLE_BOSS ||
            dungeonIndex == SCENE_FIRE_TEMPLE_BOSS || dungeonIndex == SCENE_WATER_TEMPLE_BOSS ||
            dungeonIndex == SCENE_SPIRIT_TEMPLE_BOSS || dungeonIndex == SCENE_SHADOW_TEMPLE_BOSS ||
            dungeonIndex == SCENE_GANONDORF_BOSS || dungeonIndex == SCENE_GANON_BOSS || dungeonIndex == 0x0C) {
            continue;
        }

        UIWidgets::BeginCard(SohUtils::GetSceneName(dungeonIndex).c_str());
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", SohUtils::GetSceneName(dungeonIndex).c_str());

        // Map, Compass, Small Key, Boss Key (new order)
        DrawDungeonItemButton(ITEM_DUNGEON_MAP, dungeonIndex);
        ImGui::SameLine();
        DrawDungeonItemButton(ITEM_COMPASS, dungeonIndex);
        ImGui::SameLine();

        // Small keys - clickable button with popup (only for dungeons that have keys)
        if (GetMaxKeysForDungeon(dungeonIndex) > 0) {
            auto gui =
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
            // Save cursor position before small key button to restore for boss key alignment
            ImVec2 smallKeyCursor = ImGui::GetCursorScreenPos();

            uint8_t keyCount = gSaveContext.inventory.dungeonKeys[dungeonIndex];
            std::string keyPopupId = spdlog::fmt_lib::format("##SmallKeyPopup_{}", dungeonIndex);
            std::string keySliderId = spdlog::fmt_lib::format("##KeySlider_{}", dungeonIndex);
            PushStyleButton(Colors::DarkGray);
            // keyCount is uint8_t, so -1 becomes 255. Only gray out when exactly 255 (-1)
            bool showNormal = (keyCount != 255);
            // Show count label for any value except 255 (-1)
            bool showCount = (keyCount != 255);
            if (ImGui::ImageButton(itemMapping[ITEM_KEY_SMALL].name.c_str(),
                                   gui->GetTextureByName(showNormal ? itemMapping[ITEM_KEY_SMALL].name
                                                                    : itemMapping[ITEM_KEY_SMALL].nameFaded),
                                   ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1))) {
                ImGui::OpenPopup(keyPopupId.c_str());
            }
            PopStyleButton();

            // Display key count label at bottom right of the button rectangle
            if (showCount) {
                ImVec2 buttonMax = ImGui::GetItemRectMax();
                ImVec2 framePadding = ImGui::GetStyle().FramePadding;
                ImVec2 textSize = ImGui::CalcTextSize(spdlog::fmt_lib::format("{}", keyCount).c_str());
                // Position text inside the button, accounting for frame padding
                float textX = buttonMax.x - textSize.x - framePadding.x;
                float textY = buttonMax.y - textSize.y - framePadding.y;
                // Draw text directly without affecting cursor position
                ImGui::GetForegroundDrawList()->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255),
                                                        spdlog::fmt_lib::format("{}", keyCount).c_str());
            }

            Tooltip(spdlog::fmt_lib::format("Keys: {}", keyCount == 255 ? -1 : keyCount).c_str());

            // Small key popup
            if (ImGui::BeginPopup(keyPopupId.c_str())) {
                int8_t keys = keyCount;
                int8_t maxKeys = GetMaxKeysForDungeon(dungeonIndex);
                int8_t negOne = -1;
                ImGui::Text("Small Keys");
                ImGui::PushItemWidth(150.0f);
                PushStyleInput(THEME_COLOR);
                if (ImGui::SliderScalar(keySliderId.c_str(), ImGuiDataType_S8, &keys, &negOne, &maxKeys, "Count: %d")) {
                    gSaveContext.inventory.dungeonKeys[dungeonIndex] = keys;
                    gSaveContext.ship.stats.dungeonKeys[dungeonIndex] = keys;
                }
                PopStyleInput();
                ImGui::PopItemWidth();
                ImGui::EndPopup();
            }

            // Restore cursor position to align boss key with original grid
            ImGui::SetCursorScreenPos(smallKeyCursor);
            ImGui::SameLine();
        }

        // Only show boss key button for dungeons that have one
        if (DungeonHasBossKey(dungeonIndex)) {
            DrawDungeonItemButton(ITEM_KEY_BOSS, dungeonIndex);
        }

        UIWidgets::EndCard();
    }

    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void DrawPlayerTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("playerTab", ImVec2(0, 0), true);

    UIWidgets::BeginCardLayout(
        { .columnsPerRow = 2, .minColumnWidth = 350.0f, .fixedColumnWidths = { 450.0f, 450.0f } });

    if (gPlayState != nullptr) {
        Player* player = GET_PLAYER(gPlayState);

        // Position Card
        UIWidgets::BeginCard("positionCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Position");
        ImGui::Spacing();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
        PushStyleInput(Colors::Red);
        ImGui::InputScalar("X##Pos", ImGuiDataType_Float, &player->actor.world.pos.x);
        Tooltip("X Position");
        PopStyleInput();
        PushStyleInput(Colors::Green);
        ImGui::InputScalar("Y##Pos", ImGuiDataType_Float, &player->actor.world.pos.y);
        Tooltip("Y Position");
        PopStyleInput();
        PushStyleInput(THEME_COLOR);
        ImGui::InputScalar("Z##Pos", ImGuiDataType_Float, &player->actor.world.pos.z);
        Tooltip("Z Position");
        PopStyleInput();
        ImGui::PopItemWidth();
        UIWidgets::EndCard();

        // Rotation Card
        UIWidgets::BeginCard("rotationCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Rotation");
        ImGui::Spacing();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
        PushStyleInput(Colors::Red);
        ImGui::InputScalar("X##Rot", ImGuiDataType_S16, &player->actor.world.rot.x);
        Tooltip("X Rotation");
        PopStyleInput();
        PushStyleInput(Colors::Green);
        ImGui::InputScalar("Y##Rot", ImGuiDataType_S16, &player->actor.world.rot.y);
        Tooltip("Y Rotation");
        PopStyleInput();
        PushStyleInput(THEME_COLOR);
        ImGui::InputScalar("Z##Rot", ImGuiDataType_S16, &player->actor.world.rot.z);
        Tooltip("Z Rotation");
        PopStyleInput();
        ImGui::PopItemWidth();
        UIWidgets::EndCard();

        // Model Rotation Card
        UIWidgets::BeginCard("modelRotationCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Model Rotation");
        ImGui::Spacing();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
        PushStyleInput(Colors::Red);
        ImGui::InputScalar("X##ModRot", ImGuiDataType_S16, &player->actor.shape.rot.x);
        Tooltip("Model X Rotation");
        PopStyleInput();
        PushStyleInput(Colors::Green);
        ImGui::InputScalar("Y##ModRot", ImGuiDataType_S16, &player->actor.shape.rot.y);
        Tooltip("Model Y Rotation");
        PopStyleInput();
        PushStyleInput(THEME_COLOR);
        ImGui::InputScalar("Z##ModRot", ImGuiDataType_S16, &player->actor.shape.rot.z);
        Tooltip("Model Z Rotation");
        PopStyleInput();
        ImGui::PopItemWidth();
        UIWidgets::EndCard();

        // Velocity Card
        UIWidgets::BeginCard("velocityCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Velocity");
        ImGui::Spacing();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
        PushStyleInput(THEME_COLOR);
        ImGui::InputScalar("Linear Velocity (XZ)", ImGuiDataType_Float, &player->linearVelocity);
        Tooltip("Link's speed along the XZ plane");
        PopStyleInput();
        PushStyleInput(Colors::Green);
        ImGui::InputScalar("Y Velocity", ImGuiDataType_Float, &player->actor.velocity.y);
        Tooltip("Link's speed along the Y plane. Caps at -20");
        PopStyleInput();
        ImGui::PopItemWidth();
        UIWidgets::EndCard();

        // Physics Card
        UIWidgets::BeginCard("physicsCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Physics");
        ImGui::Spacing();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
        PushStyleInput(THEME_COLOR);
        ImGui::InputScalar("Wall Height", ImGuiDataType_Float, &player->yDistToLedge);
        Tooltip("Height used to determine whether Link can climb or grab a ledge at the top");
        ImGui::InputScalar("Invincibility Timer", ImGuiDataType_S8, &player->invincibilityTimer);
        Tooltip("Can't take damage while this is nonzero");
        ImGui::InputScalar("Gravity", ImGuiDataType_Float, &player->actor.gravity);
        Tooltip("Rate at which Link falls. Default -4.0f");
        ImGui::PopItemWidth();
        PopStyleInput();
        UIWidgets::EndCard();

        // Age Card
        UIWidgets::BeginCard("ageCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Age");
        ImGui::Spacing();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
        PushStyleCombobox(THEME_COLOR);
        if (ImGui::BeginCombo("Link Age on Load", gPlayState->linkAgeOnLoad == 0 ? "Adult" : "Child")) {
            if (ImGui::Selectable("Adult")) {
                gPlayState->linkAgeOnLoad = 0;
            }
            if (ImGui::Selectable("Child")) {
                gPlayState->linkAgeOnLoad = 1;
            }
            ImGui::EndCombo();
        }
        Tooltip("This will change Link's age when you load a map");
        PopStyleCombobox();
        UIWidgets::EndCard();

        // Player State Card
        UIWidgets::BeginCard("playerStateCard");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Player State");
        ImGui::Spacing();

        // State Flags 1
        ImGui::Text("State 1");
        DrawFlagArrayWithTooltips("State1", player->stateFlags1, THEME_COLOR, state1);

        // Show active State 1 flags
        ImGui::Spacing();
        DrawActiveStates("State1", player->stateFlags1, state1);

        ImGui::Spacing();
        // State Flags 2
        ImGui::Text("State 2");
        DrawFlagArrayWithTooltips("State2", player->stateFlags2, THEME_COLOR, state2);

        // Show active State 2 flags
        ImGui::Spacing();
        DrawActiveStates("State2", player->stateFlags2, state2);

        ImGui::Spacing();
        // State Flags 3
        ImGui::Text("State 3");
        DrawFlagArrayWithTooltips("State3", player->stateFlags3, THEME_COLOR, state3);

        // Show active State 3 flags
        ImGui::Spacing();
        DrawActiveStates("State3", player->stateFlags3, state3);

        ImGui::Spacing();
        // Rotation Flags (unk_6AE_rotFlags)
        ImGui::Text("Rotation Flags");
        DrawFlagArray16("Rotation", player->unk_6AE_rotFlags, THEME_COLOR);

        ImGui::Spacing();
        ImGui::Text("Sword State");
        PushStyleCombobox(THEME_COLOR);
        std::string currentSword = "None";
        switch (player->meleeWeaponState) {
            case 0:
                currentSword = "None";
                break;
            case 1:
                currentSword = "Kokiri Sword";
                break;
            case 2:
                currentSword = "Master Sword";
                break;
            case 3:
                currentSword = "Biggoron's Sword";
                break;
            case 4:
                currentSword = "Broken Giant's Knife";
                break;
            default:
                currentSword = spdlog::fmt_lib::format("Unknown ({})", player->meleeWeaponState);
                break;
        }
        if (ImGui::BeginCombo("##SwordState", currentSword.c_str())) {
            if (ImGui::Selectable("None"))
                player->meleeWeaponState = 0;
            if (ImGui::Selectable("Kokiri Sword"))
                player->meleeWeaponState = 1;
            if (ImGui::Selectable("Master Sword"))
                player->meleeWeaponState = 2;
            if (ImGui::Selectable("Biggoron's Sword"))
                player->meleeWeaponState = 3;
            if (ImGui::Selectable("Broken Giant's Knife"))
                player->meleeWeaponState = 4;
            ImGui::EndCombo();
        }
        PopStyleCombobox();
        Tooltip("Current melee weapon state");
        UIWidgets::EndCard();
    }

    UIWidgets::EndCardLayout();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void ResetBaseOptions() {
    intSliderOptionsBase.Color(THEME_COLOR).Size({ 320.0f, 0.0f }).Tooltip("");
    buttonOptionsBase.Color(THEME_COLOR).Size(Sizes::Inline).Tooltip("");
    checkboxOptionsBase.Color(THEME_COLOR).Tooltip("");
    comboboxOptionsBase.Color(THEME_COLOR)
        .ComponentAlignment(ComponentAlignments::Left)
        .LabelPosition(LabelPositions::Near)
        .Tooltip("");
}

void SaveEditorWindow::DrawElement() {
    PushStyleTabs(THEME_COLOR);
    ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));

    if (ImGui::BeginTabBar("SaveContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        ResetBaseOptions();
        if (ImGui::BeginTabItem("General")) {
            DrawGeneralTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Inventory")) {
            DrawInventoryTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Equipment")) {
            DrawEquipmentTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Quest Status")) {
            DrawQuestStatusTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Dungeon Items")) {
            DrawDungeonItemsTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Flags")) {
            DrawFlagsTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Player")) {
            DrawPlayerTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndDisabled();
    ImGui::PopFont();
    PopStyleTabs();
}

void SaveEditorWindow::InitElement() {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    gui->LoadGuiTexture("ROCS_FEATHER", gRocsFeatherTex, "", ImVec4(1, 1, 1, 1));
    gui->LoadGuiTexture("RG_ROCS_FEATHER_PICKER", gRocsFeatherTex, "", ImVec4(1, 1, 1, 1));
}

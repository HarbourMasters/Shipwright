#include "debugSaveEditor.h"
#include "soh/util.h"
#include "soh/SohGui/ImGuiUtils.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"

#include <spdlog/fmt/fmt.h>
#include <array>
#include <bit>
#include <map>
#include <string>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include "soh_assets.h"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "soh/cvar_prefixes.h"
extern PlayState* gPlayState;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/parameter_static/parameter_static.h"
}

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

extern "C" u8 gAreaGsFlags[];

extern "C" u8 gAmmoItems[];

#define IMAGE_SIZE 48.0f

using namespace UIWidgets;

IntSliderOptions intSliderOptionsBase;
ButtonOptions buttonOptionsBase;
CheckboxOptions checkboxOptionsBase;
ComboboxOptions comboboxOptionsBase;

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

typedef enum MagicLevel { MAGIC_LEVEL_NONE, MAGIC_LEVEL_SINGLE, MAGIC_LEVEL_DOUBLE };

std::unordered_map<int8_t, const char*> magicLevelMap = {
    { MAGIC_LEVEL_NONE, "None" },
    { MAGIC_LEVEL_SINGLE, "Single" },
    { MAGIC_LEVEL_DOUBLE, "Double" },
};

typedef enum AudioOutput {
    AUDIO_STEREO,
    AUDIO_MONO,
    AUDIO_HEADSET,
    AUDIO_SURROUND,
};

std::unordered_map<uint8_t, const char*> audioMap = {
    { AUDIO_STEREO, "Stereo" },
    { AUDIO_MONO, "Mono" },
    { AUDIO_HEADSET, "Headset" },
    { AUDIO_SURROUND, "Surround" },
};

typedef enum ZTarget {
    Z_TARGET_SWITCH,
    Z_TARGET_HOLD,
};

std::unordered_map<uint8_t, const char*> zTargetMap = {
    { Z_TARGET_SWITCH, "Switch" },
    { Z_TARGET_HOLD, "Hold" },
};

void DrawInfoTab() {
    // TODO Needs a better method for name changing but for now this will work.
    std::string name;
    ImU16 one = 1;
    for (int i = 0; i < 8; i++) {
        char letter = z2ASCII(gSaveContext.playerName[i]);
        name += letter;
    }
    name += '\0';

    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

    ImGui::Text("Name: %s", name.c_str());
    Tooltip("Player Name");
    std::string nameID;
    for (int i = 0; i < 8; i++) {
        nameID = z2ASCII(i);
        if (i % 4 != 0) {
            ImGui::SameLine();
        }
        PushStyleInput(THEME_COLOR);
        ImGui::InputScalar(nameID.c_str(), ImGuiDataType_U8, &gSaveContext.playerName[i], &one, NULL);
        PopStyleInput();
    }

    // Use an intermediary to keep the health from updating (and potentially killing the player)
    // until it is done being edited
    int16_t healthIntermediary = gSaveContext.healthCapacity;
    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Max Health", ImGuiDataType_S16, &healthIntermediary);
    PopStyleInput();
    if (ImGui::IsItemDeactivated()) {
        gSaveContext.healthCapacity = healthIntermediary;
    }
    Tooltip("Maximum health. 16 units per full heart");
    if (gSaveContext.health > gSaveContext.healthCapacity) {
        gSaveContext.health = gSaveContext.healthCapacity; // Clamp health to new max
    }
    int32_t health = (int32_t)gSaveContext.health;
    if (SliderInt("Health", &health,
                  intSliderOptionsBase.Tooltip("Current health. 16 units per full heart")
                      .Min(0)
                      .Max(gSaveContext.healthCapacity))) {
        gSaveContext.health = (int16_t)health;
    }

    bool isDoubleDefenseAcquired = gSaveContext.isDoubleDefenseAcquired != 0;
    if (Checkbox("Double Defense", &isDoubleDefenseAcquired,
                 checkboxOptionsBase.Tooltip("Is double defense unlocked?"))) {
        gSaveContext.isDoubleDefenseAcquired = isDoubleDefenseAcquired;
        gSaveContext.inventory.defenseHearts =
            gSaveContext.isDoubleDefenseAcquired ? 20 : 0; // Set to get the border drawn in the UI
    }
    if (Combobox("Magic Level", &gSaveContext.magicLevel, magicLevelMap,
                 comboboxOptionsBase.Tooltip("Current magic level"))) {
        gSaveContext.isMagicAcquired = gSaveContext.magicLevel > 0;
        gSaveContext.isDoubleMagicAcquired = gSaveContext.magicLevel == 2;
    }
    gSaveContext.magicCapacity = gSaveContext.magicLevel * 0x30; // Set to get the bar drawn in the UI
    if (gSaveContext.magic > gSaveContext.magicCapacity) {
        gSaveContext.magic = gSaveContext.magicCapacity; // Clamp magic to new max
    }

    int32_t magic = (int32_t)gSaveContext.magic;
    if (SliderInt("Magic", &magic,
                  intSliderOptionsBase.Min(0)
                      .Max(gSaveContext.magicCapacity)
                      .Tooltip("Current magic. 48 units per magic level"))) {
        gSaveContext.magic = (int8_t)magic;
    }

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Rupees", ImGuiDataType_S16, &gSaveContext.rupees);
    Tooltip("Current rupees");
    PopStyleInput();

    SliderInt("Time", (int32_t*)&gSaveContext.dayTime, intSliderOptionsBase.Min(0).Max(0xFFFF).Tooltip("Time of day"));
    if (Button("Dawn", buttonOptionsBase)) {
        gSaveContext.dayTime = 0x4000;
    }
    ImGui::SameLine();
    if (Button("Noon", buttonOptionsBase)) {
        gSaveContext.dayTime = 0x8000;
    }
    ImGui::SameLine();
    if (Button("Sunset", buttonOptionsBase)) {
        gSaveContext.dayTime = 0xC001;
    }
    ImGui::SameLine();
    if (Button("Midnight", buttonOptionsBase)) {
        gSaveContext.dayTime = 0;
    }

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Total Days", ImGuiDataType_S32, &gSaveContext.totalDays);
    Tooltip("Total number of days elapsed since the start of the game");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Deaths", ImGuiDataType_U16, &gSaveContext.deaths);
    Tooltip("Total number of deaths");
    PopStyleInput();

    Checkbox("Has BGS", (bool*)&gSaveContext.bgsFlag,
             checkboxOptionsBase.Tooltip("Is Biggoron sword unlocked? Replaces Giant's knife"));

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Sword Health", ImGuiDataType_U16, &gSaveContext.swordHealth);
    Tooltip("Giant's knife health. Default is 8. Must be >0 for Biggoron sword to work");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Bgs Day Count", ImGuiDataType_S32, &gSaveContext.bgsDayCount);
    Tooltip("Total number of days elapsed since giving Biggoron the claim check");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Entrance Index", ImGuiDataType_S32, &gSaveContext.entranceIndex);
    Tooltip("From which entrance did Link arrive?");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Cutscene Index", ImGuiDataType_S32, &gSaveContext.cutsceneIndex);
    Tooltip("Which cutscene is this?");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Navi Timer", ImGuiDataType_U16, &gSaveContext.naviTimer);
    Tooltip("Navi wants to talk at 600 units, decides not to at 3000.");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Timer State", ImGuiDataType_S16, &gSaveContext.timerState);
    Tooltip("Heat timer, race timer, etc. Has white font");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Timer Seconds", ImGuiDataType_S16, &gSaveContext.timerSeconds, &one, NULL);
    Tooltip("Time, in seconds");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Sub-Timer State", ImGuiDataType_S16, &gSaveContext.subTimerState);
    Tooltip("Trade timer, Ganon collapse timer, etc. Has yellow font");
    PopStyleInput();

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("Sub-Timer Seconds", ImGuiDataType_S16, &gSaveContext.subTimerSeconds, &one, NULL);
    Tooltip("Time, in seconds");
    PopStyleInput();

    Combobox("Audio", &gSaveContext.audioSetting, audioMap, comboboxOptionsBase.Tooltip("Sound setting"));

    Checkbox("64 DD file?", (bool*)&gSaveContext.n64ddFlag,
             checkboxOptionsBase.Tooltip("WARNING! If you save, your file may be locked! Use caution!"));

    Combobox("Z Target Mode", &gSaveContext.zTargetSetting, zTargetMap,
             comboboxOptionsBase.Tooltip("Z-Targeting behavior"));

    if (IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT)) {
        PushStyleInput(THEME_COLOR);
        ImGui::InputScalar("Triforce Pieces", ImGuiDataType_U8,
                           &gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected);
        Tooltip("Currently obtained Triforce Pieces. For Triforce Hunt.");
        PopStyleInput();
    }

    ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
    static std::array<const char*, 7> minigameHS = { "Horseback Archery", "Big Poe Points",
                                                     "Fishing",           "Malon's Obstacle Course",
                                                     "Running Man Race",  "?",
                                                     "Dampe's Race" };

    if (ImGui::TreeNode("Minigames")) {
        for (int i = 0; i < 7; i++) {
            if (i == 2 && ImGui::TreeNode("Fishing")) { // fishing has a few more flags to it
                u8 fishSize = gSaveContext.highScores[i] & 0x7F;
                PushStyleInput(THEME_COLOR);
                if (ImGui::InputScalar("Child Size Record", ImGuiDataType_U8, &fishSize)) {
                    gSaveContext.highScores[i] &= ~0x7F;
                    gSaveContext.highScores[i] |= fishSize & 0x7F;
                }
                char fishMsg[64];
                std::sprintf(fishMsg, "Weight: %2.0f lbs", ((SQ(fishSize) * .0036) + .5));
                Tooltip(fishMsg);
                PopStyleInput();
                bool FishBool = gSaveContext.highScores[i] & 0x80;
                if (Checkbox("Cheated as Child", &FishBool,
                             checkboxOptionsBase.Tooltip("Used the Sinking lure to catch it."))) {
                    gSaveContext.highScores[i] &= ~0x80;
                    gSaveContext.highScores[i] |= (0x80 * FishBool);
                }
                fishSize = (gSaveContext.highScores[i] & 0x7F000000) >> 0x18;
                PushStyleInput(THEME_COLOR);
                if (ImGui::InputScalar("Adult Size Record", ImGuiDataType_U8, &fishSize)) {
                    gSaveContext.highScores[i] &= ~0x7F000000;
                    gSaveContext.highScores[i] |= (fishSize & 0x7F) << 0x18;
                }
                std::sprintf(fishMsg, "Weight: %2.0f lbs", ((SQ(fishSize) * .0036) + .5));
                Tooltip(fishMsg);
                PopStyleInput();
                FishBool = gSaveContext.highScores[i] & 0x80000000;
                if (Checkbox("Cheated as Adult", &FishBool,
                             checkboxOptionsBase.Tooltip("Used the Sinking lure to catch it."))) {
                    gSaveContext.highScores[i] &= ~0x80000000;
                    gSaveContext.highScores[i] |= (0x80000000 * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & 0x100;
                if (Checkbox("Played as Child", &FishBool,
                             checkboxOptionsBase.Tooltip("Played at least one game as a child"))) {
                    gSaveContext.highScores[i] &= ~0x100;
                    gSaveContext.highScores[i] |= (0x100 * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & 0x200;
                if (Checkbox("Played as Adult", &FishBool,
                             checkboxOptionsBase.Tooltip("Played at least one game as an adult"))) {
                    gSaveContext.highScores[i] &= ~0x200;
                    gSaveContext.highScores[i] |= (0x200 * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & 0x400;
                if (Checkbox(
                        "Got Prize as Child", &FishBool,
                        checkboxOptionsBase.Tooltip(
                            "Got the prize item (Heart Piece, unless rando.)\nunlocks Sinking Lure for Child Link."))) {
                    gSaveContext.highScores[i] &= ~0x400;
                    gSaveContext.highScores[i] |= (0x400 * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & 0x800;
                if (Checkbox("Got Prize as Adult", &FishBool,
                             checkboxOptionsBase.Tooltip("Got the prize item (Golden Scale, unless rando.)\nUnlocks "
                                                         "Sinking Lure for Adult Link."))) {
                    gSaveContext.highScores[i] &= ~0x800;
                    gSaveContext.highScores[i] |= (0x800 * FishBool);
                }
                FishBool = gSaveContext.highScores[i] & 0x1000;
                if (Checkbox("Stole Owner's Hat", &FishBool,
                             checkboxOptionsBase.Tooltip("The owner's now visibly bald when Adult Link."))) {
                    gSaveContext.highScores[i] &= ~0x1000;
                    gSaveContext.highScores[i] |= (0x1000 * FishBool);
                }
                fishSize = (gSaveContext.highScores[i] & 0xFF0000) >> 16;
                PushStyleInput(THEME_COLOR);
                if (ImGui::InputScalar("Times Played", ImGuiDataType_U8, &fishSize)) {
                    gSaveContext.highScores[i] &= ~0xFF0000;
                    gSaveContext.highScores[i] |= (fishSize) << 16;
                }
                Tooltip("Determines weather and school size during dawn/dusk.");
                PopStyleInput();

                ImGui::TreePop();
                continue;
            }

            if (i == 5 || i == 2) { // HS_UNK_05 is unused
                continue;
            }
            std::string minigameLbl = minigameHS[i];
            PushStyleInput(THEME_COLOR);
            ImGui::InputScalar(minigameLbl.c_str(), ImGuiDataType_S32, &gSaveContext.highScores[i], &one, NULL);
            PopStyleInput();
        }

        ImGui::TreePop();
    }

    ImGui::PopItemWidth();
}

void DrawBGSItemFlag(uint8_t itemID) {
    const ItemMapEntry& slotEntry = itemMapping[itemID];
    ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(slotEntry.name),
                 ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1));
}

void DrawInventoryTab() {
    static bool restrictToValid = true;

    Checkbox(
        "Restrict to valid items", &restrictToValid,
        checkboxOptionsBase.Tooltip("Restricts items and ammo to only what is possible to legally acquire in-game"));

    for (int32_t y = 0; y < 4; y++) {
        for (int32_t x = 0; x < 6; x++) {
            int32_t index = x + y * 6;
            static int32_t selectedIndex = -1;
            static const char* itemPopupPicker = "itemPopupPicker";

            ImGui::PushID(index);

            if (x != 0) {
                ImGui::SameLine();
            }

            uint8_t item = gSaveContext.inventory.items[index];
            PushStyleButton(Colors::DarkGray);
            if (item != ITEM_NONE) {
                const ItemMapEntry& slotEntry = itemMapping.find(item)->second;
                auto ret = ImGui::ImageButton(
                    slotEntry.name.c_str(),
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(slotEntry.name),
                    ImVec2(48.0f, 48.0f), ImVec2(0, 0), ImVec2(1, 1));
                if (ret) {
                    selectedIndex = index;
                    ImGui::OpenPopup(itemPopupPicker);
                }
            } else {
                if (ImGui::Button("##itemNone", ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
                    selectedIndex = index;
                    ImGui::OpenPopup(itemPopupPicker);
                }
            }
            PopStyleButton();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            if (ImGui::BeginPopup(itemPopupPicker)) {
                PushStyleButton(Colors::DarkGray);
                if (ImGui::Button("##itemNonePicker",
                                  ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
                    gSaveContext.inventory.items[selectedIndex] = ITEM_NONE;
                    ImGui::CloseCurrentPopup();
                }
                PopStyleButton();
                UIWidgets::Tooltip("None");

                std::vector<ItemMapEntry> possibleItems;
                if (restrictToValid) {
                    // Scan gItemSlots to find legal items for this slot. Bottles are a special case
                    for (int slotIndex = 0; slotIndex < 56; slotIndex++) {
                        int testIndex = (selectedIndex == SLOT_BOTTLE_1 || selectedIndex == SLOT_BOTTLE_2 ||
                                         selectedIndex == SLOT_BOTTLE_3 || selectedIndex == SLOT_BOTTLE_4)
                                            ? SLOT_BOTTLE_1
                                            : selectedIndex;
                        if (gItemSlots[slotIndex] == testIndex) {
                            possibleItems.push_back(itemMapping[slotIndex]);
                        }
                    }
                } else {
                    for (const auto& entry : itemMapping) {
                        possibleItems.push_back(entry.second);
                    }
                }

                for (int32_t pickerIndex = 0; pickerIndex < possibleItems.size(); pickerIndex++) {
                    if (((pickerIndex + 1) % 8) != 0) {
                        ImGui::SameLine();
                    }
                    const ItemMapEntry& slotEntry = possibleItems[pickerIndex];
                    PushStyleButton(Colors::DarkGray);
                    auto ret = ImGui::ImageButton(
                        slotEntry.name.c_str(),
                        Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(slotEntry.name),
                        ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
                    PopStyleButton();
                    if (ret) {
                        gSaveContext.inventory.items[selectedIndex] = slotEntry.id;
                        ImGui::CloseCurrentPopup();
                    }
                    UIWidgets::Tooltip(SohUtils::GetItemName(slotEntry.id).c_str());
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::PopID();
        }
    }

    ImGui::Text("Ammo");
    for (uint32_t ammoIndex = 0, drawnAmmoItems = 0; ammoIndex < 16; ammoIndex++) {
        uint8_t item = (restrictToValid) ? gAmmoItems[ammoIndex] : gAllAmmoItems[ammoIndex];
        if (item != ITEM_NONE) {
            // For legal items, display as 1 row of 7. For unrestricted items, display rows of 6 to match
            // inventory
            if ((restrictToValid && (drawnAmmoItems != 0)) || ((drawnAmmoItems % 6) != 0)) {
                ImGui::SameLine();
            }
            drawnAmmoItems++;

            ImGui::PushID(ammoIndex);
            ImGui::PushItemWidth(IMAGE_SIZE);
            ImGui::BeginGroup();

            ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[item].name),
                         ImVec2(IMAGE_SIZE, IMAGE_SIZE));
            PushStyleInput(THEME_COLOR);
            ImGui::InputScalar("##ammoInput", ImGuiDataType_S8, &AMMO(item));
            PopStyleInput();

            ImGui::EndGroup();
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
    }

    // Trade quest flags are only used when shuffling the trade sequence, so
    // don't show this if it isn't needed.
    if (IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE) &&
        ImGui::TreeNode("Adult trade quest items")) {
        for (int i = ITEM_POCKET_EGG; i <= ITEM_CLAIM_CHECK; i++) {
            DrawBGSItemFlag(i);
        }
        ImGui::TreePop();
    }
}

// Draw a flag bitfield as an grid of checkboxes
void DrawFlagTableArray16(const FlagTable& flagTable, uint16_t row, uint16_t& flags) {
    ImGui::PushID((std::to_string(row) + flagTable.name).c_str());
    for (int32_t flagIndex = 15; flagIndex >= 0; flagIndex--) {
        ImGui::SameLine();
        ImGui::PushID(flagIndex);
        bool hasDescription = !!flagTable.flagDescriptions.contains(row * 16 + flagIndex);
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
        if (ImGui::IsItemHovered() && hasDescription) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", UIWidgets::WrappedText(flagTable.flagDescriptions.at(row * 16 + flagIndex), 60).c_str());
            ImGui::EndTooltip();
        }
        ImGui::PopID();
    }
    ImGui::PopID();
}

void DrawFlagsTab() {
    if (ImGui::TreeNode("Player State")) {
        if (gPlayState != nullptr) {
            Player* player = GET_PLAYER(gPlayState);

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("stateFlags1");
                    DrawFlagArray32("stateFlags1", player->stateFlags1, THEME_COLOR);
                },
                "stateFlags1");

            ImGui::SameLine();

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("stateFlags2");
                    DrawFlagArray32("stateFlags2", player->stateFlags2, THEME_COLOR);
                },
                "stateFlags2");

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("stateFlags3");
                    DrawFlagArray8("stateFlags3", player->stateFlags3, THEME_COLOR);
                },
                "stateFlags3");

            ImGui::SameLine();

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("unk_6AE_rotFlags");
                    DrawFlagArray16("unk_6AE_rotFlags", player->unk_6AE_rotFlags, THEME_COLOR);
                },
                "unk_6AE_rotFlags");
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Current Scene")) {
        if (gPlayState != nullptr) {
            ActorContext* act = &gPlayState->actorCtx;
            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("Switch");
                    InsertHelpHoverText("Permanently-saved switch flags");
                    if (Button("Set All##Switch", buttonOptionsBase.Tooltip(""))) {
                        act->flags.swch = UINT32_MAX;
                    }
                    ImGui::SameLine();
                    if (Button("Clear All##Switch", buttonOptionsBase.Tooltip(""))) {
                        act->flags.swch = 0;
                    }
                    DrawFlagArray32("Switch", act->flags.swch, THEME_COLOR);
                },
                "Switch");

            ImGui::SameLine();

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("Temp Switch");
                    InsertHelpHoverText("Temporary switch flags. Unset on scene transitions");
                    if (Button("Set All##Temp Switch", buttonOptionsBase.Tooltip(""))) {
                        act->flags.tempSwch = UINT32_MAX;
                    }
                    ImGui::SameLine();
                    if (Button("Clear All##Temp Switch", buttonOptionsBase.Tooltip(""))) {
                        act->flags.tempSwch = 0;
                    }
                    DrawFlagArray32("Temp Switch", act->flags.tempSwch, THEME_COLOR);
                },
                "Temp Switch");

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("Clear");
                    InsertHelpHoverText("Permanently-saved room-clear flags");
                    if (Button("Set All##Clear", buttonOptionsBase.Tooltip(""))) {
                        act->flags.clear = UINT32_MAX;
                    }
                    ImGui::SameLine();
                    if (Button("Clear All##Clear", buttonOptionsBase.Tooltip(""))) {
                        act->flags.clear = 0;
                    }
                    DrawFlagArray32("Clear", act->flags.clear, THEME_COLOR);
                },
                "Clear");

            ImGui::SameLine();

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("Temp Clear");
                    InsertHelpHoverText("Temporary room-clear flags. Unset on scene transitions");
                    if (Button("Set All##Temp Clear", buttonOptionsBase.Tooltip(""))) {
                        act->flags.tempClear = UINT32_MAX;
                    }
                    ImGui::SameLine();
                    if (Button("Clear All##Temp Clear", buttonOptionsBase.Tooltip(""))) {
                        act->flags.tempClear = 0;
                    }
                    DrawFlagArray32("Temp Clear", act->flags.tempClear, THEME_COLOR);
                },
                "Temp Clear");

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("Collect");
                    InsertHelpHoverText("Permanently-saved collect flags");
                    if (Button("Set All##Collect", buttonOptionsBase.Tooltip(""))) {
                        act->flags.collect = UINT32_MAX;
                    }
                    ImGui::SameLine();
                    if (Button("Clear All##Collect", buttonOptionsBase.Tooltip(""))) {
                        act->flags.collect = 0;
                    }
                    DrawFlagArray32("Collect", act->flags.collect, THEME_COLOR);
                },
                "Collect");

            ImGui::SameLine();

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("Temp Collect");
                    InsertHelpHoverText("Temporary collect flags. Unset on scene transitions");
                    if (Button("Set All##Temp Collect", buttonOptionsBase.Tooltip(""))) {
                        act->flags.tempCollect = UINT32_MAX;
                    }
                    ImGui::SameLine();
                    if (Button("Clear All##Temp Collect", buttonOptionsBase.Tooltip(""))) {
                        act->flags.tempCollect = 0;
                    }
                    DrawFlagArray32("Temp Collect", act->flags.tempCollect, THEME_COLOR);
                },
                "Temp Collect");

            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("Chest");
                    InsertHelpHoverText("Permanently-saved chest flags");
                    if (Button("Set All##Chest", buttonOptionsBase.Tooltip(""))) {
                        act->flags.chest = UINT32_MAX;
                    }
                    ImGui::SameLine();
                    if (Button("Clear All##Chest", buttonOptionsBase.Tooltip(""))) {
                        act->flags.chest = 0;
                    }
                    DrawFlagArray32("Chest", act->flags.chest, THEME_COLOR);
                },
                "Chest");

            ImGui::SameLine();

            ImGui::BeginGroup();

            if (Button("Reload Flags", buttonOptionsBase.Tooltip(
                                           "Load flags from saved scene flags. Normally happens on scene load"))) {
                act->flags.swch = gSaveContext.sceneFlags[gPlayState->sceneNum].swch;
                act->flags.clear = gSaveContext.sceneFlags[gPlayState->sceneNum].clear;
                act->flags.collect = gSaveContext.sceneFlags[gPlayState->sceneNum].collect;
                act->flags.chest = gSaveContext.sceneFlags[gPlayState->sceneNum].chest;
            }

            if (Button("Save Flags",
                       buttonOptionsBase.Tooltip("Save current scene flags. Normally happens on scene exit"))) {
                gSaveContext.sceneFlags[gPlayState->sceneNum].swch = act->flags.swch;
                gSaveContext.sceneFlags[gPlayState->sceneNum].clear = act->flags.clear;
                gSaveContext.sceneFlags[gPlayState->sceneNum].collect = act->flags.collect;
                gSaveContext.sceneFlags[gPlayState->sceneNum].chest = act->flags.chest;
            }

            if (Button("Clear Flags",
                       buttonOptionsBase.Tooltip("Clear current scene flags. Reload scene to see changes"))) {
                act->flags.swch = 0;
                act->flags.clear = 0;
                act->flags.collect = 0;
                act->flags.chest = 0;
            }

            ImGui::EndGroup();
        } else {
            ImGui::Text("Current game state does not have an active scene");
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Saved Scene Flags")) {
        static uint32_t selectedSceneFlagMap = 0;
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
            if (Button("Current", buttonOptionsBase.Tooltip("Open flags for current scene"))) {
                selectedSceneFlagMap = gPlayState->sceneNum;
            }
        }

        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Switch");
                InsertHelpHoverText("Switch flags");
                DrawFlagArray32("Switch", gSaveContext.sceneFlags[selectedSceneFlagMap].swch, THEME_COLOR);
            },
            "Saved Switch");

        ImGui::SameLine();

        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Clear");
                InsertHelpHoverText("Room-clear flags");
                DrawFlagArray32("Clear", gSaveContext.sceneFlags[selectedSceneFlagMap].clear, THEME_COLOR);
            },
            "Saved Clear");

        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Collect");
                InsertHelpHoverText("Collect flags");
                DrawFlagArray32("Collect", gSaveContext.sceneFlags[selectedSceneFlagMap].collect, THEME_COLOR);
            },
            "Saved Collect");

        ImGui::SameLine();

        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Chest");
                InsertHelpHoverText("Chest flags");
                DrawFlagArray32("Chest", gSaveContext.sceneFlags[selectedSceneFlagMap].chest, THEME_COLOR);
            },
            "Saved Chest");

        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Rooms");
                InsertHelpHoverText("Flags for visted rooms");
                DrawFlagArray32("Rooms", gSaveContext.sceneFlags[selectedSceneFlagMap].rooms, THEME_COLOR);
            },
            "Saved Rooms");

        ImGui::SameLine();

        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Floors");
                InsertHelpHoverText("Flags for visted floors");
                DrawFlagArray32("Floors", gSaveContext.sceneFlags[selectedSceneFlagMap].floors, THEME_COLOR);
            },
            "Saved Floors");

        ImGui::TreePop();
    }

    DrawGroupWithBorder(
        [&]() {
            size_t selectedGsMap = 0;
            ImGui::Text("Gold Skulltulas");
            Combobox("Map##Gold Skulltulas", &selectedGsMap, gsMapping, comboboxOptionsBase.Tooltip(""));

            // TODO We should write out descriptions for each one... ugh
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
                        // Have to do this roundabout method as the macro does not support clearing flags
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
                         checkboxOptionsBase.Tooltip(
                             "Automatically adjust the number of gold skulltula tokens acquired based on set flags."));
                int32_t gsCount = 0;
                if (keepGsCountUpdated) {
                    for (int32_t gsFlagIndex = 0; gsFlagIndex < 6; gsFlagIndex++) {
                        gsCount += std::popcount(static_cast<uint32_t>(gSaveContext.gsFlags[gsFlagIndex]));
                    }
                    gSaveContext.inventory.gsTokens = gsCount;
                }
            }
        },
        "Gold Skulltulas");

    for (int i = 0; i < flagTables.size(); i++) {
        const FlagTable& flagTable = flagTables[i];
        if (flagTable.flagTableType == RANDOMIZER_INF && !IS_RANDO && !IS_BOSS_RUSH) {
            continue;
        }

        if (ImGui::TreeNode(flagTable.name)) {
            for (int j = 0; j < flagTable.size + 1; j++) {
                DrawGroupWithBorder(
                    [&]() {
                        ImGui::Text("%s", fmt::format("{:<2x}", j).c_str());
                        switch (flagTable.flagTableType) {
                            case EVENT_CHECK_INF:
                                DrawFlagTableArray16(flagTable, j, gSaveContext.eventChkInf[j]);
                                break;
                            case ITEM_GET_INF:
                                DrawFlagTableArray16(flagTable, j, gSaveContext.itemGetInf[j]);
                                break;
                            case INF_TABLE:
                                DrawFlagTableArray16(flagTable, j, gSaveContext.infTable[j]);
                                break;
                            case EVENT_INF:
                                DrawFlagTableArray16(flagTable, j, gSaveContext.eventInf[j]);
                                break;
                            case RANDOMIZER_INF:
                                DrawFlagTableArray16(flagTable, j, gSaveContext.ship.randomizerInf[j]);
                                break;
                        }
                    },
                    flagTable.name);
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

            ImGui::TreePop();
        }
    }
}

// Draws a combo that lets you choose and upgrade value from a drop-down of text values
void DrawUpgrade(const std::string& categoryName, int32_t categoryId, const std::vector<std::string>& names) {
    ImGui::Text("%s", categoryName.c_str());
    ImGui::SameLine();
    ImGui::PushID(categoryName.c_str());
    PushStyleCombobox(THEME_COLOR);
    ImGui::AlignTextToFramePadding();
    if (ImGui::BeginCombo("##upgrade", names[CUR_UPG_VALUE(categoryId)].c_str())) {
        for (int32_t i = 0; i < names.size(); i++) {
            if (ImGui::Selectable(names[i].c_str())) {
                Inventory_ChangeUpgrade(categoryId, i);
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
    static const char* upgradePopupPicker = "upgradePopupPicker";

    ImGui::PushID(categoryName.c_str());

    PushStyleButton(Colors::DarkGray);
    uint8_t item = items[CUR_UPG_VALUE(categoryId)];
    if (item != ITEM_NONE) {
        const ItemMapEntry& slotEntry = itemMapping[item];
        if (ImGui::ImageButton(slotEntry.name.c_str(),
                               Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(slotEntry.name),
                               ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1))) {
            ImGui::OpenPopup(upgradePopupPicker);
        }
    } else {
        if (ImGui::Button("##itemNone", ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
            ImGui::OpenPopup(upgradePopupPicker);
        }
    }
    PopStyleButton();
    Tooltip(categoryName.c_str());

    if (ImGui::BeginPopup(upgradePopupPicker)) {
        for (int32_t pickerIndex = 0; pickerIndex < items.size(); pickerIndex++) {
            if ((pickerIndex % 8) != 0) {
                ImGui::SameLine();
            }

            PushStyleButton(Colors::DarkGray);
            if (items[pickerIndex] == ITEM_NONE) {
                if (ImGui::Button("##upgradePopupPicker",
                                  ImVec2(IMAGE_SIZE, IMAGE_SIZE) + ImGui::GetStyle().FramePadding * 2)) {
                    Inventory_ChangeUpgrade(categoryId, pickerIndex);
                    ImGui::CloseCurrentPopup();
                }
                Tooltip("None");
            } else {
                const ItemMapEntry& slotEntry = itemMapping[items[pickerIndex]];
                auto ret = ImGui::ImageButton(
                    slotEntry.name.c_str(),
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(slotEntry.name),
                    ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
                if (ret) {
                    Inventory_ChangeUpgrade(categoryId, pickerIndex);
                    ImGui::CloseCurrentPopup();
                }
                Tooltip(SohUtils::GetItemName(slotEntry.id).c_str());
            }
            PopStyleButton();
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void DrawEquipmentTab() {
    const std::vector<uint8_t> equipmentValues = {
        ITEM_SWORD_KOKIRI, ITEM_SWORD_MASTER,  ITEM_SWORD_BGS,     ITEM_SWORD_BROKEN,
        ITEM_SHIELD_DEKU,  ITEM_SHIELD_HYLIAN, ITEM_SHIELD_MIRROR, ITEM_NONE,
        ITEM_TUNIC_KOKIRI, ITEM_TUNIC_GORON,   ITEM_TUNIC_ZORA,    ITEM_NONE,
        ITEM_BOOTS_KOKIRI, ITEM_BOOTS_IRON,    ITEM_BOOTS_HOVER,   ITEM_NONE,
    };
    for (int32_t i = 0; i < equipmentValues.size(); i++) {
        // Skip over unused 4th slots for shields, boots, and tunics
        if (equipmentValues[i] == ITEM_NONE) {
            continue;
        }
        if ((i % 4) != 0) {
            ImGui::SameLine();
        }

        ImGui::PushID(i);
        uint32_t bitMask = 1 << i;
        bool hasEquip = (bitMask & gSaveContext.inventory.equipment) != 0;
        const ItemMapEntry& entry = itemMapping[equipmentValues[i]];
        PushStyleButton(Colors::DarkGray);
        auto ret = ImGui::ImageButton(entry.name.c_str(),
                                      Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                          hasEquip ? entry.name : entry.nameFaded),
                                      ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
        if (ret) {
            if (hasEquip) {
                gSaveContext.inventory.equipment &= ~bitMask;
            } else {
                gSaveContext.inventory.equipment |= bitMask;
            }
        }
        PopStyleButton();
        Tooltip(SohUtils::GetItemName(entry.id).c_str());
        ImGui::PopID();
    }

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
    // this was const, but I needed to append to it depending in rando settings.
    std::vector<std::string> walletNamesImpl = {
        "Child (99)",
        "Adult (200)",
        "Giant (500)",
    };
    // only display Tycoon wallet if you're in a save file that would allow it.
    if (IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_INCLUDE_TYCOON_WALLET)) {
        const std::string walletName = "Tycoon (999)";
        walletNamesImpl.push_back(walletName);
    }
    // copy it to const value for display in ImGui.
    const std::vector<std::string> walletNames = walletNamesImpl;
    DrawUpgrade("Wallet", UPG_WALLET, walletNames);

    const std::vector<std::string> stickNames = {
        "None",
        "10",
        "20",
        "30",
    };
    DrawUpgrade("Deku Stick Capacity", UPG_STICKS, stickNames);

    const std::vector<std::string> nutNames = {
        "None",
        "20",
        "30",
        "40",
    };
    DrawUpgrade("Deku Nut Capacity", UPG_NUTS, nutNames);
}

// Draws a toggleable icon for a quest item that is faded when disabled
void DrawQuestItemButton(uint32_t item) {
    const QuestMapEntry& entry = questMapping[item];
    uint32_t bitMask = 1 << entry.id;
    bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
    PushStyleButton(Colors::DarkGray);
    auto ret = ImGui::ImageButton(entry.name.c_str(),
                                  Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                      hasQuestItem ? entry.name : entry.nameFaded),
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
    const ItemMapEntry& entry = itemMapping[item];
    uint32_t bitMask = 1 << (entry.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[scene]) != 0;
    PushStyleButton(Colors::DarkGray);
    auto ret = ImGui::ImageButton(
        entry.name.c_str(),
        Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasItem ? entry.name : entry.nameFaded),
        ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 0), ImVec2(1, 1));
    if (ret) {
        if (hasItem) {
            gSaveContext.inventory.dungeonItems[scene] &= ~bitMask;
        } else {
            gSaveContext.inventory.dungeonItems[scene] |= bitMask;
        }
    }
    PopStyleButton();
    Tooltip(SohUtils::GetItemName(entry.id).c_str());
}

void DrawQuestStatusTab() {

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
    for (const auto& [quest, entry] : songMapping) {
        if ((entry.id != QUEST_SONG_MINUET) && (entry.id != QUEST_SONG_LULLABY)) {
            ImGui::SameLine();
        }

        uint32_t bitMask = 1 << entry.id;
        bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
        PushStyleButton(Colors::DarkGray);
        auto ret = ImGui::ImageButton(entry.name.c_str(),
                                      Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                          hasQuestItem ? entry.name : entry.nameFaded),
                                      ImVec2(32.0f, 48.0f), ImVec2(0, 0), ImVec2(1, 1));
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

    PushStyleInput(THEME_COLOR);
    ImGui::InputScalar("GS Count", ImGuiDataType_S16, &gSaveContext.inventory.gsTokens);
    PopStyleInput();
    InsertHelpHoverText("Number of gold skulltula tokens aquired");

    uint32_t bitMask = 1 << QUEST_SKULL_TOKEN;
    bool gsUnlocked = (bitMask & gSaveContext.inventory.questItems) != 0;
    if (Checkbox("GS unlocked", &gsUnlocked, CheckboxOptions().Color(THEME_COLOR))) {
        if (gsUnlocked) {
            gSaveContext.inventory.questItems |= bitMask;
        } else {
            gSaveContext.inventory.questItems &= ~bitMask;
        }
    }
    InsertHelpHoverText("If unlocked, enables showing the gold skulltula count in the quest status menu");

    int32_t pohCount = (gSaveContext.inventory.questItems & 0xF0000000) >> 28;
    PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("PoH count", std::to_string(pohCount).c_str())) {
        for (int32_t i = 0; i < 4; i++) {
            if (ImGui::Selectable(std::to_string(i).c_str(), pohCount == i)) {
                gSaveContext.inventory.questItems &= ~0xF0000000;
                gSaveContext.inventory.questItems |= (i << 28);
            }
        }
        ImGui::EndCombo();
    }
    InsertHelpHoverText("The number of pieces of heart acquired towards the next heart container");
    PopStyleCombobox();

    DrawGroupWithBorder(
        [&]() {
            ImGui::Text("Dungeon Items");

            static int32_t dungeonItemsScene = SCENE_DEKU_TREE;
            PushStyleCombobox(THEME_COLOR);
            if (ImGui::BeginCombo("##DungeonSelect", SohUtils::GetSceneName(dungeonItemsScene).c_str())) {
                for (int32_t dungeonIndex = SCENE_DEKU_TREE; dungeonIndex < SCENE_JABU_JABU_BOSS + 1; dungeonIndex++) {
                    if (ImGui::Selectable(SohUtils::GetSceneName(dungeonIndex).c_str(),
                                          dungeonIndex == dungeonItemsScene)) {
                        dungeonItemsScene = dungeonIndex;
                    }
                }

                ImGui::EndCombo();
            }
            PopStyleCombobox();

            DrawDungeonItemButton(ITEM_KEY_BOSS, dungeonItemsScene);
            ImGui::SameLine();
            DrawDungeonItemButton(ITEM_COMPASS, dungeonItemsScene);
            ImGui::SameLine();
            DrawDungeonItemButton(ITEM_DUNGEON_MAP, dungeonItemsScene);

            if (dungeonItemsScene != SCENE_JABU_JABU_BOSS) {
                float lineHeight = ImGui::GetTextLineHeightWithSpacing();
                ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                 itemMapping[ITEM_KEY_SMALL].name),
                             ImVec2(lineHeight, lineHeight));
                ImGui::SameLine();
                PushStyleInput(THEME_COLOR);
                if (ImGui::InputScalar("##Keys", ImGuiDataType_S8,
                                       gSaveContext.inventory.dungeonKeys + dungeonItemsScene)) {
                    gSaveContext.ship.stats.dungeonKeys[dungeonItemsScene] =
                        gSaveContext.inventory.dungeonKeys[dungeonItemsScene];
                };
                PopStyleInput();
            } else {
                // dungeonItems is size 20 but dungeonKeys is size 19, so there are no keys for the last scene
                // (Barinade's Lair)
                ImGui::Text("Barinade's Lair does not have small keys");
            }
        },
        "Dungeon Items");
}

void DrawPlayerTab() {
    if (gPlayState != nullptr) {
        Player* player = GET_PLAYER(gPlayState);
        const char* curSword;
        const char* curShield;
        const char* curTunic;
        const char* curBoots;

        switch (player->currentSwordItemId) {
            case ITEM_SWORD_KOKIRI:
                curSword = "Kokiri Sword";
                break;
            case ITEM_SWORD_MASTER:
                curSword = "Master Sword";
                break;
            case ITEM_SWORD_BGS:
                curSword = "Biggoron's Sword";
                break;
            case ITEM_FISHING_POLE:
                curSword = "Fishing Pole";
                break;
            case ITEM_NONE:
                curSword = "None";
                break;
            default:
                curSword = "None";
                break;
        }

        switch (player->currentShield) {
            case PLAYER_SHIELD_NONE:
                curShield = "None";
                break;
            case PLAYER_SHIELD_DEKU:
                curShield = "Deku Shield";
                break;
            case PLAYER_SHIELD_HYLIAN:
                curShield = "Hylian Shield";
                break;
            case PLAYER_SHIELD_MIRROR:
                curShield = "Mirror Shield";
                break;
            default:
                break;
        }

        switch (player->currentTunic) {
            case PLAYER_TUNIC_KOKIRI:
                curTunic = "Kokiri Tunic";
                break;
            case PLAYER_TUNIC_GORON:
                curTunic = "Goron Tunic";
                break;
            case PLAYER_TUNIC_ZORA:
                curTunic = "Zora Tunic";
                break;
            default:
                break;
        }

        switch (player->currentBoots) {
            case PLAYER_BOOTS_KOKIRI:
                curBoots = "Kokiri Boots";
                break;
            case PLAYER_BOOTS_IRON:
                curBoots = "Iron Boots";
                break;
            case PLAYER_BOOTS_HOVER:
                curBoots = "Hover Boots";
                break;
            default:
                break;
        }

        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        PushStyleInput(THEME_COLOR);
        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Link's Position");
                ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
                ImGui::InputScalar("X##Pos", ImGuiDataType_Float, &player->actor.world.pos.x);
                ImGui::InputScalar("Y##Pos", ImGuiDataType_Float, &player->actor.world.pos.y);
                ImGui::InputScalar("Z##Pos", ImGuiDataType_Float, &player->actor.world.pos.z);
                ImGui::PopItemWidth();
            },
            "Link's Position");
        ImGui::SameLine();
        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Link's Rotation");
                InsertHelpHoverText("For Link's rotation in relation to the world");
                ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
                ImGui::InputScalar("X##Rot", ImGuiDataType_S16, &player->actor.world.rot.x);
                ImGui::InputScalar("Y##Rot", ImGuiDataType_S16, &player->actor.world.rot.y);
                ImGui::InputScalar("Z##Rot", ImGuiDataType_S16, &player->actor.world.rot.z);
                ImGui::PopItemWidth();
            },
            "Link's Rotation");
        ImGui::SameLine();
        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Link's Model Rotation");
                InsertHelpHoverText("For Link's actual model");
                ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
                ImGui::InputScalar("X##ModRot", ImGuiDataType_S16, &player->actor.shape.rot.x);
                ImGui::InputScalar("Y##ModRot", ImGuiDataType_S16, &player->actor.shape.rot.y);
                ImGui::InputScalar("Z##ModRot", ImGuiDataType_S16, &player->actor.shape.rot.z);
                ImGui::PopItemWidth();
            },
            "Link's Model Rotation");

        ImGui::InputScalar("Linear Velocity", ImGuiDataType_Float, &player->linearVelocity);
        InsertHelpHoverText("Link's speed along the XZ plane");

        ImGui::InputScalar("Y Velocity", ImGuiDataType_Float, &player->actor.velocity.y);
        InsertHelpHoverText("Link's speed along the Y plane. Caps at -20");

        ImGui::InputScalar("Wall Height", ImGuiDataType_Float, &player->yDistToLedge);
        InsertHelpHoverText("Height used to determine whether Link can climb or grab a ledge at the top");

        ImGui::InputScalar("Invincibility Timer", ImGuiDataType_S8, &player->invincibilityTimer);
        InsertHelpHoverText("Can't take damage while this is nonzero");

        ImGui::InputScalar("Gravity", ImGuiDataType_Float, &player->actor.gravity);
        InsertHelpHoverText("Rate at which Link falls. Default -4.0f");
        PopStyleInput();

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
        InsertHelpHoverText("This will change Link's age when you load a map");
        PopStyleCombobox();
        ImGui::Separator();

        DrawGroupWithBorder(
            [&]() {
                PushStyleCombobox(THEME_COLOR);
                ImGui::Text("Link's Current Equipment");
                ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
                if (ImGui::BeginCombo("Sword", curSword)) {
                    if (ImGui::Selectable("None")) {
                        player->currentSwordItemId = ITEM_NONE;
                        gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
                    }
                    if (ImGui::Selectable("Kokiri Sword")) {
                        player->currentSwordItemId = ITEM_SWORD_KOKIRI;
                        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);
                    }
                    if (ImGui::Selectable("Master Sword")) {
                        player->currentSwordItemId = ITEM_SWORD_MASTER;
                        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_MASTER);
                    }
                    if (ImGui::Selectable("Biggoron's Sword")) {
                        if (gSaveContext.bgsFlag) {
                            if (gSaveContext.swordHealth < 8) {
                                gSaveContext.swordHealth = 8;
                            }
                            player->currentSwordItemId = ITEM_SWORD_BGS;
                            gSaveContext.equips.buttonItems[0] = ITEM_SWORD_BGS;
                        } else {
                            if (gSaveContext.swordHealth < 8) {
                                gSaveContext.swordHealth = 8;
                            }
                            player->currentSwordItemId = ITEM_SWORD_BGS;
                            gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KNIFE;
                        }

                        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_BIGGORON);
                    }
                    if (ImGui::Selectable("Fishing Pole")) {
                        player->currentSwordItemId = ITEM_FISHING_POLE;
                        gSaveContext.equips.buttonItems[0] = ITEM_FISHING_POLE;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_MASTER);
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::BeginCombo("Shield", curShield)) {
                    if (ImGui::Selectable("None")) {
                        player->currentShield = PLAYER_SHIELD_NONE;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_NONE);
                    }
                    if (ImGui::Selectable("Deku Shield")) {
                        player->currentShield = PLAYER_SHIELD_DEKU;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_DEKU);
                    }
                    if (ImGui::Selectable("Hylian Shield")) {
                        player->currentShield = PLAYER_SHIELD_HYLIAN;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_HYLIAN);
                    }
                    if (ImGui::Selectable("Mirror Shield")) {
                        player->currentShield = PLAYER_SHIELD_MIRROR;
                        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_MIRROR);
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::BeginCombo("Tunic", curTunic)) {
                    if (ImGui::Selectable("Kokiri Tunic")) {
                        player->currentTunic = PLAYER_TUNIC_KOKIRI;
                        Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, EQUIP_VALUE_TUNIC_KOKIRI);
                    }
                    if (ImGui::Selectable("Goron Tunic")) {
                        player->currentTunic = PLAYER_TUNIC_GORON;
                        Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, EQUIP_VALUE_TUNIC_GORON);
                    }
                    if (ImGui::Selectable("Zora Tunic")) {
                        player->currentTunic = PLAYER_TUNIC_ZORA;
                        Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, EQUIP_VALUE_TUNIC_ZORA);
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::BeginCombo("Boots", curBoots)) {
                    if (ImGui::Selectable("Kokiri Boots")) {
                        player->currentBoots = PLAYER_BOOTS_KOKIRI;
                        Inventory_ChangeEquipment(EQUIP_TYPE_BOOTS, EQUIP_VALUE_BOOTS_KOKIRI);
                    }
                    if (ImGui::Selectable("Iron Boots")) {
                        player->currentBoots = PLAYER_BOOTS_IRON;
                        Inventory_ChangeEquipment(EQUIP_TYPE_BOOTS, EQUIP_VALUE_BOOTS_IRON);
                    }
                    if (ImGui::Selectable("Hover Boots")) {
                        player->currentBoots = PLAYER_BOOTS_HOVER;
                        Inventory_ChangeEquipment(EQUIP_TYPE_BOOTS, EQUIP_VALUE_BOOTS_HOVER);
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();
                PopStyleCombobox();
            },
            "Current Equipment");
        ImGui::SameLine();

        ImU16 one = 1;
        DrawGroupWithBorder(
            [&]() {
                ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
                PushStyleInput(THEME_COLOR);
                ImGui::Text("Current Items");
                ImGui::InputScalar("B Button", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[0], &one, NULL);
                ImGui::InputScalar("C Left", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[1], &one, NULL);
                ImGui::InputScalar("C Down", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[2], &one, NULL);
                ImGui::InputScalar("C Right", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[3], &one, NULL);
                PopStyleInput();
                ImGui::PopItemWidth();
            },
            "Current Items");

        if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0)) {
            ImGui::SameLine();
            DrawGroupWithBorder(
                [&]() {
                    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
                    PushStyleInput(THEME_COLOR);
                    ImGui::Text("Current D-pad Items");
                    // Two spaces at the end for aligning, not elegant but it's working
                    ImGui::InputScalar("D-pad Up  ", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[4], &one, NULL);
                    ImGui::InputScalar("D-pad Down", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[5], &one, NULL);
                    ImGui::InputScalar("D-pad Left", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[6], &one, NULL);
                    ImGui::InputScalar("D-pad Right", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[7], &one,
                                       NULL);
                    PopStyleInput();
                    ImGui::PopItemWidth();
                },
                "Current D-pad Items");
        }

        ImGui::Text("Player State");
        uint8_t bit[32] = {};
        uint32_t flags[3] = { player->stateFlags1, player->stateFlags2, player->stateFlags3 };
        std::vector<std::vector<std::string>> flag_strs = { state1, state2, state3 };

        for (int j = 0; j <= 2; j++) {
            std::string label = fmt::format("State Flags {}", j + 1);
            DrawGroupWithBorder(
                [&]() {
                    ImGui::Text("%s", label.c_str());
                    std::vector<std::string> state = flag_strs[j];
                    for (int i = 0; i <= 31; i++) {
                        bit[i] = ((flags[j] >> i) & 1);
                        if (bit[i] != 0) {
                            ImGui::Text("%s", state[i].c_str());
                        }
                    }
                },
                label.c_str());
            ImGui::SameLine();
        }
        DrawGroupWithBorder(
            [&]() {
                ImGui::Text("Sword");
                ImGui::Text("  %d", player->meleeWeaponState);
            },
            "Sword");

    } else {
        ImGui::Text("Global Context needed for player info!");
    }
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

    if (ImGui::BeginTabBar("SaveContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        ResetBaseOptions();
        if (ImGui::BeginTabItem("Info")) {
            DrawInfoTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Inventory")) {
            DrawInventoryTab();
            ImGui::EndTabItem();
        }

        ResetBaseOptions();
        if (ImGui::BeginTabItem("Flags")) {
            DrawFlagsTab();
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
        if (ImGui::BeginTabItem("Player")) {
            DrawPlayerTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopFont();
    PopStyleTabs();
}

void SaveEditorWindow::InitElement() {
}

#include "debugSaveEditor.h"
#include "../../util.h"
#include "../libultraship/SohImGuiImpl.h"

#include <array>
#include <bit>
#include <map>
#include <string>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
}

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    std::string texturePath;
} ItemMapEntry;

#define ITEM_MAP_ENTRY(id)                              \
    {                                                   \
        id, {                                           \
            id, #id, #id "_Faded", static_cast<char*>(gItemIcons[id]) \
        }                                               \
    }

// Maps items ids to info for use in ImGui
std::map<uint32_t, ItemMapEntry> itemMapping = {
    ITEM_MAP_ENTRY(ITEM_STICK),
    ITEM_MAP_ENTRY(ITEM_NUT),
    ITEM_MAP_ENTRY(ITEM_BOMB),
    ITEM_MAP_ENTRY(ITEM_BOW),
    ITEM_MAP_ENTRY(ITEM_ARROW_FIRE),
    ITEM_MAP_ENTRY(ITEM_DINS_FIRE),
    ITEM_MAP_ENTRY(ITEM_SLINGSHOT),
    ITEM_MAP_ENTRY(ITEM_OCARINA_FAIRY),
    ITEM_MAP_ENTRY(ITEM_OCARINA_TIME),
    ITEM_MAP_ENTRY(ITEM_BOMBCHU),
    ITEM_MAP_ENTRY(ITEM_HOOKSHOT),
    ITEM_MAP_ENTRY(ITEM_LONGSHOT),
    ITEM_MAP_ENTRY(ITEM_ARROW_ICE),
    ITEM_MAP_ENTRY(ITEM_FARORES_WIND),
    ITEM_MAP_ENTRY(ITEM_BOOMERANG),
    ITEM_MAP_ENTRY(ITEM_LENS),
    ITEM_MAP_ENTRY(ITEM_BEAN),
    ITEM_MAP_ENTRY(ITEM_HAMMER),
    ITEM_MAP_ENTRY(ITEM_ARROW_LIGHT),
    ITEM_MAP_ENTRY(ITEM_NAYRUS_LOVE),
    ITEM_MAP_ENTRY(ITEM_BOTTLE),
    ITEM_MAP_ENTRY(ITEM_POTION_RED),
    ITEM_MAP_ENTRY(ITEM_POTION_GREEN),
    ITEM_MAP_ENTRY(ITEM_POTION_BLUE),
    ITEM_MAP_ENTRY(ITEM_FAIRY),
    ITEM_MAP_ENTRY(ITEM_FISH),
    ITEM_MAP_ENTRY(ITEM_MILK_BOTTLE),
    ITEM_MAP_ENTRY(ITEM_LETTER_RUTO),
    ITEM_MAP_ENTRY(ITEM_BLUE_FIRE),
    ITEM_MAP_ENTRY(ITEM_BUG),
    ITEM_MAP_ENTRY(ITEM_BIG_POE),
    ITEM_MAP_ENTRY(ITEM_MILK_HALF),
    ITEM_MAP_ENTRY(ITEM_POE),
    ITEM_MAP_ENTRY(ITEM_WEIRD_EGG),
    ITEM_MAP_ENTRY(ITEM_CHICKEN),
    ITEM_MAP_ENTRY(ITEM_LETTER_ZELDA),
    ITEM_MAP_ENTRY(ITEM_MASK_KEATON),
    ITEM_MAP_ENTRY(ITEM_MASK_SKULL),
    ITEM_MAP_ENTRY(ITEM_MASK_SPOOKY),
    ITEM_MAP_ENTRY(ITEM_MASK_BUNNY),
    ITEM_MAP_ENTRY(ITEM_MASK_GORON),
    ITEM_MAP_ENTRY(ITEM_MASK_ZORA),
    ITEM_MAP_ENTRY(ITEM_MASK_GERUDO),
    ITEM_MAP_ENTRY(ITEM_MASK_TRUTH),
    ITEM_MAP_ENTRY(ITEM_SOLD_OUT),
    ITEM_MAP_ENTRY(ITEM_POCKET_EGG),
    ITEM_MAP_ENTRY(ITEM_POCKET_CUCCO),
    ITEM_MAP_ENTRY(ITEM_COJIRO),
    ITEM_MAP_ENTRY(ITEM_ODD_MUSHROOM),
    ITEM_MAP_ENTRY(ITEM_ODD_POTION),
    ITEM_MAP_ENTRY(ITEM_SAW),
    ITEM_MAP_ENTRY(ITEM_SWORD_BROKEN),
    ITEM_MAP_ENTRY(ITEM_PRESCRIPTION),
    ITEM_MAP_ENTRY(ITEM_FROG),
    ITEM_MAP_ENTRY(ITEM_EYEDROPS),
    ITEM_MAP_ENTRY(ITEM_CLAIM_CHECK),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_FIRE),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_ICE),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_LIGHT),
    ITEM_MAP_ENTRY(ITEM_SWORD_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_SWORD_MASTER),
    ITEM_MAP_ENTRY(ITEM_SWORD_BGS),
    ITEM_MAP_ENTRY(ITEM_SHIELD_DEKU),
    ITEM_MAP_ENTRY(ITEM_SHIELD_HYLIAN),
    ITEM_MAP_ENTRY(ITEM_SHIELD_MIRROR),
    ITEM_MAP_ENTRY(ITEM_TUNIC_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_TUNIC_GORON),
    ITEM_MAP_ENTRY(ITEM_TUNIC_ZORA),
    ITEM_MAP_ENTRY(ITEM_BOOTS_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_BOOTS_IRON),
    ITEM_MAP_ENTRY(ITEM_BOOTS_HOVER),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_30),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_40),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_50),
    ITEM_MAP_ENTRY(ITEM_QUIVER_30),
    ITEM_MAP_ENTRY(ITEM_QUIVER_40),
    ITEM_MAP_ENTRY(ITEM_QUIVER_50),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_20),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_30),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_40),
    ITEM_MAP_ENTRY(ITEM_BRACELET),
    ITEM_MAP_ENTRY(ITEM_GAUNTLETS_SILVER),
    ITEM_MAP_ENTRY(ITEM_GAUNTLETS_GOLD),
    ITEM_MAP_ENTRY(ITEM_SCALE_SILVER),
    ITEM_MAP_ENTRY(ITEM_SCALE_GOLDEN),
    ITEM_MAP_ENTRY(ITEM_SWORD_KNIFE),
    ITEM_MAP_ENTRY(ITEM_WALLET_ADULT),
    ITEM_MAP_ENTRY(ITEM_WALLET_GIANT),
    ITEM_MAP_ENTRY(ITEM_SEEDS),
    ITEM_MAP_ENTRY(ITEM_FISHING_POLE),
    ITEM_MAP_ENTRY(ITEM_KEY_BOSS),
    ITEM_MAP_ENTRY(ITEM_COMPASS),
    ITEM_MAP_ENTRY(ITEM_DUNGEON_MAP),
    ITEM_MAP_ENTRY(ITEM_KEY_SMALL),
};

// Maps entries in the GS flag array to the area name it represents
std::vector<std::string> gsMapping = {
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

// Modification of gAmmoItems that replaces ITEM_NONE with the item in inventory slot it represents
u8 gAllAmmoItems[] = {
    ITEM_STICK,     ITEM_NUT,          ITEM_BOMB,    ITEM_BOW,      ITEM_ARROW_FIRE, ITEM_DINS_FIRE,
    ITEM_SLINGSHOT, ITEM_OCARINA_TIME, ITEM_BOMBCHU, ITEM_LONGSHOT, ITEM_ARROW_ICE,  ITEM_FARORES_WIND,
    ITEM_BOOMERANG, ITEM_LENS,         ITEM_BEAN,    ITEM_HAMMER,
};

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    std::string texturePath;
} QuestMapEntry;

#define QUEST_MAP_ENTRY(id, tex)                              \
    {                                                   \
        id, {                                           \
            id, #id, #id "_Faded", tex \
        }                                               \
    }

// Maps quest items ids to info for use in ImGui
std::map<uint32_t, QuestMapEntry> questMapping = {
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FOREST, gForestMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FIRE, gFireMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_WATER, gWaterMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SPIRIT, gSpiritMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SHADOW, gShadowMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_LIGHT, gLightMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_KOKIRI_EMERALD, gKokiriEmeraldIconTex),
    QUEST_MAP_ENTRY(QUEST_GORON_RUBY, gGoronRubyIconTex),
    QUEST_MAP_ENTRY(QUEST_ZORA_SAPPHIRE, gZoraSapphireIconTex),
    QUEST_MAP_ENTRY(QUEST_STONE_OF_AGONY, gStoneOfAgonyIconTex),
    QUEST_MAP_ENTRY(QUEST_GERUDO_CARD, gGerudosCardIconTex),
};

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    ImVec4 color;
} SongMapEntry;

#define SONG_MAP_ENTRY(id, r, g, b)       \
    {                                  \
            id, #id, #id "_Faded", ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f) \
    }

// Maps song ids to info for use in ImGui
std::array<SongMapEntry, 12> songMapping = { {
    SONG_MAP_ENTRY(QUEST_SONG_LULLABY,  255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_EPONA,    255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_SARIA,    255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_SUN,      255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_TIME,     255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_STORMS,   255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_MINUET,   150, 255, 100),
    SONG_MAP_ENTRY(QUEST_SONG_BOLERO,   255, 80,  40),
    SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    SONG_MAP_ENTRY(QUEST_SONG_REQUIEM,  255, 160, 0),
    SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    SONG_MAP_ENTRY(QUEST_SONG_PRELUDE,  255, 240, 100),
} };

// Adds a text tooltip for the previous ImGui item
void SetLastItemHoverText(const std::string& text) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(text.c_str());
        ImGui::EndTooltip();
    }
}

// Adds a "?" next to the previous ImGui item with a custom tooltip
void InsertHelpHoverText(const std::string& text) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "?");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(text.c_str());
        ImGui::EndTooltip();
    }
}

// Encapsulates what is drawn by the passed-in function within a border
template<typename T>
void DrawGroupWithBorder(T&& drawFunc) {
    // First group encapsulates the inner portion and border
    ImGui::BeginGroup();

    ImVec2 padding = ImGui::GetStyle().FramePadding;
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));

    // Second group encapsulates just the inner portion
    ImGui::BeginGroup();

    drawFunc();

    ImGui::Dummy(padding);
    ImGui::EndGroup();

    ImVec2 p1 = ImGui::GetItemRectMax();
    p1.x += padding.x;
    ImVec4 borderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
    ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(borderCol.x * 255, borderCol.y * 255, borderCol.z * 255, borderCol.w * 255));

    ImGui::EndGroup();
}

void DrawInfoTab() {
    // TODO This is the bare minimum to get the player name showing
    // There will need to be more effort to get it robust and editable
    std::string name;
    for (int i = 0; i < 8; i++) {
        char letter = gSaveContext.playerName[i] + 0x3D;
        if (letter == '{') {
            letter = '\0';
        }
        name += letter;
    }
    name += '\0';

    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

    ImGui::Text("Name: %s", name.c_str());
    InsertHelpHoverText("Player Name");

    // Use an intermediary to keep the health from updating (and potentially killing the player)
    // until it is done being edited
    int16_t healthIntermediary = gSaveContext.healthCapacity;
    ImGui::InputScalar("Max Health", ImGuiDataType_S16, &healthIntermediary);
    if (ImGui::IsItemDeactivated()) {
        gSaveContext.healthCapacity = healthIntermediary;
    }
    InsertHelpHoverText("Maximum health. 16 units per full heart");
    if (gSaveContext.health > gSaveContext.healthCapacity) {
        gSaveContext.health = gSaveContext.healthCapacity; // Clamp health to new max
    }

    const uint16_t healthMin = 0;
    const uint16_t healthMax = gSaveContext.healthCapacity;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    ImGui::SliderScalar("Health", ImGuiDataType_S16, &gSaveContext.health, &healthMin, &healthMax);
    InsertHelpHoverText("Current health. 16 units per full heart");

    bool doubleDefense = gSaveContext.doubleDefense != 0;
    if (ImGui::Checkbox("Double Defense", &doubleDefense)) {
        gSaveContext.doubleDefense = doubleDefense;
        gSaveContext.inventory.defenseHearts =
            gSaveContext.doubleDefense ? 20 : 0; // Set to get the border drawn in the UI
    }
    InsertHelpHoverText("Is double defense unlocked?");

    std::string magicName;
    if (gSaveContext.magicLevel == 2) {
        magicName = "Double";
    } else if (gSaveContext.magicLevel == 1) {
        magicName = "Single";
    } else {
        magicName = "None";
    }
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    if (ImGui::BeginCombo("Magic Level", magicName.c_str())) {
        if (ImGui::Selectable("Double")) {
            gSaveContext.magicLevel = 2;
            gSaveContext.magicAcquired = true;
            gSaveContext.doubleMagic = true;
        }
        if (ImGui::Selectable("Single")) {
            gSaveContext.magicLevel = 1;
            gSaveContext.magicAcquired = true;
            gSaveContext.doubleMagic = false;
        }
        if (ImGui::Selectable("None")) {
            gSaveContext.magicLevel = 0;
            gSaveContext.magicAcquired = false;
            gSaveContext.doubleMagic = false;
        }

        ImGui::EndCombo();
    }
    InsertHelpHoverText("Current magic level");
    gSaveContext.unk_13F4 = gSaveContext.magicLevel * 0x30; // Set to get the bar drawn in the UI
    if (gSaveContext.magic > gSaveContext.unk_13F4) {
        gSaveContext.magic = gSaveContext.unk_13F4; // Clamp magic to new max
    }

    const uint8_t magicMin = 0;
    const uint8_t magicMax = gSaveContext.unk_13F4;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    ImGui::SliderScalar("Magic", ImGuiDataType_S8, &gSaveContext.magic, &magicMin, &magicMax);
    InsertHelpHoverText("Current magic. 48 units per magic level");

    ImGui::InputScalar("Rupees", ImGuiDataType_S16, &gSaveContext.rupees);
    InsertHelpHoverText("Current rupees");

    const uint16_t dayTimeMin = 0;
    const uint16_t dayTimeMax = 0xFFFF;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    ImGui::SliderScalar("Time", ImGuiDataType_U16, &gSaveContext.dayTime, &dayTimeMin, &dayTimeMax);
    InsertHelpHoverText("Time of day");
    if (ImGui::Button("Dawn")) {
        gSaveContext.dayTime = 0x4000;
    }
    ImGui::SameLine();
    if (ImGui::Button("Noon")) {
        gSaveContext.dayTime = 0x8000;
    }
    ImGui::SameLine();
    if (ImGui::Button("Sunset")) {
        gSaveContext.dayTime = 0xC000;
    }
    ImGui::SameLine();
    if (ImGui::Button("Midnight")) {
        gSaveContext.dayTime = 0;
    }

    ImGui::InputScalar("Total Days", ImGuiDataType_S32, &gSaveContext.totalDays);
    InsertHelpHoverText("Total number of days elapsed since the start of the game");

    ImGui::InputScalar("Deaths", ImGuiDataType_U16, &gSaveContext.deaths);
    InsertHelpHoverText("Total number of deaths");

    bool bgsFlag = gSaveContext.bgsFlag != 0;
    if (ImGui::Checkbox("Has BGS", &bgsFlag)) {
        gSaveContext.bgsFlag = bgsFlag;
    }
    InsertHelpHoverText("Is Biggoron sword unlocked? Replaces Giant's knife");

    ImGui::InputScalar("Sword Health", ImGuiDataType_U16, &gSaveContext.swordHealth);
    InsertHelpHoverText("Giant's knife health. Default is 8. Must be >0 for Biggoron sword to work");

    ImGui::InputScalar("Bgs Day Count", ImGuiDataType_S32, &gSaveContext.bgsDayCount);
    InsertHelpHoverText("Total number of days elapsed since giving Biggoron the claim check");

    // TODO Changing Link's age is more involved than just setting gSaveContext.linkAge
    // It might not fit here and instead should be only changable when changing scenes
    /*
    if (ImGui::BeginCombo("Link Age", LINK_IS_ADULT ? "Adult" : "Child")) {
        if (ImGui::Selectable("Adult")) {
            gSaveContext.linkAge = 0;
        }
        if (ImGui::Selectable("Child")) {
            gSaveContext.linkAge = 1;
        }

        ImGui::EndCombo();
    }
    */

    ImGui::PopItemWidth();
}

void DrawInventoryTab() {
    static bool restrictToValid = true;

    ImGui::Checkbox("Restrict to valid items", &restrictToValid);
    InsertHelpHoverText("Restricts items and ammo to only what is possible to legally acquire in-game");

    for (int32_t y = 0; y < 4; y++) {
        for (int32_t x = 0; x < 6; x++) {
            int32_t index = x + y * 6;
            static int32_t selectedIndex = -1;
            static const char* itemPopupPicker = "itemPopupPicker";

            ImGui::PushID(index);

            if (x != 0) {
                ImGui::SameLine();
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            uint8_t item = gSaveContext.inventory.items[index];
            if (item != ITEM_NONE) {
                const ItemMapEntry& slotEntry = itemMapping.find(item)->second;
                if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
                                       ImVec2(1, 1), 0)) {
                    selectedIndex = index;
                    ImGui::OpenPopup(itemPopupPicker);
                }
            } else {
                if (ImGui::Button("##itemNone", ImVec2(32.0f, 32.0f))) {
                    selectedIndex = index;
                    ImGui::OpenPopup(itemPopupPicker);
                }
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            if (ImGui::BeginPopup(itemPopupPicker)) {
                if (ImGui::Button("##itemNonePicker", ImVec2(32.0f, 32.0f))) {
                    gSaveContext.inventory.items[selectedIndex] = ITEM_NONE;
                    ImGui::CloseCurrentPopup();
                }
                SetLastItemHoverText("None");

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
                    if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f),
                                           ImVec2(0, 0), ImVec2(1, 1), 0)) {
                        gSaveContext.inventory.items[selectedIndex] = slotEntry.id;
                        ImGui::CloseCurrentPopup();
                    }
                    SetLastItemHoverText(SohUtils::GetItemName(slotEntry.id));
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
            ImGui::PushItemWidth(32.0f);
            ImGui::BeginGroup();

            ImGui::Image(SohImGui::GetTextureByName(itemMapping[item].name), ImVec2(32.0f, 32.0f));
            ImGui::InputScalar("##ammoInput", ImGuiDataType_S8, &AMMO(item));

            ImGui::EndGroup();
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
    }
}

// Draw a flag bitfield as an grid of checkboxes
void DrawFlagArray(const std::string& name, uint32_t& flags) {
    ImGui::PushID(name.c_str());
    for (int32_t flagIndex = 0; flagIndex < 32; flagIndex++) {
        if ((flagIndex % 8) != 0) {
            ImGui::SameLine();
        }
        ImGui::PushID(flagIndex);
        uint32_t bitMask = 1 << flagIndex;
        bool flag = (flags & bitMask) != 0;
        if (ImGui::Checkbox("##check", &flag)) {
            if (flag) {
                flags |= bitMask;
            } else {
                flags &= ~bitMask;
            }
        }
        ImGui::PopID();
    }
    ImGui::PopID();
}

void DrawFlagsTab() {
    if (ImGui::TreeNode("Current Scene")) {
        if (gGlobalCtx != nullptr) {
            ActorContext* act = &gGlobalCtx->actorCtx;

            DrawGroupWithBorder([&]() {
                ImGui::Text("Switch");
                InsertHelpHoverText("Permanently-saved switch flags");
                DrawFlagArray("Switch", act->flags.swch);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Switch");
                InsertHelpHoverText("Temporary switch flags. Unset on scene transitions");
                DrawFlagArray("Temp Switch", act->flags.tempSwch);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Clear");
                InsertHelpHoverText("Permanently-saved room-clear flags");
                DrawFlagArray("Clear", act->flags.clear);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Clear");
                InsertHelpHoverText("Temporary room-clear flags. Unset on scene transitions");
                DrawFlagArray("Temp Clear", act->flags.tempClear);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Collect");
                InsertHelpHoverText("Permanently-saved collect flags");
                DrawFlagArray("Collect", act->flags.collect);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Collect");
                InsertHelpHoverText("Temporary collect flags. Unset on scene transitions");
                DrawFlagArray("Temp Collect", act->flags.tempCollect);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Chest");
                InsertHelpHoverText("Permanently-saved chest flags");
                DrawFlagArray("Chest", act->flags.chest);
            });

            ImGui::SameLine();

            ImGui::BeginGroup();

            if (ImGui::Button("Reload Flags")) {
                act->flags.swch = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].swch;
                act->flags.clear = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].clear;
                act->flags.collect = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].collect;
                act->flags.chest = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].chest;
            }
            SetLastItemHoverText("Load flags from saved scene flags. Normally happens on scene load");

            if (ImGui::Button("Save Flags")) {
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].swch = act->flags.swch;
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].clear = act->flags.clear;
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].collect = act->flags.collect;
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].chest = act->flags.chest;
            }
            SetLastItemHoverText("Save current scene flags. Normally happens on scene exit");

            ImGui::EndGroup();
        } else {
            ImGui::Text("Current game state does not have an active scene");
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Saved Scene Flags")) {
        static uint32_t selectedSceneFlagMap = 0;
        ImGui::Text("Map");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Map", SohUtils::GetSceneName(selectedSceneFlagMap).c_str())) {
            for (int32_t sceneIndex = 0; sceneIndex < SCENE_ID_MAX; sceneIndex++) {
                if (ImGui::Selectable(SohUtils::GetSceneName(sceneIndex).c_str())) {
                    selectedSceneFlagMap = sceneIndex;
                }
            }

            ImGui::EndCombo();
        }

        // Don't show current scene button if there is no current scene
        if (gGlobalCtx != nullptr) {
            ImGui::SameLine();
            if (ImGui::Button("Current")) {
                selectedSceneFlagMap = gGlobalCtx->sceneNum;
            }
            SetLastItemHoverText("Open flags for current scene");
        }

        DrawGroupWithBorder([&]() {
            ImGui::Text("Switch");
            InsertHelpHoverText("Switch flags");
            DrawFlagArray("Switch", gSaveContext.sceneFlags[selectedSceneFlagMap].swch);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Clear");
            InsertHelpHoverText("Room-clear flags");
            DrawFlagArray("Clear", gSaveContext.sceneFlags[selectedSceneFlagMap].clear);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Collect");
            InsertHelpHoverText("Collect flags");
            DrawFlagArray("Collect", gSaveContext.sceneFlags[selectedSceneFlagMap].collect);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Chest");
            InsertHelpHoverText("Chest flags");
            DrawFlagArray("Chest", gSaveContext.sceneFlags[selectedSceneFlagMap].chest);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Rooms");
            InsertHelpHoverText("Flags for visted rooms");
            DrawFlagArray("Rooms", gSaveContext.sceneFlags[selectedSceneFlagMap].rooms);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Floors");
            InsertHelpHoverText("Flags for visted floors");
            DrawFlagArray("Floors", gSaveContext.sceneFlags[selectedSceneFlagMap].floors);
        });

        ImGui::TreePop();
    }

    DrawGroupWithBorder([&]() {
        static uint32_t selectedGsMap = 0;
        ImGui::Text("Gold Skulltulas");
        ImGui::Text("Map");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Gold Skulltula Map", gsMapping[selectedGsMap].c_str())) {
            for (int32_t gsIndex = 0; gsIndex < gsMapping.size(); gsIndex++) {
                if (ImGui::Selectable(gsMapping[gsIndex].c_str())) {
                    selectedGsMap = gsIndex;
                }
            }

            ImGui::EndCombo();
        }

        // TODO We should write out descriptions for each one... ugh
        ImGui::Text("Flags");
        uint32_t currentFlags = GET_GS_FLAGS(selectedGsMap);
        uint32_t allFlags = gAreaGsFlags[selectedGsMap];
        uint32_t setMask = 1;
        // Iterate over bitfield and create a checkbox for each skulltula
        while (allFlags != 0) {
            bool isThisSet = (currentFlags & 0x1) == 0x1;

            ImGui::SameLine();
            ImGui::PushID(allFlags);
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

            ImGui::PopID();

            allFlags >>= 1;
            currentFlags >>= 1;
            setMask <<= 1;
        }

        static bool keepGsCountUpdated = true;
        ImGui::Checkbox("Keep GS Count Updated", &keepGsCountUpdated);
        InsertHelpHoverText("Automatically adjust the number of gold skulltula tokens acquired based on set flags");
        int32_t gsCount = 0;
        if (keepGsCountUpdated) {
            for (int32_t gsFlagIndex = 0; gsFlagIndex < 6; gsFlagIndex++) {
                gsCount += std::popcount(static_cast<uint32_t>(gSaveContext.gsFlags[gsFlagIndex]));
            }
            gSaveContext.inventory.gsTokens = gsCount;
        }
    });
}

// Draws a combo that lets you choose and upgrade value from a drop-down of text values
void DrawUpgrade(const std::string& categoryName, int32_t categoryId, const std::vector<std::string>& names) {
    ImGui::Text(categoryName.c_str());
    ImGui::SameLine();
    ImGui::PushID(categoryName.c_str());
    if (ImGui::BeginCombo("##upgrade", names[CUR_UPG_VALUE(categoryId)].c_str())) {
        for (int32_t i = 0; i < names.size(); i++) {
            if (ImGui::Selectable(names[i].c_str())) {
                Inventory_ChangeUpgrade(categoryId, i);
            }
        }

        ImGui::EndCombo();
    }
    ImGui::PopID();
    SetLastItemHoverText(categoryName.c_str());
}

// Draws a combo that lets you choose and upgrade value from a popup grid of icons
void DrawUpgradeIcon(const std::string& categoryName, int32_t categoryId, const std::vector<uint8_t>& items) {
    static const char* upgradePopupPicker = "upgradePopupPicker";

    ImGui::PushID(categoryName.c_str());

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    uint8_t item = items[CUR_UPG_VALUE(categoryId)];
    if (item != ITEM_NONE) {
        const ItemMapEntry& slotEntry = itemMapping[item];
        if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
                               ImVec2(1, 1), 0)) {
            ImGui::OpenPopup(upgradePopupPicker);
        }
    } else {
        if (ImGui::Button("##itemNone", ImVec2(32.0f, 32.0f))) {
            ImGui::OpenPopup(upgradePopupPicker);
        }
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    SetLastItemHoverText(categoryName.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    if (ImGui::BeginPopup(upgradePopupPicker)) {
        for (int32_t pickerIndex = 0; pickerIndex < items.size(); pickerIndex++) {
            if ((pickerIndex % 8) != 0) {
                ImGui::SameLine();
            }

            if (items[pickerIndex] == ITEM_NONE) {
                if (ImGui::Button("##upgradePopupPicker", ImVec2(32.0f, 32.0f))) {
                    Inventory_ChangeUpgrade(categoryId, pickerIndex);
                    ImGui::CloseCurrentPopup();
                }
                SetLastItemHoverText("None");
            } else {
                const ItemMapEntry& slotEntry = itemMapping[items[pickerIndex]];
                if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
                                       ImVec2(1, 1), 0)) {
                    Inventory_ChangeUpgrade(categoryId, pickerIndex);
                    ImGui::CloseCurrentPopup();
                }
                SetLastItemHoverText(SohUtils::GetItemName(slotEntry.id));
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

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
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        if (ImGui::ImageButton(SohImGui::GetTextureByName(hasEquip ? entry.name : entry.nameFaded),
                               ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
            if (hasEquip) {
                gSaveContext.inventory.equipment &= ~bitMask;
            } else {
                gSaveContext.inventory.equipment |= bitMask;
            }
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
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
    const std::vector<std::string> walletNames = {
        "Child (99)",
        "Adult (200)",
        "Giant (500)",
    };
    DrawUpgrade("Wallet", UPG_WALLET, walletNames);

    const std::vector<std::string> stickNames = {
        "None",
        "10",
        "20",
        "30",
    };
    DrawUpgrade("Sticks", UPG_STICKS, stickNames);

    const std::vector<std::string> nutNames = {
        "None",
        "20",
        "30",
        "40",
    };
    DrawUpgrade("Deku Nuts", UPG_NUTS, nutNames);
}

// Draws a toggleable icon for a quest item that is faded when disabled
void DrawQuestItemButton(uint32_t item) {
    const QuestMapEntry& entry = questMapping[item];
    uint32_t bitMask = 1 << entry.id;
    bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (ImGui::ImageButton(SohImGui::GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                           ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (hasQuestItem) {
            gSaveContext.inventory.questItems &= ~bitMask;
        } else {
            gSaveContext.inventory.questItems |= bitMask;
        }
    }
    ImGui::PopStyleColor();
    SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
}

// Draws a toggleable icon for a dungeon item that is faded when disabled
void DrawDungeonItemButton(uint32_t item, uint32_t scene) {
    const ItemMapEntry& entry = itemMapping[item];
    uint32_t bitMask = 1 << (entry.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[scene]) != 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (ImGui::ImageButton(SohImGui::GetTextureByName(hasItem ? entry.name : entry.nameFaded),
                           ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (hasItem) {
            gSaveContext.inventory.dungeonItems[scene] &= ~bitMask;
        } else {
            gSaveContext.inventory.dungeonItems[scene] |= bitMask;
        }
    }
    ImGui::PopStyleColor();
    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
}

void DrawQuestStatusTab() {
    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

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
    ImGui::Dummy(ImVec2(20, 0));

    ImGui::SameLine();
    DrawQuestItemButton(QUEST_STONE_OF_AGONY);

    ImGui::SameLine();
    DrawQuestItemButton(QUEST_GERUDO_CARD);

    for (const SongMapEntry& entry : songMapping) {
        if ((entry.id != QUEST_SONG_MINUET) && (entry.id != QUEST_SONG_LULLABY)) {
            ImGui::SameLine();
        }

        uint32_t bitMask = 1 << entry.id;
        bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        if (ImGui::ImageButton(SohImGui::GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                               ImVec2(16.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
            if (hasQuestItem) {
                gSaveContext.inventory.questItems &= ~bitMask;
            } else {
                gSaveContext.inventory.questItems |= bitMask;
            }
        }
        ImGui::PopStyleColor();
        SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
    }

    ImGui::InputScalar("GS Count", ImGuiDataType_S16, &gSaveContext.inventory.gsTokens);
    InsertHelpHoverText("Number of gold skulltula tokens aquired");

    uint32_t bitMask = 1 << QUEST_SKULL_TOKEN;
    bool gsUnlocked = (bitMask & gSaveContext.inventory.questItems) != 0;
    if (ImGui::Checkbox("GS unlocked", &gsUnlocked)) {
        if (gsUnlocked) {
            gSaveContext.inventory.questItems |= bitMask;
        } else {
            gSaveContext.inventory.questItems &= ~bitMask;
        }
    }
    InsertHelpHoverText("If unlocked, enables showing the gold skulltula count in the quest status menu");

    int32_t pohCount = (gSaveContext.inventory.questItems & 0xF0000000) >> 28;
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

    DrawGroupWithBorder([&]() {
        ImGui::Text("Dungeon Items");

        static int32_t dungeonItemsScene = SCENE_YDAN;
        ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
        if (ImGui::BeginCombo("##DungeonSelect", SohUtils::GetSceneName(dungeonItemsScene).c_str())) {
            for (int32_t dungeonIndex = SCENE_YDAN; dungeonIndex < SCENE_BDAN_BOSS + 1; dungeonIndex++) {
                if (ImGui::Selectable(SohUtils::GetSceneName(dungeonIndex).c_str(),
                                      dungeonIndex == dungeonItemsScene)) {
                    dungeonItemsScene = dungeonIndex;
                }
            }

            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        DrawDungeonItemButton(ITEM_KEY_BOSS, dungeonItemsScene);
        ImGui::SameLine();
        DrawDungeonItemButton(ITEM_COMPASS, dungeonItemsScene);
        ImGui::SameLine();
        DrawDungeonItemButton(ITEM_DUNGEON_MAP, dungeonItemsScene);

        if (dungeonItemsScene != SCENE_BDAN_BOSS) {
            float lineHeight = ImGui::GetTextLineHeightWithSpacing();
            ImGui::Image(SohImGui::GetTextureByName(itemMapping[ITEM_KEY_SMALL].name), ImVec2(lineHeight, lineHeight));
            ImGui::SameLine();
            ImGui::InputScalar("##Keys", ImGuiDataType_S8, gSaveContext.inventory.dungeonKeys + dungeonItemsScene);
        } else {
            // dungeonItems is size 20 but dungeonKeys is size 19, so there are no keys for the last scene (Barinade's Lair)
            ImGui::Text("Barinade's Lair does not have small keys");
        }
    });

    ImGui::PopItemWidth();
}

void DrawSaveEditor(bool& open) {
    if (!open) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Save Editor", &open)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("SaveContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Info")) {
            DrawInfoTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Inventory")) {
            DrawInventoryTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Flags")) {
            DrawFlagsTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Equipment")) {
            DrawEquipmentTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Quest Status")) {
            DrawQuestStatusTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void InitSaveEditor() {
    SohImGui::AddWindow("Debug", "Save Editor", DrawSaveEditor);

    // Load item icons into ImGui
    for (const auto& entry : itemMapping) {
        SohImGui::LoadResource(entry.second.name, entry.second.texturePath);
        SohImGui::LoadResource(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }
    for (const auto& entry : questMapping) {
        SohImGui::LoadResource(entry.second.name, entry.second.texturePath);
        SohImGui::LoadResource(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }
    for (const auto& entry : songMapping) {
        SohImGui::LoadResource(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        SohImGui::LoadResource(entry.nameFaded, gSongNoteTex, fadedCol);
    }
}

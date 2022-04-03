#include "debugSaveEditor.h"
#include "../libultraship/SohImGuiImpl.h"

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
}

typedef struct {
    uint32_t id;
    std::string name;
    std::string texturePath;
} ItemMapEntry;

#define ITEM_MAP_ENTRY(id)                              \
    {                                                   \
        id, {                                           \
            id, #id, static_cast<char*>(gItemIcons[id]) \
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

void DrawSaveEditor(bool& open) {
    if (!open) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Save Editor", &open)) {
        ImGui::End();
        return;
    }

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

    if (ImGui::BeginTabBar("SaveContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Info")) {
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

            // TODO Move to quest status screen once the page is created
            ImGui::InputScalar("GS Count", ImGuiDataType_S16, &gSaveContext.inventory.gsTokens);
            InsertHelpHoverText("Number of gold skulltula tokens aquired");

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
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Inventory")) {
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
                        if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f),
                                               ImVec2(0, 0), ImVec2(1, 1), 0)) {
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
                        SetLastItemHoverText("ITEM_NONE");

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
                            SetLastItemHoverText(slotEntry.name);
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

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Flags")) {
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

            // TODO other flag types, like switch, clear, etc.
            // These flags interact with the actor context, so it's a bit more complicated

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
    }
}

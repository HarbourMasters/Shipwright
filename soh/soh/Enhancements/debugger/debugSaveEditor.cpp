#include "debugSaveEditor.h"
#include "../../util.h"
#include "../libultraship/SohImGuiImpl.h"
#include "ImGuiHelpers.h"

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

char z2ASCII(int code) {
    int ret;
    if (code < 10) { //Digits
        ret = code + 0x30;
    } else if (code >= 10 && code < 36) { //Uppercase letters
        ret = code + 0x37;
    } else if (code >= 36 && code < 62) { //Lowercase letters
        ret = code + 0x3D;
    } else if (code == 62) { //Space
        ret = code - 0x1E;
    } else if (code == 63 || code == 64) { // _ and .
        ret = code - 0x12;
    } else {
        ret = code;
    }
    return char(ret);

}

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
    InsertHelpHoverText("Player Name");
    std::string nameID;
    for (int i = 0; i < 8; i++) {
        nameID = z2ASCII(i);
        if (i % 4 != 0) {
            ImGui::SameLine();
        }
        ImGui::InputScalar(nameID.c_str(), ImGuiDataType_U8, &gSaveContext.playerName[i], &one, NULL);
    }

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

    ImGui::InputScalar("Entrance Index", ImGuiDataType_S32, &gSaveContext.entranceIndex);
    InsertHelpHoverText("From which entrance did Link arrive?");

    ImGui::InputScalar("Cutscene Index", ImGuiDataType_S32, &gSaveContext.cutsceneIndex);
    InsertHelpHoverText("Which cutscene is this?");

    ImGui::InputScalar("Navi Timer", ImGuiDataType_U16, &gSaveContext.naviTimer);
    InsertHelpHoverText("Navi wants to talk at 600 units, decides not to at 3000.");

    ImGui::InputScalar("Timer 1 State", ImGuiDataType_S16, &gSaveContext.timer1State);
    InsertHelpHoverText("Heat timer, race timer, etc. Has white font");

    ImGui::InputScalar("Timer 1 Value", ImGuiDataType_S16, &gSaveContext.timer1Value, &one, NULL);
    InsertHelpHoverText("Time, in seconds");

    ImGui::InputScalar("Timer 2 State", ImGuiDataType_S16, &gSaveContext.timer2State);
    InsertHelpHoverText("Trade timer, Ganon collapse timer, etc. Has yellow font");

    ImGui::InputScalar("Timer 2 Value", ImGuiDataType_S16, &gSaveContext.timer2Value, &one, NULL);
    InsertHelpHoverText("Time, in seconds");
     
    const char* audioName;
    switch (gSaveContext.audioSetting) { 
        case 0:
            audioName = "Stereo";
            break;
        case 1:
            audioName = "Mono";
            break;
        case 2:
            audioName = "Headset";
            break;
        case 3:
            audioName = "Surround";
            break;
        default:
            audioName = "?";
    }
    if (ImGui::BeginCombo("Audio", audioName)) {
        if (ImGui::Selectable("Stereo")) {
            gSaveContext.audioSetting = 0;
        }
        if (ImGui::Selectable("Mono")) {
            gSaveContext.audioSetting = 1;
        }
        if (ImGui::Selectable("Headset")) {
            gSaveContext.audioSetting = 2;
        }
        if (ImGui::Selectable("Surround")) {
            gSaveContext.audioSetting = 3;
        }

        ImGui::EndCombo();
    }
    InsertHelpHoverText("Sound setting");
    
    bool n64DDFlag = gSaveContext.n64ddFlag != 0;
    if (ImGui::Checkbox("64 DD file?", &n64DDFlag)) {
        gSaveContext.n64ddFlag = n64DDFlag;
    }
    InsertHelpHoverText("WARNING! If you save, your file may be locked! Use caution!");
    
    if (ImGui::BeginCombo("Z Target Mode", gSaveContext.zTargetSetting ? "Hold" : "Switch")) {
        if (ImGui::Selectable("Switch")) {
            gSaveContext.zTargetSetting = 0;
        }
        if (ImGui::Selectable("Hold")) {
            gSaveContext.zTargetSetting = 1;
        }
        ImGui::EndCombo();
    }
    InsertHelpHoverText("Z-Targeting behavior");


    ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
    static std::array<const char*, 7> minigameHS = { "Horseback Archery", 
        "Big Poe Points",                                            
        "Fishing",
        "Malon's Obstacle Course",                                    
        "Running Man Race",
        "?",
        "Dampe's Race" };
    
    if (ImGui::TreeNode("Minigames")) {
        for (int i = 0; i < 7; i++) {
            if (i == 5) { //HS_UNK_05 is unused
                continue;
            }
            std::string minigameLbl = minigameHS[i];
            ImGui::InputScalar(minigameLbl.c_str(), ImGuiDataType_S32, &gSaveContext.highScores[i], &one, NULL);
        }
        
        ImGui::TreePop();
    }
    
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
void DrawFlagArray32(const std::string& name, uint32_t& flags) {
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

void DrawFlagArray16(const std::string& name, uint16_t& flags) {
    ImGui::PushID(name.c_str());
    for (int32_t flagIndex = 15; flagIndex >= 0; flagIndex--) {
        ImGui::SameLine();
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
                DrawFlagArray32("Switch", act->flags.swch);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Switch");
                InsertHelpHoverText("Temporary switch flags. Unset on scene transitions");
                DrawFlagArray32("Temp Switch", act->flags.tempSwch);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Clear");
                InsertHelpHoverText("Permanently-saved room-clear flags");
                DrawFlagArray32("Clear", act->flags.clear);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Clear");
                InsertHelpHoverText("Temporary room-clear flags. Unset on scene transitions");
                DrawFlagArray32("Temp Clear", act->flags.tempClear);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Collect");
                InsertHelpHoverText("Permanently-saved collect flags");
                DrawFlagArray32("Collect", act->flags.collect);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Collect");
                InsertHelpHoverText("Temporary collect flags. Unset on scene transitions");
                DrawFlagArray32("Temp Collect", act->flags.tempCollect);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Chest");
                InsertHelpHoverText("Permanently-saved chest flags");
                DrawFlagArray32("Chest", act->flags.chest);
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
            DrawFlagArray32("Switch", gSaveContext.sceneFlags[selectedSceneFlagMap].swch);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Clear");
            InsertHelpHoverText("Room-clear flags");
            DrawFlagArray32("Clear", gSaveContext.sceneFlags[selectedSceneFlagMap].clear);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Collect");
            InsertHelpHoverText("Collect flags");
            DrawFlagArray32("Collect", gSaveContext.sceneFlags[selectedSceneFlagMap].collect);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Chest");
            InsertHelpHoverText("Chest flags");
            DrawFlagArray32("Chest", gSaveContext.sceneFlags[selectedSceneFlagMap].chest);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Rooms");
            InsertHelpHoverText("Flags for visted rooms");
            DrawFlagArray32("Rooms", gSaveContext.sceneFlags[selectedSceneFlagMap].rooms);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Floors");
            InsertHelpHoverText("Flags for visted floors");
            DrawFlagArray32("Floors", gSaveContext.sceneFlags[selectedSceneFlagMap].floors);
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

    if (ImGui::TreeNode("Event Check Inf Flags")) {
        DrawGroupWithBorder([&]() {
            ImGui::Text("0");
            InsertHelpHoverText("Mostly Kokiri Forest related");
            DrawFlagArray16("eci0", gSaveContext.eventChkInf[0]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("1");
            InsertHelpHoverText("Mostly Lon Lon Ranch related");
            DrawFlagArray16("eci1", gSaveContext.eventChkInf[1]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("2");
            InsertHelpHoverText("Dodongo Related?");
            DrawFlagArray16("eci2", gSaveContext.eventChkInf[2]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("3");
            InsertHelpHoverText("Mostly Zora related");
            DrawFlagArray16("eci3", gSaveContext.eventChkInf[3]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("4");
            InsertHelpHoverText("Random");
            DrawFlagArray16("eci4", gSaveContext.eventChkInf[4]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("5");
            InsertHelpHoverText("Mostly song learning related");
            DrawFlagArray16("eci5", gSaveContext.eventChkInf[5]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("6");
            InsertHelpHoverText("Random");
            DrawFlagArray16("eci6", gSaveContext.eventChkInf[6]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("7");
            InsertHelpHoverText("Boss Battle related");
            DrawFlagArray16("eci7", gSaveContext.eventChkInf[7]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("8");
            InsertHelpHoverText("Mask related?");
            DrawFlagArray16("eci8", gSaveContext.eventChkInf[8]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("9");
            InsertHelpHoverText("Mostly carpenter related");
            DrawFlagArray16("eci9", gSaveContext.eventChkInf[9]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("A");
            InsertHelpHoverText("First-time overworld entrance cs related");
            DrawFlagArray16("eci1", gSaveContext.eventChkInf[10]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("B");
            InsertHelpHoverText("First-time dungeon entrance cs/trial cs related");
            DrawFlagArray16("eci11", gSaveContext.eventChkInf[11]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("C");
            InsertHelpHoverText("Random");
            DrawFlagArray16("eci12", gSaveContext.eventChkInf[12]);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("D");
            InsertHelpHoverText("Frog songs/GS rewards");
            DrawFlagArray16("eci13", gSaveContext.eventChkInf[13]);
        });

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Inf Table Flags")) {
        for (int i = 0; i < 30; i++) {
            std::string it_id = "it" + std::to_string(i);
            DrawGroupWithBorder([&]() {
                ImGui::Text("%2d", i);
                DrawFlagArray16(it_id, gSaveContext.infTable[i]);
            });
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Item Get Inf Flags")) {
        for (int i = 0; i < 4; i++) {
            std::string igi_id = "igi" + std::to_string(i);
            DrawGroupWithBorder([&]() {
                ImGui::Text("%d", i);
                DrawFlagArray16(igi_id, gSaveContext.itemGetInf[i]);
            });
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Event Inf Flags")) {
        for (int i = 0; i < 4; i++) {
            std::string ei_id = "ei" + std::to_string(i);
            DrawGroupWithBorder([&]() {
                ImGui::Text("%d", i);
                DrawFlagArray16(ei_id, gSaveContext.eventInf[i]);
            });
        }
        ImGui::TreePop();
    }
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

void DrawPlayerTab() {
    if (gGlobalCtx != nullptr) {
        Player* player = GET_PLAYER(gGlobalCtx);
        const char* curSword;
        const char* curShield;
        const char* curTunic;
        const char* curBoots;

        switch (player->currentSwordItem) {
            case ITEM_SWORD_KOKIRI:
                curSword = "Kokiri Sword"; 
                break;
            case ITEM_SWORD_MASTER:
                curSword = "Master Sword";
                break;
            case ITEM_SWORD_BGS:
                curSword = "Biggoron's Sword";
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
        DrawGroupWithBorder([&]() {
            ImGui::Text("Link's Position");
            ImGui::InputScalar("X Pos", ImGuiDataType_Float, &player->actor.world.pos.x);
            ImGui::SameLine();
            ImGui::InputScalar("Y Pos", ImGuiDataType_Float, &player->actor.world.pos.y);
            ImGui::SameLine();
            ImGui::InputScalar("Z Pos", ImGuiDataType_Float, &player->actor.world.pos.z);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Link's Rotation");
            InsertHelpHoverText("For Link's rotation in relation to the world");
            ImGui::InputScalar("X Rot", ImGuiDataType_S16, &player->actor.world.rot.x);
            ImGui::SameLine();
            ImGui::InputScalar("Y Rot", ImGuiDataType_S16, &player->actor.world.rot.y);
            ImGui::SameLine();
            ImGui::InputScalar("Z Rot", ImGuiDataType_S16, &player->actor.world.rot.z);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Link's Model Rotation");
            InsertHelpHoverText("For Link's actual model");
            ImGui::InputScalar("X ModRot", ImGuiDataType_S16, &player->actor.shape.rot.x);
            ImGui::SameLine();
            ImGui::InputScalar("Y ModRot", ImGuiDataType_S16, &player->actor.shape.rot.y);
            ImGui::SameLine();
            ImGui::InputScalar("Z ModRot", ImGuiDataType_S16, &player->actor.shape.rot.z);
        });

        ImGui::InputScalar("Linear Velocity", ImGuiDataType_Float, &player->linearVelocity);
        InsertHelpHoverText("Link's speed along the XZ plane");

        ImGui::InputScalar("Y Velocity", ImGuiDataType_Float, &player->actor.velocity.y);
        InsertHelpHoverText("Link's speed along the Y plane. Caps at -20");

        ImGui::InputScalar("Wall Height", ImGuiDataType_Float, &player->wallHeight);
        InsertHelpHoverText("\"height used to determine whether link can climb or grab a ledge at the top\"");

        ImGui::InputScalar("Invincibility Timer", ImGuiDataType_S8, &player->invincibilityTimer);
        InsertHelpHoverText("Can't take damage while this is nonzero");

        ImGui::InputScalar("Gravity", ImGuiDataType_Float, &player->actor.gravity);
        InsertHelpHoverText("Rate at which Link falls. Default -4.0f");

        if (ImGui::BeginCombo("Link Age on Load", gGlobalCtx->linkAgeOnLoad == 0 ? "Adult" : "Child")) {
            if (ImGui::Selectable("Adult")) {
                gGlobalCtx->linkAgeOnLoad = 0;
            }
            if (ImGui::Selectable("Child")) {
                gGlobalCtx->linkAgeOnLoad = 1;
            }
            ImGui::EndCombo();
        }

        InsertHelpHoverText("This will change Link's age when you load a map");

        ImGui::Separator();
        
        ImGui::Text("Link's Current Equipment");
        ImGui::PushItemWidth(ImGui::GetFontSize() * 15);
        if (ImGui::BeginCombo("Sword", curSword)) {
            if (ImGui::Selectable("None")) {
                player->currentSwordItem = ITEM_NONE;
                gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_NONE);
            }
            if (ImGui::Selectable("Kokiri Sword")) {
                player->currentSwordItem = ITEM_SWORD_KOKIRI;
                gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_KOKIRI);
            }
            if (ImGui::Selectable("Master Sword")) {
                player->currentSwordItem = ITEM_SWORD_MASTER;
                gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_MASTER);
            }
            if (ImGui::Selectable("Biggoron's Sword")) {
                if (gSaveContext.bgsFlag) {
                    if (gSaveContext.swordHealth < 8) {
                        gSaveContext.swordHealth = 8;
                    }
                    player->currentSwordItem = ITEM_SWORD_BGS;
                    gSaveContext.equips.buttonItems[0] = ITEM_SWORD_BGS;
                } else {
                    if (gSaveContext.swordHealth < 8) {
                        gSaveContext.swordHealth = 8;
                    }
                    player->currentSwordItem = ITEM_SWORD_BGS;
                    gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KNIFE;
                }
                
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_BGS);
            }
            ImGui::EndCombo();

        }
        if (ImGui::BeginCombo("Shield", curShield)) {
            if (ImGui::Selectable("None")) {
                player->currentShield = PLAYER_SHIELD_NONE;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_NONE);
            }
            if (ImGui::Selectable("Deku Shield")) {
                player->currentShield = PLAYER_SHIELD_DEKU;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_DEKU);
            }
            if (ImGui::Selectable("Hylian Shield")) {
                player->currentShield = PLAYER_SHIELD_HYLIAN;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_HYLIAN);
            }
            if (ImGui::Selectable("Mirror Shield")) {
                player->currentShield = PLAYER_SHIELD_MIRROR;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_MIRROR);
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Tunic", curTunic)) {
            if (ImGui::Selectable("Kokiri Tunic")) {
                player->currentTunic = PLAYER_TUNIC_KOKIRI;
                Inventory_ChangeEquipment(EQUIP_TUNIC, PLAYER_TUNIC_KOKIRI + 1);
            }
            if (ImGui::Selectable("Goron Tunic")) {
                player->currentTunic = PLAYER_TUNIC_GORON;
                Inventory_ChangeEquipment(EQUIP_TUNIC, PLAYER_TUNIC_GORON + 1);
            }
            if (ImGui::Selectable("Zora Tunic")) {
                player->currentTunic = PLAYER_TUNIC_ZORA;
                Inventory_ChangeEquipment(EQUIP_TUNIC, PLAYER_TUNIC_ZORA + 1);
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Boots", curBoots)) {
            if (ImGui::Selectable("Kokiri Boots")) {
                player->currentBoots = PLAYER_BOOTS_KOKIRI;
                Inventory_ChangeEquipment(EQUIP_BOOTS, PLAYER_BOOTS_KOKIRI + 1);
            }
            if (ImGui::Selectable("Iron Boots")) {
                player->currentBoots = PLAYER_BOOTS_IRON;
                Inventory_ChangeEquipment(EQUIP_BOOTS, PLAYER_BOOTS_IRON + 1);
            }
            if (ImGui::Selectable("Hover Boots")) {
                player->currentBoots = PLAYER_BOOTS_HOVER;
                Inventory_ChangeEquipment(EQUIP_BOOTS, PLAYER_BOOTS_HOVER + 1);
            }
            ImGui::EndCombo();
        }

        ImU16 one = 1;
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        DrawGroupWithBorder([&]() {
            ImGui::Text("Current C Equips");
            ImGui::InputScalar("C Left", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[1], &one, NULL);
            ImGui::SameLine();
            ImGui::InputScalar("C Down", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[2], &one, NULL);
            ImGui::SameLine();
            ImGui::InputScalar("C Right", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[3], &one, NULL);
        });

    } else {
        ImGui::Text("Global Context needed for player info!");
    }
}

void DrawSaveEditor(bool& open) {
    if (!open) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Save Editor", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
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

        if (ImGui::BeginTabItem("Player")) {
            DrawPlayerTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void InitSaveEditor() {
    SohImGui::AddWindow("Developer Tools", "Save Editor", DrawSaveEditor);

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

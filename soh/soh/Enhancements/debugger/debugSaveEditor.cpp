#include "debugSaveEditor.h"
#include "../../util.h"
#include "../libultraship/ImGuiImpl.h"
#include "ImGuiHelpers.h"

#include <array>
#include <bit>
#include <map>
#include <string>
#include <Cvar.h>

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
    ITEM_MAP_ENTRY(ITEM_HEART_CONTAINER),
    ITEM_MAP_ENTRY(ITEM_MAGIC_SMALL),
    ITEM_MAP_ENTRY(ITEM_MAGIC_LARGE)
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
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FOREST, dgForestMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FIRE, dgFireMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_WATER, dgWaterMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SPIRIT, dgSpiritMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SHADOW, dgShadowMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_LIGHT, dgLightMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_KOKIRI_EMERALD, dgKokiriEmeraldIconTex),
    QUEST_MAP_ENTRY(QUEST_GORON_RUBY, dgGoronRubyIconTex),
    QUEST_MAP_ENTRY(QUEST_ZORA_SAPPHIRE, dgZoraSapphireIconTex),
    QUEST_MAP_ENTRY(QUEST_STONE_OF_AGONY, dgStoneOfAgonyIconTex),
    QUEST_MAP_ENTRY(QUEST_GERUDO_CARD, dgGerudosCardIconTex),
    QUEST_MAP_ENTRY(QUEST_SKULL_TOKEN, dgGoldSkulltulaIconTex),
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
    SONG_MAP_ENTRY(QUEST_SONG_LULLABY,  224, 107, 255),
    SONG_MAP_ENTRY(QUEST_SONG_EPONA,    255, 195, 60),
    SONG_MAP_ENTRY(QUEST_SONG_SARIA,    127, 255, 137),
    SONG_MAP_ENTRY(QUEST_SONG_SUN,      255, 255, 60),
    SONG_MAP_ENTRY(QUEST_SONG_TIME,     119, 236, 255),
    SONG_MAP_ENTRY(QUEST_SONG_STORMS,   165, 165, 165),
    SONG_MAP_ENTRY(QUEST_SONG_MINUET,   150, 255, 100),
    SONG_MAP_ENTRY(QUEST_SONG_BOLERO,   255, 80,  40),
    SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    SONG_MAP_ENTRY(QUEST_SONG_REQUIEM,  255, 160, 0),
    SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    SONG_MAP_ENTRY(QUEST_SONG_PRELUDE,  255, 240, 100),
} };

#define VANILLA_SONG_MAP_ENTRY(id, r, g, b)       \
    {                                  \
            id, #id "_Vanilla", #id "_Vanilla_Faded", ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f) \
    }

// Maps song ids to info for use in ImGui
std::array<SongMapEntry, 12> vanillaSongMapping = { {
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_LULLABY,  255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_EPONA,    255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SARIA,    255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SUN,      255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_TIME,     255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_STORMS,   255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_MINUET,   150, 255, 100),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_BOLERO,   255, 80,  40),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_REQUIEM,  255, 160, 0),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_PRELUDE,  255, 240, 100),
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
        gSaveContext.dayTime = 0xC001;
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

 struct warps {
    const char* category;
    const char* locations[20];
    const int32_t indexes[20];

    int32_t getSize() {
        int i = 0;
        while (locations[i] != NULL) {
            i = i + 1;
        }
        return (i);
    }

    int32_t* getIndexSub(char* index) {
        static int32_t returnVals[20];
        int j = 0;
        for (int i = 0; i < getSize(); i++) {
            std::string temp(locations[i]);
            
            if (temp.find(index) != -1) {
                returnVals[j] = indexes[i];
                j++;
            }
        }
        return (returnVals);
    }
    bool getInd(char* index, int32_t i) {
        static int32_t returnVal;
        std::string temp(locations[i]); 
        if (temp.find(index) != -1) {
            return true;
        }
        return false;
    }

    int32_t getIndex(char* index) {
        for (int i = 0; i < getSize(); i++) {
            if (index == locations[i]) {
                return indexes[i];
            }
        }
        return 0;
    }
    char* getLocation(int32_t index) {
        static char* returnVal;
        for (int i = 0; i < getSize(); i++) {
            if (index == indexes[i]) {
                returnVal = (char*)locations[i];
                return returnVal;
            }
        }
        return (char*)"NONE";
    }
 };

struct warps warpMap[] = {
    { "Kokiri Forest",
      { "From Deku Tree", "From Bridge", "From Links House", "From Kokiri Shop", "From Know-It-All Brothers House",
        "From Lost Woods", "From Twins House", "From Midos House", "From Sarias House", "On Links House",
        "Cutscene Entrance", "After Deku Tree Death Scene" },
      { 0x0209, 0x020D, 0x0211, 0x0266, 0x026A, 0x0286, 0x033C, 0x0443, 0x0447, 0x0338, 0x00EE, 0x0457 } },
    { "Kokiri Buildings",
      { "Kokiri Shop", "Twins House", "Know-It-All Brothers House", "Links House", "Links Bed", "Midos House",
        "Sarias House" },
      { 0x00C1, 0x009C, 0x00C9, 0x0272, 0x00BB, 0x0433, 0x0437 } },
    { "Lost Woods",
      { "From Kokiri Forest", "From Sacred Meadow", "From Goron City", "From Zora River",
        "Entrance Opposite Main Entrance", "Near Skull Kid", "Owl Cutscene" },
      { 0x011E, 0x01A9, 0x04D6, 0x04DA, 0x01B1, 0x01AD, 0x04D2 } },
    { "Lost Woods - Bridge", { "From Hyrule Field", "From Kokiri Forest" }, { 0x04DE, 0x05E0 } },
    { "Sacred Forest Meadow",
      { "From Lost Woods", "From Forest Temple", "Minuet of Forest Warp", "SBlue Warp from Phantom Ganon" },
      { 0x00FC, 0x0215, 0x0600, 0x0608 } },
    { "Hyrule Field",
      { "From Kakariko Village", "From Zora River (Land)", "From Zora River (Water)", "From Lost Woods",
        "From Lake Hylia", "From Gerudo Valley", "From Lon Lon Ranch", "From Market Entrance",
        "Owl Drop From Lake Hylia", "Lon Lon Ranch Horse Jump (SOUTH)", "Lon Lon Ranch Horse Jump (WEST)",
        "Lon Lon Ranch Horse Jump (EAST)", "Lon Lon Ranch Horse Jump (FRONT)", "Ocarina of Time Scene",
        "After Learning Zeldas Lullaby", "After Zelda Escapes w/ Impa" },
      { 0x017D, 0x0181, 0x0311, 0x0185, 0x0189, 0x018D, 0x01F9, 0x01FD, 0x027E, 0x028A, 0x028E, 0x0292, 0x0292, 0x050F,
        0x0594, 0x00CD } },
    { "Lon Lon Ranch",
      { "From Hyrule Field", "From Ranch House", "From Stables", "Epona Song Cutscene", "South Gate", "West Gate",
        "Inside Horse Track", "Horse Race Minigame" },
      { 0x0157, 0x0378, 0x042F, 0x02AE, 0x02E2, 0x02E6, 0x04CA, 0x04CE } },
     { "Lon Lon Ranch Buildings",
       { "Ranch House", "Stables", "Back Tower", "Chicken Minigame" },
       { 0x004F, 0x02F9, 0x05D0, 0x05E4 } },
    { "Castle Town",
      { "From Entrance", "From Shooting Gallery", "From Happy Mask Shop", "From Treasure Box", "From Castle",
        "From Temple of Time", "From Back Alley (Right)", "From Back Alley (Left)", "From Potion Shop (Castle Town)", "From Bazaar Shop (Castle Town)",
        "From bomchu Bowling" },
      { 0x00B1, 0x01CD, 0x01D1, 0x01D5, 0x025A, 0x025E, 0x0262, 0x029E, 0x02A2, 0x03B8, 0x03BC } },
    { "Market Entrance", { "From Market", "From Pot House", "From Hyrule Field" }, { 0x0033, 0x026E, 0x0276 } },
    { "Back Alley (Right)", { "From Market", "From Alley House" }, { 0x00AD, 0x0067 } },
    { "Back Alley (Left)", { "From Market", "From Dog House", "From Bombchu Shop" }, { 0x029A, 0x038C, 0x03C0 } },
    { "Castle Town Buildings",
      { "Pot House", "Shooting Gallery Minigame (Castle Town)", "Treasure Box Minigame", "Potion Shop (Castle Town)", "Bombchu Bowling Minigame", "Bazaar Shop (Castle Town)",
        "Happy Mask Shop", "Bombchu Shop", "Dog House", "Alley House" },
      { 0x007E, 0x016D, 0x0063, 0x0388, 0x0507, 0x052C, 0x0530, 0x0528, 0x0398, 0x043B } },
    { "Temple of Time",
      { "From Outside", "From Master Sword Pedestal", "From First Master Sword Pull", "Prelude of Light Warp",
        "In Front of Spiritual Stones", "After Light Arrow Cutscene", "Outside Temple of Time - From Market",
        "Outside Temple of Time - From Temple of Time" },
      { 0x0053, 0x02CA, 0x0324, 0x05F4, 0x0320, 0x058C, 0x0171, 0x0472 } },
    { "Hyrule Castle",
      { "From Market", "From Castle Courtyard", "From Great Fairy", "From Courtyard Guard Capture", "Great Fairy" },
      { 0x0138, 0x023D, 0x0340, 0x04FA, 0x04C2 } },
    { "Courtyard",
      { "From Crawlspace", "From Zelda", "Zeldas Courtyard", "Zeldas Courtyard after Cutscene" },
      { 0x007A, 0x0296, 0x0400, 0x05F0 } },
    { "Kakariko Village",
      { "From Hyrule Field", "From Death Mountain", "From Graveyard", "From Bazaar (Kakariko)", "From Bottom of Well",
        "From Bosss House", "From Potion Shop (Kakariko)", "From Potion Shop (Kakariko - Back Entrance)", "From Grannys Potion Shop",
        "From Impas House", "From Impas House (Cow)", "From Windmill", "From Shooting Gallery", "From Skulltula House",
        "Owl Drop Spot from Death Mountain", "After Sheik Cutscene" },
      { 0x00DB, 0x0191, 0x0195, 0x0201, 0x02A6, 0x0349, 0x044B, 0x04FF, 0x034D, 0x0345, 0x05DC, 0x0351, 0x0463, 0x04EE,
        0x0554, 0x0513 } },
    { "Kakariko Buildings",
      { "Shooting Gallery Minigame (Kakariko)", "Grannys Potion Shop", "Bazaar Shop (Kakariko)", "Potion Shop (Kakariko)", "Potion Shop (Kakariko- Back Entrance)", "Impas House",
        "Impas House (Cow)", "Bosss House", "Windmill", "Skulltula House" },
      { 0x003B, 0x0072, 0x00B7, 0x0384, 0x03EC, 0x039C, 0x05C8, 0x02FD, 0x0453, 0x0550 } },
    { "Graveyard",
      { "From Kakariko", "From Shadow Temple", "From Gravekeepers Hut", "From Dampes Grave", "From Shield Grave",
        "From Redead Grave", "From Royal Familys Tomb", "Nocturne of Shadow Warp", "Blue Warp from Bongo Bongo" },
      { 0x00E4, 0x0205, 0x0355, 0x0359, 0x035D, 0x0361, 0x050B, 0x0568, 0x0580 } },
    { "Graves",
      { "Dampes Grave Minigame", "Royal Familys Tomb", "Royal Familys Tomb, Suns Song Cutscene" },
      { 0x044F, 0x002D, 0x0574 } },
    { "Death Mountain",
      { "From Kakariko Village", "From Goron City", "From Death Mountain Crater", "From Dodongos Cavern",
        "From Great Fairy", "Great Fairy", "Goron Ruby Cutscene" },
      { 0x013D, 0x01B9, 0x01BD, 0x0242, 0x045B, 0x0315, 0x047A } },
    { "Goron City",
      { "From Death Mountain Trail", "From Death Mountain Crater", "From Goron City Shop", "From Lost Woods",
        "Goron City Shop" },
      { 0x014D, 0x01C1, 0x03FC, 0x04E2, 0x037C } },
    { "Death Mountain Crater",
      { "From Death Mountain Trail", "From Goron City", "From Fire Temple", "From Fairy Fountain", "Great Fairy",
        "Bolero of Fire Warp", "Blue Warp from Volvagia" },
      { 0x0147, 0x0246, 0x024A, 0x0482, 0x04BE, 0x04F6, 0x0564 } },
    { "Zora River",
      { "From Hyrule Field (Land)", "From Hyrule Field (Water)", "From Zoras Fountain", "From Zoras Domain",
        "From Lost Woods" },
      { 0x00EA, 0x01D9, 0x0199, 0x019D, 0x01DD } },
    { "Zoras Domain",
      { "From Zora River", "From Zoras Fountain", "From Lake Hylia", "From Zora Shop", "Zora Shop" },
      { 0x0108, 0x01A1, 0x0328, 0x03C4, 0x0380 } },
    { "Zoras Fountain",
      { "From Zoras Domain", "From Jabu Jabu", "From Ice Cavern", "From Fairy Fountain", "Great Fairy",
        "Facing Fallen Tree", "Saphire Cutscene" },
      { 0x0225, 0x0221, 0x03D4, 0x0394, 0x0371, 0x03D8, 0x010E } },
    { "Lake Hylia",
      { "From Hyrule Field", "From Gerudo Valley", "From Water Temple", "From Fishing Pond", "From Laboratory",
        "From Zoras Domain", "Sheik After Water Temple Cutscene", "Serenade Of Water Warp", "Blue Warp from Morpha" },
      { 0x0102, 0x0219, 0x021D, 0x0309, 0x03CC, 0x0560, 0x04E6, 0x0604, 0x060C } },
    { "Lake Hylia Buildings", { "Laboratory", "Fishing Pond Minigame" }, { 0x0043, 0x045F } },
    { "Gerudo Valley",
      { "From Hyrule Field", "From Gerudo Fortress", "Thrown out of Fortress", "Fortress loading zone" },
      { 0x0117, 0x022D, 0x01A5, 0x0229 } },
    { "Gerudo Fortress",
      { "From Gerudo Valley", "From Traning Grounds", "From Haunted Wasteland", "From Thieves Hideout (1)",
        "From Thieves Hideout (2)", "From Thieves Hideout (3)", "From Thieves Hideout (4)", "From Thieves Hideout (5)",
        "From Thieves Hideout (6)", "From Thieves Hideout (7)", "From Thieves Hideout (8)", "From Thieves Hideout (9)",
        "From Thieves Hideout (10)", "From Thieves Hideout (11)", "From Thieves Hideout (12)",
        "From Thieves Hideout (13)", "Horseback Riding Minigame", "Captured First Time", "Captured Second Time" },
      { 0x0129, 0x03A8, 0x03AC, 0x0231, 0x0235, 0x0239, 0x02AA, 0x02BA, 0x02BE, 0x02C2, 0x02C6, 0x02D2, 0x02D6, 0x02DA,
        0x02DE, 0x03A4, 0x03B0, 0x03B4, 0x05F8 } },
    { "Thieves Hideout",
      { "From Gerudo Fortress (1)", "From Gerudo Fortress (2)", "From Gerudo Fortress (3)", "From Gerudo Fortress (4)",
        "From Gerudo Fortress (5)", "From Gerudo Fortress (6)", "From Gerudo Fortress (7)", "From Gerudo Fortress (8)",
        "From Gerudo Fortress (9)", "From Gerudo Fortress (10)", "From Gerudo Fortress (11)",
        "From Gerudo Fortress (12)", "From Gerudo Fortress (13)" },
      { 0x0486, 0x048A, 0x048E, 0x0492, 0x0496, 0x049A, 0x049E, 0x04A2, 0x04A6, 0x04AA, 0x04AE, 0x04B2, 0x0570 } },
    { "Haunted Wasteland", { "From Gerudo Fortress", "From Desert Colossus" }, { 0x0130, 0x0365 } },
    { "Desert Colossus",
      { "From Haunted Wasteland", "From Spirit Temple", "From Spirit Temple (Left Hand)",
        "From Spirit Temple (Right Hand)", "From Fairy Fountain", "Great Fairy", "After Sheik Cutscene",
        "Naboora Capture Cutscene", "Requiem of Spirit Warp", "Blue Warp from Twinrova" },
      { 0x0123, 0x01E1, 0x01E5, 0x01E9, 0x057C, 0x0588, 0x01ED, 0x01F5, 0x01F1, 0x0610 } },
    { "Deku Tree",
      { "Deku Tree - Entrance", "Deku Tree - Before Gohma", "Deku Tree - Gohma Fight" },
      { 0x0001, 0x0252, 0x040F } },
    { "Dodongos Cavern",
      { "Dodongos Cavern - Entrance", "Dodongos Cavern - Before King Dodongo", "Dodongos Cavern - King Dodongo" },
      { 0x0004, 0x00C5, 0x040B } },
    { "Jabu Jabu",
      { "Jabu Jabu - Entrance", "Jabu Jabu - Skulltula Water Room", "Jabu Jabu - Barinade Fight" },
      { 0x0028, 0x0407, 0x0301 } },
    { "Child Link Misc",
      { "Bottom of Well - Entrancee", "Spirit Temple - Entrance", "Spirit Temple - From Left Hand" },
      { 0x0098, 0x0082, 0x03F0 } },
    { "Forest Temple",
      { "Forest Temple - Entrance", "Forest Temple - Crushing Room", "Forest Temple - Before Phantom Ganon" },
      { 0x0169, 0x0584, 0x024E } },
    { "Fire Temple",
      { "Fire Temple - Entrance", "Fire Temple - Before Volvagia", "Fire Temple - Volvagia" },
      { 0x0165, 0x0175, 0x0305 } },
    { "Water Temple", { "Water Temple - Entrance", "Water Temple - Barinade" }, { 0x0010, 0x0417 } },
    { "Shadow Temple",
      { "Shadow Temple - Entrance", "Shadow Temple - Outside Bongo Bongo", "Shadow Temple - Bongo Bongo" },
      { 0x0037, 0x02B2, 0x0413 } },
    { "Spirit Temple",
      { "Spirit Temple - Entrance", "Spirit Temple - From Left Hand", "Spirit Temple - From Right Hand",
        "Spirit Temple - Before Twinrova", "Spirit Temple - Naboora Fight", "Spirit Temple - Twinrova" },
      { 0x0082, 0x03F0, 0x03F4, 0x02F5, 0x008D, 0x05EC } },
    { "Ganons Castle",
      { "Ganons Castle - Entrance", "Ganons Castle - Forest Clear", "Ganons Castle - Fire Clear",
        "Ganons Castle - Water Clear", "Ganons Castle - Shadow Clear", "Ganons Castle - Light Clear",
        "Ganons Castle - Spirit Clear", "Ganons Castle - From Climbing Tower", "Ganons Tower - From Castle Entrance",
        "Ganons Tower - Outside Boss (1)", "Ganons Tower - Outside Boss (2)" },
      { 0x0467, 0x0538, 0x0544, 0x053C, 0x0540, 0x0548, 0x054C, 0x0534, 0x041B, 0x0427, 0x042B } },
    { "Collapsing Castle",
      { "Interior (1)", "Interior (2)", "Interior (3)", "Interior (4)", "Interior (5)", "Interior (6)", "Interior (7)",
        "Interior (8)", "Interior (9)", "Interior - Exit", "Exterior (1)", "Exterior (2)", "Exterior (3)",
        "Exterior (4)", "Exterior (5)", "Exterior (6)", "Ganon Battle", "Ganon Death Cutscene" },
      { 0x0134, 0x0179, 0x01B5, 0x0256, 0x03DC, 0x03E0, 0x03E4, 0x04B6, 0x04BA, 0x056C, 0x01C9, 0x032C, 0x0330, 0x0334,
        0x051C, 0x0524, 0x0517, 0x043F } },
    { "Adult Link Misc", { "Ice Cavern", "Gerudo Training Grounds" }, { 0x0088, 0x0008 } }
};

int32_t customWarpEntrance;
_Vec3f pos;
int16_t yaw;
bool warped = false;

int32_t getSize(int32_t array[]) {
    int i = 0;
    while (array[i] != 0) {
        i = i + 1;
    }
    return (i);
}

void DrawWarpTab() {
    
    ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

    if (ImGui::Button("Custom Warp Set")) {
        pos = GET_PLAYER(gGlobalCtx)->actor.world.pos;
        yaw = GET_PLAYER(gGlobalCtx)->actor.shape.rot.y;
        customWarpEntrance = gSaveContext.entranceIndex;
    }

    ImGui::SameLine();
    if (ImGui::Button("Go To Custom Warp")) {
        gGlobalCtx->nextEntranceIndex = customWarpEntrance;
        gGlobalCtx->sceneLoadFlag = 0x14;
        gGlobalCtx->fadeTransition = 11;
        gSaveContext.nextTransition = 11;
        warped = true;
    }

    if (warped && gGlobalCtx->sceneLoadFlag != 0x14 && gSaveContext.nextTransition == 255) {
        GET_PLAYER(gGlobalCtx)->actor.world.pos = pos;
        GET_PLAYER(gGlobalCtx)->actor.shape.rot.y = yaw;
        warped = false;
    }

    if (ImGui::Button("Child Warp")) {
        CVar_SetS32("gWarpConfig", 1);
    }
    ImGui::SameLine();
    if (ImGui::Button("Adult Warp")) {
        CVar_SetS32("gWarpConfig", 2);
    }
    ImGui::SameLine();
    if (ImGui::Button("Day Warp")) {
        CVar_SetS32("gWarpConfig", 3);
    }
    ImGui::SameLine();
    if (ImGui::Button("Night Warp")) {
        CVar_SetS32("gWarpConfig", 4);
    }

    for (int i = 0; i < (sizeof(warpMap) / sizeof(warpMap[0])); i++) {
        if (i == 0 || i == 2 || i == 6 || i == 8 || i == 14 || i == 16 || i == 20 || i == 23 || i == 26 || i == 28 ||
            i == 33 || i == 37) {
            if (i == 33 || i==37) {
                int32_t max = 0;
                const char* title;
                if (i == 33) {
                    title = "Child Link Dungeons";
                    max = 4;
                } else {
                    title = "Adult Link Dungeons";
                    max = 8;                
                }
                if (ImGui::TreeNode(title)) {
                    for (int k = 0; k < max; k++) {
                        if (ImGui::TreeNode(warpMap[i + k].category)) {
                            for (int j = 0; j < warpMap[i + k].getSize(); j++) {
                                if (ImGui::Button(warpMap[i + k].locations[j])) {
                                    CVar_SetS32("gDebugWarp", warpMap[i + k].indexes[j]);
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }
            else {
                if (ImGui::TreeNode(warpMap[i].category)) {
                    for (int j = 0; j < warpMap[i].getSize(); j++) {
                        if (ImGui::Button(warpMap[i].locations[j])) {
                            CVar_SetS32("gDebugWarp", warpMap[i].indexes[j]);
                        }
                    }
                    int32_t max = 0;
                    if (i == 6 || i == 0 || i == 14 || i == 26) {
                        max = 2;
                    } else if (i == 2 || i == 20 || i == 23) {
                        max = 3;
                    } else if (i == 16) {
                        max = 4;
                    } else if (i == 28) {
                        max = 5;
                    } else if (i == 8) {
                        max = 6;
                    }
                    for (int k = 1; k < max; k++) {
                        if (ImGui::TreeNode(warpMap[i + k].category)) {
                            for (int j = 0; j < warpMap[i + k].getSize(); j++) {
                                if (ImGui::Button(warpMap[i + k].locations[j])) {
                                    CVar_SetS32("gDebugWarp", warpMap[i + k].indexes[j]);
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                ImGui::TreePop();
                }
            }
        }
    }
    if (ImGui::TreeNode("Grottos")) {
        if (ImGui::TreeNode("Unique Grottos")) {
            if (ImGui::Button("Fairy Fountain")) {
                CVar_SetS32("gWarpGrottoCheat", 1);
            }
            if (ImGui::Button("Hyrule Field near Kakariko")) {
                CVar_SetS32("gWarpGrottoCheat", 3);
            }
            if (ImGui::Button("Kakariko near tree")) {
                CVar_SetS32("gWarpGrottoCheat", 5);
            }
            if (ImGui::Button("Hyrule Field near Gerudo Valley under boulder")) {
                CVar_SetS32("gWarpGrottoCheat", 7);
            }
            if (ImGui::Button("Under boulder in Gerudo Canyon")) {
                CVar_SetS32("gWarpGrottoCheat", 8);
            }
            if (ImGui::Button("Sacred Forest Wolfoes Grotto")) {
                CVar_SetS32("gWarpGrottoCheat", 10);
            }
            if (ImGui::Button("Tree near Hyrule Castle Courtyard")) {
                CVar_SetS32("gWarpGrottoCheat", 11);
            }
            if (ImGui::Button("Tektite Grotto between Castle Town and Gerudo Valley")) {
                CVar_SetS32("gWarpGrottoCheat", 13);
            }
            if (ImGui::Button("Deku Theater in Lost Woods")) {
                CVar_SetS32("gWarpGrottoCheat", 14);
            }
            if (ImGui::Button("Death Mountain Trail Cow Grotto")) {
                CVar_SetS32("gWarpGrottoCheat", 15);
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Treasure Chest Grottos")) {
            if (ImGui::Button(SohUtils::GetSceneName(85).c_str())) {
                //respawnData->entranceIndex = 646;
                //respawnData->roomIndex = 11264;
                //respawnData->playerParams = 0x4FF;
                //respawnData->pos = { -510.587, 380.000, -1223.780 };
                //respawnData->yaw = 16384;
                CVar_SetS32("gWarpGrottoCheat", 21);
            }
            if (ImGui::Button(SohUtils::GetSceneName(91).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 22);
            }
            char temp[30];
            sprintf(temp, SohUtils::GetSceneName(81).c_str());
            strcat(temp, ": Near Market");
            if (ImGui::Button(temp)) {
                CVar_SetS32("gWarpGrottoCheat", 23);
            }
            sprintf(temp, SohUtils::GetSceneName(81).c_str());
            strcat(temp, ": Open Grotto");
            if (ImGui::Button(temp)) {
                CVar_SetS32("gWarpGrottoCheat", 24);
            }
            sprintf(temp, SohUtils::GetSceneName(81).c_str());
            strcat(temp, ": SE Boulder");
            if (ImGui::Button(temp)) {
                CVar_SetS32("gWarpGrottoCheat", 25);
            }
            if (ImGui::Button(SohUtils::GetSceneName(82).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 26);
            }
            if (ImGui::Button(SohUtils::GetSceneName(96).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 27);
            }
            if (ImGui::Button(SohUtils::GetSceneName(97).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 28);
            }
            if (ImGui::Button(SohUtils::GetSceneName(84).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 29);
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Deku Scrub Grottos")) {
            if (ImGui::Button(SohUtils::GetSceneName(81).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 4);
            }
            if (ImGui::Button(SohUtils::GetSceneName(84).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 31);
            } else if (ImGui::Button(SohUtils::GetSceneName(86).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 32);
            } else if (ImGui::Button(SohUtils::GetSceneName(87).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 33);
            } else if (ImGui::Button(SohUtils::GetSceneName(90).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 34);
            } else if (ImGui::Button(SohUtils::GetSceneName(91).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 35);
            } else if (ImGui::Button(SohUtils::GetSceneName(97).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 36);
            } else if (ImGui::Button(SohUtils::GetSceneName(93).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 37);
            } else if (ImGui::Button(SohUtils::GetSceneName(99).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 38);
            } else if (ImGui::Button(SohUtils::GetSceneName(92).c_str())) {
                CVar_SetS32("gWarpGrottoCheat", 39);
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Shops")) {
        for (int i = 0; i < (sizeof(warpMap) / sizeof(warpMap[0])); i++) {
            for (int j = 0; j < 20; j++) {
                if (warpMap[i].getIndexSub((char*)"Shop")[j] != 0) {
                    std::string temp(warpMap[i].getLocation(warpMap[i].getIndexSub((char*)"Shop")[j]));
                    if (temp != "NONE" && temp.find("From")==-1) {
                        if (ImGui::Button(warpMap[i].getLocation(warpMap[i].getIndexSub((char*)"Shop")[j]))) {
                            CVar_SetS32("gDebugWarp", (warpMap[i].getIndexSub((char*)"Shop")[j]));
                        }
                    }
                }
            }
        }
        ImGui::TreePop(); 
    } 
    if (ImGui::TreeNode("Fairy Fountains")) {
        for (int i = 0; i < sizeof(warpMap) / sizeof(warpMap[0]); i++) {
            if (warpMap[i].getIndex((char*)"Great Fairy") != 0) {
                if (ImGui::Button(warpMap[i].category)) {
                    CVar_SetS32("gDebugWarp", warpMap[i].getIndex((char*)"Great Fairy"));
                }
            }
        }
        ImGui::TreePop();
    } 
    if (ImGui::TreeNode("Minigames")) {
        for (int i = 0; i < (sizeof(warpMap) / sizeof(warpMap[0])); i++) {
            for (int j = 0; j < 20; j++) {
                if (warpMap[i].getIndexSub((char*)"Minigame")[j] != 0) {
                    std::string temp(warpMap[i].getLocation(warpMap[i].getIndexSub((char*)"Minigame")[j]));
                    if (temp != "NONE" && temp.find("From") == -1) {
                        if (ImGui::Button(warpMap[i].getLocation(warpMap[i].getIndexSub((char*)"Minigame")[j]))) {
                            CVar_SetS32("gDebugWarp", (warpMap[i].getIndexSub((char*)"Minigame")[j]));
                        }
                    }
                }
            }
        }
        ImGui::TreePop();
    }
    static char text[128] = "";
    ImGui::InputText("Search For Warp/Area", text, IM_ARRAYSIZE(text));


    if (ImGui::TreeNode("Search Results (Entrances)")) {
        for (int i = 0; i < (sizeof(warpMap) / sizeof(warpMap[0])); i++) {
            for (int j = 0; j < warpMap[i].getSize(); j++) {
                if (warpMap[i].getInd(text, j)) {
                    std::string title(warpMap[i].category);
                    title = title + ": " + std::string(warpMap[i].locations[j]);
                    if (ImGui::Button(title.c_str())) {
                        CVar_SetS32("gDebugWarp", warpMap[i].indexes[j]);
                    }
                }
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Search Results (Areas)")) {
        for (int i = 0; i < (sizeof(warpMap) / sizeof(warpMap[0])); i++) {
            std::string temp = warpMap[i].category;
            if (temp.find(text) != -1) {
                if (i == 0 || i == 2 || i == 6 || i == 8 || i == 14 || i == 16 || i == 20 || i == 23 || i == 26 ||
                    i == 28 || i == 33 || i == 37) {
                    if (i == 33 || i == 37) {
                        int32_t max = 0;
                        const char* title;
                        if (i == 33) {
                            title = "Child Link Dungeons";
                            max = 4;
                        } else {
                            title = "Adult Link Dungeons";
                            max = 8;
                        }
                        if (ImGui::TreeNode(title)) {
                            for (int k = 0; k < max; k++) {
                                if (ImGui::TreeNode(warpMap[i + k].category)) {
                                    for (int j = 0; j < warpMap[i + k].getSize(); j++) {
                                        if (ImGui::Button(warpMap[i + k].locations[j])) {
                                            CVar_SetS32("gDebugWarp", warpMap[i + k].indexes[j]);
                                        }
                                    }
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    } else {
                        if (ImGui::TreeNode(warpMap[i].category)) {
                            for (int j = 0; j < warpMap[i].getSize(); j++) {
                                if (ImGui::Button(warpMap[i].locations[j])) {
                                    CVar_SetS32("gDebugWarp", warpMap[i].indexes[j]);
                                }
                            }
                            int32_t max = 0;
                            if (i == 6 || i == 0 || i == 14 || i == 26) {
                                max = 2;
                            } else if (i == 2 || i == 20 || i == 23) {
                                max = 3;
                            } else if (i == 16) {
                                max = 4;
                            } else if (i == 28) {
                                max = 5;
                            } else if (i == 8) {
                                max = 6;
                            }
                            for (int k = 1; k < max; k++) {
                                if (ImGui::TreeNode(warpMap[i + k].category)) {
                                    for (int j = 0; j < warpMap[i + k].getSize(); j++) {
                                        if (ImGui::Button(warpMap[i + k].locations[j])) {
                                            CVar_SetS32("gDebugWarp", warpMap[i + k].indexes[j]);
                                        }
                                    }
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                } else {
                    if (ImGui::TreeNode(warpMap[i].category)) {
                        for (int j = 0; j < warpMap[i].getSize(); j++) {
                            if (ImGui::Button(warpMap[i].locations[j])) {
                                CVar_SetS32("gDebugWarp", warpMap[i].indexes[j]);
                            }
                        }
                        ImGui::TreePop();
                    }
                }
            }
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
            DrawFlagArray16("eci10", gSaveContext.eventChkInf[10]);
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
    ImGui::Text("%s", categoryName.c_str());
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
        InsertHelpHoverText("Height used to determine whether Link can climb or grab a ledge at the top");

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
            if (ImGui::Selectable("Fishing Pole")) {
                player->currentSwordItem = ITEM_FISHING_POLE;
                gSaveContext.equips.buttonItems[0] = ITEM_FISHING_POLE;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_MASTER);
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

            if (CVar_GetS32("gDpadEquips", 0)) {
                ImGui::NewLine();
                ImGui::Text("Current D-pad Equips");
                ImGui::InputScalar("D-pad Up  ", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[4], &one, NULL); // Two spaces at the end for aligning, not elegant but it's working
                ImGui::SameLine();
                ImGui::InputScalar("D-pad Down", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[5], &one, NULL);
                // Intentionnal to not put everything on the same line, else it's taking too much for lower resolution.
                ImGui::InputScalar("D-pad Left", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[6], &one, NULL);
                ImGui::SameLine();
                ImGui::InputScalar("D-pad Right", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[7], &one, NULL);
            }
        });

    } else {
        ImGui::Text("Global Context needed for player info!");
    }
}

void DrawSaveEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gSaveEditorEnabled", 0);
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
        
        if (ImGui::BeginTabItem("Warp")) {
            DrawWarpTab();
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
    for (const auto& entry : vanillaSongMapping) {
        SohImGui::LoadResource(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        SohImGui::LoadResource(entry.nameFaded, gSongNoteTex, fadedCol);
    }
}

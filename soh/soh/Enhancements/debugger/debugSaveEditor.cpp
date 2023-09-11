#include "debugSaveEditor.h"
#include "../../util.h"
#include "../../OTRGlobals.h"
#include "../../UIWidgets.hpp"

#include <spdlog/fmt/fmt.h>
#include <array>
#include <bit>
#include <map>
#include <string>
#include <functional>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
extern PlayState* gPlayState;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/parameter_static/parameter_static.h"
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
    ITEM_MAP_ENTRY(ITEM_HEART_PIECE),
    ITEM_MAP_ENTRY(ITEM_MAGIC_SMALL),
    ITEM_MAP_ENTRY(ITEM_MAGIC_LARGE)
};

std::map<uint32_t, ItemMapEntry> gregMapping = {
    {ITEM_RUPEE_GREEN, {ITEM_RUPEE_GREEN, "ITEM_RUPEE_GREEN", "ITEM_RUPEE_GREEN_Faded", gRupeeCounterIconTex}}
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
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FOREST, dgQuestIconMedallionForestTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FIRE, dgQuestIconMedallionFireTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_WATER, dgQuestIconMedallionWaterTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SPIRIT, dgQuestIconMedallionSpiritTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SHADOW, dgQuestIconMedallionShadowTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_LIGHT, dgQuestIconMedallionLightTex),
    QUEST_MAP_ENTRY(QUEST_KOKIRI_EMERALD, dgQuestIconKokiriEmeraldTex),
    QUEST_MAP_ENTRY(QUEST_GORON_RUBY, dgQuestIconGoronRubyTex),
    QUEST_MAP_ENTRY(QUEST_ZORA_SAPPHIRE, dgQuestIconZoraSapphireTex),
    QUEST_MAP_ENTRY(QUEST_STONE_OF_AGONY, dgQuestIconStoneOfAgonyTex),
    QUEST_MAP_ENTRY(QUEST_GERUDO_CARD, dgQuestIconGerudosCardTex),
    QUEST_MAP_ENTRY(QUEST_SKULL_TOKEN, dgQuestIconGoldSkulltulaTex),
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

char ASCII2z(int code) {
    int ret;
    if (code >= 0x30 && code < 0x3A) { //Digits
        ret = code - 0x30;
    } else if (code >= 0x41 && code < 0x5B) { //Uppercase letters
        ret = code - 0x37;
    } else if (code >= 0x61 && code < 0x7B) { //Lowercase letters
        ret = code - 0x3D;
    } else if (code == 0x20 || code == 0x0) { //Space
        ret = 62;
    } else if (code == 0x5F || code == 0x2E) { // _ and .
        ret = code + 0x12;
    } else {
        ret = code;
    }
    return char(ret);
}

char* getPlayerName() {
    char* playerName = new char[9];
    for (int i = 0; i < 8; i++) {
        playerName[i] = z2ASCII(gSaveContext.playerName[i]);
    }
    playerName[8] = '\0';
    return playerName;
}

int setPlayerName(ImGuiInputTextCallbackData* data) {
    for (int i = 0; i < 8; i++) {
        gSaveContext.playerName[i] = ASCII2z(data->Buf[i]);
    }
    return 0;
};

const ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
const ImVec4 GRAY = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
const ImVec4 RED = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
const ImVec4 DARK_RED = ImVec4(0.3f, 0.0f, 0.0f, 1.0f);
const ImVec4 LIGHT_GREEN = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);
const ImVec4 GREEN = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
const ImVec4 DARK_GREEN = ImVec4(0.0f, 0.3f, 0.0f, 1.0f);
const ImVec4 YELLOW = ImVec4(1.0f, 0.627f, 0.0f, 1.0f);
const float INV_GRID_WIDTH = 46.0f;
const float INV_GRID_HEIGHT = 58.0f;
const float INV_GRID_ICON_SIZE = 40.0f;
const float INV_GRID_PADDING = 10.0f;
const float INV_GRID_TOP_MARGIN = 30.0f;
const uint8_t U8_ZERO = 0;
const int8_t S8_ZERO = 0;
const int16_t S16_MIN = INT16_MIN;
const int16_t S16_ZERO = 0;
const int16_t S16_MAX = INT16_MAX;
const float POS_MAX = 10000.0f;
const float POS_MIN = -10000.0f;
const uint16_t HEART_COUNT_MIN = 3;
const uint16_t HEART_COUNT_MAX = 20;
const uint8_t PIECE_OF_HEART_COLLECTED_MAX = 36;
const uint8_t HEART_CONTAINER_COLLECTED_MAX = 8;
const uint8_t PIECE_OF_HEART_CURRENT_MAX = 3;
const char* MAGIC_LEVEL_NAMES[3] = { "No Magic", "Single Magic", "Double Magic" };
const int8_t MAGIC_LEVEL_MAX = 2;
const char* WALLET_NAMES[3] = { "Child Wallet", "Adult Wallet", "Giant Wallet" };
const int8_t WALLET_TYPE_MAX = 2;
const char* EQUIP_EDITOR_MODES[2] = { "Item IDs", "Slot IDs" };
const int8_t EQUIP_EDITOR_MAX = 1;
const char* swordValues[] = { "None", "Kokiri Sword", "Master Sword", "Giant's Knife/BGS" };
const char* shieldValues[] = { "No Shield", "Deku Shield", "Hylian Shield", "Mirror Shield", "0x4", "0x5", "0x6", "0x7", "0x8", "0x9", "0xA", "0xB", "0xC", "0xD", "0xE", "0xF" };
const char* tunicValues[] = { "Kokiri Tunic", "Goron Tunic", "Zora Tunic", "0x3 (White)", "0x4 (Yellow)", "0x5", "0x6 (Red)", "0x7", "0x8", "0x9", "0xA", "0xB", "0xC", "0xD", "0xE", "0xF" };
const char* bootsValues[] = { "Kokiri Boots", "Iron Boots", "Hover Boots", "0x3", "0x4", "0x5", "0x6", "0x7", "0x8", "0x9", "0xA", "0xB", "0xC", "0xD", "0xE", "0xF" };

bool safeMode = true;
uint8_t equipEditorMode = 0;
int32_t selectedInventorySlot = -1;
std::vector<ItemID> legalItemsForInventorySlot[SLOT_TUNIC_KOKIRI] = {};

void initLegalItemsForInventorySlot() {
    for (int i = 0; i < sizeof(gItemSlots); i++) {
        InventorySlot invSlot = static_cast<InventorySlot>(gItemSlots[i]);
        legalItemsForInventorySlot[invSlot].push_back(static_cast<ItemID>(i));
        if (invSlot == SLOT_BOTTLE_1) {
            legalItemsForInventorySlot[SLOT_BOTTLE_2].push_back(static_cast<ItemID>(i));
            legalItemsForInventorySlot[SLOT_BOTTLE_3].push_back(static_cast<ItemID>(i));
            legalItemsForInventorySlot[SLOT_BOTTLE_4].push_back(static_cast<ItemID>(i));
        }
    }
}

template<typename C, typename T>
bool contains(C&& c, T e) { 
    return std::find(std::begin(c), std::end(c), e) != std::end(c);
};

template<typename C, typename T>
void fill(C&& c, T e) { 
    return std::fill(std::begin(c), std::end(c), e);
};

template<typename T>
void ColoredButton(const ImVec4& color, T&& drawFunc) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.x, color.y, color.z, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(color.x, color.y, color.z, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5.0f);
    drawFunc();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(4);
}

template<typename T>
void ColoredCheckbox(const ImVec4& color, T&& drawFunc) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(color.x, color.y, color.z, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(color.x, color.y, color.z, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5.0f);
    drawFunc();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(5);
}

template<typename T>
void ColoredSlider(const ImVec4& color, T&& drawFunc) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0, 1.0, 1.0, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0, 1.0, 1.0, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    drawFunc();
    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor(6);
}

template<typename T>
void ColoredInputText(const ImVec4& color, T&& drawFunc) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));
    drawFunc();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

template<typename T>
void SmallColoredInputText(const ImVec4& color, T&& drawFunc) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
    drawFunc();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

void DrawGeneralTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("generalTab", ImVec2(0, 0), true);
    ColoredInputText(GRAY, []() {
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.33f - 4.0f);
        ImGui::SetCursorPosY(10.0f);
        ImGui::BeginGroup();
        ImGui::Text("Player Name");
        ImGui::InputText("##playerNameInput", getPlayerName(), 9, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_AutoSelectAll, setPlayerName);
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::SetCursorPosY(4.0f);
        ImGui::BeginGroup();
        ImGui::Text("Deaths");
        ImGui::InputScalar("##deathsInput", ImGuiDataType_U16, &gSaveContext.deaths);
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::SetCursorPosY(4.0f);
        ImGui::BeginGroup();
        ImGui::Text("Total Days");
        ImGui::InputScalar("##totalDaysInput", ImGuiDataType_S32, &gSaveContext.totalDays);
        ImGui::EndGroup();
        ImGui::PopItemWidth();
    });
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.66f);
    ImGui::BeginGroup();
    ColoredButton(RED, []() {
        if (ImGui::Button("Max Health")) {
            gSaveContext.isDoubleDefenseAcquired = 1;
            gSaveContext.inventory.defenseHearts = 20;
            gSaveContext.healthCapacity = gSaveContext.health = 20 * 16;
            gSaveContext.inventory.questItems &= ~0xF0000000;
            gSaveContext.sohStats.heartPieces = 36;
            gSaveContext.sohStats.heartContainers = 8;
        }
    });
    ImGui::SameLine();
    ColoredButton(GRAY, []() {
        if (ImGui::Button("Reset##resetHealthButton")) {
            gSaveContext.isDoubleDefenseAcquired = 0;
            gSaveContext.inventory.defenseHearts = 0;
            gSaveContext.healthCapacity = gSaveContext.health = 3 * 16;
            gSaveContext.inventory.questItems &= ~0xF0000000;
            gSaveContext.sohStats.heartPieces = 0;
            gSaveContext.sohStats.heartContainers = 0;
        }
    });
    ImGui::SameLine();
    ColoredCheckbox(RED, []() {
        if (ImGui::Checkbox("Double Defense", (bool *)&gSaveContext.isDoubleDefenseAcquired)) {
            if (gSaveContext.isDoubleDefenseAcquired) {
                gSaveContext.inventory.defenseHearts = 20;
            } else {
                gSaveContext.inventory.defenseHearts = 0;
            }
        }
    });
    ColoredSlider(DARK_RED, []() {
        uint16_t heartCount = (uint16_t)gSaveContext.healthCapacity / 16;
        if (ImGui::SliderScalar("##heartCountSlider", ImGuiDataType_U16, &heartCount, &HEART_COUNT_MIN, &HEART_COUNT_MAX, "Max Hearts: %d")) {
            gSaveContext.healthCapacity = heartCount * 16;
            gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
        }
        ImGui::SliderScalar("##healthSlider", ImGuiDataType_S16, &gSaveContext.health, &S16_ZERO, &gSaveContext.healthCapacity, "Health: %d");
    });
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::BeginGroup();
    ColoredSlider(GRAY, []() {
        ImGui::VSliderScalar("##pieceOfHeartCollectedSlider", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f - 4.0f, 90.0f), ImGuiDataType_U8, &gSaveContext.sohStats.heartPieces, &U8_ZERO, &PIECE_OF_HEART_COLLECTED_MAX);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pieces of Heart Collected");
        ImGui::SameLine();
        ImGui::VSliderScalar("##heartContainerCollectedSlider", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - 4.0f, 90.0f), ImGuiDataType_U8, &gSaveContext.sohStats.heartContainers, &U8_ZERO, &HEART_CONTAINER_COLLECTED_MAX);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Heart Containers Collected");
        ImGui::SameLine();
        uint8_t pohCurrent = (gSaveContext.inventory.questItems & 0xF0000000) >> 28;
        if (ImGui::VSliderScalar("##pieceOfHeartCurrentSlider", ImVec2(ImGui::GetContentRegionAvail().x - 4.0f, 90.0f), ImGuiDataType_U8, &pohCurrent, &U8_ZERO, &PIECE_OF_HEART_CURRENT_MAX)) {
            gSaveContext.inventory.questItems &= ~0xF0000000;
            gSaveContext.inventory.questItems |= (pohCurrent << 28);
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pieces of Heart in quest inventory");
    });
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - 4.0f);
    ImGui::BeginGroup();
    ColoredButton(GREEN, []() {
        if (ImGui::Button("Max Magic")) {
            gSaveContext.magicLevel = 2;
            gSaveContext.magic = gSaveContext.magicCapacity = gSaveContext.magicLevel * 48;
            gSaveContext.isMagicAcquired = true;
            gSaveContext.isDoubleMagicAcquired = true;
        }
    });
    ImGui::SameLine();
    ColoredButton(GRAY, []() {
        if (ImGui::Button("Reset##resetMagicButton")) {
            gSaveContext.magicLevel = 0;
            gSaveContext.magic = gSaveContext.magicCapacity = 0;
            gSaveContext.isMagicAcquired = false;
            gSaveContext.isDoubleMagicAcquired = false;
        }
    });
    ColoredSlider(DARK_GREEN, []() {
        if (ImGui::SliderScalar("##magicLevelSlider", ImGuiDataType_S8, &gSaveContext.magicLevel, &S8_ZERO, &MAGIC_LEVEL_MAX, MAGIC_LEVEL_NAMES[gSaveContext.magicLevel])) {
            gSaveContext.magicCapacity = gSaveContext.magicLevel * 48;
            gSaveContext.magic = MIN(gSaveContext.magic, gSaveContext.magicCapacity);
            switch (gSaveContext.magicLevel) {
                case 0:
                    gSaveContext.isMagicAcquired = false;
                    gSaveContext.isDoubleMagicAcquired = false;
                    break;
                case 1:
                    gSaveContext.isMagicAcquired = true;
                    gSaveContext.isDoubleMagicAcquired = false;
                    break;
                case 2:
                    gSaveContext.isMagicAcquired = true;
                    gSaveContext.isDoubleMagicAcquired = true;
                    break;
            }
        }
        ImGui::SliderScalar("##magicSlider", ImGuiDataType_S8, &gSaveContext.magic, &S8_ZERO, &gSaveContext.magicCapacity, "Magic: %d");
    });
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ColoredButton(LIGHT_GREEN, []() {
        if (ImGui::Button("Max Rupees")) {
            Inventory_ChangeUpgrade(UPG_WALLET, 2);
            gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
        }
    });
    ImGui::SameLine();
    ColoredButton(GRAY, []() {
        if (ImGui::Button("Reset##resetRupeesButton")) {
            Inventory_ChangeUpgrade(UPG_WALLET, 0);
            gSaveContext.rupees = 0;
        }
    });
    ColoredSlider(GREEN, []() {
        uint8_t currentWallet = CUR_UPG_VALUE(UPG_WALLET);
        if (ImGui::SliderScalar("##walletSlider", ImGuiDataType_U8, &currentWallet, &U8_ZERO, &WALLET_TYPE_MAX, WALLET_NAMES[CUR_UPG_VALUE(UPG_WALLET)])) {
            Inventory_ChangeUpgrade(UPG_WALLET, currentWallet);
            gSaveContext.rupees = MIN(gSaveContext.rupees, CUR_CAPACITY(UPG_WALLET));
        }
        int16_t rupeeMax = CUR_CAPACITY(UPG_WALLET);
        ImGui::SliderScalar("##rupeesSlider", ImGuiDataType_S16, &gSaveContext.rupees, &S16_ZERO, &rupeeMax, "Rupees: %d");
    });
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void NextAmmoItemUpgradeInSlot(InventorySlot slot, UpgradeType upgradeType) {
    ItemID currentItemID = static_cast<ItemID>(gSaveContext.inventory.items[slot]);
    ItemID targetItemID = legalItemsForInventorySlot[slot][0];

    if (currentItemID == ITEM_NONE) {
        gSaveContext.inventory.items[slot] = targetItemID;
    } else if (CUR_UPG_VALUE(upgradeType) >= 3) {
        Inventory_DeleteItem(gSaveContext.inventory.items[slot], slot);
    }
    Inventory_ChangeUpgrade(upgradeType, (CUR_UPG_VALUE(upgradeType) + 1) % 4);
    AMMO(targetItemID) = CUR_CAPACITY(upgradeType);
}

void NextItemUpgrade(UpgradeType upgradeType) {
    u8 curUpgValue = CUR_UPG_VALUE(upgradeType);

    if ((upgradeType == UPG_SCALE && curUpgValue == 2) || curUpgValue == 3) {
        Inventory_ChangeUpgrade(upgradeType, 0);
    } else {
        Inventory_ChangeUpgrade(upgradeType, curUpgValue + 1);
    }
}

void NextAmmoItemInSlot(InventorySlot slot, int capacity) {
    ItemID currentItemID = static_cast<ItemID>(gSaveContext.inventory.items[slot]);
    ItemID targetItemID = legalItemsForInventorySlot[slot][0];

    if (currentItemID == ITEM_NONE) {
        gSaveContext.inventory.items[slot] = targetItemID;
        AMMO(targetItemID) = capacity;
    } else {
        Inventory_DeleteItem(gSaveContext.inventory.items[slot], slot);
        AMMO(targetItemID) = 0;
    }
}

void NextItemInSlot(InventorySlot slot) {
    ItemID currentItemID = static_cast<ItemID>(gSaveContext.inventory.items[slot]);
    int currentItemIndex = find(legalItemsForInventorySlot[slot].begin(), legalItemsForInventorySlot[slot].end(), currentItemID) - legalItemsForInventorySlot[slot].begin();

    if (currentItemID == ITEM_NONE) {
        gSaveContext.inventory.items[slot] = legalItemsForInventorySlot[slot][0];
    } else if (currentItemIndex < legalItemsForInventorySlot[slot].size() - 1) {
        Inventory_ReplaceItem(gPlayState, currentItemID, legalItemsForInventorySlot[slot][currentItemIndex + 1]);
    } else {
        Inventory_DeleteItem(gSaveContext.inventory.items[slot], slot);
    }
}

void DrawAmmoInput(InventorySlot slot) {
    float x = slot % 6;
    float y = floor(slot / 6);
    ItemID currentItemID = static_cast<ItemID>(gSaveContext.inventory.items[slot]);

    ImGui::SetCursorPos(ImVec2(x * INV_GRID_WIDTH + INV_GRID_PADDING + 7.0f, y * INV_GRID_HEIGHT + INV_GRID_TOP_MARGIN + INV_GRID_PADDING + (INV_GRID_ICON_SIZE - 2.0f)));
    ImGui::PushItemWidth(24.0f);
    SmallColoredInputText(GRAY, [&]() {
        if (ImGui::InputScalar("##ammoInput", ImGuiDataType_S8, &AMMO(currentItemID))) {
            // Crashes when < 0 and graphical issues when > 99
            AMMO(currentItemID) = MAX(0, MIN(AMMO(currentItemID), 99));
        }
    });
    ImGui::PopItemWidth();
}

void DrawInventorySlot(InventorySlot slot) {
    float x = slot % 6;
    float y = floor(slot / 6);
    ItemID currentItemID = static_cast<ItemID>(gSaveContext.inventory.items[slot]);
    
    ImGui::PushID(slot);

    if (
        currentItemID != ITEM_NONE && 
        (safeMode && currentItemID <= ITEM_BOW || currentItemID == ITEM_SLINGSHOT || currentItemID == ITEM_BOMBCHU || currentItemID == ITEM_BEAN) ||
        (!safeMode && currentItemID <= ITEM_HAMMER)
    ) {
        DrawAmmoInput(slot);
    }

    ImGui::SetCursorPos(ImVec2(x * INV_GRID_WIDTH + INV_GRID_PADDING, y * INV_GRID_HEIGHT + INV_GRID_TOP_MARGIN + INV_GRID_PADDING));

    // isEquipped border
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    if (contains(gSaveContext.equips.cButtonSlots, slot)) {
        ImGui::PushStyleColor(ImGuiCol_Border, WHITE);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

    ImTextureID textureId = currentItemID == ITEM_NONE ? 
        LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[legalItemsForInventorySlot[slot][0]].nameFaded) : 
        LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[currentItemID].name);
    if (ImGui::ImageButton(textureId, ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (safeMode && slot < SLOT_BOTTLE_1) {
            switch (slot) {
                case SLOT_STICK:
                    NextAmmoItemUpgradeInSlot(slot, UPG_STICKS);
                    break;
                case SLOT_NUT:
                    NextAmmoItemUpgradeInSlot(slot, UPG_NUTS);
                    break;
                case SLOT_BOMB:
                    NextAmmoItemUpgradeInSlot(slot, UPG_BOMB_BAG);
                    break;
                case SLOT_BOW:
                    NextAmmoItemUpgradeInSlot(slot, UPG_QUIVER);
                    break;
                case SLOT_SLINGSHOT:
                    NextAmmoItemUpgradeInSlot(slot, UPG_BULLET_BAG);
                    break;
                case SLOT_BOMBCHU:
                    NextAmmoItemInSlot(slot, 50);
                    break;
                case SLOT_BEAN:
                    NextAmmoItemInSlot(slot, 15);
                    break;
                default:
                    NextItemInSlot(slot);
                    break;
            }
        } else {
            selectedInventorySlot = slot;
            ImGui::OpenPopup("InventorySlotPopup");
        }
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);

    if (ImGui::BeginPopup("InventorySlotPopup")) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
        if (ImGui::Button("##invNonePicker", ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE))) {
            Inventory_DeleteItem(gSaveContext.inventory.items[selectedInventorySlot], selectedInventorySlot);
            ImGui::CloseCurrentPopup();
        }
        UIWidgets::SetLastItemHoverText("None");

        int32_t availableItems = safeMode ? legalItemsForInventorySlot[selectedInventorySlot].size() : ITEM_FISHING_POLE + 1;
        for (int32_t pickerIndex = 0; pickerIndex < availableItems; pickerIndex++) {
            if (((pickerIndex + 1) % 8) != 0) {
                ImGui::SameLine();
            }
            const ItemID id = safeMode ? legalItemsForInventorySlot[selectedInventorySlot][pickerIndex] : static_cast<ItemID>(pickerIndex);
            if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[id].name), ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0)) {
                gSaveContext.inventory.items[selectedInventorySlot] = id;
                // Set adult trade item flag if you're playing adult trade shuffle in rando  
                if (gSaveContext.n64ddFlag &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE) &&
                    selectedInventorySlot == SLOT_TRADE_ADULT &&
                    id >= ITEM_POCKET_EGG && id <= ITEM_CLAIM_CHECK) {
                    gSaveContext.adultTradeItems |= ADULT_TRADE_FLAG(id);
                }
                ImGui::CloseCurrentPopup();
            }
            UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(id));
        }
        ImGui::PopStyleColor(3);
        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void DrawEquipItemMenu(InventorySlot slot) {
    ItemID currentItemID = static_cast<ItemID>(gSaveContext.inventory.items[slot]);
    if (ImGui::BeginMenu("Equip")) {
        if (ImGui::MenuItem("C-Left")) {
            gSaveContext.equips.buttonItems[1] = currentItemID;
            gSaveContext.equips.cButtonSlots[0] = slot;
        }
        if (ImGui::MenuItem("C-Down")) {
            gSaveContext.equips.buttonItems[2] = currentItemID;
            gSaveContext.equips.cButtonSlots[1] = slot;
        }
        if (ImGui::MenuItem("C-Right")) {
            gSaveContext.equips.buttonItems[3] = currentItemID;
            gSaveContext.equips.cButtonSlots[2] = slot;
        }
        if (ImGui::MenuItem("D-Up")) {
            gSaveContext.equips.buttonItems[4] = currentItemID;
            gSaveContext.equips.cButtonSlots[3] = slot;
        }
        if (ImGui::MenuItem("D-Down")) {
            gSaveContext.equips.buttonItems[5] = currentItemID;
            gSaveContext.equips.cButtonSlots[4] = slot;
        }
        if (ImGui::MenuItem("D-Left")) {
            gSaveContext.equips.buttonItems[6] = currentItemID;
            gSaveContext.equips.cButtonSlots[5] = slot;
        }
        if (ImGui::MenuItem("D-Right")) {
            gSaveContext.equips.buttonItems[7] = currentItemID;
            gSaveContext.equips.cButtonSlots[6] = slot;
        }
        ImGui::EndMenu();
    }
}

void DrawInventoryTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("inventoryTab", ImVec2(0, 0), true);
    ImGui::BeginGroup();
    ColoredButton(GRAY, []() {
        if (ImGui::Button("Give All##inventory")) {
            for (int32_t i = 0; i < SLOT_TUNIC_KOKIRI; i++) {
                gSaveContext.inventory.items[i] = legalItemsForInventorySlot[i].back();
            }
            Inventory_ChangeUpgrade(UPG_STICKS, 3); AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
            Inventory_ChangeUpgrade(UPG_NUTS, 3); AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 3); AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
            Inventory_ChangeUpgrade(UPG_QUIVER, 3); AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
            Inventory_ChangeUpgrade(UPG_BULLET_BAG, 3); AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
            AMMO(ITEM_BOMBCHU) = 50;
            AMMO(ITEM_BEAN) = 15;
        }
    });
    ImGui::SameLine();
    ColoredButton(GRAY, []() {
        if (ImGui::Button("Reset##inventory")) {
            Inventory_ChangeUpgrade(UPG_STICKS, 0);
            Inventory_ChangeUpgrade(UPG_NUTS, 0);
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 0);
            Inventory_ChangeUpgrade(UPG_QUIVER, 0);
            Inventory_ChangeUpgrade(UPG_BULLET_BAG, 0);
            fill(gSaveContext.inventory.items, 0xFF);
            fill(gSaveContext.inventory.ammo, 0);
            u8 currentBEquip = gSaveContext.equips.buttonItems[0];
            fill(gSaveContext.equips.buttonItems, 0xFF);
            gSaveContext.equips.buttonItems[0] = currentBEquip;
            fill(gSaveContext.equips.cButtonSlots, 0xFF);
        }
    });
    ImGui::SameLine();
    ColoredCheckbox(GRAY, []() {
        ImGui::Checkbox("Safe Mode", &safeMode);
    });
    for (int32_t i = SLOT_STICK; i <= SLOT_TRADE_CHILD; i++) {
        InventorySlot slot = static_cast<InventorySlot>(i);
        ItemID currentItemID = static_cast<ItemID>(gSaveContext.inventory.items[slot]);

        DrawInventorySlot(slot);
        if (currentItemID != ITEM_NONE && ImGui::BeginPopupContextItem()) {
            DrawEquipItemMenu(slot);
            ImGui::EndPopup();
        }
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ColoredSlider(GRAY, []() {
        ImGui::PushItemWidth(30.0f * 3);
        ImGui::SliderScalar("##equipEditorModeSlider", ImGuiDataType_S8, &equipEditorMode, &S8_ZERO, &EQUIP_EDITOR_MAX, EQUIP_EDITOR_MODES[equipEditorMode]);
        ImGui::PopItemWidth();
    });
    ImGui::PushItemWidth(30.0f);
    SmallColoredInputText(YELLOW, []() {
        ImGui::Text("C-Pad Equips");
        ImGui::InputScalar("##cLeftEquipSlider", ImGuiDataType_U8, equipEditorMode ? &gSaveContext.equips.cButtonSlots[0] : &gSaveContext.equips.buttonItems[1]);
        ImGui::SameLine(30.0f * 2);
        ImGui::InputScalar("##cRightEquipSlider", ImGuiDataType_U8, equipEditorMode ? &gSaveContext.equips.cButtonSlots[2] : &gSaveContext.equips.buttonItems[3]);
        ImGui::Dummy(ImVec2(0.0f, 0.0f)); ImGui::SameLine(30.0f);
        ImGui::InputScalar("##cDownEquipSlider", ImGuiDataType_U8, equipEditorMode ? &gSaveContext.equips.cButtonSlots[1] : &gSaveContext.equips.buttonItems[2]);
    });
    SmallColoredInputText(GRAY, []() {
        ImGui::Text("D-Pad Equips");
        ImGui::Dummy(ImVec2(0.0f, 0.0f)); ImGui::SameLine(30.0f);
        ImGui::InputScalar("##dUpEquipSlider", ImGuiDataType_U8, equipEditorMode ? &gSaveContext.equips.cButtonSlots[3] : &gSaveContext.equips.buttonItems[4]);
        ImGui::InputScalar("##dLeftEquipSlider", ImGuiDataType_U8, equipEditorMode ? &gSaveContext.equips.cButtonSlots[5] : &gSaveContext.equips.buttonItems[6]);
        ImGui::SameLine(30.0f * 2);
        ImGui::InputScalar("##dRightEquipSlider", ImGuiDataType_U8, equipEditorMode ? &gSaveContext.equips.cButtonSlots[6] : &gSaveContext.equips.buttonItems[7]);
        ImGui::Dummy(ImVec2(0.0f, 0.0f)); ImGui::SameLine(30.0f);
        ImGui::InputScalar("##dDownEquipSlider", ImGuiDataType_U8, equipEditorMode ? &gSaveContext.equips.cButtonSlots[4] : &gSaveContext.equips.buttonItems[5]);
    });
    ImGui::PopItemWidth();
    ImGui::EndGroup();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

// Draw a flag bitfield as an grid of checkboxes
void DrawFlagTableArray16(const FlagTable& flagTable, uint16_t row, uint16_t& flags) {
    ImGui::PushID((std::to_string(row) + flagTable.name).c_str());
    for (int32_t flagIndex = 15; flagIndex >= 0; flagIndex--) {
        ImGui::SameLine();
        ImGui::PushID(flagIndex);
        bool hasDescription = !!flagTable.flagDescriptions.contains(row * 16 + flagIndex);
        uint32_t bitMask = 1 << flagIndex;
        ImGui::PushStyleColor(ImGuiCol_FrameBg, hasDescription ? ImVec4(0.16f, 0.29f, 0.48f, 0.54f) : ImVec4(0.16f, 0.29f, 0.48f, 0.24f));
        bool flag = (flags & bitMask) != 0;
        if (ImGui::Checkbox("##check", &flag)) {
            if (flag) {
                flags |= bitMask;
            } else {
                flags &= ~bitMask;
            }
        }
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() && hasDescription) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", UIWidgets::WrappedText(flagTable.flagDescriptions.at(row * 16 + flagIndex), 60));
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

            DrawGroupWithBorder([&]() {
                ImGui::Text("stateFlags1");
                UIWidgets::DrawFlagArray32("stateFlags1", player->stateFlags1);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("stateFlags2");
                UIWidgets::DrawFlagArray32("stateFlags2", player->stateFlags2);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("stateFlags3");
                UIWidgets::DrawFlagArray8("stateFlags3", player->stateFlags3);
            });
            
            ImGui::SameLine();
            
            DrawGroupWithBorder([&]() {
                ImGui::Text("unk_6AE");
                UIWidgets::DrawFlagArray16("unk_6AE", player->unk_6AE);
            });
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Current Scene")) {
        if (gPlayState != nullptr) {
            ActorContext* act = &gPlayState->actorCtx;

            DrawGroupWithBorder([&]() {
                ImGui::Text("Switch");
                UIWidgets::InsertHelpHoverText("Permanently-saved switch flags");
                ImGui::SameLine();
                if (ImGui::Button("Set All##Switch")) {
                    act->flags.swch = UINT32_MAX;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear All##Switch")) {
                    act->flags.swch = 0;
                }
                UIWidgets::DrawFlagArray32("Switch", act->flags.swch);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Switch");
                UIWidgets::InsertHelpHoverText("Temporary switch flags. Unset on scene transitions");
                ImGui::SameLine();
                if (ImGui::Button("Set All##Temp Switch")) {
                    act->flags.tempSwch = UINT32_MAX;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear All##Temp Switch")) {
                    act->flags.tempSwch = 0;
                }
                UIWidgets::DrawFlagArray32("Temp Switch", act->flags.tempSwch);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Clear");
                UIWidgets::InsertHelpHoverText("Permanently-saved room-clear flags");
                ImGui::SameLine();
                if (ImGui::Button("Set All##Clear")) {
                    act->flags.clear = UINT32_MAX;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear All##Clear")) {
                    act->flags.clear = 0;
                }
                UIWidgets::DrawFlagArray32("Clear", act->flags.clear);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Clear");
                UIWidgets::InsertHelpHoverText("Temporary room-clear flags. Unset on scene transitions");
                ImGui::SameLine();
                if (ImGui::Button("Set All##Temp Clear")) {
                    act->flags.tempClear = UINT32_MAX;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear All##Temp Clear")) {
                    act->flags.tempClear = 0;
                }
                UIWidgets::DrawFlagArray32("Temp Clear", act->flags.tempClear);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Collect");
                UIWidgets::InsertHelpHoverText("Permanently-saved collect flags");
                ImGui::SameLine();
                if (ImGui::Button("Set All##Collect")) {
                    act->flags.collect = UINT32_MAX;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear All##Collect")) {
                    act->flags.collect = 0;
                }
                UIWidgets::DrawFlagArray32("Collect", act->flags.collect);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Collect");
                UIWidgets::InsertHelpHoverText("Temporary collect flags. Unset on scene transitions");
                ImGui::SameLine();
                if (ImGui::Button("Set All##Temp Collect")) {
                    act->flags.tempCollect = UINT32_MAX;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear All##Temp Collect")) {
                    act->flags.tempCollect = 0;
                }
                UIWidgets::DrawFlagArray32("Temp Collect", act->flags.tempCollect);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Chest");
                UIWidgets::InsertHelpHoverText("Permanently-saved chest flags");
                ImGui::SameLine();
                if (ImGui::Button("Set All##Chest")) {
                    act->flags.chest = UINT32_MAX;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear All##Chest")) {
                    act->flags.chest = 0;
                }
                UIWidgets::DrawFlagArray32("Chest", act->flags.chest);
            });

            ImGui::SameLine();

            ImGui::BeginGroup();

            if (ImGui::Button("Reload Flags")) {
                act->flags.swch = gSaveContext.sceneFlags[gPlayState->sceneNum].swch;
                act->flags.clear = gSaveContext.sceneFlags[gPlayState->sceneNum].clear;
                act->flags.collect = gSaveContext.sceneFlags[gPlayState->sceneNum].collect;
                act->flags.chest = gSaveContext.sceneFlags[gPlayState->sceneNum].chest;
            }
            UIWidgets::SetLastItemHoverText("Load flags from saved scene flags. Normally happens on scene load");

            if (ImGui::Button("Save Flags")) {
                gSaveContext.sceneFlags[gPlayState->sceneNum].swch = act->flags.swch;
                gSaveContext.sceneFlags[gPlayState->sceneNum].clear = act->flags.clear;
                gSaveContext.sceneFlags[gPlayState->sceneNum].collect = act->flags.collect;
                gSaveContext.sceneFlags[gPlayState->sceneNum].chest = act->flags.chest;
            }
            UIWidgets::SetLastItemHoverText("Save current scene flags. Normally happens on scene exit");
            
            if (ImGui::Button("Clear Flags")) {
                act->flags.swch = 0;
                act->flags.clear = 0;
                act->flags.collect = 0;
                act->flags.chest = 0;
            }
            UIWidgets::SetLastItemHoverText("Clear current scene flags. Reload scene to see changes");


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
        if (gPlayState != nullptr) {
            ImGui::SameLine();
            if (ImGui::Button("Current")) {
                selectedSceneFlagMap = gPlayState->sceneNum;
            }
            UIWidgets::SetLastItemHoverText("Open flags for current scene");
        }

        DrawGroupWithBorder([&]() {
            ImGui::Text("Switch");
            UIWidgets::InsertHelpHoverText("Switch flags");
            UIWidgets::DrawFlagArray32("Switch", gSaveContext.sceneFlags[selectedSceneFlagMap].swch);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Clear");
            UIWidgets::InsertHelpHoverText("Room-clear flags");
            UIWidgets::DrawFlagArray32("Clear", gSaveContext.sceneFlags[selectedSceneFlagMap].clear);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Collect");
            UIWidgets::InsertHelpHoverText("Collect flags");
            UIWidgets::DrawFlagArray32("Collect", gSaveContext.sceneFlags[selectedSceneFlagMap].collect);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Chest");
            UIWidgets::InsertHelpHoverText("Chest flags");
            UIWidgets::DrawFlagArray32("Chest", gSaveContext.sceneFlags[selectedSceneFlagMap].chest);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Rooms");
            UIWidgets::InsertHelpHoverText("Flags for visted rooms");
            UIWidgets::DrawFlagArray32("Rooms", gSaveContext.sceneFlags[selectedSceneFlagMap].rooms);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Floors");
            UIWidgets::InsertHelpHoverText("Flags for visted floors");
            UIWidgets::DrawFlagArray32("Floors", gSaveContext.sceneFlags[selectedSceneFlagMap].floors);
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

        // If playing a Randomizer Save with Shuffle Skull Tokens on anything other than "Off" we don't want to keep
        // GS Token Count updated, since Gold Skulltulas killed will not correlate to GS Tokens Collected.
        if (!(gSaveContext.n64ddFlag && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_TOKENS) != RO_TOKENSANITY_OFF)) {
            static bool keepGsCountUpdated = true;
            ImGui::Checkbox("Keep GS Count Updated", &keepGsCountUpdated);
            UIWidgets::InsertHelpHoverText("Automatically adjust the number of gold skulltula tokens acquired based on set flags.");
            int32_t gsCount = 0;
            if (keepGsCountUpdated) {
                for (int32_t gsFlagIndex = 0; gsFlagIndex < 6; gsFlagIndex++) {
                    gsCount += std::popcount(static_cast<uint32_t>(gSaveContext.gsFlags[gsFlagIndex]));
                }
                gSaveContext.inventory.gsTokens = gsCount;
            }
        }
    });

    for (int i = 0; i < flagTables.size(); i++) {
        const FlagTable& flagTable = flagTables[i];
        if (flagTable.flagTableType == RANDOMIZER_INF && !gSaveContext.n64ddFlag && !gSaveContext.isBossRush) {
            continue;
        }

        if (ImGui::TreeNode(flagTable.name)) {
            for (int j = 0; j < flagTable.size + 1; j++) {
                DrawGroupWithBorder([&]() {
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
                            DrawFlagTableArray16(flagTable, j, gSaveContext.randomizerInf[j]);
                            break;
                    }
                });
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
    if (ImGui::BeginCombo("##upgrade", names[CUR_UPG_VALUE(categoryId)].c_str())) {
        for (int32_t i = 0; i < names.size(); i++) {
            if (ImGui::Selectable(names[i].c_str())) {
                Inventory_ChangeUpgrade(categoryId, i);
            }
        }

        ImGui::EndCombo();
    }
    ImGui::PopID();
    UIWidgets::SetLastItemHoverText(categoryName.c_str());
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
        if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
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
    UIWidgets::SetLastItemHoverText(categoryName.c_str());

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
                UIWidgets::SetLastItemHoverText("None");
            } else {
                const ItemMapEntry& slotEntry = itemMapping[items[pickerIndex]];
                if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
                                       ImVec2(1, 1), 0)) {
                    Inventory_ChangeUpgrade(categoryId, pickerIndex);
                    ImGui::CloseCurrentPopup();
                }
                UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(slotEntry.id));
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::PopID();
}

const std::vector<ItemID> equipmentValues = {
    ITEM_BULLET_BAG_30, ITEM_QUIVER_30,    ITEM_SWORD_KOKIRI, ITEM_SWORD_MASTER,  ITEM_SWORD_BGS,
    ITEM_STICK,         ITEM_BOMB_BAG_20,  ITEM_SHIELD_DEKU,  ITEM_SHIELD_HYLIAN, ITEM_SHIELD_MIRROR,
    ITEM_NUT,           ITEM_BRACELET,     ITEM_TUNIC_KOKIRI, ITEM_TUNIC_GORON,   ITEM_TUNIC_ZORA,
    ITEM_NONE,          ITEM_SCALE_SILVER, ITEM_BOOTS_KOKIRI, ITEM_BOOTS_IRON,    ITEM_BOOTS_HOVER,
};

void DrawEquipmentAmmoCapacity(int32_t slot, ItemID itemID) {
    float x = slot % 5;
    float y = floor(slot / 5);

    ImGui::SetCursorPos(ImVec2(x * INV_GRID_WIDTH + INV_GRID_PADDING + 7.0f, y * INV_GRID_HEIGHT + INV_GRID_TOP_MARGIN + INV_GRID_PADDING + (INV_GRID_ICON_SIZE - 2.0f)));
    ImGui::PushItemWidth(24.0f);
    SmallColoredInputText(GRAY, [&]() {
        if (ImGui::InputScalar("##ammoCapacity", ImGuiDataType_S8, &AMMO(itemID))) {
            // Crashes when < 0 and graphical issues when > 99
            AMMO(itemID) = MAX(0, MIN(AMMO(itemID), 99));
        }
    });
    ImGui::PopItemWidth();
}

void DrawEquipmentSlot(int32_t slot) {
    ItemID targetItemID = equipmentValues[slot];
    if (targetItemID == ITEM_NONE) return;
    float x = slot % 5;
    float y = floor(slot / 5);
    bool isOwned;
    ImTextureID textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[targetItemID].name);

    ImGui::PushID(slot);

    ImGui::SetCursorPos(ImVec2(x * INV_GRID_WIDTH + INV_GRID_PADDING, y * INV_GRID_HEIGHT + INV_GRID_TOP_MARGIN + INV_GRID_PADDING));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    if (targetItemID >= ITEM_SWORD_KOKIRI && targetItemID <= ITEM_BOOTS_HOVER) {
        bool isEquipped;
        if (targetItemID <= ITEM_SWORD_BGS) {
            isEquipped = CUR_EQUIP_VALUE(EQUIP_SWORD) == (targetItemID - ITEM_SWORD_KOKIRI + 1);
        } else if (targetItemID <= ITEM_SHIELD_MIRROR) {
            isEquipped = CUR_EQUIP_VALUE(EQUIP_SHIELD) == (targetItemID - ITEM_SHIELD_DEKU + 1);
        } else if (targetItemID <= ITEM_TUNIC_ZORA) {
            isEquipped = CUR_EQUIP_VALUE(EQUIP_TUNIC) == (targetItemID - ITEM_TUNIC_KOKIRI + 1);
        } else if (targetItemID <= ITEM_BOOTS_HOVER) {
            isEquipped = CUR_EQUIP_VALUE(EQUIP_BOOTS) == (targetItemID - ITEM_BOOTS_KOKIRI + 1);
        }
        if (isEquipped) {
            ImGui::PushStyleColor(ImGuiCol_Border, WHITE);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

    if (targetItemID >= ITEM_SWORD_KOKIRI && targetItemID <= ITEM_BOOTS_HOVER) {
        if (targetItemID <= ITEM_SWORD_BGS) {
            isOwned = CHECK_OWNED_EQUIP(EQUIP_SWORD, targetItemID - ITEM_SWORD_KOKIRI);
        } else if (targetItemID <= ITEM_SHIELD_MIRROR) {
            isOwned = CHECK_OWNED_EQUIP(EQUIP_SHIELD, targetItemID - ITEM_SHIELD_DEKU);
        } else if (targetItemID <= ITEM_TUNIC_ZORA) {
            isOwned = CHECK_OWNED_EQUIP(EQUIP_TUNIC, targetItemID - ITEM_TUNIC_KOKIRI);
        } else if (targetItemID <= ITEM_BOOTS_HOVER) {
            isOwned = CHECK_OWNED_EQUIP(EQUIP_BOOTS, targetItemID - ITEM_BOOTS_KOKIRI);
        }
    } else {
        switch (targetItemID) {
            case ITEM_BULLET_BAG_30:
                isOwned = CUR_UPG_VALUE(UPG_BULLET_BAG) > 0;
                switch (CUR_UPG_VALUE(UPG_BULLET_BAG)) {
                    case 2:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_BULLET_BAG_40].name);
                        break;
                    case 3:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_BULLET_BAG_50].name);
                        break;
                }
                break;
            case ITEM_STICK:
                isOwned = CUR_UPG_VALUE(UPG_STICKS) > 0;
                break;
            case ITEM_NUT:
                isOwned = CUR_UPG_VALUE(UPG_NUTS) > 0;
                break;
            case ITEM_QUIVER_30:
                isOwned = CUR_UPG_VALUE(UPG_QUIVER) > 0;
                switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                    case 2:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_QUIVER_40].name);
                        break;
                    case 3:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_QUIVER_50].name);
                        break;
                }
                break;
            case ITEM_BOMB_BAG_20:
                isOwned = CUR_UPG_VALUE(UPG_BOMB_BAG) > 0;
                switch (CUR_UPG_VALUE(UPG_BOMB_BAG)) {
                    case 2:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_BOMB_BAG_30].name);
                        break;
                    case 3:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_BOMB_BAG_40].name);
                        break;
                }
                break;
            case ITEM_BRACELET:
                isOwned = CUR_UPG_VALUE(UPG_STRENGTH) > 0;
                switch (CUR_UPG_VALUE(UPG_STRENGTH)) {
                    case 2:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_GAUNTLETS_SILVER].name);
                        break;
                    case 3:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_GAUNTLETS_GOLD].name);
                        break;
                }
                break;
            case ITEM_SCALE_SILVER:
                isOwned = CUR_UPG_VALUE(UPG_SCALE) > 0;
                switch (CUR_UPG_VALUE(UPG_SCALE)) {
                    case 2:
                        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_SCALE_GOLDEN].name);
                        break;
                }
                break;
        }
    }

    if (!isOwned) {
        textureId = LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[targetItemID].nameFaded);
    }

    if (ImGui::ImageButton(textureId, ImVec2(INV_GRID_ICON_SIZE, INV_GRID_ICON_SIZE), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        switch (targetItemID) {
            case ITEM_BULLET_BAG_30:
                NextAmmoItemUpgradeInSlot(SLOT_SLINGSHOT, UPG_BULLET_BAG);
                break;
            case ITEM_STICK:
                NextAmmoItemUpgradeInSlot(SLOT_STICK, UPG_STICKS);
                break;
            case ITEM_NUT:
                NextAmmoItemUpgradeInSlot(SLOT_NUT, UPG_NUTS);
                break;
            case ITEM_QUIVER_30:
                NextAmmoItemUpgradeInSlot(SLOT_BOW, UPG_QUIVER);
                break;
            case ITEM_BOMB_BAG_20:
                NextAmmoItemUpgradeInSlot(SLOT_BOMB, UPG_BOMB_BAG);
                break;
            case ITEM_BRACELET:
                NextItemUpgrade(UPG_STRENGTH);
                break;
            case ITEM_SCALE_SILVER:
                NextItemUpgrade(UPG_SCALE);
                break;
            case ITEM_SWORD_KOKIRI:
            case ITEM_SWORD_MASTER:
                if (isOwned) gSaveContext.inventory.equipment &= ~(gBitFlags[targetItemID - ITEM_SWORD_KOKIRI] << gEquipShifts[EQUIP_SWORD]);
                else gSaveContext.inventory.equipment |= (gBitFlags[targetItemID - ITEM_SWORD_KOKIRI] << gEquipShifts[EQUIP_SWORD]);
                break;
            case ITEM_SHIELD_DEKU:
            case ITEM_SHIELD_HYLIAN:
            case ITEM_SHIELD_MIRROR:
                if (isOwned) gSaveContext.inventory.equipment &= ~(gBitFlags[targetItemID - ITEM_SHIELD_DEKU] << gEquipShifts[EQUIP_SHIELD]);
                else gSaveContext.inventory.equipment |= (gBitFlags[targetItemID - ITEM_SHIELD_DEKU] << gEquipShifts[EQUIP_SHIELD]);
                break;
            case ITEM_TUNIC_KOKIRI:
            case ITEM_TUNIC_GORON:
            case ITEM_TUNIC_ZORA:
                if (isOwned) gSaveContext.inventory.equipment &= ~(gBitFlags[targetItemID - ITEM_TUNIC_KOKIRI] << gEquipShifts[EQUIP_TUNIC]);
                else gSaveContext.inventory.equipment |= (gBitFlags[targetItemID - ITEM_TUNIC_KOKIRI] << gEquipShifts[EQUIP_TUNIC]);
                break;
            case ITEM_BOOTS_KOKIRI:
            case ITEM_BOOTS_IRON:
            case ITEM_BOOTS_HOVER:
                if (isOwned) gSaveContext.inventory.equipment &= ~(gBitFlags[targetItemID - ITEM_BOOTS_KOKIRI] << gEquipShifts[EQUIP_BOOTS]);
                else gSaveContext.inventory.equipment |= (gBitFlags[targetItemID - ITEM_BOOTS_KOKIRI] << gEquipShifts[EQUIP_BOOTS]);
                break;
        }
    }

    if (isOwned) {
        switch (targetItemID) {
            case ITEM_STICK:
                DrawEquipmentAmmoCapacity(slot, ITEM_STICK);
                break;
            case ITEM_NUT:
                DrawEquipmentAmmoCapacity(slot, ITEM_NUT);
                break;
            case ITEM_BOMB_BAG_20:
                DrawEquipmentAmmoCapacity(slot, ITEM_BOMB);
                break;
            case ITEM_BULLET_BAG_30:
                DrawEquipmentAmmoCapacity(slot, ITEM_SLINGSHOT);
                break;
            case ITEM_QUIVER_30:
                DrawEquipmentAmmoCapacity(slot, ITEM_BOW);
                break;
        }
    }

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(4);
    ImGui::PopID();
}

void DrawEquipmentTab() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    ImGui::BeginChild("equipmentTab", ImVec2(0, 0), true);
    ImGui::BeginGroup();
    ColoredButton(GRAY, []() {
        if (ImGui::Button("Give All##equipment")) {
            gSaveContext.inventory.items[SLOT_STICK] = ITEM_STICK;
            Inventory_ChangeUpgrade(UPG_STICKS, 3); AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
            gSaveContext.inventory.items[SLOT_NUT] = ITEM_NUT;
            Inventory_ChangeUpgrade(UPG_NUTS, 3); AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
            gSaveContext.inventory.items[SLOT_BOMB] = ITEM_BOMB;
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 3); AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
            gSaveContext.inventory.items[SLOT_BOW] = ITEM_BOW;
            Inventory_ChangeUpgrade(UPG_QUIVER, 3); AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
            gSaveContext.inventory.items[SLOT_SLINGSHOT] = ITEM_SLINGSHOT;
            Inventory_ChangeUpgrade(UPG_BULLET_BAG, 3); AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
            Inventory_ChangeUpgrade(UPG_SCALE, 2);
            Inventory_ChangeUpgrade(UPG_STRENGTH, 3);
            gSaveContext.equips.equipment = LINK_IS_ADULT ? 0x1122 : 0x1121;
            gSaveContext.equips.buttonItems[0] = LINK_IS_ADULT ? ITEM_SWORD_MASTER : ITEM_SWORD_KOKIRI;
            gSaveContext.inventory.equipment = 0x7777;
            gSaveContext.infTable[29] = 0;
            if (gPlayState != nullptr) {
                Player* player = GET_PLAYER(gPlayState);
                player->currentShield = PLAYER_SHIELD_HYLIAN;
                player->currentSwordItemId = LINK_IS_ADULT ? ITEM_SWORD_MASTER : ITEM_SWORD_KOKIRI;
            }
        }
    });
    ImGui::SameLine();
    ColoredButton(GRAY, []() {
        if (ImGui::Button("Reset##equipment")) {
            gSaveContext.equips.equipment = LINK_IS_ADULT ? 0x1102 : 0x1100;
            gSaveContext.inventory.equipment = LINK_IS_ADULT ? 0x1102 : 0x1100;
            gSaveContext.equips.buttonItems[0] = LINK_IS_ADULT ? ITEM_SWORD_MASTER : ITEM_NONE;
            gSaveContext.inventory.upgrades = 0x0;
            gSaveContext.inventory.items[SLOT_STICK] = ITEM_NONE;
            gSaveContext.inventory.items[SLOT_NUT] = ITEM_NONE;
            gSaveContext.inventory.items[SLOT_BOMB] = ITEM_NONE;
            gSaveContext.inventory.items[SLOT_BOW] = ITEM_NONE;
            gSaveContext.inventory.items[SLOT_SLINGSHOT] = ITEM_NONE;
            if (gPlayState != nullptr) {
                Player* player = GET_PLAYER(gPlayState);
                player->currentTunic = PLAYER_TUNIC_KOKIRI;
                player->currentBoots = PLAYER_BOOTS_KOKIRI;
                Player_SetBootData(gPlayState, player);
                player->currentShield = PLAYER_SHIELD_NONE;
                if (LINK_IS_CHILD) {
                    gSaveContext.infTable[29] = 1;
                    player->currentSwordItemId = ITEM_NONE;
                } else {
                    player->currentSwordItemId = ITEM_SWORD_MASTER;
                }
            }
        }
    });
    ImGui::SameLine();
    ColoredCheckbox(GRAY, []() {
        ImGui::Checkbox("Safe Mode", &safeMode);
    });
    ImGui::EndGroup();
    ImGui::BeginGroup();
    for (int32_t i = 0; i < equipmentValues.size(); i++) {
        DrawEquipmentSlot(i);
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    if (gPlayState != nullptr) {
        ColoredSlider(GRAY, []() {
            ImGui::PushItemWidth(175.0f);
            ImGui::Text("Equipped Sword");
            if (ImGui::BeginCombo("##sword", swordValues[CUR_EQUIP_VALUE(EQUIP_SWORD)])) {
                Player* player = GET_PLAYER(gPlayState);
                for (int i = 0; i < IM_ARRAYSIZE(swordValues); i++) {
                    bool isSelected = (CUR_EQUIP_VALUE(EQUIP_SWORD)) == i;
                    if (ImGui::Selectable(swordValues[i], isSelected)) {
                        if (i == 0) {
                            gSaveContext.infTable[29] = 1;
                            gSaveContext.equips.buttonItems[0] = player->currentSwordItemId = ITEM_NONE;
                        } else {
                            gSaveContext.infTable[29] = 0;
                            gSaveContext.equips.buttonItems[0] = player->currentSwordItemId = i + ITEM_SWORD_KOKIRI - 1;
                        }
                        Inventory_ChangeEquipment(EQUIP_SWORD, i);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Equipped Shield");
            if (ImGui::BeginCombo("##shield", shieldValues[CUR_EQUIP_VALUE(EQUIP_SHIELD)])) {
                Player* player = GET_PLAYER(gPlayState);
                for (int i = 0; i < (safeMode ? 4 : IM_ARRAYSIZE(shieldValues)); i++) {
                    bool isSelected = (CUR_EQUIP_VALUE(EQUIP_SHIELD)) == i;
                    if (ImGui::Selectable(shieldValues[i], isSelected)) {
                        player->currentShield = i;
                        Inventory_ChangeEquipment(EQUIP_SHIELD, i);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Equipped Tunic");
            if (ImGui::BeginCombo("##tunic", tunicValues[CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1])) {
                Player* player = GET_PLAYER(gPlayState);
                for (int i = 0; i < (safeMode ? 3 : IM_ARRAYSIZE(tunicValues)); i++) {
                    bool isSelected = (CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1) == i;
                    if (ImGui::Selectable(tunicValues[i], isSelected)) {
                        player->currentTunic = i;
                        Inventory_ChangeEquipment(EQUIP_TUNIC, i + 1);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Equipped Boots");
            if (ImGui::BeginCombo("##boots", bootsValues[CUR_EQUIP_VALUE(EQUIP_BOOTS) - 1])) {
                Player* player = GET_PLAYER(gPlayState);
                for (int i = 0; i < (safeMode ? 3 : IM_ARRAYSIZE(bootsValues)); i++) {
                    bool isSelected = (CUR_EQUIP_VALUE(EQUIP_BOOTS) - 1) == i;
                    if (ImGui::Selectable(bootsValues[i], isSelected)) {
                        player->currentBoots = i;
                        Inventory_ChangeEquipment(EQUIP_BOOTS, i + 1);
                        Player_SetBootData(gPlayState, player);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
        });
    }
    ImGui::EndGroup();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

// Draws a toggleable icon for a quest item that is faded when disabled
void DrawQuestItemButton(uint32_t item) {
    const QuestMapEntry& entry = questMapping[item];
    uint32_t bitMask = 1 << entry.id;
    bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                           ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (hasQuestItem) {
            gSaveContext.inventory.questItems &= ~bitMask;
        } else {
            gSaveContext.inventory.questItems |= bitMask;
        }
    }
    ImGui::PopStyleColor();
    UIWidgets::SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
}

// Draws a toggleable icon for a dungeon item that is faded when disabled
void DrawDungeonItemButton(uint32_t item, uint32_t scene) {
    const ItemMapEntry& entry = itemMapping[item];
    uint32_t bitMask = 1 << (entry.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[scene]) != 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasItem ? entry.name : entry.nameFaded),
                           ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (hasItem) {
            gSaveContext.inventory.dungeonItems[scene] &= ~bitMask;
        } else {
            gSaveContext.inventory.dungeonItems[scene] |= bitMask;
        }
    }
    ImGui::PopStyleColor();
    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(entry.id));
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
        if (ImGui::ImageButton(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                               ImVec2(16.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
            if (hasQuestItem) {
                gSaveContext.inventory.questItems &= ~bitMask;
            } else {
                gSaveContext.inventory.questItems |= bitMask;
            }
        }
        ImGui::PopStyleColor();
        UIWidgets::SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
    }

    ImGui::InputScalar("GS Count", ImGuiDataType_S16, &gSaveContext.inventory.gsTokens);
    UIWidgets::InsertHelpHoverText("Number of gold skulltula tokens aquired");

    uint32_t bitMask = 1 << QUEST_SKULL_TOKEN;
    bool gsUnlocked = (bitMask & gSaveContext.inventory.questItems) != 0;
    if (ImGui::Checkbox("GS unlocked", &gsUnlocked)) {
        if (gsUnlocked) {
            gSaveContext.inventory.questItems |= bitMask;
        } else {
            gSaveContext.inventory.questItems &= ~bitMask;
        }
    }
    UIWidgets::InsertHelpHoverText("If unlocked, enables showing the gold skulltula count in the quest status menu");

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
    UIWidgets::InsertHelpHoverText("The number of pieces of heart acquired towards the next heart container");

    DrawGroupWithBorder([&]() {
        ImGui::Text("Dungeon Items");

        static int32_t dungeonItemsScene = SCENE_DEKU_TREE;
        ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
        if (ImGui::BeginCombo("##DungeonSelect", SohUtils::GetSceneName(dungeonItemsScene).c_str())) {
            for (int32_t dungeonIndex = SCENE_DEKU_TREE; dungeonIndex < SCENE_JABU_JABU_BOSS + 1; dungeonIndex++) {
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

        if (dungeonItemsScene != SCENE_JABU_JABU_BOSS) {
            float lineHeight = ImGui::GetTextLineHeightWithSpacing();
            ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemMapping[ITEM_KEY_SMALL].name), ImVec2(lineHeight, lineHeight));
            ImGui::SameLine();
            if (ImGui::InputScalar("##Keys", ImGuiDataType_S8, gSaveContext.inventory.dungeonKeys + dungeonItemsScene)) {
                gSaveContext.sohStats.dungeonKeys[dungeonItemsScene] = gSaveContext.inventory.dungeonKeys[dungeonItemsScene];
            };
        } else {
            // dungeonItems is size 20 but dungeonKeys is size 19, so there are no keys for the last scene (Barinade's Lair)
            ImGui::Text("Barinade's Lair does not have small keys");
        }
    });

    ImGui::PopItemWidth();
}

void DrawPlayerTab() {
    if (gPlayState != nullptr) {
        Player* player = GET_PLAYER(gPlayState);

        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        ColoredSlider(GRAY, [&]() {
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.33f - 4.0f);
            ImGui::DragScalar("##xpos", ImGuiDataType_Float, &player->actor.world.pos.x, 1.0f, &POS_MIN, &POS_MAX, "X Position: %.0f");
            ImGui::SameLine();
            ImGui::DragScalar("##ypos", ImGuiDataType_Float, &player->actor.world.pos.y, 1.0f, &POS_MIN, &POS_MAX, "Y Position: %.0f");
            ImGui::SameLine();
            ImGui::DragScalar("##zpos", ImGuiDataType_Float, &player->actor.world.pos.z, 1.0f, &POS_MIN, &POS_MAX, "Z Position: %.0f");
            ImGui::SliderScalar("##xmodrot", ImGuiDataType_S16, &player->actor.shape.rot.x, &S16_MIN, &S16_MAX, "X Rotation: %d");
            ImGui::SameLine();
            ImGui::SliderScalar("##ymodrot", ImGuiDataType_S16, &player->actor.shape.rot.y, &S16_MIN, &S16_MAX, "Y Rotation: %d");
            ImGui::SameLine();
            ImGui::SliderScalar("##zmodrot", ImGuiDataType_S16, &player->actor.shape.rot.z, &S16_MIN, &S16_MAX, "Z Rotation: %d");
            ImGui::PopItemWidth();
        });

        ImGui::InputScalar("Linear Velocity", ImGuiDataType_Float, &player->linearVelocity);
        UIWidgets::InsertHelpHoverText("Link's speed along the XZ plane");

        ImGui::InputScalar("Y Velocity", ImGuiDataType_Float, &player->actor.velocity.y);
        UIWidgets::InsertHelpHoverText("Link's speed along the Y plane. Caps at -20");
        ImGui::InputScalar("Wall Height", ImGuiDataType_Float, &player->wallHeight);
        UIWidgets::InsertHelpHoverText("Height used to determine whether Link can climb or grab a ledge at the top");

        ImGui::InputScalar("Invincibility Timer", ImGuiDataType_S8, &player->invincibilityTimer);
        UIWidgets::InsertHelpHoverText("Can't take damage while this is nonzero");

        ImGui::InputScalar("Gravity", ImGuiDataType_Float, &player->actor.gravity);
        UIWidgets::InsertHelpHoverText("Rate at which Link falls. Default -4.0f");

        if (ImGui::BeginCombo("Link Age on Load", gPlayState->linkAgeOnLoad == 0 ? "Adult" : "Child")) {
            if (ImGui::Selectable("Adult")) {
                gPlayState->linkAgeOnLoad = 0;
            }
            if (ImGui::Selectable("Child")) {
                gPlayState->linkAgeOnLoad = 1;
            }
            ImGui::EndCombo();
        }

        UIWidgets::InsertHelpHoverText("This will change Link's age when you load a map");

        ImGui::Separator();
        
        ImGui::Text("Player State");
        uint8_t bit[32] = {};
        uint32_t flags[3] = { player->stateFlags1, player->stateFlags2, player->stateFlags3 };
        std::vector<std::vector<std::string>> flag_strs = { state1, state2, state3 };

        for (int j = 0; j <= 2; j++) {
            DrawGroupWithBorder([&]() {
                ImGui::Text("State Flags %d", j + 1);
                std::vector<std::string> state = flag_strs[j];
                for (int i = 0; i <= 31; i++) {
                    bit[i] = ((flags[j] >> i) & 1);
                    if (bit[i] != 0) {
                        ImGui::Text("%s", state[i].c_str());
                    }
                }
            });
            ImGui::SameLine();
        }
        DrawGroupWithBorder([&]() {
            ImGui::Text("Sword");
            ImGui::Text("  %d", player->swordState);
        });

    } else {
        ImGui::Text("Global Context needed for player info!");
    }
}

void SaveEditorWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(470, 320), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Save Editor", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    ImGui::ShowDemoWindow();

    if (ImGui::BeginTabBar("SaveContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("General")) {
            DrawGeneralTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Inventory")) {
            DrawInventoryTab();
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

        if (ImGui::BeginTabItem("Flags")) {
            DrawFlagsTab();
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

void SaveEditorWindow::InitElement() {
    initLegalItemsForInventorySlot();
    // Load item icons into ImGui
    for (const auto& entry : itemMapping) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.name, entry.second.texturePath, ImVec4(1, 1, 1, 1));
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }
    for (const auto& entry : gregMapping) {
        ImVec4 gregGreen = ImVec4(42.0f / 255.0f, 169.0f / 255.0f, 40.0f / 255.0f, 1.0f);
        ImVec4 gregFadedGreen = gregGreen;
        gregFadedGreen.w = 0.3f;
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.name, entry.second.texturePath, gregGreen);
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.nameFaded, entry.second.texturePath, gregFadedGreen);
    }
    for (const auto& entry : questMapping) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.name, entry.second.texturePath, ImVec4(1, 1, 1, 1));
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }
    for (const auto& entry : songMapping) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.nameFaded, gSongNoteTex, fadedCol);
    }
    for (const auto& entry : vanillaSongMapping) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.nameFaded, gSongNoteTex, fadedCol);
    }
}

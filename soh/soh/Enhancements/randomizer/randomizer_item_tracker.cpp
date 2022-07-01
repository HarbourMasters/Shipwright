#include "randomizer_item_tracker.h"
#include "../../util.h"
#include "../libultraship/ImGuiImpl.h"
#include <soh/Enhancements/debugger/ImGuiHelpers.h>

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

#define ITEM_MAP_ENTRY(id)                                            \
    {                                                                 \
        id, {                                                         \
            id, #id, #id "_Faded", static_cast<char*>(gItemIcons[id]) \
        }                                                             \
    }

// Maps items ids to info for use in ImGui
std::map<uint32_t, ItemMapEntry> itemMappingSSS = {
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

std::vector<std::string> gsMappingSSS = {
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
u8 gAllAmmoItemsSSS[] = {
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

#define QUEST_MAP_ENTRY(id, tex)       \
    {                                  \
        id, {                          \
            id, #id, #id "_Faded", tex \
        }                              \
    }

// Maps quest items ids to info for use in ImGui
std::map<uint32_t, QuestMapEntry> questMappingSSS = {
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

#define SONG_MAP_ENTRY(id, r, g, b) \
    { id, #id, #id "_Faded", ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f) }

// Maps song ids to info for use in ImGui
std::array<SongMapEntry, 12> songMappingSSS = { {
    SONG_MAP_ENTRY(QUEST_SONG_LULLABY, 255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_EPONA, 255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_SARIA, 255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_SUN, 255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_TIME, 255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_STORMS, 255, 255, 255),
    SONG_MAP_ENTRY(QUEST_SONG_MINUET, 150, 255, 100),
    SONG_MAP_ENTRY(QUEST_SONG_BOLERO, 255, 80, 40),
    SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    SONG_MAP_ENTRY(QUEST_SONG_REQUIEM, 255, 160, 0),
    SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    SONG_MAP_ENTRY(QUEST_SONG_PRELUDE, 255, 240, 100),
} };

// Encapsulates what is drawn by the passed-in function within a border
template <typename T> void DrawGroupWithBorder(T&& drawFunc) {
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
    ImGui::GetWindowDrawList()->AddRect(
        p0, p1, IM_COL32(borderCol.x * 255, borderCol.y * 255, borderCol.z * 255, borderCol.w * 255));

    ImGui::EndGroup();
}

char z2ASCIISSS(int code) {
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

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    uint32_t bitMask;
} ItemTrackerMapEntry;

#define ITEM_TRACKER_MAP_ENTRY(id, maskShift)     \
    {                                             \
        id, {                                     \
            id, #id, #id "_Faded", 1 << maskShift \
        }                                         \
    }

std::unordered_map<uint32_t, ItemTrackerMapEntry> equipTrackerMap = { 
    ITEM_TRACKER_MAP_ENTRY(ITEM_SWORD_KOKIRI, 0), 
    ITEM_TRACKER_MAP_ENTRY(ITEM_SWORD_MASTER, 1),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SWORD_BGS, 2),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SWORD_BROKEN, 3),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SHIELD_DEKU, 4),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SHIELD_HYLIAN, 5),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SHIELD_MIRROR, 6),
    ITEM_TRACKER_MAP_ENTRY(ITEM_TUNIC_KOKIRI, 8),
    ITEM_TRACKER_MAP_ENTRY(ITEM_TUNIC_GORON, 9),
    ITEM_TRACKER_MAP_ENTRY(ITEM_TUNIC_ZORA, 10),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOOTS_KOKIRI, 12),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOOTS_IRON, 13),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOOTS_HOVER, 14),
};

void DrawEquip(uint32_t itemId) {
    const ItemTrackerMapEntry& entry = equipTrackerMap[itemId];
    bool hasEquip = (entry.bitMask & gSaveContext.inventory.equipment) != 0;

    ImGui::Image(SohImGui::GetTextureByName(hasEquip ? entry.name : entry.nameFaded), ImVec2(32.0f, 32.0f),
                 ImVec2(0, 0), ImVec2(1, 1));

    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
}


std::unordered_map<uint32_t, ItemTrackerMapEntry> questTrackerMap = {
    ITEM_TRACKER_MAP_ENTRY(QUEST_MEDALLION_FOREST, 0),
    ITEM_TRACKER_MAP_ENTRY(QUEST_MEDALLION_FIRE, 1),
    ITEM_TRACKER_MAP_ENTRY(QUEST_MEDALLION_WATER, 2),
    ITEM_TRACKER_MAP_ENTRY(QUEST_MEDALLION_SPIRIT, 3),
    ITEM_TRACKER_MAP_ENTRY(QUEST_MEDALLION_SHADOW, 4),
    ITEM_TRACKER_MAP_ENTRY(QUEST_MEDALLION_LIGHT, 5),
    ITEM_TRACKER_MAP_ENTRY(QUEST_KOKIRI_EMERALD, 18),
    ITEM_TRACKER_MAP_ENTRY(QUEST_GORON_RUBY, 19),
    ITEM_TRACKER_MAP_ENTRY(QUEST_ZORA_SAPPHIRE, 20),
    ITEM_TRACKER_MAP_ENTRY(QUEST_STONE_OF_AGONY, 21),
    ITEM_TRACKER_MAP_ENTRY(QUEST_GERUDO_CARD, 22),

};

void DrawQuest(uint32_t itemId) {
    const ItemTrackerMapEntry& entry = questTrackerMap[itemId];
    bool hasEquip = (entry.bitMask & gSaveContext.inventory.questItems) != 0;

    ImGui::Image(SohImGui::GetTextureByName(hasEquip ? entry.name : entry.nameFaded), ImVec2(32.0f, 32.0f),
                 ImVec2(0, 0), ImVec2(1, 1));

    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
};

std::unordered_map<uint32_t, ItemTrackerMapEntry> itemTrackerMap = {
    ITEM_TRACKER_MAP_ENTRY(ITEM_STICK, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_NUT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOMB, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOW, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_ARROW_FIRE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_DINS_FIRE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SLINGSHOT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_OCARINA_FAIRY, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_OCARINA_TIME, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOMBCHU, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_HOOKSHOT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_LONGSHOT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_ARROW_ICE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_FARORES_WIND, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOOMERANG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_LENS, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BEAN, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_HAMMER, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_ARROW_LIGHT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_NAYRUS_LOVE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOTTLE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POTION_RED, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POTION_GREEN, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POTION_BLUE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_FAIRY, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_FISH, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MILK_BOTTLE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_LETTER_RUTO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BLUE_FIRE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BUG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BIG_POE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MILK_HALF, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_WEIRD_EGG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_CHICKEN, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_LETTER_ZELDA, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_KEATON, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_SKULL, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_SPOOKY, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_BUNNY, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_GORON, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_ZORA, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_GERUDO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_TRUTH, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SOLD_OUT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POCKET_EGG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POCKET_CUCCO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_COJIRO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_ODD_MUSHROOM, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_ODD_POTION, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SAW, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SWORD_BROKEN, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_PRESCRIPTION, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_FROG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_EYEDROPS, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_CLAIM_CHECK, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOW_ARROW_FIRE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOW_ARROW_ICE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOW_ARROW_LIGHT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BOTTLE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POTION_RED, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POTION_GREEN, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POTION_BLUE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_FAIRY, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_FISH, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MILK_BOTTLE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_LETTER_RUTO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BLUE_FIRE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BUG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_BIG_POE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MILK_HALF, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_WEIRD_EGG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_CHICKEN, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_LETTER_ZELDA, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_KEATON, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_SKULL, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_SPOOKY, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_BUNNY, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_GORON, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_ZORA, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_GERUDO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MASK_TRUTH, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SOLD_OUT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POCKET_EGG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_POCKET_CUCCO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_COJIRO, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_ODD_MUSHROOM, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_ODD_POTION, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SAW, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_SWORD_BROKEN, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_PRESCRIPTION, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_FROG, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_EYEDROPS, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_CLAIM_CHECK, 0),

};

void DrawItem(uint32_t itemId) {
    uint32_t actualItemId = INV_CONTENT(itemId);
    bool hasEquip = actualItemId != ITEM_NONE;
    const ItemTrackerMapEntry& entry = itemTrackerMap[hasEquip ? actualItemId : itemId];

    ImGui::Image(SohImGui::GetTextureByName(hasEquip ? entry.name : entry.nameFaded), ImVec2(32.0f, 32.0f),
                 ImVec2(0, 0), ImVec2(1, 1));

    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
};

typedef struct {
    uint8_t id;
    std::string name;
    std::string nameFaded;
} ItemTrackerUpgradeEntry;

#define ITEM_TRACKER_UPGRADE_ENTRY(id)  { id, #id, #id "_Faded" }

std::unordered_map<int32_t, std::vector<ItemTrackerUpgradeEntry>> upgradeTrackerMap = {
    {UPG_STRENGTH, {
        ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BRACELET),
        ITEM_TRACKER_UPGRADE_ENTRY(ITEM_GAUNTLETS_SILVER),
        ITEM_TRACKER_UPGRADE_ENTRY(ITEM_GAUNTLETS_GOLD),
    }},
    {UPG_SCALE, {
        ITEM_TRACKER_UPGRADE_ENTRY(ITEM_SCALE_SILVER),
        ITEM_TRACKER_UPGRADE_ENTRY(ITEM_SCALE_GOLDEN),
    }},
    {UPG_QUIVER, {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_QUIVER_30),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_QUIVER_40),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_QUIVER_50),
      }},
    {UPG_BULLET_BAG, {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BULLET_BAG_30),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BULLET_BAG_40),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BULLET_BAG_50),
      }},
    {UPG_BOMB_BAG, {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BOMB_BAG_20),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BOMB_BAG_30),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BOMB_BAG_40),
      }},

};

void DrawUpgrade(int32_t categoryId) {
    if (CUR_UPG_VALUE(categoryId) == 0) {
        const ItemTrackerUpgradeEntry& entry = upgradeTrackerMap[categoryId][0];
        ImGui::Image(SohImGui::GetTextureByName(entry.nameFaded),
                     ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1));
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
    } else {
        const ItemTrackerUpgradeEntry& entry = upgradeTrackerMap[categoryId][CUR_UPG_VALUE(categoryId) - 1];
        ImGui::Image(SohImGui::GetTextureByName(entry.name),
                     ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1));
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
    }
}

void DrawItemTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gItemTrackerEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Item Tracker", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }
    
    DrawItem(ITEM_STICK);
    ImGui::SameLine();
    DrawItem(ITEM_NUT);
    ImGui::SameLine();
    DrawItem(ITEM_BOMB);
    ImGui::SameLine();
    DrawItem(ITEM_BOW);
    ImGui::SameLine();
    DrawItem(ITEM_ARROW_FIRE);
    ImGui::SameLine();
    DrawItem(ITEM_DINS_FIRE);
    ImGui::NewLine();
    DrawItem(ITEM_SLINGSHOT);
    ImGui::SameLine();
    DrawItem(ITEM_OCARINA_FAIRY);
    ImGui::SameLine();
    DrawItem(ITEM_BOMBCHU);
    ImGui::SameLine();
    DrawItem(ITEM_HOOKSHOT);
    ImGui::SameLine();
    DrawItem(ITEM_ARROW_ICE);
    ImGui::SameLine();
    DrawItem(ITEM_FARORES_WIND);
    ImGui::NewLine();
    DrawItem(ITEM_BOOMERANG);
    ImGui::SameLine();
    DrawItem(ITEM_LENS);
    ImGui::SameLine();
    DrawItem(ITEM_BEAN);
    ImGui::SameLine();
    DrawItem(ITEM_HAMMER);
    ImGui::SameLine();
    DrawItem(ITEM_ARROW_LIGHT);
    ImGui::SameLine();
    DrawItem(ITEM_NAYRUS_LOVE);
    ImGui::NewLine();
    /* DrawItem();
    DrawItem();
    DrawItem();
    DrawItem();
    DrawItem();
    DrawItem();*/
    ImGui::NewLine();
    DrawEquip(ITEM_SWORD_KOKIRI);
    ImGui::SameLine();
    DrawEquip(ITEM_SWORD_MASTER);
    ImGui::SameLine();
    DrawEquip(ITEM_SWORD_BGS); // PURPLE TODO: CHECK IF BGS OR BROKEN SWORD TO DISPLAY 
    ImGui::SameLine();
    DrawQuest(QUEST_STONE_OF_AGONY);
    ImGui::SameLine();
    DrawQuest(QUEST_GERUDO_CARD);
    ImGui::NewLine();
    DrawEquip(ITEM_SHIELD_DEKU);
    ImGui::SameLine();
    DrawEquip(ITEM_SHIELD_HYLIAN);
    ImGui::SameLine();
    DrawEquip(ITEM_SHIELD_MIRROR);
    ImGui::SameLine();
    DrawUpgrade(UPG_STRENGTH);
    ImGui::SameLine();
    DrawUpgrade(UPG_SCALE);
    ImGui::NewLine();
    DrawEquip(ITEM_TUNIC_KOKIRI);
    ImGui::SameLine();
    DrawEquip(ITEM_TUNIC_GORON);
    ImGui::SameLine();
    DrawEquip(ITEM_TUNIC_ZORA);
    ImGui::NewLine();
    DrawEquip(ITEM_BOOTS_KOKIRI);
    ImGui::SameLine();
    DrawEquip(ITEM_BOOTS_IRON);
    ImGui::SameLine();
    DrawEquip(ITEM_BOOTS_HOVER);
    ImGui::SameLine();
    DrawQuest(QUEST_KOKIRI_EMERALD);
    ImGui::SameLine();
    DrawQuest(QUEST_GORON_RUBY);
    ImGui::SameLine();
    DrawQuest(QUEST_ZORA_SAPPHIRE);
    ImGui::NewLine();
    DrawQuest(QUEST_MEDALLION_FOREST);
    ImGui::SameLine();
    DrawQuest(QUEST_MEDALLION_FIRE);
    ImGui::SameLine();
    DrawQuest(QUEST_MEDALLION_WATER);
    ImGui::SameLine();
    DrawQuest(QUEST_MEDALLION_SPIRIT);
    ImGui::SameLine();
    DrawQuest(QUEST_MEDALLION_SHADOW);
    ImGui::SameLine();
    DrawQuest(QUEST_MEDALLION_LIGHT);

    ImGui::End();
}

void InitItemTracker() {
    SohImGui::AddWindow("Randomizer", "Item Tracker", DrawItemTracker);
}

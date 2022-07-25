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
    int iconSize = CVar_GetS32("gRandoTrackIconSize", 0);
    ImGui::Image(SohImGui::GetTextureByName(hasEquip ? entry.name : entry.nameFaded), ImVec2(iconSize, iconSize),
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
    ITEM_TRACKER_MAP_ENTRY(QUEST_SKULL_TOKEN, 23),

};

void DrawQuest(uint32_t itemId) {
    const ItemTrackerMapEntry& entry = questTrackerMap[itemId];
    bool hasQuestItem = (entry.bitMask & gSaveContext.inventory.questItems) != 0;
    int iconSize = CVar_GetS32("gRandoTrackIconSize", 0);
    ImGui::BeginGroup();
    ImGui::Image(SohImGui::GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded), ImVec2(iconSize, iconSize),
                 ImVec2(0, 0), ImVec2(1, 1));

    ImVec2 p = ImGui::GetCursorScreenPos();
    int estimatedTextWidth = 10;
    int estimatedTextHeight = 10;
    ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - estimatedTextWidth, p.y - estimatedTextHeight));

    if (entry.name == "QUEST_SKULL_TOKEN") {
        if (gSaveContext.inventory.gsTokens == 0) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
            ImGui::Text("%i", gSaveContext.inventory.gsTokens);
            ImGui::PopStyleColor();
        } else if (gSaveContext.inventory.gsTokens >= 1 && gSaveContext.inventory.gsTokens <= 99) {
            ImGui::Text("%i", gSaveContext.inventory.gsTokens);
        } else if (gSaveContext.inventory.gsTokens >= 100) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::Text("%i", gSaveContext.inventory.gsTokens);
            ImGui::PopStyleColor();
        }
    }

    ImGui::EndGroup();

    SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
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
    ITEM_TRACKER_MAP_ENTRY(ITEM_HEART_CONTAINER, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MAGIC_SMALL, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_MAGIC_LARGE, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_WALLET_ADULT, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_WALLET_GIANT, 0),
};

void DrawItem(uint32_t itemId) {
    uint32_t actualItemId = INV_CONTENT(itemId);

    if (itemId == ITEM_HEART_CONTAINER) {
        actualItemId = itemId;
    }

    if (itemId == ITEM_WALLET_ADULT || itemId == ITEM_WALLET_GIANT) {
        if (CUR_UPG_VALUE(UPG_WALLET) == 2) {
            actualItemId = ITEM_WALLET_GIANT;
        } else if (CUR_UPG_VALUE(UPG_WALLET) < 2) {
            actualItemId = ITEM_WALLET_ADULT;
        }
    }

    if (itemId == ITEM_MAGIC_SMALL || itemId == ITEM_MAGIC_LARGE) {
        if (gSaveContext.magicLevel == 2) {
            actualItemId = ITEM_MAGIC_LARGE;
        } else {
            actualItemId = ITEM_MAGIC_SMALL;
        }
    }

    bool hasItem = actualItemId != ITEM_NONE;

    if (itemId == ITEM_HEART_CONTAINER) {
        if (gSaveContext.doubleDefense) {
            hasItem = true;
        } else {
            hasItem = false;
        }
    }

    if (itemId == ITEM_WALLET_ADULT || itemId == ITEM_WALLET_GIANT) {
        if (CUR_UPG_VALUE(UPG_WALLET) == 0) {
            hasItem = false;
        } else {
            hasItem = true;
        }
    }

    if (itemId == ITEM_MAGIC_SMALL || itemId == ITEM_MAGIC_LARGE) {
        if (gSaveContext.magicLevel == 0) {
            hasItem = false;
        } else {
            hasItem = true;
        }
    }

    const ItemTrackerMapEntry& entry = itemTrackerMap[hasItem ? actualItemId : itemId];
    int iconSize = CVar_GetS32("gRandoTrackIconSize", 0);

    ImGui::BeginGroup();
    ImGui::Image(SohImGui::GetTextureByName(hasItem ? entry.name : entry.nameFaded), ImVec2(iconSize, iconSize),
                 ImVec2(0, 0), ImVec2(1, 1));
    ImVec2 p = ImGui::GetCursorScreenPos();
    int estimatedTextWidth = 10;
    int estimatedTextHeight = 10;
    ImGui::SetCursorScreenPos(ImVec2(p.x - 5 + (iconSize / 2) - estimatedTextWidth, p.y - estimatedTextHeight));

    switch (actualItemId) {
        case ITEM_STICK:
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                if (AMMO(ITEM_STICK) == CUR_CAPACITY(UPG_STICKS)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%i", AMMO(ITEM_STICK));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                    ImGui::PopStyleColor();
                } else if (AMMO(ITEM_STICK) != 0 || AMMO(ITEM_STICK) == CUR_CAPACITY(UPG_STICKS) - 1) {
                    ImGui::Text("%i", AMMO(ITEM_STICK));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                } else if (AMMO(ITEM_STICK) == 0) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
                    ImGui::Text("%i", AMMO(ITEM_STICK));
                    ImGui::PopStyleColor();
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
                ImGui::SameLine(0, 0.0f);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("%i", CUR_CAPACITY(UPG_STICKS));
            ImGui::PopStyleColor();
            break;
        case ITEM_NUT:
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                if (AMMO(ITEM_NUT) == CUR_CAPACITY(UPG_NUTS)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%i", AMMO(ITEM_NUT));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                    ImGui::PopStyleColor();
                } else if (AMMO(ITEM_NUT) != 0 || AMMO(ITEM_NUT) == CUR_CAPACITY(UPG_NUTS) - 1) {
                    ImGui::Text("%i", AMMO(ITEM_NUT));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                } else if (AMMO(ITEM_NUT) == 0) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
                    ImGui::Text("%i", AMMO(ITEM_NUT));
                    ImGui::PopStyleColor();
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
                ImGui::SameLine(0, 0.0f);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("%i", CUR_CAPACITY(UPG_NUTS));
            ImGui::PopStyleColor();
            break;
        case ITEM_BOMB:
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                if (AMMO(ITEM_BOMB) == CUR_CAPACITY(UPG_BOMB_BAG)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%i", AMMO(ITEM_BOMB));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                    ImGui::PopStyleColor();
                } else if (AMMO(ITEM_BOMB) != 0 || AMMO(ITEM_BOMB) == CUR_CAPACITY(UPG_BOMB_BAG) - 1) {
                    ImGui::Text("%i", AMMO(ITEM_BOMB));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                } else if (AMMO(ITEM_BOMB) == 0) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
                    ImGui::Text("%i", AMMO(ITEM_BOMB));
                    ImGui::PopStyleColor();
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
                ImGui::SameLine(0, 0.0f);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("%i", CUR_CAPACITY(UPG_BOMB_BAG));
            ImGui::PopStyleColor();
            break;

        case ITEM_BOW:
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                if (AMMO(ITEM_BOW) == CUR_CAPACITY(UPG_QUIVER)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%i", AMMO(ITEM_BOW));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                    ImGui::PopStyleColor();
                } else if (AMMO(ITEM_BOW) != 0 || AMMO(ITEM_BOW) == CUR_CAPACITY(UPG_QUIVER) - 1) {
                    ImGui::Text("%i", AMMO(ITEM_BOW));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                } else if (AMMO(ITEM_BOW) == 0) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
                    ImGui::Text("%i", AMMO(ITEM_BOW));
                    ImGui::PopStyleColor();
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
                ImGui::SameLine(0, 0.0f);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("%i", CUR_CAPACITY(UPG_QUIVER));
            ImGui::PopStyleColor();
            break;
        case ITEM_SLINGSHOT:
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                if (AMMO(ITEM_SLINGSHOT) == CUR_CAPACITY(UPG_BULLET_BAG)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::Text("%i", AMMO(ITEM_SLINGSHOT));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                    ImGui::PopStyleColor();
                } else if (AMMO(ITEM_SLINGSHOT) != 0 || AMMO(ITEM_SLINGSHOT) == CUR_CAPACITY(UPG_BULLET_BAG) - 1) {
                    ImGui::Text("%i", AMMO(ITEM_SLINGSHOT));
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                } else if (AMMO(ITEM_SLINGSHOT) == 0) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
                    ImGui::Text("%i", AMMO(ITEM_SLINGSHOT));
                    ImGui::PopStyleColor();
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
                ImGui::SameLine(0, 0.0f);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("%i", CUR_CAPACITY(UPG_BULLET_BAG));
            ImGui::PopStyleColor();
            break;
        case ITEM_BOMBCHU:
            if (AMMO(ITEM_BOMBCHU) == 50) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("%i", AMMO(ITEM_BOMBCHU));
                if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
                ImGui::PopStyleColor();
            } else if (AMMO(ITEM_BOMBCHU) != 0 || AMMO(ITEM_BOMBCHU) < 50) {
                ImGui::Text("%i", AMMO(ITEM_BOMBCHU));
                if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
            } else if (AMMO(ITEM_BOMBCHU) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
                ImGui::Text("%i", AMMO(ITEM_BOMBCHU));
                ImGui::PopStyleColor();
                if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
            }
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                ImGui::SameLine(0, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("50");
                ImGui::PopStyleColor();
            }
            break;
        case ITEM_BEAN:
            if (AMMO(ITEM_BEAN) == 10) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("%i", AMMO(ITEM_BEAN));
                if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
                ImGui::PopStyleColor();
            } else if (AMMO(ITEM_BEAN) != 0 || AMMO(ITEM_BEAN) < 10) {
                ImGui::Text("%i", AMMO(ITEM_BEAN));
                if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
            } else if (AMMO(ITEM_BEAN) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
                ImGui::Text("%i", AMMO(ITEM_BEAN));
                ImGui::PopStyleColor();
                if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                    ImGui::SameLine(0, 0.0f);
                    ImGui::Text("/");
                }
            }
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                ImGui::SameLine(0, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("10");
                ImGui::PopStyleColor();
            }
            break;
        case ITEM_WALLET_ADULT:
        case ITEM_WALLET_GIANT:
            if (CUR_UPG_VALUE(UPG_WALLET) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("99");
                ImGui::PopStyleColor();
            } else if (CUR_UPG_VALUE(UPG_WALLET) == 1) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("200");
                ImGui::PopStyleColor();
            } else if (CUR_UPG_VALUE(UPG_WALLET) == 2) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("500");
                ImGui::PopStyleColor();
            }
            break;
        default:
            ImGui::Text(" ");
            break;
    }
    ImGui::EndGroup();

    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
}

void DrawBottle(uint32_t itemId, uint32_t bottleSlot) {
    uint32_t actualItemId = gSaveContext.inventory.items[SLOT(itemId) + bottleSlot];
    bool hasItem = actualItemId != ITEM_NONE;
    const ItemTrackerMapEntry& entry = itemTrackerMap[hasItem ? actualItemId : itemId];
    int iconSize = CVar_GetS32("gRandoTrackIconSize", 0);
    ImGui::Image(SohImGui::GetTextureByName(hasItem ? entry.name : entry.nameFaded), ImVec2(iconSize, iconSize),
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
    int iconSize = CVar_GetS32("gRandoTrackIconSize", 0);
    if (CUR_UPG_VALUE(categoryId) == 0) {
        const ItemTrackerUpgradeEntry& entry = upgradeTrackerMap[categoryId][0];
        ImGui::Image(SohImGui::GetTextureByName(entry.nameFaded), ImVec2(iconSize, iconSize),
            ImVec2(0, 0), ImVec2(1, 1));
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
    } else {
        const ItemTrackerUpgradeEntry& entry = upgradeTrackerMap[categoryId][CUR_UPG_VALUE(categoryId) - 1];
        ImGui::Image(SohImGui::GetTextureByName(entry.name), ImVec2(iconSize, iconSize),
            ImVec2(0, 0), ImVec2(1, 1));
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
    }
}

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    ImVec4 color;
} ItemTrackerSongEntry;

#define ITEM_TRACKER_SONG_ENTRY(id)               \
    {                                             \
        id, {                                     \
            id, #id, #id "_Faded"                 \
        }                                         \
    }

// Maps song ids to info for use in ImGui
std::unordered_map<int32_t, ItemTrackerSongEntry> songTrackerMap = {
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_LULLABY),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_EPONA),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_SARIA),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_SUN),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_TIME),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_STORMS),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_MINUET),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_BOLERO),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_SERENADE),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_REQUIEM),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_NOCTURNE),
    ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_PRELUDE),
};

#define VANILLA_ITEM_TRACKER_SONG_ENTRY(id)           \
    {                                                 \
        id, {                                         \
            id, #id "_Vanilla", #id "_Vanilla_Faded"  \
        }                                             \
    }

// Maps song ids to info for use in ImGui
std::unordered_map<int32_t, ItemTrackerSongEntry> vanillaSongTrackerMap = {
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_LULLABY),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_EPONA),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_SARIA),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_SUN),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_TIME),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_STORMS),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_MINUET),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_BOLERO),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_SERENADE),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_REQUIEM),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_NOCTURNE),
    VANILLA_ITEM_TRACKER_SONG_ENTRY(QUEST_SONG_PRELUDE),
};

void DrawSong(int32_t songId) {
    int iconSize = CVar_GetS32("gRandoTrackIconSize", 0);
    const ItemTrackerSongEntry& entry = CVar_GetS32("gItemTrackeSongColor", 0) ?
                                        songTrackerMap[songId] :
                                        vanillaSongTrackerMap[songId];
    uint32_t bitMask = 1 << entry.id;
    bool hasSong = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImGui::Image(SohImGui::GetTextureByName(hasSong ? entry.name : entry.nameFaded), ImVec2(iconSize/1.5, iconSize),
            ImVec2(0, 0), ImVec2(1, 1));
    SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
}

void DrawItemTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gItemTrackerEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    static uint32_t ccc = 0xFF000000;
    ImVec4 color = ImGui::ColorConvertU32ToFloat4(ccc);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, color);
    if (!ImGui::Begin("Item Tracker", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::PopStyleColor();
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("Item Tracker", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        int spacingX = CVar_GetS32("gRandoTrackIconSpacingX", 0);
        int spacingY = CVar_GetS32("gRandoTrackIconSpacingY", 0);

        if (ImGui::BeginTabItem("Item Tracker")) {

            ImGui::BeginGroup();
            DrawItem(ITEM_STICK);
            ImGui::SameLine(spacingX);
            DrawItem(ITEM_NUT);
            ImGui::SameLine(spacingX * 2);
            DrawItem(ITEM_BOMB);
            ImGui::SameLine(spacingX * 3);
            DrawItem(ITEM_BOW);
            ImGui::SameLine(spacingX * 4);
            DrawItem(ITEM_ARROW_FIRE);
            ImGui::SameLine(spacingX * 5);
            DrawItem(ITEM_DINS_FIRE);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawItem(ITEM_SLINGSHOT);
            ImGui::SameLine(spacingX);
            DrawItem(ITEM_OCARINA_FAIRY);
            ImGui::SameLine(spacingX * 2);
            DrawItem(ITEM_BOMBCHU);
            ImGui::SameLine(spacingX * 3);
            DrawItem(ITEM_HOOKSHOT);
            ImGui::SameLine(spacingX * 4);
            DrawItem(ITEM_ARROW_ICE);
            ImGui::SameLine(spacingX * 5);
            DrawItem(ITEM_FARORES_WIND);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawItem(ITEM_BOOMERANG);
            ImGui::SameLine(spacingX);
            DrawItem(ITEM_LENS);
            ImGui::SameLine(spacingX * 2);
            DrawItem(ITEM_BEAN);
            ImGui::SameLine(spacingX * 3);
            DrawItem(ITEM_HAMMER);
            ImGui::SameLine(spacingX * 4);
            DrawItem(ITEM_ARROW_LIGHT);
            ImGui::SameLine(spacingX * 5);
            DrawItem(ITEM_NAYRUS_LOVE);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawBottle(ITEM_BOTTLE, 0);
            ImGui::SameLine(spacingX);
            DrawBottle(ITEM_BOTTLE, 1);
            ImGui::SameLine(spacingX * 2);
            DrawBottle(ITEM_BOTTLE, 2);
            ImGui::SameLine(spacingX * 3);
            DrawBottle(ITEM_BOTTLE, 3);
            ImGui::SameLine(spacingX * 4);
            DrawItem(ITEM_POCKET_EGG); // ADULT TRADE
            ImGui::SameLine(spacingX * 5);
            DrawItem(ITEM_MASK_KEATON); // CHILD TRADE
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawEquip(ITEM_SWORD_KOKIRI);
            ImGui::SameLine(spacingX);
            DrawEquip(ITEM_SWORD_MASTER);
            ImGui::SameLine(spacingX * 2);
            DrawEquip(ITEM_SWORD_BGS); // PURPLE TODO: CHECK IF BGS OR BROKEN SWORD TO DISPLAY
            ImGui::SameLine(spacingX * 3);
            DrawQuest(QUEST_STONE_OF_AGONY);
            ImGui::SameLine(spacingX * 4);
            DrawQuest(QUEST_GERUDO_CARD);
            ImGui::SameLine(spacingX * 5);
            DrawQuest(QUEST_SKULL_TOKEN);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawEquip(ITEM_SHIELD_DEKU);
            ImGui::SameLine(spacingX);
            DrawEquip(ITEM_SHIELD_HYLIAN);
            ImGui::SameLine(spacingX * 2);
            DrawEquip(ITEM_SHIELD_MIRROR);
            ImGui::SameLine(spacingX * 3);
            DrawUpgrade(UPG_STRENGTH);
            ImGui::SameLine(spacingX * 4);
            DrawUpgrade(UPG_SCALE);
            ImGui::SameLine(spacingX * 5);
            DrawItem(ITEM_WALLET_ADULT);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawEquip(ITEM_TUNIC_KOKIRI);
            ImGui::SameLine(spacingX);
            DrawEquip(ITEM_TUNIC_GORON);
            ImGui::SameLine(spacingX * 2);
            DrawEquip(ITEM_TUNIC_ZORA);
            ImGui::SameLine(spacingX * 3);
            //ImGui::Dummy(ImVec2(spacingX + 48.0f, spacingY));
            DrawItem(ITEM_HEART_CONTAINER);
            ImGui::SameLine(spacingX * 4);
            DrawItem(ITEM_MAGIC_SMALL);
            ImGui::SameLine(spacingX * 5);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawEquip(ITEM_BOOTS_KOKIRI);
            ImGui::SameLine(spacingX);
            DrawEquip(ITEM_BOOTS_IRON);
            ImGui::SameLine(spacingX * 2);
            DrawEquip(ITEM_BOOTS_HOVER);
            ImGui::SameLine(spacingX * 3);
            DrawQuest(QUEST_KOKIRI_EMERALD);
            ImGui::SameLine(spacingX * 4);
            DrawQuest(QUEST_GORON_RUBY);
            ImGui::SameLine(spacingX * 5);
            DrawQuest(QUEST_ZORA_SAPPHIRE);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawQuest(QUEST_MEDALLION_FOREST);
            ImGui::SameLine(spacingX);
            DrawQuest(QUEST_MEDALLION_FIRE);
            ImGui::SameLine(spacingX * 2);
            DrawQuest(QUEST_MEDALLION_WATER);
            ImGui::SameLine(spacingX * 3);
            DrawQuest(QUEST_MEDALLION_SPIRIT);
            ImGui::SameLine(spacingX * 4);
            DrawQuest(QUEST_MEDALLION_SHADOW);
            ImGui::SameLine(spacingX * 5);
            DrawQuest(QUEST_MEDALLION_LIGHT);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawSong(QUEST_SONG_LULLABY);
            ImGui::SameLine(spacingX);
            DrawSong(QUEST_SONG_EPONA);
            ImGui::SameLine(spacingX * 2);
            DrawSong(QUEST_SONG_SARIA);
            ImGui::SameLine(spacingX * 3);
            DrawSong(QUEST_SONG_SUN);
            ImGui::SameLine(spacingX * 4);
            DrawSong(QUEST_SONG_TIME);
            ImGui::SameLine(spacingX * 5);
            DrawSong(QUEST_SONG_STORMS);
            ImGui::EndGroup();
            ImGui::BeginGroup();
            DrawSong(QUEST_SONG_MINUET);
            ImGui::SameLine(spacingX);
            DrawSong(QUEST_SONG_BOLERO);
            ImGui::SameLine(spacingX * 2);
            DrawSong(QUEST_SONG_SERENADE);
            ImGui::SameLine(spacingX * 3);
            DrawSong(QUEST_SONG_REQUIEM);
            ImGui::SameLine(spacingX * 4);
            DrawSong(QUEST_SONG_NOCTURNE);
            ImGui::SameLine(spacingX * 5);
            DrawSong(QUEST_SONG_PRELUDE);
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Options")) {
            int minimalSpacingX;
            if (CVar_GetS32("gItemTrackerAmmoDisplay", 0) == 1) {
                minimalSpacingX = 48;
            } else {
                minimalSpacingX = 32;
            }
            SohImGui::EnhancementCheckbox("Display \"Ammo/MaxAmo\"", "gItemTrackerAmmoDisplay");
            SohImGui::EnhancementCheckbox("Randomizer colors for Songs", "gItemTrackeSongColor");
            SohImGui::Tooltip("Will dispaly non-warp songs with randomizer\ncolors instead of pure white");
            SohImGui::EnhancementSliderInt("Icon size : %dpx", "##ITEMTRACKERICONSIZE", "gRandoTrackIconSize", 32, 128, "");

            SohImGui::EnhancementSliderInt("X spacing : %dpx", "##ITEMTRACKERSPACINGX", "gRandoTrackIconSpacingX", minimalSpacingX, 256,
                                           "");
            // SohImGui::EnhancementSliderInt("Y Spacing : %dpx", "##ITEMTRACKERSPACINGY", "gRandoTrackIconSpacingY", 0,
            //                                16, "");
            ImGui::Text("Chroma Key");

            auto flags = ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel;

            if (ImGui::ColorEdit4("ColorTest", (float*)&color, flags)) {
                ccc = ImGui::ColorConvertFloat4ToU32(color);
            }


            // ImVec4 colors = ImColor::HSV(0.39f, 0.00f, 0.63f, 0.11f);
            // //ImGui::ColorEdit4("TrackerBackgroundColor", colors, ImGuiColorEditFlags_DisplayHSV);
            // ImGui::PushStyleColor(ImGuiCol_WindowBg, color);
            // ImGui::PopStyleColor();
            // */
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::PopStyleColor();
    ImGui::End();
}

void InitItemTracker() {
    SohImGui::AddWindow("Randomizer", "Item Tracker", DrawItemTracker);
}
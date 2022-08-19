#include "randomizer_item_tracker.h"
#include "../../util.h"
#include "../libultraship/ImGuiImpl.h"
#include "../libultraship/Hooks.h"
#include "../libultraship/UltraController.h"
#include <soh/Enhancements/debugger/ImGuiHelpers.h>

#include <array>
#include <bit>
#include <map>
#include <string>
#include <vector>
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

void DrawEquip(ItemTrackerItem item);
void DrawItem(ItemTrackerItem item);
void DrawDungeonItem(ItemTrackerItem item);
void DrawBottle(ItemTrackerItem item);
void DrawQuest(ItemTrackerItem item);
void DrawUpgrade(ItemTrackerItem item);
void DrawSong(ItemTrackerItem item);

OSContPad* buttonsPressed;

std::vector<ItemTrackerItem> inventoryItems = {
    ITEM_TRACKER_ITEM(ITEM_STICK, 0, DrawItem),     ITEM_TRACKER_ITEM(ITEM_NUT, 0, DrawItem),           ITEM_TRACKER_ITEM(ITEM_BOMB, 0, DrawItem),
    ITEM_TRACKER_ITEM(ITEM_BOW, 0, DrawItem),       ITEM_TRACKER_ITEM(ITEM_ARROW_FIRE, 0, DrawItem),    ITEM_TRACKER_ITEM(ITEM_DINS_FIRE, 0, DrawItem),
    ITEM_TRACKER_ITEM(ITEM_SLINGSHOT, 0, DrawItem), ITEM_TRACKER_ITEM(ITEM_OCARINA_FAIRY, 0, DrawItem), ITEM_TRACKER_ITEM(ITEM_BOMBCHU, 0, DrawItem),
    ITEM_TRACKER_ITEM(ITEM_HOOKSHOT, 0, DrawItem),  ITEM_TRACKER_ITEM(ITEM_ARROW_ICE, 0, DrawItem),     ITEM_TRACKER_ITEM(ITEM_FARORES_WIND, 0, DrawItem),
    ITEM_TRACKER_ITEM(ITEM_BOOMERANG, 0, DrawItem), ITEM_TRACKER_ITEM(ITEM_LENS, 0, DrawItem),          ITEM_TRACKER_ITEM(ITEM_BEAN, 0, DrawItem),
    ITEM_TRACKER_ITEM(ITEM_HAMMER, 0, DrawItem),    ITEM_TRACKER_ITEM(ITEM_ARROW_LIGHT, 0, DrawItem),   ITEM_TRACKER_ITEM(ITEM_NAYRUS_LOVE, 0, DrawItem),
    ITEM_TRACKER_ITEM(ITEM_BOTTLE, 0, DrawBottle),  ITEM_TRACKER_ITEM(ITEM_BOTTLE, 1, DrawBottle),      ITEM_TRACKER_ITEM(ITEM_BOTTLE, 2, DrawBottle),
    ITEM_TRACKER_ITEM(ITEM_BOTTLE, 3, DrawBottle),  ITEM_TRACKER_ITEM(ITEM_POCKET_EGG, 0, DrawItem),    ITEM_TRACKER_ITEM(ITEM_MASK_KEATON, 0, DrawItem),
};

std::vector<ItemTrackerItem> equipmentItems = {
    ITEM_TRACKER_ITEM(ITEM_SWORD_KOKIRI, 1 << 0, DrawEquip),  ITEM_TRACKER_ITEM(ITEM_SWORD_MASTER, 1 << 1, DrawEquip),  ITEM_TRACKER_ITEM(ITEM_SWORD_BGS, 1 << 2, DrawEquip),
    ITEM_TRACKER_ITEM(ITEM_TUNIC_KOKIRI, 1 << 8, DrawEquip),  ITEM_TRACKER_ITEM(ITEM_TUNIC_GORON, 1 << 9, DrawEquip),   ITEM_TRACKER_ITEM(ITEM_TUNIC_ZORA, 1 << 10, DrawEquip),
    ITEM_TRACKER_ITEM(ITEM_SHIELD_DEKU, 1 << 4, DrawEquip),   ITEM_TRACKER_ITEM(ITEM_SHIELD_HYLIAN, 1 << 5, DrawEquip), ITEM_TRACKER_ITEM(ITEM_SHIELD_MIRROR, 1 << 6, DrawEquip),
    ITEM_TRACKER_ITEM(ITEM_BOOTS_KOKIRI, 1 << 12, DrawEquip), ITEM_TRACKER_ITEM(ITEM_BOOTS_IRON, 1 << 13, DrawEquip),   ITEM_TRACKER_ITEM(ITEM_BOOTS_HOVER, 1 << 14, DrawEquip),
};

std::vector<ItemTrackerItem> miscItems = {
    ITEM_TRACKER_ITEM(UPG_STRENGTH, 0, DrawUpgrade),          ITEM_TRACKER_ITEM(UPG_SCALE, 0, DrawUpgrade),             ITEM_TRACKER_ITEM(ITEM_WALLET_ADULT, 0, DrawItem),
    ITEM_TRACKER_ITEM(ITEM_HEART_CONTAINER, 0, DrawItem),     ITEM_TRACKER_ITEM(ITEM_MAGIC_SMALL, 0, DrawItem),         ITEM_TRACKER_ITEM(QUEST_STONE_OF_AGONY, 1 << 21, DrawQuest),
    ITEM_TRACKER_ITEM(QUEST_GERUDO_CARD, 1 << 22, DrawQuest), ITEM_TRACKER_ITEM(QUEST_SKULL_TOKEN, 1 << 23, DrawQuest),
};

std::vector<ItemTrackerItem> dungeonRewardStones = {
    ITEM_TRACKER_ITEM(QUEST_KOKIRI_EMERALD, 1 << 18, DrawQuest), ITEM_TRACKER_ITEM(QUEST_GORON_RUBY, 1 << 19, DrawQuest), ITEM_TRACKER_ITEM(QUEST_ZORA_SAPPHIRE, 1 << 20, DrawQuest),
};

std::vector<ItemTrackerItem> dungeonRewardMedallions = {
    ITEM_TRACKER_ITEM(QUEST_MEDALLION_FOREST, 1 << 0, DrawQuest), ITEM_TRACKER_ITEM(QUEST_MEDALLION_FIRE, 1 << 1, DrawQuest),   ITEM_TRACKER_ITEM(QUEST_MEDALLION_WATER, 1 << 2, DrawQuest),
    ITEM_TRACKER_ITEM(QUEST_MEDALLION_SPIRIT, 1 << 3, DrawQuest), ITEM_TRACKER_ITEM(QUEST_MEDALLION_SHADOW, 1 << 4, DrawQuest), ITEM_TRACKER_ITEM(QUEST_MEDALLION_LIGHT, 1 << 5, DrawQuest),
};

std::vector<ItemTrackerItem> songItems = {
    ITEM_TRACKER_ITEM(QUEST_SONG_LULLABY, 0, DrawSong), ITEM_TRACKER_ITEM(QUEST_SONG_EPONA, 0, DrawSong),    ITEM_TRACKER_ITEM(QUEST_SONG_SARIA, 0, DrawSong),
    ITEM_TRACKER_ITEM(QUEST_SONG_SUN, 0, DrawSong),     ITEM_TRACKER_ITEM(QUEST_SONG_TIME, 0, DrawSong),     ITEM_TRACKER_ITEM(QUEST_SONG_STORMS, 0, DrawSong),
    ITEM_TRACKER_ITEM(QUEST_SONG_MINUET, 0, DrawSong),  ITEM_TRACKER_ITEM(QUEST_SONG_BOLERO, 0, DrawSong),   ITEM_TRACKER_ITEM(QUEST_SONG_SERENADE, 0, DrawSong),
    ITEM_TRACKER_ITEM(QUEST_SONG_REQUIEM, 0, DrawSong), ITEM_TRACKER_ITEM(QUEST_SONG_NOCTURNE, 0, DrawSong), ITEM_TRACKER_ITEM(QUEST_SONG_PRELUDE, 0, DrawSong),
};

std::vector<ItemTrackerDungeon> itemTrackerDungeons = {
    ITEM_TRACKER_DUNGEON(SCENE_BMORI1, "FRST", true, true, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_HIDAN, "FIRE", true, true, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_MIZUSIN, "WATR", true, true, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_JYASINZOU, "SPIR", true, true, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_HAKADAN, "SHAD", true, true, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_HAKADANCH, "BOTW", true, true, false, true),
    ITEM_TRACKER_DUNGEON(SCENE_YDAN, "DEKU", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_DDAN, "DODO", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_BDAN, "JABU", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_ICE_DOUKUTO, "ICE", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_GANON, "GAN", false, false, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_MEN, "GTG", false, false, false, true),
    ITEM_TRACKER_DUNGEON(SCENE_GERUDOWAY, "HIDE", false, false, false, true),
};

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

bool IsValidSaveFile() {
    bool validSave = gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2;
    return validSave;
}

void DrawEquip(ItemTrackerItem item) {
    bool hasEquip = (item.data & gSaveContext.inventory.equipment) != 0;
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    ImGui::Image(SohImGui::GetTextureByName(hasEquip && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawQuest(ItemTrackerItem item) {
    bool hasQuestItem = (item.data & gSaveContext.inventory.questItems) != 0;
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    ImGui::BeginGroup();
    ImGui::Image(SohImGui::GetTextureByName(hasQuestItem && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    ImVec2 p = ImGui::GetCursorScreenPos();
    int estimatedTextWidth = 10;
    int estimatedTextHeight = 10;
    ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - estimatedTextWidth, p.y - estimatedTextHeight));

    if (item.name == "QUEST_SKULL_TOKEN") {
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

    SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
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
    ITEM_TRACKER_MAP_ENTRY(ITEM_DUNGEON_MAP, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_COMPASS, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_KEY_SMALL, 0),
    ITEM_TRACKER_MAP_ENTRY(ITEM_KEY_BOSS, 0)
};

void DrawItem(ItemTrackerItem item) {
    uint32_t itemId = item.id;
    uint32_t actualItemId = INV_CONTENT(itemId);
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);

    if (itemId == ITEM_NONE) {
        ImGui::BeginGroup();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(p.x - 5 + (iconSize / 2) - 10, p.y - 10));
        ImGui::EndGroup();
        return;
    }

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

    const ItemTrackerMapEntry& entry = itemTrackerMap[hasItem && IsValidSaveFile() ? actualItemId : itemId];

    ImGui::BeginGroup();
    ImGui::Image(SohImGui::GetTextureByName(hasItem && IsValidSaveFile() ? entry.name : entry.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    ImVec2 p = ImGui::GetCursorScreenPos();
    int estimatedTextWidth = 10;
    int estimatedTextHeight = 10;
    ImGui::SetCursorScreenPos(ImVec2(p.x - 5 + (iconSize / 2) - estimatedTextWidth, p.y - estimatedTextHeight));

    if (IsValidSaveFile()) {
        DrawItemAmmo(actualItemId);
    } else {
        ImGui::Text(" ");
    }
    ImGui::EndGroup();

    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
}

void DrawItemAmmo(int itemId) {
    switch (itemId) {
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
}

void DrawBottle(ItemTrackerItem item) {
    uint32_t actualItemId = gSaveContext.inventory.items[SLOT(item.id) + item.data];
    bool hasItem = actualItemId != ITEM_NONE;
    const ItemTrackerMapEntry& entry = itemTrackerMap[hasItem && IsValidSaveFile() ? actualItemId : item.id];
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    ImGui::Image(SohImGui::GetTextureByName(hasItem && IsValidSaveFile() ? entry.name : entry.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
};

void DrawDungeonItem(ItemTrackerItem item) {
    uint32_t itemId = item.id;
    uint32_t bitMask = 1 << (item.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[item.data]) != 0;
    bool hasSmallKey = (gSaveContext.inventory.dungeonKeys[item.data]) >= 0;
    ImGui::BeginGroup();
    if (itemId == ITEM_KEY_SMALL) {
        ImGui::Image(SohImGui::GetTextureByName(hasSmallKey && IsValidSaveFile() ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }
    else {
        ImGui::Image(SohImGui::GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }

    ImVec2 p = ImGui::GetCursorScreenPos();
    int estimatedTextWidth = 10;
    int estimatedTextHeight = 10;
    ImGui::SetCursorScreenPos(ImVec2(p.x - 5 + (iconSize / 2) - estimatedTextWidth, p.y - estimatedTextHeight));

    if (itemId == ITEM_KEY_SMALL) { // This check there for small key is necessary to get the text position properly and can't be on the same ITEM_KEY chack from the top
        if (gSaveContext.inventory.dungeonKeys[item.data] <= 0 || !IsValidSaveFile()) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
            ImGui::Text("0");
            ImGui::PopStyleColor();
         }
         else {
            ImGui::Text("%i", gSaveContext.inventory.dungeonKeys[item.data]);
         }  
    }
    ImGui::EndGroup();

    SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

typedef struct {
    uint8_t id;
    std::string name;
    std::string nameFaded;
} ItemTrackerUpgradeEntry;

#define ITEM_TRACKER_UPGRADE_ENTRY(id) \
    { id, #id, #id "_Faded" }

std::unordered_map<int32_t, std::vector<ItemTrackerUpgradeEntry>> upgradeTrackerMap = {
    { UPG_STRENGTH,
      {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BRACELET),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_GAUNTLETS_SILVER),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_GAUNTLETS_GOLD),
      } },
    { UPG_SCALE,
      {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_SCALE_SILVER),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_SCALE_GOLDEN),
      } },
    { UPG_QUIVER,
      {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_QUIVER_30),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_QUIVER_40),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_QUIVER_50),
      } },
    { UPG_BULLET_BAG,
      {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BULLET_BAG_30),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BULLET_BAG_40),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BULLET_BAG_50),
      } },
    { UPG_BOMB_BAG,
      {
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BOMB_BAG_20),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BOMB_BAG_30),
          ITEM_TRACKER_UPGRADE_ENTRY(ITEM_BOMB_BAG_40),
      } },

};

void DrawUpgrade(ItemTrackerItem item) {
    uint32_t categoryId = item.id;
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    if (CUR_UPG_VALUE(categoryId) == 0 || !IsValidSaveFile()) {
        const ItemTrackerUpgradeEntry& entry = upgradeTrackerMap[categoryId][0];
        ImGui::Image(SohImGui::GetTextureByName(entry.nameFaded), ImVec2(iconSize, iconSize), ImVec2(0, 0),
                     ImVec2(1, 1));
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
    } else {
        const ItemTrackerUpgradeEntry& entry = upgradeTrackerMap[categoryId][CUR_UPG_VALUE(categoryId) - 1];
        ImGui::Image(SohImGui::GetTextureByName(entry.name), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
    }
}

void DrawSong(ItemTrackerItem item) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    uint32_t bitMask = 1 << item.id;
    bool hasSong = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImGui::Image(SohImGui::GetTextureByName(hasSong && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize / 1.5, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
}

void DrawFloatingNotes(int Icon_Cells_Size, int Icon_Spacing) {
    if (CVar_GetS32("gItemTrackerNotes", 0)) {
        ImGui::BeginGroup();
            struct ItemTrackerNotes {
                static int TrackerNotesResizeCallback(ImGuiInputTextCallbackData* data) {
                    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                        ImVector<char>* itemTrackerNotes = (ImVector<char>*)data->UserData;
                        IM_ASSERT(itemTrackerNotes->begin() == data->Buf);
                        itemTrackerNotes->resize(
                            data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
                        data->Buf = itemTrackerNotes->begin();
                    }
                    return 0;
                }
                static bool TrackerNotesInputTextMultiline(const char* label, ImVector<char>* itemTrackerNotes, const ImVec2& size = ImVec2(0, 0),
                                                           ImGuiInputTextFlags flags = 0) {
                    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
                    return ImGui::InputTextMultiline(label, itemTrackerNotes->begin(), (size_t)itemTrackerNotes->size(),
                                                     size, flags | ImGuiInputTextFlags_CallbackResize,
                                                     ItemTrackerNotes::TrackerNotesResizeCallback,
                                                     (void*)itemTrackerNotes);
                }
            };
            static ImVector<char> itemTrackerNotes;
            if (itemTrackerNotes.empty()) {
                itemTrackerNotes.push_back(0);
            }
            ItemTrackerNotes::TrackerNotesInputTextMultiline("##ItemTrackerNotes", &itemTrackerNotes, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
            ImGui::EndGroup();
    }
}

// Windowing stuff
ImVec4 ChromaKeyBackground = { 0, 0, 0, 0 }; // Float value, 1 = 255 in rgb value.
void BeginFloatingWindows(std::string UniqueName) {
    ImGuiWindowFlags FloatingWndFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize;

    if (!CVar_GetS32("gItemTrackerWindowType", 0)) {
        FloatingWndFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!CVar_GetS32("gItemTrackerHudEditMode", 0)) {
            FloatingWndFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ChromaKeyBackground);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::Begin(UniqueName.c_str(), nullptr, FloatingWndFlags);
}
void EndFloatingWindows() {
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
}

/**
 * DrawItemsInRows
 * Takes in a vector of ItemTrackerItem and draws them in rows of N items
 */
void DrawItemsInRows(std::vector<ItemTrackerItem> items, int columns = 6) {
    int Icon_Cells_Size = CVar_GetS32("gItemTrackerIconSize", 32);
    int Icon_Spacing = CVar_GetS32("gItemTrackerIconSpacing", 0);
    for (int i = 0; i < items.size(); i++) {
        if (i % columns == 0) {
            ImGui::BeginGroup();
        } else {
            ImGui::SameLine(Icon_Cells_Size * (i % columns));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Icon_Spacing * (i % columns));
        }
        items[i].drawFunc(items[i]);
        if (i % columns == columns - 1) {
            ImGui::EndGroup();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Icon_Spacing);
        }
    }
    if (items.size() % columns != 0) {
        ImGui::EndGroup();
    }
}

/**
 * DrawDungeons
 * Loops over dungeons and creates vectors of items in the correct order 
 * to then call DrawItemsInRows
 */
void DrawDungeons(std::vector<ItemTrackerDungeon> dungeons, int columns = 6) {
    int Icon_Cells_Size = CVar_GetS32("gItemTrackerIconSize", 32);
    int Icon_Spacing = CVar_GetS32("gItemTrackerIconSpacing", 0);
    std::vector<ItemTrackerItem> dungeonItems = {};

    if (!CVar_GetS32("gItemTrackerDisplayDungeonItemsMaps", 0)) {
        dungeons.erase(std::remove_if(dungeons.begin(), dungeons.end(), [](ItemTrackerDungeon d) { return !d.hasBossKey && !d.hasSmallKey; }), dungeons.end());
    }

    ImGui::BeginGroup();
    ImGui::NewLine();
    for (int i = 0; i < MIN(dungeons.size(), columns); i++) {
        ImGui::SameLine(Icon_Cells_Size * i);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Icon_Spacing * i);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(p.x - 5 + (Icon_Cells_Size / 2) - 10, p.y - 4));
        ImGui::Text(dungeons[i].shortName.c_str());
    }
    ImGui::EndGroup();
    for (int i = 0; i < MIN(dungeons.size(), columns); i++) {
        if (dungeons[i].hasSmallKey) {
            dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, dungeons[i].id, DrawDungeonItem));
        } else {
            dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }
    }
    for (int i = 0; i < MIN(dungeons.size(), columns); i++) {
        if (dungeons[i].hasBossKey) {
            dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_KEY_BOSS, dungeons[i].id, DrawDungeonItem));
        } else {
            dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }
    }
    if (CVar_GetS32("gItemTrackerDisplayDungeonItemsMaps", 0)) {
        for (int i = 0; i < MIN(dungeons.size(), columns); i++) {
            if (dungeons[i].hasMap) {
                dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_DUNGEON_MAP, dungeons[i].id, DrawDungeonItem));
            } else {
                dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            }
        }
        for (int i = 0; i < MIN(dungeons.size(), columns); i++) {
            if (dungeons[i].hasCompass) {
                dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_COMPASS, dungeons[i].id, DrawDungeonItem));
            } else {
                dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            }
        }
    }
    DrawItemsInRows(dungeonItems, MIN(dungeons.size(), columns));

    if (dungeons.size() > columns) {
        DrawDungeons(std::vector<ItemTrackerDungeon>(dungeons.begin() + columns, dungeons.end()), columns);
    }
}

void DrawItemTracker(bool& open) {
    if (!open) {
        return;
    }
    int Icon_Cells_Size = CVar_GetS32("gItemTrackerIconSize", 32);
    int Icon_Spacing = CVar_GetS32("gItemTrackerIconSpacing", 0);

    if (CVar_GetS32("gItemTrackerHotKeyShow", 0) == 0 ? CVar_GetS32("gItemTrackerEnabled", 0) : buttonsPressed[0].button & BTN_L) {
        BeginFloatingWindows("Item Tracker#window");
        if (CVar_GetS32("gItemTrackerDisplayInventory", 0) && !CVar_GetS32("gItemTrackerDisplayInventorySeparate", 0)) {
            DrawItemsInRows(inventoryItems);
        }
        if (CVar_GetS32("gItemTrackerDisplayEquipment", 0) && !CVar_GetS32("gItemTrackerDisplayEquipmentSeparate", 0)) {
            DrawItemsInRows(equipmentItems);
        }
        if (CVar_GetS32("gItemTrackerDisplayMisc", 0) && !CVar_GetS32("gItemTrackerDisplayMiscSeparate", 0)) {
            DrawItemsInRows(miscItems);
        }
        if (CVar_GetS32("gItemTrackerDisplayDungeonRewards", 0) && !CVar_GetS32("gItemTrackerDisplayDungeonRewardsSeparate", 0)) {
            if (CVar_GetS32("gItemTrackerDisplayMisc", 0) && !CVar_GetS32("gItemTrackerDisplayMiscSeparate", 0)) {
                ImGui::SameLine(Icon_Cells_Size * 3);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Icon_Spacing * 3);
            }
            DrawItemsInRows(dungeonRewardStones);
            DrawItemsInRows(dungeonRewardMedallions);
        }
        if (CVar_GetS32("gItemTrackerDisplaySongs", 0) && !CVar_GetS32("gItemTrackerDisplaySongsSeparate", 0)) {
            DrawItemsInRows(songItems);
        }
        if (CVar_GetS32("gItemTrackerDisplayDungeonItems", 0) && !CVar_GetS32("gItemTrackerDisplayDungeonItemsSeparate", 0)) {
            DrawDungeons(itemTrackerDungeons);
        }
        EndFloatingWindows();

        if (CVar_GetS32("gItemTrackerDisplayInventory", 0) && CVar_GetS32("gItemTrackerDisplayInventorySeparate", 0)) {
            BeginFloatingWindows("Inventory Items Tracker");
            DrawItemsInRows(inventoryItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDisplayEquipment", 0) && CVar_GetS32("gItemTrackerDisplayEquipmentSeparate", 0)) {
            BeginFloatingWindows("Equipment Items Tracker");
            DrawItemsInRows(equipmentItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDisplayMisc", 0) && CVar_GetS32("gItemTrackerDisplayMiscSeparate", 0)) {
            BeginFloatingWindows("Misc Items Tracker");
            DrawItemsInRows(miscItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDisplayDungeonRewards", 0) && CVar_GetS32("gItemTrackerDisplayDungeonRewardsSeparate", 0)) {
            BeginFloatingWindows("Dungeon Rewards Tracker");
            DrawItemsInRows(dungeonRewardStones);
            DrawItemsInRows(dungeonRewardMedallions);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDisplaySongs", 0) && CVar_GetS32("gItemTrackerDisplaySongsSeparate", 0)) {
            BeginFloatingWindows("Songs Tracker");
            DrawItemsInRows(songItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDisplayDungeonItems", 0) && CVar_GetS32("gItemTrackerDisplayDungeonItemsSeparate", 0)) {
            BeginFloatingWindows("Dungeon Items Tracker");
            if (CVar_GetS32("gItemTrackerDisplayDungeonItemsHorizontal", 0)) {
                DrawDungeons(itemTrackerDungeons, 13);
            } else {
                DrawDungeons(itemTrackerDungeons);
            }
            EndFloatingWindows();
        }
    }
}

void DrawItemTrackerOptions(bool& open) {
    if (!open) {
        CVar_SetS32("gItemTrackerSettingsEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(240, 285), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Item Tracker Settings", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    SohImGui::PaddedText("Hotkey");
    SohImGui::PaddedEnhancementCheckbox("L Button to show Item Tracker", "gItemTrackerHotKeyShow", 0);
    if (CVar_GetS32("gItemTrackerHotKeyShow", 0)) {
        if (ImGui::BeginCombo("Button Type", CVar_GetS32("gItemTrackerHotKeyShowType", 0) == 0 ? "Hold" : "Toggle")) {
            if (ImGui::Selectable("Hold")) {
                CVar_SetS32("gItemTrackerHotKeyShowType", 0);
            }
            if (ImGui::Selectable("Toggle")) {
                CVar_SetS32("gItemTrackerHotKeyShowType", 1);
            }

            ImGui::EndCombo();
        }
        SohImGui::PaddedEnhancementCheckbox("Only enable while paused", "gItemTrackerHotKeyShowOnlyPaused", 0);
    }

    SohImGui::PaddedText("Window");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::ColorEdit4("Background Color", (float*)&ChromaKeyBackground, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel)) {
        CVar_SetFloat("gItemTrackerBgColorR", ChromaKeyBackground.x);
        CVar_SetFloat("gItemTrackerBgColorG", ChromaKeyBackground.y);
        CVar_SetFloat("gItemTrackerBgColorB", ChromaKeyBackground.z);
        CVar_SetFloat("gItemTrackerBgColorA", ChromaKeyBackground.w);
    }
    ImGui::PushItemWidth(0);
    if (ImGui::BeginCombo("Window Type", CVar_GetS32("gItemTrackerWindowType", 0) == 0 ? "Floating" : "Window")) {
        if (ImGui::Selectable("Floating")) {
            CVar_SetS32("gItemTrackerWindowType", 0);
        }
        if (ImGui::Selectable("Window")) {
            CVar_SetS32("gItemTrackerWindowType", 1);
        }

        ImGui::EndCombo();
    }
    if (CVar_GetS32("gItemTrackerWindowType", 0) == 0) {
        SohImGui::PaddedEnhancementCheckbox("Enable Dragging", "gItemTrackerHudEditMode", 0);
    }

    SohImGui::PaddedText("Icons");
    SohImGui::EnhancementSliderInt("Icon size : %dpx", "##ITEMTRACKERICONSIZE", "gItemTrackerIconSize", 25, 128, "", 32, true);
    SohImGui::EnhancementSliderInt("Icon margins : %dpx", "##ITEMTRACKERSPACING", "gItemTrackerIconSpacing", -5, 50, "", 0, true);
    SohImGui::PaddedEnhancementCheckbox("Display \"Current/Max\" values on items", "gItemTrackerDisplayCurrentMax", 0);

    SohImGui::PaddedText("Items");
    SohImGui::PaddedEnhancementCheckbox("Display general items in tracker", "gItemTrackerDisplayInventory", 0);
    if (CVar_GetS32("gItemTrackerDisplayInventory", 0)) {
        SohImGui::PaddedEnhancementCheckbox("Display general items in separate window", "gItemTrackerDisplayInventorySeparate", 0);
    }

    SohImGui::PaddedText("Equipment");
    SohImGui::PaddedEnhancementCheckbox("Display equipment in tracker", "gItemTrackerDisplayEquipment", 0);
    if (CVar_GetS32("gItemTrackerDisplayEquipment", 0)) {
        SohImGui::PaddedEnhancementCheckbox("Display equipment in separate window", "gItemTrackerDisplayEquipmentSeparate", 0);
    }

    SohImGui::PaddedText("Misc Items");
    SohImGui::PaddedEnhancementCheckbox("Display misc items in tracker", "gItemTrackerDisplayMisc", 0);
    if (CVar_GetS32("gItemTrackerDisplayMisc", 0)) {
        SohImGui::PaddedEnhancementCheckbox("Display misc items in separate window", "gItemTrackerDisplayMiscSeparate", 0);
    }

    SohImGui::PaddedText("Dungeon Rewards");
    SohImGui::PaddedEnhancementCheckbox("Display dungeon rewards in tracker", "gItemTrackerDisplayDungeonRewards", 0);
    if (CVar_GetS32("gItemTrackerDisplayDungeonRewards", 0)) {
        SohImGui::PaddedEnhancementCheckbox("Display dungeon rewards in separate window", "gItemTrackerDisplayDungeonRewardsSeparate", 0);
    }

    SohImGui::PaddedText("Songs");
    SohImGui::PaddedEnhancementCheckbox("Display songs in tracker", "gItemTrackerDisplaySongs", 0);
    if (CVar_GetS32("gItemTrackerDisplaySongs", 0)) {
        SohImGui::PaddedEnhancementCheckbox("Display songs in separate window", "gItemTrackerDisplaySongsSeparate", 0);
        SohImGui::PaddedEnhancementCheckbox("Use randomizer colors", "gItemTrackerDisplaySongsRandom", 0);
    }

    SohImGui::PaddedText("Dungeon Items");
    SohImGui::PaddedEnhancementCheckbox("Display dungeon items in tracker", "gItemTrackerDisplayDungeonItems", 0);
    if (CVar_GetS32("gItemTrackerDisplayDungeonItems", 0)) {
        SohImGui::PaddedEnhancementCheckbox("Display dungeon items in separate window", "gItemTrackerDisplayDungeonItemsSeparate", 0);
        if (CVar_GetS32("gItemTrackerDisplayDungeonItemsSeparate", 0)) {
            SohImGui::PaddedEnhancementCheckbox("Horizontal dungeon items display", "gItemTrackerDisplayDungeonItemsHorizontal", 0);
        }
        SohImGui::PaddedEnhancementCheckbox("Maps and compasses", "gItemTrackerDisplayDungeonItemsMaps", 0);
    }

    // SohImGui::EnhancementCheckbox("Personal notes space", "gItemTrackerNotes");
    // SohImGui::Tooltip("Adds a textbox under the item tracker to keep your own notes in");
    // TODO: FIX THE NOTES SPACE SIZE ON FLOATING WINDOW, DISABLED UNTIL FIXED

    ImGui::End();
}

void InitItemTracker() {
    SohImGui::AddWindow("Randomizer", "Item Tracker", DrawItemTracker);
    SohImGui::AddWindow("Randomizer", "Item Tracker Settings", DrawItemTrackerOptions);
    float trackerBgR = CVar_GetFloat("gItemTrackerBgColorR", 0);
    float trackerBgG = CVar_GetFloat("gItemTrackerBgColorG", 0);
    float trackerBgB = CVar_GetFloat("gItemTrackerBgColorB", 0);
    float trackerBgA = CVar_GetFloat("gItemTrackerBgColorA", 1);
    ChromaKeyBackground = {
        trackerBgR,
        trackerBgG,
        trackerBgB,
        trackerBgA
    }; // Float value, 1 = 255 in rgb value.
    Ship::RegisterHook<Ship::ControllerRead>([](OSContPad* cont_pad) {
        buttonsPressed = cont_pad;
    });
}

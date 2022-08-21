#include "randomizer_item_tracker.h"
#include "../../util.h"
#include "../libultraship/ImGuiImpl.h"
#include "../libultraship/Hooks.h"
#include "../libultraship/UltraController.h"
#include "../debugger/ImGuiHelpers.h"

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
}

void DrawEquip(ItemTrackerItem item);
void DrawItem(ItemTrackerItem item);
void DrawDungeonItem(ItemTrackerItem item);
void DrawBottle(ItemTrackerItem item);
void DrawQuest(ItemTrackerItem item);
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
    ITEM_TRACKER_ITEM(ITEM_BRACELET, 0, DrawItem),            ITEM_TRACKER_ITEM(ITEM_SCALE_SILVER, 0, DrawItem),        ITEM_TRACKER_ITEM(ITEM_WALLET_ADULT, 0, DrawItem),
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
    ITEM_TRACKER_DUNGEON(SCENE_JYASINZOU, "SPRT", true, true, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_HAKADAN, "SHDW", true, true, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_HAKADANCH, "BOTW", true, true, false, true),
    ITEM_TRACKER_DUNGEON(SCENE_YDAN, "DEKU", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_DDAN, "DCVN", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_BDAN, "JABU", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_ICE_DOUKUTO, "ICE", true, true, false, false),
    ITEM_TRACKER_DUNGEON(SCENE_GANON, "GANON", false, false, true, true),
    ITEM_TRACKER_DUNGEON(SCENE_MEN, "GTG", false, false, false, true),
    ITEM_TRACKER_DUNGEON(SCENE_GERUDOWAY, "HIDE", false, false, false, true),
};

std::unordered_map<uint32_t, ItemTrackerItem> actualItemTrackerItemMap = {
    { ITEM_BOTTLE, ITEM_TRACKER_ITEM(ITEM_BOTTLE, 0, DrawItem) },
    { ITEM_BIG_POE, ITEM_TRACKER_ITEM(ITEM_BIG_POE, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_BIG_POE, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_BIG_POE, 0, DrawItem) },
    { ITEM_BLUE_FIRE, ITEM_TRACKER_ITEM(ITEM_BLUE_FIRE, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_BLUE_FIRE, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_BLUE_FIRE, 0, DrawItem) },
    { ITEM_BUG, ITEM_TRACKER_ITEM(ITEM_BUG, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_BUGS, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_BUGS, 0, DrawItem) },
    { ITEM_FAIRY, ITEM_TRACKER_ITEM(ITEM_FAIRY, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_FAIRY, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_FAIRY, 0, DrawItem) },
    { ITEM_FISH, ITEM_TRACKER_ITEM(ITEM_FISH, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_FISH, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_FISH, 0, DrawItem) },
    { ITEM_POTION_GREEN, ITEM_TRACKER_ITEM(ITEM_POTION_GREEN, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_GREEN_POTION, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_GREEN_POTION, 0, DrawItem) },
    { ITEM_POE, ITEM_TRACKER_ITEM(ITEM_POE, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_POE, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_POE, 0, DrawItem) },
    { ITEM_POTION_RED, ITEM_TRACKER_ITEM(ITEM_POTION_RED, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_RED_POTION, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_RED_POTION, 0, DrawItem) },
    { ITEM_POTION_BLUE, ITEM_TRACKER_ITEM(ITEM_POTION_BLUE, 0, DrawItem) },
    { ITEM_BOTTLE_WITH_BLUE_POTION, ITEM_TRACKER_ITEM(ITEM_BOTTLE_WITH_BLUE_POTION, 0, DrawItem) },
    { ITEM_MILK, ITEM_TRACKER_ITEM(ITEM_MILK, 0, DrawItem) },
    { ITEM_MILK_BOTTLE, ITEM_TRACKER_ITEM(ITEM_MILK_BOTTLE, 0, DrawItem) },
    { ITEM_MILK_HALF, ITEM_TRACKER_ITEM(ITEM_MILK_HALF, 0, DrawItem) },
    { ITEM_LETTER_RUTO, ITEM_TRACKER_ITEM(ITEM_LETTER_RUTO, 0, DrawItem) },

    { ITEM_HOOKSHOT, ITEM_TRACKER_ITEM(ITEM_HOOKSHOT, 0, DrawItem) },
    { ITEM_LONGSHOT, ITEM_TRACKER_ITEM(ITEM_LONGSHOT, 0, DrawItem) },

    { ITEM_OCARINA_FAIRY, ITEM_TRACKER_ITEM(ITEM_OCARINA_FAIRY, 0, DrawItem) },
    { ITEM_OCARINA_TIME, ITEM_TRACKER_ITEM(ITEM_OCARINA_TIME, 0, DrawItem) },

    { ITEM_MAGIC_SMALL, ITEM_TRACKER_ITEM(ITEM_MAGIC_SMALL, 0, DrawItem) },
    { ITEM_MAGIC_LARGE, ITEM_TRACKER_ITEM(ITEM_MAGIC_LARGE, 0, DrawItem) },

    { ITEM_WALLET_ADULT, ITEM_TRACKER_ITEM(ITEM_WALLET_ADULT, 0, DrawItem) },
    { ITEM_WALLET_GIANT, ITEM_TRACKER_ITEM(ITEM_WALLET_GIANT, 0, DrawItem) },

    { ITEM_BRACELET, ITEM_TRACKER_ITEM(ITEM_BRACELET, 0, DrawItem) },
    { ITEM_GAUNTLETS_SILVER, ITEM_TRACKER_ITEM(ITEM_GAUNTLETS_SILVER, 0, DrawItem) },
    { ITEM_GAUNTLETS_GOLD, ITEM_TRACKER_ITEM(ITEM_GAUNTLETS_GOLD, 0, DrawItem) },

    { ITEM_SCALE_SILVER, ITEM_TRACKER_ITEM(ITEM_SCALE_SILVER, 0, DrawItem) },
    { ITEM_SCALE_GOLDEN, ITEM_TRACKER_ITEM(ITEM_SCALE_GOLDEN, 0, DrawItem) },

    { ITEM_WEIRD_EGG, ITEM_TRACKER_ITEM(ITEM_WEIRD_EGG, 0, DrawItem) },
    { ITEM_CHICKEN, ITEM_TRACKER_ITEM(ITEM_CHICKEN, 0, DrawItem) },
    { ITEM_LETTER_ZELDA, ITEM_TRACKER_ITEM(ITEM_LETTER_ZELDA, 0, DrawItem) },
    { ITEM_MASK_KEATON, ITEM_TRACKER_ITEM(ITEM_MASK_KEATON, 0, DrawItem) },
    { ITEM_MASK_SKULL, ITEM_TRACKER_ITEM(ITEM_MASK_SKULL, 0, DrawItem) },
    { ITEM_MASK_SPOOKY, ITEM_TRACKER_ITEM(ITEM_MASK_SPOOKY, 0, DrawItem) },
    { ITEM_MASK_BUNNY, ITEM_TRACKER_ITEM(ITEM_MASK_BUNNY, 0, DrawItem) },
    { ITEM_MASK_GORON, ITEM_TRACKER_ITEM(ITEM_MASK_GORON, 0, DrawItem) },
    { ITEM_MASK_ZORA, ITEM_TRACKER_ITEM(ITEM_MASK_ZORA, 0, DrawItem) },
    { ITEM_MASK_GERUDO, ITEM_TRACKER_ITEM(ITEM_MASK_GERUDO, 0, DrawItem) },
    { ITEM_MASK_TRUTH, ITEM_TRACKER_ITEM(ITEM_MASK_TRUTH, 0, DrawItem) },
    { ITEM_SOLD_OUT, ITEM_TRACKER_ITEM(ITEM_SOLD_OUT, 0, DrawItem) },

    { ITEM_POCKET_EGG, ITEM_TRACKER_ITEM(ITEM_POCKET_EGG, 0, DrawItem) },
    { ITEM_POCKET_CUCCO, ITEM_TRACKER_ITEM(ITEM_POCKET_CUCCO, 0, DrawItem) },
    { ITEM_COJIRO, ITEM_TRACKER_ITEM(ITEM_COJIRO, 0, DrawItem) },
    { ITEM_ODD_MUSHROOM, ITEM_TRACKER_ITEM(ITEM_ODD_MUSHROOM, 0, DrawItem) },
    { ITEM_ODD_POTION, ITEM_TRACKER_ITEM(ITEM_ODD_POTION, 0, DrawItem) },
    { ITEM_SAW, ITEM_TRACKER_ITEM(ITEM_SAW, 0, DrawItem) },
    { ITEM_SWORD_BROKEN, ITEM_TRACKER_ITEM(ITEM_SWORD_BROKEN, 0, DrawItem) },
    { ITEM_PRESCRIPTION, ITEM_TRACKER_ITEM(ITEM_PRESCRIPTION, 0, DrawItem) },
    { ITEM_FROG, ITEM_TRACKER_ITEM(ITEM_FROG, 0, DrawItem) },
    { ITEM_EYEDROPS, ITEM_TRACKER_ITEM(ITEM_EYEDROPS, 0, DrawItem) },
    { ITEM_CLAIM_CHECK, ITEM_TRACKER_ITEM(ITEM_CLAIM_CHECK, 0, DrawItem) },
};

std::vector<uint32_t> buttonMap = {
    BTN_A,
    BTN_B,
    BTN_CUP,
    BTN_CDOWN,
    BTN_CLEFT,
    BTN_CRIGHT,
    BTN_L,
    BTN_Z,
    BTN_R,
    BTN_START,
    BTN_DUP,
    BTN_DDOWN,
    BTN_DLEFT,
    BTN_DRIGHT,
};

ImVec2 GetItemCurrentAndMax(ItemTrackerItem item) {
    ImVec2 result = { 0, 0 };

    switch (item.id) {
        case ITEM_STICK:
            result.x = CUR_CAPACITY(UPG_STICKS);
            result.y = 30;
            break;
        case ITEM_NUT:
            result.x = CUR_CAPACITY(UPG_NUTS);
            result.y = 40;
            break;
        case ITEM_BOMB:
            result.x = CUR_CAPACITY(UPG_BOMB_BAG);
            result.y = 40;
            break;
        case ITEM_BOW:
            result.x = CUR_CAPACITY(UPG_QUIVER);
            result.y = 50;
            break;
        case ITEM_SLINGSHOT:
            result.x = CUR_CAPACITY(UPG_BULLET_BAG);
            result.y = 50;
            break;
        case ITEM_WALLET_ADULT:
        case ITEM_WALLET_GIANT:
            result.x = CUR_CAPACITY(UPG_WALLET);
            result.y = 500;
            break;
        case ITEM_BEAN:
            result.x = AMMO(ITEM_BEAN);
            result.y = 10;
            break;
        case QUEST_SKULL_TOKEN:
            result.x = gSaveContext.inventory.gsTokens;
            result.y = 100;
            break;
        case ITEM_KEY_SMALL:
            result.x = gSaveContext.inventory.dungeonKeys[item.data];
            switch (item.data) {
                case SCENE_BMORI1:
                    result.y = 5;
                    break;
                case SCENE_HIDAN:
                    result.y = 8;
                    break;
                case SCENE_MIZUSIN:
                    result.y = 6;
                    break;
                case SCENE_JYASINZOU:
                    result.y = 5;
                    break;
                case SCENE_HAKADAN:
                    result.y = 5;
                    break;
                case SCENE_HAKADANCH:
                    result.y = 3;
                    break;
                case SCENE_GANON:
                    result.y = 2;
                    break;
                case SCENE_MEN:
                    result.y = 9;
                    break;
                case SCENE_GERUDOWAY:
                    result.y = 4;
                    break;
            }
            break;
    }

    return result;
}

void DrawItemCount(ItemTrackerItem item) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    ImVec2 currentAndMax = GetItemCurrentAndMax(item);
    ImVec2 p = ImGui::GetCursorScreenPos();

    if (currentAndMax.x > 0) {
        if (currentAndMax.x >= currentAndMax.y) {
            std::string currentString = std::to_string((int)currentAndMax.x);

            ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(currentString.c_str()).x / 2), p.y - 10));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("%d", (int)currentAndMax.x);
            ImGui::PopStyleColor();
        } else {
            if (CVar_GetS32("gItemTrackerDisplayCurrentMax", 0) == 1) {
                std::string currentAndMaxString = std::to_string((int)currentAndMax.x) + "/" + std::to_string((int)currentAndMax.y);

                ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(currentAndMaxString.c_str()).x / 2), p.y - 10));
                ImGui::Text("%d/", (int)currentAndMax.x);
                ImGui::SameLine(0, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text("%d", (int)currentAndMax.y);
                ImGui::PopStyleColor();
            } else {
                std::string currentString = std::to_string((int)currentAndMax.x);

                ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(currentString.c_str()).x / 2), p.y - 10));
                ImGui::Text("%d", (int)currentAndMax.x);
            }
        }
    }
}

void DrawEquip(ItemTrackerItem item) {
    bool hasEquip = (item.data & gSaveContext.inventory.equipment) != 0;
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    ImGui::Image(SohImGui::GetTextureByName(hasEquip ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawQuest(ItemTrackerItem item) {
    bool hasQuestItem = (item.data & gSaveContext.inventory.questItems) != 0;
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    ImGui::BeginGroup();
    ImGui::Image(SohImGui::GetTextureByName(hasQuestItem ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    if (item.id == QUEST_SKULL_TOKEN) {
        DrawItemCount(item);
    }

    ImGui::EndGroup();

    SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
};

void DrawItem(ItemTrackerItem item) {
    uint32_t actualItemId = INV_CONTENT(item.id);
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    bool hasItem = actualItemId != ITEM_NONE;

    if (item.id == ITEM_NONE) {
        ImGui::BeginGroup();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(p.x - 5 + (iconSize / 2) - 10, p.y - 10));
        ImGui::EndGroup();
        return;
    }

    switch (item.id) {
        case ITEM_HEART_CONTAINER:
            actualItemId = item.id;
            hasItem = !!gSaveContext.doubleDefense;
            break;
        case ITEM_MAGIC_SMALL:
        case ITEM_MAGIC_LARGE:
            actualItemId = gSaveContext.magicLevel == 2 ? ITEM_MAGIC_LARGE : ITEM_MAGIC_SMALL;
            hasItem = gSaveContext.magicLevel > 0;
            break;
        case ITEM_WALLET_ADULT:
        case ITEM_WALLET_GIANT:
            actualItemId = CUR_UPG_VALUE(UPG_WALLET) == 2 ? ITEM_WALLET_GIANT : ITEM_WALLET_ADULT;
            hasItem = true;
            break;
        case ITEM_BRACELET:
        case ITEM_GAUNTLETS_SILVER:
        case ITEM_GAUNTLETS_GOLD:
            actualItemId = CUR_UPG_VALUE(UPG_STRENGTH) == 3 ? ITEM_GAUNTLETS_GOLD : CUR_UPG_VALUE(UPG_STRENGTH) == 2 ? ITEM_GAUNTLETS_SILVER : ITEM_BRACELET;
            hasItem = CUR_UPG_VALUE(UPG_STRENGTH) > 0;
            break;
        case ITEM_SCALE_SILVER:
        case ITEM_SCALE_GOLDEN:
            actualItemId = CUR_UPG_VALUE(UPG_SCALE) == 2 ? ITEM_SCALE_GOLDEN : ITEM_SCALE_SILVER;
            hasItem = CUR_UPG_VALUE(UPG_SCALE) > 0;
            break;
    }

    if (hasItem && item.id != actualItemId && actualItemTrackerItemMap.find(actualItemId) != actualItemTrackerItemMap.end()) {
        item = actualItemTrackerItemMap[actualItemId];
    }
    
    ImGui::BeginGroup();
    ImGui::Image(SohImGui::GetTextureByName(hasItem ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    DrawItemCount(item);
    ImGui::EndGroup();

    SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawBottle(ItemTrackerItem item) {
    uint32_t actualItemId = gSaveContext.inventory.items[SLOT(item.id) + item.data];
    bool hasItem = actualItemId != ITEM_NONE;

    const ItemTrackerItem& actualItem = actualItemTrackerItemMap[hasItem ? actualItemId : item.id];
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    ImGui::Image(SohImGui::GetTextureByName(hasItem ? actualItem.name : actualItem.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    SetLastItemHoverText(SohUtils::GetItemName(actualItem.id));
};

void DrawDungeonItem(ItemTrackerItem item) {
    uint32_t itemId = item.id;
    uint32_t bitMask = 1 << (item.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[item.data]) != 0;
    bool hasSmallKey = (gSaveContext.inventory.dungeonKeys[item.data]) >= 0;
    ImGui::BeginGroup();
    if (itemId == ITEM_KEY_SMALL) {
        ImGui::Image(SohImGui::GetTextureByName(hasSmallKey ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }
    else {
        ImGui::Image(SohImGui::GetTextureByName(hasItem ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }

    if (itemId == ITEM_KEY_SMALL) {
        DrawItemCount(item);
    }
    ImGui::EndGroup();

    SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawSong(ItemTrackerItem item) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    uint32_t bitMask = 1 << item.id;
    bool hasSong = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImGui::Image(SohImGui::GetTextureByName(hasSong ? item.name : item.nameFaded),
                 ImVec2(iconSize / 1.5, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
}

void DrawNotes() {
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
    // ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    ItemTrackerNotes::TrackerNotesInputTextMultiline("##ItemTrackerNotes", &itemTrackerNotes, ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y), ImGuiInputTextFlags_AllowTabInput);
    // ImGui::PopStyleColor();
    ImGui::EndGroup();
}

// Windowing stuff
ImVec4 ChromaKeyBackground = { 0, 0, 0, 0 }; // Float value, 1 = 255 in rgb value.
void BeginFloatingWindows(std::string UniqueName, ImGuiWindowFlags flags = 0) {
    ImGuiWindowFlags windowFlags = flags;

    if (windowFlags == 0) {
        windowFlags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize;
    }

    if (!CVar_GetS32("gItemTrackerWindowType", 0)) {
        windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!CVar_GetS32("gItemTrackerHudEditMode", 0)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ChromaKeyBackground);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::Begin(UniqueName.c_str(), nullptr, windowFlags);
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
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 7);
    for (int i = 0; i < items.size(); i++) {
        if (i % columns == 0) {
            ImGui::BeginGroup();
        } else {
            ImGui::SameLine(iconSize * (i % columns));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconSpacing * (i % columns));
        }
        items[i].drawFunc(items[i]);
        if (i % columns == columns - 1) {
            ImGui::EndGroup();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + iconSpacing);
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
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 7);
    std::vector<ItemTrackerItem> dungeonItems = {};

    if (!CVar_GetS32("gItemTrackerDisplayDungeonItemsMaps", 1)) {
        dungeons.erase(std::remove_if(dungeons.begin(), dungeons.end(), [](ItemTrackerDungeon d) { return !d.hasBossKey && !d.hasSmallKey; }), dungeons.end());
    }

    ImGui::BeginGroup();
    ImGui::NewLine();
    for (int i = 0; i < MIN(dungeons.size(), columns); i++) {
        ImGui::SameLine(iconSize * i);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconSpacing * i);
        ImVec2 p = ImGui::GetCursorScreenPos();
        std::string dungeonName = dungeons[i].shortName;

        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(dungeonName.c_str()).x / 2), p.y - 4));
        ImGui::Text(dungeonName.c_str());
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
    if (CVar_GetS32("gItemTrackerDisplayDungeonItemsMaps", 1)) {
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

/* TODO: These need to be moved to a common imgui file */
void LabeledComboBoxRightAligned(const char* label, const char* cvar, std::vector<std::string> options, s32 defaultValue) {
    s32 currentValue = CVar_GetS32(cvar, defaultValue);
    std::string hiddenLabel = "##" + std::string(cvar);
    ImGui::Text(label);
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(options[currentValue].c_str()).x * 1.0f + 20.0f));
    ImGui::PushItemWidth((ImGui::CalcTextSize(options[currentValue].c_str()).x * 1.0f) + 30.0f);
    if (ImGui::BeginCombo(hiddenLabel.c_str(), options[currentValue].c_str())) {
        for (int i = 0; i < options.size(); i++) {
            if (ImGui::Selectable(options[i].c_str())) {
                CVar_SetS32(cvar, i);
                SohImGui::needs_save = true;
            }
        }

        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
}

void PaddedEnhancementCheckbox(const char* text, const char* cvarName, s32 defaultValue = 0, bool padTop = true, bool padBottom = true) {
    if (padTop) {
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
    }
    bool val = (bool)CVar_GetS32(cvarName, defaultValue);
        if (ImGui::Checkbox(text, &val)) {
            CVar_SetS32(cvarName, val);
            SohImGui::needs_save = true;
        }
    if (padBottom) {
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
    }
}
/* ****************************************************** */

void DrawItemTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gItemTrackerEnabled", 0);
        return;
    }
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 32);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 7);
    int comboButton1Mask = buttonMap[CVar_GetS32("gItemTrackerComboButton1", 6)];
    int comboButton2Mask = buttonMap[CVar_GetS32("gItemTrackerComboButton2", 8)];
    bool comboButtonsHeld = buttonsPressed != nullptr && buttonsPressed[0].button & comboButton1Mask && buttonsPressed[0].button & comboButton2Mask;
    bool isPaused = CVar_GetS32("gItemTrackerShowOnlyPaused", 0) == 0 || gGlobalCtx != nullptr && gGlobalCtx->pauseCtx.state > 0;

    if (isPaused && (CVar_GetS32("gItemTrackerDisplayType", 0) == 0 ? CVar_GetS32("gItemTrackerEnabled", 0) : comboButtonsHeld)) {
        if (
            (CVar_GetS32("gItemTrackerInventoryItemsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerEquipmentItemsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerSongsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 1) == 1)
        ) {
            BeginFloatingWindows("Item Tracker##main window");
            if (CVar_GetS32("gItemTrackerInventoryItemsDisplayType", 1) == 1) {
                DrawItemsInRows(inventoryItems);
            }
            if (CVar_GetS32("gItemTrackerEquipmentItemsDisplayType", 1) == 1) {
                DrawItemsInRows(equipmentItems);
            }
            if (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 1) {
                DrawItemsInRows(miscItems);
            }
            if (CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) == 1) {
                if (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 1) {
                    ImGui::SameLine(iconSize * 3);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconSpacing * 3);
                }
                DrawItemsInRows(dungeonRewardStones);
                DrawItemsInRows(dungeonRewardMedallions);
            }
            if (CVar_GetS32("gItemTrackerSongsDisplayType", 1) == 1) {
                DrawItemsInRows(songItems);
            }
            if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 2) == 1) {
                DrawDungeons(itemTrackerDungeons);
            }
            if (CVar_GetS32("gItemTrackerNotesDisplayType", 2) == 1 && CVar_GetS32("gItemTrackerDisplayType", 0) == 0) {
                DrawNotes();
            }
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerInventoryItemsDisplayType", 1) == 2) {
            BeginFloatingWindows("Inventory Items Tracker");
            DrawItemsInRows(inventoryItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerEquipmentItemsDisplayType", 1) == 2) {
            BeginFloatingWindows("Equipment Items Tracker");
            DrawItemsInRows(equipmentItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 2) {
            BeginFloatingWindows("Misc Items Tracker");
            DrawItemsInRows(miscItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) == 2) {
            BeginFloatingWindows("Dungeon Rewards Tracker");
            DrawItemsInRows(dungeonRewardStones);
            DrawItemsInRows(dungeonRewardMedallions);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerSongsDisplayType", 1) == 2) {
            BeginFloatingWindows("Songs Tracker");
            DrawItemsInRows(songItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 2) == 2) {
            BeginFloatingWindows("Dungeon Items Tracker");
            if (CVar_GetS32("gItemTrackerDisplayDungeonItemsHorizontal", 1)) {
                DrawDungeons(itemTrackerDungeons, 13);
            } else {
                DrawDungeons(itemTrackerDungeons);
            }
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerNotesDisplayType", 2) == 2 && CVar_GetS32("gItemTrackerDisplayType", 0) == 0) {
            BeginFloatingWindows("Personal Notes", ImGuiWindowFlags_NoFocusOnAppearing);
            DrawNotes();
            EndFloatingWindows();
        }
    }
}

void DrawItemTrackerOptions(bool& open) {
    if (!open) {
        CVar_SetS32("gItemTrackerSettingsEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600,360), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Item Tracker Settings", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("itemTrackerSettingsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);
    ImGui::TableSetupColumn("General settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Section settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    LabeledComboBoxRightAligned("Display Mode", "gItemTrackerDisplayType", { "Always", "Combo Button Hold" }, 0);
    if (CVar_GetS32("gItemTrackerDisplayType", 0) > 0) {
        LabeledComboBoxRightAligned("Combo Button 1", "gItemTrackerComboButton1", { "A", "B", "C-Up", "C-Down", "C-Left", "C-Right", "L", "Z", "R", "Start", "D-Up", "D-Down", "D-Left", "D-Right" }, 6);
        LabeledComboBoxRightAligned("Combo Button 2", "gItemTrackerComboButton2", { "A", "B", "C-Up", "C-Down", "C-Left", "C-Right", "L", "Z", "R", "Start", "D-Up", "D-Down", "D-Left", "D-Right" }, 8);
    }
    PaddedEnhancementCheckbox("Only enable while paused", "gItemTrackerShowOnlyPaused", 0);
    PaddedSeparator();
    ImGui::Text("BG Color");
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::ColorEdit4("BG Color##gItemTrackerBgColor", (float*)&ChromaKeyBackground, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel)) {
        CVar_SetFloat("gItemTrackerBgColorR", ChromaKeyBackground.x);
        CVar_SetFloat("gItemTrackerBgColorG", ChromaKeyBackground.y);
        CVar_SetFloat("gItemTrackerBgColorB", ChromaKeyBackground.z);
        CVar_SetFloat("gItemTrackerBgColorA", ChromaKeyBackground.w);
        SohImGui::needs_save = true;
    }
    ImGui::PopItemWidth();

    LabeledComboBoxRightAligned("Window Type", "gItemTrackerWindowType", { "Floating", "Window" }, 0);

    if (CVar_GetS32("gItemTrackerWindowType", 0) == 0) {
        PaddedEnhancementCheckbox("Enable Dragging", "gItemTrackerHudEditMode", 0);
    }
    PaddedSeparator();
    SohImGui::EnhancementSliderInt("Icon size : %dpx", "##ITEMTRACKERICONSIZE", "gItemTrackerIconSize", 25, 128, "", 32, true);
    SohImGui::EnhancementSliderInt("Icon margins : %dpx", "##ITEMTRACKERSPACING", "gItemTrackerIconSpacing", -5, 50, "", 7, true);
    PaddedEnhancementCheckbox("Display \"Current/Max\" values", "gItemTrackerDisplayCurrentMax", 0);

    ImGui::TableNextColumn();

    LabeledComboBoxRightAligned("Inventory", "gItemTrackerInventoryItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Equipment", "gItemTrackerEquipmentItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Misc", "gItemTrackerMiscItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Dungeon Rewards", "gItemTrackerDungeonRewardsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Songs", "gItemTrackerSongsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Dungeon Items", "gItemTrackerDungeonItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 2);
    if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 2) != 0) {
        if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 2) == 2) {
            PaddedEnhancementCheckbox("Horizontal display", "gItemTrackerDisplayDungeonItemsHorizontal", 1);
        }
        PaddedEnhancementCheckbox("Maps and compasses", "gItemTrackerDisplayDungeonItemsMaps", 1);
    }

    if (CVar_GetS32("gItemTrackerDisplayType", 0) != 1) {
        LabeledComboBoxRightAligned("Personal notes", "gItemTrackerNotesDisplayType", { "Hidden", "Main Window", "Seperate" }, 2);
    }

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::End();
}

void InitItemTracker() {
    // TODO: We want to persist this open, but currently I'm not sure of a way to dependably wait till the textures are loaded before we draw the tracker
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

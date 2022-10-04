#include "randomizer_item_tracker.h"
#include "../../util.h"
#include "../../OTRGlobals.h"
#include <libultraship/ImGuiImpl.h>
#include "../../UIWidgets.hpp"

#include <map>
#include <string>
#include <vector>
#include <libultraship/Cvar.h>
#include <libultraship/Hooks.h>

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
void DrawSong(ItemTrackerItem item);

OSContPad* buttonsPressed;

bool shouldUpdateVectors = true;

std::vector<ItemTrackerItem> mainWindowItems = {};

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

std::vector<ItemTrackerItem> dungeonRewards = {};

std::vector<ItemTrackerItem> songItems = {
    ITEM_TRACKER_ITEM(QUEST_SONG_LULLABY, 0, DrawSong), ITEM_TRACKER_ITEM(QUEST_SONG_EPONA, 0, DrawSong),    ITEM_TRACKER_ITEM(QUEST_SONG_SARIA, 0, DrawSong),
    ITEM_TRACKER_ITEM(QUEST_SONG_SUN, 0, DrawSong),     ITEM_TRACKER_ITEM(QUEST_SONG_TIME, 0, DrawSong),     ITEM_TRACKER_ITEM(QUEST_SONG_STORMS, 0, DrawSong),
    ITEM_TRACKER_ITEM(QUEST_SONG_MINUET, 0, DrawSong),  ITEM_TRACKER_ITEM(QUEST_SONG_BOLERO, 0, DrawSong),   ITEM_TRACKER_ITEM(QUEST_SONG_SERENADE, 0, DrawSong),
    ITEM_TRACKER_ITEM(QUEST_SONG_REQUIEM, 0, DrawSong), ITEM_TRACKER_ITEM(QUEST_SONG_NOCTURNE, 0, DrawSong), ITEM_TRACKER_ITEM(QUEST_SONG_PRELUDE, 0, DrawSong),
};

std::vector<ItemTrackerDungeon> itemTrackerDungeonsWithMapsHorizontal = {
    { SCENE_YDAN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_DDAN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_BDAN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_BMORI1, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_HIDAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_MIZUSIN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_JYASINZOU, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_HAKADAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_GANONTIKA, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_HAKADANCH, { ITEM_KEY_SMALL, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_ICE_DOUKUTO, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_MEN, { ITEM_KEY_SMALL } },
};

std::vector<ItemTrackerDungeon> itemTrackerDungeonsHorizontal = {
    { SCENE_BMORI1, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_HIDAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_MIZUSIN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_JYASINZOU, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_HAKADAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_GANONTIKA, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_HAKADANCH, { ITEM_KEY_SMALL } },
    { SCENE_MEN, { ITEM_KEY_SMALL } },
};


std::vector<ItemTrackerDungeon> itemTrackerDungeonsWithMapsCompact = {
    { SCENE_BMORI1, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_HIDAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_MIZUSIN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_JYASINZOU, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_HAKADAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_HAKADANCH, { ITEM_KEY_SMALL, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_YDAN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_DDAN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_BDAN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_ICE_DOUKUTO, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_GANONTIKA, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_MEN, { ITEM_KEY_SMALL } },
};

std::vector<ItemTrackerDungeon> itemTrackerDungeonsCompact = {
    { SCENE_BMORI1, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_HIDAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_MIZUSIN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_JYASINZOU, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_HAKADAN, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_GANONTIKA, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_HAKADANCH, { ITEM_KEY_SMALL } },
    { SCENE_MEN, { ITEM_KEY_SMALL } },
    { SCENE_GERUDOWAY, { ITEM_KEY_SMALL } },
};

std::map<uint16_t, std::string> itemTrackerDungeonShortNames = {
    { SCENE_BMORI1, "FRST" },
    { SCENE_HIDAN, "FIRE" },
    { SCENE_MIZUSIN, "WATR" },
    { SCENE_JYASINZOU, "SPRT" },
    { SCENE_HAKADAN, "SHDW" },
    { SCENE_HAKADANCH, "BOTW" },
    { SCENE_YDAN, "DEKU" },
    { SCENE_DDAN, "DCVN" },
    { SCENE_BDAN, "JABU" },
    { SCENE_ICE_DOUKUTO, "ICE" },
    { SCENE_GANONTIKA, "GANON" },
    { SCENE_MEN, "GTG" },
    { SCENE_GERUDOWAY, "HIDE" },
};

std::vector<ItemTrackerItem> dungeonItems = {};

std::unordered_map<uint32_t, ItemTrackerItem> actualItemTrackerItemMap = {
    { ITEM_BOTTLE, ITEM_TRACKER_ITEM(ITEM_BOTTLE, 0, DrawItem) },
    { ITEM_BIG_POE, ITEM_TRACKER_ITEM(ITEM_BIG_POE, 0, DrawItem) },
    { ITEM_BLUE_FIRE, ITEM_TRACKER_ITEM(ITEM_BLUE_FIRE, 0, DrawItem) },
    { ITEM_BUG, ITEM_TRACKER_ITEM(ITEM_BUG, 0, DrawItem) },
    { ITEM_FAIRY, ITEM_TRACKER_ITEM(ITEM_FAIRY, 0, DrawItem) },
    { ITEM_FISH, ITEM_TRACKER_ITEM(ITEM_FISH, 0, DrawItem) },
    { ITEM_POTION_GREEN, ITEM_TRACKER_ITEM(ITEM_POTION_GREEN, 0, DrawItem) },
    { ITEM_POE, ITEM_TRACKER_ITEM(ITEM_POE, 0, DrawItem) },
    { ITEM_POTION_RED, ITEM_TRACKER_ITEM(ITEM_POTION_RED, 0, DrawItem) },
    { ITEM_POTION_BLUE, ITEM_TRACKER_ITEM(ITEM_POTION_BLUE, 0, DrawItem) },
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
    BTN_DRIGHT
};

typedef enum {
  ITEM_TRACKER_NUMBER_NONE,
  ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY,
  ITEM_TRACKER_NUMBER_CURRENT_AMMO_ONLY,
  ITEM_TRACKER_NUMBER_CAPACITY,
  ITEM_TRACKER_NUMBER_AMMO,
} ItemTrackerNumberOption;

struct ItemTrackerNumbers {
  int currentCapacity;
  int maxCapacity;
  int currentAmmo;
};

bool IsValidSaveFile() {
    bool validSave = gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2;
    return validSave;
}

ItemTrackerNumbers GetItemCurrentAndMax(ItemTrackerItem item) {
    ItemTrackerNumbers result;
    result.currentCapacity = 0;
    result.maxCapacity = 0;
    result.currentAmmo = 0;

    switch (item.id) {
        case ITEM_STICK:
            result.currentCapacity = CUR_CAPACITY(UPG_STICKS);
            result.maxCapacity = 30;
            result.currentAmmo = AMMO(ITEM_STICK);
            break;
        case ITEM_NUT:
            result.currentCapacity = CUR_CAPACITY(UPG_NUTS);
            result.maxCapacity = 40;
            result.currentAmmo = AMMO(ITEM_NUT);
            break;
        case ITEM_BOMB:
            result.currentCapacity = CUR_CAPACITY(UPG_BOMB_BAG);
            result.maxCapacity = 40;
            result.currentAmmo = AMMO(ITEM_BOMB);
            break;
        case ITEM_BOW:
            result.currentCapacity = CUR_CAPACITY(UPG_QUIVER);
            result.maxCapacity = 50;
            result.currentAmmo = AMMO(ITEM_BOW);
            break;
        case ITEM_SLINGSHOT:
            result.currentCapacity = CUR_CAPACITY(UPG_BULLET_BAG);
            result.maxCapacity = 50;
            result.currentAmmo = AMMO(ITEM_SLINGSHOT);
            break;
        case ITEM_WALLET_ADULT:
        case ITEM_WALLET_GIANT:
            result.currentCapacity = CUR_CAPACITY(UPG_WALLET);
            result.maxCapacity = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHOPSANITY) > 1 ? 999 : 500;
            result.currentAmmo = gSaveContext.rupees;
            break;
        case ITEM_BOMBCHU:
            result.currentCapacity = INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU ? 50 : 0;
            result.maxCapacity = 50;
            result.currentAmmo = AMMO(ITEM_BOMBCHU);
            break;
        case ITEM_BEAN:
            result.currentCapacity = INV_CONTENT(ITEM_BEAN) == ITEM_BEAN ? 10 : 0;
            result.maxCapacity = 10;
            result.currentAmmo = AMMO(ITEM_BEAN);
            break;
        case QUEST_SKULL_TOKEN:
            result.maxCapacity = result.currentCapacity = 100;
            result.currentAmmo = gSaveContext.inventory.gsTokens;
            break;
        case ITEM_KEY_SMALL:
            result.currentAmmo = MAX(gSaveContext.inventory.dungeonKeys[item.data], 0);
            switch (item.data) {
                case SCENE_BMORI1:
                    result.maxCapacity = result.currentCapacity = 5;
                    break;
                case SCENE_HIDAN:
                    result.maxCapacity = result.currentCapacity = 8;
                    break;
                case SCENE_MIZUSIN:
                    result.maxCapacity = result.currentCapacity = 6;
                    break;
                case SCENE_JYASINZOU:
                    result.maxCapacity = result.currentCapacity = 5;
                    break;
                case SCENE_HAKADAN:
                    result.maxCapacity = result.currentCapacity = 5;
                    break;
                case SCENE_HAKADANCH:
                    result.maxCapacity = result.currentCapacity = 3;
                    break;
                case SCENE_GANONTIKA:
                    result.maxCapacity = result.currentCapacity = 2;
                    break;
                case SCENE_MEN:
                    result.maxCapacity = result.currentCapacity = 9;
                    break;
                case SCENE_GERUDOWAY:
                    result.maxCapacity = result.currentCapacity = 4;
                    break;
            }
            break;
    }

    return result;
}

#define IM_COL_WHITE IM_COL32(255, 255, 255, 255)
#define IM_COL_RED IM_COL32(255, 0, 0, 255)
#define IM_COL_GREEN IM_COL32(0, 255, 0, 255)
#define IM_COL_GRAY IM_COL32(155, 155, 155, 255)

void DrawItemCount(ItemTrackerItem item) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    ItemTrackerNumbers currentAndMax = GetItemCurrentAndMax(item);
    ImVec2 p = ImGui::GetCursorScreenPos();
    int32_t trackerNumberDisplayMode = CVar_GetS32("gItemTrackerCapacityTrack", 1);

    if (currentAndMax.currentCapacity > 0 && trackerNumberDisplayMode != ITEM_TRACKER_NUMBER_NONE && IsValidSaveFile()) {
        std::string currentString = "";
        std::string maxString = "";
        ImU32 currentColor = IM_COL_WHITE;
        ImU32 maxColor = item.id == QUEST_SKULL_TOKEN ? IM_COL_RED : IM_COL_GREEN;

        bool shouldAlignToLeft = CVar_GetS32("gItemTrackerCurrentOnLeft", 0) &&
            trackerNumberDisplayMode != ITEM_TRACKER_NUMBER_CAPACITY &&
            trackerNumberDisplayMode != ITEM_TRACKER_NUMBER_AMMO;

        bool shouldDisplayAmmo = trackerNumberDisplayMode == ITEM_TRACKER_NUMBER_AMMO ||
            trackerNumberDisplayMode == ITEM_TRACKER_NUMBER_CURRENT_AMMO_ONLY ||
            // These items have a static capacity, so display ammo instead
            item.id == ITEM_BOMBCHU ||
            item.id == ITEM_BEAN ||
            item.id == QUEST_SKULL_TOKEN ||
            item.id == ITEM_KEY_SMALL;

        bool shouldDisplayMax = !(trackerNumberDisplayMode == ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY || trackerNumberDisplayMode == ITEM_TRACKER_NUMBER_CURRENT_AMMO_ONLY);

        if (shouldDisplayAmmo) {
            currentString = std::to_string(currentAndMax.currentAmmo);
            if (currentAndMax.currentAmmo >= currentAndMax.currentCapacity) {
                if (item.id == QUEST_SKULL_TOKEN) {
                    currentColor = IM_COL_RED;
                } else {
                    currentColor = IM_COL_GREEN;
                }
            }
            if (shouldDisplayMax) {
                currentString += "/";
                maxString = std::to_string(currentAndMax.currentCapacity);
            }
            if (currentAndMax.currentAmmo <= 0) {
                currentColor = IM_COL_GRAY;
            }
        } else {
            currentString = std::to_string(currentAndMax.currentCapacity);
            if (currentAndMax.currentCapacity >= currentAndMax.maxCapacity) {
                currentColor = IM_COL_GREEN;
            } else if (shouldDisplayMax) {
                currentString += "/";
                maxString = std::to_string(currentAndMax.maxCapacity);
            }
        }

        float x = shouldAlignToLeft ? p.x : p.x + (iconSize / 2) - (ImGui::CalcTextSize((currentString + maxString).c_str()).x / 2);

        ImGui::SetCursorScreenPos(ImVec2(x, p.y - 14));
        ImGui::PushStyleColor(ImGuiCol_Text, currentColor);
        ImGui::Text(currentString.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, maxColor);
        ImGui::Text(maxString.c_str());
        ImGui::PopStyleColor();
    } else {
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y - 14));
        ImGui::Text("");
    }
}

void DrawEquip(ItemTrackerItem item) {
    bool hasEquip = (item.data & gSaveContext.inventory.equipment) != 0;
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    ImGui::Image(SohImGui::GetTextureByName(hasEquip && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawQuest(ItemTrackerItem item) {
    bool hasQuestItem = (item.data & gSaveContext.inventory.questItems) != 0;
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    ImGui::BeginGroup();
    ImGui::Image(SohImGui::GetTextureByName(hasQuestItem && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    if (item.id == QUEST_SKULL_TOKEN) {
        DrawItemCount(item);
    }

    ImGui::EndGroup();

    UIWidgets::SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
};

void DrawItem(ItemTrackerItem item) {
    uint32_t actualItemId = INV_CONTENT(item.id);
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    bool hasItem = actualItemId != ITEM_NONE;

    if (item.id == ITEM_NONE) {
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
    ImGui::Image(SohImGui::GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    DrawItemCount(item);
    ImGui::EndGroup();

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawBottle(ItemTrackerItem item) {
    uint32_t actualItemId = gSaveContext.inventory.items[SLOT(item.id) + item.data];
    bool hasItem = actualItemId != ITEM_NONE;

    if (hasItem && item.id != actualItemId && actualItemTrackerItemMap.find(actualItemId) != actualItemTrackerItemMap.end()) {
        item = actualItemTrackerItemMap[actualItemId];
    }

    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    ImGui::Image(SohImGui::GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
};

void DrawDungeonItem(ItemTrackerItem item) {
    uint32_t itemId = item.id;
    uint32_t bitMask = 1 << (item.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
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

    if (itemId == ITEM_KEY_SMALL) {
        DrawItemCount(item);

        ImVec2 p = ImGui::GetCursorScreenPos();
        std::string dungeonName = itemTrackerDungeonShortNames[item.data];
        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(dungeonName.c_str()).x / 2), p.y - (iconSize + 16)));
        ImGui::Text(dungeonName.c_str());
    }

    if (itemId == ITEM_DUNGEON_MAP && 
        (item.data == SCENE_YDAN || item.data == SCENE_DDAN || item.data == SCENE_BDAN || item.data == SCENE_ICE_DOUKUTO)
    ) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        std::string dungeonName = itemTrackerDungeonShortNames[item.data];
        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(dungeonName.c_str()).x / 2), p.y - (iconSize + 13)));
        ImGui::Text(dungeonName.c_str());
    }
    ImGui::EndGroup();

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawSong(ItemTrackerItem item) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    uint32_t bitMask = 1 << item.id;
    bool hasSong = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(ImVec2(p.x + 6, p.y));
    ImGui::Image(SohImGui::GetTextureByName(hasSong && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize / 1.5, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    UIWidgets::SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
}

static ImVector<char> itemTrackerNotes;

void DrawNotes(bool resizeable = false) {
    ImGui::BeginGroup();
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 12);

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
    ImVec2 size = resizeable ? ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y) : ImVec2(((iconSize + iconSpacing) * 6) - 8, 200);
    ItemTrackerNotes::TrackerNotesInputTextMultiline("##ItemTrackerNotes", &itemTrackerNotes, size, ImGuiInputTextFlags_AllowTabInput);
    if (ImGui::IsItemDeactivatedAfterEdit() && IsValidSaveFile()) {
        CVar_SetString(("gItemTrackerNotes" + std::to_string(gSaveContext.fileNum)).c_str(), std::string(std::begin(itemTrackerNotes), std::end(itemTrackerNotes)).c_str());
        SohImGui::RequestCvarSaveOnNextTick();
    }
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
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!CVar_GetS32("gItemTrackerHudEditMode", 0)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ChromaKeyBackground);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin(UniqueName.c_str(), nullptr, windowFlags);
}
void EndFloatingWindows() {
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
}

/**
 * DrawItemsInRows
 * Takes in a vector of ItemTrackerItem and draws them in rows of N items
 */
void DrawItemsInRows(std::vector<ItemTrackerItem> items, int columns = 6) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 12);
    int topPadding = CVar_GetS32("gItemTrackerWindowType", 0) ? 20 : 0;

    for (int i = 0; i < items.size(); i++) {
        int row = i / columns;
        int column = i % columns;
        ImGui::SetCursorPos(ImVec2((column * (iconSize + iconSpacing) + 8), (row * (iconSize + iconSpacing)) + 8 + topPadding));
        items[i].drawFunc(items[i]);
    }
}

/**
 * DrawItemsInACircle
 * Takes in a vector of ItemTrackerItem and draws them evenly spread across a circle
 */
void DrawItemsInACircle(std::vector<ItemTrackerItem> items) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 12);

    ImVec2 max = ImGui::GetWindowContentRegionMax();
    float radius = (iconSize + iconSpacing) * 2;

    for (int i = 0; i < items.size(); i++) {
        float angle = (float)i / items.size() * 2.0f * M_PI;
        float x = (radius / 2.0f) * cos(angle) + max.x / 2.0f;
        float y = (radius / 2.0f) * sin(angle) + max.y / 2.0f;
        ImGui::SetCursorPos(ImVec2(x - 14, y + 4));
        items[i].drawFunc(items[i]);
    }
}

/**
 * GetDungeonItemsVector
 * Loops over dungeons and creates vectors of items in the correct order 
 * to then call DrawItemsInRows
 */
std::vector<ItemTrackerItem> GetDungeonItemsVector(std::vector<ItemTrackerDungeon> dungeons, int columns = 6) {
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 12);
    std::vector<ItemTrackerItem> dungeonItems = {};

    int rowCount = 0;
    for (int i = 0; i < dungeons.size(); i++) {
        if (dungeons[i].items.size() > rowCount) rowCount = dungeons[i].items.size();
    }

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < MIN(dungeons.size(), columns); j++) {
            if (dungeons[j].items.size() > i) {
                switch (dungeons[j].items[i]) {
                    case ITEM_KEY_SMALL:
                        dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, dungeons[j].id, DrawDungeonItem));
                        break;
                    case ITEM_KEY_BOSS:
                        // Swap Ganon's Castle boss key to the right scene ID manually
                        if (dungeons[j].id == SCENE_GANONTIKA) {
                            dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_KEY_BOSS, SCENE_GANON, DrawDungeonItem));
                        } else {
                            dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_KEY_BOSS, dungeons[j].id, DrawDungeonItem));
                        }
                        break;
                    case ITEM_DUNGEON_MAP:
                        dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_DUNGEON_MAP, dungeons[j].id, DrawDungeonItem));
                        break;
                    case ITEM_COMPASS:
                        dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_COMPASS, dungeons[j].id, DrawDungeonItem));
                        break;
                }
            } else {
                dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            }
        }
    }

    if (dungeons.size() > columns) {
        std::vector<ItemTrackerItem> nextDungeonItems = GetDungeonItemsVector(std::vector<ItemTrackerDungeon>(dungeons.begin() + columns, dungeons.end()), columns);
        dungeonItems.insert(dungeonItems.end(), nextDungeonItems.begin(), nextDungeonItems.end());
    }

    return dungeonItems;
}

/* TODO: These need to be moved to a common imgui file */
void LabeledComboBoxRightAligned(const char* label, const char* cvar, std::vector<std::string> options, s32 defaultValue) {
    s32 currentValue = CVar_GetS32(cvar, defaultValue);
    std::string hiddenLabel = "##" + std::string(cvar);
    ImGui::Text(label);
#ifdef __WIIU__
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(options[currentValue].c_str()).x * 1.0f + 40.0f));
    ImGui::PushItemWidth((ImGui::CalcTextSize(options[currentValue].c_str()).x * 1.0f) + 60.0f);
#else
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(options[currentValue].c_str()).x * 1.0f + 20.0f));
    ImGui::PushItemWidth((ImGui::CalcTextSize(options[currentValue].c_str()).x * 1.0f) + 30.0f);
#endif
    if (ImGui::BeginCombo(hiddenLabel.c_str(), options[currentValue].c_str())) {
        for (int i = 0; i < options.size(); i++) {
            if (ImGui::Selectable(options[i].c_str())) {
                CVar_SetS32(cvar, i);
                SohImGui::RequestCvarSaveOnNextTick();
                shouldUpdateVectors = true;
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
            SohImGui::RequestCvarSaveOnNextTick();
            shouldUpdateVectors = true;
        }
    if (padBottom) {
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
    }
}
/* ****************************************************** */

void UpdateVectors() {
    if  (!shouldUpdateVectors) {
        return;
    }

    dungeonRewards.clear();
    dungeonRewards.insert(dungeonRewards.end(), dungeonRewardStones.begin(), dungeonRewardStones.end());
    dungeonRewards.insert(dungeonRewards.end(), dungeonRewardMedallions.begin(), dungeonRewardMedallions.end());

    dungeonItems.clear();
    if (CVar_GetS32("gItemTrackerDisplayDungeonItemsHorizontal", 1) && CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 0) == 2) {
        if (CVar_GetS32("gItemTrackerDisplayDungeonItemsMaps", 1)) {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsWithMapsHorizontal, 12);
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems[23] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_GERUDOWAY, DrawDungeonItem);
        } else {
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsHorizontal, 8);
            dungeonItems[15] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_GERUDOWAY, DrawDungeonItem);
        }
    } else {
        if (CVar_GetS32("gItemTrackerDisplayDungeonItemsMaps", 1)) {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsWithMapsCompact);
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems[35] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_GERUDOWAY, DrawDungeonItem);
        } else {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsCompact);
        }
    }

    mainWindowItems.clear();
    if (CVar_GetS32("gItemTrackerInventoryItemsDisplayType", 1) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), inventoryItems.begin(), inventoryItems.end());
    }
    if (CVar_GetS32("gItemTrackerEquipmentItemsDisplayType", 1) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), equipmentItems.begin(), equipmentItems.end());
    }
    if (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), miscItems.begin(), miscItems.end());
    }
    if (CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) == 1) {
        if (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 1) {
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }
        mainWindowItems.insert(mainWindowItems.end(), dungeonRewardStones.begin(), dungeonRewardStones.end());
        mainWindowItems.insert(mainWindowItems.end(), dungeonRewardMedallions.begin(), dungeonRewardMedallions.end());
    }
    if (CVar_GetS32("gItemTrackerSongsDisplayType", 1) == 1) {
        if (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 1 && CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) != 1) {
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }
        mainWindowItems.insert(mainWindowItems.end(), songItems.begin(), songItems.end());
    }
    if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 0) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), dungeonItems.begin(), dungeonItems.end());
    }

    shouldUpdateVectors = false;
}

void DrawItemTracker(bool& open) {
    UpdateVectors();
    if (!open) {
        CVar_SetS32("gItemTrackerEnabled", 0);
        return;
    }
    int iconSize = CVar_GetS32("gItemTrackerIconSize", 36);
    int iconSpacing = CVar_GetS32("gItemTrackerIconSpacing", 12);
    int comboButton1Mask = buttonMap[CVar_GetS32("gItemTrackerComboButton1", 6)];
    int comboButton2Mask = buttonMap[CVar_GetS32("gItemTrackerComboButton2", 8)];
    bool comboButtonsHeld = buttonsPressed != nullptr && buttonsPressed[0].button & comboButton1Mask && buttonsPressed[0].button & comboButton2Mask;
    bool isPaused = CVar_GetS32("gItemTrackerShowOnlyPaused", 0) == 0 || gGlobalCtx != nullptr && gGlobalCtx->pauseCtx.state > 0;

    if (CVar_GetS32("gItemTrackerWindowType", 0) == 1 || isPaused && (CVar_GetS32("gItemTrackerDisplayType", 0) == 0 ? CVar_GetS32("gItemTrackerEnabled", 0) : comboButtonsHeld)) {
        if (
            (CVar_GetS32("gItemTrackerInventoryItemsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerEquipmentItemsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerSongsDisplayType", 1) == 1) ||
            (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 0) == 1) ||
            (CVar_GetS32("gItemTrackerNotesDisplayType", 0) == 1)
        ) {
            BeginFloatingWindows("Item Tracker##main window");
            DrawItemsInRows(mainWindowItems, 6);

            if (CVar_GetS32("gItemTrackerNotesDisplayType", 0) == 1 && CVar_GetS32("gItemTrackerDisplayType", 0) == 0) {
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
            DrawItemsInRows(equipmentItems, 3);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerMiscItemsDisplayType", 1) == 2) {
            BeginFloatingWindows("Misc Items Tracker");
            DrawItemsInRows(miscItems, 4);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) == 2) {
            BeginFloatingWindows("Dungeon Rewards Tracker");
            if (CVar_GetS32("gItemTrackerDungeonRewardsCircle", 0) == 1) {
                ImGui::BeginGroup();
                DrawItemsInACircle(dungeonRewardMedallions);
                ImGui::EndGroup();
                ImGui::BeginGroup();
                DrawItemsInRows(dungeonRewardStones);
                ImGui::EndGroup();
            } else {
                DrawItemsInRows(dungeonRewards, 3);
            }
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerSongsDisplayType", 1) == 2) {
            BeginFloatingWindows("Songs Tracker");
            DrawItemsInRows(songItems);
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 0) == 2) {
            BeginFloatingWindows("Dungeon Items Tracker");
            if (CVar_GetS32("gItemTrackerDisplayDungeonItemsHorizontal", 1)) {
                if (CVar_GetS32("gItemTrackerDisplayDungeonItemsMaps", 1)) {
                    DrawItemsInRows(dungeonItems, 12);
                } else {
                    DrawItemsInRows(dungeonItems, 8);
                }
            } else {
                DrawItemsInRows(dungeonItems);
            }
            EndFloatingWindows();
        }

        if (CVar_GetS32("gItemTrackerNotesDisplayType", 0) == 2 && CVar_GetS32("gItemTrackerDisplayType", 0) == 0) {
            ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
            BeginFloatingWindows("Personal Notes", ImGuiWindowFlags_NoFocusOnAppearing);
            DrawNotes(true);
            EndFloatingWindows();
        }
    }
}

const char* itemTrackerCapacityTrackOptions[5] = { "No Numbers", "Current Capacity", "Current Ammo", "Current Capacity / Max Capacity", "Current Ammo / Current Capacity" };

void DrawItemTrackerOptions(bool& open) {
    if (!open) {
        CVar_SetS32("gItemTrackerSettingsEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600,375), ImGuiCond_FirstUseEver);

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
    ImGui::Text("BG Color");
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::ColorEdit4("BG Color##gItemTrackerBgColor", (float*)&ChromaKeyBackground, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel)) {
        CVar_SetFloat("gItemTrackerBgColorR", ChromaKeyBackground.x);
        CVar_SetFloat("gItemTrackerBgColorG", ChromaKeyBackground.y);
        CVar_SetFloat("gItemTrackerBgColorB", ChromaKeyBackground.z);
        CVar_SetFloat("gItemTrackerBgColorA", ChromaKeyBackground.w);
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::PopItemWidth();

    LabeledComboBoxRightAligned("Window Type", "gItemTrackerWindowType", { "Floating", "Window" }, 0);

    if (CVar_GetS32("gItemTrackerWindowType", 0) == 0) {
        PaddedEnhancementCheckbox("Enable Dragging", "gItemTrackerHudEditMode", 0);
        PaddedEnhancementCheckbox("Only enable while paused", "gItemTrackerShowOnlyPaused", 0);
        LabeledComboBoxRightAligned("Display Mode", "gItemTrackerDisplayType", { "Always", "Combo Button Hold" }, 0);
        if (CVar_GetS32("gItemTrackerDisplayType", 0) > 0) {
            LabeledComboBoxRightAligned("Combo Button 1", "gItemTrackerComboButton1", { "A", "B", "C-Up", "C-Down", "C-Left", "C-Right", "L", "Z", "R", "Start", "D-Up", "D-Down", "D-Left", "D-Right" }, 6);
            LabeledComboBoxRightAligned("Combo Button 2", "gItemTrackerComboButton2", { "A", "B", "C-Up", "C-Down", "C-Left", "C-Right", "L", "Z", "R", "Start", "D-Up", "D-Down", "D-Left", "D-Right" }, 8);
        }
    }
    UIWidgets::PaddedSeparator();
    UIWidgets::EnhancementSliderInt("Icon size : %dpx", "##ITEMTRACKERICONSIZE", "gItemTrackerIconSize", 25, 128, "", 36, true);
    UIWidgets::EnhancementSliderInt("Icon margins : %dpx", "##ITEMTRACKERSPACING", "gItemTrackerIconSpacing", -5, 50, "", 12, true);
    
    ImGui::Text("Ammo/Capacity Tracking");
    UIWidgets::EnhancementCombobox("gItemTrackerCapacityTrack", itemTrackerCapacityTrackOptions, 5, 1);
    UIWidgets::InsertHelpHoverText("Customize what the numbers under each item are tracking."
                                    "\n\nNote: items without capacity upgrades will track ammo even in capacity mode");
    if (CVar_GetS32("gItemTrackerCapacityTrack", 1) == ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY || CVar_GetS32("gItemTrackerCapacityTrack", 1) == ITEM_TRACKER_NUMBER_CURRENT_AMMO_ONLY) {
        PaddedEnhancementCheckbox("Align count to left side", "gItemTrackerCurrentOnLeft", 0);
    }

    ImGui::TableNextColumn();

    LabeledComboBoxRightAligned("Inventory", "gItemTrackerInventoryItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Equipment", "gItemTrackerEquipmentItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Misc", "gItemTrackerMiscItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Dungeon Rewards", "gItemTrackerDungeonRewardsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    if (CVar_GetS32("gItemTrackerDungeonRewardsDisplayType", 1) == 2) {
        PaddedEnhancementCheckbox("Circle display", "gItemTrackerDungeonRewardsCircle", 1);
    }
    LabeledComboBoxRightAligned("Songs", "gItemTrackerSongsDisplayType", { "Hidden", "Main Window", "Seperate" }, 1);
    LabeledComboBoxRightAligned("Dungeon Items", "gItemTrackerDungeonItemsDisplayType", { "Hidden", "Main Window", "Seperate" }, 0);
    if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 0) != 0) {
        if (CVar_GetS32("gItemTrackerDungeonItemsDisplayType", 0) == 2) {
            PaddedEnhancementCheckbox("Horizontal display", "gItemTrackerDisplayDungeonItemsHorizontal", 1);
        }
        PaddedEnhancementCheckbox("Maps and compasses", "gItemTrackerDisplayDungeonItemsMaps", 1);
    }

    if (CVar_GetS32("gItemTrackerDisplayType", 0) != 1) {
        LabeledComboBoxRightAligned("Personal notes", "gItemTrackerNotesDisplayType", { "Hidden", "Main Window", "Seperate" }, 0);
    }

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::End();
}

void InitItemTracker() {
    SohImGui::AddWindow("Randomizer", "Item Tracker", DrawItemTracker, CVar_GetS32("gItemTrackerEnabled", 0) == 1);
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
    // Crashes when the itemTrackerNotes is empty, so add an empty character to it
    if (itemTrackerNotes.empty()) {
        itemTrackerNotes.push_back(0);
    }
    Ship::RegisterHook<Ship::ControllerRead>([](OSContPad* cont_pad) {
        buttonsPressed = cont_pad;
    });
    Ship::RegisterHook<Ship::LoadFile>([](uint32_t fileNum) {
        const char* initialTrackerNotes = CVar_GetString(("gItemTrackerNotes" + std::to_string(fileNum)).c_str(), "");
        strcpy(itemTrackerNotes.Data, initialTrackerNotes);
    });
    Ship::RegisterHook<Ship::DeleteFile>([](uint32_t fileNum) {
        CVar_SetString(("gItemTrackerNotes" + std::to_string(fileNum)).c_str(), "");
        SohImGui::RequestCvarSaveOnNextTick();
    });
}

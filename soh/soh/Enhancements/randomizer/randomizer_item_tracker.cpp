#include "randomizer_item_tracker.h"
#include "../../util.h"
#include "../../OTRGlobals.h"
#include <ImGuiImpl.h>
#include "../../UIWidgets.hpp"

#include <map>
#include <string>
#include <vector>
#include <libultraship/bridge.h>
#include <Hooks.h>
#include <algorithm>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
}
extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);

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
    ITEM_TRACKER_ITEM(ITEM_HEART_CONTAINER, 0, DrawItem),     ITEM_TRACKER_ITEM(ITEM_HEART_PIECE, 0, DrawItem),         ITEM_TRACKER_ITEM(ITEM_MAGIC_SMALL, 0, DrawItem),
    ITEM_TRACKER_ITEM(QUEST_GERUDO_CARD, 1 << 22, DrawQuest), ITEM_TRACKER_ITEM(QUEST_SKULL_TOKEN, 1 << 23, DrawQuest), ITEM_TRACKER_ITEM(QUEST_STONE_OF_AGONY, 1 << 21, DrawQuest),
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

std::vector<ItemTrackerItem> gregItems = {
    ITEM_TRACKER_ITEM(ITEM_RUPEE_GREEN, 0, DrawItem),
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
            result.maxCapacity = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHOPSANITY) > RO_SHOPSANITY_ZERO_ITEMS ? 999 : 500;
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
        case ITEM_HEART_CONTAINER:
            result.maxCapacity = result.currentCapacity = 8;
            result.currentAmmo = gSaveContext.sohStats.heartContainers;
            break;
        case ITEM_HEART_PIECE:
            result.maxCapacity = result.currentCapacity = 36;
            result.currentAmmo = gSaveContext.sohStats.heartPieces;
            break;
        case ITEM_KEY_SMALL:
            // Though the ammo/capacity naming doesn't really make sense for keys, we are
            // hijacking the same system to display key counts as there are enough similarities
            result.currentAmmo = MAX(gSaveContext.inventory.dungeonKeys[item.data], 0);
            result.currentCapacity = gSaveContext.sohStats.dungeonKeys[item.data];
            switch (item.data) {
                case SCENE_BMORI1:
                    result.maxCapacity = FOREST_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_HIDAN:
                    result.maxCapacity = FIRE_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_MIZUSIN:
                    result.maxCapacity = WATER_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_JYASINZOU:
                    result.maxCapacity = SPIRIT_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_HAKADAN:
                    result.maxCapacity = SHADOW_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_HAKADANCH:
                    result.maxCapacity = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;
                    break;
                case SCENE_MEN:
                    result.maxCapacity = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX;
                    break;
                case SCENE_GERUDOWAY:
                    result.maxCapacity = GERUDO_FORTRESS_SMALL_KEY_MAX;
                    break;
                case SCENE_GANONTIKA:
                    result.maxCapacity = GANONS_CASTLE_SMALL_KEY_MAX;
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
#define IM_COL_PURPLE IM_COL32(180, 90, 200, 255)

void DrawItemCount(ItemTrackerItem item) {
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ItemTrackerNumbers currentAndMax = GetItemCurrentAndMax(item);
    ImVec2 p = ImGui::GetCursorScreenPos();
    int32_t trackerNumberDisplayMode = CVarGetInteger("gItemTrackerCapacityTrack", 1);
    int32_t trackerKeyNumberDisplayMode = CVarGetInteger("gItemTrackerKeyTrack", 0);

    if (item.id == ITEM_KEY_SMALL && IsValidSaveFile()) {
        std::string currentString = "";
        std::string maxString = std::to_string(currentAndMax.maxCapacity);
        ImU32 currentColor = IM_COL_WHITE;
        ImU32 maxColor = IM_COL_GREEN;
        // "Collected / Max", "Current / Collected / Max", "Current / Max"
        if (trackerKeyNumberDisplayMode == 1 || trackerKeyNumberDisplayMode == 2) {
            currentString+= std::to_string(currentAndMax.currentAmmo);
            currentString+= "/";
        }
        if (trackerKeyNumberDisplayMode == 0 || trackerKeyNumberDisplayMode == 1) {
            currentString+= std::to_string(currentAndMax.currentCapacity);
            currentString+= "/";
        }

        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize((currentString + maxString).c_str()).x / 2), p.y - 14));
        ImGui::PushStyleColor(ImGuiCol_Text, currentColor);
        ImGui::Text(currentString.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, maxColor);
        ImGui::Text(maxString.c_str());
        ImGui::PopStyleColor();
    } else if (currentAndMax.currentCapacity > 0 && trackerNumberDisplayMode != ITEM_TRACKER_NUMBER_NONE && IsValidSaveFile()) {
        std::string currentString = "";
        std::string maxString = "";
        ImU32 currentColor = IM_COL_WHITE;
        ImU32 maxColor = item.id == QUEST_SKULL_TOKEN ? IM_COL_RED : IM_COL_GREEN;

        bool shouldAlignToLeft = CVarGetInteger("gItemTrackerCurrentOnLeft", 0) &&
            trackerNumberDisplayMode != ITEM_TRACKER_NUMBER_CAPACITY &&
            trackerNumberDisplayMode != ITEM_TRACKER_NUMBER_AMMO;

        bool shouldDisplayAmmo = trackerNumberDisplayMode == ITEM_TRACKER_NUMBER_AMMO ||
            trackerNumberDisplayMode == ITEM_TRACKER_NUMBER_CURRENT_AMMO_ONLY ||
            // These items have a static capacity, so display ammo instead
            item.id == ITEM_BOMBCHU ||
            item.id == ITEM_BEAN ||
            item.id == QUEST_SKULL_TOKEN ||
            item.id == ITEM_HEART_CONTAINER ||
            item.id == ITEM_HEART_PIECE;

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
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ImGui::Image(Ship::GetTextureByName(hasEquip && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawQuest(ItemTrackerItem item) {
    bool hasQuestItem = (item.data & gSaveContext.inventory.questItems) != 0;
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ImGui::BeginGroup();
    ImGui::Image(Ship::GetTextureByName(hasQuestItem && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    if (item.id == QUEST_SKULL_TOKEN) {
        DrawItemCount(item);
    }

    ImGui::EndGroup();

    UIWidgets::SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
};

void DrawItem(ItemTrackerItem item) {
    uint32_t actualItemId = INV_CONTENT(item.id);
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    bool hasItem = actualItemId != ITEM_NONE;

    if (item.id == ITEM_NONE) {
        return;
    }

    switch (item.id) {
        case ITEM_HEART_CONTAINER:
            actualItemId = item.id;
            hasItem = gSaveContext.sohStats.heartContainers > 0;
            break;
        case ITEM_HEART_PIECE:
            actualItemId = item.id;
            hasItem = gSaveContext.sohStats.heartPieces > 0;
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
        case ITEM_RUPEE_GREEN:
            actualItemId = item.id;
            hasItem = Flags_GetRandomizerInf(RAND_INF_GREG_FOUND);
            break;
    }

    if (hasItem && item.id != actualItemId && actualItemTrackerItemMap.find(actualItemId) != actualItemTrackerItemMap.end()) {
        item = actualItemTrackerItemMap[actualItemId];
    }
    
    ImGui::BeginGroup();
    ImGui::Image(Ship::GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
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

    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ImGui::Image(Ship::GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
};

void DrawDungeonItem(ItemTrackerItem item) {
    uint32_t itemId = item.id;
    ImU32 dungeonColor = IM_COL_WHITE;
    uint32_t bitMask = 1 << (item.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[item.data]) != 0;
    bool hasSmallKey = (gSaveContext.inventory.dungeonKeys[item.data]) >= 0;
    ImGui::BeginGroup();
    if (itemId == ITEM_KEY_SMALL) {
        ImGui::Image(Ship::GetTextureByName(hasSmallKey && IsValidSaveFile() ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }
    else {
        ImGui::Image(Ship::GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }

    if (ResourceMgr_IsSceneMasterQuest(item.data) && (CHECK_DUNGEON_ITEM(DUNGEON_MAP, item.data) || item.data == SCENE_MEN || item.data == SCENE_GANONTIKA)) {
        dungeonColor = IM_COL_PURPLE;
    }

    if (itemId == ITEM_KEY_SMALL) {
        DrawItemCount(item);

        ImVec2 p = ImGui::GetCursorScreenPos();
        std::string dungeonName = itemTrackerDungeonShortNames[item.data];
        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(dungeonName.c_str()).x / 2), p.y - (iconSize + 16)));
        ImGui::PushStyleColor(ImGuiCol_Text, dungeonColor);
        ImGui::Text(dungeonName.c_str());
        ImGui::PopStyleColor();
    }

    if (itemId == ITEM_DUNGEON_MAP && 
        (item.data == SCENE_YDAN || item.data == SCENE_DDAN || item.data == SCENE_BDAN || item.data == SCENE_ICE_DOUKUTO)
    ) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        std::string dungeonName = itemTrackerDungeonShortNames[item.data];
        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(dungeonName.c_str()).x / 2), p.y - (iconSize + 13)));
        ImGui::PushStyleColor(ImGuiCol_Text, dungeonColor);
        ImGui::Text(dungeonName.c_str());
        ImGui::PopStyleColor();
    }
    ImGui::EndGroup();

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawSong(ItemTrackerItem item) {
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    uint32_t bitMask = 1 << item.id;
    bool hasSong = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(ImVec2(p.x + 6, p.y));
    ImGui::Image(Ship::GetTextureByName(hasSong && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize / 1.5, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    UIWidgets::SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
}

static ImVector<char> itemTrackerNotes;

void DrawNotes(bool resizeable = false) {
    ImGui::BeginGroup();
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    int iconSpacing = CVarGetInteger("gItemTrackerIconSpacing", 12);

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
        CVarSetString(("gItemTrackerNotes" + std::to_string(gSaveContext.fileNum)).c_str(), std::string(std::begin(itemTrackerNotes), std::end(itemTrackerNotes)).c_str());
        Ship::RequestCvarSaveOnNextTick();
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

    if (!CVarGetInteger("gItemTrackerWindowType", 0)) {
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!CVarGetInteger("gItemTrackerHudEditMode", 0)) {
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
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    int iconSpacing = CVarGetInteger("gItemTrackerIconSpacing", 12);
    int topPadding = CVarGetInteger("gItemTrackerWindowType", 0) ? 20 : 0;

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
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    int iconSpacing = CVarGetInteger("gItemTrackerIconSpacing", 12);

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
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    int iconSpacing = CVarGetInteger("gItemTrackerIconSpacing", 12);
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
/* ****************************************************** */

void UpdateVectors() {
    if  (!shouldUpdateVectors) {
        return;
    }

    dungeonRewards.clear();
    dungeonRewards.insert(dungeonRewards.end(), dungeonRewardStones.begin(), dungeonRewardStones.end());
    dungeonRewards.insert(dungeonRewards.end(), dungeonRewardMedallions.begin(), dungeonRewardMedallions.end());

    dungeonItems.clear();
    if (CVarGetInteger("gItemTrackerDisplayDungeonItemsHorizontal", 1) && CVarGetInteger("gItemTrackerDungeonItemsDisplayType", 0) == 2) {
        if (CVarGetInteger("gItemTrackerDisplayDungeonItemsMaps", 1)) {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsWithMapsHorizontal, 12);
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems[23] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_GERUDOWAY, DrawDungeonItem);
        } else {
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsHorizontal, 8);
            dungeonItems[15] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_GERUDOWAY, DrawDungeonItem);
        }
    } else {
        if (CVarGetInteger("gItemTrackerDisplayDungeonItemsMaps", 1)) {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsWithMapsCompact);
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems[35] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_GERUDOWAY, DrawDungeonItem);
        } else {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsCompact);
        }
    }

    mainWindowItems.clear();
    if (CVarGetInteger("gItemTrackerInventoryItemsDisplayType", 1) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), inventoryItems.begin(), inventoryItems.end());
    }
    if (CVarGetInteger("gItemTrackerEquipmentItemsDisplayType", 1) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), equipmentItems.begin(), equipmentItems.end());
    }
    if (CVarGetInteger("gItemTrackerMiscItemsDisplayType", 1) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), miscItems.begin(), miscItems.end());
    }
    if (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", 1) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), dungeonRewardStones.begin(), dungeonRewardStones.end());
        mainWindowItems.insert(mainWindowItems.end(), dungeonRewardMedallions.begin(), dungeonRewardMedallions.end());
    }
    if (CVarGetInteger("gItemTrackerSongsDisplayType", 1) == 1) {
        if (CVarGetInteger("gItemTrackerMiscItemsDisplayType", 1) == 1 && CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", 1) != 1) {
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }
        mainWindowItems.insert(mainWindowItems.end(), songItems.begin(), songItems.end());
    }
    if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", 0) == 1) {
        mainWindowItems.insert(mainWindowItems.end(), dungeonItems.begin(), dungeonItems.end());
    }

    // if we're adding greg to the misc window,
    // and misc isn't on the main window,
    // and it doesn't already have greg, add him
    if (CVarGetInteger("gItemTrackerGregDisplayType", 0) == 2 &&
        CVarGetInteger("gItemTrackerMiscItemsDisplayType", 1) != 1 &&
        std::none_of(miscItems.begin(), miscItems.end(), [](ItemTrackerItem item){return item.id == ITEM_RUPEE_GREEN;})) {
            
        miscItems.insert(miscItems.end(), gregItems.begin(), gregItems.end());
    } else {
        for (auto it = miscItems.begin(); it != miscItems.end();) {
            if (it->id == ITEM_RUPEE_GREEN) {
                miscItems.erase(it);
            } else {
                it++;
            }
        }
    }

    // if we're adding greg to the main window
    if (CVarGetInteger("gItemTrackerGregDisplayType", 0) == 1) {
        // insert empty items until we're on a new row for greg
        while (mainWindowItems.size() % 6) {
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }

        // add greg
        mainWindowItems.insert(mainWindowItems.end(), gregItems.begin(), gregItems.end());
    }

    shouldUpdateVectors = false;
}

void DrawItemTracker(bool& open) {
    UpdateVectors();
    if (!open) {
        CVarSetInteger("gItemTrackerEnabled", 0);
        return;
    }
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    int iconSpacing = CVarGetInteger("gItemTrackerIconSpacing", 12);
    int comboButton1Mask = buttonMap[CVarGetInteger("gItemTrackerComboButton1", 6)];
    int comboButton2Mask = buttonMap[CVarGetInteger("gItemTrackerComboButton2", 8)];
    bool comboButtonsHeld = buttonsPressed != nullptr && buttonsPressed[0].button & comboButton1Mask && buttonsPressed[0].button & comboButton2Mask;
    bool isPaused = CVarGetInteger("gItemTrackerShowOnlyPaused", 0) == 0 || gPlayState != nullptr && gPlayState->pauseCtx.state > 0;

    if (CVarGetInteger("gItemTrackerWindowType", 0) == 1 || isPaused && (CVarGetInteger("gItemTrackerDisplayType", 0) == 0 ? CVarGetInteger("gItemTrackerEnabled", 0) : comboButtonsHeld)) {
        if (
            (CVarGetInteger("gItemTrackerInventoryItemsDisplayType", 1) == 1) ||
            (CVarGetInteger("gItemTrackerEquipmentItemsDisplayType", 1) == 1) ||
            (CVarGetInteger("gItemTrackerMiscItemsDisplayType", 1) == 1) ||
            (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", 1) == 1) ||
            (CVarGetInteger("gItemTrackerSongsDisplayType", 1) == 1) ||
            (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", 0) == 1) ||
            (CVarGetInteger("gItemTrackerGregDisplayType", 0) == 1) ||
            (CVarGetInteger("gItemTrackerNotesDisplayType", 0) == 1)
        ) {
            BeginFloatingWindows("Item Tracker##main window");
            DrawItemsInRows(mainWindowItems, 6);

            if (CVarGetInteger("gItemTrackerNotesDisplayType", 0) == 1 && CVarGetInteger("gItemTrackerDisplayType", 0) == 0) {
                DrawNotes();
            }
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerInventoryItemsDisplayType", 1) == 2) {
            BeginFloatingWindows("Inventory Items Tracker");
            DrawItemsInRows(inventoryItems);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerEquipmentItemsDisplayType", 1) == 2) {
            BeginFloatingWindows("Equipment Items Tracker");
            DrawItemsInRows(equipmentItems, 3);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerMiscItemsDisplayType", 1) == 2) {
            BeginFloatingWindows("Misc Items Tracker");
            DrawItemsInRows(miscItems, 4);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", 1) == 2) {
            BeginFloatingWindows("Dungeon Rewards Tracker");
            if (CVarGetInteger("gItemTrackerDungeonRewardsCircle", 0) == 1) {
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

        if (CVarGetInteger("gItemTrackerSongsDisplayType", 1) == 2) {
            BeginFloatingWindows("Songs Tracker");
            DrawItemsInRows(songItems);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", 0) == 2) {
            BeginFloatingWindows("Dungeon Items Tracker");
            if (CVarGetInteger("gItemTrackerDisplayDungeonItemsHorizontal", 1)) {
                if (CVarGetInteger("gItemTrackerDisplayDungeonItemsMaps", 1)) {
                    DrawItemsInRows(dungeonItems, 12);
                } else {
                    DrawItemsInRows(dungeonItems, 8);
                }
            } else {
                DrawItemsInRows(dungeonItems);
            }
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerGregDisplayType", 0) == 3) {
            BeginFloatingWindows("Greg Tracker");
            DrawItemsInRows(gregItems);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerNotesDisplayType", 0) == 2 && CVarGetInteger("gItemTrackerDisplayType", 0) == 0) {
            ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
            BeginFloatingWindows("Personal Notes", ImGuiWindowFlags_NoFocusOnAppearing);
            DrawNotes(true);
            EndFloatingWindows();
        }
    }
}

static const char* itemTrackerCapacityTrackOptions[5] = { "No Numbers", "Current Capacity", "Current Ammo", "Current Capacity / Max Capacity", "Current Ammo / Current Capacity" };
static const char* itemTrackerKeyTrackOptions[3] = { "Collected / Max", "Current / Collected / Max", "Current / Max" };
static const char* windowTypes[2] = { "Floating", "Window" };
static const char* displayModes[2] = { "Always", "Combo Button Hold" };
static const char* buttons[14] = { "A", "B", "C-Up", "C-Down", "C-Left", "C-Right", "L", "Z", "R", "Start", "D-Up", "D-Down", "D-Left", "D-Right" };
static const char* displayTypes[3] = { "Hidden", "Main Window", "Seperate" };
static const char* extendedDisplayTypes[4] = { "Hidden", "Main Window", "Misc Window", "Seperate" };

void DrawItemTrackerOptions(bool& open) {
    if (!open) {
        CVarSetInteger("gItemTrackerSettingsEnabled", 0);
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
        CVarSetFloat("gItemTrackerBgColorR", ChromaKeyBackground.x);
        CVarSetFloat("gItemTrackerBgColorG", ChromaKeyBackground.y);
        CVarSetFloat("gItemTrackerBgColorB", ChromaKeyBackground.z);
        CVarSetFloat("gItemTrackerBgColorA", ChromaKeyBackground.w);
        Ship::RequestCvarSaveOnNextTick();
    }
    ImGui::PopItemWidth();

    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Window Type", "gItemTrackerWindowType", windowTypes, 0)) {
        shouldUpdateVectors = true;
    }

    if (CVarGetInteger("gItemTrackerWindowType", 0) == 0) {
        if (UIWidgets::PaddedEnhancementCheckbox("Enable Dragging", "gItemTrackerHudEditMode")) {
            shouldUpdateVectors = true;
        }
        if (UIWidgets::PaddedEnhancementCheckbox("Only enable while paused", "gItemTrackerShowOnlyPaused")) {
            shouldUpdateVectors = true;
        }
        if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Display Mode", "gItemTrackerDisplayType", displayModes, 0)) {
            shouldUpdateVectors = true;
        }
        if (CVarGetInteger("gItemTrackerDisplayType", 0) > 0) {
            if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Combo Button 1", "gItemTrackerComboButton1", buttons, 6)) {
                shouldUpdateVectors = true;
            }
            if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Combo Button 2", "gItemTrackerComboButton2", buttons, 8)) {
                shouldUpdateVectors = true;
            }
        }
    }
    UIWidgets::PaddedSeparator();
    UIWidgets::EnhancementSliderInt("Icon size : %dpx", "##ITEMTRACKERICONSIZE", "gItemTrackerIconSize", 25, 128, "", 36);
    UIWidgets::EnhancementSliderInt("Icon margins : %dpx", "##ITEMTRACKERSPACING", "gItemTrackerIconSpacing", -5, 50, "", 12);
    
    ImGui::Text("Ammo/Capacity Tracking");
    UIWidgets::EnhancementCombobox("gItemTrackerCapacityTrack", itemTrackerCapacityTrackOptions, 1);
    UIWidgets::InsertHelpHoverText("Customize what the numbers under each item are tracking."
                                    "\n\nNote: items without capacity upgrades will track ammo even in capacity mode");
    if (CVarGetInteger("gItemTrackerCapacityTrack", 1) == ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY || CVarGetInteger("gItemTrackerCapacityTrack", 1) == ITEM_TRACKER_NUMBER_CURRENT_AMMO_ONLY) {
        if (UIWidgets::PaddedEnhancementCheckbox("Align count to left side", "gItemTrackerCurrentOnLeft")) {
            shouldUpdateVectors = true;
        }
    }
    ImGui::Text("Key Count Tracking");
    UIWidgets::EnhancementCombobox("gItemTrackerKeyTrack", itemTrackerKeyTrackOptions, 0);
    UIWidgets::InsertHelpHoverText("Customize what numbers are shown for key tracking.");

    ImGui::TableNextColumn();

    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Inventory", "gItemTrackerInventoryItemsDisplayType", displayTypes, 1)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Equipment", "gItemTrackerEquipmentItemsDisplayType", displayTypes, 1)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Misc", "gItemTrackerMiscItemsDisplayType", displayTypes, 1)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Dungeon Rewards", "gItemTrackerDungeonRewardsDisplayType", displayTypes, 1)) {
        shouldUpdateVectors = true;
    }
    if (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", 1) == 2) {
        if (UIWidgets::PaddedEnhancementCheckbox("Circle display", "gItemTrackerDungeonRewardsCircle", true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true)) {
            shouldUpdateVectors = true;
        }
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Songs", "gItemTrackerSongsDisplayType", displayTypes, 1)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Dungeon Items", "gItemTrackerDungeonItemsDisplayType", displayTypes, 0)) {
        shouldUpdateVectors = true;
    }
    if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", 0) != 0) {
        if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", 0) == 2) {
            if (UIWidgets::PaddedEnhancementCheckbox("Horizontal display", "gItemTrackerDisplayDungeonItemsHorizontal", true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true)) {
                shouldUpdateVectors = true;
            }
        }
        if (UIWidgets::PaddedEnhancementCheckbox("Maps and compasses", "gItemTrackerDisplayDungeonItemsMaps", true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true)) {
            shouldUpdateVectors = true;
        }
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Greg", "gItemTrackerGregDisplayType", extendedDisplayTypes, 0)) {
        shouldUpdateVectors = true;
    }

    if (CVarGetInteger("gItemTrackerDisplayType", 0) != 1) {
        if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Personal notes", "gItemTrackerNotesDisplayType", displayTypes, 0)) {
            shouldUpdateVectors = true;
        }
    }

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::End();
}

void InitItemTracker() {
    Ship::AddWindow("Randomizer", "Item Tracker", DrawItemTracker, CVarGetInteger("gItemTrackerEnabled", 0) == 1);
    Ship::AddWindow("Randomizer", "Item Tracker Settings", DrawItemTrackerOptions);
    float trackerBgR = CVarGetFloat("gItemTrackerBgColorR", 0);
    float trackerBgG = CVarGetFloat("gItemTrackerBgColorG", 0);
    float trackerBgB = CVarGetFloat("gItemTrackerBgColorB", 0);
    float trackerBgA = CVarGetFloat("gItemTrackerBgColorA", 1);
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
        const char* initialTrackerNotes = CVarGetString(("gItemTrackerNotes" + std::to_string(fileNum)).c_str(), "");
        itemTrackerNotes.resize(strlen(initialTrackerNotes) + 1);
        strcpy(itemTrackerNotes.Data, initialTrackerNotes);
    });
    Ship::RegisterHook<Ship::DeleteFile>([](uint32_t fileNum) {
        CVarSetString(("gItemTrackerNotes" + std::to_string(fileNum)).c_str(), "");
        Ship::RequestCvarSaveOnNextTick();
    });
}

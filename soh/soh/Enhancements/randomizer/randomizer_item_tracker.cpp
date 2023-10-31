#include "randomizer_item_tracker.h"
#include "../../util.h"
#include "../../OTRGlobals.h"
#include "../../UIWidgets.hpp"
#include "randomizerTypes.h"

#include <map>
#include <string>
#include <vector>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
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

std::vector<ItemTrackerItem> triforcePieces = {
    ITEM_TRACKER_ITEM(RG_TRIFORCE_PIECE, 0, DrawItem),
};

std::vector<ItemTrackerDungeon> itemTrackerDungeonsWithMapsHorizontal = {
    { SCENE_DEKU_TREE, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_DODONGOS_CAVERN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_JABU_JABU, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_FOREST_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_FIRE_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_WATER_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_SPIRIT_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_SHADOW_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_INSIDE_GANONS_CASTLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_BOTTOM_OF_THE_WELL, { ITEM_KEY_SMALL, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_ICE_CAVERN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_GERUDO_TRAINING_GROUND, { ITEM_KEY_SMALL } },
};

std::vector<ItemTrackerDungeon> itemTrackerDungeonsHorizontal = {
    { SCENE_FOREST_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_FIRE_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_WATER_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_SPIRIT_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_SHADOW_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_INSIDE_GANONS_CASTLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_BOTTOM_OF_THE_WELL, { ITEM_KEY_SMALL } },
    { SCENE_GERUDO_TRAINING_GROUND, { ITEM_KEY_SMALL } },
};


std::vector<ItemTrackerDungeon> itemTrackerDungeonsWithMapsCompact = {
    { SCENE_FOREST_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_FIRE_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_WATER_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_SPIRIT_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_SHADOW_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_BOTTOM_OF_THE_WELL, { ITEM_KEY_SMALL, ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_DEKU_TREE, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_DODONGOS_CAVERN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_JABU_JABU, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_ICE_CAVERN, { ITEM_DUNGEON_MAP, ITEM_COMPASS } },
    { SCENE_INSIDE_GANONS_CASTLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_GERUDO_TRAINING_GROUND, { ITEM_KEY_SMALL } },
};

std::vector<ItemTrackerDungeon> itemTrackerDungeonsCompact = {
    { SCENE_FOREST_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_FIRE_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_WATER_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_SPIRIT_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_SHADOW_TEMPLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_INSIDE_GANONS_CASTLE, { ITEM_KEY_SMALL, ITEM_KEY_BOSS } },
    { SCENE_BOTTOM_OF_THE_WELL, { ITEM_KEY_SMALL } },
    { SCENE_GERUDO_TRAINING_GROUND, { ITEM_KEY_SMALL } },
    { SCENE_THIEVES_HIDEOUT, { ITEM_KEY_SMALL } },
};

std::map<uint16_t, std::string> itemTrackerDungeonShortNames = {
    { SCENE_FOREST_TEMPLE, "FRST" },
    { SCENE_FIRE_TEMPLE, "FIRE" },
    { SCENE_WATER_TEMPLE, "WATR" },
    { SCENE_SPIRIT_TEMPLE, "SPRT" },
    { SCENE_SHADOW_TEMPLE, "SHDW" },
    { SCENE_BOTTOM_OF_THE_WELL, "BOTW" },
    { SCENE_DEKU_TREE, "DEKU" },
    { SCENE_DODONGOS_CAVERN, "DCVN" },
    { SCENE_JABU_JABU, "JABU" },
    { SCENE_ICE_CAVERN, "ICE" },
    { SCENE_INSIDE_GANONS_CASTLE, "GANON" },
    { SCENE_GERUDO_TRAINING_GROUND, "GTG" },
    { SCENE_THIEVES_HIDEOUT, "HIDE" },
};

std::vector<ItemTrackerItem> dungeonItems = {};

std::unordered_map<uint32_t, ItemTrackerItem> actualItemTrackerItemMap = {
    { ITEM_BOTTLE,           ITEM_TRACKER_ITEM(ITEM_BOTTLE, 0, DrawItem) },
    { ITEM_BIG_POE,          ITEM_TRACKER_ITEM(ITEM_BIG_POE, 0, DrawItem) },
    { ITEM_BLUE_FIRE,        ITEM_TRACKER_ITEM(ITEM_BLUE_FIRE, 0, DrawItem) },
    { ITEM_BUG,              ITEM_TRACKER_ITEM(ITEM_BUG, 0, DrawItem) },
    { ITEM_FAIRY,            ITEM_TRACKER_ITEM(ITEM_FAIRY, 0, DrawItem) },
    { ITEM_FISH,             ITEM_TRACKER_ITEM(ITEM_FISH, 0, DrawItem) },
    { ITEM_POTION_GREEN,     ITEM_TRACKER_ITEM(ITEM_POTION_GREEN, 0, DrawItem) },
    { ITEM_POE,              ITEM_TRACKER_ITEM(ITEM_POE, 0, DrawItem) },
    { ITEM_POTION_RED,       ITEM_TRACKER_ITEM(ITEM_POTION_RED, 0, DrawItem) },
    { ITEM_POTION_BLUE,      ITEM_TRACKER_ITEM(ITEM_POTION_BLUE, 0, DrawItem) },
    { ITEM_MILK_BOTTLE,      ITEM_TRACKER_ITEM(ITEM_MILK_BOTTLE, 0, DrawItem) },
    { ITEM_MILK_HALF,        ITEM_TRACKER_ITEM(ITEM_MILK_HALF, 0, DrawItem) },
    { ITEM_LETTER_RUTO,      ITEM_TRACKER_ITEM(ITEM_LETTER_RUTO, 0, DrawItem) },

    { ITEM_HOOKSHOT,         ITEM_TRACKER_ITEM(ITEM_HOOKSHOT, 0, DrawItem) },
    { ITEM_LONGSHOT,         ITEM_TRACKER_ITEM(ITEM_LONGSHOT, 0, DrawItem) },

    { ITEM_OCARINA_FAIRY,    ITEM_TRACKER_ITEM(ITEM_OCARINA_FAIRY, 0, DrawItem) },
    { ITEM_OCARINA_TIME,     ITEM_TRACKER_ITEM(ITEM_OCARINA_TIME, 0, DrawItem) },

    { ITEM_MAGIC_SMALL,      ITEM_TRACKER_ITEM(ITEM_MAGIC_SMALL, 0, DrawItem) },
    { ITEM_MAGIC_LARGE,      ITEM_TRACKER_ITEM(ITEM_MAGIC_LARGE, 0, DrawItem) },

    { ITEM_WALLET_ADULT,     ITEM_TRACKER_ITEM(ITEM_WALLET_ADULT, 0, DrawItem) },
    { ITEM_WALLET_GIANT,     ITEM_TRACKER_ITEM(ITEM_WALLET_GIANT, 0, DrawItem) },

    { ITEM_BRACELET,         ITEM_TRACKER_ITEM(ITEM_BRACELET, 0, DrawItem) },
    { ITEM_GAUNTLETS_SILVER, ITEM_TRACKER_ITEM(ITEM_GAUNTLETS_SILVER, 0, DrawItem) },
    { ITEM_GAUNTLETS_GOLD,   ITEM_TRACKER_ITEM(ITEM_GAUNTLETS_GOLD, 0, DrawItem) },

    { ITEM_SCALE_SILVER,     ITEM_TRACKER_ITEM(ITEM_SCALE_SILVER, 0, DrawItem) },
    { ITEM_SCALE_GOLDEN,     ITEM_TRACKER_ITEM(ITEM_SCALE_GOLDEN, 0, DrawItem) },

    { ITEM_WEIRD_EGG,        ITEM_TRACKER_ITEM(ITEM_WEIRD_EGG, 0, DrawItem) },
    { ITEM_CHICKEN,          ITEM_TRACKER_ITEM(ITEM_CHICKEN, 0, DrawItem) },
    { ITEM_LETTER_ZELDA,     ITEM_TRACKER_ITEM(ITEM_LETTER_ZELDA, 0, DrawItem) },
    { ITEM_MASK_KEATON,      ITEM_TRACKER_ITEM(ITEM_MASK_KEATON, 0, DrawItem) },
    { ITEM_MASK_SKULL,       ITEM_TRACKER_ITEM(ITEM_MASK_SKULL, 0, DrawItem) },
    { ITEM_MASK_SPOOKY,      ITEM_TRACKER_ITEM(ITEM_MASK_SPOOKY, 0, DrawItem) },
    { ITEM_MASK_BUNNY,       ITEM_TRACKER_ITEM(ITEM_MASK_BUNNY, 0, DrawItem) },
    { ITEM_MASK_GORON,       ITEM_TRACKER_ITEM(ITEM_MASK_GORON, 0, DrawItem) },
    { ITEM_MASK_ZORA,        ITEM_TRACKER_ITEM(ITEM_MASK_ZORA, 0, DrawItem) },
    { ITEM_MASK_GERUDO,      ITEM_TRACKER_ITEM(ITEM_MASK_GERUDO, 0, DrawItem) },
    { ITEM_MASK_TRUTH,       ITEM_TRACKER_ITEM(ITEM_MASK_TRUTH, 0, DrawItem) },
    { ITEM_SOLD_OUT,         ITEM_TRACKER_ITEM(ITEM_SOLD_OUT, 0, DrawItem) },

    { ITEM_POCKET_EGG,       ITEM_TRACKER_ITEM(ITEM_POCKET_EGG, 0, DrawItem) },
    { ITEM_POCKET_CUCCO,     ITEM_TRACKER_ITEM(ITEM_POCKET_CUCCO, 0, DrawItem) },
    { ITEM_COJIRO,           ITEM_TRACKER_ITEM(ITEM_COJIRO, 0, DrawItem) },
    { ITEM_ODD_MUSHROOM,     ITEM_TRACKER_ITEM(ITEM_ODD_MUSHROOM, 0, DrawItem) },
    { ITEM_ODD_POTION,       ITEM_TRACKER_ITEM(ITEM_ODD_POTION, 0, DrawItem) },
    { ITEM_SAW,              ITEM_TRACKER_ITEM(ITEM_SAW, 0, DrawItem) },
    { ITEM_SWORD_BROKEN,     ITEM_TRACKER_ITEM(ITEM_SWORD_BROKEN, 0, DrawItem) },
    { ITEM_PRESCRIPTION,     ITEM_TRACKER_ITEM(ITEM_PRESCRIPTION, 0, DrawItem) },
    { ITEM_FROG,             ITEM_TRACKER_ITEM(ITEM_FROG, 0, DrawItem) },
    { ITEM_EYEDROPS,         ITEM_TRACKER_ITEM(ITEM_EYEDROPS, 0, DrawItem) },
    { ITEM_CLAIM_CHECK,      ITEM_TRACKER_ITEM(ITEM_CLAIM_CHECK, 0, DrawItem) },
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

typedef enum {
    KEYS_COLLECTED_MAX,
    KEYS_CURRENT_COLLECTED_MAX,
    KEYS_CURRENT_MAX
} ItemTrackerKeysNumberOption;

typedef enum {
    TRIFORCE_PIECE_COLLECTED_REQUIRED,
    TRIFORCE_PIECE_COLLECTED_REQUIRED_MAX
} ItemTrackerTriforcePieceNumberOption;

typedef enum {
    SECTION_DISPLAY_HIDDEN,
    SECTION_DISPLAY_MAIN_WINDOW,
    SECTION_DISPLAY_SEPARATE
} ItemTrackerDisplayType;

typedef enum {
    SECTION_DISPLAY_EXTENDED_HIDDEN,
    SECTION_DISPLAY_EXTENDED_MAIN_WINDOW,
    SECTION_DISPLAY_EXTENDED_MISC_WINDOW,
    SECTION_DISPLAY_EXTENDED_SEPARATE
} ItemTrackerExtendedDisplayType;

struct ItemTrackerNumbers {
  int currentCapacity;
  int maxCapacity;
  int currentAmmo;
};

static ImVector<char> itemTrackerNotes;
uint32_t notesIdleFrames = 0;
bool notesNeedSave = false;
const uint32_t notesMaxIdleFrames = 40; // two seconds of game time, since OnGameFrameUpdate is used to tick

void ItemTrackerOnFrame() {
    if (notesNeedSave && notesIdleFrames <= notesMaxIdleFrames) {
        notesIdleFrames++;
    }
}

void SaveNotes(uint32_t fileNum) {
    CVarSetString(("gItemTrackerNotes" + std::to_string(fileNum)).c_str(), std::string(std::begin(itemTrackerNotes), std::end(itemTrackerNotes)).c_str());
    LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
}

bool IsValidSaveFile() {
    bool validSave = gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2;
    return validSave;
}

bool HasSong(ItemTrackerItem item) {
    return (1 << item.id) & gSaveContext.inventory.questItems;
}

bool HasQuestItem(ItemTrackerItem item) {
    return (item.data & gSaveContext.inventory.questItems) != 0;
}

bool HasEquipment(ItemTrackerItem item) {
    return (item.data & gSaveContext.inventory.equipment) != 0;
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
                case SCENE_FOREST_TEMPLE:
                    result.maxCapacity = FOREST_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_FIRE_TEMPLE:
                    result.maxCapacity = FIRE_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_WATER_TEMPLE:
                    result.maxCapacity = WATER_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_SPIRIT_TEMPLE:
                    result.maxCapacity = SPIRIT_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_SHADOW_TEMPLE:
                    result.maxCapacity = SHADOW_TEMPLE_SMALL_KEY_MAX;
                    break;
                case SCENE_BOTTOM_OF_THE_WELL:
                    result.maxCapacity = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;
                    break;
                case SCENE_GERUDO_TRAINING_GROUND:
                    result.maxCapacity = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX;
                    break;
                case SCENE_THIEVES_HIDEOUT:
                    result.maxCapacity = GERUDO_FORTRESS_SMALL_KEY_MAX;
                    break;
                case SCENE_INSIDE_GANONS_CASTLE:
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
    int32_t trackerNumberDisplayMode = CVarGetInteger("gItemTrackerCapacityTrack", ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY);
    int32_t trackerKeyNumberDisplayMode = CVarGetInteger("gItemTrackerKeyTrack", KEYS_COLLECTED_MAX);

    if (item.id == ITEM_KEY_SMALL && IsValidSaveFile()) {
        std::string currentString = "";
        std::string maxString = std::to_string(currentAndMax.maxCapacity);
        ImU32 currentColor = IM_COL_WHITE;
        ImU32 maxColor = IM_COL_GREEN;
        // "Collected / Max", "Current / Collected / Max", "Current / Max"
        if (trackerKeyNumberDisplayMode == KEYS_CURRENT_COLLECTED_MAX || trackerKeyNumberDisplayMode == KEYS_CURRENT_MAX) {
            currentString+= std::to_string(currentAndMax.currentAmmo);
            currentString+= "/";
        }
        if (trackerKeyNumberDisplayMode == KEYS_COLLECTED_MAX || trackerKeyNumberDisplayMode == KEYS_CURRENT_COLLECTED_MAX) {
            currentString+= std::to_string(currentAndMax.currentCapacity);
            currentString+= "/";
        }

        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize((currentString + maxString).c_str()).x / 2), p.y - 14));
        ImGui::PushStyleColor(ImGuiCol_Text, currentColor);
        ImGui::Text("%s", currentString.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, maxColor);
        ImGui::Text("%s", maxString.c_str());
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
        ImGui::Text("%s", currentString.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, maxColor);
        ImGui::Text("%s", maxString.c_str());
        ImGui::PopStyleColor();
    } else if (item.id == RG_TRIFORCE_PIECE && IS_RANDO &&
               OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT) && IsValidSaveFile()) {
        std::string currentString = "";
        std::string requiredString = "";
        std::string maxString = "";
        uint8_t piecesRequired = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED);
        uint8_t piecesTotal = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_TOTAL);
        ImU32 currentColor = gSaveContext.triforcePiecesCollected >= piecesRequired ? IM_COL_GREEN : IM_COL_WHITE;
        ImU32 maxColor = IM_COL_GREEN;
        int32_t trackerTriforcePieceNumberDisplayMode = CVarGetInteger("gItemTrackerTriforcePieceTrack", TRIFORCE_PIECE_COLLECTED_REQUIRED_MAX);

        currentString += std::to_string(gSaveContext.triforcePiecesCollected);
        currentString += "/";
        // gItemTrackerTriforcePieceTrack
        if (trackerTriforcePieceNumberDisplayMode == TRIFORCE_PIECE_COLLECTED_REQUIRED_MAX) {
            currentString += std::to_string(piecesRequired);
            currentString += "/";
            maxString += std::to_string(piecesTotal);
        } else if (trackerTriforcePieceNumberDisplayMode == TRIFORCE_PIECE_COLLECTED_REQUIRED) {
            maxString += std::to_string(piecesRequired);
        }

        ImGui::SetCursorScreenPos(
            ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize((currentString + maxString).c_str()).x / 2), p.y - 14));
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
    bool hasEquip = HasEquipment(item);
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasEquip && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawQuest(ItemTrackerItem item) {
    bool hasQuestItem = HasQuestItem(item);
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ImGui::BeginGroup();
    ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasQuestItem && IsValidSaveFile() ? item.name : item.nameFaded),
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
    std::string itemName = "";

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
        case RG_TRIFORCE_PIECE:
            actualItemId = item.id;
            hasItem = IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT);
            itemName = "Triforce Piece";
            break;
    }

    if (GameInteractor::IsSaveLoaded() && (hasItem && item.id != actualItemId && actualItemTrackerItemMap.find(actualItemId) != actualItemTrackerItemMap.end())) {
        item = actualItemTrackerItemMap[actualItemId];
    }
    
    ImGui::BeginGroup();

    ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    
    DrawItemCount(item);
    ImGui::EndGroup();

    if (itemName == "") {
        itemName = SohUtils::GetItemName(item.id);
    }

    UIWidgets::SetLastItemHoverText(itemName);
}

void DrawBottle(ItemTrackerItem item) {
    uint32_t actualItemId = gSaveContext.inventory.items[SLOT(item.id) + item.data];
    bool hasItem = actualItemId != ITEM_NONE;

    if (GameInteractor::IsSaveLoaded() && (hasItem && item.id != actualItemId && actualItemTrackerItemMap.find(actualItemId) != actualItemTrackerItemMap.end())) {
        item = actualItemTrackerItemMap[actualItemId];
    }

    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
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
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasSmallKey && IsValidSaveFile() ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }
    else {
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasItem && IsValidSaveFile() ? item.name : item.nameFaded),
                     ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    }

    if (ResourceMgr_IsSceneMasterQuest(item.data) && (CHECK_DUNGEON_ITEM(DUNGEON_MAP, item.data) || item.data == SCENE_GERUDO_TRAINING_GROUND || item.data == SCENE_INSIDE_GANONS_CASTLE)) {
        dungeonColor = IM_COL_PURPLE;
    }

    if (itemId == ITEM_KEY_SMALL) {
        DrawItemCount(item);

        ImVec2 p = ImGui::GetCursorScreenPos();
        std::string dungeonName = itemTrackerDungeonShortNames[item.data];
        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(dungeonName.c_str()).x / 2), p.y - (iconSize + 16)));
        ImGui::PushStyleColor(ImGuiCol_Text, dungeonColor);
        ImGui::Text("%s", dungeonName.c_str());
        ImGui::PopStyleColor();
    }

    if (itemId == ITEM_DUNGEON_MAP && 
        (item.data == SCENE_DEKU_TREE || item.data == SCENE_DODONGOS_CAVERN || item.data == SCENE_JABU_JABU || item.data == SCENE_ICE_CAVERN)
    ) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        std::string dungeonName = itemTrackerDungeonShortNames[item.data];
        ImGui::SetCursorScreenPos(ImVec2(p.x + (iconSize / 2) - (ImGui::CalcTextSize(dungeonName.c_str()).x / 2), p.y - (iconSize + 13)));
        ImGui::PushStyleColor(ImGuiCol_Text, dungeonColor);
        ImGui::Text("%s", dungeonName.c_str());
        ImGui::PopStyleColor();
    }
    ImGui::EndGroup();

    UIWidgets::SetLastItemHoverText(SohUtils::GetItemName(item.id));
}

void DrawSong(ItemTrackerItem item) {
    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    ImVec2 p = ImGui::GetCursorScreenPos();
    bool hasSong = HasSong(item);
    ImGui::SetCursorScreenPos(ImVec2(p.x + 6, p.y));
    ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(hasSong && IsValidSaveFile() ? item.name : item.nameFaded),
                 ImVec2(iconSize / 1.5, iconSize), ImVec2(0, 0), ImVec2(1, 1));
    UIWidgets::SetLastItemHoverText(SohUtils::GetQuestItemName(item.id));
}

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
    if (ItemTrackerNotes::TrackerNotesInputTextMultiline("##ItemTrackerNotes", &itemTrackerNotes, size, ImGuiInputTextFlags_AllowTabInput)) {
        notesNeedSave = true;
        notesIdleFrames = 0;
    }
    if ((ImGui::IsItemDeactivatedAfterEdit() || (notesNeedSave && notesIdleFrames > notesMaxIdleFrames)) && IsValidSaveFile()) {
        notesNeedSave = false;
        SaveNotes(gSaveContext.fileNum);
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

    if (CVarGetInteger("gItemTrackerWindowType", TRACKER_WINDOW_FLOATING) == TRACKER_WINDOW_FLOATING) {
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
    int topPadding = (CVarGetInteger("gItemTrackerWindowType", TRACKER_WINDOW_FLOATING) == TRACKER_WINDOW_WINDOW) ? 20 : 0;

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
                        if (dungeons[j].id == SCENE_INSIDE_GANONS_CASTLE) {
                            dungeonItems.push_back(ITEM_TRACKER_ITEM(ITEM_KEY_BOSS, SCENE_GANONS_TOWER, DrawDungeonItem));
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
    if (CVarGetInteger("gItemTrackerDisplayDungeonItemsHorizontal", 1) && CVarGetInteger("gItemTrackerDungeonItemsDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_SEPARATE) {
        if (CVarGetInteger("gItemTrackerDisplayDungeonItemsMaps", 1)) {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsWithMapsHorizontal, 12);
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems[23] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_THIEVES_HIDEOUT, DrawDungeonItem);
        } else {
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsHorizontal, 8);
            dungeonItems[15] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_THIEVES_HIDEOUT, DrawDungeonItem);
        }
    } else {
        if (CVarGetInteger("gItemTrackerDisplayDungeonItemsMaps", 1)) {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsWithMapsCompact);
            // Manually adding Thieves Hideout to an open spot so we don't get an additional row for one item
            dungeonItems[35] = ITEM_TRACKER_ITEM(ITEM_KEY_SMALL, SCENE_THIEVES_HIDEOUT, DrawDungeonItem);
        } else {
            dungeonItems = GetDungeonItemsVector(itemTrackerDungeonsCompact);
        }
    }

    mainWindowItems.clear();
    if (CVarGetInteger("gItemTrackerInventoryItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) {
        mainWindowItems.insert(mainWindowItems.end(), inventoryItems.begin(), inventoryItems.end());
    }
    if (CVarGetInteger("gItemTrackerEquipmentItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) {
        mainWindowItems.insert(mainWindowItems.end(), equipmentItems.begin(), equipmentItems.end());
    }
    if (CVarGetInteger("gItemTrackerMiscItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) {
        mainWindowItems.insert(mainWindowItems.end(), miscItems.begin(), miscItems.end());
    }
    if (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) {
        mainWindowItems.insert(mainWindowItems.end(), dungeonRewardStones.begin(), dungeonRewardStones.end());
        mainWindowItems.insert(mainWindowItems.end(), dungeonRewardMedallions.begin(), dungeonRewardMedallions.end());
    }
    if (CVarGetInteger("gItemTrackerSongsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) {
        if (CVarGetInteger("gItemTrackerMiscItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW &&
            CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) != SECTION_DISPLAY_MAIN_WINDOW) {
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }
        mainWindowItems.insert(mainWindowItems.end(), songItems.begin(), songItems.end());
    }
    if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_MAIN_WINDOW) {
        mainWindowItems.insert(mainWindowItems.end(), dungeonItems.begin(), dungeonItems.end());
    }

    // if we're adding greg to the misc window,
    // and misc isn't on the main window,
    // and it doesn't already have greg, add him
    if (CVarGetInteger("gItemTrackerGregDisplayType", SECTION_DISPLAY_EXTENDED_HIDDEN) == SECTION_DISPLAY_EXTENDED_MISC_WINDOW &&
        CVarGetInteger("gItemTrackerMiscItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) != SECTION_DISPLAY_MAIN_WINDOW &&
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
    if (CVarGetInteger("gItemTrackerGregDisplayType", SECTION_DISPLAY_EXTENDED_HIDDEN) == SECTION_DISPLAY_EXTENDED_MAIN_WINDOW) {
        // insert empty items until we're on a new row for greg
        while (mainWindowItems.size() % 6) {
            mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
        }

        // add greg
        mainWindowItems.insert(mainWindowItems.end(), gregItems.begin(), gregItems.end());
    }

    // If we're adding triforce pieces to the main window
    if (CVarGetInteger("gItemTrackerTriforcePiecesDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_MAIN_WINDOW) {
        // If Greg isn't on the main window, add empty items to place the triforce pieces on a new row.
        if (CVarGetInteger("gItemTrackerGregDisplayType", SECTION_DISPLAY_EXTENDED_HIDDEN) != SECTION_DISPLAY_EXTENDED_MAIN_WINDOW) {
            while (mainWindowItems.size() % 6) {
                mainWindowItems.push_back(ITEM_TRACKER_ITEM(ITEM_NONE, 0, DrawItem));
            }
        }

        // Add triforce pieces
        mainWindowItems.insert(mainWindowItems.end(), triforcePieces.begin(), triforcePieces.end());
    }

    shouldUpdateVectors = false;
}

void ItemTrackerWindow::DrawElement() {
    UpdateVectors();

    int iconSize = CVarGetInteger("gItemTrackerIconSize", 36);
    int iconSpacing = CVarGetInteger("gItemTrackerIconSpacing", 12);
    int comboButton1Mask = buttonMap[CVarGetInteger("gItemTrackerComboButton1", TRACKER_COMBO_BUTTON_L)];
    int comboButton2Mask = buttonMap[CVarGetInteger("gItemTrackerComboButton2", TRACKER_COMBO_BUTTON_R)];
    OSContPad* buttonsPressed = LUS::Context::GetInstance()->GetControlDeck()->GetPads();
    bool comboButtonsHeld = buttonsPressed != nullptr && buttonsPressed[0].button & comboButton1Mask && buttonsPressed[0].button & comboButton2Mask;
    bool isPaused = CVarGetInteger("gItemTrackerShowOnlyPaused", 0) == 0 || gPlayState != nullptr && gPlayState->pauseCtx.state > 0;

    if (CVarGetInteger("gItemTrackerWindowType", TRACKER_WINDOW_FLOATING) == TRACKER_WINDOW_WINDOW || isPaused && (CVarGetInteger("gItemTrackerDisplayType", TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_ALWAYS ? CVarGetInteger("gItemTrackerEnabled", 0) : comboButtonsHeld)) {
        if (
            (CVarGetInteger("gItemTrackerInventoryItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerEquipmentItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerMiscItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerSongsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerGregDisplayType", SECTION_DISPLAY_EXTENDED_HIDDEN) == SECTION_DISPLAY_EXTENDED_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerTriforcePiecesDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_MAIN_WINDOW) ||
            (CVarGetInteger("gItemTrackerNotesDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_MAIN_WINDOW)
        ) {
            BeginFloatingWindows("Item Tracker##main window");
            DrawItemsInRows(mainWindowItems, 6);

            if (CVarGetInteger("gItemTrackerNotesDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_MAIN_WINDOW && CVarGetInteger("gItemTrackerDisplayType", TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_ALWAYS) {
                DrawNotes();
            }
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerInventoryItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_SEPARATE) {
            BeginFloatingWindows("Inventory Items Tracker");
            DrawItemsInRows(inventoryItems);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerEquipmentItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_SEPARATE) {
            BeginFloatingWindows("Equipment Items Tracker");
            DrawItemsInRows(equipmentItems, 3);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerMiscItemsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_SEPARATE) {
            BeginFloatingWindows("Misc Items Tracker");
            DrawItemsInRows(miscItems, 4);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_SEPARATE) {
            BeginFloatingWindows("Dungeon Rewards Tracker");
            if (CVarGetInteger("gItemTrackerDungeonRewardsCircle", 0)) {
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

        if (CVarGetInteger("gItemTrackerSongsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_SEPARATE) {
            BeginFloatingWindows("Songs Tracker");
            DrawItemsInRows(songItems);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_SEPARATE) {
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

        if (CVarGetInteger("gItemTrackerGregDisplayType", SECTION_DISPLAY_EXTENDED_HIDDEN) == SECTION_DISPLAY_EXTENDED_SEPARATE) {
            BeginFloatingWindows("Greg Tracker");
            DrawItemsInRows(gregItems);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerTriforcePiecesDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_SEPARATE) {
            BeginFloatingWindows("Triforce Piece Tracker");
            DrawItemsInRows(triforcePieces);
            EndFloatingWindows();
        }

        if (CVarGetInteger("gItemTrackerNotesDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_SEPARATE && CVarGetInteger("gItemTrackerDisplayType", TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_ALWAYS) {
            ImGui::SetNextWindowSize(ImVec2(400,300), ImGuiCond_FirstUseEver);
            BeginFloatingWindows("Personal Notes", ImGuiWindowFlags_NoFocusOnAppearing);
            DrawNotes(true);
            EndFloatingWindows();
        }
    }
}

static const char* itemTrackerCapacityTrackOptions[5] = { "No Numbers", "Current Capacity", "Current Ammo", "Current Capacity / Max Capacity", "Current Ammo / Current Capacity" };
static const char* itemTrackerKeyTrackOptions[3] = { "Collected / Max", "Current / Collected / Max", "Current / Max" };
static const char* itemTrackerTriforcePieceTrackOptions[2] = { "Collected / Required", "Collected / Required / Max" };
static const char* windowTypes[2] = { "Floating", "Window" };
static const char* displayModes[2] = { "Always", "Combo Button Hold" };
static const char* buttons[14] = { "A", "B", "C-Up", "C-Down", "C-Left", "C-Right", "L", "Z", "R", "Start", "D-Up", "D-Down", "D-Left", "D-Right" };
static const char* displayTypes[3] = { "Hidden", "Main Window", "Separate" };
static const char* extendedDisplayTypes[4] = { "Hidden", "Main Window", "Misc Window", "Separate" };

void ItemTrackerSettingsWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(733, 472), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Item Tracker Settings", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
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
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    ImGui::PopItemWidth();

    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Window Type", "gItemTrackerWindowType", windowTypes, TRACKER_WINDOW_FLOATING)) {
        shouldUpdateVectors = true;
    }

    if (CVarGetInteger("gItemTrackerWindowType", TRACKER_WINDOW_FLOATING) == TRACKER_WINDOW_FLOATING) {
        if (UIWidgets::PaddedEnhancementCheckbox("Enable Dragging", "gItemTrackerHudEditMode")) {
            shouldUpdateVectors = true;
        }
        if (UIWidgets::PaddedEnhancementCheckbox("Only enable while paused", "gItemTrackerShowOnlyPaused")) {
            shouldUpdateVectors = true;
        }
        if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Display Mode", "gItemTrackerDisplayType", displayModes, TRACKER_DISPLAY_ALWAYS)) {
            shouldUpdateVectors = true;
        }
        if (CVarGetInteger("gItemTrackerDisplayType", TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Combo Button 1", "gItemTrackerComboButton1", buttons, TRACKER_COMBO_BUTTON_L)) {
                shouldUpdateVectors = true;
            }
            if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Combo Button 2", "gItemTrackerComboButton2", buttons, TRACKER_COMBO_BUTTON_R)) {
                shouldUpdateVectors = true;
            }
        }
    }
    UIWidgets::PaddedSeparator();
    UIWidgets::EnhancementSliderInt("Icon size : %dpx", "##ITEMTRACKERICONSIZE", "gItemTrackerIconSize", 25, 128, "", 36);
    UIWidgets::EnhancementSliderInt("Icon margins : %dpx", "##ITEMTRACKERSPACING", "gItemTrackerIconSpacing", -5, 50, "", 12);
    
    UIWidgets::Spacer(0);

    ImGui::Text("Ammo/Capacity Tracking");
    UIWidgets::EnhancementCombobox("gItemTrackerCapacityTrack", itemTrackerCapacityTrackOptions, ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY);
    UIWidgets::InsertHelpHoverText("Customize what the numbers under each item are tracking."
                                    "\n\nNote: items without capacity upgrades will track ammo even in capacity mode");
    if (CVarGetInteger("gItemTrackerCapacityTrack", ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY) == ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY || CVarGetInteger("gItemTrackerCapacityTrack", ITEM_TRACKER_NUMBER_CURRENT_CAPACITY_ONLY) == ITEM_TRACKER_NUMBER_CURRENT_AMMO_ONLY) {
        if (UIWidgets::PaddedEnhancementCheckbox("Align count to left side", "gItemTrackerCurrentOnLeft")) {
            shouldUpdateVectors = true;
        }
    }

    UIWidgets::Spacer(0);

    ImGui::Text("Key Count Tracking");
    UIWidgets::EnhancementCombobox("gItemTrackerKeyTrack", itemTrackerKeyTrackOptions, KEYS_COLLECTED_MAX);
    UIWidgets::InsertHelpHoverText("Customize what numbers are shown for key tracking.");

    UIWidgets::Spacer(0);

    ImGui::Text("Triforce Piece Count Tracking");
    UIWidgets::EnhancementCombobox("gItemTrackerTriforcePieceTrack", itemTrackerTriforcePieceTrackOptions, TRIFORCE_PIECE_COLLECTED_REQUIRED_MAX);
    UIWidgets::InsertHelpHoverText("Customize what numbers are shown for triforce piece tracking.");

    ImGui::TableNextColumn();

    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Inventory", "gItemTrackerInventoryItemsDisplayType", displayTypes, SECTION_DISPLAY_MAIN_WINDOW)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Equipment", "gItemTrackerEquipmentItemsDisplayType", displayTypes, SECTION_DISPLAY_MAIN_WINDOW)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Misc", "gItemTrackerMiscItemsDisplayType", displayTypes, SECTION_DISPLAY_MAIN_WINDOW)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Dungeon Rewards", "gItemTrackerDungeonRewardsDisplayType", displayTypes, SECTION_DISPLAY_MAIN_WINDOW)) {
        shouldUpdateVectors = true;
    }
    if (CVarGetInteger("gItemTrackerDungeonRewardsDisplayType", SECTION_DISPLAY_MAIN_WINDOW) == SECTION_DISPLAY_SEPARATE) {
        if (UIWidgets::PaddedEnhancementCheckbox("Circle display", "gItemTrackerDungeonRewardsCircle", true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true)) {
            shouldUpdateVectors = true;
        }
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Songs", "gItemTrackerSongsDisplayType", displayTypes, SECTION_DISPLAY_MAIN_WINDOW)) {
        shouldUpdateVectors = true;
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Dungeon Items", "gItemTrackerDungeonItemsDisplayType", displayTypes, SECTION_DISPLAY_HIDDEN)) {
        shouldUpdateVectors = true;
    }
    if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", SECTION_DISPLAY_HIDDEN) != SECTION_DISPLAY_HIDDEN) {
        if (CVarGetInteger("gItemTrackerDungeonItemsDisplayType", SECTION_DISPLAY_HIDDEN) == SECTION_DISPLAY_SEPARATE) {
            if (UIWidgets::PaddedEnhancementCheckbox("Horizontal display", "gItemTrackerDisplayDungeonItemsHorizontal", true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true)) {
                shouldUpdateVectors = true;
            }
        }
        if (UIWidgets::PaddedEnhancementCheckbox("Maps and compasses", "gItemTrackerDisplayDungeonItemsMaps", true, true, false, "", UIWidgets::CheckboxGraphics::Cross, true)) {
            shouldUpdateVectors = true;
        }
    }
    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Greg", "gItemTrackerGregDisplayType", extendedDisplayTypes, SECTION_DISPLAY_EXTENDED_HIDDEN)) {
        shouldUpdateVectors = true;
    }

    if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Triforce Pieces", "gItemTrackerTriforcePiecesDisplayType", displayTypes, SECTION_DISPLAY_HIDDEN)) {
        shouldUpdateVectors = true;
    }

    if (CVarGetInteger("gItemTrackerDisplayType", TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_ALWAYS) {
        if (UIWidgets::LabeledRightAlignedEnhancementCombobox("Personal notes", "gItemTrackerNotesDisplayType", displayTypes, SECTION_DISPLAY_HIDDEN)) {
            shouldUpdateVectors = true;
        }
    }

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::End();
}

void ItemTrackerWindow::InitElement() {
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

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadFile>([](uint32_t fileNum) {
        const char* initialTrackerNotes = CVarGetString(("gItemTrackerNotes" + std::to_string(fileNum)).c_str(), "");
        itemTrackerNotes.resize(strlen(initialTrackerNotes) + 1);
        strcpy(itemTrackerNotes.Data, initialTrackerNotes);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnDeleteFile>([](uint32_t fileNum) {
        CVarSetString(("gItemTrackerNotes" + std::to_string(fileNum)).c_str(), "");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(ItemTrackerOnFrame);
}

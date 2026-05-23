#include "randomizer_check_tracker.h"
#include "randomizer_entrance_tracker.h"
#include "randomizer_item_tracker.h"
#include "randomizerTypes.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "soh/SaveManager.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/SohMenu.h"
#include "dungeon.h"
#include "entrance.h"
#include "location_access.h"
#include "3drando/fill.hpp"
#include "soh/Enhancements/debugger/performanceTimer.h"
#include "soh/Enhancements/randomizer/randomizer.h"

#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <libultraship/libultraship.h>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include "location.h"
#include "item_location.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "z64item.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}
extern "C" GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);

extern std::vector<ItemTrackerItem> dungeonRewardStones;
extern std::vector<ItemTrackerItem> dungeonRewardMedallions;
extern std::vector<ItemTrackerItem> songItems;
extern std::vector<ItemTrackerItem> equipmentItems;

using json = nlohmann::json;
using namespace UIWidgets;

namespace CheckTracker {
static WidgetInfo backgroundColorWidget;
static WidgetInfo windowTypeWidget;
static WidgetInfo dungeonSpoilerWidget;
static WidgetInfo hideUnshuffledShopWidget;
static WidgetInfo showGSWidget;
static WidgetInfo showLogicWidget;
static WidgetInfo ageChecksWidget;
static WidgetInfo checkAvailabilityWidget;
static WidgetInfo availableChecksNoneWidget;
static WidgetInfo availableChecksChildWidget;
static WidgetInfo availableChecksAdultWidget;
static WidgetInfo availableChecksBothWidget;

// settings
bool showShops;
bool showOverworldTokens;
bool showDungeonTokens;
bool showBeans;
bool showScrubs;
bool showMajorScrubs;
bool showMerchants;
bool showSongs;
bool showBeehives;
bool showCows;
bool showOverworldFreestanding;
bool showDungeonFreestanding;
bool showAdultTrade;
bool showKokiriSword;
bool showMasterSword;
bool showHyruleLoach;
bool showWeirdEgg;
bool showGerudoCard;
bool showOverworldPots;
bool showDungeonPots;
bool showOverworldGrass;
bool showDungeonGrass;
bool showOverworldCrates;
bool showDungeonCrates;
bool showRocks;
bool showOverworldBoulders;
bool showDungeonBoulders;
bool showTrees;
bool showBushes;
bool showOverworldSigns;
bool showDungeonSigns;
bool showOverworldWonderItems;
bool showDungeonWonderItems;
bool showBeggar;
bool showIcicles;
bool showRedIce;
bool showFrogSongRupees;
bool showFountainFairies;
bool showStoneFairies;
bool showBeanFairies;
bool showSongFairies;
bool showButterflyFairies;
bool showStartingMapsCompasses;
bool showKeysanity;
bool showGerudoFortressKeys;
bool showBossKeysanity;
bool showGanonBossKey;
bool showOcarinas;
bool show100SkullReward;
bool showLinksPocket;
bool fortressFast;
bool fortressNormal;

u8 fishsanityMode;
u8 fishsanityPondCount;
bool fishsanityAgeSplit;

// persistent during gameplay
bool initialized;
bool doAreaScroll;
bool previousShowHidden = false;
bool hideShopUnshuffledChecks = false;
bool alwaysShowGS = false;

static bool presetLoaded = false;
static ImVec2 presetPos;
static ImVec2 presetSize;

std::map<uint32_t, RandomizerCheck> startingShopItem = {
    { SCENE_KOKIRI_SHOP, RC_KF_SHOP_ITEM_1 },
    { SCENE_BAZAAR, RC_MARKET_BAZAAR_ITEM_1 },
    { SCENE_POTION_SHOP_MARKET, RC_MARKET_POTION_SHOP_ITEM_1 },
    { SCENE_BOMBCHU_SHOP, RC_MARKET_BOMBCHU_SHOP_ITEM_1 },
    { SCENE_POTION_SHOP_KAKARIKO, RC_KAK_POTION_SHOP_ITEM_1 },
    { SCENE_ZORA_SHOP, RC_ZD_SHOP_ITEM_1 },
    { SCENE_GORON_SHOP, RC_GC_SHOP_ITEM_1 },
};

std::map<SceneID, RandomizerCheckArea> DungeonRCAreasBySceneID = {
    { SCENE_DEKU_TREE, RCAREA_DEKU_TREE },
    { SCENE_DODONGOS_CAVERN, RCAREA_DODONGOS_CAVERN },
    { SCENE_JABU_JABU, RCAREA_JABU_JABUS_BELLY },
    { SCENE_FOREST_TEMPLE, RCAREA_FOREST_TEMPLE },
    { SCENE_FIRE_TEMPLE, RCAREA_FIRE_TEMPLE },
    { SCENE_WATER_TEMPLE, RCAREA_WATER_TEMPLE },
    { SCENE_SHADOW_TEMPLE, RCAREA_SHADOW_TEMPLE },
    { SCENE_SPIRIT_TEMPLE, RCAREA_SPIRIT_TEMPLE },
    { SCENE_BOTTOM_OF_THE_WELL, RCAREA_BOTTOM_OF_THE_WELL },
    { SCENE_ICE_CAVERN, RCAREA_ICE_CAVERN },
    { SCENE_GERUDO_TRAINING_GROUND, RCAREA_GERUDO_TRAINING_GROUND },
    { SCENE_INSIDE_GANONS_CASTLE, RCAREA_GANONS_CASTLE },
};

// Dungeon entrances with obvious visual differences between MQ and vanilla qualifying as spoiling on sight
std::vector<uint32_t> spoilingEntrances = {
    ENTR_DEKU_TREE_ENTRANCE,
    ENTR_DODONGOS_CAVERN_BOSS_DOOR,
    ENTR_JABU_JABU_ENTRANCE,
    ENTR_JABU_JABU_BOSS_DOOR,
    ENTR_FOREST_TEMPLE_ENTRANCE,
    ENTR_FIRE_TEMPLE_ENTRANCE,
    ENTR_FIRE_TEMPLE_BOSS_DOOR,
    ENTR_WATER_TEMPLE_BOSS_DOOR,
    ENTR_SPIRIT_TEMPLE_ENTRANCE,
    ENTR_SHADOW_TEMPLE_BOSS_DOOR,
    ENTR_ICE_CAVERN_ENTRANCE,
    ENTR_GERUDO_TRAINING_GROUND_ENTRANCE,
    ENTR_INSIDE_GANONS_CASTLE_ENTRANCE,
};

std::map<RandomizerCheckArea, std::vector<RandomizerCheck>> checksByArea;
bool areasFullyChecked[RCAREA_INVALID];
u32 areasSpoiled = 0;
bool showVOrMQ;
s16 areaChecksGotten[RCAREA_INVALID]; //|     "Kokiri Forest (4/9)"
s16 areaChecksAvailable[RCAREA_INVALID];
s16 areaCheckTotals[RCAREA_INVALID];
uint16_t totalChecks = 0;
uint16_t totalChecksAvailable = 0;
uint16_t totalChecksGotten = 0;
bool optCollapseAll; // A bool that will collapse all checks once
bool optExpandAll;   // A bool that will expand all checks once
RandomizerCheck lastLocationChecked = RC_UNKNOWN_CHECK;
RandomizerCheckArea previousArea = RCAREA_INVALID;
RandomizerCheckArea currentArea = RCAREA_INVALID;
OSContPad* trackerButtonsPressed;
std::unordered_map<RandomizerCheck, std::string> checkNameOverrides;

bool ShouldShowCheck(RandomizerCheck rc);
bool UpdateFilters();
bool CompareChecks(RandomizerCheck, RandomizerCheck);
bool CheckByArea(RandomizerCheckArea);
void DrawLocation(RandomizerCheck);
void LoadSettings();
void RainbowTick();
void UpdateAreas(RandomizerCheckArea area);
void UpdateInventoryChecks();
void UpdateOrdering(RandomizerCheckArea);
int sectionId;

bool hideUnchecked = false;
bool hideScummed = false;
bool hideSeen = false;
bool hideSkipped = false;
bool hideSaved = false;
bool hideCollected = false;
bool showHidden = true;
bool mystery = false;
bool showLogicTooltip = false;

typedef enum : int { AC_DISABLED, AC_LOCKED_UNLOCKED, AC_ONE_ICON, AC_TWO_ICONS } AvailableChecksDisplay;
AvailableChecksDisplay ageChecksDisplay = AC_DISABLED;
AvailableChecksDisplay availableChecksDisplay = AC_DISABLED;
std::string availableChecksNoneAvailableIcon = ICON_FA_LOCK;
std::string availableChecksChildAvailableIcon = ICON_FA_CHILD;
std::string availableChecksAdultAvailableIcon = ICON_FA_USER;
std::string availableChecksBothAvailableIcon = ICON_FA_USERS;

typedef enum : int {
    AC_SHOW_ALL_CHECKS,
    AC_SHOW_AVAILABLE_CHECKS,
    AC_SHOW_CHILD_CHECKS,
    AC_SHOW_ADULT_CHECKS,
    AC_SHOW_CURRENT_AGE
} AvailableChecksOnlyShow;
AvailableChecksOnlyShow availableChecksOnlyShow = AC_SHOW_ALL_CHECKS;

SceneID DungeonSceneLookupByArea(RandomizerCheckArea area) {
    switch (area) {
        case RCAREA_DEKU_TREE:
            return SCENE_DEKU_TREE;
        case RCAREA_DODONGOS_CAVERN:
            return SCENE_DODONGOS_CAVERN;
        case RCAREA_JABU_JABUS_BELLY:
            return SCENE_JABU_JABU;
        case RCAREA_FOREST_TEMPLE:
            return SCENE_FOREST_TEMPLE;
        case RCAREA_FIRE_TEMPLE:
            return SCENE_FIRE_TEMPLE;
        case RCAREA_WATER_TEMPLE:
            return SCENE_WATER_TEMPLE;
        case RCAREA_SPIRIT_TEMPLE:
            return SCENE_SPIRIT_TEMPLE;
        case RCAREA_SHADOW_TEMPLE:
            return SCENE_SHADOW_TEMPLE;
        case RCAREA_BOTTOM_OF_THE_WELL:
            return SCENE_BOTTOM_OF_THE_WELL;
        case RCAREA_ICE_CAVERN:
            return SCENE_ICE_CAVERN;
        case RCAREA_GERUDO_TRAINING_GROUND:
            return SCENE_GERUDO_TRAINING_GROUND;
        case RCAREA_GANONS_CASTLE:
            return SCENE_INSIDE_GANONS_CASTLE;
        default:
            return SCENE_ID_MAX;
    }
}

const Color_RGBA8 Color_Main_Default = { 255, 255, 255, 255 };                  // White
const Color_RGBA8 Color_Area_Incomplete_Extra_Default = { 255, 255, 255, 255 }; // White
const Color_RGBA8 Color_Area_Complete_Extra_Default = { 255, 255, 255, 255 };   // White
const Color_RGBA8 Color_Unchecked_Extra_Default = { 255, 255, 255, 255 };       // White
const Color_RGBA8 Color_Skipped_Main_Default = { 160, 160, 160, 255 };          // Grey
const Color_RGBA8 Color_Skipped_Extra_Default = { 160, 160, 160, 255 };         // Grey
const Color_RGBA8 Color_Seen_Extra_Default = { 255, 255, 255, 255 };            // TODO
const Color_RGBA8 Color_Hinted_Extra_Default = { 255, 255, 255, 255 };          // TODO
const Color_RGBA8 Color_Collected_Extra_Default = { 242, 101, 34, 255 };        // Orange
const Color_RGBA8 Color_Scummed_Extra_Default = { 0, 174, 239, 255 };           // Blue
const Color_RGBA8 Color_Saved_Extra_Default = { 0, 185, 0, 255 };               // Green

Color_RGBA8 Color_Background = { 0, 0, 0, 255 };

Color_RGBA8 Color_Area_Incomplete_Main = { 255, 255, 255, 255 };  // White
Color_RGBA8 Color_Area_Incomplete_Extra = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Complete_Main = { 255, 255, 255, 255 };    // White
Color_RGBA8 Color_Area_Complete_Extra = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Unchecked_Main = { 255, 255, 255, 255 };        // White
Color_RGBA8 Color_Unchecked_Extra = { 255, 255, 255, 255 };       // Useless
Color_RGBA8 Color_Skipped_Main = { 160, 160, 160, 255 };          // Grey
Color_RGBA8 Color_Skipped_Extra = { 160, 160, 160, 255 };         // Grey
Color_RGBA8 Color_Seen_Main = { 255, 255, 255, 255 };             // TODO
Color_RGBA8 Color_Seen_Extra = { 160, 160, 160, 255 };            // TODO
Color_RGBA8 Color_Hinted_Main = { 255, 255, 255, 255 };           // TODO
Color_RGBA8 Color_Hinted_Extra = { 255, 255, 255, 255 };          // TODO
Color_RGBA8 Color_Collected_Main = { 255, 255, 255, 255 };        // White
Color_RGBA8 Color_Collected_Extra = { 242, 101, 34, 255 };        // Orange
Color_RGBA8 Color_Scummed_Main = { 255, 255, 255, 255 };          // White
Color_RGBA8 Color_Scummed_Extra = { 0, 174, 239, 255 };           // Blue
Color_RGBA8 Color_Saved_Main = { 255, 255, 255, 255 };            // White
Color_RGBA8 Color_Saved_Extra = { 0, 185, 0, 255 };               // Green

static ImGuiTextFilter checkSearch;
static s32 previousAge = 0;
static bool recalculateAvailable = false;
static RandomizerRegion availableChecksStartingRegion = RR_ROOT;
static RandoAgeTime availableChecksStartingAgeTime = RAT_NONE;
static int16_t previousEntrance = 0;
std::array<bool, RCAREA_INVALID> filterAreasHidden = { 0 };
std::array<bool, RC_MAX> filterChecksHidden = { 0 };

void TrySetAreas() {
    if (checksByArea.empty()) {
        for (int i = RCAREA_KOKIRI_FOREST; i < RCAREA_INVALID; i++) {
            checksByArea.emplace(static_cast<RandomizerCheckArea>(i), std::vector<RandomizerCheck>());
        }
    }
}

void CalculateTotals() {
    totalChecks = 0;
    totalChecksAvailable = 0;
    totalChecksGotten = 0;

    for (uint8_t i = 0; i < RCAREA_INVALID; i++) {
        totalChecks += areaCheckTotals[i];
        totalChecksAvailable += areaChecksAvailable[i];
        totalChecksGotten += areaChecksGotten[i];
    }
}

uint16_t GetTotalChecks() {
    return totalChecks;
}

uint16_t GetTotalChecksGotten() {
    return totalChecksGotten;
}

bool IsCheckHidden(RandomizerCheck rc) {
    Rando::ItemLocation* itemLocation = OTRGlobals::Instance->gRandoContext->GetItemLocation(rc);
    RandomizerCheckStatus status = itemLocation->GetCheckStatus();
    bool available = itemLocation->IsChildAvailable() || itemLocation->IsAdultAvailable();
    bool skipped = itemLocation->GetIsSkipped();
    bool obtained = itemLocation->HasObtained();
    bool seen = status == RCSHOW_SEEN || status == RCSHOW_IDENTIFIED;
    bool scummed = status == RCSHOW_SCUMMED;
    bool unchecked = status == RCSHOW_UNCHECKED;

    return !showHidden &&
           ((skipped && hideSkipped) || (seen && hideSeen) || (scummed && hideScummed) || (unchecked && hideUnchecked));
}

void RecalculateAreaTotals(RandomizerCheckArea rcArea) {
    areaChecksGotten[rcArea] = 0;
    areaChecksAvailable[rcArea] = 0;
    areaCheckTotals[rcArea] = 0;
    for (auto rc : checksByArea.at(rcArea)) {
        if (!IsVisibleInCheckTracker(rc)) {
            continue;
        }
        areaCheckTotals[rcArea]++;

        Rando::ItemLocation* itemLoc = OTRGlobals::Instance->gRandoContext->GetItemLocation(rc);

        if (itemLoc->GetIsSkipped() || itemLoc->HasObtained()) {
            areaChecksGotten[rcArea]++;
        }

        bool childAvailable = itemLoc->IsChildAvailable();
        bool adultAvailable = itemLoc->IsAdultAvailable();
        if (availableChecksDisplay != AC_DISABLED && !IsCheckHidden(rc) && !itemLoc->HasObtained()) {
            if ((availableChecksOnlyShow == AC_SHOW_ALL_CHECKS && (childAvailable || adultAvailable)) ||
                (availableChecksOnlyShow == AC_SHOW_AVAILABLE_CHECKS && (childAvailable || adultAvailable)) ||
                (availableChecksOnlyShow == AC_SHOW_CHILD_CHECKS && childAvailable) ||
                (availableChecksOnlyShow == AC_SHOW_ADULT_CHECKS && adultAvailable) ||
                (availableChecksOnlyShow == AC_SHOW_CURRENT_AGE &&
                 ((LINK_IS_CHILD && childAvailable) || (LINK_IS_ADULT && adultAvailable)))) {
                areaChecksAvailable[rcArea]++;
            }
        }
    }
    CalculateTotals();
}

std::map<RandomizerGet, RandomizerCheckArea> MapRGtoRandomizerCheckArea = {
    { RG_DEKU_TREE_MAP, RCAREA_DEKU_TREE },
    { RG_DODONGOS_CAVERN_MAP, RCAREA_DODONGOS_CAVERN },
    { RG_JABU_JABUS_BELLY_MAP, RCAREA_JABU_JABUS_BELLY },
    { RG_FOREST_TEMPLE_MAP, RCAREA_FOREST_TEMPLE },
    { RG_FIRE_TEMPLE_MAP, RCAREA_FIRE_TEMPLE },
    { RG_WATER_TEMPLE_MAP, RCAREA_WATER_TEMPLE },
    { RG_SPIRIT_TEMPLE_MAP, RCAREA_SPIRIT_TEMPLE },
    { RG_SHADOW_TEMPLE_MAP, RCAREA_SHADOW_TEMPLE },
    { RG_BOTTOM_OF_THE_WELL_MAP, RCAREA_BOTTOM_OF_THE_WELL },
    { RG_ICE_CAVERN_MAP, RCAREA_ICE_CAVERN }
};

void SpoilAreaFromCheck(RandomizerCheck rc) {
    Rando::Location* loc = Rando::StaticData::GetLocation(rc);
    Rando::ItemLocation* itemLoc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (itemLoc->GetPlacedItem().GetItemType() == ItemType::ITEMTYPE_MAP) {
        RandomizerCheckArea area = MapRGtoRandomizerCheckArea[itemLoc->GetPlacedRandomizerGet()];
        if (!IsAreaSpoiled(area)) {
            SetAreaSpoiled(area);
        }
    }
    if (!IsAreaSpoiled(loc->GetArea())) {
        SetAreaSpoiled(loc->GetArea());
    }
}

void RecalculateAllAreaTotals() {
    for (auto& [rcArea, checks] : checksByArea) {
        if (rcArea == RCAREA_INVALID) {
            return;
        }
        RecalculateAreaTotals(rcArea);
    }
}

void SetCheckCollected(RandomizerCheck rc) {
    OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetCheckStatus(RCSHOW_COLLECTED);
    Rando::Location* loc = Rando::StaticData::GetLocation(rc);
    if (IsVisibleInCheckTracker(rc)) {
        if (!OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->GetIsSkipped()) {
            areaChecksGotten[loc->GetArea()]++;
            areaChecksAvailable[loc->GetArea()]--;
        } else {
            OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetIsSkipped(false);
        }
    }
    SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);

    if (!IsAreaSpoiled(loc->GetArea())) {
        SetAreaSpoiled(loc->GetArea());
    }

    doAreaScroll = true;
    UpdateOrdering(loc->GetArea());
    UpdateInventoryChecks();
}

bool IsAreaScene(SceneID sceneNum) {
    switch (sceneNum) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_GRAVEYARD:
        case SCENE_ZORAS_RIVER:
        case SCENE_KOKIRI_FOREST:
        case SCENE_SACRED_FOREST_MEADOW:
        case SCENE_LAKE_HYLIA:
        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORAS_FOUNTAIN:
        case SCENE_GERUDO_VALLEY:
        case SCENE_LOST_WOODS:
        case SCENE_DESERT_COLOSSUS:
        case SCENE_GERUDOS_FORTRESS:
        case SCENE_HAUNTED_WASTELAND:
        case SCENE_HYRULE_CASTLE:
        case SCENE_DEATH_MOUNTAIN_TRAIL:
        case SCENE_DEATH_MOUNTAIN_CRATER:
        case SCENE_GORON_CITY:
        case SCENE_LON_LON_RANCH:
        case SCENE_DEKU_TREE:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_JABU_JABU:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
        case SCENE_GERUDO_TRAINING_GROUND:
        case SCENE_GANONS_TOWER:
        case SCENE_INSIDE_GANONS_CASTLE:
        case SCENE_BACK_ALLEY_DAY:
        case SCENE_BACK_ALLEY_NIGHT:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
            return true;
        default:
            return false;
    }
}

RandomizerCheckArea AreaFromEntranceGroup[] = {
    RCAREA_INVALID,          RCAREA_KOKIRI_FOREST, RCAREA_LOST_WOODS,           RCAREA_SACRED_FOREST_MEADOW,
    RCAREA_KAKARIKO_VILLAGE, RCAREA_GRAVEYARD,     RCAREA_DEATH_MOUNTAIN_TRAIL, RCAREA_DEATH_MOUNTAIN_CRATER,
    RCAREA_GORON_CITY,       RCAREA_ZORAS_RIVER,   RCAREA_ZORAS_DOMAIN,         RCAREA_ZORAS_FOUNTAIN,
    RCAREA_HYRULE_FIELD,     RCAREA_LON_LON_RANCH, RCAREA_LAKE_HYLIA,           RCAREA_GERUDO_VALLEY,
    RCAREA_GERUDO_FORTRESS,  RCAREA_WASTELAND,     RCAREA_DESERT_COLOSSUS,      RCAREA_MARKET,
    RCAREA_HYRULE_CASTLE,
};

RandomizerCheckArea GetCheckArea() {
    auto scene = static_cast<SceneID>(gPlayState->sceneNum);
    bool grottoScene = (scene == SCENE_GROTTOS || scene == SCENE_FAIRYS_FOUNTAIN);
    const EntranceData* ent = EntranceTracker::GetEntranceData(
        grottoScene ? ENTRANCE_GROTTO_EXIT_START + EntranceTracker::GetCurrentGrottoId() : gSaveContext.entranceIndex);
    RandomizerCheckArea area = RCAREA_INVALID;
    if (ent != nullptr && !IsAreaScene(scene) && ent->type != ENTRANCE_TYPE_DUNGEON) {
        if (ent->source == "Desert Colossus" || ent->destination == "Desert Colossus") {
            area = RCAREA_DESERT_COLOSSUS;
        } else {
            area = AreaFromEntranceGroup[ent->dstGroup];
        }
    }
    if (area == RCAREA_INVALID) {
        if (grottoScene && (EntranceTracker::GetCurrentGrottoId() == -1) &&
            (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) == RO_GENERIC_OFF)) {
            area = previousArea;
        } else {
            area = RandomizerCheckObjects::GetRCAreaBySceneID(scene);
        }
    }
    return area;
}

std::array<SceneID, 4> skipScenes = {
    SCENE_GANON_BOSS,
    SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR,
    SCENE_INSIDE_GANONS_CASTLE_COLLAPSE,
    SCENE_GANONS_TOWER_COLLAPSE_INTERIOR,
};

void ClearAreaChecksAndTotals() {
    for (auto& [rcArea, vec] : checksByArea) {
        vec.clear();
        areaChecksGotten[rcArea] = 0;
        areaChecksAvailable[rcArea] = 0;
        areaCheckTotals[rcArea] = 0;
    }
    totalChecks = 0;
    totalChecksGotten = 0;
    totalChecksAvailable = 0;
}

void SetShopSeen(uint32_t sceneNum, bool prices) {
    RandomizerCheck start = startingShopItem.find(sceneNum)->second;
    if (sceneNum == SCENE_POTION_SHOP_KAKARIKO && !LINK_IS_ADULT) {
        return;
    }
    if (GetCheckArea() == RCAREA_KAKARIKO_VILLAGE && sceneNum == SCENE_BAZAAR) {
        start = RC_KAK_BAZAAR_ITEM_1;
    }
    bool statusChanged = false;
    for (int i = start; i < start + 8; i++) {
        if (OTRGlobals::Instance->gRandoContext->GetItemLocation(i)->GetCheckStatus() == RCSHOW_UNCHECKED) {
            OTRGlobals::Instance->gRandoContext->GetItemLocation(i)->SetCheckStatus(RCSHOW_SEEN);
            statusChanged = true;
        }
    }
    if (statusChanged) {
        SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);
    }
}

void CheckTrackerLoadGame(int32_t fileNum) {
    if (IS_BOSS_RUSH) {
        return;
    }
    LoadSettings();
    TrySetAreas();
    for (auto& entry : Rando::StaticData::GetLocationTable()) {
        RandomizerCheck rc = entry.GetRandomizerCheck();
        if (rc == RC_UNKNOWN_CHECK || rc == RC_MAX || rc == RC_LINKS_POCKET ||
            !Rando::StaticData::GetLocation(rc) != RC_UNKNOWN_CHECK) {
            continue;
        }

        Rando::Location* entry2 = Rando::StaticData::GetLocation(rc);
        Rando::ItemLocation* loc = OTRGlobals::Instance->gRandoContext->GetItemLocation(rc);

        checksByArea.find(entry2->GetArea())->second.push_back(entry2->GetRandomizerCheck());
        if (IsVisibleInCheckTracker(entry2->GetRandomizerCheck())) {
            areaCheckTotals[entry2->GetArea()]++;
            if (loc->GetCheckStatus() == RCSHOW_SAVED || loc->GetIsSkipped()) {
                areaChecksGotten[entry2->GetArea()]++;
            }
            if (loc->IsChildAvailable() || loc->IsAdultAvailable()) {
                areaChecksAvailable[entry2->GetArea()]++;
            }
        }

        if (areaChecksGotten[entry2->GetArea()] != 0 || RandomizerCheckObjects::AreaIsOverworld(entry2->GetArea()) ||
            loc->GetCheckStatus() == RCSHOW_SCUMMED) {
            areasSpoiled |= (1 << entry2->GetArea());
        }

        // Create check name overrides for child pond fish if age split is disabled
        if (fishsanityMode != RO_FISHSANITY_OFF && fishsanityMode != RO_FISHSANITY_OVERWORLD &&
            entry.GetRCType() == RCTYPE_FISH && entry.GetScene() == SCENE_FISHING_POND &&
            entry.GetActorParams() != 116 && !fishsanityAgeSplit) {
            if (entry.GetShortName().starts_with("Child")) {
                checkNameOverrides[rc] = entry.GetShortName().substr(6);
            }
        }
    }
    for (int i = RCAREA_KOKIRI_FOREST; i < RCAREA_INVALID; i++) {
        if (!IsAreaSpoiled(static_cast<RandomizerCheckArea>(i)) &&
            (RandomizerCheckObjects::AreaIsOverworld(static_cast<RandomizerCheckArea>(i)) || !IS_RANDO ||
             OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_RANDOM) == RO_MQ_DUNGEONS_NONE ||
             (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_RANDOM) ==
                  RO_MQ_DUNGEONS_SELECTION &&
              OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(
                  static_cast<RandomizerSettingKey>(RSK_MQ_DEKU_TREE + (i - RCAREA_DEKU_TREE))) != RO_MQ_SET_RANDOM) ||
             (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_SET) == RO_GENERIC_ON &&
              OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(
                  static_cast<RandomizerSettingKey>(RSK_MQ_DEKU_TREE + (i - RCAREA_DEKU_TREE))) != RO_MQ_SET_RANDOM) ||
             (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_RANDOM) ==
                  RO_MQ_DUNGEONS_SET_NUMBER &&
              (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_COUNT) == MAX_MQ_DUNGEON_COUNT ||
               OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_COUNT) == 0)))) {
            SetAreaSpoiled(static_cast<RandomizerCheckArea>(i));
        }
    }
    if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LINKS_POCKET) != RO_LINKS_POCKET_NOTHING &&
        IS_RANDO) {
        uint8_t startingAge = OTRGlobals::Instance->gRandoContext->GetOption(RSK_SELECTED_STARTING_AGE).Get();
        RandomizerCheckArea startingArea;
        switch (startingAge) {
            case RO_AGE_CHILD:
                startingArea = RCAREA_KOKIRI_FOREST;
                break;
            case RO_AGE_ADULT:
                startingArea = RCAREA_MARKET;
                break;
            default:
                startingArea = RCAREA_KOKIRI_FOREST;
                break;
        }

        checksByArea.find(startingArea)->second.push_back(RC_LINKS_POCKET);
        areaChecksGotten[startingArea]++;
        areaCheckTotals[startingArea]++;
    }

    showVOrMQ =
        (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_RANDOM) ==
             RO_MQ_DUNGEONS_RANDOM_NUMBER ||
         (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_RANDOM) == RO_MQ_DUNGEONS_SET_NUMBER &&
          OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_COUNT) < MAX_MQ_DUNGEON_COUNT));
    initialized = true;
    UpdateAllOrdering();
    UpdateInventoryChecks();
    UpdateFilters();

    RegionTable_Init();

    Rando::Context::GetInstance()->GetEntranceShuffler()->ApplyEntranceOverrides();

    CalculateCheckAges();

    recalculateAvailable = true;
}

void CheckTrackerShopSlotChange(uint8_t cursorSlot, int16_t basePrice) {
    if (gPlayState->sceneNum == SCENE_HAPPY_MASK_SHOP) { // Happy Mask Shop is not used in rando, so is not tracked
        return;
    }

    auto slot = startingShopItem.find(gPlayState->sceneNum)->second + cursorSlot;
    if (GetCheckArea() == RCAREA_KAKARIKO_VILLAGE && gPlayState->sceneNum == SCENE_BAZAAR) {
        slot = RC_KAK_BAZAAR_ITEM_1 + cursorSlot;
    }
    auto status = OTRGlobals::Instance->gRandoContext->GetItemLocation(slot)->GetCheckStatus();
    if (status == RCSHOW_SEEN) {
        OTRGlobals::Instance->gRandoContext->GetItemLocation(slot)->SetCheckStatus(RCSHOW_IDENTIFIED);
        SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);
        RecalculateAvailableChecks();
    }
}

void CheckTrackerTransition(uint32_t sceneNum) {
    if (!GameInteractor::IsSaveLoaded()) {
        return;
    }
    doAreaScroll = true;
    previousArea = currentArea;
    currentArea = GetCheckArea();
    switch (sceneNum) {
        case SCENE_KOKIRI_SHOP:
        case SCENE_BAZAAR:
        case SCENE_POTION_SHOP_MARKET:
        case SCENE_BOMBCHU_SHOP:
        case SCENE_POTION_SHOP_KAKARIKO:
        case SCENE_GORON_SHOP:
        case SCENE_ZORA_SHOP:
            SetShopSeen(sceneNum, false);
            break;
    }
    if (!IsAreaSpoiled(currentArea) && (RandomizerCheckObjects::AreaIsOverworld(currentArea) ||
                                        std::find(spoilingEntrances.begin(), spoilingEntrances.end(),
                                                  gPlayState->nextEntranceIndex) != spoilingEntrances.end())) {
        SetAreaSpoiled(currentArea);
    }
}

void CheckTrackerItemReceive(GetItemEntry giEntry) {
    if (!GameInteractor::IsSaveLoaded() || std::find(std::begin(skipScenes), std::end(skipScenes),
                                                     (SceneID)gPlayState->sceneNum) != std::end(skipScenes)) {
        return;
    }
    auto scene = static_cast<SceneID>(gPlayState->sceneNum);
    // Vanilla special item checks
    if (!IS_RANDO) {
        if (giEntry.itemId == ITEM_SHIELD_DEKU) {
            SetCheckCollected(RC_KF_SHOP_ITEM_1);
            return;
        } else if (giEntry.itemId == ITEM_KOKIRI_EMERALD) {
            SetCheckCollected(RC_QUEEN_GOHMA);
            return;
        } else if (giEntry.itemId == ITEM_GORON_RUBY) {
            SetCheckCollected(RC_KING_DODONGO);
            return;
        } else if (giEntry.itemId == ITEM_ZORA_SAPPHIRE) {
            SetCheckCollected(RC_BARINADE);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_FOREST) {
            SetCheckCollected(RC_PHANTOM_GANON);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_FIRE) {
            SetCheckCollected(RC_VOLVAGIA);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_WATER) {
            SetCheckCollected(RC_MORPHA);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_SHADOW) {
            SetCheckCollected(RC_BONGO_BONGO);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_SPIRIT) {
            SetCheckCollected(RC_TWINROVA);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_LIGHT) {
            SetCheckCollected(RC_GIFT_FROM_RAURU);
            return;
        } else if (giEntry.itemId == ITEM_SONG_EPONA) {
            SetCheckCollected(RC_SONG_FROM_MALON);
            return;
        } else if (giEntry.itemId == ITEM_SONG_SARIA) {
            SetCheckCollected(RC_SONG_FROM_SARIA);
            return;
        } else if (giEntry.itemId == ITEM_BEAN) {
            SetCheckCollected(RC_ZR_MAGIC_BEAN_SALESMAN);
            return;
        } else if (giEntry.itemId == ITEM_BRACELET) {
            SetCheckCollected(RC_GC_DARUNIAS_JOY);
            return;
        } /* else if (giEntry.itemId == ITEM_SONG_SUN) {
             SetCheckCollected(RC_SONG_FROM_ROYAL_FAMILYS_TOMB);
             return;
         } else if (giEntry.itemId == ITEM_SONG_TIME) {
             SetCheckCollected(RC_SONG_FROM_OCARINA_OF_TIME);
             return;
         } else if (giEntry.itemId == ITEM_SONG_STORMS) {
             SetCheckCollected(RC_SONG_FROM_WINDMILL);
             return;
         } else if (giEntry.itemId == ITEM_SONG_MINUET) {
             SetCheckCollected(RC_SHEIK_IN_FOREST);
             return;
         } else if (giEntry.itemId == ITEM_SONG_BOLERO) {
             SetCheckCollected(RC_SHEIK_IN_CRATER);
             return;
         } else if (giEntry.itemId == ITEM_SONG_SERENADE) {
             SetCheckCollected(RC_SHEIK_IN_ICE_CAVERN);
             return;
         } else if (giEntry.itemId == ITEM_SONG_NOCTURNE) {
             SetCheckCollected(RC_SHEIK_IN_KAKARIKO);
             return;
         } else if (giEntry.itemId == ITEM_SONG_REQUIEM) {
             SetCheckCollected(RC_SHEIK_AT_COLOSSUS);
             return;
         } else if (giEntry.itemId == ITEM_SONG_PRELUDE) {
             SetCheckCollected(RC_SHEIK_AT_TEMPLE);
             return;
         }*/
    }
}

void CheckTrackerSceneFlagSet(int16_t sceneNum, int16_t flagType, int32_t flag) {
    if (IS_RANDO) {
        return;
    }

    if (flagType != FLAG_SCENE_TREASURE && flagType != FLAG_SCENE_COLLECTIBLE) {
        return;
    }
    if (sceneNum == SCENE_GRAVEYARD && flag == 0x19 &&
        flagType == FLAG_SCENE_COLLECTIBLE) { // Gravedigging tour special case
        SetCheckCollected(RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR);
        return;
    }
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if (!IsVisibleInCheckTracker(loc.GetRandomizerCheck())) {
            continue;
        }
        SpoilerCollectionCheckType checkMatchType = flagType == FLAG_SCENE_TREASURE
                                                        ? SpoilerCollectionCheckType::SPOILER_CHK_CHEST
                                                        : SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE;
        Rando::SpoilerCollectionCheck scCheck = loc.GetCollectionCheck();
        if (scCheck.scene == sceneNum && scCheck.flag == flag && scCheck.type == checkMatchType) {
            SetCheckCollected(loc.GetRandomizerCheck());
            return;
        }
    }
}

void CheckTrackerFlagSet(int16_t flagType, int32_t flag) {
    if (IS_RANDO) {
        return;
    }

    SpoilerCollectionCheckType checkMatchType = SpoilerCollectionCheckType::SPOILER_CHK_NONE;
    switch (flagType) {
        case FLAG_GS_TOKEN:
            checkMatchType = SpoilerCollectionCheckType::SPOILER_CHK_GOLD_SKULLTULA;
            break;
        case FLAG_EVENT_CHECK_INF:
            if ((flag == EVENTCHKINF_CARPENTERS_FREE(0) || flag == EVENTCHKINF_CARPENTERS_FREE(1) ||
                 flag == EVENTCHKINF_CARPENTERS_FREE(2) || flag == EVENTCHKINF_CARPENTERS_FREE(3)) &&
                GET_EVENTCHKINF_CARPENTERS_FREE_ALL()) {
                SetCheckCollected(RC_TH_FREED_CARPENTERS);
                return;
            }
            checkMatchType = SpoilerCollectionCheckType::SPOILER_CHK_EVENT_CHK_INF;
            break;
        case FLAG_INF_TABLE:
            if (flag == INFTABLE_190) {
                SetCheckCollected(RC_GF_HBA_1000_POINTS);
                return;
            } else if (flag == INFTABLE_11E) {
                SetCheckCollected(RC_GC_ROLLING_GORON_AS_CHILD);
                return;
            } else if (flag == INFTABLE_GORON_CITY_DOORS_UNLOCKED) {
                SetCheckCollected(RC_GC_ROLLING_GORON_AS_ADULT);
                return;
            } else if (flag == INFTABLE_139) {
                SetCheckCollected(RC_ZD_KING_ZORA_THAWED);
                return;
            } else if (flag == INFTABLE_191) {
                SetCheckCollected(RC_MARKET_LOST_DOG);
                return;
            }
            if (!IS_RANDO) {
                if (flag == INFTABLE_BOUGHT_STICK_UPGRADE) {
                    SetCheckCollected(RC_LW_DEKU_SCRUB_NEAR_BRIDGE);
                    return;
                } else if (flag == INFTABLE_BOUGHT_NUT_UPGRADE) {
                    SetCheckCollected(RC_LW_DEKU_SCRUB_GROTTO_FRONT);
                    return;
                }
            }
            break;
        case FLAG_ITEM_GET_INF:
            if (!IS_RANDO) {
                if (flag == ITEMGETINF_OBTAINED_STICK_UPGRADE_FROM_STAGE) {
                    SetCheckCollected(RC_DEKU_THEATER_SKULL_MASK);
                    return;
                } else if (flag == ITEMGETINF_OBTAINED_NUT_UPGRADE_FROM_STAGE) {
                    SetCheckCollected(RC_DEKU_THEATER_MASK_OF_TRUTH);
                    return;
                } else if (flag == ITEMGETINF_DEKU_SCRUB_HEART_PIECE) {
                    SetCheckCollected(RC_HF_DEKU_SCRUB_GROTTO);
                    return;
                }
            }
            checkMatchType = SpoilerCollectionCheckType::SPOILER_CHK_ITEM_GET_INF;
            break;
        case FLAG_RANDOMIZER_INF:
            checkMatchType = SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF;
            break;
    }
    if (checkMatchType == SpoilerCollectionCheckType::SPOILER_CHK_NONE) {
        return;
    }
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if ((!IS_RANDO && ((loc.GetQuest() == RCQUEST_MQ && !IS_MASTER_QUEST) ||
                           (loc.GetQuest() == RCQUEST_VANILLA && IS_MASTER_QUEST))) ||
            (IS_RANDO &&
             !(OTRGlobals::Instance->gRandoContext->GetDungeons()->GetDungeonFromScene(loc.GetScene()) == nullptr) &&
             ((OTRGlobals::Instance->gRandoContext->GetDungeons()->GetDungeonFromScene(loc.GetScene())->IsMQ() &&
               loc.GetQuest() == RCQUEST_VANILLA) ||
              OTRGlobals::Instance->gRandoContext->GetDungeons()->GetDungeonFromScene(loc.GetScene())->IsVanilla() &&
                  loc.GetQuest() == RCQUEST_MQ))) {
            continue;
        }
        Rando::SpoilerCollectionCheck scCheck = loc.GetCollectionCheck();
        SpoilerCollectionCheckType scCheckType = scCheck.type;
        if (checkMatchType == SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF &&
            scCheckType == SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF) {
            if (flag == OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(loc.GetRandomizerCheck())) {
                SetCheckCollected(loc.GetRandomizerCheck());
                return;
            }
            continue;
        }
        int16_t checkFlag = scCheck.flag;
        if (checkMatchType == SpoilerCollectionCheckType::SPOILER_CHK_GOLD_SKULLTULA) {
            checkFlag = loc.GetActorParams();
        }
        if (checkFlag == flag && scCheck.type == checkMatchType) {
            SetCheckCollected(loc.GetRandomizerCheck());
            return;
        }
    }
}

void InitTrackerData(bool isDebug) {
    TrySetAreas();
    areasSpoiled = 0;
}

void SaveTrackerData(SaveContext* saveContext, int sectionID, bool fullSave) {
    bool updateOrdering = false;
    std::vector<RandomizerCheck> checkCount;
    for (int i = RC_UNKNOWN_CHECK; i < RC_MAX; i++) {
        if (OTRGlobals::Instance->gRandoContext->GetItemLocation(i)->GetCheckStatus() != RCSHOW_UNCHECKED ||
            OTRGlobals::Instance->gRandoContext->GetItemLocation(i)->GetIsSkipped())
            checkCount.push_back(static_cast<RandomizerCheck>(i));
    }
    SaveManager::Instance->SaveArray("checkStatus", checkCount.size(), [&](size_t i) {
        RandomizerCheck check = checkCount.at(i);
        RandomizerCheckStatus savedStatus =
            OTRGlobals::Instance->gRandoContext->GetItemLocation(check)->GetCheckStatus();
        bool isSkipped = OTRGlobals::Instance->gRandoContext->GetItemLocation(check)->GetIsSkipped();
        if (savedStatus == RCSHOW_COLLECTED) {
            if (fullSave) {
                OTRGlobals::Instance->gRandoContext->GetItemLocation(check)->SetCheckStatus(RCSHOW_SAVED);
                savedStatus = RCSHOW_SAVED;
                updateOrdering = true;
            } else {
                savedStatus = RCSHOW_SCUMMED;
            }
        }
        if (savedStatus != RCSHOW_UNCHECKED || isSkipped) {
            SaveManager::Instance->SaveStruct("", [&]() {
                SaveManager::Instance->SaveData("randomizerCheck", check);
                SaveManager::Instance->SaveData("status", savedStatus);
                SaveManager::Instance->SaveData("skipped", isSkipped);
            });
        }
    });
    SaveManager::Instance->SaveData("areasSpoiled", areasSpoiled);
    if (updateOrdering) {
        UpdateAllOrdering();
        UpdateAllAreas();
    }
}

void SaveFile(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveTrackerData(saveContext, sectionID, fullSave);
    if (fullSave) {
        recalculateAvailable = true;
    }
}

void LoadFile() {
    SaveManager::Instance->LoadArray("checkStatus", RC_MAX, [](size_t i) {
        SaveManager::Instance->LoadStruct("", [&]() {
            RandomizerCheckStatus status;
            bool skipped;
            RandomizerCheck rc;
            SaveManager::Instance->LoadData("randomizerCheck", rc, RC_UNKNOWN_CHECK);
            SaveManager::Instance->LoadData("status", status, RCSHOW_UNCHECKED);
            SaveManager::Instance->LoadData("skipped", skipped, false);
            OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetCheckStatus(status);
            OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetIsSkipped(skipped);
        });
    });
    SaveManager::Instance->LoadData("areasSpoiled", areasSpoiled, (uint32_t)0);
    UpdateAllOrdering();
    UpdateAllAreas();
}

void Teardown() {
    initialized = false;
    ClearAreaChecksAndTotals();
    checksByArea.clear();
    areasSpoiled = 0;
    filterAreasHidden = { 0 };
    filterChecksHidden = { 0 };

    lastLocationChecked = RC_UNKNOWN_CHECK;
}

bool IsAreaSpoiled(RandomizerCheckArea rcArea) {
    return areasSpoiled & (1 << rcArea);
}

void SetAreaSpoiled(RandomizerCheckArea rcArea) {
    areasSpoiled |= (1 << rcArea);
    SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);
}

static std::string MapAvailableCheckIcon(std::string name) {
    if (name == "ICON_FA_GLASS")
        return ICON_FA_GLASS;
    if (name == "ICON_FA_MUSIC")
        return ICON_FA_MUSIC;
    if (name == "ICON_FA_SEARCH")
        return ICON_FA_SEARCH;
    if (name == "ICON_FA_ENVELOPE_O")
        return ICON_FA_ENVELOPE_O;
    if (name == "ICON_FA_HEART")
        return ICON_FA_HEART;
    if (name == "ICON_FA_STAR")
        return ICON_FA_STAR;
    if (name == "ICON_FA_STAR_O")
        return ICON_FA_STAR_O;
    if (name == "ICON_FA_USER")
        return ICON_FA_USER;
    if (name == "ICON_FA_FILM")
        return ICON_FA_FILM;
    if (name == "ICON_FA_TH_LARGE")
        return ICON_FA_TH_LARGE;
    if (name == "ICON_FA_TH")
        return ICON_FA_TH;
    if (name == "ICON_FA_TH_LIST")
        return ICON_FA_TH_LIST;
    if (name == "ICON_FA_CHECK")
        return ICON_FA_CHECK;
    if (name == "ICON_FA_TIMES")
        return ICON_FA_TIMES;
    if (name == "ICON_FA_SEARCH_PLUS")
        return ICON_FA_SEARCH_PLUS;
    if (name == "ICON_FA_SEARCH_MINUS")
        return ICON_FA_SEARCH_MINUS;
    if (name == "ICON_FA_POWER_OFF")
        return ICON_FA_POWER_OFF;
    if (name == "ICON_FA_SIGNAL")
        return ICON_FA_SIGNAL;
    if (name == "ICON_FA_COG")
        return ICON_FA_COG;
    if (name == "ICON_FA_TRASH_O")
        return ICON_FA_TRASH_O;
    if (name == "ICON_FA_HOME")
        return ICON_FA_HOME;
    if (name == "ICON_FA_FILE_O")
        return ICON_FA_FILE_O;
    if (name == "ICON_FA_CLOCK_O")
        return ICON_FA_CLOCK_O;
    if (name == "ICON_FA_ROAD")
        return ICON_FA_ROAD;
    if (name == "ICON_FA_DOWNLOAD")
        return ICON_FA_DOWNLOAD;
    if (name == "ICON_FA_ARROW_CIRCLE_O_DOWN")
        return ICON_FA_ARROW_CIRCLE_O_DOWN;
    if (name == "ICON_FA_ARROW_CIRCLE_O_UP")
        return ICON_FA_ARROW_CIRCLE_O_UP;
    if (name == "ICON_FA_INBOX")
        return ICON_FA_INBOX;
    if (name == "ICON_FA_PLAY_CIRCLE_O")
        return ICON_FA_PLAY_CIRCLE_O;
    if (name == "ICON_FA_REPEAT")
        return ICON_FA_REPEAT;
    if (name == "ICON_FA_REFRESH")
        return ICON_FA_REFRESH;
    if (name == "ICON_FA_LIST_ALT")
        return ICON_FA_LIST_ALT;
    if (name == "ICON_FA_LOCK")
        return ICON_FA_LOCK;
    if (name == "ICON_FA_FLAG")
        return ICON_FA_FLAG;
    if (name == "ICON_FA_HEADPHONES")
        return ICON_FA_HEADPHONES;
    if (name == "ICON_FA_VOLUME_OFF")
        return ICON_FA_VOLUME_OFF;
    if (name == "ICON_FA_VOLUME_DOWN")
        return ICON_FA_VOLUME_DOWN;
    if (name == "ICON_FA_VOLUME_UP")
        return ICON_FA_VOLUME_UP;
    if (name == "ICON_FA_QRCODE")
        return ICON_FA_QRCODE;
    if (name == "ICON_FA_BARCODE")
        return ICON_FA_BARCODE;
    if (name == "ICON_FA_TAG")
        return ICON_FA_TAG;
    if (name == "ICON_FA_TAGS")
        return ICON_FA_TAGS;
    if (name == "ICON_FA_BOOK")
        return ICON_FA_BOOK;
    if (name == "ICON_FA_BOOKMARK")
        return ICON_FA_BOOKMARK;
    if (name == "ICON_FA_PRINT")
        return ICON_FA_PRINT;
    if (name == "ICON_FA_CAMERA")
        return ICON_FA_CAMERA;
    if (name == "ICON_FA_FONT")
        return ICON_FA_FONT;
    if (name == "ICON_FA_BOLD")
        return ICON_FA_BOLD;
    if (name == "ICON_FA_ITALIC")
        return ICON_FA_ITALIC;
    if (name == "ICON_FA_TEXT_HEIGHT")
        return ICON_FA_TEXT_HEIGHT;
    if (name == "ICON_FA_TEXT_WIDTH")
        return ICON_FA_TEXT_WIDTH;
    if (name == "ICON_FA_ALIGN_LEFT")
        return ICON_FA_ALIGN_LEFT;
    if (name == "ICON_FA_ALIGN_CENTER")
        return ICON_FA_ALIGN_CENTER;
    if (name == "ICON_FA_ALIGN_RIGHT")
        return ICON_FA_ALIGN_RIGHT;
    if (name == "ICON_FA_ALIGN_JUSTIFY")
        return ICON_FA_ALIGN_JUSTIFY;
    if (name == "ICON_FA_LIST")
        return ICON_FA_LIST;
    if (name == "ICON_FA_OUTDENT")
        return ICON_FA_OUTDENT;
    if (name == "ICON_FA_INDENT")
        return ICON_FA_INDENT;
    if (name == "ICON_FA_VIDEO_CAMERA")
        return ICON_FA_VIDEO_CAMERA;
    if (name == "ICON_FA_PICTURE_O")
        return ICON_FA_PICTURE_O;
    if (name == "ICON_FA_PENCIL")
        return ICON_FA_PENCIL;
    if (name == "ICON_FA_MAP_MARKER")
        return ICON_FA_MAP_MARKER;
    if (name == "ICON_FA_ADJUST")
        return ICON_FA_ADJUST;
    if (name == "ICON_FA_TINT")
        return ICON_FA_TINT;
    if (name == "ICON_FA_PENCIL_SQUARE_O")
        return ICON_FA_PENCIL_SQUARE_O;
    if (name == "ICON_FA_SHARE_SQUARE_O")
        return ICON_FA_SHARE_SQUARE_O;
    if (name == "ICON_FA_CHECK_SQUARE_O")
        return ICON_FA_CHECK_SQUARE_O;
    if (name == "ICON_FA_ARROWS")
        return ICON_FA_ARROWS;
    if (name == "ICON_FA_STEP_BACKWARD")
        return ICON_FA_STEP_BACKWARD;
    if (name == "ICON_FA_FAST_BACKWARD")
        return ICON_FA_FAST_BACKWARD;
    if (name == "ICON_FA_BACKWARD")
        return ICON_FA_BACKWARD;
    if (name == "ICON_FA_PLAY")
        return ICON_FA_PLAY;
    if (name == "ICON_FA_PAUSE")
        return ICON_FA_PAUSE;
    if (name == "ICON_FA_STOP")
        return ICON_FA_STOP;
    if (name == "ICON_FA_FORWARD")
        return ICON_FA_FORWARD;
    if (name == "ICON_FA_FAST_FORWARD")
        return ICON_FA_FAST_FORWARD;
    if (name == "ICON_FA_STEP_FORWARD")
        return ICON_FA_STEP_FORWARD;
    if (name == "ICON_FA_EJECT")
        return ICON_FA_EJECT;
    if (name == "ICON_FA_CHEVRON_LEFT")
        return ICON_FA_CHEVRON_LEFT;
    if (name == "ICON_FA_CHEVRON_RIGHT")
        return ICON_FA_CHEVRON_RIGHT;
    if (name == "ICON_FA_PLUS_CIRCLE")
        return ICON_FA_PLUS_CIRCLE;
    if (name == "ICON_FA_MINUS_CIRCLE")
        return ICON_FA_MINUS_CIRCLE;
    if (name == "ICON_FA_TIMES_CIRCLE")
        return ICON_FA_TIMES_CIRCLE;
    if (name == "ICON_FA_CHECK_CIRCLE")
        return ICON_FA_CHECK_CIRCLE;
    if (name == "ICON_FA_QUESTION_CIRCLE")
        return ICON_FA_QUESTION_CIRCLE;
    if (name == "ICON_FA_INFO_CIRCLE")
        return ICON_FA_INFO_CIRCLE;
    if (name == "ICON_FA_CROSSHAIRS")
        return ICON_FA_CROSSHAIRS;
    if (name == "ICON_FA_TIMES_CIRCLE_O")
        return ICON_FA_TIMES_CIRCLE_O;
    if (name == "ICON_FA_CHECK_CIRCLE_O")
        return ICON_FA_CHECK_CIRCLE_O;
    if (name == "ICON_FA_BAN")
        return ICON_FA_BAN;
    if (name == "ICON_FA_ARROW_LEFT")
        return ICON_FA_ARROW_LEFT;
    if (name == "ICON_FA_ARROW_RIGHT")
        return ICON_FA_ARROW_RIGHT;
    if (name == "ICON_FA_ARROW_UP")
        return ICON_FA_ARROW_UP;
    if (name == "ICON_FA_ARROW_DOWN")
        return ICON_FA_ARROW_DOWN;
    if (name == "ICON_FA_SHARE")
        return ICON_FA_SHARE;
    if (name == "ICON_FA_EXPAND")
        return ICON_FA_EXPAND;
    if (name == "ICON_FA_COMPRESS")
        return ICON_FA_COMPRESS;
    if (name == "ICON_FA_PLUS")
        return ICON_FA_PLUS;
    if (name == "ICON_FA_MINUS")
        return ICON_FA_MINUS;
    if (name == "ICON_FA_ASTERISK")
        return ICON_FA_ASTERISK;
    if (name == "ICON_FA_EXCLAMATION_CIRCLE")
        return ICON_FA_EXCLAMATION_CIRCLE;
    if (name == "ICON_FA_GIFT")
        return ICON_FA_GIFT;
    if (name == "ICON_FA_LEAF")
        return ICON_FA_LEAF;
    if (name == "ICON_FA_FIRE")
        return ICON_FA_FIRE;
    if (name == "ICON_FA_EYE")
        return ICON_FA_EYE;
    if (name == "ICON_FA_EYE_SLASH")
        return ICON_FA_EYE_SLASH;
    if (name == "ICON_FA_EXCLAMATION_TRIANGLE")
        return ICON_FA_EXCLAMATION_TRIANGLE;
    if (name == "ICON_FA_PLANE")
        return ICON_FA_PLANE;
    if (name == "ICON_FA_CALENDAR")
        return ICON_FA_CALENDAR;
    if (name == "ICON_FA_RANDOM")
        return ICON_FA_RANDOM;
    if (name == "ICON_FA_COMMENT")
        return ICON_FA_COMMENT;
    if (name == "ICON_FA_MAGNET")
        return ICON_FA_MAGNET;
    if (name == "ICON_FA_CHEVRON_UP")
        return ICON_FA_CHEVRON_UP;
    if (name == "ICON_FA_CHEVRON_DOWN")
        return ICON_FA_CHEVRON_DOWN;
    if (name == "ICON_FA_RETWEET")
        return ICON_FA_RETWEET;
    if (name == "ICON_FA_SHOPPING_CART")
        return ICON_FA_SHOPPING_CART;
    if (name == "ICON_FA_FOLDER")
        return ICON_FA_FOLDER;
    if (name == "ICON_FA_FOLDER_OPEN")
        return ICON_FA_FOLDER_OPEN;
    if (name == "ICON_FA_ARROWS_V")
        return ICON_FA_ARROWS_V;
    if (name == "ICON_FA_ARROWS_H")
        return ICON_FA_ARROWS_H;
    if (name == "ICON_FA_BAR_CHART")
        return ICON_FA_BAR_CHART;
    if (name == "ICON_FA_TWITTER_SQUARE")
        return ICON_FA_TWITTER_SQUARE;
    if (name == "ICON_FA_FACEBOOK_SQUARE")
        return ICON_FA_FACEBOOK_SQUARE;
    if (name == "ICON_FA_CAMERA_RETRO")
        return ICON_FA_CAMERA_RETRO;
    if (name == "ICON_FA_KEY")
        return ICON_FA_KEY;
    if (name == "ICON_FA_COGS")
        return ICON_FA_COGS;
    if (name == "ICON_FA_COMMENTS")
        return ICON_FA_COMMENTS;
    if (name == "ICON_FA_THUMBS_O_UP")
        return ICON_FA_THUMBS_O_UP;
    if (name == "ICON_FA_THUMBS_O_DOWN")
        return ICON_FA_THUMBS_O_DOWN;
    if (name == "ICON_FA_STAR_HALF")
        return ICON_FA_STAR_HALF;
    if (name == "ICON_FA_HEART_O")
        return ICON_FA_HEART_O;
    if (name == "ICON_FA_SIGN_OUT")
        return ICON_FA_SIGN_OUT;
    if (name == "ICON_FA_LINKEDIN_SQUARE")
        return ICON_FA_LINKEDIN_SQUARE;
    if (name == "ICON_FA_THUMB_TACK")
        return ICON_FA_THUMB_TACK;
    if (name == "ICON_FA_EXTERNAL_LINK")
        return ICON_FA_EXTERNAL_LINK;
    if (name == "ICON_FA_SIGN_IN")
        return ICON_FA_SIGN_IN;
    if (name == "ICON_FA_TROPHY")
        return ICON_FA_TROPHY;
    if (name == "ICON_FA_GITHUB_SQUARE")
        return ICON_FA_GITHUB_SQUARE;
    if (name == "ICON_FA_UPLOAD")
        return ICON_FA_UPLOAD;
    if (name == "ICON_FA_LEMON_O")
        return ICON_FA_LEMON_O;
    if (name == "ICON_FA_PHONE")
        return ICON_FA_PHONE;
    if (name == "ICON_FA_SQUARE_O")
        return ICON_FA_SQUARE_O;
    if (name == "ICON_FA_BOOKMARK_O")
        return ICON_FA_BOOKMARK_O;
    if (name == "ICON_FA_PHONE_SQUARE")
        return ICON_FA_PHONE_SQUARE;
    if (name == "ICON_FA_TWITTER")
        return ICON_FA_TWITTER;
    if (name == "ICON_FA_FACEBOOK")
        return ICON_FA_FACEBOOK;
    if (name == "ICON_FA_GITHUB")
        return ICON_FA_GITHUB;
    if (name == "ICON_FA_UNLOCK")
        return ICON_FA_UNLOCK;
    if (name == "ICON_FA_CREDIT_CARD")
        return ICON_FA_CREDIT_CARD;
    if (name == "ICON_FA_RSS")
        return ICON_FA_RSS;
    if (name == "ICON_FA_HDD_O")
        return ICON_FA_HDD_O;
    if (name == "ICON_FA_BULLHORN")
        return ICON_FA_BULLHORN;
    if (name == "ICON_FA_BELL")
        return ICON_FA_BELL;
    if (name == "ICON_FA_CERTIFICATE")
        return ICON_FA_CERTIFICATE;
    if (name == "ICON_FA_HAND_O_RIGHT")
        return ICON_FA_HAND_O_RIGHT;
    if (name == "ICON_FA_HAND_O_LEFT")
        return ICON_FA_HAND_O_LEFT;
    if (name == "ICON_FA_HAND_O_UP")
        return ICON_FA_HAND_O_UP;
    if (name == "ICON_FA_HAND_O_DOWN")
        return ICON_FA_HAND_O_DOWN;
    if (name == "ICON_FA_ARROW_CIRCLE_LEFT")
        return ICON_FA_ARROW_CIRCLE_LEFT;
    if (name == "ICON_FA_ARROW_CIRCLE_RIGHT")
        return ICON_FA_ARROW_CIRCLE_RIGHT;
    if (name == "ICON_FA_ARROW_CIRCLE_UP")
        return ICON_FA_ARROW_CIRCLE_UP;
    if (name == "ICON_FA_ARROW_CIRCLE_DOWN")
        return ICON_FA_ARROW_CIRCLE_DOWN;
    if (name == "ICON_FA_GLOBE")
        return ICON_FA_GLOBE;
    if (name == "ICON_FA_WRENCH")
        return ICON_FA_WRENCH;
    if (name == "ICON_FA_TASKS")
        return ICON_FA_TASKS;
    if (name == "ICON_FA_FILTER")
        return ICON_FA_FILTER;
    if (name == "ICON_FA_BRIEFCASE")
        return ICON_FA_BRIEFCASE;
    if (name == "ICON_FA_ARROWS_ALT")
        return ICON_FA_ARROWS_ALT;
    if (name == "ICON_FA_USERS")
        return ICON_FA_USERS;
    if (name == "ICON_FA_LINK")
        return ICON_FA_LINK;
    if (name == "ICON_FA_CLOUD")
        return ICON_FA_CLOUD;
    if (name == "ICON_FA_FLASK")
        return ICON_FA_FLASK;
    if (name == "ICON_FA_SCISSORS")
        return ICON_FA_SCISSORS;
    if (name == "ICON_FA_FILES_O")
        return ICON_FA_FILES_O;
    if (name == "ICON_FA_PAPERCLIP")
        return ICON_FA_PAPERCLIP;
    if (name == "ICON_FA_FLOPPY_O")
        return ICON_FA_FLOPPY_O;
    if (name == "ICON_FA_SQUARE")
        return ICON_FA_SQUARE;
    if (name == "ICON_FA_BARS")
        return ICON_FA_BARS;
    if (name == "ICON_FA_LIST_UL")
        return ICON_FA_LIST_UL;
    if (name == "ICON_FA_LIST_OL")
        return ICON_FA_LIST_OL;
    if (name == "ICON_FA_STRIKETHROUGH")
        return ICON_FA_STRIKETHROUGH;
    if (name == "ICON_FA_UNDERLINE")
        return ICON_FA_UNDERLINE;
    if (name == "ICON_FA_TABLE")
        return ICON_FA_TABLE;
    if (name == "ICON_FA_MAGIC")
        return ICON_FA_MAGIC;
    if (name == "ICON_FA_TRUCK")
        return ICON_FA_TRUCK;
    if (name == "ICON_FA_PINTEREST")
        return ICON_FA_PINTEREST;
    if (name == "ICON_FA_PINTEREST_SQUARE")
        return ICON_FA_PINTEREST_SQUARE;
    if (name == "ICON_FA_GOOGLE_PLUS_SQUARE")
        return ICON_FA_GOOGLE_PLUS_SQUARE;
    if (name == "ICON_FA_GOOGLE_PLUS")
        return ICON_FA_GOOGLE_PLUS;
    if (name == "ICON_FA_MONEY")
        return ICON_FA_MONEY;
    if (name == "ICON_FA_CARET_DOWN")
        return ICON_FA_CARET_DOWN;
    if (name == "ICON_FA_CARET_UP")
        return ICON_FA_CARET_UP;
    if (name == "ICON_FA_CARET_LEFT")
        return ICON_FA_CARET_LEFT;
    if (name == "ICON_FA_CARET_RIGHT")
        return ICON_FA_CARET_RIGHT;
    if (name == "ICON_FA_COLUMNS")
        return ICON_FA_COLUMNS;
    if (name == "ICON_FA_SORT")
        return ICON_FA_SORT;
    if (name == "ICON_FA_SORT_DESC")
        return ICON_FA_SORT_DESC;
    if (name == "ICON_FA_SORT_ASC")
        return ICON_FA_SORT_ASC;
    if (name == "ICON_FA_ENVELOPE")
        return ICON_FA_ENVELOPE;
    if (name == "ICON_FA_LINKEDIN")
        return ICON_FA_LINKEDIN;
    if (name == "ICON_FA_UNDO")
        return ICON_FA_UNDO;
    if (name == "ICON_FA_GAVEL")
        return ICON_FA_GAVEL;
    if (name == "ICON_FA_TACHOMETER")
        return ICON_FA_TACHOMETER;
    if (name == "ICON_FA_COMMENT_O")
        return ICON_FA_COMMENT_O;
    if (name == "ICON_FA_COMMENTS_O")
        return ICON_FA_COMMENTS_O;
    if (name == "ICON_FA_BOLT")
        return ICON_FA_BOLT;
    if (name == "ICON_FA_SITEMAP")
        return ICON_FA_SITEMAP;
    if (name == "ICON_FA_UMBRELLA")
        return ICON_FA_UMBRELLA;
    if (name == "ICON_FA_CLIPBOARD")
        return ICON_FA_CLIPBOARD;
    if (name == "ICON_FA_LIGHTBULB_O")
        return ICON_FA_LIGHTBULB_O;
    if (name == "ICON_FA_EXCHANGE")
        return ICON_FA_EXCHANGE;
    if (name == "ICON_FA_CLOUD_DOWNLOAD")
        return ICON_FA_CLOUD_DOWNLOAD;
    if (name == "ICON_FA_CLOUD_UPLOAD")
        return ICON_FA_CLOUD_UPLOAD;
    if (name == "ICON_FA_USER_MD")
        return ICON_FA_USER_MD;
    if (name == "ICON_FA_STETHOSCOPE")
        return ICON_FA_STETHOSCOPE;
    if (name == "ICON_FA_SUITCASE")
        return ICON_FA_SUITCASE;
    if (name == "ICON_FA_BELL_O")
        return ICON_FA_BELL_O;
    if (name == "ICON_FA_COFFEE")
        return ICON_FA_COFFEE;
    if (name == "ICON_FA_CUTLERY")
        return ICON_FA_CUTLERY;
    if (name == "ICON_FA_FILE_TEXT_O")
        return ICON_FA_FILE_TEXT_O;
    if (name == "ICON_FA_BUILDING_O")
        return ICON_FA_BUILDING_O;
    if (name == "ICON_FA_HOSPITAL_O")
        return ICON_FA_HOSPITAL_O;
    if (name == "ICON_FA_AMBULANCE")
        return ICON_FA_AMBULANCE;
    if (name == "ICON_FA_MEDKIT")
        return ICON_FA_MEDKIT;
    if (name == "ICON_FA_FIGHTER_JET")
        return ICON_FA_FIGHTER_JET;
    if (name == "ICON_FA_BEER")
        return ICON_FA_BEER;
    if (name == "ICON_FA_H_SQUARE")
        return ICON_FA_H_SQUARE;
    if (name == "ICON_FA_PLUS_SQUARE")
        return ICON_FA_PLUS_SQUARE;
    if (name == "ICON_FA_ANGLE_DOUBLE_LEFT")
        return ICON_FA_ANGLE_DOUBLE_LEFT;
    if (name == "ICON_FA_ANGLE_DOUBLE_RIGHT")
        return ICON_FA_ANGLE_DOUBLE_RIGHT;
    if (name == "ICON_FA_ANGLE_DOUBLE_UP")
        return ICON_FA_ANGLE_DOUBLE_UP;
    if (name == "ICON_FA_ANGLE_DOUBLE_DOWN")
        return ICON_FA_ANGLE_DOUBLE_DOWN;
    if (name == "ICON_FA_ANGLE_LEFT")
        return ICON_FA_ANGLE_LEFT;
    if (name == "ICON_FA_ANGLE_RIGHT")
        return ICON_FA_ANGLE_RIGHT;
    if (name == "ICON_FA_ANGLE_UP")
        return ICON_FA_ANGLE_UP;
    if (name == "ICON_FA_ANGLE_DOWN")
        return ICON_FA_ANGLE_DOWN;
    if (name == "ICON_FA_DESKTOP")
        return ICON_FA_DESKTOP;
    if (name == "ICON_FA_LAPTOP")
        return ICON_FA_LAPTOP;
    if (name == "ICON_FA_TABLET")
        return ICON_FA_TABLET;
    if (name == "ICON_FA_MOBILE")
        return ICON_FA_MOBILE;
    if (name == "ICON_FA_CIRCLE_O")
        return ICON_FA_CIRCLE_O;
    if (name == "ICON_FA_QUOTE_LEFT")
        return ICON_FA_QUOTE_LEFT;
    if (name == "ICON_FA_QUOTE_RIGHT")
        return ICON_FA_QUOTE_RIGHT;
    if (name == "ICON_FA_SPINNER")
        return ICON_FA_SPINNER;
    if (name == "ICON_FA_CIRCLE")
        return ICON_FA_CIRCLE;
    if (name == "ICON_FA_REPLY")
        return ICON_FA_REPLY;
    if (name == "ICON_FA_GITHUB_ALT")
        return ICON_FA_GITHUB_ALT;
    if (name == "ICON_FA_FOLDER_O")
        return ICON_FA_FOLDER_O;
    if (name == "ICON_FA_FOLDER_OPEN_O")
        return ICON_FA_FOLDER_OPEN_O;
    if (name == "ICON_FA_SMILE_O")
        return ICON_FA_SMILE_O;
    if (name == "ICON_FA_FROWN_O")
        return ICON_FA_FROWN_O;
    if (name == "ICON_FA_MEH_O")
        return ICON_FA_MEH_O;
    if (name == "ICON_FA_GAMEPAD")
        return ICON_FA_GAMEPAD;
    if (name == "ICON_FA_KEYBOARD_O")
        return ICON_FA_KEYBOARD_O;
    if (name == "ICON_FA_FLAG_O")
        return ICON_FA_FLAG_O;
    if (name == "ICON_FA_FLAG_CHECKERED")
        return ICON_FA_FLAG_CHECKERED;
    if (name == "ICON_FA_TERMINAL")
        return ICON_FA_TERMINAL;
    if (name == "ICON_FA_CODE")
        return ICON_FA_CODE;
    if (name == "ICON_FA_REPLY_ALL")
        return ICON_FA_REPLY_ALL;
    if (name == "ICON_FA_STAR_HALF_O")
        return ICON_FA_STAR_HALF_O;
    if (name == "ICON_FA_LOCATION_ARROW")
        return ICON_FA_LOCATION_ARROW;
    if (name == "ICON_FA_CROP")
        return ICON_FA_CROP;
    if (name == "ICON_FA_CODE_FORK")
        return ICON_FA_CODE_FORK;
    if (name == "ICON_FA_CHAIN_BROKEN")
        return ICON_FA_CHAIN_BROKEN;
    if (name == "ICON_FA_QUESTION")
        return ICON_FA_QUESTION;
    if (name == "ICON_FA_INFO")
        return ICON_FA_INFO;
    if (name == "ICON_FA_EXCLAMATION")
        return ICON_FA_EXCLAMATION;
    if (name == "ICON_FA_SUPERSCRIPT")
        return ICON_FA_SUPERSCRIPT;
    if (name == "ICON_FA_SUBSCRIPT")
        return ICON_FA_SUBSCRIPT;
    if (name == "ICON_FA_ERASER")
        return ICON_FA_ERASER;
    if (name == "ICON_FA_PUZZLE_PIECE")
        return ICON_FA_PUZZLE_PIECE;
    if (name == "ICON_FA_MICROPHONE")
        return ICON_FA_MICROPHONE;
    if (name == "ICON_FA_MICROPHONE_SLASH")
        return ICON_FA_MICROPHONE_SLASH;
    if (name == "ICON_FA_SHIELD")
        return ICON_FA_SHIELD;
    if (name == "ICON_FA_CALENDAR_O")
        return ICON_FA_CALENDAR_O;
    if (name == "ICON_FA_FIRE_EXTINGUISHER")
        return ICON_FA_FIRE_EXTINGUISHER;
    if (name == "ICON_FA_ROCKET")
        return ICON_FA_ROCKET;
    if (name == "ICON_FA_MAXCDN")
        return ICON_FA_MAXCDN;
    if (name == "ICON_FA_CHEVRON_CIRCLE_LEFT")
        return ICON_FA_CHEVRON_CIRCLE_LEFT;
    if (name == "ICON_FA_CHEVRON_CIRCLE_RIGHT")
        return ICON_FA_CHEVRON_CIRCLE_RIGHT;
    if (name == "ICON_FA_CHEVRON_CIRCLE_UP")
        return ICON_FA_CHEVRON_CIRCLE_UP;
    if (name == "ICON_FA_CHEVRON_CIRCLE_DOWN")
        return ICON_FA_CHEVRON_CIRCLE_DOWN;
    if (name == "ICON_FA_HTML5")
        return ICON_FA_HTML5;
    if (name == "ICON_FA_CSS3")
        return ICON_FA_CSS3;
    if (name == "ICON_FA_ANCHOR")
        return ICON_FA_ANCHOR;
    if (name == "ICON_FA_UNLOCK_ALT")
        return ICON_FA_UNLOCK_ALT;
    if (name == "ICON_FA_BULLSEYE")
        return ICON_FA_BULLSEYE;
    if (name == "ICON_FA_ELLIPSIS_H")
        return ICON_FA_ELLIPSIS_H;
    if (name == "ICON_FA_ELLIPSIS_V")
        return ICON_FA_ELLIPSIS_V;
    if (name == "ICON_FA_RSS_SQUARE")
        return ICON_FA_RSS_SQUARE;
    if (name == "ICON_FA_PLAY_CIRCLE")
        return ICON_FA_PLAY_CIRCLE;
    if (name == "ICON_FA_TICKET")
        return ICON_FA_TICKET;
    if (name == "ICON_FA_MINUS_SQUARE")
        return ICON_FA_MINUS_SQUARE;
    if (name == "ICON_FA_MINUS_SQUARE_O")
        return ICON_FA_MINUS_SQUARE_O;
    if (name == "ICON_FA_LEVEL_UP")
        return ICON_FA_LEVEL_UP;
    if (name == "ICON_FA_LEVEL_DOWN")
        return ICON_FA_LEVEL_DOWN;
    if (name == "ICON_FA_CHECK_SQUARE")
        return ICON_FA_CHECK_SQUARE;
    if (name == "ICON_FA_PENCIL_SQUARE")
        return ICON_FA_PENCIL_SQUARE;
    if (name == "ICON_FA_EXTERNAL_LINK_SQUARE")
        return ICON_FA_EXTERNAL_LINK_SQUARE;
    if (name == "ICON_FA_SHARE_SQUARE")
        return ICON_FA_SHARE_SQUARE;
    if (name == "ICON_FA_COMPASS")
        return ICON_FA_COMPASS;
    if (name == "ICON_FA_CARET_SQUARE_O_DOWN")
        return ICON_FA_CARET_SQUARE_O_DOWN;
    if (name == "ICON_FA_CARET_SQUARE_O_UP")
        return ICON_FA_CARET_SQUARE_O_UP;
    if (name == "ICON_FA_CARET_SQUARE_O_RIGHT")
        return ICON_FA_CARET_SQUARE_O_RIGHT;
    if (name == "ICON_FA_EUR")
        return ICON_FA_EUR;
    if (name == "ICON_FA_GBP")
        return ICON_FA_GBP;
    if (name == "ICON_FA_USD")
        return ICON_FA_USD;
    if (name == "ICON_FA_INR")
        return ICON_FA_INR;
    if (name == "ICON_FA_JPY")
        return ICON_FA_JPY;
    if (name == "ICON_FA_RUB")
        return ICON_FA_RUB;
    if (name == "ICON_FA_KRW")
        return ICON_FA_KRW;
    if (name == "ICON_FA_BTC")
        return ICON_FA_BTC;
    if (name == "ICON_FA_FILE")
        return ICON_FA_FILE;
    if (name == "ICON_FA_FILE_TEXT")
        return ICON_FA_FILE_TEXT;
    if (name == "ICON_FA_SORT_ALPHA_ASC")
        return ICON_FA_SORT_ALPHA_ASC;
    if (name == "ICON_FA_SORT_ALPHA_DESC")
        return ICON_FA_SORT_ALPHA_DESC;
    if (name == "ICON_FA_SORT_AMOUNT_ASC")
        return ICON_FA_SORT_AMOUNT_ASC;
    if (name == "ICON_FA_SORT_AMOUNT_DESC")
        return ICON_FA_SORT_AMOUNT_DESC;
    if (name == "ICON_FA_SORT_NUMERIC_ASC")
        return ICON_FA_SORT_NUMERIC_ASC;
    if (name == "ICON_FA_SORT_NUMERIC_DESC")
        return ICON_FA_SORT_NUMERIC_DESC;
    if (name == "ICON_FA_THUMBS_UP")
        return ICON_FA_THUMBS_UP;
    if (name == "ICON_FA_THUMBS_DOWN")
        return ICON_FA_THUMBS_DOWN;
    if (name == "ICON_FA_YOUTUBE_SQUARE")
        return ICON_FA_YOUTUBE_SQUARE;
    if (name == "ICON_FA_YOUTUBE")
        return ICON_FA_YOUTUBE;
    if (name == "ICON_FA_XING")
        return ICON_FA_XING;
    if (name == "ICON_FA_XING_SQUARE")
        return ICON_FA_XING_SQUARE;
    if (name == "ICON_FA_YOUTUBE_PLAY")
        return ICON_FA_YOUTUBE_PLAY;
    if (name == "ICON_FA_DROPBOX")
        return ICON_FA_DROPBOX;
    if (name == "ICON_FA_STACK_OVERFLOW")
        return ICON_FA_STACK_OVERFLOW;
    if (name == "ICON_FA_INSTAGRAM")
        return ICON_FA_INSTAGRAM;
    if (name == "ICON_FA_FLICKR")
        return ICON_FA_FLICKR;
    if (name == "ICON_FA_ADN")
        return ICON_FA_ADN;
    if (name == "ICON_FA_BITBUCKET")
        return ICON_FA_BITBUCKET;
    if (name == "ICON_FA_BITBUCKET_SQUARE")
        return ICON_FA_BITBUCKET_SQUARE;
    if (name == "ICON_FA_TUMBLR")
        return ICON_FA_TUMBLR;
    if (name == "ICON_FA_TUMBLR_SQUARE")
        return ICON_FA_TUMBLR_SQUARE;
    if (name == "ICON_FA_LONG_ARROW_DOWN")
        return ICON_FA_LONG_ARROW_DOWN;
    if (name == "ICON_FA_LONG_ARROW_UP")
        return ICON_FA_LONG_ARROW_UP;
    if (name == "ICON_FA_LONG_ARROW_LEFT")
        return ICON_FA_LONG_ARROW_LEFT;
    if (name == "ICON_FA_LONG_ARROW_RIGHT")
        return ICON_FA_LONG_ARROW_RIGHT;
    if (name == "ICON_FA_APPLE")
        return ICON_FA_APPLE;
    if (name == "ICON_FA_WINDOWS")
        return ICON_FA_WINDOWS;
    if (name == "ICON_FA_ANDROID")
        return ICON_FA_ANDROID;
    if (name == "ICON_FA_LINUX")
        return ICON_FA_LINUX;
    if (name == "ICON_FA_DRIBBBLE")
        return ICON_FA_DRIBBBLE;
    if (name == "ICON_FA_SKYPE")
        return ICON_FA_SKYPE;
    if (name == "ICON_FA_FOURSQUARE")
        return ICON_FA_FOURSQUARE;
    if (name == "ICON_FA_TRELLO")
        return ICON_FA_TRELLO;
    if (name == "ICON_FA_FEMALE")
        return ICON_FA_FEMALE;
    if (name == "ICON_FA_MALE")
        return ICON_FA_MALE;
    if (name == "ICON_FA_GRATIPAY")
        return ICON_FA_GRATIPAY;
    if (name == "ICON_FA_SUN_O")
        return ICON_FA_SUN_O;
    if (name == "ICON_FA_MOON_O")
        return ICON_FA_MOON_O;
    if (name == "ICON_FA_ARCHIVE")
        return ICON_FA_ARCHIVE;
    if (name == "ICON_FA_BUG")
        return ICON_FA_BUG;
    if (name == "ICON_FA_VK")
        return ICON_FA_VK;
    if (name == "ICON_FA_WEIBO")
        return ICON_FA_WEIBO;
    if (name == "ICON_FA_RENREN")
        return ICON_FA_RENREN;
    if (name == "ICON_FA_PAGELINES")
        return ICON_FA_PAGELINES;
    if (name == "ICON_FA_STACK_EXCHANGE")
        return ICON_FA_STACK_EXCHANGE;
    if (name == "ICON_FA_ARROW_CIRCLE_O_RIGHT")
        return ICON_FA_ARROW_CIRCLE_O_RIGHT;
    if (name == "ICON_FA_ARROW_CIRCLE_O_LEFT")
        return ICON_FA_ARROW_CIRCLE_O_LEFT;
    if (name == "ICON_FA_CARET_SQUARE_O_LEFT")
        return ICON_FA_CARET_SQUARE_O_LEFT;
    if (name == "ICON_FA_DOT_CIRCLE_O")
        return ICON_FA_DOT_CIRCLE_O;
    if (name == "ICON_FA_WHEELCHAIR")
        return ICON_FA_WHEELCHAIR;
    if (name == "ICON_FA_VIMEO_SQUARE")
        return ICON_FA_VIMEO_SQUARE;
    if (name == "ICON_FA_TRY")
        return ICON_FA_TRY;
    if (name == "ICON_FA_PLUS_SQUARE_O")
        return ICON_FA_PLUS_SQUARE_O;
    if (name == "ICON_FA_SPACE_SHUTTLE")
        return ICON_FA_SPACE_SHUTTLE;
    if (name == "ICON_FA_SLACK")
        return ICON_FA_SLACK;
    if (name == "ICON_FA_ENVELOPE_SQUARE")
        return ICON_FA_ENVELOPE_SQUARE;
    if (name == "ICON_FA_WORDPRESS")
        return ICON_FA_WORDPRESS;
    if (name == "ICON_FA_OPENID")
        return ICON_FA_OPENID;
    if (name == "ICON_FA_UNIVERSITY")
        return ICON_FA_UNIVERSITY;
    if (name == "ICON_FA_GRADUATION_CAP")
        return ICON_FA_GRADUATION_CAP;
    if (name == "ICON_FA_YAHOO")
        return ICON_FA_YAHOO;
    if (name == "ICON_FA_GOOGLE")
        return ICON_FA_GOOGLE;
    if (name == "ICON_FA_REDDIT")
        return ICON_FA_REDDIT;
    if (name == "ICON_FA_REDDIT_SQUARE")
        return ICON_FA_REDDIT_SQUARE;
    if (name == "ICON_FA_STUMBLEUPON_CIRCLE")
        return ICON_FA_STUMBLEUPON_CIRCLE;
    if (name == "ICON_FA_STUMBLEUPON")
        return ICON_FA_STUMBLEUPON;
    if (name == "ICON_FA_DELICIOUS")
        return ICON_FA_DELICIOUS;
    if (name == "ICON_FA_DIGG")
        return ICON_FA_DIGG;
    if (name == "ICON_FA_PIED_PIPER_PP")
        return ICON_FA_PIED_PIPER_PP;
    if (name == "ICON_FA_PIED_PIPER_ALT")
        return ICON_FA_PIED_PIPER_ALT;
    if (name == "ICON_FA_DRUPAL")
        return ICON_FA_DRUPAL;
    if (name == "ICON_FA_JOOMLA")
        return ICON_FA_JOOMLA;
    if (name == "ICON_FA_LANGUAGE")
        return ICON_FA_LANGUAGE;
    if (name == "ICON_FA_FAX")
        return ICON_FA_FAX;
    if (name == "ICON_FA_BUILDING")
        return ICON_FA_BUILDING;
    if (name == "ICON_FA_CHILD")
        return ICON_FA_CHILD;
    if (name == "ICON_FA_PAW")
        return ICON_FA_PAW;
    if (name == "ICON_FA_SPOON")
        return ICON_FA_SPOON;
    if (name == "ICON_FA_CUBE")
        return ICON_FA_CUBE;
    if (name == "ICON_FA_CUBES")
        return ICON_FA_CUBES;
    if (name == "ICON_FA_BEHANCE")
        return ICON_FA_BEHANCE;
    if (name == "ICON_FA_BEHANCE_SQUARE")
        return ICON_FA_BEHANCE_SQUARE;
    if (name == "ICON_FA_STEAM")
        return ICON_FA_STEAM;
    if (name == "ICON_FA_STEAM_SQUARE")
        return ICON_FA_STEAM_SQUARE;
    if (name == "ICON_FA_RECYCLE")
        return ICON_FA_RECYCLE;
    if (name == "ICON_FA_CAR")
        return ICON_FA_CAR;
    if (name == "ICON_FA_TAXI")
        return ICON_FA_TAXI;
    if (name == "ICON_FA_TREE")
        return ICON_FA_TREE;
    if (name == "ICON_FA_SPOTIFY")
        return ICON_FA_SPOTIFY;
    if (name == "ICON_FA_DEVIANTART")
        return ICON_FA_DEVIANTART;
    if (name == "ICON_FA_SOUNDCLOUD")
        return ICON_FA_SOUNDCLOUD;
    if (name == "ICON_FA_DATABASE")
        return ICON_FA_DATABASE;
    if (name == "ICON_FA_FILE_PDF_O")
        return ICON_FA_FILE_PDF_O;
    if (name == "ICON_FA_FILE_WORD_O")
        return ICON_FA_FILE_WORD_O;
    if (name == "ICON_FA_FILE_EXCEL_O")
        return ICON_FA_FILE_EXCEL_O;
    if (name == "ICON_FA_FILE_POWERPOINT_O")
        return ICON_FA_FILE_POWERPOINT_O;
    if (name == "ICON_FA_FILE_IMAGE_O")
        return ICON_FA_FILE_IMAGE_O;
    if (name == "ICON_FA_FILE_ARCHIVE_O")
        return ICON_FA_FILE_ARCHIVE_O;
    if (name == "ICON_FA_FILE_AUDIO_O")
        return ICON_FA_FILE_AUDIO_O;
    if (name == "ICON_FA_FILE_VIDEO_O")
        return ICON_FA_FILE_VIDEO_O;
    if (name == "ICON_FA_FILE_CODE_O")
        return ICON_FA_FILE_CODE_O;
    if (name == "ICON_FA_VINE")
        return ICON_FA_VINE;
    if (name == "ICON_FA_CODEPEN")
        return ICON_FA_CODEPEN;
    if (name == "ICON_FA_JSFIDDLE")
        return ICON_FA_JSFIDDLE;
    if (name == "ICON_FA_LIFE_RING")
        return ICON_FA_LIFE_RING;
    if (name == "ICON_FA_CIRCLE_O_NOTCH")
        return ICON_FA_CIRCLE_O_NOTCH;
    if (name == "ICON_FA_REBEL")
        return ICON_FA_REBEL;
    if (name == "ICON_FA_EMPIRE")
        return ICON_FA_EMPIRE;
    if (name == "ICON_FA_GIT_SQUARE")
        return ICON_FA_GIT_SQUARE;
    if (name == "ICON_FA_GIT")
        return ICON_FA_GIT;
    if (name == "ICON_FA_HACKER_NEWS")
        return ICON_FA_HACKER_NEWS;
    if (name == "ICON_FA_TENCENT_WEIBO")
        return ICON_FA_TENCENT_WEIBO;
    if (name == "ICON_FA_QQ")
        return ICON_FA_QQ;
    if (name == "ICON_FA_WEIXIN")
        return ICON_FA_WEIXIN;
    if (name == "ICON_FA_PAPER_PLANE")
        return ICON_FA_PAPER_PLANE;
    if (name == "ICON_FA_PAPER_PLANE_O")
        return ICON_FA_PAPER_PLANE_O;
    if (name == "ICON_FA_HISTORY")
        return ICON_FA_HISTORY;
    if (name == "ICON_FA_CIRCLE_THIN")
        return ICON_FA_CIRCLE_THIN;
    if (name == "ICON_FA_HEADER")
        return ICON_FA_HEADER;
    if (name == "ICON_FA_PARAGRAPH")
        return ICON_FA_PARAGRAPH;
    if (name == "ICON_FA_SLIDERS")
        return ICON_FA_SLIDERS;
    if (name == "ICON_FA_SHARE_ALT")
        return ICON_FA_SHARE_ALT;
    if (name == "ICON_FA_SHARE_ALT_SQUARE")
        return ICON_FA_SHARE_ALT_SQUARE;
    if (name == "ICON_FA_BOMB")
        return ICON_FA_BOMB;
    if (name == "ICON_FA_FUTBOL_O")
        return ICON_FA_FUTBOL_O;
    if (name == "ICON_FA_TTY")
        return ICON_FA_TTY;
    if (name == "ICON_FA_BINOCULARS")
        return ICON_FA_BINOCULARS;
    if (name == "ICON_FA_PLUG")
        return ICON_FA_PLUG;
    if (name == "ICON_FA_SLIDESHARE")
        return ICON_FA_SLIDESHARE;
    if (name == "ICON_FA_TWITCH")
        return ICON_FA_TWITCH;
    if (name == "ICON_FA_YELP")
        return ICON_FA_YELP;
    if (name == "ICON_FA_NEWSPAPER_O")
        return ICON_FA_NEWSPAPER_O;
    if (name == "ICON_FA_WIFI")
        return ICON_FA_WIFI;
    if (name == "ICON_FA_CALCULATOR")
        return ICON_FA_CALCULATOR;
    if (name == "ICON_FA_PAYPAL")
        return ICON_FA_PAYPAL;
    if (name == "ICON_FA_GOOGLE_WALLET")
        return ICON_FA_GOOGLE_WALLET;
    if (name == "ICON_FA_CC_VISA")
        return ICON_FA_CC_VISA;
    if (name == "ICON_FA_CC_MASTERCARD")
        return ICON_FA_CC_MASTERCARD;
    if (name == "ICON_FA_CC_DISCOVER")
        return ICON_FA_CC_DISCOVER;
    if (name == "ICON_FA_CC_AMEX")
        return ICON_FA_CC_AMEX;
    if (name == "ICON_FA_CC_PAYPAL")
        return ICON_FA_CC_PAYPAL;
    if (name == "ICON_FA_CC_STRIPE")
        return ICON_FA_CC_STRIPE;
    if (name == "ICON_FA_BELL_SLASH")
        return ICON_FA_BELL_SLASH;
    if (name == "ICON_FA_BELL_SLASH_O")
        return ICON_FA_BELL_SLASH_O;
    if (name == "ICON_FA_TRASH")
        return ICON_FA_TRASH;
    if (name == "ICON_FA_COPYRIGHT")
        return ICON_FA_COPYRIGHT;
    if (name == "ICON_FA_AT")
        return ICON_FA_AT;
    if (name == "ICON_FA_EYEDROPPER")
        return ICON_FA_EYEDROPPER;
    if (name == "ICON_FA_PAINT_BRUSH")
        return ICON_FA_PAINT_BRUSH;
    if (name == "ICON_FA_BIRTHDAY_CAKE")
        return ICON_FA_BIRTHDAY_CAKE;
    if (name == "ICON_FA_AREA_CHART")
        return ICON_FA_AREA_CHART;
    if (name == "ICON_FA_PIE_CHART")
        return ICON_FA_PIE_CHART;
    if (name == "ICON_FA_LINE_CHART")
        return ICON_FA_LINE_CHART;
    if (name == "ICON_FA_LASTFM")
        return ICON_FA_LASTFM;
    if (name == "ICON_FA_LASTFM_SQUARE")
        return ICON_FA_LASTFM_SQUARE;
    if (name == "ICON_FA_TOGGLE_OFF")
        return ICON_FA_TOGGLE_OFF;
    if (name == "ICON_FA_TOGGLE_ON")
        return ICON_FA_TOGGLE_ON;
    if (name == "ICON_FA_BICYCLE")
        return ICON_FA_BICYCLE;
    if (name == "ICON_FA_BUS")
        return ICON_FA_BUS;
    if (name == "ICON_FA_IOXHOST")
        return ICON_FA_IOXHOST;
    if (name == "ICON_FA_ANGELLIST")
        return ICON_FA_ANGELLIST;
    if (name == "ICON_FA_CC")
        return ICON_FA_CC;
    if (name == "ICON_FA_ILS")
        return ICON_FA_ILS;
    if (name == "ICON_FA_MEANPATH")
        return ICON_FA_MEANPATH;
    if (name == "ICON_FA_BUYSELLADS")
        return ICON_FA_BUYSELLADS;
    if (name == "ICON_FA_CONNECTDEVELOP")
        return ICON_FA_CONNECTDEVELOP;
    if (name == "ICON_FA_DASHCUBE")
        return ICON_FA_DASHCUBE;
    if (name == "ICON_FA_FORUMBEE")
        return ICON_FA_FORUMBEE;
    if (name == "ICON_FA_LEANPUB")
        return ICON_FA_LEANPUB;
    if (name == "ICON_FA_SELLSY")
        return ICON_FA_SELLSY;
    if (name == "ICON_FA_SHIRTSINBULK")
        return ICON_FA_SHIRTSINBULK;
    if (name == "ICON_FA_SIMPLYBUILT")
        return ICON_FA_SIMPLYBUILT;
    if (name == "ICON_FA_SKYATLAS")
        return ICON_FA_SKYATLAS;
    if (name == "ICON_FA_CART_PLUS")
        return ICON_FA_CART_PLUS;
    if (name == "ICON_FA_CART_ARROW_DOWN")
        return ICON_FA_CART_ARROW_DOWN;
    if (name == "ICON_FA_DIAMOND")
        return ICON_FA_DIAMOND;
    if (name == "ICON_FA_SHIP")
        return ICON_FA_SHIP;
    if (name == "ICON_FA_USER_SECRET")
        return ICON_FA_USER_SECRET;
    if (name == "ICON_FA_MOTORCYCLE")
        return ICON_FA_MOTORCYCLE;
    if (name == "ICON_FA_STREET_VIEW")
        return ICON_FA_STREET_VIEW;
    if (name == "ICON_FA_HEARTBEAT")
        return ICON_FA_HEARTBEAT;
    if (name == "ICON_FA_VENUS")
        return ICON_FA_VENUS;
    if (name == "ICON_FA_MARS")
        return ICON_FA_MARS;
    if (name == "ICON_FA_MERCURY")
        return ICON_FA_MERCURY;
    if (name == "ICON_FA_TRANSGENDER")
        return ICON_FA_TRANSGENDER;
    if (name == "ICON_FA_TRANSGENDER_ALT")
        return ICON_FA_TRANSGENDER_ALT;
    if (name == "ICON_FA_VENUS_DOUBLE")
        return ICON_FA_VENUS_DOUBLE;
    if (name == "ICON_FA_MARS_DOUBLE")
        return ICON_FA_MARS_DOUBLE;
    if (name == "ICON_FA_VENUS_MARS")
        return ICON_FA_VENUS_MARS;
    if (name == "ICON_FA_MARS_STROKE")
        return ICON_FA_MARS_STROKE;
    if (name == "ICON_FA_MARS_STROKE_V")
        return ICON_FA_MARS_STROKE_V;
    if (name == "ICON_FA_MARS_STROKE_H")
        return ICON_FA_MARS_STROKE_H;
    if (name == "ICON_FA_NEUTER")
        return ICON_FA_NEUTER;
    if (name == "ICON_FA_GENDERLESS")
        return ICON_FA_GENDERLESS;
    if (name == "ICON_FA_FACEBOOK_OFFICIAL")
        return ICON_FA_FACEBOOK_OFFICIAL;
    if (name == "ICON_FA_PINTEREST_P")
        return ICON_FA_PINTEREST_P;
    if (name == "ICON_FA_WHATSAPP")
        return ICON_FA_WHATSAPP;
    if (name == "ICON_FA_SERVER")
        return ICON_FA_SERVER;
    if (name == "ICON_FA_USER_PLUS")
        return ICON_FA_USER_PLUS;
    if (name == "ICON_FA_USER_TIMES")
        return ICON_FA_USER_TIMES;
    if (name == "ICON_FA_BED")
        return ICON_FA_BED;
    if (name == "ICON_FA_VIACOIN")
        return ICON_FA_VIACOIN;
    if (name == "ICON_FA_TRAIN")
        return ICON_FA_TRAIN;
    if (name == "ICON_FA_SUBWAY")
        return ICON_FA_SUBWAY;
    if (name == "ICON_FA_MEDIUM")
        return ICON_FA_MEDIUM;
    if (name == "ICON_FA_Y_COMBINATOR")
        return ICON_FA_Y_COMBINATOR;
    if (name == "ICON_FA_OPTIN_MONSTER")
        return ICON_FA_OPTIN_MONSTER;
    if (name == "ICON_FA_OPENCART")
        return ICON_FA_OPENCART;
    if (name == "ICON_FA_EXPEDITEDSSL")
        return ICON_FA_EXPEDITEDSSL;
    if (name == "ICON_FA_BATTERY_FULL")
        return ICON_FA_BATTERY_FULL;
    if (name == "ICON_FA_BATTERY_THREE_QUARTERS")
        return ICON_FA_BATTERY_THREE_QUARTERS;
    if (name == "ICON_FA_BATTERY_HALF")
        return ICON_FA_BATTERY_HALF;
    if (name == "ICON_FA_BATTERY_QUARTER")
        return ICON_FA_BATTERY_QUARTER;
    if (name == "ICON_FA_BATTERY_EMPTY")
        return ICON_FA_BATTERY_EMPTY;
    if (name == "ICON_FA_MOUSE_POINTER")
        return ICON_FA_MOUSE_POINTER;
    if (name == "ICON_FA_I_CURSOR")
        return ICON_FA_I_CURSOR;
    if (name == "ICON_FA_OBJECT_GROUP")
        return ICON_FA_OBJECT_GROUP;
    if (name == "ICON_FA_OBJECT_UNGROUP")
        return ICON_FA_OBJECT_UNGROUP;
    if (name == "ICON_FA_STICKY_NOTE")
        return ICON_FA_STICKY_NOTE;
    if (name == "ICON_FA_STICKY_NOTE_O")
        return ICON_FA_STICKY_NOTE_O;
    if (name == "ICON_FA_CC_JCB")
        return ICON_FA_CC_JCB;
    if (name == "ICON_FA_CC_DINERS_CLUB")
        return ICON_FA_CC_DINERS_CLUB;
    if (name == "ICON_FA_CLONE")
        return ICON_FA_CLONE;
    if (name == "ICON_FA_BALANCE_SCALE")
        return ICON_FA_BALANCE_SCALE;
    if (name == "ICON_FA_HOURGLASS_O")
        return ICON_FA_HOURGLASS_O;
    if (name == "ICON_FA_HOURGLASS_START")
        return ICON_FA_HOURGLASS_START;
    if (name == "ICON_FA_HOURGLASS_HALF")
        return ICON_FA_HOURGLASS_HALF;
    if (name == "ICON_FA_HOURGLASS_END")
        return ICON_FA_HOURGLASS_END;
    if (name == "ICON_FA_HOURGLASS")
        return ICON_FA_HOURGLASS;
    if (name == "ICON_FA_HAND_ROCK_O")
        return ICON_FA_HAND_ROCK_O;
    if (name == "ICON_FA_HAND_PAPER_O")
        return ICON_FA_HAND_PAPER_O;
    if (name == "ICON_FA_HAND_SCISSORS_O")
        return ICON_FA_HAND_SCISSORS_O;
    if (name == "ICON_FA_HAND_LIZARD_O")
        return ICON_FA_HAND_LIZARD_O;
    if (name == "ICON_FA_HAND_SPOCK_O")
        return ICON_FA_HAND_SPOCK_O;
    if (name == "ICON_FA_HAND_POINTER_O")
        return ICON_FA_HAND_POINTER_O;
    if (name == "ICON_FA_HAND_PEACE_O")
        return ICON_FA_HAND_PEACE_O;
    if (name == "ICON_FA_TRADEMARK")
        return ICON_FA_TRADEMARK;
    if (name == "ICON_FA_REGISTERED")
        return ICON_FA_REGISTERED;
    if (name == "ICON_FA_CREATIVE_COMMONS")
        return ICON_FA_CREATIVE_COMMONS;
    if (name == "ICON_FA_GG")
        return ICON_FA_GG;
    if (name == "ICON_FA_GG_CIRCLE")
        return ICON_FA_GG_CIRCLE;
    if (name == "ICON_FA_TRIPADVISOR")
        return ICON_FA_TRIPADVISOR;
    if (name == "ICON_FA_ODNOKLASSNIKI")
        return ICON_FA_ODNOKLASSNIKI;
    if (name == "ICON_FA_ODNOKLASSNIKI_SQUARE")
        return ICON_FA_ODNOKLASSNIKI_SQUARE;
    if (name == "ICON_FA_GET_POCKET")
        return ICON_FA_GET_POCKET;
    if (name == "ICON_FA_WIKIPEDIA_W")
        return ICON_FA_WIKIPEDIA_W;
    if (name == "ICON_FA_SAFARI")
        return ICON_FA_SAFARI;
    if (name == "ICON_FA_CHROME")
        return ICON_FA_CHROME;
    if (name == "ICON_FA_FIREFOX")
        return ICON_FA_FIREFOX;
    if (name == "ICON_FA_OPERA")
        return ICON_FA_OPERA;
    if (name == "ICON_FA_INTERNET_EXPLORER")
        return ICON_FA_INTERNET_EXPLORER;
    if (name == "ICON_FA_TELEVISION")
        return ICON_FA_TELEVISION;
    if (name == "ICON_FA_CONTAO")
        return ICON_FA_CONTAO;
    if (name == "ICON_FA_500PX")
        return ICON_FA_500PX;
    if (name == "ICON_FA_AMAZON")
        return ICON_FA_AMAZON;
    if (name == "ICON_FA_CALENDAR_PLUS_O")
        return ICON_FA_CALENDAR_PLUS_O;
    if (name == "ICON_FA_CALENDAR_MINUS_O")
        return ICON_FA_CALENDAR_MINUS_O;
    if (name == "ICON_FA_CALENDAR_TIMES_O")
        return ICON_FA_CALENDAR_TIMES_O;
    if (name == "ICON_FA_CALENDAR_CHECK_O")
        return ICON_FA_CALENDAR_CHECK_O;
    if (name == "ICON_FA_INDUSTRY")
        return ICON_FA_INDUSTRY;
    if (name == "ICON_FA_MAP_PIN")
        return ICON_FA_MAP_PIN;
    if (name == "ICON_FA_MAP_SIGNS")
        return ICON_FA_MAP_SIGNS;
    if (name == "ICON_FA_MAP_O")
        return ICON_FA_MAP_O;
    if (name == "ICON_FA_MAP")
        return ICON_FA_MAP;
    if (name == "ICON_FA_COMMENTING")
        return ICON_FA_COMMENTING;
    if (name == "ICON_FA_COMMENTING_O")
        return ICON_FA_COMMENTING_O;
    if (name == "ICON_FA_HOUZZ")
        return ICON_FA_HOUZZ;
    if (name == "ICON_FA_VIMEO")
        return ICON_FA_VIMEO;
    if (name == "ICON_FA_BLACK_TIE")
        return ICON_FA_BLACK_TIE;
    if (name == "ICON_FA_FONTICONS")
        return ICON_FA_FONTICONS;
    if (name == "ICON_FA_REDDIT_ALIEN")
        return ICON_FA_REDDIT_ALIEN;
    if (name == "ICON_FA_EDGE")
        return ICON_FA_EDGE;
    if (name == "ICON_FA_CREDIT_CARD_ALT")
        return ICON_FA_CREDIT_CARD_ALT;
    if (name == "ICON_FA_CODIEPIE")
        return ICON_FA_CODIEPIE;
    if (name == "ICON_FA_MODX")
        return ICON_FA_MODX;
    if (name == "ICON_FA_FORT_AWESOME")
        return ICON_FA_FORT_AWESOME;
    if (name == "ICON_FA_USB")
        return ICON_FA_USB;
    if (name == "ICON_FA_PRODUCT_HUNT")
        return ICON_FA_PRODUCT_HUNT;
    if (name == "ICON_FA_MIXCLOUD")
        return ICON_FA_MIXCLOUD;
    if (name == "ICON_FA_SCRIBD")
        return ICON_FA_SCRIBD;
    if (name == "ICON_FA_PAUSE_CIRCLE")
        return ICON_FA_PAUSE_CIRCLE;
    if (name == "ICON_FA_PAUSE_CIRCLE_O")
        return ICON_FA_PAUSE_CIRCLE_O;
    if (name == "ICON_FA_STOP_CIRCLE")
        return ICON_FA_STOP_CIRCLE;
    if (name == "ICON_FA_STOP_CIRCLE_O")
        return ICON_FA_STOP_CIRCLE_O;
    if (name == "ICON_FA_SHOPPING_BAG")
        return ICON_FA_SHOPPING_BAG;
    if (name == "ICON_FA_SHOPPING_BASKET")
        return ICON_FA_SHOPPING_BASKET;
    if (name == "ICON_FA_HASHTAG")
        return ICON_FA_HASHTAG;
    if (name == "ICON_FA_BLUETOOTH")
        return ICON_FA_BLUETOOTH;
    if (name == "ICON_FA_BLUETOOTH_B")
        return ICON_FA_BLUETOOTH_B;
    if (name == "ICON_FA_PERCENT")
        return ICON_FA_PERCENT;
    if (name == "ICON_FA_GITLAB")
        return ICON_FA_GITLAB;
    if (name == "ICON_FA_WPBEGINNER")
        return ICON_FA_WPBEGINNER;
    if (name == "ICON_FA_WPFORMS")
        return ICON_FA_WPFORMS;
    if (name == "ICON_FA_ENVIRA")
        return ICON_FA_ENVIRA;
    if (name == "ICON_FA_UNIVERSAL_ACCESS")
        return ICON_FA_UNIVERSAL_ACCESS;
    if (name == "ICON_FA_WHEELCHAIR_ALT")
        return ICON_FA_WHEELCHAIR_ALT;
    if (name == "ICON_FA_QUESTION_CIRCLE_O")
        return ICON_FA_QUESTION_CIRCLE_O;
    if (name == "ICON_FA_BLIND")
        return ICON_FA_BLIND;
    if (name == "ICON_FA_AUDIO_DESCRIPTION")
        return ICON_FA_AUDIO_DESCRIPTION;
    if (name == "ICON_FA_VOLUME_CONTROL_PHONE")
        return ICON_FA_VOLUME_CONTROL_PHONE;
    if (name == "ICON_FA_BRAILLE")
        return ICON_FA_BRAILLE;
    if (name == "ICON_FA_ASSISTIVE_LISTENING_SYSTEMS")
        return ICON_FA_ASSISTIVE_LISTENING_SYSTEMS;
    if (name == "ICON_FA_AMERICAN_SIGN_LANGUAGE_INTERPRETING")
        return ICON_FA_AMERICAN_SIGN_LANGUAGE_INTERPRETING;
    if (name == "ICON_FA_DEAF")
        return ICON_FA_DEAF;
    if (name == "ICON_FA_GLIDE")
        return ICON_FA_GLIDE;
    if (name == "ICON_FA_GLIDE_G")
        return ICON_FA_GLIDE_G;
    if (name == "ICON_FA_SIGN_LANGUAGE")
        return ICON_FA_SIGN_LANGUAGE;
    if (name == "ICON_FA_LOW_VISION")
        return ICON_FA_LOW_VISION;
    if (name == "ICON_FA_VIADEO")
        return ICON_FA_VIADEO;
    if (name == "ICON_FA_VIADEO_SQUARE")
        return ICON_FA_VIADEO_SQUARE;
    if (name == "ICON_FA_SNAPCHAT")
        return ICON_FA_SNAPCHAT;
    if (name == "ICON_FA_SNAPCHAT_GHOST")
        return ICON_FA_SNAPCHAT_GHOST;
    if (name == "ICON_FA_SNAPCHAT_SQUARE")
        return ICON_FA_SNAPCHAT_SQUARE;
    if (name == "ICON_FA_PIED_PIPER")
        return ICON_FA_PIED_PIPER;
    if (name == "ICON_FA_FIRST_ORDER")
        return ICON_FA_FIRST_ORDER;
    if (name == "ICON_FA_YOAST")
        return ICON_FA_YOAST;
    if (name == "ICON_FA_THEMEISLE")
        return ICON_FA_THEMEISLE;
    if (name == "ICON_FA_GOOGLE_PLUS_OFFICIAL")
        return ICON_FA_GOOGLE_PLUS_OFFICIAL;
    if (name == "ICON_FA_FONT_AWESOME")
        return ICON_FA_FONT_AWESOME;
    if (name == "ICON_FA_HANDSHAKE_O")
        return ICON_FA_HANDSHAKE_O;
    if (name == "ICON_FA_ENVELOPE_OPEN")
        return ICON_FA_ENVELOPE_OPEN;
    if (name == "ICON_FA_ENVELOPE_OPEN_O")
        return ICON_FA_ENVELOPE_OPEN_O;
    if (name == "ICON_FA_LINODE")
        return ICON_FA_LINODE;
    if (name == "ICON_FA_ADDRESS_BOOK")
        return ICON_FA_ADDRESS_BOOK;
    if (name == "ICON_FA_ADDRESS_BOOK_O")
        return ICON_FA_ADDRESS_BOOK_O;
    if (name == "ICON_FA_ADDRESS_CARD")
        return ICON_FA_ADDRESS_CARD;
    if (name == "ICON_FA_ADDRESS_CARD_O")
        return ICON_FA_ADDRESS_CARD_O;
    if (name == "ICON_FA_USER_CIRCLE")
        return ICON_FA_USER_CIRCLE;
    if (name == "ICON_FA_USER_CIRCLE_O")
        return ICON_FA_USER_CIRCLE_O;
    if (name == "ICON_FA_USER_O")
        return ICON_FA_USER_O;
    if (name == "ICON_FA_ID_BADGE")
        return ICON_FA_ID_BADGE;
    if (name == "ICON_FA_ID_CARD")
        return ICON_FA_ID_CARD;
    if (name == "ICON_FA_ID_CARD_O")
        return ICON_FA_ID_CARD_O;
    if (name == "ICON_FA_QUORA")
        return ICON_FA_QUORA;
    if (name == "ICON_FA_FREE_CODE_CAMP")
        return ICON_FA_FREE_CODE_CAMP;
    if (name == "ICON_FA_TELEGRAM")
        return ICON_FA_TELEGRAM;
    if (name == "ICON_FA_THERMOMETER_FULL")
        return ICON_FA_THERMOMETER_FULL;
    if (name == "ICON_FA_THERMOMETER_THREE_QUARTERS")
        return ICON_FA_THERMOMETER_THREE_QUARTERS;
    if (name == "ICON_FA_THERMOMETER_HALF")
        return ICON_FA_THERMOMETER_HALF;
    if (name == "ICON_FA_THERMOMETER_QUARTER")
        return ICON_FA_THERMOMETER_QUARTER;
    if (name == "ICON_FA_THERMOMETER_EMPTY")
        return ICON_FA_THERMOMETER_EMPTY;
    if (name == "ICON_FA_SHOWER")
        return ICON_FA_SHOWER;
    if (name == "ICON_FA_BATH")
        return ICON_FA_BATH;
    if (name == "ICON_FA_PODCAST")
        return ICON_FA_PODCAST;
    if (name == "ICON_FA_WINDOW_MAXIMIZE")
        return ICON_FA_WINDOW_MAXIMIZE;
    if (name == "ICON_FA_WINDOW_MINIMIZE")
        return ICON_FA_WINDOW_MINIMIZE;
    if (name == "ICON_FA_WINDOW_RESTORE")
        return ICON_FA_WINDOW_RESTORE;
    if (name == "ICON_FA_WINDOW_CLOSE")
        return ICON_FA_WINDOW_CLOSE;
    if (name == "ICON_FA_WINDOW_CLOSE_O")
        return ICON_FA_WINDOW_CLOSE_O;
    if (name == "ICON_FA_BANDCAMP")
        return ICON_FA_BANDCAMP;
    if (name == "ICON_FA_GRAV")
        return ICON_FA_GRAV;
    if (name == "ICON_FA_ETSY")
        return ICON_FA_ETSY;
    if (name == "ICON_FA_IMDB")
        return ICON_FA_IMDB;
    if (name == "ICON_FA_RAVELRY")
        return ICON_FA_RAVELRY;
    if (name == "ICON_FA_EERCAST")
        return ICON_FA_EERCAST;
    if (name == "ICON_FA_MICROCHIP")
        return ICON_FA_MICROCHIP;
    if (name == "ICON_FA_SNOWFLAKE_O")
        return ICON_FA_SNOWFLAKE_O;
    if (name == "ICON_FA_SUPERPOWERS")
        return ICON_FA_SUPERPOWERS;
    if (name == "ICON_FA_WPEXPLORER")
        return ICON_FA_WPEXPLORER;
    if (name == "ICON_FA_MEETUP")
        return ICON_FA_MEETUP;
    return name;
}

static std::map<int32_t, const char*> availableChecksOnlyShowOptions = { { AC_SHOW_ALL_CHECKS, "All Checks" },
                                                                         { AC_SHOW_AVAILABLE_CHECKS, "Either Age" },
                                                                         { AC_SHOW_CHILD_CHECKS, "Child Checks" },
                                                                         { AC_SHOW_ADULT_CHECKS, "Adult Checks" },
                                                                         { AC_SHOW_CURRENT_AGE, "Current Age" } };

void InternalRecalculateAvailableChecks(RandomizerRegion startingRegion, RandoAgeTime startingAgeTime);

void CheckTrackerWindow::DrawElement() {
    Color_Background = CVarGetColor(CVAR_TRACKER_CHECK("BgColor.Value"), Color_Bg_Default);
    Color_Area_Incomplete_Main = CVarGetColor(CVAR_TRACKER_CHECK("AreaIncomplete.MainColor.Value"), Color_Main_Default);
    Color_Area_Incomplete_Extra =
        CVarGetColor(CVAR_TRACKER_CHECK("AreaIncomplete.ExtraColor.Value"), Color_Area_Incomplete_Extra_Default);
    Color_Area_Complete_Main = CVarGetColor(CVAR_TRACKER_CHECK("AreaComplete.MainColor.Value"), Color_Main_Default);
    Color_Area_Complete_Extra =
        CVarGetColor(CVAR_TRACKER_CHECK("AreaComplete.ExtraColor.Value"), Color_Area_Complete_Extra_Default);
    Color_Unchecked_Main = CVarGetColor(CVAR_TRACKER_CHECK("Unchecked.MainColor.Value"), Color_Main_Default);
    Color_Unchecked_Extra =
        CVarGetColor(CVAR_TRACKER_CHECK("Unchecked.ExtraColor.Value"), Color_Unchecked_Extra_Default);
    Color_Skipped_Main = CVarGetColor(CVAR_TRACKER_CHECK("Skipped.MainColor.Value"), Color_Main_Default);
    Color_Skipped_Extra = CVarGetColor(CVAR_TRACKER_CHECK("Skipped.ExtraColor.Value"), Color_Skipped_Extra_Default);
    Color_Seen_Main = CVarGetColor(CVAR_TRACKER_CHECK("Seen.MainColor.Value"), Color_Main_Default);
    Color_Seen_Extra = CVarGetColor(CVAR_TRACKER_CHECK("Seen.ExtraColor.Value"), Color_Seen_Extra_Default);
    Color_Hinted_Main = CVarGetColor(CVAR_TRACKER_CHECK("Hinted.MainColor.Value"), Color_Main_Default);
    Color_Hinted_Extra = CVarGetColor(CVAR_TRACKER_CHECK("Hinted.ExtraColor.Value"), Color_Hinted_Extra_Default);
    Color_Collected_Main = CVarGetColor(CVAR_TRACKER_CHECK("Collected.MainColor.Value"), Color_Main_Default);
    Color_Collected_Extra =
        CVarGetColor(CVAR_TRACKER_CHECK("Collected.ExtraColor.Value"), Color_Collected_Extra_Default);
    Color_Scummed_Main = CVarGetColor(CVAR_TRACKER_CHECK("Scummed.MainColor.Value"), Color_Main_Default);
    Color_Scummed_Extra = CVarGetColor(CVAR_TRACKER_CHECK("Scummed.ExtraColor.Value"), Color_Scummed_Extra_Default);
    Color_Saved_Main = CVarGetColor(CVAR_TRACKER_CHECK("Saved.MainColor.Value"), Color_Main_Default);
    Color_Saved_Extra = CVarGetColor(CVAR_TRACKER_CHECK("Saved.ExtraColor.Value"), Color_Saved_Extra_Default);
    hideUnchecked = CVarGetInteger(CVAR_TRACKER_CHECK("Unchecked.Hide"), 0);
    hideScummed = CVarGetInteger(CVAR_TRACKER_CHECK("Scummed.Hide"), 0);
    hideSeen = CVarGetInteger(CVAR_TRACKER_CHECK("Seen.Hide"), 0);
    hideSkipped = CVarGetInteger(CVAR_TRACKER_CHECK("Skipped.Hide"), 0);
    hideSaved = CVarGetInteger(CVAR_TRACKER_CHECK("Saved.Hide"), 0);
    hideCollected = CVarGetInteger(CVAR_TRACKER_CHECK("Collected.Hide"), 0);
    showHidden = CVarGetInteger(CVAR_TRACKER_CHECK("ShowHidden"), 0);
    mystery = CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0);
    showLogicTooltip = CVarGetInteger(CVAR_TRACKER_CHECK("ShowLogic"), 0);
    ageChecksDisplay = (AvailableChecksDisplay)CVarGetInteger(CVAR_TRACKER_CHECK("AgeChecksDisplay"), 0);
    availableChecksDisplay = (AvailableChecksDisplay)CVarGetInteger(CVAR_TRACKER_CHECK("AvailableChecksDisplay"), 0);
    availableChecksNoneAvailableIcon =
        MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksNoneIcon"), "ICON_FA_LOCK"));
    availableChecksChildAvailableIcon =
        MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksChildIcon"), "ICON_FA_CHILD"));
    availableChecksAdultAvailableIcon =
        MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksAdultIcon"), "ICON_FA_USER"));
    availableChecksBothAvailableIcon =
        MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksBothIcon"), "ICON_FA_USERS"));
    availableChecksOnlyShow = (AvailableChecksOnlyShow)CVarGetInteger(CVAR_TRACKER_CHECK("AvailableChecksOnlyShow"), 0);

    hideShopUnshuffledChecks = CVarGetInteger(CVAR_TRACKER_CHECK("HideUnshuffledShopChecks"), 0);
    alwaysShowGS = CVarGetInteger(CVAR_TRACKER_CHECK("AlwaysShowGSLocs"), 0);
    if (CVarGetInteger(CVAR_TRACKER_CHECK("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        if (CVarGetInteger(CVAR_TRACKER_CHECK("ShowOnlyPaused"), 0) &&
            (gPlayState == nullptr || gPlayState->pauseCtx.state == 0)) {
            return;
        }

        if (CVarGetInteger(CVAR_TRACKER_CHECK("DisplayType"), TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            int comboButton1Mask = buttons[CVarGetInteger(CVAR_TRACKER_CHECK("ComboButton1"), TRACKER_COMBO_BUTTON_L)];
            int comboButton2Mask = buttons[CVarGetInteger(CVAR_TRACKER_CHECK("ComboButton2"), TRACKER_COMBO_BUTTON_R)];
            OSContPad* trackerButtonsPressed =
                std::dynamic_pointer_cast<LUS::ControlDeck>(Ship::Context::GetInstance()->GetControlDeck())->GetPads();
            bool comboButtonsHeld = trackerButtonsPressed != nullptr &&
                                    trackerButtonsPressed[0].button & comboButton1Mask &&
                                    trackerButtonsPressed[0].button & comboButton2Mask;
            if (!comboButtonsHeld) {
                return;
            }
        }
    }

    if (presetLoaded) {
        ImGui::SetNextWindowSize(presetSize);
        ImGui::SetNextWindowPos(presetPos);
        presetLoaded = false;
    } else {
        ImGui::SetNextWindowSize(ImVec2(400, 540), ImGuiCond_FirstUseEver);
    }
    if (Trackers::BeginFloatWindows(
            "Check Tracker", mIsVisible, Color_Background,
            static_cast<TrackerWindowType>(CVarGetInteger(CVAR_TRACKER_CHECK("WindowType"), TRACKER_WINDOW_WINDOW)),
            CVarGetInteger(CVAR_TRACKER_CHECK("Draggable"), 1), ImGuiWindowFlags_NoScrollbar)) {
        if (!GameInteractor::IsSaveLoaded() || !initialized) {
            ImGui::Text("Waiting for file load..."); // TODO Language
            Trackers::EndFloatWindows();
            return;
        }

        if (gSaveContext.linkAge != previousAge) {
            previousAge = gSaveContext.linkAge;
            recalculateAvailable = true;
        }

        if (recalculateAvailable) {
            recalculateAvailable = false;
            InternalRecalculateAvailableChecks(availableChecksStartingRegion, availableChecksStartingAgeTime);
            availableChecksStartingRegion = RR_ROOT;
            availableChecksStartingAgeTime = RAT_NONE;
        }

        // Quick Options
#ifdef __WIIU__
        float headerHeight = 40.0f;
#else
        float headerHeight = 20.0f;
#endif
        if (!ImGui::BeginTable("Check Tracker", 1, 0)) {
            Trackers::EndFloatWindows();
            return;
        }

        ImGui::SetWindowFontScale(CVarGetFloat(CVAR_TRACKER_CHECK("FontSize"), 1.0f));

        ImGui::TableNextRow(0, 0);
        ImGui::TableNextColumn();
        if (CVarGetInteger(CVAR_TRACKER_CHECK("HiddenItemsToggleVisible"), 1) &&
            UIWidgets::CVarCheckbox(
                "Show Hidden Items", CVAR_TRACKER_CHECK("ShowHidden"),
                UIWidgets::CheckboxOptions(
                    { { .tooltip =
                            "When active, items will show hidden checks by default when updated to this state." } })
                    .Color(THEME_COLOR))) {
            doAreaScroll = true;
            showHidden = CVarGetInteger(CVAR_TRACKER_CHECK("ShowHidden"), 0);
            RecalculateAllAreaTotals();
        }
        if (availableChecksDisplay != AC_DISABLED &&
            CVarGetInteger(CVAR_TRACKER_CHECK("AvailableChecksToggleVisible"), 1)) {
            if (UIWidgets::CVarCombobox("Only Show Available Checks", CVAR_TRACKER_CHECK("AvailableChecksOnlyShow"),
                                        availableChecksOnlyShowOptions,
                                        UIWidgets::ComboboxOptions()
                                            .LabelPosition(UIWidgets::LabelPositions::Near)
                                            .ComponentAlignment(UIWidgets::ComponentAlignments::Right)
                                            .Color(THEME_COLOR)
                                            .DefaultIndex(AC_SHOW_ALL_CHECKS))) {
                availableChecksOnlyShow = (AvailableChecksOnlyShow)CVarGetInteger(
                    CVAR_TRACKER_CHECK("AvailableChecksOnlyShow"), AC_SHOW_ALL_CHECKS);
                doAreaScroll = true;
                RecalculateAllAreaTotals();
            }
        }
        if (CVarGetInteger(CVAR_TRACKER_CHECK("ExpandCollapseButtonsVisible"), 0)) {
            if (UIWidgets::Button("Expand All", UIWidgets::ButtonOptions()
                                                    .Color(THEME_COLOR)
                                                    .Size({ ImGui::GetContentRegionAvail().x / 2 - 6, 0 }))) {
                optCollapseAll = false;
                optExpandAll = true;
                doAreaScroll = true;
            }
            ImGui::SameLine();
            if (UIWidgets::Button(
                    "Collapse All",
                    UIWidgets::ButtonOptions().Color(THEME_COLOR).Size({ ImGui::GetContentRegionAvail().x - 6, 0 }))) {
                optExpandAll = false;
                optCollapseAll = true;
            }
        }
        UIWidgets::PushStyleCombobox(THEME_COLOR);
        if (CVarGetInteger(CVAR_TRACKER_CHECK("SearchInputVisible"), 1)) {
            if (checkSearch.Draw("", ImGui::GetContentRegionAvail().x - 42)) {
                UpdateFilters();
            }
            std::string checkSearchText = checkSearch.InputBuf;
            checkSearchText.erase(std::remove(checkSearchText.begin(), checkSearchText.end(), ' '),
                                  checkSearchText.end());
            ImGui::SameLine();
            if (UIWidgets::Button(ICON_FA_ERASER, UIWidgets::ButtonOptions()
                                                      .Size(UIWidgets::Sizes::Inline)
                                                      .Color(THEME_COLOR)
                                                      .Padding(ImVec2(10.f, 6.f)))) {
                checkSearch.Clear();
                UpdateFilters();
                doAreaScroll = true;
            }
            if (checkSearchText.length() < 1) {
                ImGui::SameLine(20.0f);
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), "Search...");
            }
        }
        UIWidgets::PopStyleCombobox();

        if (CVarGetInteger(CVAR_TRACKER_CHECK("CheckTotalsVisible"), 1)) {
            std::ostringstream totalChecksSS;
            totalChecksSS << "";
            if (availableChecksDisplay != AC_DISABLED) {
                totalChecksSS << totalChecksAvailable << " Available / ";
            }
            totalChecksSS << totalChecksGotten << " Checked / " << totalChecks << " Total";
            ImGui::Text("%s", totalChecksSS.str().c_str());
        }

        bool headerPresent = CVarGetInteger(CVAR_TRACKER_CHECK("HiddenItemsToggleVisible"), 1) ||
                             (availableChecksDisplay != AC_DISABLED &&
                              CVarGetInteger(CVAR_TRACKER_CHECK("AvailableChecksToggleVisible"), 1)) ||
                             CVarGetInteger(CVAR_TRACKER_CHECK("ExpandCollapseButtonsVisible"), 0) ||
                             CVarGetInteger(CVAR_TRACKER_CHECK("SearchInputVisible"), 1) ||
                             CVarGetInteger(CVAR_TRACKER_CHECK("CheckTotalsVisible"), 1);
        if (headerPresent) {
            ImGui::Separator();
        }

        // Checks Section Lead-in
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (!ImGui::BeginTable("CheckTracker##Checks", 1, ImGuiTableFlags_ScrollY)) {
            ImGui::EndTable();
            Trackers::EndFloatWindows();
            return;
        }
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        // Prep for loop
        RainbowTick();
        bool doDraw = false;
        bool thisAreaFullyChecked = false;
        bool mqSpoilers = CVarGetInteger(CVAR_TRACKER_CHECK("MQSpoilers"), 0);
        bool hideIncomplete = CVarGetInteger(CVAR_TRACKER_CHECK("AreaIncomplete.Hide"), 0);
        bool hideComplete = CVarGetInteger(CVAR_TRACKER_CHECK("AreaComplete.Hide"), 0);
        bool collapseLogic;
        bool doingCollapseOrExpand = optExpandAll || optCollapseAll;
        bool isThisAreaSpoiled;
        RandomizerCheckArea lastArea = RCAREA_INVALID;
        Color_RGBA8 mainColor;
        Color_RGBA8 extraColor;
        std::string stemp;

        bool shouldHideFilteredAreas = CVarGetInteger(CVAR_TRACKER_CHECK("HideFilteredAreas"), 1);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 3.0f));
        for (auto& [rcArea, checks] : checksByArea) {
            RandomizerCheckArea thisArea = currentArea;

            thisAreaFullyChecked = (areaChecksGotten[rcArea] == areaCheckTotals[rcArea]);
            // Last Area needs to be cleaned up
            if (lastArea != RCAREA_INVALID && doDraw) {
                UIWidgets::PaddedSeparator();
            }
            lastArea = rcArea;
            if (previousShowHidden != showHidden) {
                previousShowHidden = showHidden;
                doAreaScroll = true;
            }
            if ((shouldHideFilteredAreas && filterAreasHidden[rcArea]) ||
                (!showHidden &&
                 ((hideComplete && thisAreaFullyChecked) || (hideIncomplete && !thisAreaFullyChecked))) ||
                (availableChecksDisplay != AC_DISABLED && availableChecksOnlyShow != AC_SHOW_ALL_CHECKS &&
                 areaChecksAvailable[rcArea] == 0)) {
                doDraw = false;
            } else {
                // Get the colour for the area
                if (thisAreaFullyChecked) {
                    mainColor = Color_Area_Complete_Main;
                    extraColor = Color_Area_Complete_Extra;
                } else {
                    mainColor = Color_Area_Incomplete_Main;
                    extraColor = Color_Area_Incomplete_Extra;
                }

                // Draw the area
                collapseLogic = !thisAreaFullyChecked;
                if (doingCollapseOrExpand) {
                    if (optExpandAll) {
                        collapseLogic = true;
                    } else if (optCollapseAll) {
                        collapseLogic = false;
                    }
                }
                stemp = RandomizerCheckObjects::GetRCAreaName(rcArea) + "##TreeNode";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(mainColor.r / 255.0f, mainColor.g / 255.0f,
                                                            mainColor.b / 255.0f, mainColor.a / 255.0f));
                if (doingCollapseOrExpand) {
                    ImGui::SetNextItemOpen(collapseLogic, ImGuiCond_Always);
                } else {
                    ImGui::SetNextItemOpen(!thisAreaFullyChecked, ImGuiCond_Once);
                }
                doDraw = ImGui::TreeNodeEx(stemp.c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen);
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(extraColor.r / 255.0f, extraColor.g / 255.0f,
                                                            extraColor.b / 255.0f, extraColor.a / 255.0f));

                isThisAreaSpoiled = IsAreaSpoiled(rcArea) || mqSpoilers;

                if (isThisAreaSpoiled) {
                    std::ostringstream areaTotalsSS;
                    std::ostringstream areaTotalsTooltipSS;

                    areaTotalsSS << "(";
                    if (availableChecksDisplay != AC_DISABLED) {
                        areaTotalsSS << static_cast<uint16_t>(areaChecksAvailable[rcArea]) << " / ";
                        areaTotalsTooltipSS << "Available / ";
                    }
                    areaTotalsSS << static_cast<uint16_t>(areaChecksGotten[rcArea]) << " / "
                                 << static_cast<uint16_t>(areaCheckTotals[rcArea]) << ")";
                    areaTotalsTooltipSS << "Checked / Total";

                    if (showVOrMQ && RandomizerCheckObjects::AreaIsDungeon(rcArea)) {
                        if (OTRGlobals::Instance->gRandoContext->GetDungeons()
                                ->GetDungeonFromScene(DungeonSceneLookupByArea(rcArea))
                                ->IsMQ()) {
                            areaTotalsSS << " - MQ";
                        } else {
                            areaTotalsSS << " - Vanilla";
                        }
                    }

                    ImGui::Text("%s", areaTotalsSS.str().c_str());
                    UIWidgets::Tooltip(areaTotalsTooltipSS.str().c_str());
                } else {
                    ImGui::Text("???");
                }

                ImGui::PopStyleColor();

                // Keep areas loaded between transitions
                if (thisArea == rcArea && doAreaScroll) {
                    ImGui::SetScrollHereY(0.0f);
                    doAreaScroll = false;
                }
                for (auto rc : checks) {
                    if (doDraw && isThisAreaSpoiled && !filterChecksHidden[rc]) {
                        DrawLocation(rc);
                    }
                }
            }
        }
        ImGui::PopStyleVar();

        ImGui::EndTable(); // Checks Lead-out
        ImGui::EndTable(); // Quick Options Lead-out
        if (doingCollapseOrExpand) {
            optCollapseAll = false;
            optExpandAll = false;
        }
    }
    Trackers::EndFloatWindows();
}

bool UpdateFilters() {
    for (auto& [rcArea, checks] : checksByArea) {
        filterAreasHidden[rcArea] = !checkSearch.PassFilter(RandomizerCheckObjects::GetRCAreaName(rcArea).c_str());
        for (auto check : checks) {
            if (ShouldShowCheck(check)) {
                filterAreasHidden[rcArea] = false;
                filterChecksHidden[check] = false;
            } else {
                filterChecksHidden[check] = true;
            }
        }
    }

    return true;
}

bool ShouldShowCheck(RandomizerCheck check) {
    auto itemLoc = Rando::Context::GetInstance()->GetItemLocation(check);
    std::string search = (Rando::StaticData::GetLocation(check)->GetShortName() + " " +
                          Rando::StaticData::GetLocation(check)->GetName() + " " +
                          RandomizerCheckObjects::GetRCAreaName(Rando::StaticData::GetLocation(check)->GetArea()));
    if (itemLoc->HasObtained() || itemLoc->GetCheckStatus() == RCSHOW_SCUMMED ||
        (!mystery && (itemLoc->GetCheckStatus() == RCSHOW_IDENTIFIED || itemLoc->GetCheckStatus() == RCSHOW_SEEN) &&
         itemLoc->GetPlacedRandomizerGet() != RG_ICE_TRAP)) {
        search += " " + itemLoc->GetPlacedItemName().GetForLanguage(gSaveContext.language);
    } else if (itemLoc->GetCheckStatus() == RCSHOW_IDENTIFIED && !mystery) {
        search +=
            OTRGlobals::Instance->gRandoContext->overrides[check].GetTrickName().GetForLanguage(gSaveContext.language);
    } else if (itemLoc->GetCheckStatus() == RCSHOW_SEEN && !mystery) {
        search += Rando::StaticData::RetrieveItem(OTRGlobals::Instance->gRandoContext->overrides[check].LooksLike())
                      .GetName()
                      .GetForLanguage(gSaveContext.language);
    }
    return (IsVisibleInCheckTracker(check) &&
            (checkSearch.Filters.Size == 0 || checkSearch.PassFilter(search.c_str())));
}

void LoadSettings() {
    // If in randomzer, then get the setting and check if in general we should be showing the settings
    // If in vanilla, _try_ to show items that at least are needed for 100%

    showShops =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHOPSANITY) != RO_SHOPSANITY_OFF : false;
    showBeans = IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MERCHANTS) ==
                                   RO_SHUFFLE_MERCHANTS_BEANS_ONLY ||
                               OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MERCHANTS) ==
                                   RO_SHUFFLE_MERCHANTS_ALL
                         : true;
    showScrubs =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) == RO_SCRUBS_ALL : false;
    showMajorScrubs =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) != RO_SCRUBS_OFF : false;
    showMerchants = IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MERCHANTS) ==
                                       RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS ||
                                   OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MERCHANTS) ==
                                       RO_SHUFFLE_MERCHANTS_ALL
                             : true;
    showSongs = IS_RANDO
                    ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_SONGS) != RO_SONG_SHUFFLE_OFF
                    : false;
    showBeehives = IS_RANDO
                       ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_BEEHIVES) == RO_GENERIC_YES
                       : false;
    showCows =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_COWS) == RO_GENERIC_YES : false;
    showAdultTrade =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE) == RO_GENERIC_YES
                 : true;
    showKokiriSword =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_KOKIRI_SWORD) == RO_GENERIC_YES
                 : true;
    showMasterSword =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MASTER_SWORD) == RO_GENERIC_YES
                 : true;
    showHyruleLoach =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY) == RO_FISHSANITY_HYRULE_LOACH
                 : false;
    showWeirdEgg =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_WEIRD_EGG) == RO_GENERIC_YES
                 : true;
    showGerudoCard = IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(
                                    RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD) == RO_GENERIC_YES
                              : true;
    showFrogSongRupees =
        IS_RANDO
            ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_FROG_SONG_RUPEES) == RO_GENERIC_YES
            : false;
    showFountainFairies =
        IS_RANDO
            ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_FOUNTAIN_FAIRIES) == RO_GENERIC_YES
            : false;
    showStoneFairies =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_STONE_FAIRIES) == RO_GENERIC_YES
                 : false;
    showBeanFairies =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_BEAN_FAIRIES) == RO_GENERIC_YES
                 : false;
    showSongFairies =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_SONG_FAIRIES) == RO_GENERIC_YES
                 : false;
    showButterflyFairies =
        IS_RANDO
            ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_BUTTERFLY_FAIRIES) == RO_GENERIC_YES
            : false;
    showStartingMapsCompasses = IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(
                                               RSK_SHUFFLE_MAPANDCOMPASS) != RO_DUNGEON_ITEM_LOC_VANILLA
                                         : false;
    showKeysanity =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) != RO_DUNGEON_ITEM_LOC_VANILLA
                 : false;
    showBossKeysanity = IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) !=
                                       RO_DUNGEON_ITEM_LOC_VANILLA
                                 : false;
    showGerudoFortressKeys =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_KEYS) != RO_GERUDO_KEYS_VANILLA
                 : false;
    showGanonBossKey = IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) !=
                                      RO_GANON_BOSS_KEY_VANILLA
                                : false;
    showOcarinas = IS_RANDO
                       ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_OCARINA) == RO_GENERIC_YES
                       : false;
    show100SkullReward =
        IS_RANDO ? OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_100_GS_REWARD) == RO_GENERIC_YES
                 : false;
    showLinksPocket =
        IS_RANDO ? // don't show Link's Pocket if not randomizer, or if rando and pocket is disabled
            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LINKS_POCKET) != RO_LINKS_POCKET_NOTHING
                 : false;

    if (IS_RANDO) {
        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_TOKENS)) {
            case RO_TOKENSANITY_ALL:
                showOverworldTokens = true;
                showDungeonTokens = true;
                break;
            case RO_TOKENSANITY_OVERWORLD:
                showOverworldTokens = true;
                showDungeonTokens = false;
                break;
            case RO_TOKENSANITY_DUNGEONS:
                showOverworldTokens = false;
                showDungeonTokens = true;
                break;
            default:
                showOverworldTokens = false;
                showDungeonTokens = false;
                break;
        }

        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_POTS)) {
            case RO_SHUFFLE_POTS_ALL:
                showOverworldPots = true;
                showDungeonPots = true;
                break;
            case RO_SHUFFLE_POTS_OVERWORLD:
                showOverworldPots = true;
                showDungeonPots = false;
                break;
            case RO_SHUFFLE_POTS_DUNGEONS:
                showOverworldPots = false;
                showDungeonPots = true;
                break;
            default:
                showOverworldPots = false;
                showDungeonPots = false;
                break;
        }

        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_GRASS)) {
            case RO_SHUFFLE_GRASS_ALL:
                showOverworldGrass = true;
                showDungeonGrass = true;
                break;
            case RO_SHUFFLE_GRASS_OVERWORLD:
                showOverworldGrass = true;
                showDungeonGrass = false;
                break;
            case RO_SHUFFLE_GRASS_DUNGEONS:
                showOverworldGrass = false;
                showDungeonGrass = true;
                break;
            default:
                showOverworldGrass = false;
                showDungeonGrass = false;
                break;
        }

        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_CRATES)) {
            case RO_SHUFFLE_CRATES_ALL:
                showOverworldCrates = true;
                showDungeonCrates = true;
                break;
            case RO_SHUFFLE_CRATES_OVERWORLD:
                showOverworldCrates = true;
                showDungeonCrates = false;
                break;
            case RO_SHUFFLE_CRATES_DUNGEONS:
                showOverworldCrates = false;
                showDungeonCrates = true;
                break;
            default:
                showOverworldCrates = false;
                showDungeonCrates = false;
                break;
        }

        showRocks = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ROCKS);
        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_BOULDERS)) {
            case RO_SHUFFLE_BOULDERS_ALL:
                showOverworldBoulders = true;
                showDungeonBoulders = true;
                break;
            case RO_SHUFFLE_BOULDERS_OVERWORLD:
                showOverworldBoulders = true;
                showDungeonBoulders = false;
                break;
            case RO_SHUFFLE_BOULDERS_DUNGEONS:
                showOverworldBoulders = false;
                showDungeonBoulders = true;
                break;
            default:
                showOverworldBoulders = false;
                showDungeonBoulders = false;
                break;
        }

        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_SIGNS)) {
            case RO_SHUFFLE_SIGNS_ALL:
                showOverworldSigns = true;
                showDungeonSigns = true;
                break;
            case RO_SHUFFLE_SIGNS_OVERWORLD:
                showOverworldSigns = true;
                showDungeonSigns = false;
                break;
            case RO_SHUFFLE_SIGNS_DUNGEONS:
                showOverworldSigns = false;
                showDungeonSigns = true;
                break;
            default:
                showOverworldSigns = false;
                showDungeonSigns = false;
                break;
        }

        showTrees = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_TREES);
        showBushes = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_BUSHES);

        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_WONDER_ITEMS)) {
            case RO_SHUFFLE_WONDER_ITEMS_ALL:
                showOverworldWonderItems = true;
                showDungeonWonderItems = true;
                break;
            case RO_SHUFFLE_WONDER_ITEMS_OVERWORLD:
                showOverworldWonderItems = true;
                showDungeonWonderItems = false;
                break;
            case RO_SHUFFLE_WONDER_ITEMS_DUNGEONS:
                showOverworldWonderItems = false;
                showDungeonWonderItems = true;
                break;
            default:
                showOverworldWonderItems = false;
                showDungeonWonderItems = false;
                break;
        }
        showBeggar = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_BEGGAR);
        showIcicles = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ICICLES);
        showRedIce = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_RED_ICE);
    } else { // Vanilla
        showOverworldTokens = true;
        showDungeonTokens = true;
        showOverworldPots = false;
        showDungeonPots = false;
        showOverworldGrass = false;
        showDungeonGrass = false;
        showOverworldCrates = false;
        showDungeonCrates = false;
        showRocks = false;
        showOverworldBoulders = false;
        showDungeonBoulders = false;
        showTrees = false;
        showBushes = false;
        showOverworldWonderItems = false;
        showDungeonWonderItems = false;
        showOverworldSigns = false;
        showDungeonSigns = false;
        showBeggar = false;
        showIcicles = false;
        showRedIce = false;
    }

    fortressFast = false;
    fortressNormal = false;
    switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_FORTRESS)) {
        case RO_GF_CARPENTERS_FREE:
            showGerudoFortressKeys = false;
            showGerudoCard = false;
            break;
        case RO_GF_CARPENTERS_FAST:
            fortressFast = true;
            break;
        case RO_GF_CARPENTERS_NORMAL:
            fortressNormal = true;
            break;
    }

    fishsanityMode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
    fishsanityPondCount = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_POND_COUNT);
    fishsanityAgeSplit = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_AGE_SPLIT);

    if (IS_RANDO) {
        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_FREESTANDING)) {
            case RO_SHUFFLE_FREESTANDING_ALL:
                showOverworldFreestanding = true;
                showDungeonFreestanding = true;
                break;
            case RO_SHUFFLE_FREESTANDING_OVERWORLD:
                showOverworldFreestanding = true;
                showDungeonFreestanding = false;
                break;
            case RO_SHUFFLE_FREESTANDING_DUNGEONS:
                showOverworldFreestanding = false;
                showDungeonFreestanding = true;
                break;
            default:
                showOverworldFreestanding = false;
                showDungeonFreestanding = false;
                break;
        }
    } else { // Vanilla
        showOverworldFreestanding = false;
        showDungeonFreestanding = true;
    }

    switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY)) {
        case RO_GANON_BOSS_KEY_LACS_STONES:
            Rando::Context::GetInstance()->LACSCondition(RO_LACS_STONES);
            break;
        case RO_GANON_BOSS_KEY_LACS_MEDALLIONS:
            Rando::Context::GetInstance()->LACSCondition(RO_LACS_MEDALLIONS);
            break;
        case RO_GANON_BOSS_KEY_LACS_REWARDS:
            Rando::Context::GetInstance()->LACSCondition(RO_LACS_REWARDS);
            break;
        case RO_GANON_BOSS_KEY_LACS_DUNGEONS:
            Rando::Context::GetInstance()->LACSCondition(RO_LACS_DUNGEONS);
            break;
        case RO_GANON_BOSS_KEY_LACS_TOKENS:
            Rando::Context::GetInstance()->LACSCondition(RO_LACS_TOKENS);
            break;
        default:
            Rando::Context::GetInstance()->LACSCondition(RO_LACS_VANILLA);
            break;
    }
}

bool IsCheckShuffled(RandomizerCheck rc) {
    Rando::Location* loc = Rando::StaticData::GetLocation(rc);
    if (loc->GetRCType() == RCTYPE_SHOP) {
        auto identity = OTRGlobals::Instance->gRandomizer->IdentifyShopItem(loc->GetScene(), loc->GetActorParams() + 1);
    }
    if (IS_RANDO) {
        return (loc->GetArea() != RCAREA_INVALID) &&        // don't show Invalid locations
               (loc->GetRCType() != RCTYPE_GOSSIP_STONE) && // TODO: Don't show hints until tracker supports them
               (loc->GetRCType() != RCTYPE_STATIC_HINT) &&  // TODO: Don't show hints until tracker supports them
               (loc->GetRCType() != RCTYPE_CHEST_GAME) &&   // don't show non final reward chest game checks until we
                                                            // support shuffling them
               (rc != RC_HC_ZELDAS_LETTER) &&               // don't show zeldas letter until we support shuffling it
               (rc != RC_LINKS_POCKET || showLinksPocket) &&
               OTRGlobals::Instance->gRandoContext->IsQuestOfLocationActive(rc) &&
               (loc->GetRCType() != RCTYPE_SHOP ||
                (showShops &&
                 OTRGlobals::Instance->gRandomizer->IdentifyShopItem(loc->GetScene(), loc->GetActorParams() + 1)
                         .enGirlAShopItem == 50)) &&
               (rc != RC_TRIFORCE_COMPLETED) && (rc != RC_GANON) &&
               (loc->GetRCType() != RCTYPE_SCRUB || showScrubs ||
                (showMajorScrubs && (rc == RC_LW_DEKU_SCRUB_NEAR_BRIDGE || // The 3 scrubs that are always randomized
                                     rc == RC_HF_DEKU_SCRUB_GROTTO || rc == RC_LW_DEKU_SCRUB_GROTTO_FRONT))) &&
               (loc->GetRCType() != RCTYPE_MERCHANT || showMerchants) &&
               (loc->GetRCType() != RCTYPE_BEGGAR || showBeggar) &&
               (loc->GetRCType() != RCTYPE_SONG_LOCATION || showSongs) &&
               (loc->GetRCType() != RCTYPE_BEEHIVE || showBeehives) &&
               (loc->GetRCType() != RCTYPE_OCARINA || showOcarinas) &&
               (loc->GetRCType() != RCTYPE_SKULL_TOKEN || alwaysShowGS ||
                (showOverworldTokens && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonTokens && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_POT ||
                (showOverworldPots && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonPots && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_GRASS ||
                (showOverworldGrass && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonGrass && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_CRATE ||
                (showOverworldCrates && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonCrates && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_NLCRATE ||
                (showOverworldCrates && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea()) &&
                 OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LOGIC_RULES) == RO_LOGIC_NO_LOGIC) ||
                (showDungeonCrates && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_SMALL_CRATE ||
                (showOverworldCrates && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonCrates && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_ROCK || showRocks) &&
               (loc->GetRCType() != RCTYPE_BOULDER ||
                (showOverworldBoulders && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonBoulders && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_TREE || showTrees) &&
               (loc->GetRCType() != RCTYPE_NLTREE ||
                (showTrees &&
                 OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LOGIC_RULES) == RO_LOGIC_NO_LOGIC)) &&
               (loc->GetRCType() != RCTYPE_BUSH || showBushes) && (loc->GetRCType() != RCTYPE_COW || showCows) &&
               (loc->GetRCType() != RCTYPE_SIGN ||
                (showOverworldSigns && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonSigns && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_WONDER_ITEM ||
                (showOverworldWonderItems && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonWonderItems && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_ICICLE || showIcicles) &&
               (loc->GetRCType() != RCTYPE_RED_ICE || showRedIce) &&
               (loc->GetRCType() != RCTYPE_FISH ||
                OTRGlobals::Instance->gRandoContext->GetFishsanity()->GetFishLocationIncluded(loc)) &&
               (loc->GetRCType() != RCTYPE_FREESTANDING ||
                (showOverworldFreestanding && RandomizerCheckObjects::AreaIsOverworld(loc->GetArea())) ||
                (showDungeonFreestanding && RandomizerCheckObjects::AreaIsDungeon(loc->GetArea()))) &&
               (loc->GetRCType() != RCTYPE_ADULT_TRADE || showAdultTrade ||
                rc == RC_KAK_ANJU_AS_ADULT ||  // adult trade checks that are always shuffled
                rc == RC_DMT_TRADE_CLAIM_CHECK // even when shuffle adult trade is off
                ) &&
               (rc != RC_KF_KOKIRI_SWORD_CHEST || showKokiriSword) && (rc != RC_TOT_MASTER_SWORD || showMasterSword) &&
               (rc != RC_LH_HYRULE_LOACH || showHyruleLoach) && (rc != RC_ZR_MAGIC_BEAN_SALESMAN || showBeans) &&
               (rc != RC_HC_MALON_EGG || showWeirdEgg) &&
               (loc->GetRCType() != RCTYPE_FROG_SONG || showFrogSongRupees) &&
               ((loc->GetRCType() != RCTYPE_MAP && loc->GetRCType() != RCTYPE_COMPASS) || showStartingMapsCompasses) &&
               (loc->GetRCType() != RCTYPE_FOUNTAIN_FAIRY || showFountainFairies) &&
               (loc->GetRCType() != RCTYPE_STONE_FAIRY || showStoneFairies) &&
               (loc->GetRCType() != RCTYPE_BEAN_FAIRY || showBeanFairies) &&
               (loc->GetRCType() != RCTYPE_SONG_FAIRY || showSongFairies) &&
               (loc->GetRCType() != RCTYPE_BUTTERFLY_FAIRY || showButterflyFairies) &&
               (loc->GetRCType() != RCTYPE_SMALL_KEY || showKeysanity) &&
               (loc->GetRCType() != RCTYPE_BOSS_KEY || showBossKeysanity) &&
               (loc->GetRCType() != RCTYPE_GANON_BOSS_KEY || showGanonBossKey) &&
               (rc != RC_KAK_100_GOLD_SKULLTULA_REWARD || show100SkullReward) &&
               (loc->GetRCType() != RCTYPE_GF_KEY && rc != RC_TH_FREED_CARPENTERS ||
                (showGerudoCard && rc == RC_TH_FREED_CARPENTERS) ||
                (fortressNormal && showGerudoFortressKeys && loc->GetRCType() == RCTYPE_GF_KEY) ||
                (fortressFast && showGerudoFortressKeys && rc == RC_TH_1_TORCH_CARPENTER));
    } else if (loc->IsVanillaCompletion()) {
        return (OTRGlobals::Instance->gRandoContext->IsQuestOfLocationActive(rc) || rc == RC_GIFT_FROM_RAURU) &&
               rc != RC_LINKS_POCKET;
    }
    return false;
}

bool IsVisibleInCheckTracker(RandomizerCheck rc) {
    auto loc = Rando::StaticData::GetLocation(rc);
    if (IS_RANDO) {
        return !Rando::Context::GetInstance()->GetItemLocation(rc)->IsExcluded() &&
               (IsCheckShuffled(rc) ||
                (alwaysShowGS && loc->GetRCType() == RCTYPE_SKULL_TOKEN &&
                 OTRGlobals::Instance->gRandoContext->IsQuestOfLocationActive(rc)) ||
                (loc->GetRCType() == RCTYPE_SHOP && showShops && !hideShopUnshuffledChecks));
    } else {
        return loc->IsVanillaCompletion() &&
               (!loc->IsDungeon() || (loc->IsDungeon() && loc->GetQuest() == gSaveContext.ship.quest.id));
    }
}

void UpdateInventoryChecks() {
    // For all the areas with maps, if you have one, spoil the area
    for (auto [scene, area] : DungeonRCAreasBySceneID) {
        if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, scene)) {
            SetAreaSpoiled(area);
        }
    }
}

void UpdateAreaFullyChecked(RandomizerCheckArea area) {
}

void UpdateAllAreas() {
    // Sort the entire thing
    for (int i = 0; i < RCAREA_INVALID; i++) {
        UpdateAreas(static_cast<RandomizerCheckArea>(i));
    }
}

void UpdateAreas(RandomizerCheckArea area) {
    if (checksByArea.contains(area)) {
        areasFullyChecked[area] = areaChecksGotten[area] == checksByArea.find(area)->second.size();
    }
}

void UpdateAllOrdering() {
    // Sort the entire thing
    for (int i = 0; i < RCAREA_INVALID; i++) {
        UpdateOrdering(static_cast<RandomizerCheckArea>(i));
    }
}

void UpdateOrdering(RandomizerCheckArea rcArea) {
    // Sort a single area
    if (checksByArea.contains(rcArea)) {
        std::sort(checksByArea.find(rcArea)->second.begin(), checksByArea.find(rcArea)->second.end(), CompareChecks);
    }
    RecalculateAllAreaTotals();
    CalculateTotals();
}

bool IsEoDCheck(RandomizerCheckType type) {
    return type == RCTYPE_BOSS_HEART_OR_OTHER_REWARD || type == RCTYPE_DUNGEON_REWARD;
}

bool CompareChecks(RandomizerCheck i, RandomizerCheck j) {
    Rando::Location* x = Rando::StaticData::GetLocation(i);
    Rando::Location* y = Rando::StaticData::GetLocation(j);
    auto itemI = OTRGlobals::Instance->gRandoContext->GetItemLocation(i);
    auto itemJ = OTRGlobals::Instance->gRandoContext->GetItemLocation(j);
    bool iCollected = itemI->HasObtained();
    bool iSaved = itemI->GetCheckStatus() == RCSHOW_SAVED;
    bool jCollected = itemJ->HasObtained();
    bool jSaved = itemJ->GetCheckStatus() == RCSHOW_SAVED;

    if (!iCollected && jCollected) {
        return true;
    } else if (iCollected && !jCollected) {
        return false;
    }

    if (!iSaved && jSaved) {
        return true;
    } else if (iSaved && !jSaved) {
        return false;
    }

    if (!itemI->GetIsSkipped() && itemJ->GetIsSkipped()) {
        return true;
    } else if (itemI->GetIsSkipped() && !itemJ->GetIsSkipped()) {
        return false;
    }

    if (!IsEoDCheck(x->GetRCType()) && IsEoDCheck(y->GetRCType())) {
        return true;
    } else if (IsEoDCheck(x->GetRCType()) && !IsEoDCheck(y->GetRCType())) {
        return false;
    }

    if (i < j) {
        return true;
    } else if (i > j) {
        return false;
    }

    return false;
}

bool IsHeartPiece(GetItemID giid) {
    return giid == GI_HEART_PIECE || giid == GI_HEART_PIECE_WIN;
}

void DrawAvailability(ImVec4 styleColor, AvailableChecksDisplay display, bool child, bool adult, bool obtained) {
    if (display == AC_DISABLED) {
        return;
    }

    auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();
    auto textSize = ImGui::CalcTextSize(ICON_FA_LOCK);
    auto framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 iconSize(textSize.y + framePadding.y * 2, textSize.y + framePadding.y * 2), zero(0.0f, 0.0f), one(1, 1);

    if (display == AC_LOCKED_UNLOCKED) {
        if (obtained) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, styleColor);
        }

        if (!obtained) {
            auto texture = gui->GetTextureByName(availableChecksNoneAvailableIcon);
            if (texture != nullptr) {
                ImGui::ImageWithBg(texture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 0));
            } else {
                ImGui::Text(availableChecksNoneAvailableIcon.c_str());
            }
        } else if (child || adult) {
            auto texture = gui->GetTextureByName(availableChecksBothAvailableIcon);
            if (texture != nullptr) {
                ImGui::ImageWithBg(texture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
            } else {
                ImGui::Text(availableChecksBothAvailableIcon.c_str());
            }
            UIWidgets::Tooltip("Available");
        } else {
            auto texture = gui->GetTextureByName(availableChecksNoneAvailableIcon);
            if (texture != nullptr) {
                ImGui::ImageWithBg(texture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
            } else {
                ImGui::Text(availableChecksNoneAvailableIcon.c_str());
            }
            UIWidgets::Tooltip("Unavailable");
        }
        ImGui::SameLine();
        ImGui::PopStyleColor();
    } else if (display == AC_ONE_ICON) {
        if (obtained) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, styleColor);
        }

        if (child && adult) {
            auto texture = gui->GetTextureByName(availableChecksBothAvailableIcon);
            if (texture != nullptr) {
                ImGui::Image(texture, iconSize, zero, one);
            } else {
                ImGui::Text(availableChecksBothAvailableIcon.c_str());
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Child & Adult Available");
        } else if (child) {
            auto texture = gui->GetTextureByName(availableChecksChildAvailableIcon);
            if (texture != nullptr) {
                ImGui::Image(texture, iconSize, zero, one);
            } else {
                ImGui::Text(availableChecksChildAvailableIcon.c_str());
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Child Available");
        } else if (adult) {
            auto texture = gui->GetTextureByName(availableChecksAdultAvailableIcon);
            if (texture != nullptr) {
                ImGui::Image(texture, iconSize, zero, one);
            } else {
                ImGui::Text(availableChecksAdultAvailableIcon.c_str());
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Adult Available");
        } else {
            auto texture = gui->GetTextureByName(availableChecksNoneAvailableIcon);
            if (texture != nullptr) {
                ImGui::Image(texture, iconSize, zero, one);
            } else {
                ImGui::Text(availableChecksNoneAvailableIcon.c_str());
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Neither Available");
        }

        ImGui::PopStyleColor();
    } else if (display == AC_TWO_ICONS) {
        auto childTexture = gui->GetTextureByName(availableChecksChildAvailableIcon);
        if (obtained) {
            if (childTexture != nullptr) {
                ImGui::ImageWithBg(childTexture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 0));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
                ImGui::Text(availableChecksChildAvailableIcon.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
        } else if (child) {
            if (childTexture != nullptr) {
                ImGui::ImageWithBg(childTexture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, styleColor);
                ImGui::Text(availableChecksChildAvailableIcon.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Child Available");
        } else {
            if (childTexture != nullptr) {
                ImGui::ImageWithBg(childTexture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 0.25));
            } else {
                auto dimmed = styleColor;
                dimmed.w *= 0.25;
                ImGui::PushStyleColor(ImGuiCol_Text, dimmed);
                ImGui::Text(availableChecksChildAvailableIcon.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Child Unavailable");
        }

        auto adultTexture = gui->GetTextureByName(availableChecksAdultAvailableIcon);
        if (obtained) {
            if (adultTexture != nullptr) {
                ImGui::ImageWithBg(adultTexture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 0));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
                ImGui::Text(availableChecksAdultAvailableIcon.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
        } else if (adult) {
            if (adultTexture != nullptr) {
                ImGui::ImageWithBg(adultTexture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, styleColor);
                ImGui::Text(availableChecksAdultAvailableIcon.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Adult Available");
        } else {
            if (adultTexture != nullptr) {
                ImGui::ImageWithBg(adultTexture, iconSize, zero, one, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 0.25));
            } else {
                auto dimmed = styleColor;
                dimmed.w *= 0.25;
                ImGui::PushStyleColor(ImGuiCol_Text, dimmed);
                ImGui::Text(availableChecksAdultAvailableIcon.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
            UIWidgets::Tooltip("Adult Unavailable");
        }
    }
}

void DrawLocation(RandomizerCheck rc) {
    Color_RGBA8 mainColor;
    Color_RGBA8 extraColor;
    std::string txt;
    Rando::Location* loc = Rando::StaticData::GetLocation(rc);
    Rando::ItemLocation* itemLoc = OTRGlobals::Instance->gRandoContext->GetItemLocation(rc);
    RandomizerCheckStatus status = itemLoc->GetCheckStatus();
    bool skipped = itemLoc->GetIsSkipped();
    bool childAvailable = itemLoc->IsChildAvailable(false);
    bool adultAvailable = itemLoc->IsAdultAvailable(false);

    if (availableChecksDisplay != AC_DISABLED) {
        if (availableChecksOnlyShow == AC_SHOW_AVAILABLE_CHECKS && !childAvailable && !adultAvailable) {
            return;
        }
        if (availableChecksOnlyShow == AC_SHOW_CHILD_CHECKS && !childAvailable) {
            return;
        }
        if (availableChecksOnlyShow == AC_SHOW_ADULT_CHECKS && !adultAvailable) {
            return;
        }
        if (availableChecksOnlyShow == AC_SHOW_CURRENT_AGE) {
            if (LINK_IS_CHILD && !childAvailable) {
                return;
            }
            if (LINK_IS_ADULT && !adultAvailable) {
                return;
            }
        }
    }

    if (status == RCSHOW_COLLECTED) {
        if (!showHidden && hideCollected) {
            return;
        }
        mainColor =
            !IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID()) && !IS_RANDO
                ? Color_Collected_Extra
                : Color_Collected_Main;
        extraColor = Color_Collected_Extra;
    } else if (status == RCSHOW_SAVED) {
        if (!showHidden && hideSaved) {
            return;
        }
        mainColor =
            !IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID()) && !IS_RANDO
                ? Color_Saved_Extra
                : Color_Saved_Main;
        extraColor = Color_Saved_Extra;
    } else if (skipped) {
        if (!showHidden && hideSkipped) {
            return;
        }
        mainColor =
            !IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID()) && !IS_RANDO
                ? Color_Skipped_Extra
                : Color_Skipped_Main;
        extraColor = Color_Skipped_Extra;
    } else if (status == RCSHOW_SEEN || status == RCSHOW_IDENTIFIED) {
        if (!showHidden && hideSeen) {
            return;
        }
        mainColor =
            !IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID()) && !IS_RANDO
                ? Color_Seen_Extra
                : Color_Seen_Main;
        extraColor = Color_Seen_Extra;
    } else if (status == RCSHOW_SCUMMED) {
        if (!showHidden && hideScummed) {
            return;
        }
        mainColor =
            !IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID()) && !IS_RANDO
                ? Color_Scummed_Extra
                : Color_Scummed_Main;
        extraColor = Color_Scummed_Extra;
    } else if (status == RCSHOW_UNCHECKED) {
        if (!showHidden && hideUnchecked) {
            return;
        }
        mainColor =
            !IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID()) && !IS_RANDO
                ? Color_Unchecked_Extra
                : Color_Unchecked_Main;
        extraColor = Color_Unchecked_Extra;
    }

    // Main Text
    if (checkNameOverrides.contains(loc->GetRandomizerCheck())) {
        txt = checkNameOverrides[loc->GetRandomizerCheck()];
    } else {
        txt = loc->GetShortName();
    }

    if (lastLocationChecked == loc->GetRandomizerCheck()) {
        txt = "* " + txt;
    }

    // Draw button - for Skipped/Seen/Scummed/Unchecked only
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 3.0f });
    float sz = ImGui::GetFrameHeight();
    if (status == RCSHOW_UNCHECKED || status == RCSHOW_SEEN || status == RCSHOW_IDENTIFIED ||
        status == RCSHOW_SCUMMED || skipped) {
        if (UIWidgets::StateButton(std::to_string(rc).c_str(), skipped ? ICON_FA_PLUS : ICON_FA_TIMES, ImVec2(sz, sz),
                                   UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
            if (skipped) {
                OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetIsSkipped(false);
                areaChecksGotten[loc->GetArea()]--;
                totalChecksGotten--;
                if (childAvailable || adultAvailable) {
                    areaChecksAvailable[loc->GetArea()]++;
                    totalChecksAvailable++;
                }
            } else {
                OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetIsSkipped(true);
                areaChecksGotten[loc->GetArea()]++;
                totalChecksGotten++;
                if (childAvailable || adultAvailable) {
                    areaChecksAvailable[loc->GetArea()]--;
                    totalChecksAvailable--;
                }
            }
            UpdateOrdering(loc->GetArea());
            UpdateInventoryChecks();
            SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);
        }
    } else {
        ImGui::Dummy(ImVec2(sz, sz));
    }
    ImGui::PopStyleVar();

    ImGui::SameLine();

    // Draw
    ImVec4 styleColor(mainColor.r / 255.0f, mainColor.g / 255.0f, mainColor.b / 255.0f, mainColor.a / 255.0f);

    if (ageChecksDisplay != AC_DISABLED) {
        DrawAvailability(styleColor, ageChecksDisplay, itemLoc->IsChildAvailable(true), itemLoc->IsAdultAvailable(true),
                         itemLoc->HasObtained());
    }
    DrawAvailability(styleColor, availableChecksDisplay, itemLoc->IsChildAvailable(false),
                     itemLoc->IsAdultAvailable(false), itemLoc->HasObtained());

    ImGui::PushStyleColor(ImGuiCol_Text, styleColor);
    ImGui::Text("%s", txt.c_str());
    ImGui::PopStyleColor();

    // Draw the extra info
    txt = "";

    if (status != RCSHOW_UNCHECKED) {
        switch (status) {
            case RCSHOW_SAVED:
            case RCSHOW_COLLECTED:
            case RCSHOW_SCUMMED:
                if (IS_RANDO) {
                    txt = itemLoc->GetPlacedItem().GetName().GetForLanguage(gSaveContext.language);
                } else {
                    if (IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID())) {
                        if (gSaveContext.language == LANGUAGE_ENG || gSaveContext.language == LANGUAGE_GER ||
                            gSaveContext.language == LANGUAGE_JPN) {
                            txt = Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetName().english;
                        } else if (gSaveContext.language == LANGUAGE_FRA) {
                            txt = Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetName().french;
                        }
                    }
                }
                break;
            case RCSHOW_IDENTIFIED:
            case RCSHOW_SEEN:
                if (IS_RANDO) {
                    if (itemLoc->GetPlacedRandomizerGet() == RG_ICE_TRAP && !mystery) {
                        if (status == RCSHOW_IDENTIFIED) {
                            txt = OTRGlobals::Instance->gRandoContext->overrides[rc].GetTrickName().GetForLanguage(
                                gSaveContext.language);
                        } else {
                            txt = Rando::StaticData::RetrieveItem(
                                      OTRGlobals::Instance->gRandoContext->overrides[rc].LooksLike())
                                      .GetName()
                                      .GetForLanguage(gSaveContext.language);
                        }
                    } else if (!mystery) {
                        txt = itemLoc->GetPlacedItem().GetName().GetForLanguage(gSaveContext.language);
                    }
                    if (IsVisibleInCheckTracker(rc) && status == RCSHOW_IDENTIFIED && !mystery) {
                        auto price = OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->GetPrice();
                        if (price) {
                            txt += fmt::format(" - {}", price);
                        }
                    }
                } else {
                    if (IsHeartPiece((GetItemID)Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetItemID())) {
                        if (gSaveContext.language == LANGUAGE_ENG || gSaveContext.language == LANGUAGE_GER ||
                            gSaveContext.language == LANGUAGE_JPN) {
                            txt = Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetName().english;
                        } else if (gSaveContext.language == LANGUAGE_FRA) {
                            txt = Rando::StaticData::RetrieveItem(loc->GetVanillaItem()).GetName().french;
                        }
                    }
                }
                break;
        }
    }
    if (txt == "" && skipped) {
        txt = "Skipped"; // TODO language
    }

    if (txt != "") {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(extraColor.r / 255.0f, extraColor.g / 255.0f, extraColor.b / 255.0f,
                                                    extraColor.a / 255.0f));
        ImGui::SameLine();
        ImGui::Text(" (%s)", txt.c_str());
        ImGui::PopStyleColor();
    }

    if (showLogicTooltip) {
        for (auto& locationInRegion : areaTable[itemLoc->GetParentRegionKey()].locations) {
            if (locationInRegion.GetLocation() == rc) {
                std::string conditionStr = locationInRegion.GetConditionStr();
                if (conditionStr != "true") {
                    UIWidgets::Tooltip(conditionStr.c_str());
                }
                break;
            }
        }
    }
}

static std::set<std::string> rainbowCVars = {
    CVAR_TRACKER_CHECK("AreaIncomplete.MainColor"), CVAR_TRACKER_CHECK("AreaIncomplete.ExtraColor"),
    CVAR_TRACKER_CHECK("AreaComplete.MainColor"),   CVAR_TRACKER_CHECK("AreaComplete.ExtraColor"),
    CVAR_TRACKER_CHECK("Unchecked.MainColor"),      CVAR_TRACKER_CHECK("Unchecked.ExtraColor"),
    CVAR_TRACKER_CHECK("Skipped.MainColor"),        CVAR_TRACKER_CHECK("Skipped.ExtraColor"),
    CVAR_TRACKER_CHECK("Seen.MainColor"),           CVAR_TRACKER_CHECK("Seen.ExtraColor"),
    CVAR_TRACKER_CHECK("Hinted.MainColor"),         CVAR_TRACKER_CHECK("Hinted.ExtraColor"),
    CVAR_TRACKER_CHECK("Collected.MainColor"),      CVAR_TRACKER_CHECK("Collected.ExtraColor"),
    CVAR_TRACKER_CHECK("Scummed.MainColor"),        CVAR_TRACKER_CHECK("Scummed.ExtraColor"),
    CVAR_TRACKER_CHECK("Saved.MainColor"),          CVAR_TRACKER_CHECK("Saved.ExtraColor"),
};

int hue = 0;
void RainbowTick() {
    float freqHue = hue * 2 * M_PIf / (360 * CVarGetFloat(CVAR_COSMETIC("RainbowSpeed"), 0.6f));
    for (auto& cvar : rainbowCVars) {
        if (CVarGetInteger((cvar + ".Rainbow").c_str(), 0) == 0) {
            continue;
        }

        Color_RGBA8 newColor;
        newColor.r = static_cast<uint8_t>(sin(freqHue + 0) * 127) + 128;
        newColor.g = static_cast<uint8_t>(sin(freqHue + (2 * M_PI / 3)) * 127) + 128;
        newColor.b = static_cast<uint8_t>(sin(freqHue + (4 * M_PI / 3)) * 127) + 128;
        newColor.a = 255;

        CVarSetColor((cvar + ".Value").c_str(), newColor);
    }

    hue++;
    hue %= 360;
}

void ImGuiDrawTwoColorPickerSection(const char* text, const char* cvarMainName, const char* cvarExtraName,
                                    Color_RGBA8& main_color, Color_RGBA8& extra_color,
                                    const Color_RGBA8& main_default_color, const Color_RGBA8& extra_default_color,
                                    const char* cvarHideName, const char* tooltip, UIWidgets::Colors theme) {
    Color_RGBA8 cvarMainColor = CVarGetColor(cvarMainName, main_default_color);
    Color_RGBA8 cvarExtraColor = CVarGetColor(cvarExtraName, extra_default_color);
    main_color = cvarMainColor;
    extra_color = cvarExtraColor;

    UIWidgets::PushStyleCombobox(theme);
    if (ImGui::CollapsingHeader(text)) {
        if (*cvarHideName != '\0') {
            std::string label = cvarHideName;
            label += "##Hidden";
            ImGui::PushID(label.c_str());
            UIWidgets::CVarCheckbox(
                "Hidden", cvarHideName,
                UIWidgets::CheckboxOptions(
                    { { .tooltip = "When active, checks will hide by default when updated to this state. Can "
                                   "be overridden with the \"Show Hidden Items\" option." } })
                    .Color(theme));
            ImGui::PopID();
        }
        std::string mainLabel = "Name##" + std::string(cvarMainName);
        if (UIWidgets::CVarColorPicker(mainLabel.c_str(), cvarMainName, main_default_color, false,
                                       UIWidgets::ColorPickerRandomButton | UIWidgets::ColorPickerResetButton |
                                           UIWidgets::ColorPickerRainbowCheck,
                                       theme)) {
            main_color = CVarGetColor(cvarMainName, main_default_color);
        }

        std::string extraLabel = "Details##" + std::string(cvarExtraName);
        if (UIWidgets::CVarColorPicker(extraLabel.c_str(), cvarExtraName, extra_default_color, false,
                                       UIWidgets::ColorPickerRandomButton | UIWidgets::ColorPickerResetButton |
                                           UIWidgets::ColorPickerRainbowCheck,
                                       theme)) {
            extra_color = CVarGetColor(cvarExtraName, extra_default_color);
        }
    }
    if (tooltip != NULL && strlen(tooltip) != 0) {
        ImGui::SameLine();
        ImGui::Text(" ?");
        UIWidgets::Tooltip(tooltip);
    }
    UIWidgets::PopStyleCombobox();
}

void InternalRecalculateAvailableChecks(RandomizerRegion startingRegion, RandoAgeTime startingAgeTime) {
    if (availableChecksDisplay == AC_DISABLED || !GameInteractor::IsSaveLoaded()) {
        return;
    }

    ResetPerformanceTimer(PT_RECALCULATE_AVAILABLE_CHECKS);
    StartPerformanceTimer(PT_RECALCULATE_AVAILABLE_CHECKS);

    int16_t entranceIndex = gPlayState->nextEntranceIndex;
    if (startingRegion == RR_ROOT && entranceIndex >= 0 && entranceIndex < ENTR_MAX) {
        // Try to find a mapped entrance
        // e.g. ENTR_DEKU_TREE_0_1 (index 1) is not mapped, but ENTR_DEKU_TREE_ENTRANCE (index 0) is mapped
        const int8_t scene = gEntranceTable[entranceIndex].scene;
        for (; entranceIndex >= 0 && gEntranceTable[entranceIndex].scene == scene; entranceIndex--) {
            const auto entrance = Rando::EntranceShuffler::GetEntranceByIndex(entranceIndex);
            if (entrance != nullptr) {
                startingRegion = entrance->GetOriginalConnectedRegionKey();
                break;
            }
        }
    }

    if (startingAgeTime == RAT_NONE) {
        if (LINK_IS_CHILD && IS_DAY) {
            startingAgeTime = RAT_CHILD_DAY;
        } else if (LINK_IS_CHILD && IS_NIGHT) {
            startingAgeTime = RAT_CHILD_NIGHT;
        } else if (LINK_IS_ADULT && IS_DAY) {
            startingAgeTime = RAT_ADULT_DAY;
        } else if (LINK_IS_ADULT && IS_NIGHT) {
            startingAgeTime = RAT_ADULT_NIGHT;
        }
    }

    ReachabilitySearch({}, RG_NONE, true, startingRegion, startingAgeTime);
    RecalculateAllAreaTotals();

    StopPerformanceTimer(PT_RECALCULATE_AVAILABLE_CHECKS);
    SPDLOG_INFO("Recalculate Available Checks Time: {}ms",
                GetPerformanceTimer(PT_RECALCULATE_AVAILABLE_CHECKS).count());
}

void RecalculateAvailableChecks(RandomizerRegion startingRegion /* = RR_ROOT */,
                                RandoAgeTime startingAgeTime /* = RAT_NONE */) {
    recalculateAvailable = true;
    availableChecksStartingRegion = startingRegion;
    availableChecksStartingAgeTime = startingAgeTime;
}

void LoadFromPreset(nlohmann::json info) {
    presetLoaded = true;
    presetPos = { info["pos"]["x"], info["pos"]["y"] };
    presetSize = { info["size"]["width"], info["size"]["height"] };
}

void CheckTrackerWindow::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

static std::map<int32_t, const char*> ageChecksDisplayOptions = { { AC_DISABLED, "Disabled" },
                                                                  { AC_ONE_ICON, "One Age Icon" },
                                                                  { AC_TWO_ICONS, "Two Age Icons" } };
static std::map<int32_t, const char*> availableChecksDisplayOptions = { { AC_DISABLED, "Disabled" },
                                                                        { AC_LOCKED_UNLOCKED, "Locked / Unlocked" },
                                                                        { AC_ONE_ICON, "One Age Icon" },
                                                                        { AC_TWO_ICONS, "Two Age Icons" } };

void CheckTrackerSettingsWindow::DrawElement() {
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    if (ImGui::BeginTable("CheckTrackerSettingsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("General settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Section settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        SohGui::GetSohMenu()->MenuDrawItem(backgroundColorWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

        SohGui::GetSohMenu()->MenuDrawItem(windowTypeWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

        UIWidgets::CVarSliderFloat("Font Size", CVAR_TRACKER_CHECK("FontSize"),
                                   UIWidgets::FloatSliderOptions()
                                       .Tooltip("Sets the font size used in the check tracker.")
                                       .Format("%.1f")
                                       .Step(0.1f)
                                       .Min(0.3f)
                                       .Max(2.0f)
                                       .Color(THEME_COLOR)
                                       .DefaultValue(1.0f));

        if (CVarGetInteger(CVAR_TRACKER_CHECK("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
            UIWidgets::CVarCheckbox("Enable Dragging", CVAR_TRACKER_CHECK("Draggable"),
                                    UIWidgets::CheckboxOptions().Color(THEME_COLOR));
            UIWidgets::CVarCheckbox("Only Enable While Paused", CVAR_TRACKER_CHECK("ShowOnlyPaused"),
                                    UIWidgets::CheckboxOptions().Color(THEME_COLOR));
            UIWidgets::CVarCombobox("Display Mode", CVAR_TRACKER_CHECK("DisplayType"), showMode,
                                    UIWidgets::ComboboxOptions()
                                        .LabelPosition(UIWidgets::LabelPositions::Far)
                                        .ComponentAlignment(UIWidgets::ComponentAlignments::Right)
                                        .Color(THEME_COLOR)
                                        .DefaultIndex(0));
            if (CVarGetInteger(CVAR_TRACKER_CHECK("DisplayType"), TRACKER_DISPLAY_ALWAYS) ==
                TRACKER_DISPLAY_COMBO_BUTTON) {
                UIWidgets::CVarCombobox("Combo Button 1", CVAR_TRACKER_CHECK("ComboButton1"), buttonStrings,
                                        UIWidgets::ComboboxOptions()
                                            .LabelPosition(UIWidgets::LabelPositions::Far)
                                            .ComponentAlignment(UIWidgets::ComponentAlignments::Right)
                                            .Color(THEME_COLOR)
                                            .DefaultIndex(TRACKER_COMBO_BUTTON_L));
                UIWidgets::CVarCombobox("Combo Button 2", CVAR_TRACKER_CHECK("ComboButton2"), buttonStrings,
                                        UIWidgets::ComboboxOptions()
                                            .LabelPosition(UIWidgets::LabelPositions::Far)
                                            .ComponentAlignment(UIWidgets::ComponentAlignments::Right)
                                            .Color(THEME_COLOR)
                                            .DefaultIndex(TRACKER_COMBO_BUTTON_L));
            }
        }
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        SohGui::GetSohMenu()->MenuDrawItem(dungeonSpoilerWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
        ImGui::EndDisabled();

        SohGui::GetSohMenu()->MenuDrawItem(hideUnshuffledShopWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

        SohGui::GetSohMenu()->MenuDrawItem(showGSWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

        SohGui::GetSohMenu()->MenuDrawItem(showLogicWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

        SohGui::GetSohMenu()->MenuDrawItem(ageChecksWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        SohGui::GetSohMenu()->MenuDrawItem(checkAvailabilityWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
        SohGui::GetSohMenu()->MenuDrawItem(availableChecksNoneWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
        SohGui::GetSohMenu()->MenuDrawItem(availableChecksChildWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
        SohGui::GetSohMenu()->MenuDrawItem(availableChecksAdultWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
        SohGui::GetSohMenu()->MenuDrawItem(availableChecksBothWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
        ImGui::EndDisabled();

        // Filtering settings
        UIWidgets::CVarCheckbox(
            "Filter Empty Areas", CVAR_TRACKER_CHECK("HideFilteredAreas"),
            UIWidgets::CheckboxOptions()
                .Tooltip("If enabled, will hide area headers that have no locations matching filter")
                .Color(THEME_COLOR)
                .DefaultValue(true));

        ImGui::SeparatorText("Tracker Header Visibility");
        UIWidgets::CVarCheckbox("Hidden Items Toggle", CVAR_TRACKER_CHECK("HiddenItemsToggleVisible"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        UIWidgets::CVarCheckbox("Available Checks Toggle", CVAR_TRACKER_CHECK("AvailableChecksToggleVisible"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        UIWidgets::CVarCheckbox("Expand/Collapse Buttons", CVAR_TRACKER_CHECK("ExpandCollapseButtonsVisible"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR).DefaultValue(false));
        UIWidgets::CVarCheckbox("Search Input", CVAR_TRACKER_CHECK("SearchInputVisible"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        UIWidgets::CVarCheckbox("Check Totals", CVAR_TRACKER_CHECK("CheckTotalsVisible"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));

        ImGui::TableNextColumn();

        CheckTracker::ImGuiDrawTwoColorPickerSection("Area Incomplete", CVAR_TRACKER_CHECK("AreaIncomplete.MainColor"),
                                                     CVAR_TRACKER_CHECK("AreaIncomplete.ExtraColor"),
                                                     Color_Area_Incomplete_Main, Color_Area_Incomplete_Extra,
                                                     Color_Main_Default, Color_Area_Incomplete_Extra_Default,
                                                     CVAR_TRACKER_CHECK("AreaIncomplete.Hide"), "", THEME_COLOR);
        CheckTracker::ImGuiDrawTwoColorPickerSection("Area Complete", CVAR_TRACKER_CHECK("AreaComplete.MainColor"),
                                                     CVAR_TRACKER_CHECK("AreaComplete.ExtraColor"),
                                                     Color_Area_Complete_Main, Color_Area_Complete_Extra,
                                                     Color_Main_Default, Color_Area_Complete_Extra_Default,
                                                     CVAR_TRACKER_CHECK("AreaComplete.Hide"), "", THEME_COLOR);
        CheckTracker::ImGuiDrawTwoColorPickerSection(
            "Unchecked", CVAR_TRACKER_CHECK("Unchecked.MainColor"), CVAR_TRACKER_CHECK("Unchecked.ExtraColor"),
            Color_Unchecked_Main, Color_Unchecked_Extra, Color_Main_Default, Color_Unchecked_Extra_Default,
            CVAR_TRACKER_CHECK("Unchecked.Hide"), "Checks you have not interacted with at all.", THEME_COLOR);
        CheckTracker::ImGuiDrawTwoColorPickerSection(
            "Skipped", CVAR_TRACKER_CHECK("Skipped.MainColor"), CVAR_TRACKER_CHECK("Skipped.ExtraColor"),
            Color_Skipped_Main, Color_Skipped_Extra, Color_Main_Default, Color_Skipped_Extra_Default,
            CVAR_TRACKER_CHECK("Skipped.Hide"), "", THEME_COLOR);
        CheckTracker::ImGuiDrawTwoColorPickerSection(
            "Seen", CVAR_TRACKER_CHECK("Seen.MainColor"), CVAR_TRACKER_CHECK("Seen.ExtraColor"), Color_Seen_Main,
            Color_Seen_Extra, Color_Main_Default, Color_Seen_Extra_Default, CVAR_TRACKER_CHECK("Seen.Hide"),
            "Used for shops. Shows item names for shop slots when walking in, and prices when highlighting them in buy "
            "mode.",
            THEME_COLOR);
        CheckTracker::ImGuiDrawTwoColorPickerSection(
            "Scummed", CVAR_TRACKER_CHECK("Scummed.MainColor"), CVAR_TRACKER_CHECK("Scummed.ExtraColor"),
            Color_Scummed_Main, Color_Scummed_Extra, Color_Main_Default, Color_Scummed_Extra_Default,
            CVAR_TRACKER_CHECK("Scummed.Hide"),
            "Checks you collect, but then reload before saving so you no longer have them.", THEME_COLOR);
        // CheckTracker::ImGuiDrawTwoColorPickerSection("Hinted (WIP)",     CVAR_TRACKER_CHECK("Hinted.MainColor"),
        // CVAR_TRACKER_CHECK("Hinted.ExtraColor"),          Color_Hinted_Main,            Color_Hinted_Extra,
        // Color_Main_Default, Color_Hinted_Extra_Default,          CVAR_TRACKER_CHECK("Hinted.Hide"),         "",
        // THEME_COLOR);
        CheckTracker::ImGuiDrawTwoColorPickerSection(
            "Collected", CVAR_TRACKER_CHECK("Collected.MainColor"), CVAR_TRACKER_CHECK("Collected.ExtraColor"),
            Color_Collected_Main, Color_Collected_Extra, Color_Main_Default, Color_Collected_Extra_Default,
            CVAR_TRACKER_CHECK("Collected.Hide"), "Checks you have collected without saving or reloading yet.",
            THEME_COLOR);
        CheckTracker::ImGuiDrawTwoColorPickerSection(
            "Saved", CVAR_TRACKER_CHECK("Saved.MainColor"), CVAR_TRACKER_CHECK("Saved.ExtraColor"), Color_Saved_Main,
            Color_Saved_Extra, Color_Main_Default, Color_Saved_Extra_Default, CVAR_TRACKER_CHECK("Saved.Hide"),
            "Checks that you saved the game while having collected.", THEME_COLOR);

        ImGui::PopStyleVar(1);
        ImGui::EndTable();
    }
}

void CheckTrackerWindow::InitElement() {
    SaveManager::Instance->AddInitFunction(InitTrackerData);
    sectionId = SaveManager::Instance->AddSaveFunction("trackerData", 1, SaveFile, true, SECTION_PARENT_NONE);
    SaveManager::Instance->AddLoadFunction("trackerData", 1, LoadFile);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(CheckTrackerLoadGame);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](uint32_t fileNum) { Teardown(); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(CheckTrackerItemReceive);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>(CheckTrackerTransition);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShopSlotChange>(CheckTrackerShopSlotChange);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(CheckTrackerSceneFlagSet);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(CheckTrackerFlagSet);
}

void CheckTrackerWindow::UpdateElement() {
}

void RegisterCheckTrackerWidgets() {
    backgroundColorWidget = { .name = "Background Color##CheckTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    backgroundColorWidget.CVar(CVAR_TRACKER_CHECK("BgColor"))
        .Options(
            ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_Bg_Default).UseAlpha().ShowReset().ShowRandom());
    SohGui::GetSohMenu()->AddSearchWidget({ backgroundColorWidget, "Randomizer", "Check Tracker", "General Settings" });

    windowTypeWidget = { .name = "Window Type##CheckTracker", .type = WidgetType::WIDGET_CVAR_COMBOBOX };
    windowTypeWidget.CVar(CVAR_TRACKER_CHECK("WindowType"))
        .Options(ComboboxOptions()
                     .DefaultIndex(TRACKER_WINDOW_WINDOW)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Far)
                     .Color(THEME_COLOR)
                     .ComboMap(windowType));
    SohGui::GetSohMenu()->AddSearchWidget({ windowTypeWidget, "Randomizer", "Check Tracker", "General Settings" });

    dungeonSpoilerWidget = { .name = "Vanilla/MQ Dungeon Spoilers", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    dungeonSpoilerWidget.CVar(CVAR_TRACKER_CHECK("MQSpoilers"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("If enabled, Vanilla/MQ dungeons will show on the tracker immediately. "
                              "Otherwise, Vanilla/MQ dungeon locations must be unlocked."));
    SohGui::GetSohMenu()->AddSearchWidget({ dungeonSpoilerWidget, "Randomizer", "Check Tracker", "General Settings" });

    hideUnshuffledShopWidget = { .name = "Hide Unshuffled Shop Item Checks", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    hideUnshuffledShopWidget.CVar(CVAR_TRACKER_CHECK("HideUnshuffledShopChecks"))
        .Options(
            CheckboxOptions()
                .Color(THEME_COLOR)
                .Tooltip("If enabled, will prevent the tracker from displaying slots with non-shop-item shuffles."))
        .Callback([&](WidgetInfo& info) {
            hideShopUnshuffledChecks = CVarGetInteger(CVAR_TRACKER_CHECK("HideUnshuffledShopChecks"), 0);
            UpdateFilters();
        });
    SohGui::GetSohMenu()->AddSearchWidget(
        { hideUnshuffledShopWidget, "Randomizer", "Check Tracker", "General Settings" });

    showGSWidget = { .name = "Always Show Gold Skulltulas", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    showGSWidget.CVar(CVAR_TRACKER_CHECK("AlwaysShowGSLocs"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("If enabled, will show GS locations in the tracker regardless of tokensanity settings."))
        .Callback([&](WidgetInfo& info) {
            alwaysShowGS = !alwaysShowGS;
            UpdateFilters();
        });
    SohGui::GetSohMenu()->AddSearchWidget({ showGSWidget, "Randomizer", "Check Tracker", "General Settings" });

    showLogicWidget = { .name = "Show Logic", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    showLogicWidget.CVar(CVAR_TRACKER_CHECK("ShowLogic"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("If enabled, will show a check's logic when hovering over it."));
    SohGui::GetSohMenu()->AddSearchWidget({ showLogicWidget, "Randomizer", "Check Tracker", "General Settings" });

    ageChecksWidget = { .name = "Show Check Ages", .type = WidgetType::WIDGET_CVAR_COMBOBOX };
    ageChecksWidget.CVar(CVAR_TRACKER_CHECK("AgeChecksDisplay"))
        .Options(ComboboxOptions()
                     .DefaultIndex(AC_DISABLED)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Near)
                     .Color(THEME_COLOR)
                     .ComboMap(ageChecksDisplayOptions)
                     .Tooltip("If enabled, will show the age for when a check can be collected."));

    checkAvailabilityWidget = { .name = "Enable Available Checks", .type = WidgetType::WIDGET_CVAR_COMBOBOX };
    checkAvailabilityWidget.CVar(CVAR_TRACKER_CHECK("AvailableChecksDisplay"))
        .Options(ComboboxOptions()
                     .DefaultIndex(AC_DISABLED)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Near)
                     .Color(THEME_COLOR)
                     .ComboMap(availableChecksDisplayOptions)
                     .Tooltip("If enabled, will show the checks that are available to be collected "
                              "with your current progress."))
        .Callback([&](WidgetInfo& info) {
            auto previousAvailableChecksDisplay = availableChecksDisplay;
            availableChecksDisplay =
                (AvailableChecksDisplay)CVarGetInteger(CVAR_TRACKER_CHECK("AvailableChecksDisplay"), 0);
            if (previousAvailableChecksDisplay == AC_DISABLED && availableChecksDisplay != AC_DISABLED) {
                RecalculateAvailableChecks();
            }
        });

    availableChecksNoneWidget = { .name = "None Available Icon", .type = WidgetType::WIDGET_CVAR_INPUT };
    availableChecksNoneWidget.CVar(CVAR_TRACKER_CHECK("AvailableChecksNoneIcon"))
        .Options(InputOptions()
                     .DefaultValue("ICON_FA_LOCK")
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Near)
                     .Size(ImVec2(400, 0))
                     .Color(THEME_COLOR)
                     .Tooltip("Icon for when the check is not available to child or adult."))
        .Callback([&](WidgetInfo& info) {
            availableChecksNoneAvailableIcon =
                MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksNoneIcon"), "ICON_FA_LOCK"));
        });

    availableChecksChildWidget = { .name = "Child Available Icon", .type = WidgetType::WIDGET_CVAR_INPUT };
    availableChecksChildWidget.CVar(CVAR_TRACKER_CHECK("AvailableChecksChildIcon"))
        .Options(InputOptions()
                     .DefaultValue("ICON_FA_CHILD")
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Near)
                     .Size(ImVec2(400, 0))
                     .Color(THEME_COLOR)
                     .Tooltip("Icon for when the check is available as a child."))
        .Callback([&](WidgetInfo& info) {
            availableChecksChildAvailableIcon =
                MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksChildIcon"), "ICON_FA_CHILD"));
        });

    availableChecksAdultWidget = { .name = "Adult Available Icon", .type = WidgetType::WIDGET_CVAR_INPUT };
    availableChecksAdultWidget.CVar(CVAR_TRACKER_CHECK("AvailableChecksAdultIcon"))
        .Options(InputOptions()
                     .DefaultValue("ICON_FA_USER")
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Near)
                     .Size(ImVec2(400, 0))
                     .Color(THEME_COLOR)
                     .Tooltip("Icon for when the check is available as an adult."))
        .Callback([&](WidgetInfo& info) {
            availableChecksAdultAvailableIcon =
                MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksAdultIcon"), "ICON_FA_USER"));
        });

    availableChecksBothWidget = { .name = "Both Available Icon", .type = WidgetType::WIDGET_CVAR_INPUT };
    availableChecksBothWidget.CVar(CVAR_TRACKER_CHECK("AvailableChecksBothIcon"))
        .Options(InputOptions()
                     .DefaultValue("ICON_FA_USERS")
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Near)
                     .Size(ImVec2(400, 0))
                     .Color(THEME_COLOR)
                     .Tooltip("Icon for when the check is available as a child and an adult."))
        .Callback([&](WidgetInfo& info) {
            availableChecksBothAvailableIcon =
                MapAvailableCheckIcon(CVarGetString(CVAR_TRACKER_CHECK("AvailableChecksBothIcon"), "ICON_FA_USERS"));
        });
}

static RegisterMenuInitFunc menuInitFunc(RegisterCheckTrackerWidgets);
} // namespace CheckTracker

#ifndef RANDO_LOGIC_H
#define RANDO_LOGIC_H

#include "Rando/Rando.h"
#include "Rando/ActorBehavior/Souls.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"

#include <set>
#include <cassert>

extern "C" {
#include "functions.h"
#include "variables.h"
}

namespace Rando {

namespace Logic {

// Time slice enum - 45 granular time points throughout MM's 3-day cycle
enum TimeSlice {
    TIME_DAY1_AM_06_00 = 0,
    TIME_DAY1_AM_07_00,
    TIME_DAY1_AM_08_00,
    TIME_DAY1_AM_10_00,
    TIME_DAY1_PM_01_45,
    TIME_DAY1_PM_03_00,
    TIME_DAY1_PM_04_00,
    TIME_NIGHT1_PM_06_00,
    TIME_NIGHT1_PM_08_00,
    TIME_NIGHT1_PM_09_00,
    TIME_NIGHT1_PM_10_00,
    TIME_NIGHT1_PM_11_00,
    TIME_NIGHT1_AM_12_00,
    TIME_NIGHT1_AM_02_30,
    TIME_NIGHT1_AM_04_00,
    TIME_NIGHT1_AM_05_00,
    TIME_DAY2_AM_06_00,
    TIME_DAY2_AM_07_00,
    TIME_DAY2_AM_08_00,
    TIME_DAY2_AM_10_00,
    TIME_DAY2_AM_11_30,
    TIME_DAY2_PM_02_00,
    TIME_DAY2_PM_04_00,
    TIME_NIGHT2_PM_06_00,
    TIME_NIGHT2_PM_08_00,
    TIME_NIGHT2_PM_09_00,
    TIME_NIGHT2_PM_10_00,
    TIME_NIGHT2_PM_11_00,
    TIME_NIGHT2_AM_12_00,
    TIME_NIGHT2_AM_04_00,
    TIME_NIGHT2_AM_05_00,
    TIME_DAY3_AM_06_00,
    TIME_DAY3_AM_07_00,
    TIME_DAY3_AM_08_00,
    TIME_DAY3_AM_10_00,
    TIME_DAY3_AM_11_30,
    TIME_DAY3_PM_01_00,
    TIME_NIGHT3_PM_06_00,
    TIME_NIGHT3_PM_08_00,
    TIME_NIGHT3_PM_09_00,
    TIME_NIGHT3_PM_10_00,
    TIME_NIGHT3_PM_11_00,
    TIME_NIGHT3_AM_12_00,
    TIME_NIGHT3_AM_04_00,
    TIME_NIGHT3_AM_05_00 // = 44
};

// Time slice count and bitmask constants
// Derived from enum - update if last enum value changes
constexpr int TIME_SLICE_COUNT = TIME_NIGHT3_AM_05_00 + 1;
constexpr uint64_t TIME_BIT_ONE = 1ULL;              // Base value for bit shifting
constexpr uint64_t TIME_ALL_SLICES = 0x1FFFFFFFFFFF; // All 45 time bits set

// Half-day period definitions for Clock Shuffle
struct HalfDayRange {
    int startSlice;
    int endSlice;
};

// Map half-day indices to their time slice ranges
// Index 0-5 correspond to HALF_DAY1_DAY through HALF_DAY3_NIGHT
constexpr HalfDayRange HALF_DAY_TIME_RANGES[6] = {
    { 0, 6 },   // HALF_DAY1_DAY   (TIME_DAY1_AM_06_00 to TIME_DAY1_PM_04_00)
    { 7, 15 },  // HALF_DAY1_NIGHT (TIME_NIGHT1_PM_06_00 to TIME_NIGHT1_AM_05_00)
    { 16, 22 }, // HALF_DAY2_DAY   (TIME_DAY2_AM_06_00 to TIME_DAY2_PM_04_00)
    { 23, 30 }, // HALF_DAY2_NIGHT (TIME_NIGHT2_PM_06_00 to TIME_NIGHT2_AM_05_00)
    { 31, 36 }, // HALF_DAY3_DAY   (TIME_DAY3_AM_06_00 to TIME_DAY3_PM_01_00)
    { 37, 44 }, // HALF_DAY3_NIGHT (TIME_NIGHT3_PM_06_00 to TIME_NIGHT3_AM_05_00)
};

// Game time constants for day/night transitions
constexpr u16 GAME_TIME_DAY_START = 0x4000;   // 6:00 AM
constexpr u16 GAME_TIME_NIGHT_START = 0xc000; // 6:00 PM

// Time state for tracking time accessibility during logic solving
struct RegionTimeState {
    uint64_t timeSlices;
    bool canStayOverTime;
};

extern uint64_t gCurrentRegionTime;

// Helper: Convert runtime game time to TimeSlice enum
TimeSlice TimeSliceFromGameTime(s32 day, u16 time);

// Helper: Returns the initial time state for logic solving
RegionTimeState InitialTimeState();

// Shared initialization function for region time states
std::unordered_map<RandoRegionId, RegionTimeState> InitializeRegionTimeStates(RandoRegionId startRegion);

// Helper to ensure region time state exists
void EnsureRegionTimeState(std::unordered_map<RandoRegionId, RegionTimeState>& regionTimeStates,
                           RandoRegionId regionId);

// Helper to set current region time
inline void SetCurrentRegionTime(const std::unordered_map<RandoRegionId, RegionTimeState>& regionTimeStates,
                                 RandoRegionId regionId) {
    gCurrentRegionTime = regionTimeStates.at(regionId).timeSlices;
}

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions,
                          std::unordered_map<RandoRegionId, RegionTimeState>& regionTimeStates);
RandoRegionId GetRegionIdFromEntrance(s32 entrance);
void GeneratePools(RandoSaveInfo& saveInfo, std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool);
void ApplyGlitchlessLogicToSaveContext(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool);
void ApplyNearlyNoLogicToSaveContext(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool);
void ApplyNoLogicToSaveContext(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool);
bool StaysAtVanillaCheck(RandoItemId itemId, const RandoSaveInfo& saveInfo);
bool IsSongLocationItem(RandoItemId itemId);
bool IsItemAllowedAtCheck(RandoItemId itemId, RandoCheckId checkId);
size_t SelectItemForCheck(const std::vector<RandoItemId>& itemPool, const std::vector<RandoCheckId>& checkPool,
                          RandoCheckId checkId);
void PreplaceConfinedItems(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool);

struct RandoRegionExit {
    s32 returnEntrance;
    std::function<bool()> condition;
    std::string conditionString;
};

struct RandoRegion {
    const char* name = "";
    SceneId sceneId;
    std::map<RandoCheckId, std::pair<std::function<bool()>, std::string>> checks;
    std::map<s32, RandoRegionExit> exits;
    std::map<RandoRegionId, std::pair<std::function<bool()>, std::string>> connections;
    std::vector<std::pair<RandoEvent, std::function<bool()>>> events;
    std::set<s32> oneWayEntrances;

    // Time logic fields for Clock Shuffle and time-based region access
    uint64_t timeSlices = 0;     // Bitfield: accessible time slices (bits 0-44) - unused in current implementation
    bool canStayOverTime = true; // Can player wait for time to pass? Set false for dungeons, shops with closing hours
    std::unordered_map<TimeSlice, std::function<bool()>>
        timeStayRestrictions; // Time slices where staying is restricted - use STAY() macro in region definitions
};

extern std::map<RandoRegionId, RandoRegion> Regions;

// ============================================================================
// TIME LOGIC NAMESPACE
// ============================================================================
namespace TimeLogic {
// Core expansion function - sequential time expansion with stay restrictions
uint64_t ExpandTimeForward(uint64_t timeSlices, const RandoRegion& region);

// Owned time calculation - aggregates all owned half-day time slices
uint64_t GetOwnedTimeSlices();

// Validation helper for clock ownership during logic generation
void ValidateRegionTimeOwnership(RandoRegionId regionId, RandoCheckId checkId, uint64_t regionTime,
                                 const char* context);
} // namespace TimeLogic

// TODO: This may not stay here
#define IS_DEKU (GET_PLAYER_FORM == PLAYER_FORM_DEKU)
#define IS_ZORA (GET_PLAYER_FORM == PLAYER_FORM_ZORA)
#define IS_DEITY (GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY)
#define IS_GORON (GET_PLAYER_FORM == PLAYER_FORM_GORON)
#define IS_HUMAN (GET_PLAYER_FORM == PLAYER_FORM_HUMAN)
#define HAS_ITEM(item) (INV_CONTENT(item) == item)
#define CAN_BE_DEKU (IS_DEKU || HAS_ITEM(ITEM_MASK_DEKU))
#define CAN_BE_ZORA (IS_ZORA || HAS_ITEM(ITEM_MASK_ZORA))
#define CAN_BE_DEITY (IS_DEITY || HAS_ITEM(ITEM_MASK_FIERCE_DEITY))
#define CAN_BE_GORON (IS_GORON || HAS_ITEM(ITEM_MASK_GORON))
#define CAN_BE_HUMAN                                                                                        \
    (IS_HUMAN || (IS_DEITY && HAS_ITEM(ITEM_MASK_FIERCE_DEITY)) || (IS_ZORA && HAS_ITEM(ITEM_MASK_ZORA)) || \
     (IS_DEKU && HAS_ITEM(ITEM_MASK_DEKU)) || (IS_GORON && HAS_ITEM(ITEM_MASK_GORON)))
#define CHECK_MAX_HP(TARGET_HP) ((TARGET_HP * 16) <= gSaveContext.save.saveInfo.playerData.healthCapacity)
#define HAS_MAGIC (gSaveContext.save.saveInfo.playerData.isMagicAcquired)
#define CAN_HOOK_SCARECROW \
    (HAS_ITEM(ITEM_OCARINA_OF_TIME) && HAS_ITEM(ITEM_HOOKSHOT) && canPlaySong(OCARINA_SONG_SCARECROW_SPAWN))
#define CAN_USE_EXPLOSIVE                             \
    (HAS_ITEM(ITEM_BOMB) || HAS_ITEM(ITEM_BOMBCHU) || \
     (HAS_ITEM(ITEM_MASK_BLAST) && GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) > EQUIP_VALUE_SHIELD_NONE))
#define CAN_USE_HUMAN_SWORD (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI)
#define CAN_USE_SWORD (CAN_USE_HUMAN_SWORD || HAS_ITEM(ITEM_SWORD_GREAT_FAIRY) || CAN_BE_DEITY)
#define CAN_FULLY_CUT_KEATON_GRASS                                                      \
    (HAS_MAGIC && (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_RAZOR) && \
     CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK))
// Be careful here, as some checks require you to play the song as a specific form
#define CAN_PLAY_SONG(song)                                                   \
    (HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_##song) && \
     Rando::Logic::canPlaySong((QUEST_SONG_##song - QUEST_SONG_SONATA) + OCARINA_SONG_SONATA))
#define CAN_RIDE_EPONA (CAN_PLAY_SONG(EPONA))
#define GBT_CAN_REVERSE_WATER_FLOW                                                         \
    (RANDO_EVENTS[RE_GREAT_BAY_RED_SWITCH_1] && RANDO_EVENTS[RE_GREAT_BAY_RED_SWITCH_2] && \
     HAS_ITEM(ITEM_HOOKSHOT)) // Keeping for the sake of check tracker clarity
#define GBT_GREEN_SWITCH_FLOW                                                                  \
    (RANDO_EVENTS[RE_GREAT_BAY_GREEN_SWITCH_1] && RANDO_EVENTS[RE_GREAT_BAY_GREEN_SWITCH_2] && \
     RANDO_EVENTS[RE_GREAT_BAY_GREEN_SWITCH_3])
#define ONE_WAY_EXIT -1
#define CAN_OWL_WARP(owlId) ((gSaveContext.save.saveInfo.playerData.owlActivationFlags >> owlId) & 1)
#define SET_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags |= (1 << owlId))
#define CLEAR_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags &= ~(1 << owlId))
#define HAS_BOTTLE_ITEM(item) (Inventory_HasItemInBottle(item))
// TODO: Maybe not reliable because of theif bird stealing bottle
#define HAS_BOTTLE (INV_CONTENT(ITEM_BOTTLE) != ITEM_NONE)
#define CAN_USE_PROJECTILE (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || (CAN_BE_DEKU && HAS_MAGIC) || CAN_BE_ZORA)
#define CAN_ACCESS(access) (RANDO_EVENTS[RE_ACCESS_##access])
#define CAN_GROW_BEAN_PLANT        \
    (HAS_ITEM(ITEM_MAGIC_BEANS) && \
     (CAN_PLAY_SONG(STORMS) || (HAS_BOTTLE && (CAN_ACCESS(SPRING_WATER) || CAN_ACCESS(HOT_SPRING_WATER)))))
// Bean patches that auto-water on Day 2 (Doggy Racetrack, Deku Palace Upper)
// Rain only occurs Day 2 from 7:00 AM to 5:30 PM (not Night 2)
// Requires magic beans and (Day 2 OR manual watering)
#define CAN_USE_DAY2_RAIN_BEAN (CAN_GROW_BEAN_PLANT || (HAS_ITEM(ITEM_MAGIC_BEANS) && CLOCK_DAY2()))
#define CAN_USE_MAGIC_ARROW(arrowType) (HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_##arrowType) && HAS_MAGIC)
#define CAN_LIGHT_TORCH_NEAR_ANOTHER (HAS_ITEM(ITEM_DEKU_STICK) || CAN_USE_MAGIC_ARROW(FIRE))
#define KEY_COUNT(dungeon) (gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_##dungeon])
#define CAN_AFFORD(rc)                                                                                                \
    ((RANDO_SAVE_CHECKS[rc].price < 100) || (RANDO_SAVE_CHECKS[rc].price <= 200 && CUR_UPG_VALUE(UPG_WALLET) >= 1) || \
     (CUR_UPG_VALUE(UPG_WALLET) >= 2))
#define HAS_ENOUGH_STRAY_FAIRIES(dungeonIndex) \
    (gSaveContext.save.saveInfo.inventory.strayFairies[dungeonIndex] >= RANDO_SAVE_OPTIONS[RO_STRAY_FAIRIES_REQUIRED])
#define FOUND_ALL_FROGS                                                                  \
    (CHECK_WEEKEVENTREG(WEEKEVENTREG_33_01) && CHECK_WEEKEVENTREG(WEEKEVENTREG_32_40) && \
     CHECK_WEEKEVENTREG(WEEKEVENTREG_32_80) && CHECK_WEEKEVENTREG(WEEKEVENTREG_33_02))
#define CAN_USE_ABILITY(ability) Flags_GetRandoInf(RI_ABILITY_##ability - RI_ABILITY_SWIM + RANDO_INF_OBTAINED_SWIM)
#define HAS_ENOUGH_SKULLTULA_TOKENS(sceneId) \
    (Inventory_GetSkullTokenCount(sceneId) >= RANDO_SAVE_OPTIONS[RO_SKULLTULA_TOKENS_REQUIRED])

#define EVENT(randoEvent, condition)         \
    {                                        \
        randoEvent, [] { return condition; } \
    }
#define EXIT(toEntrance, fromEntrance, condition)                           \
    {                                                                       \
        toEntrance, {                                                       \
            fromEntrance, [] { return condition; }, LogicString(#condition) \
        }                                                                   \
    }
#define CONNECTION(region, condition)                         \
    {                                                         \
        region, {                                             \
            [] { return condition; }, LogicString(#condition) \
        }                                                     \
    }
#define CHECK(check, condition)                               \
    {                                                         \
        check, {                                              \
            [] { return condition; }, LogicString(#condition) \
        }                                                     \
    }
// STAY macro for region time restrictions - defines when player can stay in a region over time
// Usage in region definitions: .timeStayRestrictions = { STAY(TIME_NIGHT1_PM_08_00, !HAS_ROOM_KEY) }
// If condition is false at the specified time, player is kicked out (expansion stops permanently)
// Examples:
//   STAY(TIME_NIGHT1_PM_08_00, !Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)) // Kicked out without room key
//   STAY(TIME_NIGHT3_PM_10_00, false) // Always kicked out at this time (shop closes)
#define STAY(timeSlice, condition)          \
    {                                       \
        timeSlice, [] { return condition; } \
    }

inline std::string LogicString(std::string condition) {
    if (condition == "true")
        return "";

    return condition;
}

inline uint8_t FoundOcarinaButtons() {
    uint8_t foundButtons = 0;
    for (int i = RANDO_INF_OBTAINED_OCARINA_BUTTON_A; i <= RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP; i++) {
        if (Flags_GetRandoInf((RandoInf)i)) {
            foundButtons++;
        }
    }
    return foundButtons;
}

inline bool canPlaySong(u8 songId) {
    switch (songId) {
        case OCARINA_SONG_SONATA:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT));
        case OCARINA_SONG_GORON_LULLABY:
        case OCARINA_SONG_GORON_LULLABY_INTRO:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT));
        case OCARINA_SONG_NEW_WAVE:
        case OCARINA_SONG_ELEGY:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN));
        case OCARINA_SONG_OATH:
        case OCARINA_SONG_WIND_FISH_ZORA:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP));
        case OCARINA_SONG_TIME:
        case OCARINA_SONG_INVERTED_TIME:
        case OCARINA_SONG_DOUBLE_TIME:
        case OCARINA_SONG_WIND_FISH_GORON:
        case OCARINA_SONG_EVAN_PART1:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN));
        case OCARINA_SONG_HEALING:
        case OCARINA_SONG_SARIAS:
        case OCARINA_SONG_EVAN_PART2:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN));
        case OCARINA_SONG_EPONAS:
        case OCARINA_SONG_WIND_FISH_HUMAN:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT));
        case OCARINA_SONG_SOARING:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP));
        case OCARINA_SONG_STORMS:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP));
        case OCARINA_SONG_SUNS:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP));
        case OCARINA_SONG_WIND_FISH_DEKU:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT));
        case OCARINA_SONG_SCARECROW_SPAWN:
            return FoundOcarinaButtons() >= 2;
        case OCARINA_SONG_TERMINA_WALL:
            return (Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_DOWN) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_RIGHT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_LEFT) &&
                    Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_C_UP));
        default:
            return true;
    }
}

inline bool CanAccessDungeon(DungeonSceneIndex dungeonIndex) {
    bool hasSongAccess = false;
    bool hasFormAccess = false;
    switch (dungeonIndex) {
        case DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE:
            hasSongAccess = CAN_PLAY_SONG(SONATA);
            hasFormAccess = CAN_BE_DEKU && HAS_ITEM(ITEM_OCARINA_OF_TIME);
            break;
        case DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE:
            hasSongAccess = CAN_PLAY_SONG(LULLABY);
            hasFormAccess = CAN_BE_GORON && HAS_ITEM(ITEM_OCARINA_OF_TIME);
            break;
        case DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE:
            hasSongAccess = CAN_PLAY_SONG(BOSSA_NOVA);
            hasFormAccess = CAN_BE_ZORA && HAS_ITEM(ITEM_OCARINA_OF_TIME);
            break;
        default:
            break;
    }
    switch (RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS]) {
        case RO_ACCESS_DUNGEONS_FORM_OR_SONG:
            return hasSongAccess || hasFormAccess;
        case RO_ACCESS_DUNGEONS_FORM_ONLY:
            return hasFormAccess;
        case RO_ACCESS_DUNGEONS_SONG_ONLY:
            return hasSongAccess;
        case RO_ACCESS_DUNGEONS_OPEN:
            return true;
        case RO_ACCESS_DUNGEONS_FORM_AND_SONG:
        default:
            return hasSongAccess && hasFormAccess;
    }
}

inline uint32_t MoonMaskCount() {
    uint32_t count = 0;
    for (int i = ITEM_MASK_TRUTH; i <= ITEM_MASK_GIANT; i++) {
        if (INV_CONTENT(i) == i) {
            count++;
        }
    }
    return count;
}

inline uint32_t RemainsCount() {
    uint32_t count = 0;
    for (int i = QUEST_REMAINS_ODOLWA; i <= QUEST_REMAINS_TWINMOLD; i++) {
        if (CHECK_QUEST_ITEM(i)) {
            count++;
        }
    }
    return count;
}

inline bool MeetsMoonRequirements() {
    return RemainsCount() >= RANDO_SAVE_OPTIONS[RO_ACCESS_MOON_REMAINS_COUNT] &&
           MoonMaskCount() >= RANDO_SAVE_OPTIONS[RO_ACCESS_MOON_MASKS_COUNT];
}

// ============================================================================
// CLOCK OWNERSHIP HELPERS
// ============================================================================

inline uint32_t ClockCount() {
    uint32_t count = 0;
    for (int i = 0; i < 6; ++i) {
        if (Flags_GetRandoInf(static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + i))) {
            count++;
        }
    }
    return count;
}

inline bool SettingClocks() {
    return RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE] != 0;
}

// Centralized clock ownership check
inline bool OwnsClockHalfDay(int halfDayIndex) {
    if (halfDayIndex < 0 || halfDayIndex >= 6)
        return false;
    RandoInf clockFlag = static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + halfDayIndex);
    return Flags_GetRandoInf(clockFlag);
}

// New consolidated helper that encapsulates ascending/descending/random logic
inline bool OwnsHalfDayForMode(int halfDayIndex) {
    if (!SettingClocks() || halfDayIndex < 0 || halfDayIndex >= 6) {
        return !SettingClocks(); // If not shuffling clocks, all time is available
    }

    int clockMode = RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE_PROGRESSIVE];
    uint32_t totalClocks = ClockCount();

    switch (clockMode) {
        case RO_CLOCK_SHUFFLE_RANDOM:
            // Random mode: check if this specific half-day is owned
            return OwnsClockHalfDay(halfDayIndex);

        case RO_CLOCK_SHUFFLE_ASCENDING:
            // Ascending: own first N half-days in sequence (0,1,2,3,4,5)
            return totalClocks > halfDayIndex;

        case RO_CLOCK_SHUFFLE_DESCENDING:
            // Descending: own last N half-days in reverse sequence (5,4,3,2,1,0)
            return totalClocks > (5 - halfDayIndex);

        default:
            return false;
    }
}

// ============================================================================
// TIME OPERATOR FUNCTIONS
// ============================================================================

inline bool RawAt(TimeSlice slice) {
    return (gCurrentRegionTime & (TIME_BIT_ONE << slice)) != 0;
}

inline bool RawBefore(TimeSlice slice) {
    if (slice == 0)
        return false;
    uint64_t mask = (TIME_BIT_ONE << slice) - 1;
    return (gCurrentRegionTime & mask) != 0;
}

inline bool RawAfter(TimeSlice slice) {
    uint64_t mask = ~((TIME_BIT_ONE << slice) - 1) & TIME_ALL_SLICES;
    return (gCurrentRegionTime & mask) != 0;
}

inline bool RawBetween(TimeSlice start, TimeSlice end) {
    uint64_t mask = ((TIME_BIT_ONE << end) - 1) & ~((TIME_BIT_ONE << start) - 1);
    return (gCurrentRegionTime & mask) != 0;
}

// Generate bitmask for a half-day period's time slices
inline constexpr uint64_t GetHalfDayTimeMask(int halfDayIndex) {
    if (halfDayIndex < 0 || halfDayIndex >= 6)
        return 0;

    uint64_t mask = 0;
    const auto& range = HALF_DAY_TIME_RANGES[halfDayIndex];
    for (int slice = range.startSlice; slice <= range.endSlice; ++slice) {
        mask |= (1ULL << slice);
    }
    return mask;
}

// Merge two time states (bitwise OR on time slices)
inline RegionTimeState MergeTimeStates(const RegionTimeState& a, const RegionTimeState& b) {
    return { .timeSlices = a.timeSlices | b.timeSlices, .canStayOverTime = a.canStayOverTime || b.canStayOverTime };
}

// Check if 'a' covers 'b' (a is a superset - adding b gives nothing new)
inline bool TimeStateCovers(const RegionTimeState& a, const RegionTimeState& b) {
    return (a.timeSlices | b.timeSlices) == a.timeSlices;
}

// ============================================================================
// CLOCK ITEM MACROS
// ============================================================================

// Simplified clock macros using consolidated helper
#define CLOCK_DAY1() OwnsHalfDayForMode(0)
#define CLOCK_NIGHT1() OwnsHalfDayForMode(1)
#define CLOCK_DAY2() OwnsHalfDayForMode(2)
#define CLOCK_NIGHT2() OwnsHalfDayForMode(3)
#define CLOCK_DAY3() OwnsHalfDayForMode(4)
#define CLOCK_NIGHT3() OwnsHalfDayForMode(5)

// ============================================================================
// CLOCK SHUFFLE VALIDATION FUNCTIONS
// ============================================================================

// Validation: Check if a time slice is in an owned half-day period
inline bool IsTimeSliceOwned(TimeSlice slice) {
    if (!SettingClocks())
        return true;

    for (int i = 0; i < 6; ++i) {
        const auto& range = HALF_DAY_TIME_RANGES[i];
        if (slice >= range.startSlice && slice <= range.endSlice) {
            return OwnsClockHalfDay(i);
        }
    }
    return false;
}

// Validation: Check if any time in the timeslice mask is owned
inline bool HasAnyOwnedTime(uint64_t timeSlices) {
    if (!SettingClocks())
        return true;

    for (int i = 0; i < 6; ++i) {
        if (OwnsClockHalfDay(i)) {
            uint64_t halfDayMask = GetHalfDayTimeMask(i);
            if (timeSlices & halfDayMask) {
                return true;
            }
        }
    }
    return false;
}

// ============================================================================
// COMPOSITE TIME CHECKS
// ============================================================================

#define IS_DAY1() (RawBefore(TIME_NIGHT1_PM_06_00) && CLOCK_DAY1())
#define IS_NIGHT1() (RawBetween(TIME_NIGHT1_PM_06_00, TIME_DAY2_AM_06_00) && CLOCK_NIGHT1())
#define IS_DAY2() (RawBetween(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_06_00) && CLOCK_DAY2())
#define IS_NIGHT2() (RawBetween(TIME_NIGHT2_PM_06_00, TIME_DAY3_AM_06_00) && CLOCK_NIGHT2())
#define IS_DAY3() (RawBetween(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_06_00) && CLOCK_DAY3())
#define IS_NIGHT3() (RawAfter(TIME_NIGHT3_PM_06_00) && CLOCK_NIGHT3())

// Global clock filter for owned time periods
// Returns true if Clock Shuffle is disabled OR if player has access to any time period
inline bool ClockFilter() {
    if (!SettingClocks())
        return true;

    return IS_DAY1() || IS_NIGHT1() || IS_DAY2() || IS_NIGHT2() || IS_DAY3() || IS_NIGHT3();
}

#define IS_DAY() (IS_DAY1() || IS_DAY2() || IS_DAY3())
#define IS_NIGHT() (IS_NIGHT1() || IS_NIGHT2() || IS_NIGHT3())
#define FIRST_DAY() (IS_DAY1() || IS_NIGHT1())
#define SECOND_DAY() (IS_DAY2() || IS_NIGHT2())
#define FINAL_DAY() (IS_DAY3() || IS_NIGHT3())

// ============================================================================
// PUBLIC TIME API
// ============================================================================

#define AT(slice) (RawAt(slice) && ClockFilter())

#define BEFORE(slice) (RawBefore(slice) && ClockFilter())

#define AFTER(slice) (RawAfter(slice) && ClockFilter())

#define BETWEEN(s, e) (RawBetween(s, e) && ClockFilter())

#define MIDNIGHT()                                                                                             \
    (BETWEEN(TIME_NIGHT1_AM_12_00, TIME_DAY2_AM_06_00) || BETWEEN(TIME_NIGHT2_AM_12_00, TIME_DAY3_AM_06_00) || \
     AFTER(TIME_NIGHT3_AM_12_00))

inline bool CanKillEnemy(ActorId EnemyId) {
    // If enemy souls are shuffled, and the relevant soul is not obtained, we cannot kill that enemy.
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENEMY_SOULS] && !HaveEnemySoul(EnemyId)) {
        return false;
    }

    switch (EnemyId) {
        case ACTOR_BOSS_01: // Odolwa
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_USE_EXPLOSIVE || CAN_USE_MAGIC_ARROW(FIRE) ||
                    CAN_USE_MAGIC_ARROW(LIGHT)) &&
                   (Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_ODOLWA) ||
                    RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_NO);
        case ACTOR_BOSS_02: // Twinmold
            return (HAS_ITEM(ITEM_BOW) || (HAS_ITEM(ITEM_MASK_GIANT) && HAS_MAGIC && CAN_USE_HUMAN_SWORD)) &&
                   (Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_TWINMOLD) ||
                    RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_NO);
        case ACTOR_BOSS_03: // Gyorg
            return ((CAN_BE_DEITY && HAS_MAGIC) || (CAN_BE_ZORA && HAS_MAGIC)) &&
                   (Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_GYORG) ||
                    RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_NO);
        case ACTOR_BOSS_04: // Wart
            return (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA);
        case ACTOR_BOSS_HAKUGIN: // Goht
            return (CAN_USE_MAGIC_ARROW(FIRE)) && (Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_GOHT) ||
                                                   RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_NO);
        case ACTOR_EN_KNIGHT: // Igos du Ikana/IdI Lackey
            return (CAN_USE_MAGIC_ARROW(FIRE) &&
                    (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) >= EQUIP_VALUE_SHIELD_MIRROR) &&
                    (CAN_USE_SWORD || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA));
        case ACTOR_EN_KAIZOKU: // Fighter Pirate
            return (CAN_USE_SWORD || CAN_BE_ZORA);
        case ACTOR_EN_PAMETFROG: // Woodfall Temple Gekko (and Snapper)
            return (HAS_ITEM(ITEM_BOW) && CanKillEnemy(ACTOR_EN_BIGPAMET));
        case ACTOR_EN_BIGSLIME: // Great Bay Temple Gekko
            return (CAN_USE_MAGIC_ARROW(ICE));
        case ACTOR_EN_SW: // Gold Skulltula & Skullwalltula
            return (CAN_USE_PROJECTILE || CAN_BE_DEKU || CAN_BE_GORON || CAN_USE_SWORD || CAN_USE_EXPLOSIVE);
        case ACTOR_EN_DINOFOS: // Dinolfos
            return (CAN_USE_SWORD || CAN_BE_GORON || HAS_ITEM(ITEM_BOW) || (CAN_BE_DEKU && HAS_MAGIC));
        case ACTOR_EN_WIZ: // Wizrobe
            return (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_USE_SWORD || CAN_BE_GORON);
        case ACTOR_EN_WF: // Wolfos
            return (CAN_USE_SWORD || (CAN_BE_DEKU && HAS_MAGIC) || CAN_BE_GORON || CAN_BE_ZORA);
        case ACTOR_EN_JSO: // Garo
            return (HAS_ITEM(ITEM_MASK_GARO) && (HAS_ITEM(ITEM_BOW) || CAN_BE_GORON || CAN_USE_SWORD));
        case ACTOR_EN_JSO2: // Garo Master
            return (HAS_ITEM(ITEM_BOW) || CAN_BE_GORON || CAN_USE_SWORD);
        case ACTOR_EN_IK: // Iron Knuckle
            return (CAN_USE_SWORD || CAN_BE_GORON);
        case ACTOR_EN_GRASSHOPPER: // Dragonfly
            return ((CAN_BE_DEKU && HAS_MAGIC) || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_NUT) || CAN_USE_SWORD ||
                    CAN_BE_ZORA);
        case ACTOR_EN_MKK: // Boe
            return ((CAN_BE_DEKU && HAS_MAGIC) || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_NUT) || CAN_USE_SWORD ||
                    CAN_BE_ZORA || CAN_BE_GORON);
        case ACTOR_EN_BIGPAMET: // Snapper
            return (CAN_BE_DEKU || CAN_USE_EXPLOSIVE || CAN_BE_GORON);
        case ACTOR_EN_ST: // Large Skulltula
            return (CAN_USE_SWORD || CAN_USE_PROJECTILE || CAN_BE_GORON || CAN_USE_EXPLOSIVE);
        case ACTOR_EN_BAT: // Bad Bat
            return (CAN_USE_SWORD || HAS_ITEM(ITEM_HOOKSHOT) || HAS_ITEM(ITEM_BOW) || CAN_USE_EXPLOSIVE ||
                    CAN_BE_GORON || CAN_BE_ZORA);
        case ACTOR_EN_DEKUBABA: // Neck bending Deku Baba
            return (CAN_USE_SWORD || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA || HAS_ITEM(ITEM_BOW) ||
                    CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK));
        case ACTOR_OBJ_SNOWBALL: // Large Snowball
            return (CAN_USE_EXPLOSIVE || CAN_BE_GORON || CAN_USE_MAGIC_ARROW(FIRE));
        case ACTOR_EN_AM: // Armos
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU || CAN_USE_EXPLOSIVE);
        case ACTOR_EN_VM: // Beamos
            return (CAN_USE_EXPLOSIVE);
        case ACTOR_EN_BB:     // Blue Bubble
        case ACTOR_EN_BBFALL: // Red Bubble
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_DEKU_STICK));
        case ACTOR_EN_RAT:       // Real Bombchu
        case ACTOR_EN_TUBO_TRAP: // Flying Pot
            return true;
        case ACTOR_EN_FAMOS: // Death Armos
            return (CAN_USE_MAGIC_ARROW(LIGHT));
        case ACTOR_EN_DODONGO: // Dodongo
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC));
        case ACTOR_EN_FLOORMAS: // Floormaster
        case ACTOR_EN_WALLMAS:  // Wallmaster
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK));
        case ACTOR_EN_FZ: // Freezard
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_USE_MAGIC_ARROW(FIRE) ||
                    HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_CROW: // Guay (Generic, excludes the one circling Clock Town En_Ruppecrow)
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_FIREFLY: // Keese
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_RR: // Like Like
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK));
        case ACTOR_EN_DEKUNUTS: // Mad Scrub
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_KAREBABA: // Wilted/Mini Babas
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU);
        case ACTOR_EN_PEEHAT: // Peahat
            return ((CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU || HAS_ITEM(ITEM_DEKU_STICK)) &&
                    IS_DAY());
        case ACTOR_EN_RD: // Redead & Gibdos
            return (CAN_USE_SWORD || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA || HAS_ITEM(ITEM_DEKU_STICK));
        case ACTOR_EN_BSB: // Captain Keeta (May be possible without bow, but the window is tight. Requiring for now)
            return (HAS_ITEM(ITEM_BOW) &&
                    (CAN_USE_SWORD || HAS_ITEM(ITEM_DEKU_STICK) || CAN_USE_EXPLOSIVE || CAN_BE_GORON || CAN_BE_ZORA));
        case ACTOR_EN_SKB: // Stalchild
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_TITE: // Tektite
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW));
        case ACTOR_EN_SLIME: // Chuchus
            return (CAN_USE_SWORD || CAN_BE_ZORA || CAN_BE_DEKU || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_DEKU_STICK));
        case ACTOR_EN_SNOWMAN: // Eeno
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_WDHAND: // Dexihand (Basic kill method, seems like a pain to require other things)
            return (CAN_BE_ZORA && HAS_MAGIC);
        case ACTOR_EN_KAME: // Snapper (non Gekko Miniboss)
            return (CAN_USE_EXPLOSIVE || CAN_BE_GORON);
        case ACTOR_EN_OKUTA: // Octorok
        case ACTOR_EN_EGOL:  // Eyegore
            return (CAN_USE_PROJECTILE);
        case ACTOR_EN_BAGUO: // Nejiron
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_NEO_REEBA: // Leever
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK));
        case ACTOR_EN_PP: // Hiploop
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) ||
                    HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_HOOKSHOT));
        case ACTOR_EN_PR:  // Desbreko
        case ACTOR_EN_PR2: // Skull fish
        case ACTOR_EN_SB:  // Shellblade
            return (CAN_BE_ZORA && HAS_MAGIC && CAN_USE_ABILITY(SWIM));
        case ACTOR_BOSS_05: // Bio Deku Baba
            return CAN_BE_ZORA && CAN_USE_ABILITY(SWIM);
        case ACTOR_EN_BEE: // Giant Bee
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU || HAS_ITEM(ITEM_DEKU_STICK) ||
                    CAN_USE_PROJECTILE || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_NUT));
        case ACTOR_EN_DRAGON: // Deep Python
            return (CAN_BE_ZORA && HAS_MAGIC);
        case ACTOR_EN_BIGPO:
            return HAS_ITEM(ITEM_BOW);
        case ACTOR_EN_PO_SISTERS:
            // The first three sisters can be damaged with almost anything, but Meg requires ranged attacks. Not using
            // CAN_USE_EXPLOSIVE here, as the Blast Mask cannot reach, and the Powder Keg can only be used once.
            return (CAN_USE_PROJECTILE || HAS_ITEM(ITEM_BOMB) || HAS_ITEM(ITEM_BOMBCHU));
        case ACTOR_EN_INVADEPOH: // Them
            return HAS_ITEM(ITEM_BOW);
        case ACTOR_EN_THIEFBIRD: // Takkuri
            return (CAN_USE_PROJECTILE || CAN_BE_GORON || CAN_BE_ZORA || CAN_USE_EXPLOSIVE || CAN_USE_SWORD);
        case ACTOR_EN_DEATH: // Gomess
            return (CAN_USE_SWORD || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA || CAN_BE_GORON ||
                    (CAN_BE_DEKU && HAS_MAGIC));
        default: // Incorrect actor ID inputed.
            assert(false);
            return false;
    }
}

} // namespace Logic

} // namespace Rando

#endif // RANDO_LOGIC_H

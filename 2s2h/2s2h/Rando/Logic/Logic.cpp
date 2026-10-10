#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "Logic.h"

namespace Rando {

namespace Logic {

std::map<RandoRegionId, RandoRegion> Regions = {};

uint64_t gCurrentRegionTime = 0;

// GROTTOS
// grottos are a bit weird, 15 of them share entrances with other grottos, so we have to check an extra field to
// determine which grotto we're in, and convert the entrance ID to an unused one that we store for logic purposes.
// ENTRANCE(GROTTOS, 17) = RR_TERMINA_FIELD_COW_GROTTO
// ENTRANCE(GROTTOS, 18) = RR_TERMINA_FIELD_PILLAR_GROTTO
// ENTRANCE(GROTTOS, 19) = RR_TERMINA_FIELD_TALL_GRASS_GROTTO
// ENTRANCE(GROTTOS, 20) = RR_ROAD_TO_SOUTHERN_SWAMP_GROTTO
// ENTRANCE(GROTTOS, 21) = RR_SOUTHERN_SWAMP_GROTTO
// ENTRANCE(GROTTOS, 22) = RR_WOODS_OF_MYSTERY_GROTTO
// ENTRANCE(GROTTOS, 23) = RR_MOUNTAIN_VILLAGE_TUNNEL_GROTTO
// ENTRANCE(GROTTOS, 24) = RR_PATH_TO_GORON_VILLAGE_RAMP_GROTTO
// ENTRANCE(GROTTOS, 25) = RR_PATH_TO_SNOWHEAD_GROTTO
// ENTRANCE(GROTTOS, 26) = RR_GREAT_BAY_COAST_FISHERMAN_GROTTO
// ENTRANCE(GROTTOS, 27) = RR_GREAT_BAY_COAST_COW_GROTTO
// ENTRANCE(GROTTOS, 28) = RR_ZORA_CAPE_GROTTO
// ENTRANCE(GROTTOS, 29) = RR_IKANA_CANYON_GROTTO
// ENTRANCE(GROTTOS, 30) = RR_IKANA_GRAVEYARD_GROTTO
// ENTRANCE(GROTTOS, 31) = RR_ROAD_TO_IKANA_GROTTO

RandoRegionId GetRegionIdFromEntrance(s32 entrance) {
    static std::map<s32, RandoRegionId> entranceToRegionId;
    if (entranceToRegionId.empty()) {
        for (auto& [randoRegionId, randoRegion] : Regions) {
            for (auto& [_, regionExit] : randoRegion.exits) {
                if (regionExit.returnEntrance == ONE_WAY_EXIT) {
                    continue;
                }
                entranceToRegionId[regionExit.returnEntrance] = randoRegionId;
            }
            for (auto& entrance : randoRegion.oneWayEntrances) {
                entranceToRegionId[entrance] = randoRegionId;
            }
        }
    }

    s32 modifiedEntrance = entrance;
    if (entrance == ENTRANCE(GROTTOS, 4)) {
        switch (gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance) {
            case ENTRANCE(TERMINA_FIELD, 0):
                modifiedEntrance = gSaveContext.respawn[RESPAWN_MODE_UNK_3].data == 0x3F ? ENTRANCE(GROTTOS, 19)
                                                                                         : ENTRANCE(GROTTOS, 18);
                break;
            case ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 20);
                break;
            case ENTRANCE(SOUTHERN_SWAMP_POISONED, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 21);
                break;
            case ENTRANCE(WOODS_OF_MYSTERY, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 22);
                break;
            case ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 23);
                break;
            case ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 24);
                break;
            case ENTRANCE(PATH_TO_SNOWHEAD, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 25);
                break;
            case ENTRANCE(GREAT_BAY_COAST, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 26);
                break;
            case ENTRANCE(ZORA_CAPE, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 28);
                break;
            case ENTRANCE(IKANA_CANYON, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 29);
                break;
            case ENTRANCE(IKANA_GRAVEYARD, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 30);
                break;
            case ENTRANCE(ROAD_TO_IKANA, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 31);
                break;
        }
    } else if (entrance == ENTRANCE(GROTTOS, 10)) {
        switch (gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance) {
            case ENTRANCE(TERMINA_FIELD, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 17);
                break;
            case ENTRANCE(GREAT_BAY_COAST, 0):
                modifiedEntrance = ENTRANCE(GROTTOS, 27);
                break;
        }
    }

    if (entranceToRegionId.contains(modifiedEntrance)) {
        return entranceToRegionId[modifiedEntrance];
    }

    return RR_MAX;
}

// Helper: Convert runtime game time to TimeSlice enum for dynamic time checking
TimeSlice TimeSliceFromGameTime(s32 day, u16 time) {
    // Handle edge cases: day 0 or invalid inputs
    if (day < 1 || day > 3) {
        return TIME_DAY1_AM_06_00; // Default fallback
    }

    // Convert to time slice based on day/time ranges
    // This is approximate - exact mapping would need game time constants
    bool isNight = (time >= GAME_TIME_NIGHT_START || time < GAME_TIME_DAY_START);
    int halfDayOffset = (day - 1) * 2 + (isNight ? 1 : 0);

    // Map to approximate time slice within the half-day
    if (halfDayOffset >= 6)
        return TIME_NIGHT3_AM_05_00;

    const auto& range = HALF_DAY_TIME_RANGES[halfDayOffset];
    return static_cast<TimeSlice>(range.startSlice);
}

// Helper: Returns the initial time state for logic solving (start at Day 1, 6:00 AM)
RegionTimeState InitialTimeState() {
    return { .timeSlices = (TIME_BIT_ONE << TIME_DAY1_AM_06_00), .canStayOverTime = false };
}

// Shared initialization function for region time states
std::unordered_map<RandoRegionId, RegionTimeState> InitializeRegionTimeStates(RandoRegionId startRegion) {
    std::unordered_map<RandoRegionId, RegionTimeState> states;

    // Start with appropriate time based on Clock Shuffle
    if (SettingClocks()) {
        // Clock Shuffle: start with owned time slices only
        states[startRegion] = { .timeSlices = TimeLogic::GetOwnedTimeSlices(), .canStayOverTime = false };
    } else {
        // No Clock Shuffle: start at Day 1 6am
        states[startRegion] = InitialTimeState();
    }

    return states;
}

// Helper to ensure region time state exists
void EnsureRegionTimeState(std::unordered_map<RandoRegionId, RegionTimeState>& regionTimeStates,
                           RandoRegionId regionId) {
    if (regionTimeStates.find(regionId) == regionTimeStates.end()) {
        auto& region = Regions[regionId];
        regionTimeStates[regionId] = { .timeSlices = TimeLogic::GetOwnedTimeSlices(),
                                       .canStayOverTime = region.canStayOverTime };
    }
}

// Time expansion during region traversal with stay restrictions
// Time expansion semantics: if canStayOverTime, sequentially test each future time slice
// Stop permanently if any timeStayRestrictions check fails
void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions,
                          std::unordered_map<RandoRegionId, RegionTimeState>& regionTimeStates) {
    // Ensure current region has time state
    EnsureRegionTimeState(regionTimeStates, currentRegion);

    auto& sourceRegion = Regions[currentRegion];
    auto& sourceTimeState = regionTimeStates[currentRegion];

    // Expand time if player can wait in this region
    uint64_t currentTime = sourceTimeState.timeSlices;
    if (sourceTimeState.canStayOverTime) {
        currentTime = TimeLogic::ExpandTimeForward(currentTime, sourceRegion);
        sourceTimeState.timeSlices = currentTime;
    }

    // Explore connections
    for (auto& [connectedRegionId, condition] : sourceRegion.connections) {
        // Set global time for check evaluation
        gCurrentRegionTime = currentTime;

        if (condition.first()) {
            auto& targetRegion = Regions[connectedRegionId];
            RegionTimeState incomingState = { .timeSlices = currentTime,
                                              .canStayOverTime = targetRegion.canStayOverTime };

            auto existingIt = regionTimeStates.find(connectedRegionId);
            if (existingIt != regionTimeStates.end()) {
                // Region already visited - check if we have new time to add
                if (TimeStateCovers(existingIt->second, incomingState)) {
                    continue; // Skip - existing state already covers incoming
                }
                // Merge time states for re-exploration
                existingIt->second = MergeTimeStates(existingIt->second, incomingState);
            } else {
                // First visit to this region
                reachableRegions.insert(connectedRegionId);
                regionTimeStates[connectedRegionId] = incomingState;
            }

            FindReachableRegions(connectedRegionId, reachableRegions, regionTimeStates);
        }
    }

    // Explore exits
    for (auto& [exitId, regionExit] : sourceRegion.exits) {
        // Set global time for check evaluation
        gCurrentRegionTime = currentTime;

        RandoRegionId connectedRegionId = GetRegionIdFromEntrance(exitId);
        if (regionExit.condition()) {
            auto& targetRegion = Regions[connectedRegionId];
            RegionTimeState incomingState = { .timeSlices = currentTime,
                                              .canStayOverTime = targetRegion.canStayOverTime };

            auto existingIt = regionTimeStates.find(connectedRegionId);
            if (existingIt != regionTimeStates.end()) {
                // Region already visited - check if we have new time to add
                if (TimeStateCovers(existingIt->second, incomingState)) {
                    continue; // Skip - existing state already covers incoming
                }
                // Merge time states for re-exploration
                existingIt->second = MergeTimeStates(existingIt->second, incomingState);
            } else {
                // First visit to this region
                reachableRegions.insert(connectedRegionId);
                regionTimeStates[connectedRegionId] = incomingState;
            }

            FindReachableRegions(connectedRegionId, reachableRegions, regionTimeStates);
        }
    }
}

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_MAX] = RandoRegion{ .sceneId = SCENE_MAX,
        .checks = {
            CHECK(RC_STARTING_ITEM_DEKU_MASK, true),
            CHECK(RC_STARTING_ITEM_SONG_OF_HEALING, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),                      ONE_WAY_EXIT, true), // Save warp
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 9),                      ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_CLOCK_TOWN)),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 10),              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP)),
            EXIT(ENTRANCE(WOODFALL, 4),                              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_WOODFALL)),
            EXIT(ENTRANCE(MILK_ROAD, 4),                             ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MILK_ROAD)),
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),               ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE)),
            EXIT(ENTRANCE(SNOWHEAD, 3),                              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SNOWHEAD)),
            EXIT(ENTRANCE(GREAT_BAY_COAST, 11),                      ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_GREAT_BAY_COAST)),
            EXIT(ENTRANCE(ZORA_CAPE, 6),                             ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_ZORA_CAPE)),
            EXIT(ENTRANCE(IKANA_CANYON, 4),                          ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_IKANA_CANYON)),
            EXIT(ENTRANCE(STONE_TOWER, 3),                           ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_STONE_TOWER)),
        },
    };
}, {});
// clang-format on

} // namespace Logic

} // namespace Rando

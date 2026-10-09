#include "Logic.h"
#include "2s2h/Rando/Types.h"

#include <numeric>
#include <iterator>
#include <spdlog/spdlog.h>

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
uint64_t GetUnixTimestamp();
}

namespace Rando {

namespace Logic {

void ApplyGlitchlessLogicToSaveContext(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool) {
    uint64_t tick = GetUnixTimestamp();

    SaveContext copiedSaveContext;
    memcpy(&copiedSaveContext, &gSaveContext, sizeof(SaveContext));

    std::set<RandoRegionId> regionsInLogic = { RR_MAX };
    std::set<RandoCheckId> checksInLogic;
    std::set<std::pair<RandoEvent, std::function<bool()>>*> eventsInLogic;

    // Initialize time states using shared function
    std::unordered_map<RandoRegionId, RegionTimeState> regionTimeStates = InitializeRegionTimeStates(RR_MAX);

    RandoCheckId checkWithJunk = RC_UNKNOWN;
    std::set<RandoItemId> nonJunkItemsThatWeHaveTried;
    std::vector<RandoCheckId> checksWithJunk;
    std::vector<int> checksWithJunkWeights;
    int weight = 1;

    // Inital shuffle of the item pool (Following shuffles done at the end of the loop)
    if (itemPool.size() > 1) {
        for (size_t i = 0; i < itemPool.size(); i++) {
            size_t j = Ship_Random(0, itemPool.size());
            std::swap(itemPool[i], itemPool[j]);
        }
    }

    auto handleError = [&](std::string message) {
        SPDLOG_ERROR("Items/Checks: {}/{}", itemPool.size(), checkPool.size());

        // Log out the checks that are still in the pool
        for (auto& randoCheckId : checkPool) {
            SPDLOG_ERROR("Check still in pool: {}", Rando::StaticData::Checks[randoCheckId].name);
        }
        // Log out the items that are still in the pool
        for (RandoItemId randoItemId : itemPool) {
            SPDLOG_ERROR("Item still in pool: {}", Rando::StaticData::Items[randoItemId].spoilerName);
        }

        memcpy(&gSaveContext, &copiedSaveContext, sizeof(SaveContext));
        throw std::runtime_error(message);
    };

    while (true) {
        // Break if we've been running for too long
        if (GetUnixTimestamp() - tick > 10000) {
            handleError("Generation took too long, aborting");
        }

        bool regionsInLogicChanged = false;
        bool eventsInLogicChanged = false;
        bool checksInLogicChanged = false;

        // Crawl through all reachable regions and add any new reachable regions
        auto prevRegionsInLogicSize = regionsInLogic.size();
        for (RandoRegionId regionId : regionsInLogic) {
            FindReachableRegions(regionId, regionsInLogic, regionTimeStates);
        }
        if (regionsInLogic.size() != prevRegionsInLogicSize) {
            regionsInLogicChanged = true;
        }

        for (RandoRegionId regionId : regionsInLogic) {
            auto& randoRegion = Regions[regionId];

            // Set current region time for check evaluation
            SetCurrentRegionTime(regionTimeStates, regionId);

            // Apply any new events
            for (auto& randoEvent : randoRegion.events) {
                if (!eventsInLogic.contains(&randoEvent) && randoEvent.second()) {
                    RANDO_EVENTS[randoEvent.first]++;
                    eventsInLogic.insert(&randoEvent);
                    eventsInLogicChanged = true;
                }
            }

            // Apply any new checks
            for (auto& [randoCheckId, checkLogic] : randoRegion.checks) {
                if (!checksInLogic.contains(randoCheckId) && checkLogic.first()) {
                    // VALIDATION: Verify check is reachable with owned time
                    TimeLogic::ValidateRegionTimeOwnership(regionId, randoCheckId,
                                                           regionTimeStates[regionId].timeSlices, "Glitchless");

                    checksInLogic.insert(randoCheckId);

                    RandoItemId randoItemId = RANDO_SAVE_CHECKS[randoCheckId].randoItemId;

                    auto it = std::find(checkPool.begin(), checkPool.end(), randoCheckId);
                    bool inPool = it != checkPool.end();
                    if (inPool) {
                        checkPool.erase(it);

                        size_t pickIndex = SelectItemForCheck(itemPool, checkPool, randoCheckId);
                        if (pickIndex == itemPool.size()) {
                            handleError("No allowed item remains for reachable check: " +
                                        std::string(Rando::StaticData::Checks[randoCheckId].name));
                        }

                        randoItemId = RANDO_SAVE_CHECKS[randoCheckId].randoItemId = itemPool[pickIndex];
                        RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;

                        itemPool.erase(itemPool.begin() + pickIndex);

                        if (Rando::StaticData::Items[randoItemId].randoItemType == RITYPE_JUNK ||
                            Rando::StaticData::Items[randoItemId].randoItemType == RITYPE_HEALTH) {
                            checksWithJunk.push_back(randoCheckId);
                            checksWithJunkWeights.push_back(weight);
                            SPDLOG_TRACE("Item Placed: {}:{}", Rando::StaticData::Checks[randoCheckId].name,
                                         Rando::StaticData::Items[randoItemId].spoilerName);
                        } else {
                            SPDLOG_DEBUG("Item Placed: {}:{}", Rando::StaticData::Checks[randoCheckId].name,
                                         Rando::StaticData::Items[randoItemId].spoilerName);
                        }
                    }

                    GiveItem(ConvertItem(randoItemId));

                    // Update time states for all regions when time items are obtained
                    if (randoItemId >= RI_TIME_DAY_1 && randoItemId <= RI_TIME_PROGRESSIVE) {
                        uint64_t newTimeSlices = TimeLogic::GetOwnedTimeSlices();
                        // Update RR_MAX time state first - this is the source for new region discoveries
                        if (regionTimeStates.find(RR_MAX) != regionTimeStates.end()) {
                            regionTimeStates[RR_MAX].timeSlices = newTimeSlices;
                        }
                        // Update existing region time states to reflect new owned time
                        for (auto& [regionId, timeState] : regionTimeStates) {
                            timeState.timeSlices = newTimeSlices;
                            // Expand time forward based on region's stay restrictions
                            if (timeState.canStayOverTime) {
                                timeState.timeSlices = TimeLogic::ExpandTimeForward(newTimeSlices, Regions[regionId]);
                            }
                        }
                    }

                    checksInLogicChanged = true;
                }
            }
        }

        if (itemPool.empty()) {
            // Done!
            break;
        }

        // Choose a random check with junk, and attempt to place progressive items until we unlock something
        if (!regionsInLogicChanged && !checksInLogicChanged && !eventsInLogicChanged) {
            if (checkWithJunk == RC_UNKNOWN) {
                if (checksWithJunk.empty()) {
                    handleError("Out of checks to replace, cannot place remaining items");
                }

                if (checksWithJunk.size() == 1) {
                    checkWithJunk = checksWithJunk[0];
                    checksWithJunk.pop_back();
                    checksWithJunkWeights.pop_back();
                } else {
                    std::vector<double> cumulativeWeights(checksWithJunkWeights.size());
                    std::partial_sum(checksWithJunkWeights.begin(), checksWithJunkWeights.end(),
                                     cumulativeWeights.begin());
                    double random = Ship_Random(0, cumulativeWeights.back());
                    auto it = std::upper_bound(cumulativeWeights.begin(), cumulativeWeights.end(), random);
                    size_t index = std::distance(cumulativeWeights.begin(), it);

                    checkWithJunk = checksWithJunk[index];

                    // Remove the check from the list of checks with junk
                    checksWithJunk.erase(checksWithJunk.begin() + index);
                    checksWithJunkWeights.erase(checksWithJunkWeights.begin() + index);
                }

                SPDLOG_DEBUG("Nothing changed, attempting to replace check: {}",
                             Rando::StaticData::Checks[checkWithJunk].name);
            }

            std::vector<std::pair<RandoItemId, int>> nonJunkItemsThatWeHaveNotTried;
            bool anyNonJunkItemsLeft = false;
            for (size_t i = 0; i < itemPool.size(); i++) {
                if (Rando::StaticData::Items[itemPool[i]].randoItemType != RITYPE_JUNK &&
                    Rando::StaticData::Items[itemPool[i]].randoItemType != RITYPE_HEALTH) {
                    anyNonJunkItemsLeft = true;
                    if (nonJunkItemsThatWeHaveTried.find(itemPool[i]) == nonJunkItemsThatWeHaveTried.end() &&
                        IsItemAllowedAtCheck(itemPool[i], checkWithJunk)) {
                        nonJunkItemsThatWeHaveNotTried.push_back({ itemPool[i], i });
                    }
                }
            }

            if (!anyNonJunkItemsLeft) {
                handleError("No non-junk items left to place, progression is impossible");
            }

            if (nonJunkItemsThatWeHaveNotTried.empty()) {
                SPDLOG_TRACE("Already tried all non-junk items, leaving the last non-junk item in place: {}: {}",
                             Rando::StaticData::Checks[checkWithJunk].name,
                             Rando::StaticData::Items[RANDO_SAVE_CHECKS[checkWithJunk].randoItemId].spoilerName);
                checkWithJunk = RC_UNKNOWN;
                nonJunkItemsThatWeHaveTried.clear();
                continue;
            }

            // Remove item and place it back in the pool
            RandoItemId oldRandoItemId = RANDO_SAVE_CHECKS[checkWithJunk].randoItemId;
            auto& [newRandoItemId, indexInPool] = nonJunkItemsThatWeHaveNotTried[0];

            RANDO_SAVE_CHECKS[checkWithJunk].randoItemId = newRandoItemId;

            SPDLOG_DEBUG("Item Replaced Junk: {}:{}", Rando::StaticData::Checks[checkWithJunk].name,
                         Rando::StaticData::Items[newRandoItemId].spoilerName);

            RemoveItem(oldRandoItemId);
            GiveItem(ConvertItem(newRandoItemId));

            itemPool.erase(itemPool.begin() + indexInPool);
            itemPool.push_back(oldRandoItemId);

            nonJunkItemsThatWeHaveTried.insert(newRandoItemId);
            SPDLOG_TRACE("Attempting to replaced junk item: {}:{}", Rando::StaticData::Checks[checkWithJunk].name,
                         Rando::StaticData::Items[newRandoItemId].spoilerName);
        } else {
            weight++;
            if (checkWithJunk != RC_UNKNOWN) {
                SPDLOG_DEBUG("Successfully Replaced junk item with: {}:{}",
                             Rando::StaticData::Checks[checkWithJunk].name,
                             Rando::StaticData::Items[RANDO_SAVE_CHECKS[checkWithJunk].randoItemId].spoilerName);
            }
            checkWithJunk = RC_UNKNOWN;
            nonJunkItemsThatWeHaveTried.clear();

            // Shuffle the item pool
            if (itemPool.size() > 1) {
                for (size_t i = 0; i < itemPool.size(); i++) {
                    size_t j = Ship_Random(0, itemPool.size());
                    std::swap(itemPool[i], itemPool[j]);
                }
            }
        }
    }

    for (auto& randoCheckId : checksInLogic) {
        copiedSaveContext.save.shipSaveInfo.rando.randoSaveChecks[randoCheckId].randoItemId =
            RANDO_SAVE_CHECKS[randoCheckId].randoItemId;
        copiedSaveContext.save.shipSaveInfo.rando.randoSaveChecks[randoCheckId].shuffled =
            RANDO_SAVE_CHECKS[randoCheckId].shuffled;
    }

    memcpy(&gSaveContext, &copiedSaveContext, sizeof(SaveContext));

    SPDLOG_INFO("Successfully placed all items with Glitchless logic");
}

} // namespace Logic

} // namespace Rando

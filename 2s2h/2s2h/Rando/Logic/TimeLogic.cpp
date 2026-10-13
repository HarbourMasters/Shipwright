#include "Logic.h"
#include <spdlog/spdlog.h>

extern "C" {
#include "ShipUtils.h"
}

namespace Rando {
namespace Logic {
namespace TimeLogic {

// Core expansion function - expands accessible time forward with stay restrictions
// Implements sequential expansion: if a stay restriction fails, expansion stops permanently
// For unrestricted regions without Clock Shuffle: fast bitwise fill across all time slices (O(1))
// For Clock Shuffle or restricted regions: sequential expansion respecting boundaries
uint64_t ExpandTimeForward(uint64_t timeSlices, const RandoRegion& region) {
    // Fast path: unrestricted time expansion using bitwise fill (only when Clock Shuffle is off)
    if (region.timeStayRestrictions.empty() && !SettingClocks()) {
        uint64_t expanded = timeSlices;

        // Non-Clock Shuffle: expand across ALL time slices using bitwise fill
        expanded |= (expanded << 1);
        expanded |= (expanded << 2);
        expanded |= (expanded << 4);
        expanded |= (expanded << 8);
        expanded |= (expanded << 16);
        expanded |= (expanded << 32);
        expanded &= TIME_ALL_SLICES;

        return expanded;
    }

    // Slow path: restricted time expansion with sequential checking
    // In Clock Shuffle, filter input to only owned time
    uint64_t filteredTimeSlices = timeSlices;
    if (SettingClocks()) {
        filteredTimeSlices &= GetOwnedTimeSlices();
    }

    uint64_t expanded = filteredTimeSlices;
    bool canWait = false;

    for (int i = 0; i < TIME_SLICE_COUNT; ++i) {
        uint64_t mask = (TIME_BIT_ONE << i);

        if (filteredTimeSlices & mask) {
            // We can be at this time
            canWait = true;
            expanded |= mask;
        } else if (canWait) {
            // During Clock Shuffle, check if this time slice is owned
            if (SettingClocks() && !IsTimeSliceOwned(static_cast<TimeSlice>(i))) {
                canWait = false; // Can't expand into unowned time period
                continue;
            }

            // Check if we can wait to this time
            auto it = region.timeStayRestrictions.find(static_cast<TimeSlice>(i));
            if (it != region.timeStayRestrictions.end()) {
                if (it->second()) {
                    expanded |= mask; // Condition passed, add time
                } else {
                    canWait = false; // Kicked out, STOP expansion
                }
            } else {
                // No restriction = default true, can stay
                expanded |= mask;
            }
        }
    }

    // VALIDATION: In Clock Shuffle, expanded time must not exceed owned time
    if (SettingClocks()) {
        uint64_t ownedTimeSlices = GetOwnedTimeSlices();
        bool expandedBeyondOwned = (expanded & ~ownedTimeSlices) != 0;
        assert(!expandedBeyondOwned && "Time expansion exceeded owned half-day boundaries!");
    }

    return expanded;
}

// Owned time calculation - aggregates all owned half-day time slices
uint64_t GetOwnedTimeSlices() {
    if (!RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE]) {
        return TIME_ALL_SLICES;
    }

    uint64_t timeSlices = 0;
    for (int halfDayIndex = 0; halfDayIndex < 6; ++halfDayIndex) {
        if (OwnsClockHalfDay(halfDayIndex)) {
            timeSlices |= GetHalfDayTimeMask(halfDayIndex);
        }
    }

    // If no clocks are owned, ensure we at least have access to the start of the game (Day 1 6 AM)
    return timeSlices ? timeSlices : (TIME_BIT_ONE << TIME_DAY1_AM_06_00);
}

// Validation helper for clock ownership during logic generation
void ValidateRegionTimeOwnership(RandoRegionId regionId, RandoCheckId checkId, uint64_t regionTime,
                                 const char* context) {
    if (!SettingClocks())
        return;

    if (!HasAnyOwnedTime(regionTime)) {
        auto& region = Regions[regionId];
        SPDLOG_ERROR("CLOCK SHUFFLE VALIDATION FAILED ({})!", context);
        SPDLOG_ERROR("Check: {}", Rando::StaticData::Checks[checkId].name);
        SPDLOG_ERROR("Region: {} - {}", Ship_GetSceneName(region.sceneId), region.name);
        SPDLOG_ERROR("Region time mask: 0x{:X}", regionTime);
        SPDLOG_ERROR("Owned clocks: D1={} N1={} D2={} N2={} D3={} N3={}", OwnsClockHalfDay(0), OwnsClockHalfDay(1),
                     OwnsClockHalfDay(2), OwnsClockHalfDay(3), OwnsClockHalfDay(4), OwnsClockHalfDay(5));
        assert(false && "Check placed in unowned time period during Clock Shuffle!");
    }
}

} // namespace TimeLogic
} // namespace Logic
} // namespace Rando

#pragma once

#include <vector>
#include <array>
#include <chrono>

typedef enum {
  PT_WHOLE_SEED,
  PT_LOGIC_RESET,
  PT_REGION_RESET,
  PT_SPOILER_LOG,
  PT_ENTRANCE_SHUFFLE,
  PT_SHOPSANITY,
  PT_OWN_DUNGEON,
  PT_LIMITED_CHECKS,
  PT_ADVANCEMENT_ITEMS,
  PT_REMAINING_ITEMS,
  PT_PLAYTHROUGH_GENERATION,
  PT_PARE_DOWN_PLAYTHROUGH,
  PT_WOTH,
  PT_FOOLISH,
  PT_OVERRIDES,
  PT_HINTS,
  PT_EVENT_ACCESS,
  PT_TOD_ACCESS,
  PT_ENTRANCE_LOGIC,
  PT_LOCATION_LOGIC,
  PT_MAX
} TimerID;

void StartPerformanceTimer(TimerID timer);
void StopPerformanceTimer(TimerID timer);
std::chrono::duration<double, std::milli> GetPerformanceTimer(TimerID timer);
void ResetPerformanceTimers();
static std::array<std::chrono::duration<double, std::milli>, PT_MAX> totalTimes = {};
static std::array<std::chrono::high_resolution_clock::time_point, PT_MAX> timeStarted = {};


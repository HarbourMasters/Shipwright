#ifndef RANDO_CLOCK_SHUFFLE_H
#define RANDO_CLOCK_SHUFFLE_H

#include "Rando/Types.h"

namespace Rando {
namespace ClockItems {

enum ClockHalfIndex : int {
    INVALID = -1,
    HALF_DAY1_DAY = 0,
    HALF_DAY1_NIGHT = 1,
    HALF_DAY2_DAY = 2,
    HALF_DAY2_NIGHT = 3,
    HALF_DAY3_DAY = 4,
    HALF_DAY3_NIGHT = 5,
    TERMINAL_STATE = 6,
    HALF_COUNT = 6,
};

RandoItemId GetClockItemFromHalfDayIndex(int halfDayIndex);
int GetHalfDayIndexFromClockItem(RandoItemId clockItemId);
int FindOwnedHalfDay(bool fromEnd = false);
u8 GetAllOwnedHalfDaysMask();
bool IsClockItem(RandoItemId itemId);
bool IsDayClock(RandoItemId itemId);

} // namespace ClockItems

namespace ClockShuffle {

void OnFileLoad();
void SetTimeToHalfDayStart(int halfDayIndex);

bool IsTimeOwnedForClockShuffle(s32 day, u16 time);
std::string GetTimeDescriptionForMessage(s32 day, u16 time);

} // namespace ClockShuffle
} // namespace Rando

#endif // RANDO_CLOCK_SHUFFLE_H

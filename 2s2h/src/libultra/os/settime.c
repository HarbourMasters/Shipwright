#include "ultra64.h"

void osSetTime(OSTime ticks) {
    __osCurrentTime = ticks;
}

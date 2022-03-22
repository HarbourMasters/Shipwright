#include "global.h"

s32 osStopTimer(OSTimer* timer) {
    register u32 prevInt;
    OSTimer* next;

    if (!timer->next) {
        return -1;
    }

    prevInt = __osDisableInt();

    next = timer->next;
    if (next != __osTimerList) {
        next->value += timer->value;
    }

    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    timer->next = NULL;
    timer->prev = NULL;
    if (__osTimerList->next == __osTimerList) {
        __osSetCompare(0);
    }

    __osRestoreInt(prevInt);
    return 0;
}

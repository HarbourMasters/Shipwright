#include "ultra64.h"

int osStopTimer(OSTimer* t) {
    register u32 savedMask;
    OSTimer* timep;

    if (t->next == NULL) {
        return -1;
    }

    savedMask = __osDisableInt();

    timep = t->next;

    if (timep != __osTimerList) {
        timep->value += t->value;
    }

    t->prev->next = t->next;
    t->next->prev = t->prev;
    t->next = NULL;
    t->prev = NULL;

    if (__osTimerList->next == __osTimerList) {
        __osSetCompare(0);
    }

    __osRestoreInt(savedMask);

    return 0;
}

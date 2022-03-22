#include "global.h"

s32 osSetTimer(OSTimer* timer, OSTime countdown, OSTime interval, OSMesgQueue* mq, OSMesg msg) {
    OSTime time;
    OSTimer* next;
    u32 count;
    u32 value;
    u32 prevInt;

    timer->next = NULL;
    timer->prev = NULL;
    timer->interval = interval;

    if (countdown != 0) {
        timer->value = countdown;
    } else {
        timer->value = interval;
    }
    timer->mq = mq;
    timer->msg = msg;

    prevInt = __osDisableInt();
    if (__osTimerList->next != __osTimerList) {
        if (1) {}

        next = __osTimerList->next;
        count = osGetCount();
        value = count - __osTimerCounter;

        if (value < next->value) {
            next->value -= value;
        } else {
            next->value = 1;
        }
    }

    time = __osInsertTimer(timer);
    __osSetTimerIntr(__osTimerList->next->value);

    __osRestoreInt(prevInt);

    if (time) {} // suppresses set but unused warning

    return 0;
}

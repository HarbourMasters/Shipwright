#include "global.h"

OSTimer __osBaseTimer;
OSTime __osCurrentTime;
u32 __osBaseCounter;
u32 __osViIntrCount;
u32 __osTimerCounter;
OSTimer* __osTimerList = &__osBaseTimer;

void __osTimerServicesInit(void) {
    __osCurrentTime = 0;
    __osBaseCounter = 0;
    __osViIntrCount = 0;
    __osTimerList->prev = __osTimerList;
    __osTimerList->next = __osTimerList->prev;
    __osTimerList->value = 0;
    __osTimerList->interval = __osTimerList->value;
    __osTimerList->mq = NULL;
    __osTimerList->msg = NULL;
}

void __osTimerInterrupt(void) {
    OSTimer* timer;
    u32 sp20;
    u32 sp1c;

    if (__osTimerList->next == __osTimerList) {
        return;
    }

    while (true) {
        timer = __osTimerList->next;
        if (timer == __osTimerList) {
            __osSetCompare(0);
            __osTimerCounter = 0;
            break;
        }

        sp20 = osGetCount();
        sp1c = sp20 - __osTimerCounter;
        __osTimerCounter = sp20;
        if (sp1c < timer->value) {
            timer->value -= sp1c;
            __osSetTimerIntr(timer->value);
            break;
        }

        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        timer->next = NULL;
        timer->prev = NULL;
        if (timer->mq != NULL) {
            osSendMesg(timer->mq, timer->msg, OS_MESG_NOBLOCK);
        }
        if (timer->interval != 0) {
            timer->value = timer->interval;
            __osInsertTimer(timer);
        }
    }
}

void __osSetTimerIntr(OSTime time) {
    OSTime newTime;
    u32 prevInt;

    if (time < 468) {
        time = 468;
    }

    prevInt = __osDisableInt();

    __osTimerCounter = osGetCount();
    newTime = time + __osTimerCounter;
    __osSetCompare((u32)newTime);
    __osRestoreInt(prevInt);
}

OSTime __osInsertTimer(OSTimer* timer) {
    OSTimer* nextTimer;
    u64 timerValue;
    u32 prevInt = __osDisableInt();

    for (nextTimer = __osTimerList->next, timerValue = timer->value;
         nextTimer != __osTimerList && timerValue > nextTimer->value;
         timerValue -= nextTimer->value, nextTimer = nextTimer->next) {
        ;
    }

    timer->value = timerValue;
    if (nextTimer != __osTimerList) {
        nextTimer->value -= timerValue;
    }

    timer->next = nextTimer;
    timer->prev = nextTimer->prev;
    nextTimer->prev->next = timer;
    nextTimer->prev = timer;
    __osRestoreInt(prevInt);

    return timerValue;
}

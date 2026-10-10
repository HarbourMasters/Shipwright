#include "ultra64.h"

__OSThreadTail __osThreadTail = { NULL, OS_PRIORITY_THREADTAIL };
OSThread* __osRunQueue = (OSThread*)&__osThreadTail;
OSThread* __osActiveQueue = (OSThread*)&__osThreadTail;
OSThread* __osRunningThread = NULL;
OSThread* __osFaultedThread = NULL;

void __osDequeueThread(OSThread** queue, OSThread* t) {
    register OSThread* pred;
    register OSThread* succ;

    pred = (OSThread*)queue;
    succ = pred->next;
    while (succ != NULL) {
        if (succ == t) {
            pred->next = t->next;
            return;
        }
        pred = succ;
        succ = pred->next;
    }
}

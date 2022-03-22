#include "global.h"

void __osDequeueThread(OSThread** queue, OSThread* thread) {
    register OSThread** a2 = queue;
    register OSThread* a3 = *a2;

    while (a3 != NULL) {
        if (a3 == thread) {
            *a2 = thread->next;
            return;
        }
        a2 = &a3->next;
        a3 = *a2;
    }
}

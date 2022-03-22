#include "global.h"

void Sleep_Cycles(OSTime cycles) {
    OSMesgQueue mq;
    OSMesg msg;
    OSTimer timer;

    osCreateMesgQueue(&mq, &msg, OS_MESG_BLOCK);
    osSetTimer(&timer, cycles, 0, &mq, NULL);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);
}

void Sleep_Nsec(u32 nsec) {
    Sleep_Cycles(OS_NSEC_TO_CYCLES(nsec));
}

void Sleep_Usec(u32 usec) {
    Sleep_Cycles(OS_USEC_TO_CYCLES(usec));
}

// originally "msleep"
void Sleep_Msec(u32 ms) {
    Sleep_Cycles((ms * OS_CPU_COUNTER) / 1000ull);
}

void Sleep_Sec(u32 sec) {
    Sleep_Cycles(sec * OS_CPU_COUNTER);
}

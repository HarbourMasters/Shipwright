#ifndef PR_OS_TIME_H
#define PR_OS_TIME_H

#include "ultratypes.h"
#include "os_message.h"

#include <libultraship/libultra/time.h>

#if 0
typedef u64 OSTime;

typedef struct OSTimer_s {
    /* 0x00 */ struct OSTimer_s* next;
    /* 0x04 */ struct OSTimer_s* prev;
    /* 0x08 */ OSTime interval;
    /* 0x10 */ OSTime value;
    /* 0x18 */ OSMesgQueue* mq;
    /* 0x1C */ OSMesg msg;
} OSTimer; // size = 0x20

OSTime osGetTime(void);
void osSetTime(OSTime ticks);
int osSetTimer(OSTimer* t, OSTime countdown, OSTime interval, OSMesgQueue* mq, OSMesg msg);
int osStopTimer(OSTimer* t);

#endif
#endif
#ifndef PR_OSINT_H
#define PR_OSINT_H

#include <libultraship/libultra/interrupt.h>

#if 0

#include "ultratypes.h"
#include "os_message.h"
#include "os.h"
#include "os_internal.h"

typedef struct __OSEventState {
    /* 0x0 */ OSMesgQueue* messageQueue;
    /* 0x4 */ OSMesg message;
} __OSEventState; // size = 0x8

typedef struct __OSThreadTail {
    /* 0x0 */ OSThread* next;
    /* 0x4 */ OSPri priority;
} __OSThreadTail; // size = 0x8

void __osEnqueueAndYield(OSThread** param_1);
void __osDequeueThread(OSThread** queue, OSThread* t);
void __osEnqueueThread(OSThread** param_1, OSThread* param_2);
OSThread* __osPopThread(OSThread** param_1);
void __osDispatchThread(void);
void __osCleanupThread(void);

void __osSetTimerIntr(OSTime tim);
OSTime __osInsertTimer(OSTimer* t);
void __osTimerInterrupt(void);
u32 __osProbeTLB(void* param_1);
s32 __osSpDeviceBusy(void);

void __osTimerServicesInit(void);

extern __osHwInt __osHwIntTable[];
extern __OSEventState __osEventStateTab[OS_NUM_EVENTS];

extern __OSThreadTail __osThreadTail;
extern OSThread* __osRunQueue;
extern OSThread* __osActiveQueue;
extern OSThread* __osRunningThread;
extern OSThread* __osFaultedThread;

extern u32 __osShutdown;

extern OSTimer* __osTimerList;

extern OSTimer __osBaseTimer;
extern OSTime __osCurrentTime;
extern u32 __osBaseCounter;
extern u32 __osViIntrCount;
extern u32 __osTimerCounter;

#endif
#endif
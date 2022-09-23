#ifndef ULTRA64_MESSAGE_H
#define ULTRA64_MESSAGE_H

#include "thread.h"

#define OS_MESG_NOBLOCK         0
#define OS_MESG_BLOCK           1

typedef union {
    u8 data8;
    u16 data16;
    u32 data32;
    void* ptr;
} OSMesg;

#define OS_MESG_8(x) ((OSMesg) { .data8 = (x) })
#define OS_MESG_16(x) ((OSMesg) { .data16 = (x) })
#define OS_MESG_32(x) ((OSMesg) { .data32 = (x) })
#define OS_MESG_PTR(x) ((OSMesg) { .ptr = (x) })

#define osSendMesg8(queue, msg, flag) osSendMesg(queue, OS_MESG_8(msg), flag)
#define osSendMesg16(queue, msg, flag) osSendMesg(queue, OS_MESG_16(msg), flag)
#define osSendMesg32(queue, msg, flag) osSendMesg(queue, OS_MESG_32(msg), flag)
#define osSendMesgPtr(queue, msg, flag) osSendMesg(queue, OS_MESG_PTR(msg), flag)

typedef u32 OSEvent;

#define OS_NUM_EVENTS           15

#define OS_EVENT_SW1            0     /* CPU SW1 interrupt */
#define OS_EVENT_SW2            1     /* CPU SW2 interrupt */
#define OS_EVENT_CART           2     /* Cartridge interrupt: used by rmon */
#define OS_EVENT_COUNTER        3     /* Counter int: used by VI/Timer Mgr */
#define OS_EVENT_SP             4     /* SP task done interrupt */
#define OS_EVENT_SI             5     /* SI (controller) interrupt */
#define OS_EVENT_AI             6     /* AI interrupt */
#define OS_EVENT_VI             7     /* VI interrupt: used by VI/Timer Mgr */
#define OS_EVENT_PI             8     /* PI interrupt: used by PI Manager */
#define OS_EVENT_DP             9     /* DP full sync interrupt */
#define OS_EVENT_CPU_BREAK      10    /* CPU breakpoint: used by rmon */
#define OS_EVENT_SP_BREAK       11    /* SP breakpoint:  used by rmon */
#define OS_EVENT_FAULT          12    /* CPU fault event: used by rmon */
#define OS_EVENT_THREADSTATUS   13    /* CPU thread status: used by rmon */
#define OS_EVENT_PRENMI         14    /* Pre NMI interrupt */

typedef struct OSMesgQueue {
    /* 0x00 */ OSThread* mtqueue;
    /* 0x04 */ OSThread* fullqueue;
    /* 0x08 */ s32 validCount;
    /* 0x0C */ s32 first;
    /* 0x10 */ s32 msgCount;
    /* 0x14 */ OSMesg* msg;
} OSMesgQueue; // size = 0x18

#endif

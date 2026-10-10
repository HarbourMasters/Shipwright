#ifndef PR_OS_MESSAGE_H
#define PR_OS_MESSAGE_H

#include "os_thread.h"

#include <libultraship/libultra/message.h>

#if 0

typedef void* OSMesg;
typedef u32 OSEvent;

typedef struct OSMesgQueue {
    /* 0x00 */ OSThread* mtQueue;
    /* 0x04 */ OSThread* fullQueue;
    /* 0x08 */ s32 validCount;
    /* 0x0C */ s32 first;
    /* 0x10 */ s32 msgCount;
    /* 0x14 */ OSMesg* msg;
} OSMesgQueue; // size = 0x18

#define OS_NUM_EVENTS 15

#define OS_EVENT_SW1 0           /* CPU SW1 interrupt */
#define OS_EVENT_SW2 1           /* CPU SW2 interrupt */
#define OS_EVENT_CART 2          /* Cartridge interrupt: used by rmon */
#define OS_EVENT_COUNTER 3       /* Counter int: used by VI/Timer Mgr */
#define OS_EVENT_SP 4            /* SP task done interrupt */
#define OS_EVENT_SI 5            /* SI (controller) interrupt */
#define OS_EVENT_AI 6            /* AI interrupt */
#define OS_EVENT_VI 7            /* VI interrupt: used by VI/Timer Mgr */
#define OS_EVENT_PI 8            /* PI interrupt: used by PI Manager */
#define OS_EVENT_DP 9            /* DP full sync interrupt */
#define OS_EVENT_CPU_BREAK 10    /* CPU breakpoint: used by rmon */
#define OS_EVENT_SP_BREAK 11     /* SP breakpoint:  used by rmon */
#define OS_EVENT_FAULT 12        /* CPU fault event: used by rmon */
#define OS_EVENT_THREADSTATUS 13 /* CPU thread status: used by rmon */
#define OS_EVENT_PRENMI 14       /* Pre NMI interrupt */

#define OS_EVENT_RDB_READ_DONE 15   /* RDB read ok event: used by rmon */
#define OS_EVENT_RDB_LOG_DONE 16    /* read of log data complete */
#define OS_EVENT_RDB_DATA_DONE 17   /* read of host io data complete */
#define OS_EVENT_RDB_REQ_RAMROM 18  /* host needs ramrom access */
#define OS_EVENT_RDB_FREE_RAMROM 19 /* host is done with ramrom access */
#define OS_EVENT_RDB_DBG_DONE 20
#define OS_EVENT_RDB_FLUSH_PROF 21
#define OS_EVENT_RDB_ACK_PROF 22

#define OS_MESG_NOBLOCK 0
#define OS_MESG_BLOCK 1

#endif
#define MQ_GET_COUNT(mq) ((mq)->validCount)

#define MQ_IS_EMPTY(mq) (MQ_GET_COUNT(mq) == 0)
#define MQ_IS_FULL(mq) (MQ_GET_COUNT(mq) >= (mq)->msgCount)

void osCreateMesgQueue(OSMesgQueue* mq, OSMesg* msq, s32 count);
s32 osSendMesg(OSMesgQueue* mq, OSMesg msg, s32 flags);
s32 osJamMesg(OSMesgQueue* mq, OSMesg msg, s32 flag);
s32 osRecvMesg(OSMesgQueue* mq, OSMesg* msg, s32 flags);

void osSetEventMesg(OSEvent e, OSMesgQueue* mq, OSMesg m);

#endif
#ifndef IRQMGR_H
#define IRQMGR_H

#include <libultraship/libultra.h>

#define OS_SC_RETRACE_MSG       1
#define OS_SC_DONE_MSG          2
#define OS_SC_NMI_MSG           3 // name is made up, 3 is OS_SC_RDP_DONE_MSG in the original sched.c
#define OS_SC_PRE_NMI_MSG       4

typedef enum {
    IRQ_RESET_STATUS_IDLE,
    IRQ_RESET_STATUS_PRENMI,
    IRQ_RESET_STATUS_NMI
} IrqResetStatus;

typedef struct {
    /* 0x00 */ s16 type;
    /* 0x02 */ char misc[0x1E];
} OSScMsg; // size = 0x20

typedef struct IrqMgrClient {
    /* 0x00 */ struct IrqMgrClient* prev;
    /* 0x04 */ OSMesgQueue* queue;
} IrqMgrClient;

typedef struct {
    /* 0x000 */ OSScMsg retraceMsg;
    /* 0x020 */ OSScMsg prenmiMsg;
    /* 0x040 */ OSScMsg nmiMsg;
    /* 0x060 */ OSMesgQueue queue;
    /* 0x078 */ OSMesg msgBuf[8];
    /* 0x098 */ OSThread thread;
    /* 0x248 */ IrqMgrClient* clients;
    /* 0x24C */ u8 resetStatus;
    /* 0x250 */ OSTime resetTime;
    /* 0x258 */ OSTimer timer;
    /* 0x278 */ OSTime retraceTime;
} IrqMgr; // size = 0x280


void IrqMgr_Init(IrqMgr* irqMgr, void* stack, OSPri pri, u8 retraceCount);

void IrqMgr_AddClient(IrqMgr* irqMgr, IrqMgrClient* client, OSMesgQueue* msgQueue);
void IrqMgr_RemoveClient(IrqMgr* irqMgr, IrqMgrClient* client);

extern vu32 gIrqMgrResetStatus;
extern volatile OSTime gIrqMgrRetraceTime;

#endif
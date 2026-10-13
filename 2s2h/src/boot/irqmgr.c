#include "global.h"
#include "stackcheck.h"
#include "z64thread.h"

vs32 gIrqMgrResetStatus = 0;
volatile OSTime sIrqMgrResetTime = 0;
volatile OSTime gIrqMgrRetraceTime = 0;
s32 sIrqMgrRetraceCount = 0;

void IrqMgr_AddClient(IrqMgr* irqmgr, IrqMgrClient* client, OSMesgQueue* msgQueue) {
#if 0
    u32 saveMask;

    saveMask = osSetIntMask(1);

    client->queue = msgQueue;
    client->next = irqmgr->callbacks;
    irqmgr->callbacks = client;

    osSetIntMask(saveMask);

    if (irqmgr->prenmiStage > 0) {
        osSendMesg(client->queue, &irqmgr->prenmiMsg.type, OS_MESG_NOBLOCK);
    }
    if (irqmgr->prenmiStage > 1) {
        osSendMesg(client->queue, &irqmgr->nmiMsg.type, OS_MESG_NOBLOCK);
    }
#endif
}

void IrqMgr_RemoveClient(IrqMgr* irqmgr, IrqMgrClient* remove) {
#if 0
    IrqMgrClient* iter;
    IrqMgrClient* last;
    u32 saveMask;

    iter = irqmgr->callbacks;
    last = NULL;

    saveMask = osSetIntMask(1);

    while (iter != NULL) {
        if (iter == remove) {
            if (last != NULL) {
                last->next = remove->next;
            } else {
                irqmgr->callbacks = remove->next;
            }
            break;
        }
        last = iter;
        iter = iter->next;
    }

    osSetIntMask(saveMask);
#endif
}

void IrqMgr_SendMesgForClient(IrqMgr* irqmgr, OSMesg msg) {
#if 0
    IrqMgrClient* iter = irqmgr->callbacks;

    while (iter != NULL) {
        osSendMesg(iter->queue, msg, OS_MESG_NOBLOCK);
        iter = iter->next;
    }
#endif
}

void IrqMgr_JamMesgForClient(IrqMgr* irqmgr, OSMesg msg) {
#if 0
    IrqMgrClient* iter = irqmgr->callbacks;

    while (iter != NULL) {
        if (iter->queue->validCount < iter->queue->msgCount) {
            osSendMesg(iter->queue, msg, OS_MESG_NOBLOCK);
        }
        iter = iter->next;
    }
#endif
}

void IrqMgr_HandlePreNMI(IrqMgr* irqmgr) {
#if 0
    gIrqMgrResetStatus = 1;
    irqmgr->prenmiStage = 1;

    sIrqMgrResetTime = irqmgr->lastPrenmiTime = osGetTime();

    // Wait .45 seconds then generate a stage 2 prenmi interrupt
    osSetTimer(&irqmgr->prenmiTimer, OS_USEC_TO_CYCLES(450000), 0, &irqmgr->irqQueue, (OSMesg)0x29F);

    IrqMgr_JamMesgForClient(irqmgr, &irqmgr->prenmiMsg.type);
#endif
}

void IrqMgr_CheckStack(void) {
    // StackCheck_Check(NULL);
}

void IrqMgr_HandlePRENMI450(IrqMgr* irqmgr) {
#if 0
    gIrqMgrResetStatus = 2;
    irqmgr->prenmiStage = 2;

    // Wait .03 seconds then generate a stage 3 prenmi interrupt
    osSetTimer(&irqmgr->prenmiTimer, OS_USEC_TO_CYCLES(30000), 0, &irqmgr->irqQueue, (OSMesg)0x2A0);

    IrqMgr_SendMesgForClient(irqmgr, &irqmgr->nmiMsg.type);
#endif
}

void IrqMgr_HandlePRENMI480(IrqMgr* irqmgr) {
    // Wait .52 seconds. After this we will have waited an entire second
    // osSetTimer(&irqmgr->prenmiTimer, OS_USEC_TO_CYCLES(520000), 0, &irqmgr->irqQueue, (OSMesg)0x2A1);

    // osAfterPreNMI();
}

void IrqMgr_HandlePRENMI500(IrqMgr* irqmgr) {
    // IrqMgr_CheckStack();
}
void IrqMgr_HandleRetrace(IrqMgr* irqmgr) {
#if 0
    if (gIrqMgrRetraceTime == 0) {
        if (irqmgr->lastFrameTime == 0) {
            irqmgr->lastFrameTime = osGetTime();
        } else {
            gIrqMgrRetraceTime = osGetTime() - irqmgr->lastFrameTime;
        }
    }

    sIrqMgrRetraceCount += 1;
    IrqMgr_SendMesgForClient(irqmgr, irqmgr);
#endif
}

void IrqMgr_ThreadEntry(IrqMgr* irqmgr) {
#if 0
    u32 interrupt;
    u32 stop;

    interrupt = 0;
    stop = 0;
    while (stop == 0) {
        if (stop) {
            ;
        }

        osRecvMesg(&irqmgr->irqQueue, (OSMesg*)&interrupt, OS_MESG_BLOCK);
        switch (interrupt) {
            case 0x29A:
                IrqMgr_HandleRetrace(irqmgr);
                break;
            case 0x29D:
                IrqMgr_HandlePreNMI(irqmgr);
                break;
            case 0x29F:
                IrqMgr_HandlePRENMI450(irqmgr);
                break;
            case 0x2A0:
                IrqMgr_HandlePRENMI480(irqmgr);
                break;
            case 0x2A1:
                IrqMgr_HandlePRENMI500(irqmgr);
                break;
        }
    }
#endif
}

void IrqMgr_Init(IrqMgr* irqmgr, void* stack, OSPri pri, u8 retraceCount) {
#if 0
    irqmgr->callbacks = NULL;
    irqmgr->verticalRetraceMesg.type = 1;
    irqmgr->prenmiMsg.type = 4;
    irqmgr->nmiMsg.type = 3;
    irqmgr->prenmiStage = 0;
    irqmgr->lastPrenmiTime = 0;

    osCreateMesgQueue(&irqmgr->irqQueue, (OSMesg*)irqmgr->irqBuffer, ARRAY_COUNT(irqmgr->irqBuffer));
    osSetEventMesg(OS_EVENT_PRENMI, &irqmgr->irqQueue, (OSMesg)0x29D);
    osViSetEvent(&irqmgr->irqQueue, (OSMesg)0x29A, retraceCount);

    osCreateThread(&irqmgr->thread, Z_THREAD_ID_IRQMGR, IrqMgr_ThreadEntry, irqmgr, stack, pri);
    osStartThread(&irqmgr->thread);
#endif
}

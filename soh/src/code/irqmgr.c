#include "global.h"
#include "vt.h"

vu32 gIrqMgrResetStatus = 0;
volatile OSTime sIrqMgrResetTime = 0;
volatile OSTime gIrqMgrRetraceTime = 0;
u32 sIrqMgrRetraceCount = 0;

#define IRQ_RETRACE_MSG 666
#define IRQ_PRENMI_MSG 669
#define IRQ_PRENMI450_MSG 671
#define IRQ_PRENMI480_MSG 672
#define IRQ_PRENMI500_MSG 673

#define IRQ_RESET_STATUS_IDLE 0
#define IRQ_RESET_STATUS_PRENMI 1
#define IRQ_RESET_STATUS_NMI 2

void IrqMgr_AddClient(IrqMgr* irqMgr, IrqMgrClient* c, OSMesgQueue* msgQueue) {
    OSIntMask prevInt;

    LOG_CHECK_NULL_POINTER("this", irqMgr);
    LOG_CHECK_NULL_POINTER("c", c);
    LOG_CHECK_NULL_POINTER("msgQ", msgQueue);

    prevInt = osSetIntMask(1);

    c->queue = msgQueue;
    c->prev = irqMgr->clients;
    irqMgr->clients = c;

    osSetIntMask(prevInt);

    if (irqMgr->resetStatus > IRQ_RESET_STATUS_IDLE) {
        osSendMesgPtr(c->queue, &irqMgr->prenmiMsg, OS_MESG_NOBLOCK);
    }

    if (irqMgr->resetStatus >= IRQ_RESET_STATUS_NMI) {
        osSendMesgPtr(c->queue, &irqMgr->nmiMsg, OS_MESG_NOBLOCK);
    }
}

void IrqMgr_RemoveClient(IrqMgr* irqMgr, IrqMgrClient* c) {
    IrqMgrClient* iter = irqMgr->clients;
    IrqMgrClient* lastIter = NULL;
    OSIntMask prevInt;

    LOG_CHECK_NULL_POINTER("this", irqMgr);
    LOG_CHECK_NULL_POINTER("c", c);

    prevInt = osSetIntMask(1);

    while (iter != NULL) {
        if (iter == c) {
            if (lastIter) {
                lastIter->prev = c->prev;
            } else {
                irqMgr->clients = c->prev;
            }
            break;
        }
        lastIter = iter;
        iter = iter->prev;
    }

    osSetIntMask(prevInt);
}

void IrqMgr_SendMesgForClient(IrqMgr* irqMgr, OSMesg msg) {
    IrqMgrClient* iter = irqMgr->clients;

    while (iter != NULL) {
        if (iter->queue->validCount >= iter->queue->msgCount) {
            // "irqmgr_SendMesgForClient: Message queue is overflowing mq=%08x cnt=%d"
            osSyncPrintf(
                VT_COL(RED, WHITE) "irqmgr_SendMesgForClient:メッセージキューがあふれています mq=%08x cnt=%d\n" VT_RST,
                iter->queue, iter->queue->validCount);
        } else {
            osSendMesg(iter->queue, msg, OS_MESG_NOBLOCK);
        }

        iter = iter->prev;
    }
}

void IrqMgr_JamMesgForClient(IrqMgr* irqMgr, OSMesg msg) {
    IrqMgrClient* iter = irqMgr->clients;

    while (iter != NULL) {
        if (iter->queue->validCount >= iter->queue->msgCount) {
            // "irqmgr_JamMesgForClient: Message queue is overflowing mq=%08x cnt=%d"
            osSyncPrintf(
                VT_COL(RED, WHITE) "irqmgr_JamMesgForClient:メッセージキューがあふれています mq=%08x cnt=%d\n" VT_RST,
                iter->queue, iter->queue->validCount);
        } else {
            // mistake? the function's name suggests it would use osJamMesg
            osSendMesg(iter->queue, msg, OS_MESG_NOBLOCK);
        }
        iter = iter->prev;
    }
}

void IrqMgr_HandlePreNMI(IrqMgr* irqMgr) {
    u64 temp = IRQ_RESET_STATUS_PRENMI; // required to match

    gIrqMgrResetStatus = temp;
    irqMgr->resetStatus = IRQ_RESET_STATUS_PRENMI;
    sIrqMgrResetTime = irqMgr->resetTime = osGetTime();

    osSetTimer(&irqMgr->timer, OS_USEC_TO_CYCLES(450000), 0ull, &irqMgr->queue, OS_MESG_32(IRQ_PRENMI450_MSG));
    IrqMgr_JamMesgForClient(irqMgr, OS_MESG_PTR(&irqMgr->prenmiMsg));
}

void IrqMgr_CheckStack() {
    osSyncPrintf("irqmgr.c: PRENMIから0.5秒経過\n"); // "0.5 seconds after PRENMI"
    if (StackCheck_Check(NULL) == 0) {
        osSyncPrintf("スタックは大丈夫みたいです\n"); // "The stack looks ok"
    } else {
        osSyncPrintf("%c", BEL);
        osSyncPrintf(VT_FGCOL(RED));
        // "Stack overflow or dangerous"
        osSyncPrintf("スタックがオーバーフローしたか危険な状態です\n");
        // "Increase stack size early or don't consume stack"
        osSyncPrintf("早々にスタックサイズを増やすか、スタックを消費しないようにしてください\n");
        osSyncPrintf(VT_RST);
    }
}

void IrqMgr_HandlePRENMI450(IrqMgr* irqMgr) {
    u64 temp = IRQ_RESET_STATUS_NMI; // required to match
    gIrqMgrResetStatus = temp;
    irqMgr->resetStatus = IRQ_RESET_STATUS_NMI;

    osSetTimer(&irqMgr->timer, OS_USEC_TO_CYCLES(30000), 0ull, &irqMgr->queue, OS_MESG_32(IRQ_PRENMI480_MSG));
    IrqMgr_SendMesgForClient(irqMgr, OS_MESG_PTR(&irqMgr->nmiMsg));
}

void IrqMgr_HandlePRENMI480(IrqMgr* irqMgr) {
    u32 ret;

    osSetTimer(&irqMgr->timer, OS_USEC_TO_CYCLES(20000), 0ull, &irqMgr->queue, OS_MESG_32(IRQ_PRENMI500_MSG));
    ret = osAfterPreNMI();
    if (ret) {
        // "osAfterPreNMI returned %d !?"
        osSyncPrintf("osAfterPreNMIが %d を返しました！？\n", ret);
        osSetTimer(&irqMgr->timer, OS_USEC_TO_CYCLES(1000), 0ull, &irqMgr->queue, OS_MESG_32(IRQ_PRENMI480_MSG));
    }
}

void IrqMgr_HandlePRENMI500(IrqMgr* irqMgr) {
    IrqMgr_CheckStack();
}

void IrqMgr_HandleRetrace(IrqMgr* irqMgr) {
    if (gIrqMgrRetraceTime == 0ull) {
        if (irqMgr->retraceTime == 0) {
            irqMgr->retraceTime = osGetTime();
        } else {
            gIrqMgrRetraceTime = osGetTime() - irqMgr->retraceTime;
        }
    }
    sIrqMgrRetraceCount++;
    IrqMgr_SendMesgForClient(irqMgr, OS_MESG_PTR(&irqMgr->retraceMsg));
}

void IrqMgr_ThreadEntry(void* arg0) {
    OSMesg msg;
    IrqMgr* irqMgr = (IrqMgr*)arg0;
    u8 exit;

    msg.data32 = 0;
    osSyncPrintf("ＩＲＱマネージャスレッド実行開始\n"); // "Start IRQ manager thread execution"
    exit = false;

    while (!exit) {
        osRecvMesg(&irqMgr->queue, &msg, OS_MESG_BLOCK);
        switch (msg.data32) {
            case IRQ_RETRACE_MSG:
                IrqMgr_HandleRetrace(irqMgr);
                break;
            case IRQ_PRENMI_MSG:
                osSyncPrintf("IRQ_PRENMI_MSG\n");
                // "Scheduler: Receives PRE_NMI message"
                osSyncPrintf("スケジューラ：PRE_NMIメッセージを受信\n");
                IrqMgr_HandlePreNMI(irqMgr);
                break;
            case IRQ_PRENMI450_MSG:
                osSyncPrintf("IRQ_PRENMI450_MSG\n");
                // "Scheduler: Receives PRENMI450 message"
                osSyncPrintf("スケジューラ：PRENMI450メッセージを受信\n");
                IrqMgr_HandlePRENMI450(irqMgr);
                break;
            case IRQ_PRENMI480_MSG:
                osSyncPrintf("IRQ_PRENMI480_MSG\n");
                // "Scheduler: Receives PRENMI480 message"
                osSyncPrintf("スケジューラ：PRENMI480メッセージを受信\n");
                IrqMgr_HandlePRENMI480(irqMgr);
                break;
            case IRQ_PRENMI500_MSG:
                osSyncPrintf("IRQ_PRENMI500_MSG\n");
                // "Scheduler: Receives PRENMI500 message"
                osSyncPrintf("スケジューラ：PRENMI500メッセージを受信\n");
                exit = true;
                IrqMgr_HandlePRENMI500(irqMgr);
                break;
            default:
                // "Unexpected message received"
                osSyncPrintf("irqmgr.c:予期しないメッセージを受け取りました(%08x)\n", msg);
                break;
        }
    }

    osSyncPrintf("ＩＲＱマネージャスレッド実行終了\n"); // "End of IRQ manager thread execution"
}

void IrqMgr_Init(IrqMgr* irqMgr, void* stack, OSPri pri, u8 retraceCount) {
    LOG_CHECK_NULL_POINTER("this", irqMgr);
    LOG_CHECK_NULL_POINTER("stack", stack);

    irqMgr->clients = NULL;
    irqMgr->retraceMsg.type = OS_SC_RETRACE_MSG;
    irqMgr->prenmiMsg.type = OS_SC_PRE_NMI_MSG;
    irqMgr->nmiMsg.type = OS_SC_NMI_MSG;
    irqMgr->resetStatus = IRQ_RESET_STATUS_IDLE;
    irqMgr->resetTime = 0;

    osCreateMesgQueue(&irqMgr->queue, irqMgr->msgBuf, ARRAY_COUNT(irqMgr->msgBuf));
    osSetEventMesg(OS_EVENT_PRENMI, &irqMgr->queue, OS_MESG_32(IRQ_PRENMI_MSG));
    osViSetEvent(&irqMgr->queue, OS_MESG_32(IRQ_RETRACE_MSG), retraceCount);
    osCreateThread(&irqMgr->thread, 0x13, IrqMgr_ThreadEntry, irqMgr, stack, pri);
    osStartThread(&irqMgr->thread);
}

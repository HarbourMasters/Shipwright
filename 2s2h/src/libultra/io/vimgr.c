#include "ultra64.h"
#include "PR/osint.h"
#include "stack.h"
#include "PR/osint.h"
#include "libc/stdbool.h"
#include "macros.h"
#include "alignment.h"

OSThread viThread;
STACK(sViStack, OS_VIM_STACKSIZE) ALIGNED(16);
OSMesgQueue viEventQueue ALIGNED(8);
OSMesg viEventBuf[6] ALIGNED(8);
OSIoMesg viRetraceMsg ALIGNED(8);
OSIoMesg viCounterMsg ALIGNED(8);
OSDevMgr __osViDevMgr = { 0 };
u32 __additional_scanline = 0;

void viMgrMain(void*);

void osCreateViManager(OSPri pri) {
    u32 prevInt;
    OSPri newPri;
    OSPri currentPri;

    if (!__osViDevMgr.active) {
        __osTimerServicesInit();
        __additional_scanline = 0;
        osCreateMesgQueue(&viEventQueue, viEventBuf, ARRAY_COUNT(viEventBuf) - 1);
        viRetraceMsg.hdr.type = OS_MESG_TYPE_VRETRACE;
        viRetraceMsg.hdr.pri = OS_MESG_PRI_NORMAL;
        viRetraceMsg.hdr.retQueue = NULL;
        viCounterMsg.hdr.type = OS_MESG_TYPE_COUNTER;
        viCounterMsg.hdr.pri = OS_MESG_PRI_NORMAL;
        viCounterMsg.hdr.retQueue = NULL;
        osSetEventMesg(OS_EVENT_VI, &viEventQueue, &viRetraceMsg);
        osSetEventMesg(OS_EVENT_COUNTER, &viEventQueue, &viCounterMsg);
        newPri = -1;
        currentPri = osGetThreadPri(NULL);
        if (currentPri < pri) {
            newPri = currentPri;
            osSetThreadPri(NULL, pri);
        }

        prevInt = __osDisableInt();
        __osViDevMgr.active = true;
        __osViDevMgr.thread = &viThread;
        __osViDevMgr.cmdQueue = &viEventQueue;
        __osViDevMgr.evtQueue = &viEventQueue;
        __osViDevMgr.acsQueue = NULL;
        __osViDevMgr.piDmaCallback = NULL;
        __osViDevMgr.epiDmaCallback = NULL;

        osCreateThread(&viThread, 0, &viMgrMain, &__osViDevMgr, STACK_TOP(sViStack), pri);
        __osViInit();
        osStartThread(&viThread);
        __osRestoreInt(prevInt);
        if (newPri != -1) {
            osSetThreadPri(NULL, newPri);
        }
    }
}

void viMgrMain(void* arg) {
    OSDevMgr* dmArgs;
    static u16 viRetrace;
    u32 addTime;
    OSIoMesg* mesg;
    u32 temp = 0; // always 0

    mesg = NULL;
    viRetrace = __osViGetCurrentContext()->retraceCount;
    if (viRetrace == 0) {
        viRetrace = 1;
    }

    dmArgs = (OSDevMgr*)arg;

    while (true) {
        osRecvMesg(dmArgs->evtQueue, (OSMesg*)&mesg, OS_MESG_BLOCK);
        switch (mesg->hdr.type) {
            case OS_MESG_TYPE_VRETRACE:
                __osViSwapContext();
                viRetrace--;
                if (!viRetrace) {
                    __OSViContext* ctx = __osViGetCurrentContext();
                    if (ctx->mq) {
                        osSendMesg(ctx->mq, ctx->msg, OS_MESG_NOBLOCK);
                    }
                    viRetrace = ctx->retraceCount;
                }

                __osViIntrCount++;

                // block optimized out since temp is always 0,
                // but it changes register allocation and ordering for __osCurrentTime
                if (temp != 0) {
                    addTime = osGetCount();
                    __osCurrentTime = addTime;
                    temp = 0;
                }

                addTime = __osBaseCounter;
                __osBaseCounter = osGetCount();
                addTime = __osBaseCounter - addTime;
                __osCurrentTime = __osCurrentTime + addTime;
                break;
            case OS_MESG_TYPE_COUNTER:
                __osTimerInterrupt();
                break;
        }
    }
}

#include "ultra64.h"
#include "alignment.h"
#include "stack.h"
#include "macros.h"

OSPiHandle __Dom1SpeedParam ALIGNED(8);
OSPiHandle __Dom2SpeedParam ALIGNED(8);
OSThread sPiMgrThread;
STACK(sPiMgrStack, 0x1000);
OSMesgQueue piEventQueue ALIGNED(8);
OSMesg piEventBuf[1];

OSDevMgr __osPiDevMgr = { 0 };
OSPiHandle* __osPiTable = NULL;
OSPiHandle* __osCurrentHandle[2] ALIGNED(8) = { &__Dom1SpeedParam, &__Dom2SpeedParam };

void osCreatePiManager(OSPri pri, OSMesgQueue* cmdQ, OSMesg* cmdBuf, s32 cmdMsgCnt) {
    u32 savedMask;
    OSPri oldPri;
    OSPri myPri;

    if (!__osPiDevMgr.active) {
        osCreateMesgQueue(cmdQ, cmdBuf, cmdMsgCnt);
        osCreateMesgQueue(&piEventQueue, piEventBuf, ARRAY_COUNT(piEventBuf));
        if (!__osPiAccessQueueEnabled) {
            __osPiCreateAccessQueue();
        }
        osSetEventMesg(OS_EVENT_PI, &piEventQueue, (OSMesg)0x22222222);
        oldPri = -1;
        myPri = osGetThreadPri(NULL);
        if (myPri < pri) {
            oldPri = myPri;
            osSetThreadPri(NULL, pri);
        }
        savedMask = __osDisableInt();
        __osPiDevMgr.active = 1;
        __osPiDevMgr.thread = &sPiMgrThread;
        __osPiDevMgr.cmdQueue = cmdQ;
        __osPiDevMgr.evtQueue = &piEventQueue;
        __osPiDevMgr.acsQueue = &__osPiAccessQueue;
        __osPiDevMgr.piDmaCallback = __osPiRawStartDma;
        __osPiDevMgr.epiDmaCallback = __osEPiRawStartDma;
        osCreateThread(&sPiMgrThread, 0, __osDevMgrMain, &__osPiDevMgr, STACK_TOP(sPiMgrStack), pri);
        osStartThread(&sPiMgrThread);
        __osRestoreInt(savedMask);
        if (oldPri != -1) {
            osSetThreadPri(NULL, oldPri);
        }
    }
}

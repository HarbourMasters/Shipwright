#include "global.h"
#include "ultra64/internal.h"

OSMgrArgs __osPiDevMgr = { 0 };

OSPiHandle __Dom1SpeedParam;
OSPiHandle __Dom2SpeedParam;
OSThread piThread;
u8 piStackThread[0x1000];
OSMesgQueue piEventQueue;
OSMesg piEventBuf[2];
OSThread __osThreadSave;

OSPiHandle* __osPiTable = NULL;
OSPiHandle* __osCurrentHandle[] = {
    &__Dom1SpeedParam,
    &__Dom2SpeedParam,
};

void osCreatePiManager(OSPri pri, OSMesgQueue* cmdQ, OSMesg* cmdBuf, s32 cmdMsgCnt) {
    u32 prevInt;
    OSPri newPri;
    OSPri currentPri;

    if (!__osPiDevMgr.initialized) {
        osCreateMesgQueue(cmdQ, cmdBuf, cmdMsgCnt);
        osCreateMesgQueue(&piEventQueue, piEventBuf, 1);
        if (!__osPiAccessQueueEnabled) {
            __osPiCreateAccessQueue();
        }

        osSetEventMesg(OS_EVENT_PI, &piEventQueue, (OSMesg)0x22222222);
        newPri = -1;
        currentPri = osGetThreadPri(NULL);
        if (currentPri < pri) {
            newPri = currentPri;
            osSetThreadPri(NULL, pri);
        }
        prevInt = __osDisableInt();

        __osPiDevMgr.initialized = true;
        __osPiDevMgr.cmdQueue = cmdQ;
        __osPiDevMgr.mgrThread = &piThread;
        __osPiDevMgr.eventQueue = &piEventQueue;
        __osPiDevMgr.acccessQueue = &__osPiAccessQueue;
        __osPiDevMgr.piDmaCallback = __osPiRawStartDma;
        __osPiDevMgr.epiDmaCallback = __osEPiRawStartDma;

        osCreateThread(&piThread, 0, __osDevMgrMain, (void*)&__osPiDevMgr, piStackThread + sizeof(piStackThread), pri);
        osStartThread(&piThread);

        __osRestoreInt(prevInt);

        if (newPri != -1) {
            osSetThreadPri(NULL, newPri);
        }
    }
}

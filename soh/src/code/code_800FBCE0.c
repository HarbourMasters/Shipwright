#include "global.h"

#define printSpStatus(x, name) \
    if (x & SP_STATUS_##name)  \
    osSyncPrintf(#name " ")
#define printDpStatus(x, name) \
    if (x & DPC_STATUS_##name) \
    osSyncPrintf(#name " ")

void func_800FBCE0() {
    u32 spStatus = __osSpGetStatus();
    u32 dpStatus = osDpGetStatus();

    osSyncPrintf("osSpGetStatus=%08x: ", spStatus);
    printSpStatus(spStatus, HALT);
    printSpStatus(spStatus, BROKE);
    printSpStatus(spStatus, DMA_BUSY);
    printSpStatus(spStatus, DMA_FULL);
    printSpStatus(spStatus, IO_FULL);
    printSpStatus(spStatus, SSTEP);
    printSpStatus(spStatus, INTR_BREAK);
    printSpStatus(spStatus, YIELD);
    printSpStatus(spStatus, YIELDED);
    printSpStatus(spStatus, TASKDONE);
    printSpStatus(spStatus, SIG3);
    printSpStatus(spStatus, SIG4);
    printSpStatus(spStatus, SIG5);
    printSpStatus(spStatus, SIG6);
    printSpStatus(spStatus, SIG7);
    osSyncPrintf("\n");

    osSyncPrintf("osDpGetStatus=%08x:", dpStatus);
    printDpStatus(dpStatus, XBUS_DMEM_DMA);
    printDpStatus(dpStatus, FREEZE);
    printDpStatus(dpStatus, FLUSH);
    printDpStatus(dpStatus, START_GCLK);
    printDpStatus(dpStatus, TMEM_BUSY);
    printDpStatus(dpStatus, PIPE_BUSY);
    printDpStatus(dpStatus, CMD_BUSY);
    printDpStatus(dpStatus, CBUF_READY);
    printDpStatus(dpStatus, DMA_BUSY);
    printDpStatus(dpStatus, END_VALID);
    printDpStatus(dpStatus, START_VALID);
    osSyncPrintf("\n");
}

void func_800FBFD8() {
    func_800FBCE0();
    osDpSetStatus(DPC_SET_FREEZE | DPC_SET_FLUSH);
    __osSpSetStatus(SP_SET_HALT | SP_SET_SIG2 | SP_CLR_INTR_BREAK);
    func_800FBCE0();
}

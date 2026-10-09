#include "ultra64.h"

void RcpUtils_PrintRegisterStatus(void) {
    // u32 spStatus = __osSpGetStatus();
    // u32 dpStatus = osDpGetStatus();
    //
    // if (spStatus) {
    //     // stubbed debug prints
    // }
    //
    // if (dpStatus) {
    //     // stubbed debug prints
    // }
}

void RcpUtils_Reset(void) {
    // RcpUtils_PrintRegisterStatus();
    //// Flush the RDP pipeline and freeze clock counter
    // osDpSetStatus(DPC_SET_FREEZE | DPC_SET_FLUSH);
    //// Halt the RSP, disable interrupt on break and set "task done" signal
    //__osSpSetStatus(SP_SET_HALT | SP_SET_TASKDONE | SP_CLR_INTR_BREAK);
    // RcpUtils_PrintRegisterStatus();
}

#include "ultra64.h"

#define _osVirtualToPhysical(ptr)              \
    if (ptr != NULL) {                         \
        ptr = (void*)osVirtualToPhysical(ptr); \
    }                                          \
    (void)0

static OSTask sTmpTask;

OSTask* _VirtualToPhysicalTask(OSTask* intp) {
    OSTask* tp = &sTmpTask;

    bcopy(intp, tp, sizeof(OSTask));

    _osVirtualToPhysical(tp->t.ucode);
    _osVirtualToPhysical(tp->t.ucodeData);
    _osVirtualToPhysical(tp->t.dramStack);
    _osVirtualToPhysical(tp->t.outputBuff);
    _osVirtualToPhysical(tp->t.outputBuffSize);
    _osVirtualToPhysical(tp->t.dataPtr);
    _osVirtualToPhysical(tp->t.yieldDataPtr);

    return tp;
}

void osSpTaskLoad(OSTask* intp) {
    OSTask* tp = _VirtualToPhysicalTask(intp);

    if (tp->t.flags & OS_TASK_YIELDED) {
        tp->t.ucodeData = tp->t.yieldDataPtr;
        tp->t.ucodeDataSize = tp->t.yieldDataSize;
        intp->t.flags &= ~OS_TASK_YIELDED;

        if (tp->t.flags & OS_TASK_LOADABLE) {
            tp->t.ucode = (void*)IO_READ((uintptr_t)intp->t.yieldDataPtr + OS_YIELD_DATA_SIZE - 4);
        }
    }

    osWritebackDCache(tp, sizeof(OSTask));
    __osSpSetStatus(SP_CLR_SIG0 | SP_CLR_SIG1 | SP_CLR_SIG2 | SP_SET_INTR_BREAK);

    while (__osSpSetPc(SP_IMEM_START) == -1) {}

    while (__osSpRawStartDma(1, (SP_IMEM_START - sizeof(*tp)), tp, sizeof(OSTask)) == -1) {}

    while (__osSpDeviceBusy()) {}

    while (__osSpRawStartDma(1, SP_IMEM_START, tp->t.ucodeBoot, tp->t.ucodeBootSize) == -1) {}
}

void osSpTaskStartGo(OSTask* tp) {
    while (__osSpDeviceBusy()) {}

    __osSpSetStatus(SP_SET_INTR_BREAK | SP_CLR_SSTEP | SP_CLR_BROKE | SP_CLR_HALT);
}

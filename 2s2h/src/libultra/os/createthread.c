#include "ultra64.h"

void osCreateThread(OSThread* thread, OSId id, void* entry, void* arg, void* sp, OSPri p) {
    register u32 saveMask;
    OSIntMask mask;

    thread->id = id;
    thread->priority = p;
    thread->next = NULL;
    thread->queue = NULL;
    thread->context.pc = (u32)entry;
    thread->context.a0 = (intptr_t)arg;
    thread->context.sp = (u64)(s32)sp - 16;
    thread->context.ra = (intptr_t)__osCleanupThread;

    mask = OS_IM_ALL;
    thread->context.sr = (mask & (SR_IMASK | SR_IE)) | SR_EXL;
    thread->context.rcp = (mask & RCP_IMASK) >> RCP_IMASKSHIFT;
    thread->context.fpcsr = (u32)(FPCSR_FS | FPCSR_EV);
    thread->fp = 0;
    thread->state = OS_STATE_STOPPED;
    thread->flags = 0;

    saveMask = __osDisableInt();
    thread->tlnext = __osActiveQueue;
    __osActiveQueue = thread;
    __osRestoreInt(saveMask);
}

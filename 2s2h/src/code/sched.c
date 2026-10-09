#include "fault.h"
#include "idle.h"
#include "z64.h"
#include "BenPort.h"

// Variables are put before most headers as a hacky way to bypass bss reordering
FaultClient sSchedFaultClient;

OSTime sRSPGFXStartTime;
OSTime sRSPAudioStartTime;
OSTime sRSPOtherStartTime;
OSTime sRDPStartTime;

u64* gAudioSPDataPtr;
u32 gAudioSPDataSize;

#include "functions.h"
#include "variables.h"
#include "stackcheck.h"
#include "z64speed_meter.h"
#include "z64thread.h"
#include <string.h>

#define RSP_DONE_MSG 667
#define RDP_DONE_MSG 668
#define ENTRY_MSG 670
#define RDP_AUDIO_CANCEL_MSG 671
#define RSP_GFX_CANCEL_MSG 672

void Sched_SwapFramebuffer(CfbInfo* cfbInfo) {
    if (cfbInfo->swapBuffer != NULL) {
        osViSwapBuffer(cfbInfo->swapBuffer);
        cfbInfo->updateRate2 = cfbInfo->updateRate;

        if ((SREG(62) == 0) && (cfbInfo->viMode != NULL)) {
            D_80096B20 = 1;
            osViSetMode(cfbInfo->viMode);
            osViSetSpecialFeatures(cfbInfo->features);
            osViSetXScale(cfbInfo->xScale);
            osViSetYScale(cfbInfo->yScale);
            cfbInfo->viMode = NULL;
        }
    }
    cfbInfo->unk_10 = 0;
}

void Sched_RetraceUpdateFramebuffer(SchedContext* sched, CfbInfo* cfbInfo) {
    if (sched->shouldUpdateVi) {
        sched->shouldUpdateVi = false;

        if (gIrqMgrResetStatus == 0) {
            ViConfig_UpdateVi(false);
        }
    }
    Sched_SwapFramebuffer(cfbInfo);
}

void Sched_HandleReset(SchedContext* sched) {
}

void Sched_HandleStop(SchedContext* sched) {
    ViConfig_UpdateVi(true);
}

/**
 * Attempt to stop the RSP, if it is not already halted, by setting the halt bit in the
 * SP status register and waiting. Regardless of the result, the scheduler will send an
 * RSP_DONE_MSG back to itself.
 * If there was no currently running audio task, it will dequeue the currently waiting
 * audio task and notify the sender if the task is associated with a message queue.
 */
void Sched_HandleAudioCancel(SchedContext* sched) {
    s32 i;

    // AUDIO SP Cancel
    osSyncPrintf("AUDIO SP キャンセルします\n");

    if ((sched->curRSPTask != NULL) && (sched->curRSPTask->list.t.type == M_AUDTASK)) {
        if (!(IO_READ(SP_STATUS_REG) & SP_STATUS_HALT)) {
            // Attempts to stop AUDIO SP
            osSyncPrintf("AUDIO SP止めようとします\n");

            IO_WRITE(SP_STATUS_REG, SP_SET_HALT);

            i = 0;
            while (!(IO_READ(SP_STATUS_REG) & SP_STATUS_HALT)) {
                if (i++ > 100) {
                    // AUDIO SP did not stop (10ms timeout)
                    osSyncPrintf("AUDIO SP止まりませんでした(10msタイムアウト)\n");
                    goto send_mesg;
                }
                Sleep_Usec(100);
            }
            // AUDIO SP stopped (% d * 100us)
            osSyncPrintf("AUDIO SP止まりました(%d * 100us)\n", i);
        } else {
            // AUDIO SP seems to be stopped
            osSyncPrintf("AUDIO SP止まっているようです\n");
        }

    send_mesg:
        osSendMesg(&sched->interruptQ, OS_MESG_32(RSP_DONE_MSG), OS_MESG_NOBLOCK);
        return;
    }

    if (sched->audioListHead != NULL) {
        OSScTask* cur = sched->audioListHead;
        OSScTask* next = cur->next;

        sched->audioListHead = next;
        if (next == NULL) {
            sched->audioListTail = NULL;
        }
        if (cur->msgQ != NULL) {
            osSendMesg(cur->msgQ, cur->msg, OS_MESG_BLOCK);
        }
        // Removed AUDIO SP task from pending list
        osSyncPrintf("AUDIO SP タスクを実行待ちリストから削除しました\n");
        return;
    }

    // There are no AUDIO SP tasks to cancel
    osSyncPrintf("キャンセルすべき AUDIO SP タスクがありません\n");
}

/**
 * Attempt to stop the RSP, if it is not already halted, by setting the halt bit in the
 * SP status register and waiting. Regardless of the result, the scheduler will send an
 * RSP_DONE_MSG back to itself and attempt to stop the RDP.
 * If there was no currently running gfx task, it will dequeue the currently waiting gfx
 * task and notify the sender if the task is associated with a message queue.
 * If there is an RDP task, the output buffer will be cleared and the scheduler will send
 * an RDP_DONE_MSG back to itself.
 */
void Sched_HandleGfxCancel(SchedContext* sched) {
#if 0
    s32 i;

    // GRAPH SP Cancel
    osSyncPrintf("GRAPH SP キャンセルします\n");

    if ((sched->curRSPTask != NULL) && (sched->curRSPTask->list.t.type == M_GFXTASK)) {
        if (!(IO_READ(SP_STATUS_REG) & SP_STATUS_HALT)) {
            // GRAPH SP tries to stop
            osSyncPrintf("GRAPH SP止めようとします\n");

            IO_WRITE(SP_STATUS_REG, SP_SET_HALT);

            i = 0;
            while (!(IO_READ(SP_STATUS_REG) & SP_STATUS_HALT)) {
                if (i++ > 100) {
                    // GRAPH SP did not stop (10ms timeout)
                    osSyncPrintf("GRAPH SP止まりませんでした(10msタイムアウト)\n");
                    goto send_mesg;
                }
                Sleep_Usec(100);
            }
            // GRAPH SP stopped (%d * 100us)
            osSyncPrintf("GRAPH SP止まりました(%d * 100us)\n", i);
        } else {
            // GRAPH SP seems to be stopped
            osSyncPrintf("GRAPH SP止まっているようです\n");
        }

    send_mesg:
        osSendMesg(&sched->interruptQ, OS_MESG_PTR(RSP_DONE_MSG), OS_MESG_NOBLOCK);
        goto halt_rdp;
    }

    if (sched->gfxListHead != NULL) {
        OSScTask* cur = sched->gfxListHead;
        OSScTask* next = cur->next;

        sched->gfxListHead = next;
        if (next == NULL) {
            sched->gfxListTail = NULL;
        }
        if (cur->msgQ != NULL) {
            osSendMesg(cur->msgQ, cur->msg, OS_MESG_BLOCK);
        }
        goto halt_rdp;
    }

    // There are no GRAPH SP tasks to cancel
    osSyncPrintf("キャンセルすべき GRAPH SP タスクがありません\n");

halt_rdp:
    if (sched->curRDPTask != NULL) {
        OSTask_t* dpTask = &sched->curRDPTask->list.t;

        if (dpTask->type == M_GFXTASK) {
            // Try to stop DP
            osSyncPrintf("DP止めようとします\n");
            bzero(dpTask->outputBuff, (uintptr_t)dpTask->outputBuffSize - (uintptr_t)dpTask->outputBuff);
            osSendMesg(&sched->interruptQ, (OSMesg)RDP_DONE_MSG, OS_MESG_NOBLOCK);
        }
    }
#endif
}

/**
 * Adds a scheduler task to the appropriate linked list.
 */
void Sched_QueueTask(SchedContext* sched, OSScTask* task) {
    s32 type = task->list.t.type;

    if (type == M_AUDTASK) {
        if (sched->audioListTail != NULL) {
            sched->audioListTail->next = task;
        } else {
            sched->audioListHead = task;
        }
        sched->audioListTail = task;
    } else {
        if (sched->gfxListTail != NULL) {
            sched->gfxListTail->next = task;
        } else {
            sched->gfxListHead = task;
        }
        sched->gfxListTail = task;
    }
    task->next = NULL;
    task->state = task->flags & OS_SC_RCP_MASK;
}

void Sched_Yield(SchedContext* sched) {
    // Don't yield audio tasks
    if (sched->curRSPTask->list.t.type == M_AUDTASK) {
        // A new audio task has been entered even though the previous audio task has not been completed yet
        osSyncPrintf("まだ前回のオーディオタスクが完了していないのに新たなオーディオタスクがエントリされた\n");
    } else if (!(sched->curRSPTask->state & OS_SC_YIELD)) {
        sched->curRSPTask->state |= OS_SC_YIELD;
        osSpTaskYield();
    }
}

s32 Sched_TaskCheckFramebuffers(SchedContext* sched, OSScTask* task) {
    void* nextFB = osViGetNextFramebuffer();
    void* curFB = osViGetCurrentFramebuffer();

    if ((task == NULL) || (sched->pendingSwapBuf1 != NULL) ||
        ((curFB == TASK_FRAMEBUFFER(task)->fb1) && (curFB != nextFB))) {
        return 0;
    }
    return 1;
}

/**
 * Pops the next task from the appropriate linked list and returns it through spTask and dpTask.
 */
s32 Sched_Schedule(SchedContext* sched, OSScTask** spTask, OSScTask** dpTask, s32 state) {
    s32 ret = state;
    OSScTask* gfxTask = sched->gfxListHead;
    OSScTask* audioTask = sched->audioListHead;

    if ((ret & OS_SC_SP) && (sched->audioListHead != NULL)) {
        *spTask = audioTask;
        ret &= ~OS_SC_SP;
        sched->audioListHead = sched->audioListHead->next;
        if (sched->audioListHead == NULL) {
            sched->audioListTail = NULL;
        }
    } else if (gfxTask != NULL) {
        if ((gfxTask->state & OS_SC_YIELDED) || !(gfxTask->flags & OS_SC_NEEDS_RDP)) {
            if (ret & OS_SC_SP) {
                *spTask = gfxTask;
                ret &= ~OS_SC_SP;
                sched->gfxListHead = sched->gfxListHead->next;
                if (sched->gfxListHead == NULL) {
                    sched->gfxListTail = NULL;
                }
            }
        } else if (ret == (OS_SC_SP | OS_SC_DP)) {
            if ((TASK_FRAMEBUFFER(gfxTask) == NULL) || Sched_TaskCheckFramebuffers(sched, gfxTask)) {
                *spTask = *dpTask = gfxTask;
                ret &= ~(OS_SC_SP | OS_SC_DP);
                sched->gfxListHead = sched->gfxListHead->next;
                if (sched->gfxListHead == NULL) {
                    sched->gfxListTail = NULL;
                }
            }
        }
    }
    return ret;
}

void Sched_TaskUpdateFramebuffer(SchedContext* sched, OSScTask* task) {
    sched->pendingSwapBuf1 = TASK_FRAMEBUFFER(task);

    if ((sched->curBuf != NULL) && (sched->curBuf->updateRate2 > 0)) {
        return;
    }
    Sched_RetraceUpdateFramebuffer(sched, sched->pendingSwapBuf1);
}

/**
 * If the task has been marked as completed, notify the sender through the task's
 * associated message queue (if it has one) that the task has been completed. If the task
 * flags dictate it should swap the framebuffer, do so.
 */
void Sched_NotifyDone(SchedContext* sched, OSScTask* task) {
    if (!(task->state & (OS_SC_DP | OS_SC_SP))) {
        if (task->msgQ != NULL) {
            osSendMesg(task->msgQ, task->msg, OS_MESG_BLOCK);
        }
        if (task->flags & OS_SC_SWAPBUFFER) {
            Sched_TaskUpdateFramebuffer(sched, task);
        }
    }
}

void Sched_RunTask(SchedContext* sched, OSScTask* spTask, OSScTask* dpTask) {
    u64 time;

    if (spTask != NULL) {
        if (spTask->list.t.type == M_NULTASK) {
            if (spTask->flags & OS_SC_NEEDS_RSP) {
                spTask->state &= ~OS_SC_SP;
                sched->curRSPTask = NULL;
            }
            if (spTask->flags & OS_SC_NEEDS_RDP) {
                spTask->state &= ~OS_SC_DP;
                sched->curRDPTask = NULL;
            }
            Sched_NotifyDone(sched, spTask);
            return;
        }
        // Write back the data cache to ensure imminent SP DMA does not miss anything
        if ((spTask->list.t.type != M_AUDTASK) && !(spTask->state & OS_SC_YIELDED)) {
            osWritebackDCacheAll();
        }
        spTask->state &= ~(OS_SC_YIELD | OS_SC_YIELDED);
        // Have the RSP download the task and prepare the RSP program counter
        osSpTaskLoad(&spTask->list);

        // Log the start time based on the type of task
        time = osGetTime();
        switch (spTask->list.t.type) {
            case M_AUDTASK:
                sRSPAudioStartTime = time;
                break;

            case M_GFXTASK:
                sRSPGFXStartTime = time;
                break;

            default:
                if (1) {}
                sRSPOtherStartTime = time;
                break;
        }

        if (spTask->list.t.type == M_AUDTASK) {
            // Set global pointers to audio task data for use in audio processing
            gAudioSPDataPtr = spTask->list.t.data_ptr;
            gAudioSPDataSize = spTask->list.t.data_size;
        }

        // Begin task execution
        osSpTaskStartGo(&spTask->list);
        sched->curRSPTask = spTask;
        if (spTask == dpTask && sched->curRDPTask == NULL) {
            sched->curRDPTask = dpTask;
            sRDPStartTime = sRSPGFXStartTime;
        }
    }
}

/**
 * Enqueues any tasks that have been sent to the scheduler down the command queue.
 */
void Sched_HandleEntry(SchedContext* sched) {
#if 0
    OSScTask* spTask = NULL;
    OSScTask* dpTask = NULL;
    OSMesg msg = NULL;
    s32 state;

    // Fetch and enqueue waiting tasks
    while (osRecvMesg(&sched->cmdQ, &msg, OS_MESG_NOBLOCK) != -1) {
        Sched_QueueTask(sched, msg);
    }
    // If there is an audio task pending and an RSP task is running, yield the current task.
    if ((sched->audioListHead != NULL) && (sched->curRSPTask != NULL)) {
        Sched_Yield(sched);
        return;
    }
    // Schedule and run the next task
    state = ((sched->curRSPTask == NULL) << 1) | (sched->curRDPTask == NULL);
    if (Sched_Schedule(sched, &spTask, &dpTask, state) != state) {
        Sched_RunTask(sched, spTask, dpTask);
    }
#endif
}

void Sched_HandleRetrace(SchedContext* sched) {
    ViConfig_UpdateBlack();
    sched->retraceCount++;

    if (osViGetCurrentFramebuffer() ==
        (void*)((sched->pendingSwapBuf1 != NULL) ? sched->pendingSwapBuf1->swapBuffer : NULL)) {
        if (sched->curBuf != NULL) {
            sched->curBuf->unk_10 = 0;
        }
        if (sched->pendingSwapBuf1 != NULL) {
            sched->pendingSwapBuf1->unk_10 = 0;
        }
        sched->curBuf = sched->pendingSwapBuf1;
        sched->pendingSwapBuf1 = NULL;
    }
    if (sched->curBuf != NULL) {
        if (sched->curBuf->updateRate2 > 0) {
            sched->curBuf->updateRate2--;
        }
        if ((sched->curBuf->updateRate2 <= 0) && (sched->pendingSwapBuf1 != NULL)) {
            Sched_RetraceUpdateFramebuffer(sched, sched->pendingSwapBuf1);
        }
    }
    Sched_HandleEntry(sched);
}

void Sched_HandleRSPDone(SchedContext* sched) {
    OSScTask* curRSP;
    OSScTask* nextRSP = NULL;
    OSScTask* nextRDP = NULL;
    s32 state;
    u64 time;

    if (sched->curRSPTask == NULL) {
        osSyncPrintf("__scHandleRSP:sc->curRSPTask == NULL\n");
        return;
    }

    // Log the time based on the type of task
    time = osGetTime();
    switch (sched->curRSPTask->list.t.type) {
        case M_AUDTASK:
            gRSPAudioTimeAcc += time - sRSPAudioStartTime;
            break;

        case M_GFXTASK:
            gRSPGfxTimeAcc += time - sRSPGFXStartTime;
            break;

        default:
            if (1) {}
            gRSPOtherTimeAcc += time - sRSPOtherStartTime;
            break;
    }

    curRSP = sched->curRSPTask;
    sched->curRSPTask = NULL;

    if (curRSP->list.t.type == M_AUDTASK) {
        // Reset the global audio task data pointers
        gAudioSPDataPtr = NULL;
        gAudioSPDataSize = 0;
    }

    if ((curRSP->state & OS_SC_YIELD) && osSpTaskYielded(&curRSP->list)) {
        // If the task was yielded, re-queue the task
        curRSP->state |= OS_SC_YIELDED;
        curRSP->next = sched->gfxListHead;
        sched->gfxListHead = curRSP;
        if (sched->gfxListTail == NULL) {
            sched->gfxListTail = curRSP;
        }
    } else {
        // Mark task completed
        curRSP->state &= ~OS_SC_SP;
        Sched_NotifyDone(sched, curRSP);
    }

    // Schedule and run next task
    state = ((sched->curRSPTask == NULL) << 1) | (sched->curRDPTask == NULL);
    if (Sched_Schedule(sched, &nextRSP, &nextRDP, state) != state) {
        Sched_RunTask(sched, nextRSP, nextRDP);
    }
}

void Sched_HandleRDPDone(SchedContext* sched) {
    OSScTask* curRDP;
    OSScTask* nextRSP = NULL;
    OSScTask* nextRDP = NULL;
    s32 state;

    if (sched->curRDPTask == NULL) {
        osSyncPrintf("__scHandleRDP:sc->curRDPTask == NULL\n");
        return;
    }

    // Log run time
    gRDPTimeAcc = osGetTime() - sRDPStartTime;

    // Mark task done
    curRDP = sched->curRDPTask;
    sched->curRDPTask = NULL;
    curRDP->state &= ~OS_SC_DP;

    Sched_NotifyDone(sched, curRDP);

    // Schedule and run next task
    state = ((sched->curRSPTask == NULL) << 1) | (sched->curRDPTask == NULL);
    if (Sched_Schedule(sched, &nextRSP, &nextRDP, state) != state) {
        Sched_RunTask(sched, nextRSP, nextRDP);
    }
}

/**
 * Sends a message to the scheduler to inform it that a new task has
 * been sent down the command queue.
 */
void Sched_SendEntryMsg(SchedContext* sched) {
    osSendMesg(&sched->interruptQ, OS_MESG_32(ENTRY_MSG), OS_MESG_BLOCK);
}

/**
 * Sends a message to the scheduler to inform it that it should attempt
 * to stop the last dispatched audio task.
 */
void Sched_SendAudioCancelMsg(SchedContext* sched) {
    osSendMesg(&sched->interruptQ, OS_MESG_32(RDP_AUDIO_CANCEL_MSG), OS_MESG_BLOCK);
}

/**
 * Sends a message to the scheduler to inform it that it should attempt
 * to stop the last dispatched gfx task.
 */
void Sched_SendGfxCancelMsg(SchedContext* sched) {
    osSendMesg(&sched->interruptQ, OS_MESG_32(RSP_GFX_CANCEL_MSG), OS_MESG_BLOCK);
}

/**
 * Fault Client for the scheduler. Reports information about the state of the scheduler
 * and any current tasks in the crash debugger.
 */
void Sched_FaultClient(void* param1, void* param2) {
    SchedContext* sched = (SchedContext*)param1;
    OSScTask* spTask;
    OSScTask* dpTask;

    FaultDrawer_Printf("sched info\n", sched->gfxListHead, sched->gfxListTail, sched->audioListHead,
                       sched->audioListTail);
    FaultDrawer_Printf("GRAPH %08x %08x\n", sched->gfxListHead, sched->gfxListTail);
    FaultDrawer_Printf("AUDIO %08x %08x\n\n", sched->audioListHead, sched->audioListTail);

    spTask = sched->curRSPTask;
    if (spTask != NULL) {
        FaultDrawer_Printf("RSPTask %08x %08x %02x %02x\n%01x %08x %08x\n", spTask, spTask->next, spTask->state,
                           spTask->flags, spTask->list.t.type, spTask->list.t.data_ptr, spTask->list.t.data_size);
    }

    dpTask = sched->curRDPTask;
    if (dpTask != NULL) {
        FaultDrawer_Printf("RDPTask %08x %08x %02x %02x\n", dpTask, dpTask->next, dpTask->state, dpTask->flags);
    }
}

/**
 * The main loop of the scheduler thread. Processes interrupt messages from
 * the IrqMgr received through its IrqClient and messages sent to it from other
 * threads or the OS.
 */
void Sched_ThreadEntry(void* arg) {
    OSMesg msg = OS_MESG_PTR(NULL);
    ;
    SchedContext* sched = (SchedContext*)arg;

    while (true) {
        osRecvMesg(&sched->interruptQ, &msg, OS_MESG_BLOCK);

        // Check if it's a message from another thread or the OS
        switch ((s32)msg.data32) {
            case RDP_AUDIO_CANCEL_MSG:
                Sched_HandleAudioCancel(sched);
                continue;

            case RSP_GFX_CANCEL_MSG:
                Sched_HandleGfxCancel(sched);
                continue;

            case ENTRY_MSG:
                Sched_HandleEntry(sched);
                continue;

            case RSP_DONE_MSG:
                Sched_HandleRSPDone(sched);
                continue;

            case RDP_DONE_MSG:
                Sched_HandleRDPDone(sched);
                continue;
        }
        // Check if it's a message from the IrqMgr
        switch (((OSScMsg*)msg.data32)->type) {
            case OS_SC_RETRACE_MSG:
                Sched_HandleRetrace(sched);
                continue;

            case OS_SC_PRE_NMI_MSG:
                Sched_HandleReset(sched);
                continue;

            case OS_SC_NMI_MSG:
                Sched_HandleStop(sched);
                continue;
        }
    }
}

/**
 * Initializes the SchedContext and scheduler thread.
 * Registers an IrqClient for the thread and fault client for the SchedContext.
 * Directs the OS to send SP and DP OS messages to interruptQ when the RSP or RDP signal task completion.
 */
void Sched_Init(SchedContext* sched, void* stack, OSPri pri, u8 viModeType, UNK_TYPE arg4, IrqMgr* irqMgr) {
    memset(sched, 0, sizeof(SchedContext));

    sched->shouldUpdateVi = true;

    osCreateMesgQueue(&sched->interruptQ, sched->intBuf, ARRAY_COUNT(sched->intBuf));
    osCreateMesgQueue(&sched->cmdQ, sched->cmdMsgBuf, ARRAY_COUNT(sched->cmdMsgBuf));
    osSetEventMesg(OS_EVENT_SP, &sched->interruptQ, OS_MESG_32(RSP_DONE_MSG));
    osSetEventMesg(OS_EVENT_DP, &sched->interruptQ, OS_MESG_32(RDP_DONE_MSG));
    IrqMgr_AddClient(irqMgr, &sched->irqClient, &sched->interruptQ);
    Fault_AddClient(&sSchedFaultClient, Sched_FaultClient, sched, NULL);
    osCreateThread(&sched->thread, Z_THREAD_ID_SCHED, Sched_ThreadEntry, sched, stack, pri);
    osStartThread(&sched->thread);
}

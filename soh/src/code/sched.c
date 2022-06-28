#include "global.h"

#include <string.h>

#define RSP_DONE_MSG 667
#define RDP_DONE_MSG 668
#define ENTRY_MSG 670

// data
vs32 sLogScheduler = false;

// bss
OSTime sRSPGFXStartTime;
OSTime sRSPAudioStartTime;
OSTime sRSPOtherStartTime;
OSTime sRDPStartTime;

void Sched_SwapFrameBuffer(CfbInfo* cfbInfo) {
    u16 width;

    LOG_CHECK_VALID_POINTER("cfbinfo->swapbuffer", cfbInfo->swapBuffer);
    if (cfbInfo->swapBuffer != NULL) {
        osViSwapBuffer(cfbInfo->swapBuffer);
        cfbInfo->updateRate2 = cfbInfo->updateRate;

        if (sLogScheduler) {
            osSyncPrintf("osViSwapBuffer %08x %08x %08x\n", osViGetCurrentFramebuffer(), osViGetNextFramebuffer(),
                         (cfbInfo != NULL ? cfbInfo->swapBuffer : NULL));
        }
        width = cfbInfo->viMode != NULL ? cfbInfo->viMode->comRegs.width : (u32)gScreenWidth;
        Fault_SetFB(cfbInfo->swapBuffer, width, 0x10);

        if (HREG(80) == 0xD && HREG(95) != 0xD) {
            HREG(81) = 0;
            HREG(82) = 0;
            HREG(83) = 1;
            HREG(84) = 0;
            HREG(85) = 1;
            HREG(86) = 0;
            HREG(87) = 0;
            HREG(88) = 0;
            HREG(89) = 0;
            HREG(90) = 0;
            HREG(91) = 0;
            HREG(92) = 0;
            HREG(93) = 0;
            HREG(94) = 0;
            HREG(95) = 0xD;
        }
        if (HREG(80) == 0xD && HREG(81) == 2) {
            osViSetSpecialFeatures(HREG(82) != 0 ? OS_VI_GAMMA_ON : OS_VI_GAMMA_OFF);
            osViSetSpecialFeatures(HREG(83) != 0 ? OS_VI_DITHER_FILTER_ON : OS_VI_DITHER_FILTER_OFF);
            osViSetSpecialFeatures(HREG(84) != 0 ? OS_VI_GAMMA_DITHER_ON : OS_VI_GAMMA_DITHER_OFF);
            osViSetSpecialFeatures(HREG(85) != 0 ? OS_VI_DIVOT_ON : OS_VI_DIVOT_OFF);
        }
    }
    cfbInfo->unk_10 = 0;
}

void func_800C84E4(SchedContext* sc, CfbInfo* cfbInfo) {
    if (sc->unk_24C != 0) {
        sc->unk_24C = 0;

        if (gIrqMgrResetStatus == 0) {
            ViConfig_UpdateVi(0);
        }
    }

    Sched_SwapFrameBuffer(cfbInfo);
}

void Sched_HandleReset(SchedContext* sc) {
    OSTime now;

    if (sc->curRSPTask != NULL) {
        now = osGetTime();

        if (sc->curRSPTask->framebuffer == NULL) {
            LOG_TIME("(((u64)(now - audio_rsp_start_time)*(1000000LL/15625LL))/((62500000LL*3/4)/15625LL))",
                     OS_CYCLES_TO_USEC(now - sRSPAudioStartTime));
        } else if (OS_CYCLES_TO_USEC(now - sRSPGFXStartTime) > 1000000 ||
                   OS_CYCLES_TO_USEC(now - sRDPStartTime) > 1000000) {
            func_800FBFD8();
            if (sc->curRSPTask != NULL) {
                LOG_TIME("(((u64)(now - graph_rsp_start_time)*(1000000LL/15625LL))/((62500000LL*3/4)/15625LL))",
                         OS_CYCLES_TO_USEC(now - sRSPGFXStartTime));
                osSendMesg32(&sc->interruptQ, RSP_DONE_MSG, OS_MESG_NOBLOCK);
            }
            if (sc->curRDPTask != NULL) {
                LOG_TIME("(((u64)(now - rdp_start_time)*(1000000LL/15625LL))/((62500000LL*3/4)/15625LL))",
                         OS_CYCLES_TO_USEC(now - sRDPStartTime));
                osSendMesg32(&sc->interruptQ, RDP_DONE_MSG, OS_MESG_NOBLOCK);
            }
        }
    }
}

void Sched_HandleStart(SchedContext* sc) {
    ViConfig_UpdateVi(1);
}

void Sched_QueueTask(SchedContext* sc, OSScTask* task) {
    s32 type = task->list.t.type;

    ASSERT((type == M_AUDTASK) || (type == M_GFXTASK) || (type == M_NJPEGTASK) || (type == M_NULTASK));

    if (type == M_AUDTASK) {
        if (sLogScheduler) {
            // "You have entered an audio task"
            osSyncPrintf("オーディオタスクをエントリしました\n");
        }
        if (sc->audioListTail != NULL) {
            sc->audioListTail->next = task;
        } else {
            sc->audioListHead = task;
        }
        sc->audioListTail = task;
        sc->doAudio = 1;
    } else {
        if (sLogScheduler) {
            osSyncPrintf("グラフタスクをエントリしました\n"); // "Entered graph task"
        }

        if (sc->gfxListTail != NULL) {
            sc->gfxListTail->next = task;
        } else {
            sc->gfxListHead = task;
        }
        sc->gfxListTail = task;
    }
    task->next = NULL;
    task->state = task->flags & (OS_SC_NEEDS_RDP | OS_SC_NEEDS_RSP);
}

void Sched_Yield(SchedContext* sc) {
    if (!(sc->curRSPTask->state & OS_SC_YIELD)) {
        ASSERT(sc->curRSPTask->list.t.type != M_AUDTASK);

        sc->curRSPTask->state |= OS_SC_YIELD;

        osSpTaskYield();

        if (sLogScheduler) {
            osSyncPrintf("%08d:osSpTaskYield\n", (u32)(OS_CYCLES_TO_USEC(osGetTime())));
        }
    }
}

OSScTask* func_800C89D4(SchedContext* sc, OSScTask* task) {
    if (task == NULL) {
        return NULL;
    }

    if (sc->pendingSwapBuf1 != NULL) {
        if (0) {
            ASSERT(sc->pendingSwapBuf1 != NULL);
        }
        return NULL;
    }

    if (sc->pendingSwapBuf2 != NULL) {
        if (0) {
            ASSERT(sc->pendingSwapBuf2 != NULL);
        }
        return NULL;
    }

    if ((sc->pendingSwapBuf2 != NULL ? sc->pendingSwapBuf2->swapBuffer : NULL) == task->framebuffer->fb1) {
        return NULL;
    }

    if ((sc->pendingSwapBuf1 != NULL ? sc->pendingSwapBuf1->swapBuffer : NULL) == task->framebuffer->fb1) {
        return NULL;
    }

    if (osViGetCurrentFramebuffer() == (u32*)task->framebuffer->fb1) {
        return NULL;
    }

    return task;
}

s32 Sched_Schedule(SchedContext* sc, OSScTask** sp, OSScTask** dp, s32 state) {
    s32 ret = state;
    OSScTask* gfxTask = sc->gfxListHead;
    OSScTask* audioTask = sc->audioListHead;

    if (sc->doAudio && (ret & OS_SC_SP)) {
        *sp = audioTask;
        ret &= ~OS_SC_SP;
        sc->doAudio = 0;
        sc->audioListHead = sc->audioListHead->next;
        if (sc->audioListHead == NULL) {
            sc->audioListTail = NULL;
        }
    } else if (gfxTask != NULL) {
        if (gfxTask->state & OS_SC_YIELDED || !(gfxTask->flags & OS_SC_NEEDS_RDP)) {
            if (ret & OS_SC_SP) {
                *sp = gfxTask;
                ret &= ~OS_SC_SP;
                sc->gfxListHead = sc->gfxListHead->next;
                if (sc->gfxListHead == NULL) {
                    sc->gfxListTail = NULL;
                }
            }
        } else if (ret == (OS_SC_SP | OS_SC_DP)) {
            if (gfxTask->framebuffer == NULL || func_800C89D4(sc, gfxTask) != NULL) {
                *sp = *dp = gfxTask;
                ret &= ~(OS_SC_SP | OS_SC_DP);
                sc->gfxListHead = sc->gfxListHead->next;
                if (sc->gfxListHead == NULL) {
                    sc->gfxListTail = NULL;
                }
            }
        }
    }
    return ret;
}

void func_800C8BC4(SchedContext* sc, OSScTask* task) {
    if (sc->pendingSwapBuf1 == NULL) {
        sc->pendingSwapBuf1 = task->framebuffer;

        LOG_CHECK_VALID_POINTER("sc->pending_swapbuffer1", sc->pendingSwapBuf1);

        if ((sc->curBuf == NULL) || (sc->curBuf->updateRate2 < 1)) {
            func_800C84E4(sc, task->framebuffer);
        }
    }
}

u32 Sched_IsComplete(SchedContext* sc, OSScTask* task) {
    if (!(task->state & (OS_SC_DP | OS_SC_SP))) {
        if (task->msgQ != NULL) {
            osSendMesg(task->msgQ, task->msg, OS_MESG_BLOCK);
        }

        if (task->flags & OS_SC_SWAPBUFFER) {
            func_800C8BC4(sc, task);
        }

        return 1;
    }

    return 0;
}

void Sched_RunTask(SchedContext* sc, OSScTask* spTask, OSScTask* dpTask) {
    ASSERT(sc->curRSPTask == NULL);
    if (spTask != NULL) {
        if (spTask->list.t.type == M_NULTASK) {
            if (spTask->flags & OS_SC_NEEDS_RSP) {
                spTask->state &= ~OS_SC_SP;
                sc->curRSPTask = NULL;
            }
            if (spTask->flags & OS_SC_NEEDS_RDP) {
                spTask->state &= ~OS_SC_DP;
                sc->curRDPTask = NULL;
            }
            Sched_IsComplete(sc, spTask);
            return;
        }

        spTask->state &= ~(OS_SC_YIELD | OS_SC_YIELDED);
        osWritebackDCacheAll();
        osSpTaskLoad(&spTask->list);

        if (spTask->list.t.type == M_AUDTASK) {
            sRSPAudioStartTime = osGetTime();
        } else if (spTask->list.t.type == M_GFXTASK) {
            sRSPGFXStartTime = osGetTime();
        } else {
            sRSPOtherStartTime = osGetTime();
        }

        osSpTaskStartGo(&spTask->list);
        if (sLogScheduler) {
            osSyncPrintf(
                "%08d:osSpTaskStartGo(%08x) %s\n", (u32)OS_CYCLES_TO_USEC(osGetTime()), &spTask->list,
                (spTask->list.t.type == M_AUDTASK ? "AUDIO" : (spTask->list.t.type == M_GFXTASK ? "GRAPH" : "OTHER")));
        }
        sc->curRSPTask = spTask;

        if (spTask == dpTask && sc->curRDPTask == NULL) {
            sc->curRDPTask = dpTask;
            sRDPStartTime = sRSPGFXStartTime;
        }
    }
}

void Sched_HandleEntry(SchedContext* sc) {
    OSScTask* nextRSP = NULL;
    OSScTask* nextRDP = NULL;
    s32 state;
    OSMesg msg = OS_MESG_PTR(NULL);

    while (osRecvMesg(&sc->cmdQ, &msg, OS_MESG_NOBLOCK) != -1) {
        Sched_QueueTask(sc, msg.ptr);
    }

    if (sc->doAudio != 0 && sc->curRSPTask != NULL) {
        if (sLogScheduler) {
            osSyncPrintf("[YIELD B]");
        }
        Sched_Yield(sc);
        return;
    }

    state = ((sc->curRSPTask == 0) * 2) | (sc->curRDPTask == 0);
    if (Sched_Schedule(sc, &nextRSP, &nextRDP, state) != state) {
        Sched_RunTask(sc, nextRSP, nextRDP);
    }
    if (sLogScheduler) {
        osSyncPrintf("EN sc:%08x sp:%08x dp:%08x state:%x\n", sc, nextRSP, nextRDP, state);
    }
}

void Sched_HandleRetrace(SchedContext* sc) {
    if (sLogScheduler) {
        osSyncPrintf("%08d:scHandleRetrace %08x\n", (u32)OS_CYCLES_TO_USEC(osGetTime()), osViGetCurrentFramebuffer());
    }
    ViConfig_UpdateBlack();
    sc->retraceCnt++;

    if (osViGetCurrentFramebuffer() == (u32*)(sc->pendingSwapBuf1 != NULL ? sc->pendingSwapBuf1->swapBuffer : NULL)) {
        if (sc->curBuf != NULL) {
            sc->curBuf->unk_10 = 0;
        }

        if (sc->pendingSwapBuf1 != NULL) {
            sc->pendingSwapBuf1->unk_10 = 0;
        }
        sc->curBuf = sc->pendingSwapBuf1;
        sc->pendingSwapBuf1 = sc->pendingSwapBuf2;
        sc->pendingSwapBuf2 = NULL;
    }
    if (sc->curBuf != NULL) {
        if (sc->curBuf->updateRate2 > 0) {
            sc->curBuf->updateRate2--;
        }
        if ((sc->curBuf->updateRate2 <= 0) && (sc->pendingSwapBuf1 != NULL)) {
            func_800C84E4(sc, sc->pendingSwapBuf1);
        }
    }
    if (sLogScheduler) {
        osSyncPrintf("%08x %08x %08x %d\n", osViGetCurrentFramebuffer(), osViGetNextFramebuffer(),
                     sc->pendingSwapBuf1 != NULL ? sc->pendingSwapBuf1->swapBuffer : NULL,
                     sc->curBuf != NULL ? sc->curBuf->updateRate2 : 0);
    }
    Sched_HandleEntry(sc);
}

void Sched_HandleRSPDone(SchedContext* sc) {
    OSScTask* curRSPTask;
    OSScTask* nextRSP = NULL;
    OSScTask* nextRDP = NULL;
    s32 state;

    ASSERT(sc->curRSPTask != NULL);

    if (sc->curRSPTask->list.t.type == M_AUDTASK) {
        gRSPAudioTotalTime += osGetTime() - sRSPAudioStartTime;
    } else if (sc->curRSPTask->list.t.type == M_GFXTASK) {
        gRSPGFXTotalTime += osGetTime() - sRSPGFXStartTime;
    } else {
        gRSPOtherTotalTime += osGetTime() - sRSPOtherStartTime;
    }

    curRSPTask = sc->curRSPTask;
    sc->curRSPTask = NULL;
    if (sLogScheduler) {
        osSyncPrintf("RSP DONE %d %d", curRSPTask->state & 0x10, osSpTaskYielded(&curRSPTask->list));
    }
    if (curRSPTask->state & OS_SC_YIELD && osSpTaskYielded(&curRSPTask->list)) {
        if (sLogScheduler) {
            osSyncPrintf("[YIELDED]\n");
        }
        curRSPTask->state |= OS_SC_YIELDED;
        curRSPTask->next = sc->gfxListHead;
        sc->gfxListHead = curRSPTask;
        if (sc->gfxListTail == NULL) {
            sc->gfxListTail = curRSPTask;
        }
    } else {
        if (sLogScheduler) {
            osSyncPrintf("[NOT YIELDED]\n");
        }
        curRSPTask->state &= ~OS_SC_SP;
        Sched_IsComplete(sc, curRSPTask);
    }

    state = ((sc->curRSPTask == NULL) << 1) | (sc->curRDPTask == NULL);
    if (Sched_Schedule(sc, &nextRSP, &nextRDP, state) != state) {
        Sched_RunTask(sc, nextRSP, nextRDP);
    }
    if (sLogScheduler) {
        osSyncPrintf("SP sc:%08x sp:%08x dp:%08x state:%x\n", sc, nextRSP, nextRDP, state);
    }
}

void Sched_HandleRDPDone(SchedContext* sc) {
    OSScTask* curTask;
    OSScTask* nextRSP = NULL;
    OSScTask* nextRDP = NULL;
    s32 state;

    gRDPTotalTime = osGetTime() - sRDPStartTime;
    ASSERT(sc->curRDPTask != NULL);
    ASSERT(sc->curRDPTask->list.t.type == M_GFXTASK);
    curTask = sc->curRDPTask;
    sc->curRDPTask = NULL;
    curTask->state &= ~OS_SC_DP;
    Sched_IsComplete(sc, curTask);
    state = ((sc->curRSPTask == NULL) << 1) | (sc->curRDPTask == NULL);
    if (Sched_Schedule(sc, &nextRSP, &nextRDP, state) != state) {
        Sched_RunTask(sc, nextRSP, nextRDP);
    }
    if (sLogScheduler) {
        osSyncPrintf("DP sc:%08x sp:%08x dp:%08x state:%x\n", sc, nextRSP, nextRDP, state);
    }
}

void Sched_SendEntryMsg(SchedContext* sc) {
    if (sLogScheduler) {
        osSyncPrintf("osScKickEntryMsg\n");
    }

    osSendMesg32(&sc->interruptQ, ENTRY_MSG, OS_MESG_BLOCK);
}

void Sched_ThreadEntry(void* arg) {
    OSMesg msg;
    SchedContext* sc = (SchedContext*)arg;

    msg.ptr = NULL;

    while (true) {
        if (sLogScheduler) {
            // "%08d: standby"
            osSyncPrintf("%08d:待機中\n", (u32)OS_CYCLES_TO_USEC(osGetTime()));
        }

        osRecvMesg(&sc->interruptQ, &msg, OS_MESG_BLOCK);

        switch (msg.data32) {
            case ENTRY_MSG:
                if (sLogScheduler) {
                    osSyncPrintf("%08d:ENTRY_MSG\n", (u32)OS_CYCLES_TO_USEC(osGetTime()));
                }
                Sched_HandleEntry(sc);
                continue;
            case RSP_DONE_MSG:
                if (sLogScheduler) {
                    osSyncPrintf("%08d:RSP_DONE_MSG\n", (u32)OS_CYCLES_TO_USEC(osGetTime()));
                }
                Sched_HandleRSPDone(sc);
                continue;
            case RDP_DONE_MSG:
                if (sLogScheduler) {
                    osSyncPrintf("%08d:RDP_DONE_MSG\n", (u32)OS_CYCLES_TO_USEC(osGetTime()));
                }
                Sched_HandleRDPDone(sc);
                continue;
            default:
                break;
        }
        switch (((OSScMsg*)msg.ptr)->type) {
            case 1:
                Sched_HandleRetrace(sc);
                continue;
            case 4:
                Sched_HandleReset(sc);
                continue;
            case 3:
                Sched_HandleStart(sc);
                continue;
        }
    }
}

void Sched_Init(SchedContext* sc, void* stack, OSPri priority, UNK_TYPE arg3, UNK_TYPE arg4, IrqMgr* irqMgr) {
    memset(sc,0, sizeof(SchedContext));
    sc->unk_24C = 1;
    osCreateMesgQueue(&sc->interruptQ, sc->intBuf, 8);
    osCreateMesgQueue(&sc->cmdQ, sc->cmdMsgBuf, 8);
    osSetEventMesg(OS_EVENT_SP, &sc->interruptQ, OS_MESG_32(RSP_DONE_MSG));
    osSetEventMesg(OS_EVENT_DP, &sc->interruptQ, OS_MESG_32(RDP_DONE_MSG));
    IrqMgr_AddClient(irqMgr, &sc->irqClient, &sc->interruptQ);
    osCreateThread(&sc->thread, 5, Sched_ThreadEntry, sc, stack, priority);
    osStartThread(&sc->thread);
}

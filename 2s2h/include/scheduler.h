#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "PR/ultratypes.h"
#include "PR/os_sched.h"
#include "PR/os_vi.h"
#include "PR/sptask.h"
#include "irqmgr.h"

#define TASK_FRAMEBUFFER(task) ((CfbInfo*)(task)->framebuffer)

typedef struct {
    /* 0x00 */ u16*      fb1;
    /* 0x04 */ u16*      swapBuffer;
    /* 0x08 */ OSViMode* viMode;
    /* 0x0C */ u32       features;
    /* 0x10 */ u8        unk_10;
    /* 0x11 */ s8        updateRate;
    /* 0x12 */ s8        updateRate2;
    /* 0x13 */ u8        unk_13;
    /* 0x14 */ f32       xScale;
    /* 0x18 */ f32       yScale;
} CfbInfo; // size = 0x1C

typedef struct {
    /* 0x000 */ OSMesgQueue  interruptQ;
    /* 0x018 */ OSMesg       intBuf[64];
    /* 0x118 */ OSMesgQueue  cmdQ;
    /* 0x130 */ OSMesg       cmdMsgBuf[8];
    /* 0x150 */ OSThread     thread;
    /* 0x300 */ OSScTask*    audioListHead;
    /* 0x304 */ OSScTask*    gfxListHead;
    /* 0x308 */ OSScTask*    audioListTail;
    /* 0x30C */ OSScTask*    gfxListTail;
    /* 0x310 */ OSScTask*    curRSPTask;
    /* 0x314 */ OSScTask*    curRDPTask;
    /* 0x318 */ s32          retraceCount;
    /* 0x318 */ s32          doAudio;
    /* 0x320 */ CfbInfo*     curBuf;
    /* 0x324 */ CfbInfo*     pendingSwapBuf1;
    /* 0x328 */ CfbInfo*     pendingSwapBuf2;
    /* 0x32C */ char         unk_32C[0x3];
    /* 0x32F */ u8           shouldUpdateVi;
    /* 0x330 */ IrqMgrClient irqClient;
} SchedContext; // size = 0x338

#endif

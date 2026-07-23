#ifndef SCHED_H
#define SCHED_H

#include <libultraship/libultra.h>
#include "irqmgr.h"
#include "unk.h"

#include "extern_c_helper.h"

#define OS_SC_NEEDS_RDP         0x0001
#define OS_SC_NEEDS_RSP         0x0002
#define OS_SC_DRAM_DLIST        0x0004
#define OS_SC_PARALLEL_TASK     0x0010
#define OS_SC_LAST_TASK         0x0020
#define OS_SC_SWAPBUFFER        0x0040

#define OS_SC_DP                OS_SC_NEEDS_RDP
#define OS_SC_SP                OS_SC_NEEDS_RSP
#define OS_SC_YIELD             0x0010
#define OS_SC_YIELDED           0x0020

#define OS_SC_RCP_MASK          (OS_SC_NEEDS_RDP | OS_SC_NEEDS_RSP)
#define OS_SC_TYPE_MASK         (OS_SC_NEEDS_RDP | OS_SC_NEEDS_RSP | OS_SC_DRAM_DLIST)

typedef struct CfbInfo {
    /* 0x00 */ u16* fb1;
    /* 0x04 */ u16* swapBuffer;
    /* 0x08 */ OSViMode* viMode;
    /* 0x0C */ u32 features;
    /* 0x10 */ u8 unk_10;
    /* 0x11 */ s8 updateRate;
    /* 0x12 */ s8 updateRate2;
    /* 0x13 */ u8 unk_13;
#if true//OOT_VERSION >= PAL_1_0
    /* 0x14 */ f32 xScale;
    /* 0x18 */ f32 yScale;
#endif
} CfbInfo; // size = 0x1C

typedef struct OSScTask {
    /* 0x00 */ struct OSScTask* next;
    /* 0x04 */ u32 state;
    /* 0x08 */ u32 flags;
    /* 0x0C */ CfbInfo* framebuffer;
    /* 0x10 */ OSTask list;
    /* 0x50 */ OSMesgQueue* msgQ;
    /* 0x54 */ OSMesg msg;
} OSScTask;

// TODO: Update the type to upstream's
typedef struct SchedContext {
    /* 0x0000 */ OSMesgQueue  interruptQ;
    /* 0x0018 */ OSMesg       intBuf[8];
    /* 0x0038 */ OSMesgQueue  cmdQ;
    /* 0x0050 */ OSMesg       cmdMsgBuf[8];
    /* 0x0070 */ OSThread     thread;
    /* 0x0220 */ OSScTask*    audioListHead;
    /* 0x0224 */ OSScTask*    gfxListHead;
    /* 0x0228 */ OSScTask*    audioListTail;
    /* 0x022C */ OSScTask*    gfxListTail;
    /* 0x0230 */ OSScTask*    curRSPTask;
    /* 0x0234 */ OSScTask*    curRDPTask;
    /* 0x0238 */ s32          retraceCnt;
    /* 0x023C */ s32          doAudio;
    /* 0x0240 */ CfbInfo*     curBuf;
    /* 0x0244 */ CfbInfo*     pendingSwapBuf1;
    /* 0x0220 */ CfbInfo*     pendingSwapBuf2;
    /* 0x0220 */ UNK_TYPE4    unk_24C;
    /* 0x0250 */ IrqMgrClient irqClient;
} SchedContext; // size = 0x258

// TODO: Rename to Sched_Notify
EXTERN_C void Sched_SendEntryMsg(SchedContext* sc);
EXTERN_C void Sched_Init(SchedContext* sc, void* stack, OSPri priority, UNK_TYPE arg3, UNK_TYPE arg4, IrqMgr* irqMgr);
// TODO: Rename to Sched_FlushTaskQueue
EXTERN_C void MsgEvent_SendNullTask(void);

// TODO: Rename to gScheduler
EXTERN_C SchedContext gSchedContext;

#endif
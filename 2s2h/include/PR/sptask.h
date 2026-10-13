#ifndef PR_SPTASK_H
#define PR_SPTASK_H

#include <libultraship/libultra/sptask.h>

#if 0

#include "PR/ultratypes.h"
#include "libc/stddef.h"

/* Task Types */
#define M_NULTASK 0
#define M_GFXTASK 1
#define M_AUDTASK 2
#define M_VIDTASK 3
#define M_NJPEGTASK 4
#define M_HVQTASK 6
#define M_HVQMTASK 7

/* Task Flags  */
#define M_TASK_FLAG0 (1 << 0)
#define M_TASK_FLAG1 (1 << 1)

/* Task Flag Fields */
#define OS_TASK_YIELDED (1 << 0)
#define OS_TASK_DP_WAIT (1 << 1)
#define OS_TASK_LOADABLE (1 << 2)
#define OS_TASK_SP_ONLY (1 << 3)
#define OS_TASK_USR0 (1 << 4)
#define OS_TASK_USR1 (1 << 5)
#define OS_TASK_USR2 (1 << 6)
#define OS_TASK_USR3 (1 << 7)

#define OS_YIELD_DATA_SIZE 0xC00
#define OS_YIELD_AUDIO_SIZE 0x400

/* SpStatus */

/* Write */
#define SPSTATUS_CLEAR_HALT (1 << 0)
#define SPSTATUS_SET_HALT (1 << 1)
#define SPSTATUS_CLEAR_BROKE (1 << 2)
#define SPSTATUS_CLEAR_INTR (1 << 3)
#define SPSTATUS_SET_INTR (1 << 4)
#define SPSTATUS_CLEAR_SSTEP (1 << 5)
#define SPSTATUS_SET_SSTEP (1 << 6)
#define SPSTATUS_CLEAR_INTR_ON_BREAK (1 << 7)
#define SPSTATUS_SET_INTR_ON_BREAK (1 << 8)
#define SPSTATUS_CLEAR_SIGNAL0 (1 << 9)
#define SPSTATUS_SET_SIGNAL0 (1 << 10)
#define SPSTATUS_CLEAR_SIGNAL1 (1 << 11)
#define SPSTATUS_SET_SIGNAL1 (1 << 12)
#define SPSTATUS_CLEAR_SIGNAL2 (1 << 13)
#define SPSTATUS_SET_SIGNAL2 (1 << 14)
#define SPSTATUS_CLEAR_SIGNAL3 (1 << 15)
#define SPSTATUS_SET_SIGNAL3 (1 << 16)
#define SPSTATUS_CLEAR_SIGNAL4 (1 << 17)
#define SPSTATUS_SET_SIGNAL4 (1 << 18)
#define SPSTATUS_CLEAR_SIGNAL5 (1 << 19)
#define SPSTATUS_SET_SIGNAL5 (1 << 20)
#define SPSTATUS_CLEAR_SIGNAL6 (1 << 21)
#define SPSTATUS_SET_SIGNAL6 (1 << 23)
#define SPSTATUS_CLEAR_SIGNAL7 (1 << 24)
#define SPSTATUS_SET_SIGNAL7 (1 << 25)

/* Read */
#define SPSTATUS_HALT (1 << 0)
#define SPSTATUS_BROKE (1 << 1)
#define SPSTATUS_DMA_BUSY (1 << 2)
#define SPSTATUS_DMA_FULL (1 << 3)
#define SPSTATUS_IO_FULL (1 << 4)
#define SPSTATUS_SINGLE_STEP (1 << 5)
#define SPSTATUS_INTERRUPT_ON_BREAK (1 << 6)
#define SPSTATUS_SIGNAL0_SET (1 << 7)
#define SPSTATUS_SIGNAL1_SET (1 << 8)
#define SPSTATUS_SIGNAL2_SET (1 << 9)
#define SPSTATUS_SIGNAL3_SET (1 << 10)
#define SPSTATUS_SIGNAL4_SET (1 << 11)
#define SPSTATUS_SIGNAL5_SET (1 << 12)
#define SPSTATUS_SIGNAL6_SET (1 << 13)
#define SPSTATUS_SIGNAL7_SET (1 << 14)

typedef struct {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 flags;

    /* 0x08 */ u64* ucodeBoot;
    /* 0x0C */ u32 ucodeBootSize; // ucode will load these sizes with lw, so need to be 32-bit

    /* 0x10 */ u64* ucode;
    /* 0x14 */ u32 ucodeSize;

    /* 0x18 */ u64* ucodeData;
    /* 0x1C */ u32 ucodeDataSize;

    /* 0x20 */ u64* dramStack;
    /* 0x24 */ u32 dramStackSize;

    /* 0x28 */ u64* outputBuff;
    /* 0x2C */ u64* outputBuffSize;

    /* 0x30 */ u64* dataPtr;
    /* 0x34 */ u32 dataSize;

    /* 0x38 */ u64* yieldDataPtr;
    /* 0x3C */ u32 yieldDataSize;
} OSTask_t; // size = 0x40

typedef union {
    OSTask_t t;
    long long int force_structure_alignment;
} OSTask;

typedef u32 OSYieldResult;

void osSpTaskLoad(OSTask* intp);
void osSpTaskStartGo(OSTask* tp);

void osSpTaskYield(void);
OSYieldResult osSpTaskYielded(OSTask* task);
#endif
#endif

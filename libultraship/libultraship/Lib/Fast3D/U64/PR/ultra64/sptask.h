#ifndef ULTRA64_SPTASK_H
#define ULTRA64_SPTASK_H

#include "types.h"

/* Task Types */
#define M_NULTASK   0
#define M_GFXTASK   1
#define M_AUDTASK   2
#define M_VIDTASK   3
#define M_NJPEGTASK 4
#define M_HVQTASK   6
#define M_HVQMTASK  7

/* Task Flags  */
#define M_TASK_FLAG0 1
#define M_TASK_FLAG1 2

/* Task Flag Fields */
#define OS_TASK_YIELDED       0x0001
#define OS_TASK_DP_WAIT       0x0002
#define OS_TASK_LOADABLE      0x0004
#define OS_TASK_SP_ONLY       0x0008
#define OS_TASK_USR0          0x0010
#define OS_TASK_USR1          0x0020
#define OS_TASK_USR2          0x0040
#define OS_TASK_USR3          0x0080

#define OS_YIELD_DATA_SIZE 0xC00

typedef struct {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 flags;

    /* 0x08 */ u64* ucode_boot;
    /* 0x0C */ u32 ucode_boot_size;

    /* 0x10 */ u64* ucode;
    /* 0x14 */ u32 ucode_size;

    /* 0x18 */ u64* ucode_data;
    /* 0x1C */ u32 ucode_data_size;

    /* 0x20 */ u64* dram_stack;
    /* 0x24 */ u32 dram_stack_size;

    /* 0x28 */ u64* output_buff;
    /* 0x2C */ u64* output_buff_size;

    /* 0x30 */ u64* data_ptr;
    /* 0x34 */ u32 data_size;

    /* 0x38 */ u64* yield_data_ptr;
    /* 0x3C */ u32 yield_data_size;
} OSTask_t; // size = 0x40

typedef union {
    OSTask_t t;
    long long int force_structure_alignment;
} OSTask;

typedef u32 OSYieldResult;

#define osSpTaskStart(p) \
    osSpTaskLoad(p);     \
    osSpTaskStartGo(p);

#endif

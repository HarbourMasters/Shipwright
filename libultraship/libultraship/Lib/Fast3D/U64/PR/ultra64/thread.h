#ifndef ULTRA64_THREAD_H
#define ULTRA64_THREAD_H

#include "types.h"

#define OS_PRIORITY_MAX      255
#define OS_PRIORITY_VIMGR    254
#define OS_PRIORITY_RMON     250
#define OS_PRIORITY_RMONSPIN 200
#define OS_PRIORITY_PIMGR    150
#define OS_PRIORITY_SIMGR    140
#define OS_PRIORITY_APPMAX   127
#define OS_PRIORITY_IDLE       0

#define OS_STATE_STOPPED    1
#define OS_STATE_RUNNABLE   2
#define OS_STATE_RUNNING    4
#define OS_STATE_WAITING    8

#define OS_FLAG_CPU_BREAK   1
#define OS_FLAG_FAULT       2

typedef s32 OSPri;
typedef s32 OSId;

typedef union {
    struct {
        /* 0x00 */ f32 f_odd;
        /* 0x04 */ f32 f_even;
    } f;
} __OSfp; // size = 0x08

typedef struct {
    /* 0x000 */ u64 at, v0, v1, a0, a1, a2, a3;
    /* 0x038 */ u64 t0, t1, t2, t3, t4, t5, t6, t7;
    /* 0x078 */ u64 s0, s1, s2, s3, s4, s5, s6, s7;
    /* 0x0B8 */ u64 t8, t9, gp, sp, s8, ra;
    /* 0x0E8 */ u64 lo, hi;
    /* 0x0F8 */ u32 sr, pc, cause, badvaddr, rcp;
    /* 0x10C */ u32 fpcsr;
    /* 0x110 */ __OSfp  fp0,  fp2,  fp4,  fp6,  fp8, fp10, fp12, fp14;
    /* 0x150 */ __OSfp fp16, fp18, fp20, fp22, fp24, fp26, fp28, fp30;
} __OSThreadContext; // size = 0x190

typedef struct {
    /* 0x00 */ u32 flag;
    /* 0x04 */ u32 count;
    /* 0x08 */ u64 time;
} __OSThreadprofile; // size = 0x10

typedef struct OSThread {
    /* 0x00 */ struct OSThread* next;
    /* 0x04 */ OSPri priority;
    /* 0x08 */ struct OSThread** queue;
    /* 0x0C */ struct OSThread* tlnext;
    /* 0x10 */ u16 state;
    /* 0x12 */ u16 flags;
    /* 0x14 */ OSId id;
    /* 0x18 */ s32 fp;
    /* 0x1C */ __OSThreadprofile* thprof;
    /* 0x20 */ __OSThreadContext context;
} OSThread; // size = 0x1B0

#endif

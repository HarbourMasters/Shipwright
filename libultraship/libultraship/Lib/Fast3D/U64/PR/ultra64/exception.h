#ifndef ULTRA64_EXCEPTION_H
#define ULTRA64_EXCEPTION_H

#include "types.h"

// Interrupt masks
#define OS_IM_NONE      0x00000001
#define OS_IM_SW1       0x00000501
#define OS_IM_SW2       0x00000601
#define OS_IM_CART      0x00000C01
#define OS_IM_PRENMI    0x00001401
#define OS_IM_RDBWRITE  0x00002401
#define OS_IM_RDBREAD   0x00004401
#define OS_IM_COUNTER   0x00008401
#define OS_IM_CPU       0x0000FF01
#define OS_IM_SP        0x00010401
#define OS_IM_SI        0x00020401
#define OS_IM_AI        0x00040401
#define OS_IM_VI        0x00080401
#define OS_IM_PI        0x00100401
#define OS_IM_DP        0x00200401
#define OS_IM_ALL       0x003FFF01

#define RCP_IMASK       0x003F0000
#define RCP_IMASKSHIFT  16

typedef u32 OSIntMask;
typedef u32 OSHWIntr;

typedef struct  {
    /* 0x00 */ void* callback;
    /* 0x04 */ void* sp;
} __osHwInt; // size = 0x08

OSIntMask osSetIntMask(OSIntMask);

void __osSetHWIntrRoutine(OSHWIntr interrupt, s32 (*callback)(void), void *sp);
void __osGetHWIntrRoutine(OSHWIntr interrupt, s32 (**callbackOut)(void), void **spOut);

void __osSetGlobalIntMask(OSHWIntr mask);
void __osResetGlobalIntMask(OSHWIntr mask);

extern __osHwInt __osHwIntTable[];

#endif

#ifndef ULTRA64_CONVERT_H
#define ULTRA64_CONVERT_H

#define OS_CLOCK_RATE           62500000LL
#define OS_CPU_COUNTER          (OS_CLOCK_RATE*3/4)

#define OS_NSEC_TO_CYCLES(n)    (((u64)(n)*(OS_CPU_COUNTER/15625000LL))/(1000000000LL/15625000LL))
#define OS_USEC_TO_CYCLES(n)    (((u64)(n)*(OS_CPU_COUNTER/15625LL))/(1000000LL/15625LL))
#define OS_CYCLES_TO_NSEC(c)    (((u64)(c)*(1000000000LL/15625000LL))/(OS_CPU_COUNTER/15625000LL))
#define OS_CYCLES_TO_USEC(c)    (((u64)(c)*(1000000LL/15625LL))/(OS_CPU_COUNTER/15625LL))

#define OS_K0_TO_PHYSICAL(x)    (u32)(((char*)(x)-0x80000000))
#define OS_K1_TO_PHYSICAL(x)    (0)//(u32)(((char*)(x)-0xA0000000))

#define OS_PHYSICAL_TO_K0(x)    (void*)(((u32)(x)+0x80000000))
#define OS_PHYSICAL_TO_K1(x)    (void*)(((u32)(x)+0xA0000000))

#endif

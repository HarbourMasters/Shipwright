#include "ultra64.h"
#include "libc/stdbool.h"

#define COLD_RESET 0

typedef struct __osExceptionVector {
    /* 0x0 */ u32 ins_00; // lui     k0, 0x8000
    /* 0x4 */ u32 ins_04; // addiu   k0, k0, 0x39E0
    /* 0x8 */ u32 ins_08; // jr      k0 ; __osException
    /* 0xC */ u32 ins_0C; // nop
} __osExceptionVector;    // size = 0x10

extern __osExceptionVector __osExceptionPreamble;

u64 osClockRate = OS_CLOCK_RATE;
s32 osViClock = VI_NTSC_CLOCK;
u32 __osShutdown = 0;
u32 __OSGlobalIntMask = OS_IM_ALL;

u32 __osFinalrom;

void __createSpeedParam(void) {
    __Dom1SpeedParam.type = DEVICE_TYPE_INIT;
    __Dom1SpeedParam.latency = IO_READ(PI_BSD_DOM1_LAT_REG);
    __Dom1SpeedParam.pulse = IO_READ(PI_BSD_DOM1_PWD_REG);
    __Dom1SpeedParam.pageSize = IO_READ(PI_BSD_DOM1_PGS_REG);
    __Dom1SpeedParam.relDuration = IO_READ(PI_BSD_DOM1_RLS_REG);

    __Dom2SpeedParam.type = DEVICE_TYPE_INIT;
    __Dom2SpeedParam.latency = IO_READ(PI_BSD_DOM2_LAT_REG);
    __Dom2SpeedParam.pulse = IO_READ(PI_BSD_DOM2_PWD_REG);
    __Dom2SpeedParam.pageSize = IO_READ(PI_BSD_DOM2_PGS_REG);
    __Dom2SpeedParam.relDuration = IO_READ(PI_BSD_DOM2_RLS_REG);
}

void __osInitialize_common(void) {
    u32 pifdata;

    __osFinalrom = 1;

    __osSetSR(__osGetSR() | SR_CU1);
    __osSetFpcCsr(FPCSR_FS | FPCSR_EV);
    __osSetWatchLo(0x04900000);

    while (__osSiRawReadIo(PIF_RAM_END - 3, &pifdata)) {}

    while (__osSiRawWriteIo(PIF_RAM_END - 3, pifdata | 8)) {}

    *(__osExceptionVector*)UT_VEC = __osExceptionPreamble;
    *(__osExceptionVector*)XUT_VEC = __osExceptionPreamble;
    *(__osExceptionVector*)ECC_VEC = __osExceptionPreamble;
    *(__osExceptionVector*)E_VEC = __osExceptionPreamble;

    osWritebackDCache((void*)UT_VEC, E_VEC - UT_VEC + sizeof(__osExceptionVector));
    osInvalICache((void*)UT_VEC, E_VEC - UT_VEC + sizeof(__osExceptionVector));
    __createSpeedParam();
    osUnmapTLBAll();
    osMapTLBRdb();

    osClockRate = osClockRate * 3 / 4;

    if (osResetType == COLD_RESET) {
        bzero(osAppNMIBuffer, OS_APP_NMI_BUFSIZE);
    }

    if (osTvType == OS_TV_PAL) {
        osViClock = VI_PAL_CLOCK;
    } else if (osTvType == OS_TV_MPAL) {
        osViClock = VI_MPAL_CLOCK;
    } else {
        osViClock = VI_NTSC_CLOCK;
    }

    if (__osGetCause() & CAUSE_IP5) {
        while (true) {}
    }

    IO_WRITE(AI_CONTROL_REG, AI_CONTROL_DMA_ON);
    IO_WRITE(AI_DACRATE_REG, AI_MAX_DAC_RATE - 1);
    IO_WRITE(AI_BITRATE_REG, AI_MAX_BIT_RATE - 1);
}

void __osInitialize_autodetect(void) {
}

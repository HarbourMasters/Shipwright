#ifndef PR_OS_INTERNAL_H
#define PR_OS_INTERNAL_H

#include <libultraship/libultra/exception.h>

#if 0
#include "ultratypes.h"
#include "os_message.h"
#include "os_pi.h"
#include "os_internal_rsp.h"

typedef struct __osHwInt {
    /* 0x0 */ s32 (*handler)(void);
    /* 0x4 */ void* stackEnd;
} __osHwInt; // size = 0x8

typedef struct {
    /* 0x00 */ u32 initialized;
    /* 0x04 */ OSThread* mgrThread;
    /* 0x08 */ OSMesgQueue* cmdQueue;
    /* 0x0C */ OSMesgQueue* eventQueue;
    /* 0x10 */ OSMesgQueue* accessQueue;
    /* 0x14 */ s32 (*piDmaCallback)(s32, uintptr_t, void*, size_t);
    /* 0x18 */ s32 (*epiDmaCallback)(OSPiHandle*, s32, uintptr_t, void*, size_t);
} OSMgrArgs; // size = 0x1C

#endif
#endif
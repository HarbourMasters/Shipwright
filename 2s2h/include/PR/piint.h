#ifndef PR_PIINT_H
#define PR_PIINT_H

#include <libultraship/libultra/printf.h>

#if 0

#include "ultratypes.h"
#include "os_pi.h"
#include "libc/stdint.h"


extern OSDevMgr __osPiDevMgr;
extern OSPiHandle* __osCurrentHandle[];
extern u32 __osPiAccessQueueEnabled;

extern OSPiHandle __Dom1SpeedParam;
extern OSPiHandle __Dom2SpeedParam;

extern OSMesgQueue __osPiAccessQueue;

void __osDevMgrMain(void* arg);
void __osPiCreateAccessQueue(void);
void __osPiRelAccess(void);
void __osPiGetAccess(void);
s32 __osPiRawStartDma(s32 direction, uintptr_t devAddr, void* dramAddr, size_t size);
s32 __osEPiRawWriteIo(OSPiHandle* handle, uintptr_t devAddr, u32 data);
s32 __osEPiRawReadIo(OSPiHandle* handle, uintptr_t devAddr, u32* data);
s32 __osEPiRawStartDma(OSPiHandle* handle, s32 direction, uintptr_t cartAddr, void* dramAddr, size_t size);
OSMesgQueue* osPiGetCmdQueue(void);
#endif
#endif
#ifndef PR_OS_INTERNAL_RSP_H
#define PR_OS_INTERNAL_RSP_H

#include "ultratypes.h"

u32 __osSpGetStatus(void);
void __osSpSetStatus(u32 data);
s32 __osSpSetPc(u32 pc);
s32 __osSpRawStartDma(s32 direction, u32 devAddr, void* dramAddr, size_t size);

#endif

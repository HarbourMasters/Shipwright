#ifndef PR_OS_INTERNAL_SI_H
#define PR_OS_INTERNAL_SI_H

#include "ultratypes.h"
#include "libc/stdint.h"


s32 __osSiRawWriteIo(uintptr_t devAddr, u32 data);
s32 __osSiRawReadIo(uintptr_t devAddr, u32* data);
s32 __osSiRawStartDma(s32 direction, void* dramAddr);


#endif

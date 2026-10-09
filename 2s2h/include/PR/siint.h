#ifndef PR_SIINT_H
#define PR_SIINT_H

#include "ultratypes.h"

extern u8 __osPfsInodeCacheBank;

void __osSiGetAccess(void);
void __osSiRelAccess(void);
s32 __osSiDeviceBusy(void);
void __osSiCreateAccessQueue(void);

#endif

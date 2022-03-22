#include "global.h"

void __osResetGlobalIntMask(OSHWIntr mask) {
    register u32 prevInt = __osDisableInt();

    __OSGlobalIntMask &= ~(mask & ~0x401);
    __osRestoreInt(prevInt);
}

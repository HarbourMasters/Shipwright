#include "ultra64.h"

void __osResetGlobalIntMask(OSHWIntr mask) {
    register s32 prevInt = __osDisableInt();

    __OSGlobalIntMask &= ~(mask & ~OS_IM_RCP);
    __osRestoreInt(prevInt);
}

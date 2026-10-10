#include "ultra64.h"

void __osSetGlobalIntMask(OSHWIntr mask) {
    register s32 prevInt = __osDisableInt();

    __OSGlobalIntMask |= mask;
    __osRestoreInt(prevInt);
}

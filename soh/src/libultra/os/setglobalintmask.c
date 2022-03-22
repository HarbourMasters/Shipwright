#include "global.h"

void __osSetGlobalIntMask(OSHWIntr mask) {
    register u32 prevInt = __osDisableInt();

    __OSGlobalIntMask |= mask;
    __osRestoreInt(prevInt);
}

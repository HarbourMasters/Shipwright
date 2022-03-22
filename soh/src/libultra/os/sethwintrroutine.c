#include "global.h"
#include "ultra64/internal.h"

void __osSetHWIntrRoutine(OSHWIntr intr, s32 (*callback)(void), void* sp) {
    register u32 prevInt = __osDisableInt();

    __osHwIntTable[intr].callback = callback;
    __osHwIntTable[intr].sp = sp;

    __osRestoreInt(prevInt);
}

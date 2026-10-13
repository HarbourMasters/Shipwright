#include "PR/os_internal.h"
#include "PR/osint.h"

void __osSetHWIntrRoutine(OSHWIntr interrupt, s32 (*handler)(void), void* stackEnd) {
    register s32 prevInt = __osDisableInt();

    __osHwIntTable[interrupt].handler = handler;
    __osHwIntTable[interrupt].stackEnd = stackEnd;

    __osRestoreInt(prevInt);
}

#include "PR/os_internal.h"
#include "PR/osint.h"

void __osGetHWIntrRoutine(OSHWIntr interrupt, s32 (**handler)(void), void** stackEnd) {
    *handler = __osHwIntTable[interrupt].handler;
    *stackEnd = __osHwIntTable[interrupt].stackEnd;
}

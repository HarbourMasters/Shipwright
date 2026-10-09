#include "global.h"
#include "fault.h"

void _dbg_hungup(const char* file, int lineNum) {
    // osGetThreadId(NULL);
    Fault_AddHungupAndCrash(file, lineNum);
}

void Reset(void) {
    Fault_AddHungupAndCrash("Reset", 0);
}

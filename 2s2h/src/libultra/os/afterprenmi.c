#include "ultra64.h"

s32 osAfterPreNMI(void) {
    return __osSpSetPc(0);
}

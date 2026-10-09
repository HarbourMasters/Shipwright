#include "ultra64.h"

u32 __osSpGetStatus() {
    return IO_READ(SP_STATUS_REG);
}

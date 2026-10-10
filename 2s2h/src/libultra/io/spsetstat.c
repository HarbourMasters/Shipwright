#include "ultra64.h"

void __osSpSetStatus(u32 data) {
    IO_WRITE(SP_STATUS_REG, data);
}

#include "ultra64.h"

void osDpSetStatus(u32 data) {
    IO_WRITE(DPC_STATUS_REG, data);
}

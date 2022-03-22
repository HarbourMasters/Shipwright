#include "global.h"

void osDpSetStatus(u32 status) {
    DPC_STATUS_REG = status;
}

#include "global.h"

u32 __osSpGetStatus(void) {
    return HW_REG(SP_STATUS_REG, u32);
}

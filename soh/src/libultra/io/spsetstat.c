// clang-format off

#include "global.h"

void __osSpSetStatus(u32 status) {
    HW_REG(SP_STATUS_REG, u32) = status;
}

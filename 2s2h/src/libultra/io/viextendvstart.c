#include "ultra64.h"

void osViExtendVStart(u32 value) {
    __additional_scanline = value;
}

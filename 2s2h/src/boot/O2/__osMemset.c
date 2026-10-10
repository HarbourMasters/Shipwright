#include "global.h"

void* __osMemset(void* ptr, s32 val, size_t size) {
    u8* dst = ptr;
    register s32 rem;

    for (rem = size--; rem != 0; rem = size--) {
        *dst++ = val;
    }
    return ptr;
}

#include "global.h"

#define STEP 0x100000

u32 osGetMemSize(void) {
    u32* ptr;
    size_t size = 0x400000;
    u32 data0;
    u32 data1;

    while (size < 0x800000) {
        ptr = (u32*)(0xA0000000 + size);

        data0 = *ptr;
        data1 = ptr[STEP / 4 - 1];

        *ptr ^= ~0;
        ptr[STEP / 4 - 1] ^= ~0;

        if ((*ptr != (data0 ^ ~0)) || (ptr[STEP / 4 - 1] != (data1 ^ ~0))) {
            return size;
        }

        *ptr = data0;
        ptr[STEP / 4 - 1] = data1;

        size += STEP;
    }

    return size;
}

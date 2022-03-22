#include "global.h"

u32 osVirtualToPhysical(void* vaddr) {
    if ((u32)vaddr >= 0x80000000 && (u32)vaddr < 0xA0000000) {
        return (u32)vaddr & 0x1FFFFFFF;
    }

    if ((u32)vaddr >= 0xA0000000 && (u32)vaddr < 0xC0000000) {
        return (u32)vaddr & 0x1FFFFFFF;
    }

    return __osProbeTLB(vaddr);
}

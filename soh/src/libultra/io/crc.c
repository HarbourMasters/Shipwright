#include "global.h"

// Valid addr up to 0x7FF
// It's the address of a block of 0x20 bytes in the mempak
// So that means the whole mempak has a 16-bit address space
u8 __osContAddressCrc(u16 addr) {
    u32 addr32 = addr;
    u32 ret = 0;
    u32 bit;
    s32 i;

    for (bit = 0x400; bit; bit >>= 1) {
        ret <<= 1;
        if (addr32 & bit) {
            if (ret & 0x20) {
                ret ^= 0x14;
            } else {
                ++ret;
            }
        } else if (ret & 0x20) {
            ret ^= 0x15;
        }
    }
    for (i = 0; i < 5; ++i) {
        ret <<= 1;
        if (ret & 0x20) {
            ret ^= 0x15;
        }
    }

    return ret & 0x1F;
}

u8 __osContDataCrc(u8* data) {
    s32 ret = 0;
    u32 bit;
    u32 byte;

    for (byte = 0x20; byte; --byte, ++data) {
        for (bit = 0x80; bit; bit >>= 1) {
            ret <<= 1;
            if ((*data & bit) != 0) {
                if ((ret & 0x100) != 0) {
                    ret ^= 0x84;
                } else {
                    ++ret;
                }
            } else if (ret & 0x100) {
                ret ^= 0x85;
            }
        }
    }
    do {
        ret <<= 1;
        if (ret & 0x100) {
            ret ^= 0x85;
        }
        ++byte;
    } while (byte < 8U);

    return ret;
}

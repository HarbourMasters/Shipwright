/**
 * File: voicecrc.c
 * Description: CRC check used by the Voice library functions in libultra.
 *
 * For general information about CRC, see the crc.c file (that's a lot of c's!).
 */
#include "libc/stddef.h"

#define VOICE_CRC_LENGTH 8
#define VOICE_CRC_GENERATOR 0x85

/**
 * This function is essentially the same as __osContDataCrc, but allows for a variable message length, specified by
 * `numBytes`.
 */
u8 __osVoiceContDataCrc(u8* data, size_t numBytes) {
    s32 ret = 0;
    u32 bit;
    size_t byte;

    for (byte = numBytes; byte != 0; byte--, data++) {
        // Loop over each bit in the byte starting with most significant
        for (bit = (1 << (VOICE_CRC_LENGTH - 1)); bit != 0; bit >>= 1) {
            ret <<= 1;
            if (*data & bit) {
                if (ret & (1 << VOICE_CRC_LENGTH)) {
                    // Same as ret++; ret ^= 0x85 since last bit always 0 after the shift
                    ret ^= VOICE_CRC_GENERATOR - 1;
                } else {
                    ret++;
                }
            } else if (ret & (1 << VOICE_CRC_LENGTH)) {
                ret ^= VOICE_CRC_GENERATOR;
            }
        }
    }
    // Act like a byte of zeros is appended to data
    do {
        ret <<= 1;
        if (ret & (1 << VOICE_CRC_LENGTH)) {
            ret ^= VOICE_CRC_GENERATOR;
        }
        byte++;
    } while (byte < VOICE_CRC_LENGTH);

    // Discarding the excess is done automatically by the return type
    return ret;
}

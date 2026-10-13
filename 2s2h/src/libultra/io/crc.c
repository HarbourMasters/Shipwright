/**
 * File: crc.c
 * Description: Functions to compute Cyclic Redundancy Check for specific addresses and data.
 *
 * CRC notes:
 *
 * General
 * ===
 * - CRC (Cyclic Redundancy Check) is a way of verifying that no errors were introduced in transmitted data.
 * - It reads the entire message and generates a check number that is appended to it.
 * - A CRC is specified by the length `n` of the check number and a number (called the generator) smaller than `1 << n`.
 * - Different generators have different error-checking capabilities. The choice of a generator is a sophisticated
 * mathematical problem.
 *
 * Mathematical basis
 * ===
 * - The algorithm is based on division of polynomials. The polynomials have coefficients in the field with two
 * elements, 0 and 1, with addition given by XOR and multiplication by AND (it turns out this really is a field).
 * Subtraction is the same as addition.
 * - There is a one-to-one correspondence between binary polynomials and binary numbers: just evaluate the polynomial at
 * 2, or write down an \f$ X^k \f$ corresponding to each `1 << k` the number is composed of.
 * - The message bits `m{L}m{L-1}...m{1}m{0}` correspond to a polynomial \f$ m(X) = m_L X^L + m_{L-1} X^{L-1} + \dotsb +
 * m_1 X^1 + m_0 X^0 \f$. We multiply this by \f$ X^n \f$ to make a space to insert the remainder at the end; this new
 * polynomial will be the dividend.
 * - The generator `p{n-1}p{n-2}...p{1}p{0}` corresponds to a polynomial \f$ p(X) = X^n + p_{n-1} X^{n-1} + \dotsb + p_1
 * X^1 + p_0 X^0 \f$: the leading term is omitted in the binary description because it is always \f$ X^n \f$. The
 * generator polynomial is the divisor.
 * - The usual division algorithm is followed: we look along the dividend until we see a nonzero coefficient, then
 * subtract an appropriate multiple of the divisor to cancel it out. We repeat this until we reach the end of the
 * number.
 * - Arithmetic in the field with two elements is particularly simple: subtraction is identical to addition, so also
 * given by XOR, and the only multipliers required for subtracting the divisor are \f$ X^k \f$.
 * - After applying the algorithm, the output is a polynomial \f$ R(X) \f$ so that we have
 * \f[ m(X) X^n = Q(X) p(X) + R(X) \f]
 * (\f$ R(X) \f$ is the *remainder after dividing by \f$ p(X) \f$*).
 * - Therefore, \f$ m(X) X^n - R(X) \f$ is divisible by the generator polynomial. This means that if we append the
 * binary number corresponding to \f$ R(X) \f$ to the message and rerun the algorithm, we will get 0 if no errors have
 * been introduced.
 *
 *
 * Implementation
 * ===
 * - We translate the binary polynomials to binary numbers by evaluating them at 2. The leading term in the generator
 * polynomial is always \f$ X^n \f$, so we discard it to save space. In the binary field, subtraction is the same as
 * addition, and given by XOR. Multiplication by \f$ X \f$ is given by shifting left.
 * - Instead of fixing the message and moving the divisor polynomial right, we scan the message from the most
 * significant digit, adding it to the end of the return value, (that is, for 1s, we shift and add 1, for 0s we just
 * shift, effectively using the return value as a shift register).
 * - When the return value has a 1 in the nth position (corresponding to the leading term in the generator polynomial),
 * we binary-subtract (i.e. XOR) the return value with the generator polynomial's number.
 * - This is repeated until we reach the end of the message.
 * - Finally, to take into account the final multiplication by \f$ X^n \f$, we run another loop, which acts like we
 * passed \f$ n \f$ more digits in the message that are all zero. Remember this gives us the extra space at the end for
 * the check digits to be added.
 *
 *
 * - To specify a CRC, at minimum we need the length of the check (i.e. the degree of the generator polynomial), \f$ n
 * \f$, and the rest of the generator polynomial. This is usually expressed in the binary form, written as hex for
 * compactness. Algorithms may also reverse or invert certain parts of the data or check to improve particular aspects
 * of the algorithm, but the libultra functions use the simplest version.
 *
 *
 * Resources
 * ===
 * - Wikipedia: [Cyclic redundancy check](https://en.wikipedia.org/wiki/Cyclic_redundancy_check), and more specifically
 * [Computation of cyclic redundancy checks](https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks)
 * - Ben Eater has two videos on CRCs, the last two linked on [Error Detection | Ben Eater](https://eater.net/crc)
 * - A page that specifically describes the same shift-register-style algorithms as libultra uses: [Understanding and
 * implementing CRC (Cyclic Redundancy Check) calculation
 * ](http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html)
 */
#include "ultra64.h"

#define ADDRESS_CRC_MESSAGE_LENGTH 10
#define ADDRESS_CRC_LENGTH 5
#define ADDRESS_CRC_GENERATOR 0x15
#define ADDRESS_CRC_MASK ((1 << ADDRESS_CRC_LENGTH) - 1)

/**
 * CRC-5 with the generating polynomial \f$ x^5 + x^4 + x^2 + 1 \f$, AKA 0x15 = 0b(1)1 0101.
 * It only works on the bits from 0x7FF = 11 11111111, i.e. 10 bits.
 *
 * Usually used as __osContAddressCrc(addr) | (addr << 5) to add the CRC to the end. The overall length of 10 + 5 bits
 * allows the address + CRC to fit into one s16.
 *
 * `addr` is the address of a block in the mempak, only valid up to 0x7FF.
 */
u8 __osContAddressCrc(u16 addr) {
    u32 addr32 = addr;
    u32 ret = 0;
    u32 bit;
    s32 i;

    // ret is used as a shift register for the CRC
    for (bit = (1 << ADDRESS_CRC_MESSAGE_LENGTH); bit != 0; bit >>= 1) {
        ret <<= 1;
        if (addr32 & bit) {
            if (ret & (1 << ADDRESS_CRC_LENGTH)) {
                // Same as ret++; ret ^= 0x15 since last bit always 0 after the shift
                ret ^= ADDRESS_CRC_GENERATOR - 1;
            } else {
                ret++;
            }
        } else if (ret & (1 << ADDRESS_CRC_LENGTH)) {
            ret ^= ADDRESS_CRC_GENERATOR;
        }
    }
    // Acts like 5 bits of 0s are appended to addr
    for (i = 0; i < ADDRESS_CRC_LENGTH; i++) {
        ret <<= 1;
        if (ret & (1 << ADDRESS_CRC_LENGTH)) {
            ret ^= ADDRESS_CRC_GENERATOR;
        }
    }

    // Discard the irrelevant bits above the actual remainder
    return ret & ADDRESS_CRC_MASK;
}

#define DATA_CRC_MESSAGE_BYTES 32
#define DATA_CRC_LENGTH 8
#define DATA_CRC_GENERATOR 0x85

/**
 * CRC-8 with generating polynomial \f$ x^8 + x^7 + x^2 + 1 \f$, AKA 0x85 = 0b(1) 1000 0101.
 * Expects exactly 0x20 = 32 bytes of data.
 */
u8 __osContDataCrc(u8* data) {
    s32 ret = 0;
    u32 bit;
    u32 byte;

    for (byte = DATA_CRC_MESSAGE_BYTES; byte != 0; byte--, data++) {
        // Loop over each bit in the byte starting with most significant
        for (bit = (1 << (DATA_CRC_LENGTH - 1)); bit != 0; bit >>= 1) {
            ret <<= 1;
            if (*data & bit) {
                if (ret & (1 << DATA_CRC_LENGTH)) {
                    // Same as ret++; ret ^= 0x85 since last bit always 0 after the shift
                    ret ^= DATA_CRC_GENERATOR - 1;
                } else {
                    ret++;
                }
            } else if (ret & (1 << DATA_CRC_LENGTH)) {
                ret ^= DATA_CRC_GENERATOR;
            }
        }
    }
    // Act like a byte of zeros is appended to data
    do {
        ret <<= 1;
        if (ret & (1 << DATA_CRC_LENGTH)) {
            ret ^= DATA_CRC_GENERATOR;
        }
        byte++;
    } while (byte < DATA_CRC_LENGTH);

    // Discarding the excess is done automatically by the return type
    return ret;
}

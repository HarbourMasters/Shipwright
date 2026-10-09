#include "ultra64.h"

/**
 * Compute the sine of a hex angle and return a short, using the formula cos(x) = sin(x+pi).
 */
s16 coss(u16 x) {
    return sins(x + 0x4000);
}

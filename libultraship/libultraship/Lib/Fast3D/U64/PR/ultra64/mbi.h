#ifndef ULTRA64_MBI_H
#define ULTRA64_MBI_H

/*
 * Header file for the Media Binary Interface
 *
 * NOTE: This file is included by the RSP microcode, so any C-specific
 * constructs must be bracketed by #ifdef _LANGUAGE_C
 *
 */


/*
 * the SHIFT macros are used to build display list commands, inserting
 * bit-fields into a 32-bit word. They take a value, a shift amount, 
 * and a width.
 *
 * For the left shift, the lower bits of the value are masked, 
 * then shifted left.
 *
 * For the right shift, the value is shifted right, then the lower bits
 * are masked.
 *
 * (NOTE: _SHIFTL(v, 0, 32) won't work, just use an assignment)
 *
 */
#define _SHIFTL(v, s, w)    \
    ((u32) (((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define _SHIFTR(v, s, w)    \
    ((u32)(((u32)(v) >> (s)) & ((0x01 << (w)) - 1)))

#define G_ON    (1)
#define G_OFF   (0)

#include <stdint.h>
#include "types.h"
#include "gbi.h"
#include "abi.h"

#define NUM_SEGMENTS        (16)
#define SEGMENT_OFFSET(a)   ((u32)(a) & 0x00FFFFFF)
#define SEGMENT_NUMBER(a)   (((u32)(a) << 4) >> 28)
#define SEGMENT_ADDR(num, off)  (((num) << 24) + (off))

#endif

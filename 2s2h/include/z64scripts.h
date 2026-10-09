#ifndef Z64SCRIPTS_H
#define Z64SCRIPTS_H

#include "PR/ultratypes.h"

// Macros to convert the time format used in the save struct into the format used in scripts (Schedule or Message)
#define SCRIPT_CONVERT_TIME(time) ((s32)((time) - 0x10000 / 360 * 90))

#define SCRIPT_CALC_TIME(hour, minute, dest, temp) \
    (temp) = (hour)*60.0f;                         \
    (temp) += (minute);                            \
    (dest) = (temp) * (0x10000 / 60 / 24.0f);      \
    (dest) = SCRIPT_CONVERT_TIME(dest)

#define SCRIPT_TIME(hour, minute) SCRIPT_CONVERT_TIME((((hour)*60.0f) + (minute)) * (0x10000 / 60 / 24.0f))

#define SCRIPT_TIME_NOW SCRIPT_CONVERT_TIME(CURRENT_TIME)

#define SCRIPT_PACK_16(h, l) (((h) << 8) | (l))

#define SCRIPT_UNPACK_8(v) (u8)(v)
#define SCRIPT_UNPACK_16(v) (u8)_SHIFTR(v, 8, 8), (u8)_SHIFTR(v, 0, 8)

#endif

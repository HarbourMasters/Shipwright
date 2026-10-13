#ifndef LIBC_STDINT_H
#define LIBC_STDINT_H

#include "PR/ultratypes.h"

// #region 2S2H [Port] These are defined by LUS
#if 0
typedef s32 intptr_t;
typedef u32 uintptr_t;
#endif
// #endregion
#if 0
#define INT8_MIN    (-0x80)
#define INT16_MIN   (-0x8000)
#define INT32_MIN   (-0x80000000)
#define INT64_MIN   (-0x8000000000000000)

#define INT8_MAX    0x7F
#define INT16_MAX   0x7FFF
#define INT32_MAX   0x7FFFFFFF
#define INT64_MAX   0x7FFFFFFFFFFFFFFF

#define UINT8_MAX   0xFF
#define UINT16_MAX  0xFFFF
#define UINT32_MAX  0xFFFFFFFF
#define UINT64_MAX  0xFFFFFFFFFFFFFFFF

#define INTPTR_MIN  (-0x80000000)
#define INTPTR_MAX  0x7FFFFFFF
#define UINTPTR_MAX 0xFFFFFFFF
#endif

#endif /* STDINT_H */

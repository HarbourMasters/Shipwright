#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

// clang-cl supports __attribute__ but does not define __GNUC__, and clang's own intrinsics headers
// rely on it - erasing it turns __m64 and friends into scalars.
#if !defined(__GNUC__) && !defined(__clang__) && !defined(__attribute__)
#define __attribute__(x)
#endif

#define UNUSED      __attribute__((unused))
#define FALLTHROUGH __attribute__((fallthrough))
#define NORETURN    __attribute__((noreturn))

#endif

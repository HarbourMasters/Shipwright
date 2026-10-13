#ifndef LIBC_STDDEF_H
#define LIBC_STDDEF_H

#include "PR/ultratypes.h"
#include <stddef.h>
#if 0
typedef s32 ptrdiff_t;

#ifdef __GNUC__
#define offsetof(structure, member) __builtin_offsetof (structure, member)
#else
#define offsetof(structure, member) ((size_t)&(((structure*)0)->member))
#endif
#endif
#endif /* STDDEF_H */

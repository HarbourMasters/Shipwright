#ifndef LIBC_STDLIB_H
#define LIBC_STDLIB_H

#include "libc/stddef.h"
#if 0
typedef struct {
    /* 0x0 */ int quot;
    /* 0x4 */ int rem;
} div_t;

typedef struct {
    /* 0x0 */ long quot;
    /* 0x4 */ long rem;
} ldiv_t;

typedef struct {
    /* 0x0 */ long long quot;
    /* 0x8 */ long long rem;
} lldiv_t;

typedef int ssize_t;

typedef long wchar_t;

ldiv_t ldiv(long numer, long denom);
lldiv_t lldiv(long long numer, long long denom);
#endif
#endif /* STDLIB_H */

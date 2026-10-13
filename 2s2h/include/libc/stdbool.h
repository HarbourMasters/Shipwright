#ifndef LIBC_STDBOOL_H
#define LIBC_STDBOOL_H
#include <stdbool.h>
#if 0
#define __bool_true_false_are_defined 1

#ifndef __cplusplus

#if (__STDC_VERSION__ >= 202311L)
// bool is a type in C23, do not define it
#elif (__STDC_VERSION__ >= 199901L)
#define bool    _Bool
#else
#define bool    int
#endif

#define false   0
#define true    1

#endif /* __cplusplus */
#endif
#endif /* STDBOOL */

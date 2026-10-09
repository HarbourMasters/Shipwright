#ifndef LIBC_ASSERT_H
#define LIBC_ASSERT_H

// Static/compile-time assertions

#if (__STDC_VERSION__ >= 202311L)
// static_assert is a keyword in C23, do not define it
#elif (__STDC_VERSION__ >= 201112L)
# define static_assert(cond, msg) _Static_assert(cond, msg)
#else
# ifndef GLUE
#  define GLUE(a, b) a##b
# endif
# ifndef GLUE2
#  define GLUE2(a, b) GLUE(a, b)
# endif

# define static_assert(cond, msg) typedef char GLUE2(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]
#endif

#endif

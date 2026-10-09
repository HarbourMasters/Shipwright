#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

// If not building with a modern GCC-like compiler then make any use of __attribute__ a no-op
#if (!defined(__GNUC__) && !defined(__clang__)) || defined(M2CTX) || defined(__sgi)
    #ifndef __attribute__
        #define __attribute__(x)
    #endif
#endif

#define UNUSED      __attribute__((unused))
#define FALLTHROUGH __attribute__((fallthrough))
#define NORETURN    __attribute__((noreturn))

// GCC and MSVC provide a way to force enable optimizations. Clang does not.
#if defined (__GNUC__) && !defined (__clang__)
#define FORCE_OPTIMIZE __attribute__ ((optimize("O2"))) 
#elif defined (_MSC_VER)
#define FORCE_OPTIMIZE _Pragma("optimize( \"g\", on )")
#else
#define FORCE_OPTIMIZE
#endif


#endif

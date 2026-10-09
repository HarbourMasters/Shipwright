#ifndef PR_ULTRATYPES_H
#define PR_ULTRATYPES_H

#include <libultraship/libultra/types.h>

#if 0
typedef signed char            s8;
typedef unsigned char          u8;
typedef signed short int       s16;
typedef unsigned short int     u16;
typedef signed long            s32;
typedef unsigned long          u32;
typedef signed long long int   s64;
typedef unsigned long long int u64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8  vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef float  f32;
typedef double f64;

#if defined(_MIPS_SZLONG) && (_MIPS_SZLONG == 64)
typedef unsigned long size_t;
#else
typedef unsigned int  size_t;
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif
#endif
// TODO: move this somewhere else
typedef void* TexturePtr;

#endif
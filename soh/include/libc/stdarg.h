#ifndef STDARG_H
#define STDARG_H

// When building with GCC, use the official vaarg macros to avoid warnings
// and possibly bad codegen.
#ifdef __GNUC__
#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_arg __builtin_va_arg
#define va_end __builtin_va_end
#else

typedef char* va_list;
#define _FP 1
#define _INT 0
#define _STRUCT 2

#define _VA_FP_SAVE_AREA 0x10
#define _VA_ALIGN(p, a) (((uintptr_t)(((char*)p) + ((a) > 4 ? (a) : 4) - 1)) & -((a) > 4 ? (a) : 4))
#define va_start(vp, parmN) (vp = ((va_list)&parmN + sizeof(parmN)))

#define __va_stack_arg(list, mode)                          \
    (                                                       \
        ((list) = (char*)_VA_ALIGN(list, __builtin_alignof(mode)) + \
                  _VA_ALIGN(sizeof(mode), 4)),              \
        (((char*)list) - (_VA_ALIGN(sizeof(mode), 4) - sizeof(mode))))

#define __va_double_arg(list, mode)                                                                    \
    (                                                                                                  \
        (((s32)list & 0x1) /* 1 byte aligned? */                                                      \
             ? (list = (char*)((s32)list + 7), (char*)((s32)list - 6 - _VA_FP_SAVE_AREA))          \
             : (((s32)list & 0x2) /* 2 byte aligned? */                                               \
                    ? (list = (char*)((s32)list + 10), (char*)((s32)list - 24 - _VA_FP_SAVE_AREA)) \
                    : __va_stack_arg(list, mode))))

#define va_arg(list, mode) ((mode*)(((__builtin_classof(mode) == _FP &&          \
                                       __builtin_alignof(mode) == sizeof(f64)) \
                                          ? __va_double_arg(list, mode)           \
                                          : __va_stack_arg(list, mode))))[-1]
#define va_end(__list)

#endif
#endif

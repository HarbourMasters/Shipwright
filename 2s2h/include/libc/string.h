#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include "libc/stddef.h"

#ifdef __sgi
const char* strchr(const char* s, int c);
size_t strlen(const char* s);
void* memcpy(void* s1, const void* s2, size_t n);
#endif

#endif

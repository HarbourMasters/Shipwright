#include "ultra64.h"
#include "libc/stdlib.h"
#include "libc/string.h"

const char* strchr(const char* s, int c) {
    const unsigned char ch = c;

    while (*s != ch) {
        if (*s == '\0') {
            return NULL;
        }
        s++;
    }

    return s;
}

size_t strlen(const char* s) {
    const char* sc = s;

    while (*sc != '\0') {
        sc++;
    }

    return (size_t)(sc - s);
}

void* memcpy(void* s1, const void* s2, size_t n) {
    unsigned char* su1 = (unsigned char*)s1;
    const unsigned char* su2 = (const unsigned char*)s2;

    while (n > 0) {
        *su1 = *su2;
        su1++;
        su2++;
        n--;
    }

    return s1;
}

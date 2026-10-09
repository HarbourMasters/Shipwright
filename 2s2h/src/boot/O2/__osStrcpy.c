#include "global.h"

char* __osStrcpy(char* dst, const char* src) {
    char* _dst = dst;

    while (*src != '\0') {
        *_dst++ = *src++;
    }
    *_dst = '\0';

    return dst;
}

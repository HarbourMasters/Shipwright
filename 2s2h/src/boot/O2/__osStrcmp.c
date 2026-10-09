#include "global.h"

s32 __osStrcmp(const char* str1, const char* str2) {
    char c1;
    char c2;

    do {
        c1 = *str1++;
        c2 = *str2++;
        if (c1 != c2) {
            return c1 - c2;
        }
    } while (c1);

    return 0;
}

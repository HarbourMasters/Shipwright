#include "global.h"

void* __osMemcpy(void* dst, void* src, size_t size) {
    u8* _dst = dst;
    u8* _src = src;
    register s32 rem;

    if (_dst == _src) {
        return dst;
    }
    if (_dst < _src) {
        for (rem = size--; rem != 0; rem = size--) {
            *_dst++ = *_src++;
        }
    } else {
        _dst += size - 1;
        _src += size - 1;
        for (rem = size--; rem != 0; rem = size--) {
            *_dst-- = *_src--;
        }
    }
    return dst;
}

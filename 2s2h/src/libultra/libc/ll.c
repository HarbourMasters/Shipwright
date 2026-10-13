#include "ultra64.h"

long long __ull_rshift(unsigned long long left, unsigned long long right) {
    return left >> right;
}

unsigned long long __ull_rem(unsigned long long left, unsigned long long right) {
    return left % right;
}

unsigned long long __ull_div(unsigned long long left, unsigned long long right) {
    return left / right;
}

long long __ll_lshift(long long left, long long right) {
    return left << right;
}

long long __ll_rem(long long left, unsigned long long right) {
    return left % right;
}

long long __ll_div(long long left, long long right) {
    return left / right;
}

long long __ll_mul(long long left, long long right) {
    return left * right;
}

void __ull_divremi(unsigned long long* quotient, unsigned long long* remainder, unsigned long long dividend,
                   unsigned short divisor) {
    *quotient = dividend / divisor;
    *remainder = dividend % divisor;
}

long long __ll_mod(long long left, long long right) {
    long long rem = left % right;
    if (((rem < 0) && (right > 0)) || ((rem > 0) && (right < 0))) {
        rem += right;
    }
    return rem;
}

long long __ll_rshift(long long left, long long right) {
    return left >> right;
}

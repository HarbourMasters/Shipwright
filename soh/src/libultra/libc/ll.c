#include "global.h"

s64 __ull_rshift(u64 l, s64 r) {
    return l >> r;
}

u64 __ull_rem(u64 l, u64 r) {
    return l % r;
}

u64 __ull_div(u64 l, u64 r) {
    return l / r;
}

s64 __ll_lshift(s64 l, s64 r) {
    return l << r;
}

s64 __ll_rem(s64 l, u64 r) {
    return l % r;
}

s64 __ll_div(s64 l, s64 r) {
    return l / r;
}

s64 __ll_mul(s64 l, s64 r) {
    return l * r;
}

void __ull_divremi(u64* quotient, u64* remainder, u64 dividend, u16 divisor) {
    *quotient = dividend / divisor;
    *remainder = dividend % divisor;
}

s64 __ll_mod(s64 l, s64 r) {
    s64 remainder = l % r;

    if (((remainder < 0) && (r > 0)) || ((remainder > 0) && (r < 0))) {
        remainder += r;
    }
    return remainder;
}

s64 __ll_rshift(s64 l, s64 r) {
    return l >> r;
}

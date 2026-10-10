/**
 * Maths library: two factorials, integer power, wrappers for libultra's sins and coss (the main ones used), and some
 * random functions moved from OoT's z_actor
 */

#include "global.h"

static f32 sFactorialTbl[] = {
    1.0f, 1.0f, 2.0f, 6.0f, 24.0f, 120.0f, 720.0f, 5040.0f, 40320.0f, 362880.0f, 3628800.0f, 39916800.0f, 479001600.0f,
};

/**
 * Takes a float, returns the factorial of it(s trunctation), iteratively
 * Unused
 *
 * @remark original name: "ffact"
 */
f32 Math_FactorialF(f32 n) {
    f32 ret = 1.0f;
    s32 i;

    //! @bug No check for negative argument. Will return 1.0f if the argument truncates to a negative int.
    for (i = n; i > 1; i--) {
        ret *= i;
    }
    return ret;
}

/**
 * Takes an int and returns its factorial as a float. Uses the lookup table for n <= 12.
 * Unused
 *
 * @remark original name: "ifact"
 */
f32 Math_Factorial(s32 n) {
    f32 ret;
    s32 i;

    //! @bug No check for negative argument. Will read the array out-of-bounds if the argument is negative.
    //! (The OoT version does an unsigned check instead, which will return sFactorialTbl[12] for a negative argument.)
    if (n >= ARRAY_COUNT(sFactorialTbl)) {
        ret = sFactorialTbl[12];
        for (i = 13; i <= n; i++) {
            ret *= i;
        }
    } else {
        ret = sFactorialTbl[n];
    }
    return ret;
}

/**
 * Returns base^{exp} if exp is nonnegative and 1.0f otherwise.
 * Unused
 *
 * @remark original name: "powi"
 */
f32 Math_PowF(f32 base, s32 exp) {
    f32 ret = 1.0f;

    while (exp > 0) {
        exp--;
        ret *= base;
    }

    return ret;
}

/**
 * Takes an angle in radians and returns the sine.
 *
 * @remark original name: "sinf_table"
 */
f32 Math_SinF(f32 rad) {
    return sins(RAD_TO_BINANG(rad)) * SHT_MINV;
}

/**
 * Takes an angle in radians and returns the cosine.
 *
 * @remark original name: "cosf_table"
 */
f32 Math_CosF(f32 rad) {
    return coss(RAD_TO_BINANG(rad)) * SHT_MINV;
}

/**
 * Returns a pseudo-random floating-point number between 0.0f and scale. Originally in z_actor in OoT.
 */
f32 Rand_ZeroFloat(f32 scale) {
    return Rand_ZeroOne() * scale;
}

/**
 * Returns a pseudo-random floating-point number between (- scale / 2) and (scale / 2). Originally in z_actor in OoT.
 */
f32 Rand_CenteredFloat(f32 scale) {
    return Rand_Centered() * scale;
}

/**
 * MathF library
 * Contains tangent function, wrappers for a number of the handwritten functions in fp, and a suite of arctangents
 */
#include "global.h"
#include "fixed_point.h"

s32 gUseAtanContFrac;

/**
 * Tangent function computed using libultra sinf and cosf
 */
f32 Math_FTanF(f32 x) {
    return sinf(x) / cosf(x);
}

// Unused
f32 Math_FFloorF(f32 x) {
    return floorf(x);
}

// Unused
f32 Math_FCeilF(f32 x) {
    return ceilf(x);
}

// Unused
f32 Math_FRoundF(f32 x) {
    return roundf(x);
}

// Unused
f32 Math_FTruncF(f32 x) {
    return truncf(x);
}

f32 Math_FNearbyIntF(f32 x) {
    return nearbyintf(x);
}

/**
 * Arctangent approximation using a Maclaurin series [https://mathworld.wolfram.com/MaclaurinSeries.html]
 * (one quadrant, i.e. |x| < 1)
 */
f32 Math_FAtanTaylorQF(f32 x) {
    // Coefficients of Maclaurin series of arctangent
    static const f32 coeffs[] = {
        -1.0f / 3, +1.0f / 5, -1.0f / 7, +1.0f / 9, -1.0f / 11, +1.0f / 13, -1.0f / 15, +1.0f / 17, 0.0f,
    };

    f32 poly = x;
    f32 sq = SQ(x);
    f32 exp = x * sq;
    const f32* c = coeffs;
    f32 term;

    // Calculate the series until adding more terms does not change the float
    while (true) {
        term = *c++ * exp;
        if (poly + term == poly) {
            break;
        }
        poly += term;
        exp *= sq;
    }

    return poly;
}

/**
 * Extends previous arctangent function to the rest of the real numbers.
 * Uses the formulae arctan(x) = pi/2 - arctan(1/x)
 * and arctan(x) = pi/4 - arctan( (1-x)/(1+x) )
 * to extend the range in which the series computed by Math_FAtanTaylorQF is a good approximation
 */
f32 Math_FAtanTaylorF(f32 x) {
    f32 t;
    f32 q;

    if (x > 0.0f) {
        t = x;
    } else if (x < 0.0f) {
        t = -x;
    } else if (x == 0.0f) {
        return 0.0f;
    } else {
        return qNaN0x10000;
    }

    if (t <= M_SQRT2f - 1.0f) {
        return Math_FAtanTaylorQF(x);
    }

    if (t >= M_SQRT2f + 1.0f) {
        q = M_PIf / 2 - Math_FAtanTaylorQF(1.0f / t);
    } else { // in the interval (\sqrt{2} - 1, \sqrt{2} + 1)
        q = M_PIf / 4 - Math_FAtanTaylorQF((1.0f - t) / (1.0f + t));
    }

    if (x > 0.0f) {
        return q;
    } else {
        return -q;
    }
}

/**
 * Arctangent approximation using a continued fraction
 * Cf. https://en.wikipedia.org/wiki/Gauss%27s_continued_fraction#The_series_2F1_2 ,
 * https://dlmf.nist.gov/4.25#E4
 */
f32 Math_FAtanContFracF(f32 x) {
    s32 sector;
    f32 z;
    f32 conv;
    f32 sq;
    s32 i;

    if (x >= -1.0f && x <= 1.0f) {
        sector = 0;
    } else if (x > 1.0f) {
        sector = 1;
        x = 1.0f / x;
    } else if (x < -1.0f) {
        sector = -1;
        x = 1.0f / x;
    } else {
        return qNaN0x10000;
    }

    // Builds the continued fraction from the innermost fraction out
    sq = SQ(x);
    conv = 0.0f;
    z = 8.0f;
    for (i = 8; i != 0; i--) {
        conv = SQ(z) * sq / (2.0f * z + 1.0f + conv);
        z -= 1.0f;
    }
    conv = x / (1.0f + conv);

    if (sector == 0) {
        return conv;
    } else if (sector > 0) {
        return M_PIf / 2 - conv;
    } else {
        return -M_PIf / 2 - conv;
    }
}

/**
 * Single-argument arctangent, only used by the two-argument function.
 * Nothing else sets the bss variable gUseAtanContFrac, so the Maclaurin series is always used
 */
f32 Math_FAtanF(f32 x) {
    if (!gUseAtanContFrac) {
        return Math_FAtanTaylorF(x);
    } else {
        return Math_FAtanContFracF(x);
    }
}

/**
 * Main two-argument arctangent function
 */
f32 Math_FAtan2F(f32 y, f32 x) {
    if (x == 0.0f) {
        if (y == 0.0f) {
            return 0.0f;
        } else if (y > 0.0f) {
            return M_PIf / 2;
        } else if (y < 0.0f) {
            return -M_PIf / 2;
        } else {
            return qNaN0x10000;
        }
    } else if (x >= 0.0f) {
        return Math_FAtanF(y / x);
    } else if (y < 0.0f) {
        return Math_FAtanF(y / x) - M_PIf;
    } else {
        return M_PIf - Math_FAtanF(-(y / x));
    }
}

f32 Math_FAsinF(f32 x) {
    return Math_FAtan2F(x, sqrtf(1.0f - SQ(x)));
}

f32 Math_FAcosF(f32 x) {
    return M_PIf / 2 - Math_FAsinF(x);
}

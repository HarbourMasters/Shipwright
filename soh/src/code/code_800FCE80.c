#include "global.h"
#include "fp.h"

s32 gUseAtanContFrac;

f32 Math_FTanF(f32 x) {
    f32 sin = sinf(x);
    f32 cos = cosf(x);

    return sin / cos;
}

f32 Math_FFloorF(f32 x) {
    return floorf(x);
}

f32 Math_FCeilF(f32 x) {
    return ceilf(x);
}

f32 Math_FRoundF(f32 x) {
    return roundf(x);
}

f32 Math_FTruncF(f32 x) {
    return truncf(x);
}

f32 Math_FNearbyIntF(f32 x) {
    return nearbyintf(x);
}

/* Arctangent approximation using a Taylor series (one quadrant) */
f32 Math_FAtanTaylorQF(f32 x) {
    static const f32 coeffs[] = {
        -1.0f / 3, +1.0f / 5, -1.0f / 7, +1.0f / 9, -1.0f / 11, +1.0f / 13, -1.0f / 15, +1.0f / 17, 0.0f,
    };

    f32 poly = x;
    f32 sq = SQ(x);
    f32 exp = x * sq;
    const f32* c = coeffs;
    f32 term;

    while (1) {
        term = *c++ * exp;
        if (poly + term == poly) {
            break;
        }
        poly = poly + term;
        exp = exp * sq;
    }

    return poly;
}

/* Ditto for two quadrants */
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

    if (t <= M_SQRT2 - 1.0f) {
        return Math_FAtanTaylorQF(x);
    }

    if (t >= M_SQRT2 + 1.0f) {
        q = M_PI / 2 - Math_FAtanTaylorQF(1.0f / t);
    } else {
        q = M_PI / 4 - Math_FAtanTaylorQF((1.0f - t) / (1.0f + t));
    }

    if (x > 0.0f) {
        return q;
    } else {
        return -q;
    }
}

/* Arctangent approximation using a continued fraction */
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
        return M_PI / 2 - conv;
    } else {
        return -M_PI / 2 - conv;
    }
}

f32 Math_FAtanF(f32 x) {
    if (!gUseAtanContFrac) {
        return Math_FAtanTaylorF(x);
    } else {
        return Math_FAtanContFracF(x);
    }
}

f32 Math_FAtan2F(f32 y, f32 x) {
    if (x == 0.0f) {
        if (y == 0.0f) {
            return 0.0f;
        } else if (y > 0.0f) {
            return M_PI / 2;
        } else if (y < 0.0f) {
            return -M_PI / 2;
        } else {
            return qNaN0x10000;
        }
    } else if (x >= 0.0f) {
        return Math_FAtanF(y / x);
    } else if (y < 0.0f) {
        return Math_FAtanF(y / x) - M_PI;
    } else {
        return M_PI - Math_FAtanF(-(y / x));
    }
}

f32 Math_FAsinF(f32 x) {
    return Math_FAtan2F(x, sqrtf(1.0f - SQ(x)));
}

f32 Math_FAcosF(f32 x) {
    return M_PI / 2 - Math_FAsinF(x);
}

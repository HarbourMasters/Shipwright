#include "global.h"
#include "ultra64.h"

static const du P[] = {
    { 0x3FF00000, 0x00000000 }, { 0xBFC55554, 0xBC83656D }, { 0x3F8110ED, 0x3804C2A0 },
    { 0xBF29F6FF, 0xEEA56814 }, { 0x3EC5DBDF, 0x0E314BFE },
};

static const du rpi = { 0x3FD45F30, 0x6DC9C883 };

static const du pihi = { 0x400921FB, 0x50000000 };

static const du pilo = { 0x3E6110B4, 0x611A6263 };

static const fu zero = { 0x00000000 };

f32 sinf(f32 x) {
    f64 dx;
    f64 xSq;
    f64 polyApprox;
    f64 dn;
    s32 n;
    f64 result;
    s32 ix = *(s32*)&x;
    s32 xpt = (ix >> 22);

    xpt &= 0x1FF;

    if (xpt < 0xFF) {
        dx = x;

        if (xpt >= 0xE6) {
            xSq = SQ(dx);
            polyApprox = ((P[4].d * xSq + P[3].d) * xSq + P[2].d) * xSq + P[1].d;
            result = dx + (dx * xSq) * polyApprox;
            return (f32)result;
        }
        return x;
    }

    if (xpt < 0x136) {
        dx = x;
        dn = dx * rpi.d;
        n = ROUND(dn);
        dn = n;

        dx -= dn * pihi.d;
        dx -= dn * pilo.d;
        xSq = SQ(dx);

        polyApprox = ((P[4].d * xSq + P[3].d) * xSq + P[2].d) * xSq + P[1].d;
        result = dx + (dx * xSq) * polyApprox;

        if (!(n & 1)) {
            return (f32)result;
        }
        return -(f32)result;
    }

    if (x != x) {
        return __libm_qnan_f;
    }
    return zero.f;
}

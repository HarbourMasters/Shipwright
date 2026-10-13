/**
 * @file z_fcurve_data.c
 * @brief Interpolation functions for use with Curve SkelAnime
 */
#include "global.h"
#include "z64curve.h"

#define FCURVE_INTERP_CUBIC 0  // Interpolate using a Hermite cubic spline
#define FCURVE_INTERP_NONE 1   // Return the value at the left endpoint instead of interpolating
#define FCURVE_INTERP_LINEAR 2 // Interpolate linearly

/**
 * Hermite cubic spline interpolation between two endpoints, a,b. More information available at
 * https://en.wikipedia.org/wiki/Cubic_Hermite_spline
 *
 * @param t interpolation parameter rescaled to lie in [0,1], (x-a)/(b-a)
 * @param interval distance (b-a) between the endpoints
 * @param y0 p(a)
 * @param y1 p(b)
 * @param m0 p'(a)
 * @param m1 p'(b)
 * @return f32 p(t), value of the cubic interpolating polynomial
 */
f32 Curve_CubicHermiteSpline(f32 t, f32 interval, f32 y0, f32 y1, f32 m0, f32 m1) {
    f32 t2 = t * t;
    f32 t3 = t2 * t;
    f32 t3x2 = t3 * 2.0f;
    f32 t2x3 = t2 * 3.0f;

    // Hermite basis cubics h_{ij} satisfy h_{ij}^{(j)}(i) = 1, the other three values being 0
    f32 h00 = t3x2 - t2x3 + 1.0f; // h_{00}(t) = 2t^3 - 3t^2 + 1
    f32 h01 = t2x3 - t3x2;        // h_{01}(t) = 3t^2 - 2t^3
    f32 h10 = t3 - t2 * 2.0f + t; // h_{10}(t) = t^3 - 2t^2 + t
    f32 h11 = t3 - t2;            // h_{11}(t) = t^3 - t^2

    f32 ret = h00 * y0;

    ret += h01 * y1;
    ret += h10 * m0 * interval;
    ret += h11 * m1 * interval;

    return ret;
}

/**
 * Interpolates based on an array of CurveInterpKnot.
 *
 * @param x point at which to interpolate.
 * @param knots Beginning of CurveInterpKnot array to use.
 * @param knotCount number of knots to read from the array.
 * @return f32 interpolated value
 */
f32 Curve_Interpolate(f32 x, CurveInterpKnot* knots, s32 knotCount) {
    // If outside the entire interpolation interval, return the value at the near endpoint.
    if (x <= knots[0].abscissa) {
        return knots[0].ordinate;
    } else if (x >= knots[knotCount - 1].abscissa) {
        return knots[knotCount - 1].ordinate;
    } else {
        s32 cur;

        for (cur = 0;; cur++) {
            s32 next = cur + 1;

            // Find the subinterval in which x lies
            if (x < knots[next].abscissa) {
                if (knots[cur].flags & FCURVE_INTERP_NONE) {
                    // No interpolation
                    return knots[cur].ordinate;
                } else if (knots[cur].flags & FCURVE_INTERP_LINEAR) {
                    // Linear interpolation
                    return knots[cur].ordinate +
                           ((x - (f32)knots[cur].abscissa) / ((f32)knots[next].abscissa - (f32)knots[cur].abscissa)) *
                               (knots[next].ordinate - knots[cur].ordinate);
                } else {
                    // Cubic interpolation
                    f32 diff = (f32)knots[next].abscissa - (f32)knots[cur].abscissa;
                    f32 t = (x - (f32)knots[cur].abscissa) / ((f32)knots[next].abscissa - (f32)knots[cur].abscissa);

                    return Curve_CubicHermiteSpline(t, diff * (1.0f / 30.0f), knots[cur].ordinate, knots[next].ordinate,
                                                    knots[cur].rightGradient, knots[next].leftGradient);
                }
            }
        }
    }
}

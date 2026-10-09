/**
 * @file su_mtx.c
 * @brief "Fast" functions for constructing RSP-compatible matrices directly
 *
 * The three functions in this file construct scaling, rotation, and translation matrices, and combinations thereof. The
 * intention appears to be to exploit the peculiar structure of the RSP's fixed-point matrix format, which we recall is
 *
 *     typedef long int Mtx_t[4][4];
 *     typedef union {
 *         Mtx_t m;
 *         struct {
 *             u16 intPart[4][4];
 *             u16 fracPart[4][4];
 *         };
 *         long long int forc_structure_alignment;
 *     } Mtx; // size = 0x40
 *
 * This means it can be written to as either words or two sets of shorts; the latter is the correct format for
 * interpreting it the way the RSP does, but the first is likely faster to access and write to. The functions save
 * writing halves by
 *
 * - writing a word 0 if two adjacent elements are 0 in a convenient place, or 1 if they are 0 and 1,
 * - writing a word using a shift and overwriting the dirty half afterwards.
 *
 * Examples of both of these are seen in Mtx_SetTranslateScaleMtx(); we explain there in full detail due to the
 * function's relative simplicity. The other two functions we merely point out the interesting parts rather than
 * explaining the whole process.
 *
 * We use the following notation throughout: round brackets denote the word-sized elements, square the pairs they cover.
 * The integer and fractional parts of a fixed-point number are denoted by i and f respectively, and these letters are
 * also used to represent which half each of the pairs belongs to.
 *
 *     (00) [i00,i01], (01) [i02,i03]
 *     (02) [i10,i11], (03) [i12,i13]
 *     (10) [i20,i21], (11) [i22,i23]
 *     (12) [i30,i31], (13) [i32,i33]
 *     (20) [f00,f01], (21) [f02,f03]
 *     (22) [f10,f11], (23) [f12,f13]
 *     (30) [f20,f21], (31) [f22,f23]
 *     (32) [f30,f31], (33) [f32,f33]
 *
 * We keep the RSP's column-major convention; this file is confusing enough without attempting to transpose everything.
 * Bear in mind therefore that we are acting on *row vectors* on the *right*.
 *
 * @warning The behaviour of the output of the functions in this file is undefined in C89, since both members of the
 * union are used to set the mtxs. (C99+ allow type-punning, but the behaviour is still implementation-defined because
 * it relies on the storage being big-endian.)
 *
 * @remark Name inferred from shared Animal Forest functions, meaning of "su" is unclear.
 */

#include "global.h"

/**
 * Constructs a matrix \$f ST \$f, i.e. a scaling \$f S \$f followed by a translation \$f T \$f.
 *
 * The final result is
 *
 * \f[
 *  \begin{bmatrix}
 *      s_x & 0 & 0 & 0 \\
 *      0 & s_y & 0 & 0 \\
 *      0 & 0 & s_z & 0 \\
 *      t_x & t_y & t_z & 1
 *  \end{bmatrix}
 * \f]
 *
 * @param[out] mtx Fixed-point matrix pointer to output to
 * @param[in] scaleX Scale in the X direction
 * @param[in] scaleY Scale in the Y direction
 * @param[in] scaleZ Scale in the Z direction
 * @param[in] translateX X component of translation
 * @param[in] translateY Y component of translation
 * @param[in] translateZ Z component of translation
 *
 * @remark Original name: "suMtxMakeTS"
 */
void Mtx_SetTranslateScaleMtx(Mtx* mtx, f32 scaleX, f32 scaleY, f32 scaleZ, f32 translateX, f32 translateY,
                              f32 translateZ) {
    // #region 2S2H [Port] For compatibility with modern systems this has been changed to use guMtxF2L
    MtxF mtxf = {
        { { scaleX, 0, 0, 0 }, { 0, scaleY, 0, 0 }, { 0, 0, scaleZ, 0 }, { translateX, translateY, translateZ, 1 } }
    };
    guMtxF2L(&mtxf, mtx);
}

// Unused
/**
 * Should create an axis-angle rotation matrix.
 *
 * @note The axis vector is expected to be normalised.
 *
 * The result should be:
 *
 * \f[
 * \begin{bmatrix}
 *         (1-a_x^2) \cos \theta + a_x^2 & a_x a_y (1 - \cos \theta) + a_z \sin \theta & a_z a_x (1 - \cos \theta) - a_y
 *     \sin \theta & 0 \\
 *         a_x a_y (1 - \cos \theta) - a_z \sin \theta & (1-a_y^2) \cos \theta + a_y^2 & a_y a_z (1 - \cos \theta) + a_x
 *     \sin \theta & 0 \\
 *         a_z a_x (1 - \cos \theta) + a_y \sin \theta & a_y a_z (1 - \cos \theta) - a_x \sin \theta & (1-a_z^2) \cos
 *     \theta
 *     + a_z^2 & 0 \\
 *     t_x & t_y & t_z & 1
 * \end{bmatrix}
 * \f]
 *
 * @warning There is a significant bug in this function, which means it does not actually produce this correct rotation
 * matrix; see inline for details.
 *
 * @param[out] mtx Fixed-point matrix pointer to output to
 * @param[in] angle angle to rotate about axis
 * @param[in] axisX X component of axis to rotate about
 * @param[in] axisY Y component of axis to rotate about
 * @param[in] axisZ Z component of axis to rotate about
 *
 * @remark Original name: probably something like "suMtxMakeR" or "suMtxMakeRotateVector"
 */
void Mtx_SetRotationMtx(Mtx* mtx, s32 angle, f32 axisX, f32 axisY, f32 axisZ) {
    //! FAKE? The somewhat peculiar distribution of temps in this function seems necessary to match?
    f32 tempX;
    f32 tempZ;
    f32 tempY;
    f32 sin = Math_SinS(angle);
    f32 cos = Math_CosS(angle);
    f32 tempXX;
    f32 tempYY;
    f32 tempZZ;

    s32 fixedPoint;

    mtx->m[1][2] = 0; // [i30, i31] == [0, 0]
    mtx->m[1][3] = 1; // [i32, i33] == [0, 1]
    mtx->m[3][2] = 0; // [f30, f31] == [0, 0]
    mtx->m[3][3] = 0; // [f31, f32] == [0, 0]

    tempXX = axisX * axisX;
    tempX = (axisY * axisZ) * (1.0f - cos);

    // i00 and f00
    fixedPoint = (((1.0f - tempXX) * cos) + tempXX) * 0x10000;
    mtx->intPart[0][0] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[0][0] = fixedPoint & 0xFFFF;

    tempZ = ((axisX * axisY) * (1.0f - cos));
    tempZZ = axisZ * axisZ;

    // i21 and f21
    fixedPoint = (tempX - (axisX * sin)) * 0x10000;
    mtx->intPart[2][1] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[2][1] = fixedPoint & 0xFFFF;

    tempY = (axisZ * axisX) * (1.0f - cos);
    tempYY = axisY * axisY;

    // [i12, i13] and [f12, f13]
    fixedPoint = (tempX + (axisX * sin)) * 0x10000;
    mtx->m[0][3] = fixedPoint; // i13 dirty
    mtx->intPart[1][3] = 0;    // clean i13
    mtx->m[2][3] = fixedPoint << 0x10;

    // i11 and f11
    fixedPoint = ((((1.0f - tempYY) * cos) + tempYY) * 0x10000);
    mtx->intPart[1][1] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[1][1] = fixedPoint & 0xFFFF;

    //! @bug The sign of the `axisY * sin` term is wrong on the next two elements (i/f20 and i/f02), which causes the
    //! matrix to not be a rotation matrix (and indeed not even necessarily invertible).
    // i20 and f20
    fixedPoint = (tempY - (axisY * sin)) * 0x10000;
    mtx->intPart[2][0] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[2][0] = fixedPoint & 0xFFFF;

    // [i02, i03] and [f02, f03]
    fixedPoint = (tempY + (axisY * sin)) * 0x10000;
    mtx->m[0][1] = fixedPoint; // i03 dirty
    mtx->intPart[0][3] = 0;    // clean i03
    mtx->m[2][1] = fixedPoint << 0x10;

    // [i22, i23] and [f22, f23]
    fixedPoint = ((((1.0f - tempZZ) * cos) + tempZZ) * 0x10000);
    mtx->m[1][1] = fixedPoint; // i23 dirty
    mtx->intPart[2][3] = 0;    // clean i23
    mtx->m[3][1] = fixedPoint << 0x10;

    // i10 and f10
    fixedPoint = (tempZ - axisZ * sin) * 0x10000;
    mtx->intPart[1][0] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[1][0] = fixedPoint & 0xFFFF;

    // i01 and f01
    fixedPoint = (tempZ + axisZ * sin) * 0x10000;
    mtx->intPart[0][1] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[0][1] = fixedPoint & 0xFFFF;
}

/**
 * Creates a general scale, rotation, translation matrix.
 *
 * @note The axis vector is expected to be normalised.
 *
 * The transformations are applied in the order scale, rotate, translate: \$f SRT \$f
 *
 * The result should be
 * \f[
 * \begin{bmatrix}
 *     s_x((1-a_x^2) \cos \theta + a_x^2) & s_x(a_x a_y (1 - \cos \theta) + a_z \sin \theta) & s_x(a_z a_x (1 - \cos
 *        \theta) - a_y \sin \theta & 0) \\
 *     s_y(a_x a_y (1 - \cos \theta) - a_z \sin \theta) & s_y((1-a_y^2) \cos \theta + a_y^2) & s_z(a_y a_z (1 - \cos
 *        \theta) + a_x \sin \theta) & 0 \\
 *     s_z(a_z a_x (1 - \cos \theta) + a_y \sin \theta) & s_z(a_y a_z (1 - \cos \theta) - a_x \sin \theta) &
 *        s_z((1-a_z^2) \cos \theta + a_z^2) & 0 \\
 *     0 & 0 & 0 & 1
 * \end{bmatrix} .
 * \f]
 *
 * @warning There is a significant bug in this function, which means it does not actually produce a proper rotation
 * matrix; see inline for details.
 *
 * @param[out] mtx Fixed-point matrix pointer to output to
 * @param[in] scaleX Scale in the X direction
 * @param[in] scaleY Scale in the Y direction
 * @param[in] scaleZ Scale in the Z direction
 * @param[in] angle angle to rotate about axis
 * @param[in] axisX X component of axis to rotate about
 * @param[in] axisY Y component of axis to rotate about
 * @param[in] axisZ Z component of axis to rotate about
 * @param[in] translateX X component of translation
 * @param[in] translateY Y component of translation
 * @param[in] translateZ Z component of translation
 *
 * @remark Original name: probably something like "suMtxMakeSRT", although Animal Forest function is a Tait-Bryan
 * rotation rather than axis-angle.
 */
void Mtx_SetTranslationRotationScaleMtx(Mtx* mtx, f32 scaleX, f32 scaleY, f32 scaleZ, s32 angle, f32 axisX, f32 axisY,
                                        f32 axisZ, f32 translateX, f32 translateY, f32 translateZ) {
    f32 tempX;
    f32 tempY;
    f32 tempZ;
    f32 sin = Math_SinS(angle);
    f32 cos = Math_CosS(angle);
    f32 tempXX = axisX * axisX;
    f32 tempYY = axisY * axisY;
    f32 tempZZ = axisZ * axisZ;
    s32 fixedPoint;

    tempX = axisY * axisZ * (1.0f - cos);

    // i00 and f00
    fixedPoint = (tempXX + (1.0f - tempXX) * cos) * scaleX * 0x10000;
    mtx->intPart[0][0] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[0][0] = fixedPoint & 0xFFFF;

    tempY = axisZ * axisX * (1.0f - cos);

    // i21 and f21
    fixedPoint = (tempX - axisX * sin) * scaleZ * 0x10000;
    mtx->intPart[2][1] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[2][1] = fixedPoint & 0xFFFF;

    tempZ = axisX * axisY * (1.0f - cos);

    // [i12, i13] and [f12, f13]
    fixedPoint = (tempX + axisX * sin) * scaleY * 0x10000;
    mtx->m[0][3] = fixedPoint; // i13 dirty
    mtx->intPart[1][3] = 0;    // clean i13
    mtx->m[2][3] = fixedPoint << 0x10;

    // i11 and f11
    fixedPoint = (tempYY + (1.0f - tempYY) * cos) * scaleY * 0x10000;
    mtx->intPart[1][1] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[1][1] = fixedPoint & 0xFFFF;

    //! @bug The sign of the `axisY * sin` term is wrong on the next two elements (i/f20 and i/f02), which causes the
    //! matrix to not be a rotation matrix (and indeed not even necessarily invertible).
    // i20, f20
    fixedPoint = (tempY - axisY * sin) * scaleZ * 0x10000;
    mtx->intPart[2][0] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[2][0] = fixedPoint & 0xFFFF;

    // [i02, i03] and [f02, f03]
    fixedPoint = (tempY + axisY * sin) * scaleX * 0x10000;
    mtx->m[0][1] = fixedPoint;         // [i03, i03], i03 dirty
    mtx->intPart[0][3] = 0;            // clean i03
    mtx->m[2][1] = fixedPoint << 0x10; // [f02, f03]

    // [i22, i23] and [f22, f23]
    fixedPoint = (tempZZ + (1.0f - tempZZ) * cos) * scaleZ * 0x10000;
    mtx->m[1][1] = fixedPoint;
    mtx->intPart[2][3] = 0;
    mtx->m[3][1] = fixedPoint << 0x10;

    // i10 and f10
    fixedPoint = (tempZ - axisZ * sin) * scaleY * 0x10000;
    mtx->intPart[1][0] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[1][0] = fixedPoint & 0xFFFF;

    // i01 and f01
    fixedPoint = (tempZ + axisZ * sin) * scaleX * 0x10000;
    mtx->intPart[0][1] = ((u32)fixedPoint >> 0x10) & 0xFFFF;
    mtx->fracPart[0][1] = fixedPoint & 0xFFFF;

    // [i30, i31] and f30
    fixedPoint = translateX * 0x10000;
    mtx->m[1][2] = fixedPoint;                 // [i30, i31], i31 dirty
    mtx->fracPart[3][0] = fixedPoint & 0xFFFF; // f30

    // i31 and f31
    fixedPoint = translateY * 0x10000;
    mtx->intPart[3][1] = ((u32)fixedPoint >> 0x10) & 0xFFFF; // overwrite i31
    mtx->fracPart[3][1] = fixedPoint & 0xFFFF;               // overwrite f31

    // [i32, i33] and [f32, f33]
    fixedPoint = translateZ * 0x10000;
    mtx->m[1][3] = fixedPoint;           // [i32, i33]
    mtx->intPart[3][3] = 1;              // clean i33
    mtx->m[3][3] = (fixedPoint << 0x10); // [f32, f33]
}

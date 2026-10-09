/**
 * @file sys_matrix.c
 * @brief: Matrix system that mostly uses a matrix stack, and concerns affine transformations.
 *
 * @note The RSP matrix format (and hence the `MtxF` format) is column-major: vectors are presumed to be row vectors,
 * and matrices as a column of row vectors. This means that, for example, a translation matrix
 * \f[
 *  \begin{pmatrix}
 *      1 & 0 & 0 & x \\
 *      0 & 1 & 0 & y \\
 *      0 & 0 & 1 & z \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix}
 * \f]
 * will be stored as
 *
 *     { { 1, 0, 0, 0 },
 *       { 0, 1, 0, 0 },
 *       { 0, 0, 1, 0 },
 *       { x, y, z, 1 }, }
 *
 * @note As such, we label the elements in column-major order so we can follow the same conventions for multiplying
 * matrices as the rest of the world, i.e. that \f$ [AB]_{ij} = \sum_k A_{ik} B_{kj} \f$.
 *
 * This file is primarily concerned with matrices representing affine transformations, implemented using an augmented
 * matrix formalism,
 *
 * \f[
 *  \begin{pmatrix}
 *      A & b \\
 *      0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where \f$ A \f$ is a \f$ 3 \times 3 \f$ matrix (the *linear part*) and \f$ b \f$ a \f$ 3 \times 1 \f$ matrix, i.e. a
 * 3D vector (the *translation part*), and most of the functions assume that the matrices have this form.
 *
 * Throughout this file, `mode` indicates whether to multiply the matrix on top of the stack by the new construction
 * (APPLY), or to just overwrite it (NEW).
 */

#include "prevent_bss_reordering.h"
#include "global.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

/* data */
#define qs1616(e) ((s32)((e)*0x00010000))

#define IPART(x) ((qs1616(x) >> 16) & 0xFFFF)
#define FPART(x) (qs1616(x) & 0xFFFF)

#define gdSPDefMtx(xx, yx, zx, wx, xy, yy, zy, wy, xz, yz, zz, wz, xw, yw, zw, ww)                                 \
    {                                                                                                              \
        {                                                                                                          \
            (IPART(xx) << 0x10) | IPART(xy), (IPART(xz) << 0x10) | IPART(xw), (IPART(yx) << 0x10) | IPART(yy),     \
                (IPART(yz) << 0x10) | IPART(yw), (IPART(zx) << 0x10) | IPART(zy), (IPART(zz) << 0x10) | IPART(zw), \
                (IPART(wx) << 0x10) | IPART(wy), (IPART(wz) << 0x10) | IPART(ww), (FPART(xx) << 0x10) | FPART(xy), \
                (FPART(xz) << 0x10) | FPART(xw), (FPART(yx) << 0x10) | FPART(yy), (FPART(yz) << 0x10) | FPART(yw), \
                (FPART(zx) << 0x10) | FPART(zy), (FPART(zz) << 0x10) | FPART(zw), (FPART(wx) << 0x10) | FPART(wy), \
                (FPART(wz) << 0x10) | FPART(ww),                                                                   \
        }                                                                                                          \
    }
// clang-format off
Mtx gIdentityMtx = gdSPDefMtx(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);
// clang-format on

MtxF gIdentityMtxF = { {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
} };

/* bss */

MtxF* sMatrixStack;   //!< original name: "Matrix_stack"
MtxF* sCurrentMatrix; //!< original name: "Matrix_now"

#define MATRIX_STACK_SIZE 20

/* Stack operations */

/**
 * @brief Create the matrix stack and set the pointer to the top of it.
 *
 * @remark original name: "new_Matrix"
 */
void Matrix_Init(GameState* gameState) {
    sMatrixStack = THA_AllocTailAlign16(&gameState->tha, MATRIX_STACK_SIZE * sizeof(MtxF));
    sCurrentMatrix = sMatrixStack;
}

/**
 * @brief Place a new matrix on the top of the stack and move the stack pointer up.
 *
 * @remark original name: "Matrix_push"
 */
void Matrix_Push(void) {
    MtxF* prev = sCurrentMatrix;

    FrameInterpolation_RecordMatrixPush();
    sCurrentMatrix++;
    Matrix_MtxFCopy(sCurrentMatrix, prev);
}

/**
 * @brief Discard the top matrix on the stack and move stack pointer to the next one down.
 *
 * @remark original name: "Matrix_pull"
 */
void Matrix_Pop(void) {
    FrameInterpolation_RecordMatrixPop();
    sCurrentMatrix--;
}

/**
 * @brief Copy the top matrix from the stack.
 *
 * @param[out] dest Matrix into which to copy.
 *
 * @remark original name: "Matrix_get"
 */
void Matrix_Get(MtxF* dest) {
    Matrix_MtxFCopy(dest, sCurrentMatrix);
}

/**
 * @brief Overwrite the top matrix on the stack.
 *
 * @param[in] src Matrix from which to copy.
 *
 * @remark original name: "Matrix_put"
 */
void Matrix_Put(MtxF* src) {
    FrameInterpolation_RecordMatrixPut(src);
    Matrix_MtxFCopy(sCurrentMatrix, src);
}

/**
 * @brief Return pointer to the top of the matrix stack.
 *
 * @return pointer to top matrix on the stack.
 *
 * @remark original name: get_Matrix_now
 */
MtxF* Matrix_GetCurrent(void) {
    return sCurrentMatrix;
}

/* General transformation matrix functions */

/**
 * @brief General multiplication of current by a matrix.
 *      - APPLY: current * mf -> current
 *      - NEW: mf -> current
 *
 * @param mf Matrix to multiply by.
 * @param mode APPLY or NEW.
 *
 * @remark original name: "Matrix_mult"
 */
void Matrix_Mult(MtxF* mf, MatrixMode mode) {
    FrameInterpolation_RecordMatrixMult(mf, mode);
    MtxF* cmf = Matrix_GetCurrent();

    if (mode == MTXMODE_APPLY) {
        SkinMatrix_MtxFMtxFMult(cmf, mf, cmf);
    } else {
        Matrix_MtxFCopy(sCurrentMatrix, mf);
    }
}

/**
 * @brief Right-multiply current by a translation matrix T.
 *      - APPLY: current * T -> current
 *      - NEW: T -> current
 *
 * T is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      1 & 0 & 0 & x \\
 *      0 & 1 & 0 & y \\
 *      0 & 0 & 1 & z \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix} .
 * \f]
 *
 * @param x translation distance in the x direction.
 * @param y translation distance in the y direction.
 * @param z translation distance in the z direction.
 * @param mode APPLY or NEW.
 *
 * @remark original name: "Matrix_translate"
 */
void Matrix_Translate(f32 x, f32 y, f32 z, MatrixMode mode) {
    FrameInterpolation_RecordMatrixTranslate(x, y, z, mode);
    MtxF* cmf = sCurrentMatrix;
    f32 tempX;
    f32 tempY;

    if (mode == MTXMODE_APPLY) {
        tempX = cmf->xx;
        tempY = cmf->xy;
        cmf->xw += tempX * x + tempY * y + cmf->xz * z;
        tempX = cmf->yx;
        tempY = cmf->yy;
        cmf->yw += tempX * x + tempY * y + cmf->yz * z;
        tempX = cmf->zx;
        tempY = cmf->zy;
        cmf->zw += tempX * x + tempY * y + cmf->zz * z;
        tempX = cmf->wx;
        tempY = cmf->wy;
        cmf->ww += tempX * x + tempY * y + cmf->wz * z;
    } else {
        SkinMatrix_SetTranslate(cmf, x, y, z);
    }
}

/**
 * @brief Right-multiply by the diagonal scale matrix S = diag(x,y,z,1).
 *      - APPLY: current * S -> current
 *      - NEW: S -> current
 *
 * S is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      x & 0 & 0 & 0 \\
 *      0 & y & 0 & 0 \\
 *      0 & 0 & z & 0 \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix} .
 * \f]
 *
 * @param x scale in x direction.
 * @param y scale in y direction.
 * @param z scale in z direction.
 * @param mode APPLY or NEW.
 *
 * @remark original name: "Matrix_scale"
 */
void Matrix_Scale(f32 x, f32 y, f32 z, MatrixMode mode) {
    FrameInterpolation_RecordMatrixScale(x, y, z, mode);
    MtxF* cmf = sCurrentMatrix;

    if (mode == MTXMODE_APPLY) {
        cmf->xx *= x;
        cmf->yx *= x;
        cmf->zx *= x;
        cmf->xy *= y;
        cmf->yy *= y;
        cmf->zy *= y;
        cmf->xz *= z;
        cmf->yz *= z;
        cmf->zz *= z;
        cmf->wx *= x;
        cmf->wy *= y;
        cmf->wz *= z;
    } else {
        SkinMatrix_SetScale(cmf, x, y, z);
    }
}

/**
 * @brief Right-multiply by a rotation about the x axis
 *      - APPLY: current * R -> current
 *      - NEW: R -> current
 *
 * R is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      1 & 0 & 0 & 0 \\
 *      0 & c & -s & 0 \\
 *      0 & s & c & 0 \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where \f$ c = \cos x, s = \sin x \f$.
 *
 * @note The same as Matrix_RotateXF(), but uses a binary angle.
 *
 * @param x rotation angle (binary).
 * @param mode APPLY or NEW.
 *
 * @remark original name: "Matrix_RotateX"
 */
void Matrix_RotateXS(s16 x, MatrixMode mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempY;
    f32 tempZ;

    if (mode == MTXMODE_APPLY) {
        if (x != 0) {
            cmf = sCurrentMatrix;

            sin = Math_SinS(x);
            cos = Math_CosS(x);

            tempY = cmf->xy;
            tempZ = cmf->xz;
            cmf->xy = tempY * cos + tempZ * sin;
            cmf->xz = tempZ * cos - tempY * sin;

            tempY = cmf->yy;
            tempZ = cmf->yz;
            cmf->yy = tempY * cos + tempZ * sin;
            cmf->yz = tempZ * cos - tempY * sin;

            tempY = cmf->zy;
            tempZ = cmf->zz;
            cmf->zy = tempY * cos + tempZ * sin;
            cmf->zz = tempZ * cos - tempY * sin;

            tempY = cmf->wy;
            tempZ = cmf->wz;
            cmf->wy = tempY * cos + tempZ * sin;
            cmf->wz = tempZ * cos - tempY * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (x != 0) {
            sin = Math_SinS(x);
            cos = Math_CosS(x);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->yx = 0.0f;
        cmf->zx = 0.0f;
        cmf->wx = 0.0f;
        cmf->xy = 0.0f;
        cmf->wy = 0.0f;
        cmf->xz = 0.0f;
        cmf->wz = 0.0f;
        cmf->xw = 0.0f;
        cmf->yw = 0.0f;
        cmf->zw = 0.0f;
        cmf->xx = 1.0f;
        cmf->ww = 1.0f;
        cmf->yy = cos;
        cmf->zz = cos;
        cmf->zy = sin;
        cmf->yz = -sin;
    }
}

// Unused
/**
 * @brief Right-multiply by a rotation about the x axis.
 *      - APPLY: current * R -> current
 *      - NEW: R -> current
 *
 * R is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      1 & 0 & 0 & 0 \\
 *      0 & c & -s & 0 \\
 *      0 & s & c & 0 \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where \f$ c = \cos x, s = \sin x \f$.
 *
 * @note The same as Matrix_RotateXS(), but uses a float angle in radians.
 *
 * @param x rotation angle (radians).
 * @param mode APPLY or NEW.
 *
 * @remark original name may have been "Matrix_RotateX", but clashed with the previous function.
 */
void Matrix_RotateXF(f32 x, MatrixMode mode) {
    FrameInterpolation_RecordMatrixRotate1Coord(0, x, mode);
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempY;
    f32 tempZ;
    f32 zero = 0.0;
    f32 one = 1.0;

    if (mode == MTXMODE_APPLY) {
        if (x != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(x);
            cos = cosf(x);

            tempY = cmf->xy;
            tempZ = cmf->xz;
            cmf->xy = tempY * cos + tempZ * sin;
            cmf->xz = tempZ * cos - tempY * sin;

            tempY = cmf->yy;
            tempZ = cmf->yz;
            cmf->yy = tempY * cos + tempZ * sin;
            cmf->yz = tempZ * cos - tempY * sin;

            tempY = cmf->zy;
            tempZ = cmf->zz;
            cmf->zy = tempY * cos + tempZ * sin;
            cmf->zz = tempZ * cos - tempY * sin;

            tempY = cmf->wy;
            tempZ = cmf->wz;
            cmf->wy = tempY * cos + tempZ * sin;
            cmf->wz = tempZ * cos - tempY * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (x != 0) {
            sin = sinf(x);
            cos = cosf(x);
        } else {
            sin = zero;
            cos = one;
        }

        cmf->xx = one;
        cmf->yx = zero;
        cmf->zx = zero;
        cmf->wx = zero;
        cmf->xy = zero;
        cmf->yy = cos;
        cmf->zy = sin;
        cmf->wy = zero;
        cmf->xz = zero;
        cmf->yz = -sin;
        cmf->zz = cos;
        cmf->wz = zero;
        cmf->xw = zero;
        cmf->yw = zero;
        cmf->zw = zero;
        cmf->ww = one;
    }
}

/**
 * @brief Right-multiply by a rotation about the x axis.
 *      current * R -> current
 *
 * @note Matrix_RotateXF() with mode APPLY.
 *
 * @param x rotation angle (radians).
 */
void Matrix_RotateXFApply(f32 x) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempY;
    f32 tempZ;
    s32 pad;

    if (x != 0.0f) {
        cmf = sCurrentMatrix;

        sin = sins(RAD_TO_BINANG(x)) * SHT_MINV;
        cos = coss(RAD_TO_BINANG(x)) * SHT_MINV;

        tempY = cmf->xy;
        tempZ = cmf->xz;
        cmf->xy = (tempY * cos) + (tempZ * sin);
        cmf->xz = (tempZ * cos) - (tempY * sin);

        tempY = cmf->yy;
        tempZ = cmf->yz;
        cmf->yy = (tempY * cos) + (tempZ * sin);
        cmf->yz = (tempZ * cos) - (tempY * sin);

        tempY = cmf->zy;
        tempZ = cmf->zz;
        cmf->zy = (tempY * cos) + (tempZ * sin);
        cmf->zz = (tempZ * cos) - (tempY * sin);

        tempY = cmf->wy;
        tempZ = cmf->wz;
        cmf->wy = (tempY * cos) + (tempZ * sin);
        cmf->wz = (tempZ * cos) - (tempY * sin);
    }
}

/**
 * @brief Replace current by a rotation about the x axis.
 *      R -> current
 *
 * @note Matrix_RotateXF() with mode NEW.
 *
 * @param x rotation angle (radians).
 */
void Matrix_RotateXFNew(f32 x) {
    MtxF* cmf = sCurrentMatrix;
    s32 pad[2];
    f32 sin;
    f32 cos;

    cmf->xx = 1.0f;
    cmf->yx = 0.0f;
    cmf->zx = 0.0f;
    cmf->wx = 0.0f;
    cmf->xy = 0.0f;
    cmf->wy = 0.0f;
    cmf->xz = 0.0f;
    cmf->wz = 0.0f;
    cmf->xw = 0.0f;
    cmf->yw = 0.0f;
    cmf->zw = 0.0f;
    cmf->ww = 1.0f;

    if (x != 0.0f) {
        sin = sinf(x);
        cos = cosf(x);

        cmf->yy = cos;
        cmf->zz = cos;
        cmf->yz = -sin;
        cmf->zy = sin;
    } else {
        cmf->yy = 1.0f;
        cmf->zy = 0.0f;
        cmf->yz = 0.0f;
        cmf->zz = 1.0f;
    }
}

/**
 * @brief Right-multiply by a rotation about the y axis
 *      - APPLY: current * R -> current
 *      - NEW: R -> current
 *
 * R is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      c & 0 & s & 0 \\
 *      0 & 1 & 0 & 0 \\
 *      -s & 0 & c & 0 \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where \f$ c = \cos y, s = \sin y \f$.
 *
 * @note The same as Matrix_RotateYF(), but uses a binary angle.
 *
 * @param y rotation angle (binary).
 * @param mode APPLY or NEW.
 *
 * @remark original name: "Matrix_RotateY"
 */
void Matrix_RotateYS(s16 y, MatrixMode mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempX;
    f32 tempZ;

    if (mode == MTXMODE_APPLY) {
        if (y != 0) {
            cmf = sCurrentMatrix;

            sin = Math_SinS(y);
            cos = Math_CosS(y);

            tempX = cmf->xx;
            tempZ = cmf->xz;
            cmf->xx = tempX * cos - tempZ * sin;
            cmf->xz = tempX * sin + tempZ * cos;

            tempX = cmf->yx;
            tempZ = cmf->yz;
            cmf->yx = tempX * cos - tempZ * sin;
            cmf->yz = tempX * sin + tempZ * cos;

            tempX = cmf->zx;
            tempZ = cmf->zz;
            cmf->zx = tempX * cos - tempZ * sin;
            cmf->zz = tempX * sin + tempZ * cos;

            tempX = cmf->wx;
            tempZ = cmf->wz;
            cmf->wx = tempX * cos - tempZ * sin;
            cmf->wz = tempX * sin + tempZ * cos;
        }
    } else {
        cmf = sCurrentMatrix;

        if (y != 0) {
            sin = Math_SinS(y);
            cos = Math_CosS(y);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->yx = 0.0f;
        cmf->wx = 0.0f;
        cmf->xy = 0.0f;
        cmf->zy = 0.0f;
        cmf->wy = 0.0f;
        cmf->yz = 0.0f;
        cmf->wz = 0.0f;
        cmf->xw = 0.0f;
        cmf->yw = 0.0f;
        cmf->zw = 0.0f;
        cmf->yy = 1.0f;
        cmf->ww = 1.0f;
        cmf->xx = cos;
        cmf->zz = cos;
        cmf->zx = -sin;
        cmf->xz = sin;
    }
}

/**
 * @brief Right-multiply by a rotation about the y axis.
 *      - APPLY: current * R -> current
 *      - NEW: R -> current
 *
 * R is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      c & 0 & s & 0 \\
 *      0 & 1 & 0 & 0 \\
 *      -s & 0 & c & 0 \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where \f$ c = \cos y, s = \sin y \f$.
 *
 * @note The same as Matrix_RotateYS(), but uses a float angle in radians.
 *
 * @param y rotation angle (radians).
 * @param mode APPLY or NEW.
 *
 * @remark original name may have been "Matrix_RotateY", but clashed with the previous function.
 */
void Matrix_RotateYF(f32 y, MatrixMode mode) {
    FrameInterpolation_RecordMatrixRotate1Coord(1, y, mode);
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempX;
    f32 tempZ;
    f32 zero = 0.0;
    f32 one = 1.0;

    if (mode == MTXMODE_APPLY) {
        if (y != 0.0f) {
            cmf = sCurrentMatrix;

            sin = sinf(y);
            cos = cosf(y);

            tempX = cmf->xx;
            tempZ = cmf->xz;
            cmf->xx = tempX * cos - tempZ * sin;
            cmf->xz = tempX * sin + tempZ * cos;

            tempX = cmf->yx;
            tempZ = cmf->yz;
            cmf->yx = tempX * cos - tempZ * sin;
            cmf->yz = tempX * sin + tempZ * cos;

            tempX = cmf->zx;
            tempZ = cmf->zz;
            cmf->zx = tempX * cos - tempZ * sin;
            cmf->zz = tempX * sin + tempZ * cos;

            tempX = cmf->wx;
            tempZ = cmf->wz;
            cmf->wx = tempX * cos - tempZ * sin;
            cmf->wz = tempX * sin + tempZ * cos;
        }
    } else {
        cmf = sCurrentMatrix;

        if (y != 0.0f) {
            sin = sinf(y);
            cos = cosf(y);
        } else {
            cos = one;
            sin = zero;
        }

        cmf->yx = zero;
        cmf->wx = zero;
        cmf->xy = zero;
        cmf->zy = zero;
        cmf->wy = zero;
        cmf->yz = zero;
        cmf->wz = zero;
        cmf->xw = zero;
        cmf->yw = zero;
        cmf->zw = zero;
        cmf->yy = one;
        cmf->ww = one;
        cmf->xx = cos;
        cmf->zz = cos;
        cmf->zx = -sin;
        cmf->xz = sin;
    }
}

/**
 * @brief Right-multiply by a rotation about the z axis.
 *      - APPLY: current * R -> current
 *      - NEW: R -> current
 *
 * R is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      c & -s & 0 & 0 \\
 *      s & c & 0 & 0 \\
 *      0 & 0 & 1 & 0 \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where \f$ c = \cos z, s = \sin z \f$.
 *
 * @note The same as Matrix_RotateZF, but uses a binary angle.
 *
 * @param z rotation angle (binary).
 * @param mode APPLY or NEW.
 *
 * @remark original name: "Matrix_RotateZ"
 */
void Matrix_RotateZS(s16 z, MatrixMode mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempX;
    f32 tempY;
    f32 zero = 0.0;
    f32 one = 1.0;

    if (mode == MTXMODE_APPLY) {
        if (z != 0) {
            cmf = sCurrentMatrix;

            sin = Math_SinS(z);
            cos = Math_CosS(z);

            tempX = cmf->xx;
            tempY = cmf->xy;
            cmf->xx = tempX * cos + tempY * sin;
            cmf->xy = tempY * cos - tempX * sin;

            tempX = cmf->yx;
            tempY = cmf->yy;
            cmf->yx = tempX * cos + tempY * sin;
            cmf->yy = tempY * cos - tempX * sin;

            tempX = cmf->zx;
            tempY = cmf->zy;
            cmf->zx = tempX * cos + tempY * sin;
            cmf->zy = tempY * cos - tempX * sin;

            tempX = cmf->wx;
            tempY = cmf->wy;
            cmf->wx = tempX * cos + tempY * sin;
            cmf->wy = tempY * cos - tempX * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (z != 0) {
            sin = Math_SinS(z);
            cos = Math_CosS(z);
        } else {
            sin = zero;
            cos = one;
        }

        cmf->zx = zero;
        cmf->wx = zero;
        cmf->zy = zero;
        cmf->wy = zero;
        cmf->xz = zero;
        cmf->yz = zero;
        cmf->wz = zero;
        cmf->xw = zero;
        cmf->yw = zero;
        cmf->zw = zero;
        cmf->zz = one;
        cmf->ww = one;
        cmf->xx = cos;
        cmf->yy = cos;
        cmf->yx = sin;
        cmf->xy = -sin;
    }
}

/**
 * @brief Right-multiply by a rotation about the z axis.
 *      - APPLY: current * R -> current
 *      - NEW: R -> current
 *
 * R is given by
 *
 * \f[
 *  \begin{pmatrix}
 *      c & -s & 0 & 0 \\
 *      s & c & 0 & 0 \\
 *      0 & 0 & 1 & 0 \\
 *      0 & 0 & 0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where \f$ c = \cos z, s = \sin z \f$.
 *
 * @note The same as Matrix_RotateYS(), but uses a float angle in radians.
 *
 * @param z rotation angle (radians).
 * @param mode APPLY or NEW.
 *
 * @remark original name may have been "Matrix_RotateZ", but clashed with the previous function.
 */
void Matrix_RotateZF(f32 z, MatrixMode mode) {
    FrameInterpolation_RecordMatrixRotate1Coord(2, z, mode);
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempX;
    f32 tempY;

    if (mode == MTXMODE_APPLY) {
        if (z != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(z);
            cos = cosf(z);

            tempX = cmf->xx;
            tempY = cmf->xy;
            cmf->xx = tempX * cos + tempY * sin;
            cmf->xy = tempY * cos - tempX * sin;

            tempX = cmf->yx;
            tempY = cmf->yy;
            cmf->yx = tempX * cos + tempY * sin;
            cmf->yy = tempY * cos - tempX * sin;

            tempX = cmf->zx;
            tempY = cmf->zy;
            cmf->zx = tempX * cos + tempY * sin;
            cmf->zy = tempY * cos - tempX * sin;

            tempX = cmf->wx;
            tempY = cmf->wy;
            cmf->wx = tempX * cos + tempY * sin;
            cmf->wy = tempY * cos - tempX * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (z != 0) {
            sin = sinf(z);
            cos = cosf(z);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->zx = 0.0f;
        cmf->wx = 0.0f;
        cmf->zy = 0.0f;
        cmf->wy = 0.0f;
        cmf->xz = 0.0f;
        cmf->yz = 0.0f;
        cmf->wz = 0.0f;
        cmf->xw = 0.0f;
        cmf->yw = 0.0f;
        cmf->zw = 0.0f;
        cmf->zz = 1.0f;
        cmf->ww = 1.0f;
        cmf->xx = cos;
        cmf->yy = cos;
        cmf->yx = sin;
        cmf->xy = -sin;
    }
}

/**
 * @brief Rotate using ZYX Tait-Bryan angles.
 *      - APPLY: current Rz Ry Rx -> current
 *      - NEW: Rz Ry Rx -> current
 *
 * This means a (column) vector is first rotated around X, then around Y, then around Z, then (if `mode` is APPLY) gets
 * transformed by what the matrix was before adding the ZYX rotation.
 *
 * See previous functions for the forms of Rz, Ry, Rx
 *
 * @param x binary angle to rotate about x axis
 * @param y binary angle to rotate about y axis
 * @param z binary angle to rotate about z axis
 * @param mode APPLY or NEW
 *
 * @remark original name: "Matrix_RotateXYZ", changed to reflect rotation order.
 */
void Matrix_RotateZYX(s16 x, s16 y, s16 z, MatrixMode mode) {
    FrameInterpolation_RecordMatrixRotateZYX(x, y, z, mode);
    MtxF* cmf = sCurrentMatrix;
    f32 temp1;
    f32 temp2;
    f32 sin;
    f32 cos;

    if (mode == MTXMODE_APPLY) {
        if (z != 0) { // Added in MM, OoT always follows the nonzero path
            sin = Math_SinS(z);
            cos = Math_CosS(z);

            temp1 = cmf->xx;
            temp2 = cmf->xy;
            cmf->xx = temp1 * cos + temp2 * sin;
            cmf->xy = temp2 * cos - temp1 * sin;

            temp1 = cmf->yx;
            temp2 = cmf->yy;
            cmf->yx = temp1 * cos + temp2 * sin;
            cmf->yy = temp2 * cos - temp1 * sin;

            temp1 = cmf->zx;
            temp2 = cmf->zy;
            cmf->zx = temp1 * cos + temp2 * sin;
            cmf->zy = temp2 * cos - temp1 * sin;

            temp1 = cmf->wx;
            temp2 = cmf->wy;
            cmf->wx = temp1 * cos + temp2 * sin;
            cmf->wy = temp2 * cos - temp1 * sin;
        }

        if (y != 0) {
            sin = Math_SinS(y);
            cos = Math_CosS(y);

            temp1 = cmf->xx;
            temp2 = cmf->xz;
            cmf->xx = temp1 * cos - temp2 * sin;
            cmf->xz = temp1 * sin + temp2 * cos;

            temp1 = cmf->yx;
            temp2 = cmf->yz;
            cmf->yx = temp1 * cos - temp2 * sin;
            cmf->yz = temp1 * sin + temp2 * cos;

            temp1 = cmf->zx;
            temp2 = cmf->zz;
            cmf->zx = temp1 * cos - temp2 * sin;
            cmf->zz = temp1 * sin + temp2 * cos;

            temp1 = cmf->wx;
            temp2 = cmf->wz;
            cmf->wx = temp1 * cos - temp2 * sin;
            cmf->wz = temp1 * sin + temp2 * cos;
        }

        if (x != 0) {
            sin = Math_SinS(x);
            cos = Math_CosS(x);

            temp1 = cmf->xy;
            temp2 = cmf->xz;
            cmf->xy = temp1 * cos + temp2 * sin;
            cmf->xz = temp2 * cos - temp1 * sin;

            temp1 = cmf->yy;
            temp2 = cmf->yz;
            cmf->yy = temp1 * cos + temp2 * sin;
            cmf->yz = temp2 * cos - temp1 * sin;

            temp1 = cmf->zy;
            temp2 = cmf->zz;
            cmf->zy = temp1 * cos + temp2 * sin;
            cmf->zz = temp2 * cos - temp1 * sin;

            temp1 = cmf->wy;
            temp2 = cmf->wz;
            cmf->wy = temp1 * cos + temp2 * sin;
            cmf->wz = temp2 * cos - temp1 * sin;
        }
    } else {
        SkinMatrix_SetRotateRPY(cmf, x, y, z);
    }
}

/**
 * @brief Translate and rotate using ZYX Tait-Bryan angles.
 *      current T Rz Ry Rx -> current
 *
 * This means a (column) vector is first rotated around X, then around Y, then around Z, then translated, then gets
 * transformed by whatever the matrix was previously.
 *
 * @param translation vector by which to translate.
 * @param rot vector of rotation angles.
 *
 * @remark original name appears to be "Matrix_softcv3_mult"
 */
void Matrix_TranslateRotateZYX(Vec3f* translation, Vec3s* rot) {
    FrameInterpolation_RecordMatrixTranslateRotateZYX(translation, rot);
    MtxF* cmf = sCurrentMatrix;
    f32 sin = Math_SinS(rot->z);
    f32 cos = Math_CosS(rot->z);
    f32 temp1;
    f32 temp2;

    // No check for z != 0, presumably since translation is interleaved.
    temp1 = cmf->xx;
    temp2 = cmf->xy;
    cmf->xw += temp1 * translation->x + temp2 * translation->y + cmf->xz * translation->z;
    cmf->xx = temp1 * cos + temp2 * sin;
    cmf->xy = temp2 * cos - temp1 * sin;

    temp1 = cmf->yx;
    temp2 = cmf->yy;
    cmf->yw += temp1 * translation->x + temp2 * translation->y + cmf->yz * translation->z;
    cmf->yx = temp1 * cos + temp2 * sin;
    cmf->yy = temp2 * cos - temp1 * sin;

    temp1 = cmf->zx;
    temp2 = cmf->zy;
    cmf->zw += temp1 * translation->x + temp2 * translation->y + cmf->zz * translation->z;
    cmf->zx = temp1 * cos + temp2 * sin;
    cmf->zy = temp2 * cos - temp1 * sin;

    temp1 = cmf->wx;
    temp2 = cmf->wy;
    cmf->ww += temp1 * translation->x + temp2 * translation->y + cmf->wz * translation->z;
    cmf->wx = temp1 * cos + temp2 * sin;
    cmf->wy = temp2 * cos - temp1 * sin;

    if (rot->y != 0) {
        sin = Math_SinS(rot->y);
        cos = Math_CosS(rot->y);

        temp1 = cmf->xx;
        temp2 = cmf->xz;
        cmf->xx = temp1 * cos - temp2 * sin;
        cmf->xz = temp1 * sin + temp2 * cos;

        temp1 = cmf->yx;
        temp2 = cmf->yz;
        cmf->yx = temp1 * cos - temp2 * sin;
        cmf->yz = temp1 * sin + temp2 * cos;

        temp1 = cmf->zx;
        temp2 = cmf->zz;
        cmf->zx = temp1 * cos - temp2 * sin;
        cmf->zz = temp1 * sin + temp2 * cos;

        temp1 = cmf->wx;
        temp2 = cmf->wz;
        cmf->wx = temp1 * cos - temp2 * sin;
        cmf->wz = temp1 * sin + temp2 * cos;
    }

    if (rot->x != 0) {
        sin = Math_SinS(rot->x);
        cos = Math_CosS(rot->x);

        temp1 = cmf->xy;
        temp2 = cmf->xz;
        cmf->xy = temp1 * cos + temp2 * sin;
        cmf->xz = temp2 * cos - temp1 * sin;

        temp1 = cmf->yy;
        temp2 = cmf->yz;
        cmf->yy = temp1 * cos + temp2 * sin;
        cmf->yz = temp2 * cos - temp1 * sin;

        temp1 = cmf->zy;
        temp2 = cmf->zz;
        cmf->zy = temp1 * cos + temp2 * sin;
        cmf->zz = temp2 * cos - temp1 * sin;

        temp1 = cmf->wy;
        temp2 = cmf->wz;
        cmf->wy = temp1 * cos + temp2 * sin;
        cmf->wz = temp2 * cos - temp1 * sin;
    }
}

/**
 * @brief Set current to a general translation and rotation using YXZ Tait-Bryan angles: T Ry Rx Rz -> current
 *
 * This means a (column) vector is first rotated around Y, then around X, then around Z, then translated, then gets
 * transformed by whatever the matrix was previously.
 *
 * @param x amount to translate in X direction.
 * @param y amount to translate in Y direction.
 * @param z amount to translate in Z direction.
 * @param rot vector of rotation angles.
 *
 * @remark original name appears to be "Matrix_softcv3_load"
 */
void Matrix_SetTranslateRotateYXZ(f32 x, f32 y, f32 z, Vec3s* rot) {
    MtxF* cmf = sCurrentMatrix;
    f32 sinY = Math_SinS(rot->y);
    f32 cosY = Math_CosS(rot->y);
    f32 cosTemp;
    f32 sinTemp;

    cmf->xx = cosY;
    cmf->zx = -sinY;
    cmf->xw = x;
    cmf->yw = y;
    cmf->zw = z;
    cmf->wx = 0.0f;
    cmf->wy = 0.0f;
    cmf->wz = 0.0f;
    cmf->ww = 1.0f;

    if (rot->x != 0) {
        sinTemp = Math_SinS(rot->x);
        cosTemp = Math_CosS(rot->x);

        cmf->zz = cosY * cosTemp;
        cmf->zy = cosY * sinTemp;
        cmf->xz = sinY * cosTemp;
        cmf->xy = sinY * sinTemp;
        cmf->yz = -sinTemp;
        cmf->yy = cosTemp;
    } else {
        cmf->zz = cosY;
        cmf->xz = sinY;
        cmf->yz = 0.0f;
        cmf->zy = 0.0f;
        cmf->xy = 0.0f;
        cmf->yy = 1.0f;
    }

    if (rot->z != 0) {
        sinTemp = Math_SinS(rot->z);
        cosTemp = Math_CosS(rot->z);

        sinY = cmf->xx;
        cosY = cmf->xy;
        cmf->xx = sinY * cosTemp + cosY * sinTemp;
        cmf->xy = cosY * cosTemp - sinY * sinTemp;

        sinY = cmf->zx;
        cosY = cmf->zy;
        cmf->zx = sinY * cosTemp + cosY * sinTemp;
        cmf->zy = cosY * cosTemp - sinY * sinTemp;

        cosY = cmf->yy;
        cmf->yx = cosY * sinTemp;
        cmf->yy = cosY * cosTemp;
    } else {
        cmf->yx = 0.0f;
    }
    FrameInterpolation_RecordMatrixSetTranslateRotateYXZ(x, y, z, rot);
}

/**
 * @brief Converts a floating-point MtxF to a fixed-point RSP-compatible matrix.
 *
 * @param[in] src MtxF to convert.
 * @param[out] dest mtx to output to.
 *
 * @return dest
 *
 * @remark original name: "_MtxF_to_Mtx"
 */
Mtx* Matrix_MtxFToMtx(MtxF* src, Mtx* dest) {
    FrameInterpolation_RecordMatrixMtxFToMtx(src, dest);
    // 2S2H [Port] For compatibility with modern systems this has been changed to use guMtxF2L
    guMtxF2L(src, dest);
    return dest;
}

/**
 * @brief Converts current to a fixed-point RSP-compatible matrix.
 *
 * @note Debug uses Matrix_CheckFloats to test current first.
 *
 * @param[out] dest mtx to output to.
 *
 * @return dest
 *
 * @remark original name: "_Matrix_to_Mtx"
 */
Mtx* Matrix_ToMtx(Mtx* dest) {
    FrameInterpolation_RecordMatrixToMtx(dest, __FILE__, __LINE__);
    guMtxF2L(sCurrentMatrix, dest);
    return dest;
    // return Matrix_MtxFToMtx(sCurrentMatrix, dest);
}

/**
 * @brief Converts current to a RSP-compatible matrix and saves it to allocated space in the OPA buffer.
 *
 * @param[in,out] gfxCtx Graphics context.
 *
 * @return allocated mtx.
 *
 * @remark original name: "_Matrix_to_Mtx_new"
 */
Mtx* Matrix_Finalize(GraphicsContext* gfxCtx) {
    return Matrix_ToMtx(GRAPH_ALLOC(gfxCtx, sizeof(Mtx)));
}

// Unused
/**
 * @brief Converts src to a RSP-compatible matrix and saves it to allocated space in the OPA buffer.
 *
 * @param[in] src MtxF to convert.
 * @param[in,out] gfxCtx Graphics context.
 *
 * @return allocated mtx.
 *
 * @remark original name unknown, likely close to "_Matrix_MtxF_to_Mtx_new"
 */
Mtx* Matrix_MtxFToNewMtx(MtxF* src, GraphicsContext* gfxCtx) {
    return Matrix_MtxFToMtx(src, GRAPH_ALLOC(gfxCtx, sizeof(Mtx)));
}

/**
 * @brief Calculates current * (src,1) and writes its components to dest.
 *
 * This assumes that current has the form
 *
 * \f[
 *  M =
 *  \begin{pmatrix}
 *      A & b \\
 *      0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * where A is \f$ 3 \times 3 \f$ and b \f$ 3 \times 1 \f$, and so calculates
 *
 * \f[
 *  MX =
 *  \begin{pmatrix}
 *      A & b \\
 *      0 & 1
 *  \end{pmatrix}
 *  \begin{pmatrix}
 *      x \\
 *      1
 *  \end{pmatrix}
 *  =
 *  \begin{pmatrix}
 *      Ax + b \\
 *      1
 *  \end{pmatrix}
 * \f]
 *
 * and discards the extra w component (1).
 *
 * @param[in] src input vector
 * @param[out] dest output vector
 *
 * @remark original name: "Matrix_Position"
 */
void Matrix_MultVec3f(Vec3f* src, Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw + (cmf->xx * src->x + cmf->xy * src->y + cmf->xz * src->z);
    dest->y = cmf->yw + (cmf->yx * src->x + cmf->yy * src->y + cmf->yz * src->z);
    dest->z = cmf->zw + (cmf->zx * src->x + cmf->zy * src->y + cmf->zz * src->z);
}

/**
 * @brief Multiply the vector `(0, 0, 0, 1)` by current.
 *
 * Can also see it as obtaining the translation vector part of current, but the former interpretation is consistent with
 * the other functions nearby.
 *
 * @note Special case of Matrix_MultVec3f() with `src = { 0, 0, 0 }`; the same assumptions apply.
 *
 * @param[out] dest output vector.
 *
 * @remark original name: "Matrix_Position_Zero"
 */
void Matrix_MultZero(Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw;
    dest->y = cmf->yw;
    dest->z = cmf->zw;
}

/**
 * @brief Multiply the vector `(x, 0, 0, 1)` by current.
 *
 * I.e. calculate \f$ A(x, 0, 0) + b \f$.
 *
 * @note Special case of Matrix_MultVec3f() with `src = { x, 0, 0 }`; the same assumptions apply.
 *
 * @param[in] x multiplier of unit vector in x direction.
 * @param[out] dest output vector.
 *
 * @remark original name: "Matrix_Position_VecX"
 */
void Matrix_MultVecX(f32 x, Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw + cmf->xx * x;
    dest->y = cmf->yw + cmf->yx * x;
    dest->z = cmf->zw + cmf->zx * x;
}

/**
 * @brief Multiply the vector `(0, y, 0, 1)` by current.
 *
 * I.e. calculate \f$ A(0, y, 0) + b \f$.
 *
 * @note Special case of Matrix_MultVec3f() with `src = { 0, y, 0 }`; the same assumptions apply.
 *
 * @param[in] y multiplier of unit vector in y direction.
 * @param[out] dest output vector.
 *
 * @remark original name is most likely "Matrix_Position_VecY" by analogy with the other two.
 */
void Matrix_MultVecY(f32 y, Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw + cmf->xy * y;
    dest->y = cmf->yw + cmf->yy * y;
    dest->z = cmf->zw + cmf->zy * y;
}

/**
 * @brief Multiply the vector `(0, 0, z, 1)` by current.
 *
 * I.e. calculate \f$ A(0, 0, z) + b \f$.
 *
 * @note Special case of Matrix_MultVec3f() with `src = { 0, 0, z }`; the same assumptions apply.
 *
 * @param[in] z multiplier of unit vector in z direction.
 * @param[out] dest output vector.
 *
 * @remark original name: "Matrix_Position_VecZ"
 */
void Matrix_MultVecZ(f32 z, Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw + cmf->xz * z;
    dest->y = cmf->yw + cmf->yz * z;
    dest->z = cmf->zw + cmf->zz * z;
}

/**
 * @brief Calculates current * (src,1) and writes its x and z components to dest.
 *
 * The same as Matrix_MultVec3f(), but only applies to the x and z components; the same assumptions apply.
 *
 * @note Unlike the previous functions, does *not* just multiply (x, 0, z, 1) and save the x,y,z components.
 *
 * @param[in] src input vector.
 * @param[out] dest output vector.
 */
void Matrix_MultVec3fXZ(Vec3f* src, Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw + (cmf->xx * src->x + cmf->xy * src->y + cmf->xz * src->z);
    dest->z = cmf->zw + (cmf->zx * src->x + cmf->zy * src->y + cmf->zz * src->z);
}

/**
 * @brief Copies the matrix src into dest.
 *
 * @param[out] dest matrix to copy to.
 * @param[in] src matrix to copy from.
 *
 * @remark original name: "Matrix_copy_MtxF"
 */
void Matrix_MtxFCopy(MtxF* dest, MtxF* src) {
    dest->xx = src->xx;
    dest->yx = src->yx;
    dest->zx = src->zx;
    dest->wx = src->wx;
    dest->xy = src->xy;
    dest->yy = src->yy;
    dest->zy = src->zy;
    dest->wy = src->wy;
    dest->xx = src->xx;
    dest->yx = src->yx;
    dest->zx = src->zx;
    dest->wx = src->wx;
    dest->xy = src->xy;
    dest->yy = src->yy;
    dest->zy = src->zy;
    dest->wy = src->wy;
    dest->xz = src->xz;
    dest->yz = src->yz;
    dest->zz = src->zz;
    dest->wz = src->wz;
    dest->xw = src->xw;
    dest->yw = src->yw;
    dest->zw = src->zw;
    dest->ww = src->ww;
    dest->xz = src->xz;
    dest->yz = src->yz;
    dest->zz = src->zz;
    dest->wz = src->wz;
    dest->xw = src->xw;
    dest->yw = src->yw;
    dest->zw = src->zw;
    dest->ww = src->ww;
}

/**
 * @brief Converts fixed-point RSP-compatible matrix to an MtxF.
 *
 * @param[in] src mtx to convert
 * @param[out] dest MtxF to output to
 *
 * @remark original name: "Matrix_MtxtoMtxF"
 */
void Matrix_MtxToMtxF(Mtx* src, MtxF* dest) {
    guMtxL2F(dest, src);
}

// Unused
/**
 * @brief Calculates mf * (src,1) and writes its components to dest.
 *
 * This is the same as Matrix_MultVec3f() but using a specified matrix rather than the current one; the same
 * assumptions apply.
 *
 * @param[in] src input vector
 * @param[out] dest output vector
 * @param[in] mf matrix to multiply by
 */
void Matrix_MultVec3fExt(Vec3f* src, Vec3f* dest, MtxF* mf) {
    dest->x = mf->xw + (mf->xx * src->x + mf->xy * src->y + mf->xz * src->z);
    dest->y = mf->yw + (mf->yx * src->x + mf->yy * src->y + mf->yz * src->z);
    dest->z = mf->zw + (mf->zx * src->x + mf->zy * src->y + mf->zz * src->z);
}

/**
 * @brief Overwrite the linear part of mf with its transpose (ignores the translational part).
 *
 * Viz.,
 *
 * \f[
 *  \begin{pmatrix}
 *      A & b \\
 *      0 & 1
 *  \end{pmatrix}
 *  \longrightarrow
 *  \begin{pmatrix}
 *      A^T & b \\
 *      0 & 1
 *  \end{pmatrix}
 * \f]
 *
 * @param[in,out] mf matrix to transpose
 *
 * @remark original name: "Matrix_reverse"
 */
void Matrix_Transpose(MtxF* mf) {
    f32 temp;

    temp = mf->yx;
    mf->yx = mf->xy;
    mf->xy = temp;

    temp = mf->zx;
    mf->zx = mf->xz;
    mf->xz = temp;

    temp = mf->zy;
    mf->zy = mf->yz;
    mf->yz = temp;
}

/**
 * @brief Decompose the linear part A of current into B * S, where B has normalised columns and S is diagonal, and
 * replace B by `mf`.
 *
 * Since B is typically a rotation matrix, and the linear part R * S to `mf` * S, this operation can be
 * seen as replacing the B rotation with `mf`, hence the function name.
 *
 * @param[in] mf matrix whose linear part will replace the normalised part of A.
 */
void Matrix_ReplaceRotation(MtxF* mf) {
    FrameInterpolation_RecordMatrixReplaceRotation(mf);
    MtxF* cmf = sCurrentMatrix;
    f32 acc;
    f32 component;
    f32 curColNorm;

    // compute the Euclidean norm of the first column of the current matrix
    acc = cmf->xx;
    acc *= acc;
    component = cmf->yx;
    acc += SQ(component);
    component = cmf->zx;
    acc += SQ(component);
    curColNorm = sqrtf(acc);

    cmf->xx = mf->xx * curColNorm;
    cmf->yx = mf->yx * curColNorm;
    cmf->zx = mf->zx * curColNorm;

    // second column
    acc = cmf->xy;
    acc *= acc;
    component = cmf->yy;
    acc += SQ(component);
    component = cmf->zy;
    acc += SQ(component);
    curColNorm = sqrtf(acc);

    cmf->xy = mf->xy * curColNorm;
    cmf->yy = mf->yy * curColNorm;
    cmf->zy = mf->zy * curColNorm;

    // third column
    acc = cmf->xz;
    acc *= acc;
    component = cmf->yz;
    acc += SQ(component);
    component = cmf->zz;
    acc += SQ(component);
    curColNorm = sqrtf(acc);

    cmf->xz = mf->xz * curColNorm;
    cmf->yz = mf->yz * curColNorm;
    cmf->zz = mf->zz * curColNorm;
}

/**
 * @brief Extract the YXZ Tait-Bryan rotation angles from the linear part \f$ A \f$ of a matrix.
 *
 * \f$ A \f$ should have orthogonal columns; the most general matrix of this form can be written as \f$ RS \f$
 * with \f$ S \f$ a scale matrix.
 *
 * If A has columns with the same norm (such as if it is just a rotation matrix), it is sufficient (and faster) to use
 * `nonUniformScale` off: `nonUniformScale` being set enables extraction of the angles from a matrix with columns that
 * are orthogonal but have different scales, at the cost of requiring extra calculation.
 *
 * @param[in] src Matrix to extract angles from.
 * @param[out] dest vector to write angles to.
 * @param[in] nonUniformScale boolean: true enables handling matrices with differently-scaled columns.
 *
 * @remark original name: "Matrix_to_rotate_new"?
 */
void Matrix_MtxFToYXZRot(MtxF* src, Vec3s* dest, s32 nonUniformScale) {
    f32 temp;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    temp = src->xz;
    temp *= temp;
    temp += SQ(src->zz);
    dest->x = Math_Atan2S(-src->yz, sqrtf(temp));

    if ((dest->x == 0x4000) || (dest->x == -0x4000)) {
        // cos(x) = 0 if either of these is true, and we get gimbal locking
        // (https://en.wikipedia.org/wiki/Gimbal_lock#Loss_of_a_degree_of_freedom_with_Euler_angles); fix z to make y
        // well-defined.
        dest->z = 0;

        dest->y = Math_Atan2S(-src->zx, src->xx);
    } else {
        dest->y = Math_Atan2S(src->xz, src->zz);

        if (!nonUniformScale) {
            // assume the columns have the same normalisation
            dest->z = Math_Atan2S(src->yx, src->yy);
        } else {
            temp = src->xx;
            temp2 = src->zx;
            temp3 = src->zy;

            // find norm of the first column
            temp *= temp;
            temp += SQ(temp2);
            temp2 = src->yx;
            temp += SQ(temp2);
            // temp = xx^2+zx^2+yx^2 == 1 for a rotation matrix
            temp = sqrtf(temp);
            temp = temp2 / temp; // yx in normalised column

            // find norm of the second column
            temp2 = src->xy;
            temp2 *= temp2;
            temp2 += SQ(temp3);
            temp3 = src->yy;
            temp2 += SQ(temp3);
            // temp2 = xy^2+zy^2+yy^2 == 1 for a rotation matrix
            temp2 = sqrtf(temp2);
            temp2 = temp3 / temp2; // yy in normalised column

            // for a rotation matrix, temp == yx and temp2 == yy which is the same as in the !nonUniformScale branch
            dest->z = Math_Atan2S(temp, temp2);
        }
    }
}

/**
 * @brief Extract the ZYX Tait-Bryan rotation angles from the linear part \f$ A \f$ of a matrix.
 *
 * \f$ A \f$ should have orthogonal columns; the most general matrix of this form can be written as \f$ RS \f$
 * with \f$ S \f$ a scale matrix.
 *
 * If A has columns with the same norm (such as if it is just a rotation matrix), it is sufficient (and faster) to use
 * `nonUniformScale` off: `nonUniformScale` being set enables extraction of the angles from a matrix with columns that
 * are orthogonal but have different scales, at the cost of requiring extra calculation.
 *
 * @param[in] src Matrix to extract angles from.
 * @param[out] dest vector to write angles to.
 * @param[in] nonUniformScale boolean: true enables handling matrices with unnormalised columns.
 *
 * @remark original name: "Matrix_to_rotate2_new"?
 *
 * See Matrix_MtxFToYXZRot() for full inline documentation.
 */
void Matrix_MtxFToZYXRot(MtxF* src, Vec3s* dest, s32 nonUniformScale) {
    f32 temp;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    temp = src->xx;
    temp *= temp;
    temp += SQ(src->yx);
    dest->y = Math_Atan2S(-src->zx, sqrtf(temp));

    if ((dest->y == 0x4000) || (dest->y == -0x4000)) {
        dest->x = 0;
        dest->z = Math_Atan2S(-src->xy, src->yy);
    } else {
        dest->z = Math_Atan2S(src->yx, src->xx);

        if (!nonUniformScale) {
            dest->x = Math_Atan2S(src->zy, src->zz);
        } else {
            temp = src->xy;
            temp2 = src->yy;
            temp3 = src->yz;

            temp *= temp;
            temp += SQ(temp2);
            temp2 = src->zy;
            temp += SQ(temp2);
            temp = sqrtf(temp);
            temp = temp2 / temp;

            temp2 = src->xz;
            temp2 *= temp2;
            temp2 += SQ(temp3);
            temp3 = src->zz;
            temp2 += SQ(temp3);
            temp2 = sqrtf(temp2);
            temp2 = temp3 / temp2;

            dest->x = Math_Atan2S(temp, temp2);
        }
    }
}

/**
 * @brief Rotate by `angle` radians about `axis`, which is assumed to be a unit vector.
 *
 * @param angle rotation angle (radians).
 * @param axis axis about which to rotate, must be a unit vector.
 * @param mode APPLY or NEW.
 *
 * @remark original name may have been "Matrix_RotateVector", but clashed with the next function.
 */
void Matrix_RotateAxisF(f32 angle, Vec3f* axis, MatrixMode mode) {
    FrameInterpolation_RecordMatrixRotateAxis(angle, axis, mode);
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 versin;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;
    f32 temp5;

    if (mode == MTXMODE_APPLY) {
        if (angle != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(angle);
            cos = cosf(angle);

            temp1 = cmf->xx;
            temp2 = cmf->xy;
            temp3 = cmf->xz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->xx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->xy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->xz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);

            temp1 = cmf->yx;
            temp2 = cmf->yy;
            temp3 = cmf->yz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->yx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->yy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->yz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);

            temp1 = cmf->zx;
            temp2 = cmf->zy;
            temp3 = cmf->zz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->zx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->zy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->zz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);
        }
    } else {
        cmf = sCurrentMatrix;

        if (angle != 0) {
            sin = sinf(angle);
            cos = cosf(angle);
            versin = 1.0f - cos;

            cmf->xx = axis->x * axis->x * versin + cos;
            cmf->yy = axis->y * axis->y * versin + cos;
            cmf->zz = axis->z * axis->z * versin + cos;

            if (0) {}

            temp2 = axis->x * versin * axis->y;
            temp3 = axis->z * sin;
            cmf->yx = temp2 + temp3;
            cmf->xy = temp2 - temp3;

            temp2 = axis->x * versin * axis->z;
            temp3 = axis->y * sin;
            cmf->zx = temp2 - temp3;
            cmf->xz = temp2 + temp3;

            temp2 = axis->y * versin * axis->z;
            temp3 = axis->x * sin;
            cmf->zy = temp2 + temp3;
            cmf->yz = temp2 - temp3;

            cmf->wx = cmf->wy = cmf->wz = cmf->xw = cmf->yw = cmf->zw = 0.0f;
            cmf->ww = 1.0f;
        } else {
            cmf->xx = 1.0f;
            cmf->yx = 0.0f;
            cmf->zx = 0.0f;
            cmf->wx = 0.0f;
            cmf->xy = 0.0f;
            cmf->yy = 1.0f;
            cmf->zy = 0.0f;
            cmf->wy = 0.0f;
            cmf->xz = 0.0f;
            cmf->yz = 0.0f;
            cmf->zz = 1.0f;
            cmf->wz = 0.0f;
            cmf->xw = 0.0f;
            cmf->yw = 0.0f;
            cmf->zw = 0.0f;
            cmf->ww = 1.0f;
        }
    }
}

/**
 * @brief Rotate by binary angle `angle` about `axis`, which is assumed to be a unit vector.
 *
 * @param angle rotation angle (binary).
 * @param axis axis about which to rotate, must be a unit vector.
 * @param mode APPLY or NEW.
 *
 * @remark original name: "Matrix_RotateVector"
 */
void Matrix_RotateAxisS(s16 angle, Vec3f* axis, MatrixMode mode) {
    MtxF* cmf;
    f32 cos;
    f32 sin;
    f32 versin;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    if (mode == MTXMODE_APPLY) {
        if (angle != 0) {
            cmf = sCurrentMatrix;

            sin = Math_SinS(angle);
            cos = Math_CosS(angle);

            temp1 = cmf->xx;
            temp2 = cmf->xy;
            temp3 = cmf->xz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->xx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->xy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->xz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);

            temp1 = cmf->yx;
            temp2 = cmf->yy;
            temp3 = cmf->yz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->yx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->yy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->yz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);

            temp1 = cmf->zx;
            temp2 = cmf->zy;
            temp3 = cmf->zz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->zx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->zy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->zz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);
        }
    } else {
        cmf = sCurrentMatrix;

        if (angle != 0) {
            sin = Math_SinS(angle);
            cos = Math_CosS(angle);
            versin = 1.0f - cos;

            cmf->xx = axis->x * axis->x * versin + cos;
            cmf->yy = axis->y * axis->y * versin + cos;
            cmf->zz = axis->z * axis->z * versin + cos;

            if (0) {}

            temp2 = axis->x * versin * axis->y;
            temp3 = axis->z * sin;
            cmf->yx = temp2 + temp3;
            cmf->xy = temp2 - temp3;

            temp2 = axis->x * versin * axis->z;
            temp3 = axis->y * sin;
            cmf->zx = temp2 - temp3;
            cmf->xz = temp2 + temp3;

            temp2 = axis->y * versin * axis->z;
            temp3 = axis->x * sin;
            cmf->zy = temp2 + temp3;
            cmf->yz = temp2 - temp3;

            cmf->wx = cmf->wy = cmf->wz = cmf->xw = cmf->yw = cmf->zw = 0.0f;
            cmf->ww = 1.0f;
        } else {
            cmf->xx = 1.0f;
            cmf->yx = 0.0f;
            cmf->zx = 0.0f;
            cmf->wx = 0.0f;
            cmf->xy = 0.0f;
            cmf->yy = 1.0f;
            cmf->zy = 0.0f;
            cmf->wy = 0.0f;
            cmf->xz = 0.0f;
            cmf->yz = 0.0f;
            cmf->zz = 1.0f;
            cmf->wz = 0.0f;
            cmf->xw = 0.0f;
            cmf->yw = 0.0f;
            cmf->zw = 0.0f;
            cmf->ww = 1.0f;
        }
    }
}

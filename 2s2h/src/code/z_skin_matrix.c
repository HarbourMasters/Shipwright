#include "z64skin_matrix.h"

#include "gfx.h"
#include "macros.h"
#include "z64math.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

MtxF sMtxFClear = { {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
} };

/**
 * Multiplies a 4 component row vector [ src , 1 ] by the matrix mf and writes the resulting 4 components to xyzDest
 * and wDest.
 *
 * \f[ [\texttt{xyzDest}, \texttt{wDest}] = [\texttt{src}, 1] \cdot [mf] \f]
 */
void SkinMatrix_Vec3fMtxFMultXYZW(MtxF* mf, Vec3f* src, Vec3f* xyzDest, f32* wDest) {
    xyzDest->x = mf->xw + ((src->x * mf->xx) + (src->y * mf->xy) + (src->z * mf->xz));
    xyzDest->y = mf->yw + ((src->x * mf->yx) + (src->y * mf->yy) + (src->z * mf->yz));
    xyzDest->z = mf->zw + ((src->x * mf->zx) + (src->y * mf->zy) + (src->z * mf->zz));
    *wDest = mf->ww + ((src->x * mf->wx) + (src->y * mf->wy) + (src->z * mf->wz));
}

/**
 * Multiplies a 4 component row vector [ src , 1 ] by the matrix mf and writes the resulting xyz components to dest.
 *
 * \f[ [\texttt{dest}, -] = [\texttt{src}, 1] \cdot [mf] \f]
 */
void SkinMatrix_Vec3fMtxFMultXYZ(MtxF* mf, Vec3f* src, Vec3f* dest) {
    f32 mx = mf->xx;
    f32 my = mf->xy;
    f32 mz = mf->xz;
    f32 mw = mf->xw;

    dest->x = mw + ((src->x * mx) + (src->y * my) + (src->z * mz));

    mx = mf->yx;
    my = mf->yy;
    mz = mf->yz;
    mw = mf->yw;
    dest->y = mw + ((src->x * mx) + (src->y * my) + (src->z * mz));

    mx = mf->zx;
    my = mf->zy;
    mz = mf->zz;
    mw = mf->zw;
    dest->z = mw + ((src->x * mx) + (src->y * my) + (src->z * mz));
}

/**
 * Matrix multiplication, dest = mfA * mfB.
 * mfA and dest should not be the same matrix.
 */
void SkinMatrix_MtxFMtxFMult(MtxF* mfB, MtxF* mfA, MtxF* dest) {
    f32 rx;
    f32 ry;
    f32 rz;
    f32 rw;

    //---COL1---
    f32 cx = mfB->xx;
    f32 cy = mfB->xy;
    f32 cz = mfB->xz;
    f32 cw = mfB->xw;
    //--------

    rx = mfA->xx;
    ry = mfA->yx;
    rz = mfA->zx;
    rw = mfA->wx;
    dest->xx = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xy;
    ry = mfA->yy;
    rz = mfA->zy;
    rw = mfA->wy;
    dest->xy = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xz;
    ry = mfA->yz;
    rz = mfA->zz;
    rw = mfA->wz;
    dest->xz = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xw;
    ry = mfA->yw;
    rz = mfA->zw;
    rw = mfA->ww;
    dest->xw = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    //---2Col---
    cx = mfB->yx;
    cy = mfB->yy;
    cz = mfB->yz;
    cw = mfB->yw;
    //--------
    rx = mfA->xx;
    ry = mfA->yx;
    rz = mfA->zx;
    rw = mfA->wx;
    dest->yx = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xy;
    ry = mfA->yy;
    rz = mfA->zy;
    rw = mfA->wy;
    dest->yy = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xz;
    ry = mfA->yz;
    rz = mfA->zz;
    rw = mfA->wz;
    dest->yz = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xw;
    ry = mfA->yw;
    rz = mfA->zw;
    rw = mfA->ww;
    dest->yw = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    //---3Col---
    cx = mfB->zx;
    cy = mfB->zy;
    cz = mfB->zz;
    cw = mfB->zw;
    //--------
    rx = mfA->xx;
    ry = mfA->yx;
    rz = mfA->zx;
    rw = mfA->wx;
    dest->zx = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xy;
    ry = mfA->yy;
    rz = mfA->zy;
    rw = mfA->wy;
    dest->zy = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xz;
    ry = mfA->yz;
    rz = mfA->zz;
    rw = mfA->wz;
    dest->zz = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xw;
    ry = mfA->yw;
    rz = mfA->zw;
    rw = mfA->ww;
    dest->zw = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    //---4Col---
    cx = mfB->wx;
    cy = mfB->wy;
    cz = mfB->wz;
    cw = mfB->ww;
    //--------
    rx = mfA->xx;
    ry = mfA->yx;
    rz = mfA->zx;
    rw = mfA->wx;
    dest->wx = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xy;
    ry = mfA->yy;
    rz = mfA->zy;
    rw = mfA->wy;
    dest->wy = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xz;
    ry = mfA->yz;
    rz = mfA->zz;
    rw = mfA->wz;
    dest->wz = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);

    rx = mfA->xw;
    ry = mfA->yw;
    rz = mfA->zw;
    rw = mfA->ww;
    dest->ww = (cx * rx) + (cy * ry) + (cz * rz) + (cw * rw);
}

/**
 * "Clear" in this file means the identity matrix.
 */
void SkinMatrix_GetClear(MtxF** mfp) {
    *mfp = &sMtxFClear;
}

void SkinMatrix_Clear(MtxF* mf) {
    mf->xx = 1.0f;
    mf->yx = 0.0f;
    mf->zx = 0.0f;
    mf->wx = 0.0f;
    mf->xy = 0.0f;
    mf->yy = 1.0f;
    mf->zy = 0.0f;
    mf->wy = 0.0f;
    mf->xz = 0.0f;
    mf->yz = 0.0f;
    mf->zz = 1.0f;
    mf->wz = 0.0f;
    mf->xw = 0.0f;
    mf->yw = 0.0f;
    mf->zw = 0.0f;
    mf->ww = 1.0f;
}

void SkinMatrix_MtxFCopy(MtxF* src, MtxF* dest) {
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
}

/**
 * Inverts a matrix using a slight modification of the Gauss-Jordan method
 * (column operations instead of row operations).
 * returns 0 if successfully inverted
 * returns 2 if matrix non-invertible (0 determinant)
 */
s32 SkinMatrix_Invert(MtxF* src, MtxF* dest) {
    MtxF mfCopy;
    s32 i;
    f32 temp2;
    s32 thisRow;
    s32 thisCol;

    SkinMatrix_MtxFCopy(src, &mfCopy);
    SkinMatrix_Clear(dest);

    for (thisRow = 0; thisRow < 4; thisRow++) {
        thisCol = thisRow;
        while ((thisCol < 4) && (fabsf(mfCopy.mf[thisRow][thisCol]) < 0.0005f)) {
            thisCol++;
        }
        if (thisCol == 4) {
            // reaching col = 4 means the row is either all 0 or a duplicate row.
            // therefore singular matrix (0 determinant).
            return 2;
        }
        if (thisCol != thisRow) { // responsible for swapping columns if zero on diagonal
            for (i = 0; i < 4; i++) {
                SWAP(f32, mfCopy.mf[i][thisCol], mfCopy.mf[i][thisRow]);
                SWAP(f32, dest->mf[i][thisCol], dest->mf[i][thisRow]);
            }
        }

        // Scale this whole column s.t. the diag element = 1
        temp2 = mfCopy.mf[thisRow][thisRow];
        for (i = 0; i < 4; i++) {
            mfCopy.mf[i][thisRow] /= temp2;
            dest->mf[i][thisRow] /= temp2;
        }

        for (thisCol = 0; thisCol < 4; thisCol++) {
            if (thisCol != thisRow) {
                temp2 = mfCopy.mf[thisRow][thisCol];
                for (i = 0; i < 4; i++) {
                    mfCopy.mf[i][thisCol] -= mfCopy.mf[i][thisRow] * temp2;
                    dest->mf[i][thisCol] -= dest->mf[i][thisRow] * temp2;
                }
            }
        }
    }

    return 0;
}

/**
 * Produces a matrix which scales x,y,z components of vectors or x,y,z rows of matrices (when applied on LHS)
 */
void SkinMatrix_SetScale(MtxF* mf, f32 x, f32 y, f32 z) {
    mf->yx = 0.0f;
    mf->zx = 0.0f;
    mf->wx = 0.0f;
    mf->xy = 0.0f;
    mf->zy = 0.0f;
    mf->wy = 0.0f;
    mf->xz = 0.0f;
    mf->yz = 0.0f;
    mf->wz = 0.0f;
    mf->xw = 0.0f;
    mf->yw = 0.0f;
    mf->zw = 0.0f;
    mf->ww = 1.0f;
    mf->xx = x;
    mf->yy = y;
    mf->zz = z;
}

/**
 * Produces a rotation matrix = (roll rotation matrix) * (pitch rotation matrix) * (yaw rotation matrix)
 */
void SkinMatrix_SetRotateRPY(MtxF* mf, s16 roll, s16 pitch, s16 yaw) {
    f32 cos2;
    f32 sin = Math_SinS(yaw);
    f32 cos = Math_CosS(yaw);
    f32 xy;
    f32 sin2;
    f32 xz;
    f32 yy;
    f32 yz;

    mf->yy = cos;
    mf->xy = -sin;
    mf->wx = mf->wy = mf->wz = 0;
    mf->xw = mf->yw = mf->zw = 0;
    mf->ww = 1;

    if (pitch != 0) {
        sin2 = Math_SinS(pitch);
        cos2 = Math_CosS(pitch);

        mf->xx = cos * cos2;
        mf->xz = cos * sin2;

        mf->yx = sin * cos2;
        mf->yz = sin * sin2;
        mf->zx = -sin2;
        mf->zz = cos2;
    } else {
        mf->xx = cos;
        if (1) {}
        if (1) {}
        xz = sin; // required to match
        mf->yx = sin;
        mf->zx = mf->xz = mf->yz = 0;
        mf->zz = 1;
    }

    if (roll != 0) {
        sin2 = Math_SinS(roll);
        cos2 = Math_CosS(roll);

        xy = mf->xy;
        xz = mf->xz;
        mf->xy = (xy * cos2) + (xz * sin2);
        mf->xz = (xz * cos2) - (xy * sin2);

        if (1) {}
        yz = mf->yz;
        yy = mf->yy;
        mf->yy = (yy * cos2) + (yz * sin2);
        mf->yz = (yz * cos2) - (yy * sin2);

        if (cos2) {}
        mf->zy = mf->zz * sin2;
        mf->zz = mf->zz * cos2;
    } else {
        mf->zy = 0;
    }
}

/**
 * Produces a rotation matrix = (yaw rotation matrix) * (roll rotation matrix) * (pitch rotation matrix)
 */
void SkinMatrix_SetRotateYRP(MtxF* mf, s16 yaw, s16 roll, s16 pitch) {
    f32 cos2;
    f32 sin;
    f32 cos;
    f32 zx;
    f32 sin2;
    f32 zy;
    f32 xx;
    f32 xy;

    sin = Math_SinS(roll);
    cos = Math_CosS(roll);

    mf->xx = cos;
    mf->zx = -sin;
    mf->wz = 0;
    mf->wy = 0;
    mf->wx = 0;
    mf->zw = 0;
    mf->yw = 0;
    mf->xw = 0;
    mf->ww = 1;

    if (yaw != 0) {
        sin2 = Math_SinS(yaw);
        cos2 = Math_CosS(yaw);

        mf->zz = cos * cos2;
        mf->zy = cos * sin2;

        mf->xz = sin * cos2;
        mf->xy = sin * sin2;
        mf->yz = -sin2;
        mf->yy = cos2;
    } else {
        mf->zz = cos;
        if (1) {}
        if (1) {}
        xy = sin; // required to match
        mf->xz = sin;
        mf->xy = mf->zy = mf->yz = 0;
        mf->yy = 1;
    }

    if (pitch != 0) {
        sin2 = Math_SinS(pitch);
        cos2 = Math_CosS(pitch);
        xx = mf->xx;
        xy = mf->xy;
        mf->xx = (xx * cos2) + (xy * sin2);
        mf->xy = xy * cos2 - (xx * sin2);
        if (1) {}
        zy = mf->zy;
        zx = mf->zx;
        mf->zx = (zx * cos2) + (zy * sin2);
        mf->zy = (zy * cos2) - (zx * sin2);
        if (cos2) {}
        mf->yx = mf->yy * sin2;
        mf->yy = mf->yy * cos2;
    } else {
        mf->yx = 0;
    }
}

/**
 * Produces a matrix which translates a vector by amounts in the x, y and z directions
 */
void SkinMatrix_SetTranslate(MtxF* mf, f32 x, f32 y, f32 z) {
    mf->yx = 0.0f;
    mf->zx = 0.0f;
    mf->wx = 0.0f;
    mf->xy = 0.0f;
    mf->zy = 0.0f;
    mf->wy = 0.0f;
    mf->xz = 0.0f;
    mf->yz = 0.0f;
    mf->wz = 0.0f;
    mf->xx = 1.0f;
    mf->yy = 1.0f;
    mf->zz = 1.0f;
    mf->ww = 1.0f;
    mf->xw = x;
    mf->yw = y;
    mf->zw = z;
}

/**
 * Produces a matrix which scales, then rotates (RPY), then translates a vector
 */
void SkinMatrix_SetScaleRotateRPYTranslate(MtxF* mf, f32 scaleX, f32 scaleY, f32 scaleZ, s16 roll, s16 pitch, s16 yaw,
                                           f32 dx, f32 dy, f32 dz) {
    MtxF mft1;
    MtxF mft2;

    SkinMatrix_SetTranslate(mf, dx, dy, dz);
    SkinMatrix_SetRotateRPY(&mft1, roll, pitch, yaw);
    SkinMatrix_MtxFMtxFMult(mf, &mft1, &mft2);
    SkinMatrix_SetScale(&mft1, scaleX, scaleY, scaleZ);
    SkinMatrix_MtxFMtxFMult(&mft2, &mft1, mf);
}

/**
 * Produces a matrix which scales, then rotates (YRP), then translates a vector
 */
void SkinMatrix_SetScaleRotateYRPTranslate(MtxF* mf, f32 scaleX, f32 scaleY, f32 scaleZ, s16 yaw, s16 roll, s16 pitch,
                                           f32 dx, f32 dy, f32 dz) {
    MtxF mft1;
    MtxF mft2;

    SkinMatrix_SetTranslate(mf, dx, dy, dz);
    SkinMatrix_SetRotateYRP(&mft1, yaw, roll, pitch);
    SkinMatrix_MtxFMtxFMult(mf, &mft1, &mft2);
    SkinMatrix_SetScale(&mft1, scaleX, scaleY, scaleZ);
    SkinMatrix_MtxFMtxFMult(&mft2, &mft1, mf);
}

/**
 * Produces a matrix which rotates (RPY), then translates a vector
 */
void SkinMatrix_SetRotateRPYTranslate(MtxF* mf, s16 roll, s16 pitch, s16 yaw, f32 dx, f32 dy, f32 dz) {
    MtxF mft1;
    MtxF mft2;

    SkinMatrix_SetTranslate(&mft2, dx, dy, dz);
    SkinMatrix_SetRotateRPY(&mft1, roll, pitch, yaw);
    SkinMatrix_MtxFMtxFMult(&mft2, &mft1, mf);
}

void SkinMatrix_Vec3fToVec3s(Vec3f* src, Vec3s* dest) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void SkinMatrix_Vec3sToVec3f(Vec3s* src, Vec3f* dest) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void SkinMatrix_MtxFToMtx(MtxF* src, Mtx* dest) {
    FrameInterpolation_RecordSkinMatrixMtxFToMtx(src, dest);
    // 2S2H [Port] For compatibility with modern systems this has been changed to use guMtxF2L
    guMtxF2L(src, dest);
}

Mtx* SkinMatrix_MtxFToNewMtx(GraphicsContext* gfxCtx, MtxF* src) {
    Mtx* mtx = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));

    if (mtx == NULL) {
        return NULL;
    }

    SkinMatrix_MtxFToMtx(src, mtx);
    return mtx;
}

/**
 * Produces a matrix which rotates vectors by angle a around a unit vector with components (x,y,z)
 */
void SkinMatrix_SetRotateAroundVec(MtxF* mf, s16 a, f32 x, f32 y, f32 z) {
    f32 sinA;
    f32 cosA;
    f32 xx;
    f32 yy;
    f32 zz;
    f32 xy;
    f32 yz;
    f32 xz;
    s32 pad;

    sinA = Math_SinS(a);
    cosA = Math_CosS(a);

    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    xz = x * z;

    mf->xx = (1.0f - xx) * cosA + xx;
    mf->yx = (1.0f - cosA) * xy + z * sinA;
    mf->zx = (1.0f - cosA) * xz - y * sinA;
    mf->wx = 0.0f;

    mf->xy = (1.0f - cosA) * xy - z * sinA;
    mf->yy = (1.0f - yy) * cosA + yy;
    mf->zy = (1.0f - cosA) * yz + x * sinA;
    mf->wy = 0.0f;

    mf->xz = (1.0f - cosA) * xz + y * sinA;
    mf->yz = (1.0f - cosA) * yz - x * sinA;
    mf->zz = (1.0f - zz) * cosA + zz;
    mf->wz = 0.0f;

    mf->xw = mf->yw = mf->zw = 0.0f;
    mf->ww = 1.0f;
}

void SkinMatrix_SetXRotation(MtxF* mf, s16 a) {
    f32 sinA;
    f32 cosA;

    if (a != 0) {
        sinA = Math_SinS(a);
        cosA = Math_CosS(a);
    } else {
        sinA = 0.0f;
        cosA = 1.0f;
    }

    mf->yx = 0.0f;
    mf->zx = 0.0f;
    mf->wx = 0.0f;

    mf->xy = 0.0f;
    mf->wy = 0.0f;

    mf->xz = 0.0f;
    mf->wz = 0.0f;

    mf->xw = 0.0f;
    mf->yw = 0.0f;
    mf->zw = 0.0f;

    mf->xx = 1.0f;
    mf->ww = 1.0f;

    mf->yy = cosA;
    mf->zz = cosA;
    mf->zy = sinA;
    mf->yz = -sinA;
}

void SkinMatrix_MulXRotation(MtxF* mf, s16 a) {
    f32 sinA;
    f32 cosA;
    f32 ry;
    f32 rz;

    if (a != 0) {
        sinA = Math_SinS(a);
        cosA = Math_CosS(a);

        ry = mf->xy;
        rz = mf->xz;
        mf->xy = ry * cosA + rz * sinA;
        mf->xz = rz * cosA - ry * sinA;

        ry = mf->yy;
        rz = mf->yz;
        mf->yy = ry * cosA + rz * sinA;
        mf->yz = rz * cosA - ry * sinA;

        ry = mf->zy;
        rz = mf->zz;
        mf->zy = ry * cosA + rz * sinA;
        mf->zz = rz * cosA - ry * sinA;

        ry = mf->wy;
        rz = mf->wz;
        mf->wy = ry * cosA + rz * sinA;
        mf->wz = rz * cosA - ry * sinA;
    }
}

void SkinMatrix_SetYRotation(MtxF* mf, s16 a) {
    f32 sinA;
    f32 cosA;

    if (a != 0) {
        sinA = Math_SinS(a);
        cosA = Math_CosS(a);
    } else {
        sinA = 0.0f;
        cosA = 1.0f;
    }

    mf->yx = 0.0f;
    mf->wx = 0.0f;

    mf->xy = 0.0f;
    mf->zy = 0.0f;
    mf->wy = 0.0f;

    mf->yz = 0.0f;
    mf->wz = 0.0f;

    mf->xw = 0.0f;
    mf->yw = 0.0f;
    mf->zw = 0.0f;

    mf->yy = 1.0f;
    mf->ww = 1.0f;

    mf->xx = cosA;
    mf->zz = cosA;
    mf->zx = -sinA;
    mf->xz = sinA;
}

void SkinMatrix_MulYRotation(MtxF* mf, s16 a) {
    f32 sinA;
    f32 cosA;
    f32 rx;
    f32 rz;

    if (a != 0) {
        sinA = Math_SinS(a);
        cosA = Math_CosS(a);

        rx = mf->xx;
        rz = mf->xz;
        mf->xx = rx * cosA - rz * sinA;
        mf->xz = rx * sinA + rz * cosA;

        rx = mf->yx;
        rz = mf->yz;
        mf->yx = rx * cosA - rz * sinA;
        mf->yz = rx * sinA + rz * cosA;

        rx = mf->zx;
        rz = mf->zz;
        mf->zx = rx * cosA - rz * sinA;
        mf->zz = rx * sinA + rz * cosA;

        rx = mf->wx;
        rz = mf->wz;
        mf->wx = rx * cosA - rz * sinA;
        mf->wz = rx * sinA + rz * cosA;
    }
}

void SkinMatrix_SetZRotation(MtxF* mf, s16 a) {
    f32 sinA;
    f32 cosA;

    if (a != 0) {
        sinA = Math_SinS(a);
        cosA = Math_CosS(a);
    } else {
        sinA = 0.0f;
        cosA = 1.0f;
    }

    mf->zx = 0.0f;
    mf->wx = 0.0f;

    mf->zy = 0.0f;
    mf->wy = 0.0f;

    mf->xz = 0.0f;
    mf->yz = 0.0f;
    mf->wz = 0.0f;

    mf->xw = 0.0f;
    mf->yw = 0.0f;
    mf->zw = 0.0f;

    mf->zz = 1.0f;
    mf->ww = 1.0f;

    mf->xx = cosA;
    mf->yy = cosA;
    mf->yx = sinA;
    mf->xy = -sinA;
}

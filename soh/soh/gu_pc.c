#include <math.h>
#include "z64.h"

void guMtxF2L(float mf[4][4], Mtx* m) {
    unsigned int r, c;
    s32 tmp1;
    s32 tmp2;
    s32* m1 = &m->m[0][0];
    s32* m2 = &m->m[2][0];
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 2; c++) {
            tmp1 = mf[r][2 * c] * 65536.0f;
            tmp2 = mf[r][2 * c + 1] * 65536.0f;
            *m1++ = (tmp1 & 0xffff0000) | ((tmp2 >> 0x10) & 0xffff);
            *m2++ = ((tmp1 << 0x10) & 0xffff0000) | (tmp2 & 0xffff);
        }
    }
}

void guMtxL2F(float mf[4][4], Mtx* m) {
    unsigned int r, c;
    u32 tmp1;
    u32 tmp2;
    u32* m1;
    u32* m2;
    s32 stmp1, stmp2;
    m1 = (u32*)&m->m[0][0];
    m2 = (u32*)&m->m[2][0];
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 2; c++) {
            tmp1 = (*m1 & 0xffff0000) | ((*m2 >> 0x10) & 0xffff);
            tmp2 = ((*m1++ << 0x10) & 0xffff0000) | (*m2++ & 0xffff);
            stmp1 = *(s32*)&tmp1;
            stmp2 = *(s32*)&tmp2;
            mf[r][c * 2 + 0] = stmp1 / 65536.0f;
            mf[r][c * 2 + 1] = stmp2 / 65536.0f;
        }
    }
}

void guMtxIdentF(f32 mf[4][4]) {
    unsigned int r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            if (r == c) {
                mf[r][c] = 1.0f;
            } else {
                mf[r][c] = 0.0f;
            }
        }
    }
}

void guMtxIdent(Mtx* m) {
    guMtxIdentF(m->m);
}

void guTranslateF(float m[4][4], float x, float y, float z) {
    guMtxIdentF(m);
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
}
void guTranslate(Mtx* m, float x, float y, float z) {
    float mf[4][4];
    guTranslateF(mf, x, y, z);
    guMtxF2L(mf, m);
}

void guScaleF(float mf[4][4], float x, float y, float z) {
    guMtxIdentF(mf);
    mf[0][0] = x;
    mf[1][1] = y;
    mf[2][2] = z;
    mf[3][3] = 1.0;
}
void guScale(Mtx* m, float x, float y, float z) {
    float mf[4][4];
    guScaleF(mf, x, y, z);
    guMtxF2L(mf, m);
}

void guNormalize(f32* x, f32* y, f32* z) {
    f32 tmp = 1.0f / sqrtf(*x * *x + *y * *y + *z * *z);
    *x = *x * tmp;
    *y = *y * tmp;
    *z = *z * tmp;
}

#include "ultra64.h"

void guOrthoF(float m[4][4], float l, float r, float b, float t, float n, float f, float scale) {
    int i;
    int j;

    guMtxIdentF(m);
    m[0][0] = 2 / (r - l);
    m[1][1] = 2 / (t - b);
    m[2][2] = -2 / (f - n);
    m[3][0] = -(r + l) / (r - l);
    m[3][1] = -(t + b) / (t - b);
    m[3][2] = -(f + n) / (f - n);
    m[3][3] = 1;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            m[i][j] *= scale;
        }
    }
}

void guOrtho(Mtx* m, float l, float r, float b, float t, float n, float f, float scale) {
    float mf[4][4];

    guOrthoF(mf, l, r, b, t, n, f, scale);
    guMtxF2L(mf, m);
}

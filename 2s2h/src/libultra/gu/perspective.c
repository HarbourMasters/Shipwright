#include "ultra64.h"

void guPerspectiveF(float mf[4][4], u16* perspNorm, float fovy, float aspect, float near, float far, float scale) {
    float cot;
    int i;
    int j;

    guMtxIdentF(mf);
    fovy *= 3.1415926 / 180.0;
    cot = cosf(fovy / 2) / sinf(fovy / 2);

    mf[0][0] = cot / aspect;
    mf[1][1] = cot;
    mf[2][2] = (near + far) / (near - far);
    mf[2][3] = -1;
    mf[3][2] = 2 * near * far / (near - far);
    mf[3][3] = 0.0f;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            mf[i][j] *= scale;
        }
    }

    if (perspNorm != NULL) {
        if (near + far <= 2.0) {
            *perspNorm = 65535;
        } else {
            *perspNorm = (double)(1 << 17) / (near + far);
            if (*perspNorm <= 0) {
                *perspNorm = 1;
            }
        }
    }
}

void guPerspective(Mtx* m, u16* perspNorm, float fovy, float aspect, float near, float far, float scale) {
    float mf[4][4];

    guPerspectiveF(mf, perspNorm, fovy, aspect, near, far, scale);
    guMtxF2L(mf, m);
}

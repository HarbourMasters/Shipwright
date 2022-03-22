#include "global.h"

#define FTOFRAC8(x) ((s32)MIN(((x) * (128.0f)), 127.0f) & 0xFF)

/**
 * guLookAtHiliteF
 * This function creates the viewing matrix (floating point) and sets the LookAt/Hilite structures
 **/
void guLookAtHiliteF(f32 mf[4][4], LookAt* l, Hilite* h, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt,
                     f32 xUp, f32 yUp, f32 zUp, f32 xl1, f32 yl1, f32 zl1, /* light 1 direction */
                     f32 xl2, f32 yl2, f32 zl2,                            /* light 2 direction */
                     s32 hiliteWidth, s32 hiliteHeight)                    /* size of hilite texture */
{
    f32 length;
    f32 xLook;
    f32 yLook;
    f32 zLook;
    f32 xRight;
    f32 yRight;
    f32 zRight;
    f32 xHilite;
    f32 yHilite;
    f32 zHilite;

    guMtxIdentF(mf);

    xLook = xAt - xEye;
    yLook = yAt - yEye;
    zLook = zAt - zEye;
    length = -1.0 / sqrtf(xLook * xLook + yLook * yLook + zLook * zLook);
    xLook *= length;
    yLook *= length;
    zLook *= length;

    xRight = yUp * zLook - zUp * yLook;
    yRight = zUp * xLook - xUp * zLook;
    zRight = xUp * yLook - yUp * xLook;
    length = 1.0 / sqrtf(xRight * xRight + yRight * yRight + zRight * zRight);
    xRight *= length;
    yRight *= length;
    zRight *= length;

    xUp = yLook * zRight - zLook * yRight;
    yUp = zLook * xRight - xLook * zRight;
    zUp = xLook * yRight - yLook * xRight;
    length = 1.0 / sqrtf(xUp * xUp + yUp * yUp + zUp * zUp);
    xUp *= length;
    yUp *= length;
    zUp *= length;

    /* hilite vectors */

    length = 1.0 / sqrtf(xl1 * xl1 + yl1 * yl1 + zl1 * zl1);
    xl1 *= length;
    yl1 *= length;
    zl1 *= length;

    xHilite = xl1 + xLook;
    yHilite = yl1 + yLook;
    zHilite = zl1 + zLook;

    length = sqrtf(xHilite * xHilite + yHilite * yHilite + zHilite * zHilite);

    if (length > 0.1) {
        length = 1.0 / length;
        xHilite *= length;
        yHilite *= length;
        zHilite *= length;

        h->h.x1 = hiliteWidth * 4 + (xHilite * xRight + yHilite * yRight + zHilite * zRight) * hiliteWidth * 2;

        h->h.y1 = hiliteHeight * 4 + (xHilite * xUp + yHilite * yUp + zHilite * zUp) * hiliteHeight * 2;
    } else {
        h->h.x1 = hiliteWidth * 2;
        h->h.y1 = hiliteHeight * 2;
    }

    length = 1.0 / sqrtf(xl2 * xl2 + yl2 * yl2 + zl2 * zl2);
    xl2 *= length;
    yl2 *= length;
    zl2 *= length;

    xHilite = xl2 + xLook;
    yHilite = yl2 + yLook;
    zHilite = zl2 + zLook;
    length = sqrtf(xHilite * xHilite + yHilite * yHilite + zHilite * zHilite);
    if (length > 0.1) {
        length = 1.0 / length;
        xHilite *= length;
        yHilite *= length;
        zHilite *= length;

        h->h.x2 = hiliteWidth * 4 + (xHilite * xRight + yHilite * yRight + zHilite * zRight) * hiliteWidth * 2;

        h->h.y2 = hiliteHeight * 4 + (xHilite * xUp + yHilite * yUp + zHilite * zUp) * hiliteHeight * 2;
    } else {
        h->h.x2 = hiliteWidth * 2;
        h->h.y2 = hiliteHeight * 2;
    }

    /* reflectance vectors = Up and Right */

    l->l[0].l.dir[0] = FTOFRAC8(xRight);
    l->l[0].l.dir[1] = FTOFRAC8(yRight);
    l->l[0].l.dir[2] = FTOFRAC8(zRight);
    l->l[1].l.dir[0] = FTOFRAC8(xUp);
    l->l[1].l.dir[1] = FTOFRAC8(yUp);
    l->l[1].l.dir[2] = FTOFRAC8(zUp);
    l->l[0].l.col[0] = 0x00;
    l->l[0].l.col[1] = 0x00;
    l->l[0].l.col[2] = 0x00;
    l->l[0].l.pad1 = 0x00;
    l->l[0].l.colc[0] = 0x00;
    l->l[0].l.colc[1] = 0x00;
    l->l[0].l.colc[2] = 0x00;
    l->l[0].l.pad2 = 0x00;
    l->l[1].l.col[0] = 0x00;
    l->l[1].l.col[1] = 0x80;
    l->l[1].l.col[2] = 0x00;
    l->l[1].l.pad1 = 0x00;
    l->l[1].l.colc[0] = 0x00;
    l->l[1].l.colc[1] = 0x80;
    l->l[1].l.colc[2] = 0x00;
    l->l[1].l.pad2 = 0x00;

    mf[0][0] = xRight;
    mf[1][0] = yRight;
    mf[2][0] = zRight;
    mf[3][0] = -(xEye * xRight + yEye * yRight + zEye * zRight);

    mf[0][1] = xUp;
    mf[1][1] = yUp;
    mf[2][1] = zUp;
    mf[3][1] = -(xEye * xUp + yEye * yUp + zEye * zUp);

    mf[0][2] = xLook;
    mf[1][2] = yLook;
    mf[2][2] = zLook;
    mf[3][2] = -(xEye * xLook + yEye * yLook + zEye * zLook);

    mf[0][3] = 0;
    mf[1][3] = 0;
    mf[2][3] = 0;
    mf[3][3] = 1;
}

/**
 * guLookAtHilite
 * This function creates the viewing matrix (fixed point) and sets the LookAt/Hilite structures
 * Same args as previous function
 **/
void guLookAtHilite(Mtx* m, LookAt* l, Hilite* h, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp,
                    f32 yUp, f32 zUp, f32 xl1, f32 yl1, f32 zl1, f32 xl2, f32 yl2, f32 zl2, s32 hiliteWidth,
                    s32 hiliteHeight) {
    f32 mf[4][4];

    guLookAtHiliteF(mf, l, h, xEye, yEye, zEye, xAt, yAt, zAt, xUp, yUp, zUp, xl1, yl1, zl1, xl2, yl2, zl2, hiliteWidth,
                    hiliteHeight);

    guMtxF2L((MtxF*)mf, m);
}

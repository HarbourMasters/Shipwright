#include "ultra64.h"
#include "libc/stdlib.h"
#include "libc/string.h"

#define BUFF_LEN 0x20

s16 _Ldunscale(s16*, _Pft*);
void _Genld(_Pft*, u8, u8*, s16, s16);

const f64 digs[] = { 10e0L, 10e1L, 10e3L, 10e7L, 10e15L, 10e31L, 10e63L, 10e127L, 10e255L };

/* float properties */
#define _D0 0
#define _DBIAS 0x3FF
#define _DLONG 1
#define _DOFF 4
#define _FBIAS 0x7E
#define _FOFF 7
#define _FRND 1
#define _LBIAS 0x3FFE
#define _LOFF 15
/* integer properties */
#define _C2 1
#define _CSIGN 1
#define _ILONG 0
#define _MBMAX 8
#define NAN 2
#define INF 1
#define FINITE -1
#define _DFRAC ((1 << _DOFF) - 1)
#define _DMASK (0x7FFF & ~_DFRAC)
#define _DMAX ((1 << (15 - _DOFF)) - 1)
#define _DNAN (0x8000 | _DMAX << _DOFF | 1 << (_DOFF - 1))
#define _DSIGN 0x8000
#if _D0 == 3
#define _D1 2 /* little-endian order */
#define _D2 1
#define _D3 0
#else
#define _D1 1 /* big-endian order */
#define _D2 2
#define _D3 3
#endif

void _Ldtob(_Pft* args, u8 type) {
    u8 buff[BUFF_LEN];
    u8* ptr = buff;
    u32 sp70;
    f64 val = args->v.ld;
    /* maybe struct? */
    s16 err;
    s16 nsig;
    s16 exp;
    s32 i;
    s32 n;
    f64 factor;
    s32 gen;
    s32 j;
    s32 lo;
    ldiv_t qr;
    u8 drop;
    s32 n2;

    if (args->prec < 0) {
        args->prec = 6;
    } else if ((args->prec == 0) && ((type == 'g') || (type == 'G'))) {
        args->prec = 1;
    }
    err = _Ldunscale(&exp, (_Pft*)args);
    if (err > 0) {
        memcpy(args->s, err == 2 ? "NaN" : "Inf", args->n1 = 3);
        return;
    }
    if (err == 0) {
        nsig = 0;
        exp = 0;
    } else {
        if (val < 0) {
            val = -val;
        }
        exp = exp * 30103 / 0x000186A0 - 4;
        if (exp < 0) {
            n = (3 - exp) & ~3;
            exp = -n;
            for (i = 0; n > 0; n >>= 1, i++) {
                if ((n & 1) != 0) {
                    val *= digs[i];
                }
            }
        } else if (exp > 0) {
            factor = 1;
            exp &= ~3;

            for (n = exp, i = 0; n > 0; n >>= 1, i++) {
                if ((n & 1) != 0) {
                    factor *= digs[i];
                }
            }
            val /= factor;
        }
        gen = ((type == 'f') ? exp + 10 : 6) + args->prec;
        if (gen > 0x13) {
            gen = 0x13;
        }
        *ptr++ = '0';
        while ((gen > 0) && (0 < val)) {
            lo = val;
            if ((gen -= 8) > 0) {
                val = (val - lo) * 1.0e8;
            }
            ptr = ptr + 8;
            for (j = 8; (lo > 0) && (--j >= 0);) {
                qr = ldiv(lo, 10);
                *--ptr = qr.rem + '0';
                lo = qr.quot;
            }
            while (--j >= 0) {
                ptr--;
                *ptr = '0';
            }
            ptr += 8;
        }

        gen = ptr - &buff[1];
        for (ptr = &buff[1], exp += 7; *ptr == '0'; ptr++) {
            --gen, --exp;
        }

        nsig = ((type == 'f') ? exp + 1 : (((type == 'e') || (type == 'E')) ? 1 : 0)) + args->prec;
        if (gen < nsig) {
            nsig = gen;
        }
        if (nsig > 0) {
            if ((nsig < gen) && (ptr[nsig] > '4')) {
                drop = '9';
            } else {
                drop = '0';
            }

            for (n2 = nsig; ptr[--n2] == drop;) {
                nsig--;
            }
            if (drop == '9') {
                ptr[n2]++;
            }
            if (n2 < 0) {
                --ptr, ++nsig, ++exp;
            }
        }
    }
    _Genld((_Pft*)args, type, ptr, nsig, exp);
}

s16 _Ldunscale(s16* pex, _Pft* px) {
    u16* ps = (u16*)px;
    s16 xchar = (ps[_D0] & _DMASK) >> _DOFF;

    if (xchar == _DMAX) { /* NaN or INF */
        *pex = 0;
        return (s16)(ps[_D0] & _DFRAC || ps[_D1] || ps[_D2] || ps[_D3] ? NAN : INF);
    } else if (0 < xchar) {
        ps[_D0] = (ps[_D0] & ~_DMASK) | (_DBIAS << _DOFF);
        *pex = xchar - (_DBIAS - 1);
        return FINITE;
    }
    if (0 > xchar) {
        return NAN;
    } else {
        *pex = 0;
        return 0;
    }
}

void _Genld(_Pft* px, u8 code, u8* p, s16 nsig, s16 xexp) {
    u8 point = '.';

    if (nsig <= 0) {
        nsig = 1,

        p = (u8*)"0";
    }

    if ((code == 'f') || (((code == 'g') || (code == 'G')) && (-4 <= xexp) && (xexp < px->prec))) { /* 'f' format */
        ++xexp;            /* change to leading digit count */
        if (code != 'f') { /* fixup for 'g' */
            if (!(px->flags & FLAGS_HASH) && nsig < px->prec) {
                px->prec = nsig;
            }
            if ((px->prec -= xexp) < 0) {
                px->prec = 0;
            }
        }
        if (xexp <= 0) { /* digits only to right of point */
            px->s[px->n1++] = '0';
            if (0 < px->prec || px->flags & FLAGS_HASH) {
                px->s[px->n1++] = point;
            }
            if (px->prec < -xexp) {
                xexp = -px->prec;
            }
            px->nz1 = -xexp;
            px->prec += xexp;
            if (px->prec < nsig) {
                nsig = px->prec;
            }
            memcpy(&px->s[px->n1], p, px->n2 = nsig);
            px->nz2 = px->prec - nsig;
        } else if (nsig < xexp) { /* zeros before point */
            memcpy(&px->s[px->n1], p, nsig);
            px->n1 += nsig;
            px->nz1 = xexp - nsig;
            if (0 < px->prec || px->flags & FLAGS_HASH) {
                px->s[px->n1] = point, ++px->n2;
            }
            px->nz2 = px->prec;
        } else { /* enough digits before point */
            memcpy(&px->s[px->n1], p, xexp);
            px->n1 += xexp;
            nsig -= xexp;
            if (0 < px->prec || px->flags & FLAGS_HASH) {
                px->s[px->n1++] = point;
            }
            if (px->prec < nsig) {
                nsig = px->prec;
            }
            memcpy(&px->s[px->n1], p + xexp, nsig);
            px->n1 += nsig;
            px->nz1 = px->prec - nsig;
        }
    } else {                                  /* 'e' format */
        if ((code == 'g') || (code == 'G')) { /* fixup for 'g' */
            if (nsig < px->prec) {
                px->prec = nsig;
            }
            if (--px->prec < 0) {
                px->prec = 0;
            }
            code = code == 'g' ? 'e' : 'E';
        }
        px->s[px->n1++] = *p++;
        if (0 < px->prec || px->flags & FLAGS_HASH) {
            px->s[px->n1++] = point;
        }
        if (0 < px->prec) { /* put fraction digits */
            if (px->prec < --nsig) {
                nsig = px->prec;
            }
            memcpy(&px->s[px->n1], p, nsig);
            px->n1 += nsig;
            px->nz1 = px->prec - nsig;
        }
        p = (u8*)&px->s[px->n1]; /* put exponent */
        *p++ = code;
        if (0 <= xexp) {
            *p++ = '+';
        } else { /* negative exponent */
            *p++ = '-';
            xexp = -xexp;
        }
        if (100 <= xexp) { /* put oversize exponent */
            if (1000 <= xexp) {
                *p++ = xexp / 1000 + '0', xexp %= 1000;
            }
            *p++ = xexp / 100 + '0', xexp %= 100;
        }
        *p++ = xexp / 10 + '0', xexp %= 10;
        *p++ = xexp + '0';
        px->n2 = p - (u8*)&px->s[px->n1];
    }
    if ((px->flags & (FLAGS_ZERO | FLAGS_MINUS)) == FLAGS_ZERO) { /* pad with leading zeros */
        s32 n = px->n0 + px->n1 + px->nz1 + px->n2 + px->nz2;

        if (n < px->width) {
            px->nz0 = px->width - n;
        }
    }
}

#include "global.h"

#define BUFF_LEN 0x18

u8 D_8000AF70[] = "0123456789abcdef";
u8 D_8000AF84[] = "0123456789ABCDEF";

void _Litob(_Pft* args, u8 type) {
    u8 buff[BUFF_LEN];
    const u8* numMap;
    s32 base;
    s32 idx;
    u64 num;
    lldiv_t quotrem;

    if (type == 'X') {
        numMap = D_8000AF84;
    } else {
        numMap = D_8000AF70;
    }

    base = (type == 'o') ? 8 : ((type != 'x' && type != 'X') ? 10 : 16);
    idx = BUFF_LEN;
    num = args->v.ll;

    if ((type == 'd' || type == 'i') && args->v.ll < 0) {
        num = -num;
    }

    if (num != 0 || args->prec != 0) {
        buff[--idx] = numMap[num % base];
    }

    args->v.ll = num / base;

    while (args->v.ll > 0 && idx > 0) {
        quotrem = lldiv(args->v.ll, base);
        args->v.ll = quotrem.quot;
        buff[--idx] = numMap[quotrem.rem];
    }

    args->n1 = BUFF_LEN - idx;

    memcpy(args->s, buff + idx, args->n1);

    if (args->n1 < args->prec) {
        args->nz0 = args->prec - args->n1;
    }

    if (args->prec < 0 && (args->flags & (FLAGS_ZERO | FLAGS_MINUS)) == FLAGS_ZERO) {
        idx = args->width - args->n0 - args->nz0 - args->n1;
        if (idx > 0) {
            args->nz0 += idx;
        }
    }
}

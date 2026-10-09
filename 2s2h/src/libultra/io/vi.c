#include "ultra64.h"
#include "alignment.h"

static __OSViContext vi[2] ALIGNED(8) = { 0 };
__OSViContext* __osViCurr = &vi[0];
__OSViContext* __osViNext = &vi[1];

void __osViInit(void) {
    bzero(vi, sizeof(vi));
    __osViCurr = &vi[0];
    __osViNext = &vi[1];
    __osViNext->retraceCount = 1;
    __osViCurr->retraceCount = 1;
    __osViNext->buffer = (void*)K0BASE;
    __osViCurr->buffer = (void*)K0BASE;

    if (osTvType == OS_TV_PAL) {
        __osViNext->modep = &osViModePalLan1;
    } else if (osTvType == OS_TV_MPAL) {
        __osViNext->modep = &osViModeMpalLan1;
    } else { // OS_TV_NTSC or OS_TV_UNK28
        __osViNext->modep = &osViModeNtscLan1;
    }

    __osViNext->state = 0x20;
    __osViNext->features = __osViNext->modep->comRegs.ctrl;

    while (IO_READ(VI_CURRENT_REG) > 10) {}

    IO_WRITE(VI_STATUS_REG, 0);

    __osViSwapContext();
}

#include "ultra64.h"

void __osViSwapContext(void) {
    register OSViMode* viMode;
    register __OSViContext* viNext;
    u32 origin;
    u32 hStart;
    u32 vstart;
    u32 sp34;
    u32 field;

    field = 0;
    viNext = __osViNext;
    viMode = viNext->modep;
    field = IO_READ(VI_V_CURRENT_LINE_REG) & 1;
    origin = osVirtualToPhysical(viNext->buffer) + viMode->fldRegs[field].origin;
    if (viNext->state & VI_STATE_XSCALE_UPDATED) {
        viNext->x.scale |= viMode->comRegs.xScale & ~0xFFF;
    } else {
        viNext->x.scale = viMode->comRegs.xScale;
    }
    if (viNext->state & 4) {
        sp34 = (u32)(viMode->fldRegs[field].yScale & 0xFFF);
        viNext->y.scale = viNext->y.factor * sp34;
        viNext->y.scale |= viMode->fldRegs[field].yScale & ~0xFFF;
    } else {
        viNext->y.scale = viMode->fldRegs[field].yScale;
    }

    vstart = (viMode->fldRegs[field].vStart - (__additional_scanline << 0x10)) + __additional_scanline;
    hStart = viMode->comRegs.hStart;

    if (viNext->state & 0x20) {
        hStart = 0;
    }

    if (viNext->state & 0x40) {
        viNext->y.scale = 0;
        origin = osVirtualToPhysical(viNext->buffer);
    }

    if (viNext->state & 0x80) {
        viNext->y.scale = (viNext->y.offset << 0x10) & 0x3FF0000;
        origin = osVirtualToPhysical(viNext->buffer);
    }

    IO_WRITE(VI_ORIGIN_REG, origin);
    IO_WRITE(VI_WIDTH_REG, viMode->comRegs.width);
    IO_WRITE(VI_BURST_REG, viMode->comRegs.burst);
    IO_WRITE(VI_V_SYNC_REG, viMode->comRegs.vSync);
    IO_WRITE(VI_H_SYNC_REG, viMode->comRegs.hSync);
    IO_WRITE(VI_LEAP_REG, viMode->comRegs.leap);
    IO_WRITE(VI_H_START_REG, hStart);
    IO_WRITE(VI_V_START_REG, vstart);
    IO_WRITE(VI_V_BURST_REG, viMode->fldRegs[field].vBurst);
    IO_WRITE(VI_INTR_REG, viMode->fldRegs[field].vIntr);
    IO_WRITE(VI_X_SCALE_REG, viNext->x.scale);
    IO_WRITE(VI_Y_SCALE_REG, viNext->y.scale);
    IO_WRITE(VI_CONTROL_REG, viNext->features);

    __osViNext = __osViCurr;
    __osViCurr = viNext;
    *__osViNext = *__osViCurr;
}

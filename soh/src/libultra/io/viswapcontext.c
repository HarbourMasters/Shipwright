#include "global.h"

void __osViSwapContext(void) {
    register OSViMode* viMode;
    register OSViContext* viNext;
    u32 origin;
    u32 hStart;
    u32 vstart;
    u32 sp34;
    u32 field;
    register u32 s2;

    field = 0;
    viNext = __osViNext;
    viMode = viNext->modep;
    field = HW_REG(VI_V_CURRENT_LINE_REG, u32) & 1;
    s2 = osVirtualToPhysical(viNext->buffer);
    origin = (viMode->fldRegs[field].origin) + s2;
    if (viNext->state & 2) {
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
    HW_REG(VI_ORIGIN_REG, u32) = origin;
    HW_REG(VI_WIDTH_REG, u32) = viMode->comRegs.width;
    HW_REG(VI_BURST_REG, u32) = viMode->comRegs.burst;
    HW_REG(VI_V_SYNC_REG, u32) = viMode->comRegs.vSync;
    HW_REG(VI_H_SYNC_REG, u32) = viMode->comRegs.hSync;
    HW_REG(VI_LEAP_REG, u32) = viMode->comRegs.leap;
    HW_REG(VI_H_START_REG, u32) = hStart;
    HW_REG(VI_V_START_REG, u32) = vstart;
    HW_REG(VI_V_BURST_REG, u32) = viMode->fldRegs[field].vBurst;
    HW_REG(VI_INTR_REG, u32) = viMode->fldRegs[field].vIntr;
    HW_REG(VI_X_SCALE_REG, u32) = viNext->x.scale;
    HW_REG(VI_Y_SCALE_REG, u32) = viNext->y.scale;
    HW_REG(VI_CONTROL_REG, u32) = viNext->features;
    __osViNext = __osViCurr;
    __osViCurr = viNext;
    *__osViNext = *__osViCurr;
}

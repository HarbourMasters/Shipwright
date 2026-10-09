#ifndef PR_VIINT_H
#define PR_VIINT_H

#include <libultraship/libultra/vi.h>

#if 0
#include "ultratypes.h"

#define OS_TV_TYPE_PAL 0
#define OS_TV_TYPE_NTSC 1
#define OS_TV_TYPE_MPAL 2

#define VI_STATE_MODE_UPDATED (1 << 0)
#define VI_STATE_XSCALE_UPDATED (1 << 1)
#define VI_STATE_YSCALE_UPDATED (1 << 2)
#define VI_STATE_CTRL_UPDATED (1 << 3)   // related to control regs changing
#define VI_STATE_BUFFER_UPDATED (1 << 4) // swap buffer
#define VI_STATE_BLACK (1 << 5)          // probably related to a black screen
#define VI_STATE_REPEATLINE (1 << 6)     // repeat line?
#define VI_STATE_FADE (1 << 7)           // fade

#define VI_CTRL_ANTIALIAS_MODE_3 0x00300 /* Bit [9:8] anti-alias mode */
#define VI_CTRL_ANTIALIAS_MODE_2 0x00200 /* Bit [9:8] anti-alias mode */
#define VI_CTRL_ANTIALIAS_MODE_1 0x00100 /* Bit [9:8] anti-alias mode */

#define VI_SCALE_MASK 0xFFF
#define VI_2_10_FPART_MASK 0x3FF
#define VI_SUBPIXEL_SH 0x10

// For use in initializing OSViMode structures

#define BURST(hsync_width, color_width, vsync_width, color_start)                                      \
    (((u32)(hsync_width)&0xFF) | (((u32)(color_width)&0xFF) << 8) | (((u32)(vsync_width)&0xF) << 16) | \
     (((u32)(color_start)&0xFFFF) << 20))
#define WIDTH(v) (v)
#define VSYNC(v) (v)
#define HSYNC(duration, leap) (((u32)(leap) << 16) | ((u32)(duration)&0xFFFF))
#define LEAP(upper, lower) (((u32)(upper) << 16) | ((u32)(lower)&0xFFFF))
#define START(start, end) (((u32)(start) << 16) | ((u32)(end)&0xFFFF))

#define FTOFIX(val, i, f) ((u32)((val) * (f32)(1 << (f))) & ((1 << ((i) + (f))) - 1))

#define F210(val) FTOFIX(val, 2, 10)
#define SCALE(scaleup, off) (F210((1.0f / (f32)(scaleup))) | (F210((f32)(off)) << 16))

#define VCURRENT(v) v
#define ORIGIN(v) v
#define VINTR(v) v
#define HSTART START


typedef struct {
    /* 0x0 */ f32 factor;
    /* 0x4 */ u16 offset;
    /* 0x8 */ u32 scale;
} __OSViScale; // size = 0xC

typedef struct {
    /* 0x00 */ u16 state;
    /* 0x02 */ u16 retraceCount;
    /* 0x04 */ void* buffer;
    /* 0x08 */ OSViMode* modep;
    /* 0x0C */ u32 features;
    /* 0x10 */ OSMesgQueue* mq;
    /* 0x14 */ OSMesg* msg;
    /* 0x18 */ __OSViScale x;
    /* 0x24 */ __OSViScale y;
} __OSViContext; // size = 0x30

extern OSDevMgr __osViDevMgr;

void __osViSwapContext(void);
extern __OSViContext* __osViCurr;
extern __OSViContext* __osViNext;
extern u32 __additional_scanline;
__OSViContext* __osViGetCurrentContext(void);
void __osViInit(void);
#endif
#endif

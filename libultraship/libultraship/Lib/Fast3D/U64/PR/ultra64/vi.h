#ifndef ULTRA64_VI_H
#define ULTRA64_VI_H

#include "message.h"

/* Special Features */
#define OS_VI_GAMMA_ON          0x0001
#define OS_VI_GAMMA_OFF         0x0002
#define OS_VI_GAMMA_DITHER_ON   0x0004
#define OS_VI_GAMMA_DITHER_OFF  0x0008
#define OS_VI_DIVOT_ON          0x0010
#define OS_VI_DIVOT_OFF         0x0020
#define OS_VI_DITHER_FILTER_ON  0x0040
#define OS_VI_DITHER_FILTER_OFF 0x0080

#define OS_VI_GAMMA         0x08
#define OS_VI_GAMMA_DITHER  0x04
#define OS_VI_DIVOT         0x10
#define OS_VI_DITHER_FILTER 0x10000
#define OS_VI_UNK1          0x1
#define OS_VI_UNK2          0x2
#define OS_VI_UNK40         0x40
#define OS_VI_UNK100        0x100
#define OS_VI_UNK200        0x200
#define OS_VI_UNK1000       0x1000
#define OS_VI_UNK2000       0x2000

typedef struct {
    /* 0x00 */ u32 ctrl;
    /* 0x04 */ u32 width;
    /* 0x08 */ u32 burst;
    /* 0x0C */ u32 vSync;
    /* 0x10 */ u32 hSync;
    /* 0x14 */ u32 leap;
    /* 0x18 */ u32 hStart;
    /* 0x1C */ u32 xScale;
    /* 0x20 */ u32 vCurrent;
} OSViCommonRegs; // size = 0x20

typedef struct {
    /* 0x00 */ u32 origin;
    /* 0x04 */ u32 yScale;
    /* 0x08 */ u32 vStart;
    /* 0x0C */ u32 vBurst;
    /* 0x10 */ u32 vIntr;
} OSViFieldRegs; // size = 0x14

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x04 */ OSViCommonRegs comRegs;
    /* 0x24 */ OSViFieldRegs fldRegs[2];
} OSViMode; // size = 0x4C

typedef struct {
    /* 0x0 */ f32 factor;
    /* 0x4 */ u16 offset;
    /* 0x8 */ u32 scale;
} __OSViScale; // size = 0x0C

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
} OSViContext; // size = 0x30

#define OS_VI_NTSC_LPN1     0   /* NTSC */
#define OS_VI_NTSC_LPF1     1
#define OS_VI_NTSC_LAN1     2
#define OS_VI_NTSC_LAF1     3
#define OS_VI_NTSC_LPN2     4
#define OS_VI_NTSC_LPF2     5
#define OS_VI_NTSC_LAN2     6
#define OS_VI_NTSC_LAF2     7
#define OS_VI_NTSC_HPN1     8
#define OS_VI_NTSC_HPF1     9
#define OS_VI_NTSC_HAN1     10
#define OS_VI_NTSC_HAF1     11
#define OS_VI_NTSC_HPN2     12
#define OS_VI_NTSC_HPF2     13

#define OS_VI_PAL_LPN1      14  /* PAL */
#define OS_VI_PAL_LPF1      15
#define OS_VI_PAL_LAN1      16
#define OS_VI_PAL_LAF1      17
#define OS_VI_PAL_LPN2      18
#define OS_VI_PAL_LPF2      19
#define OS_VI_PAL_LAN2      20
#define OS_VI_PAL_LAF2      21
#define OS_VI_PAL_HPN1      22
#define OS_VI_PAL_HPF1      23
#define OS_VI_PAL_HAN1      24
#define OS_VI_PAL_HAF1      25
#define OS_VI_PAL_HPN2      26
#define OS_VI_PAL_HPF2      27

#define OS_VI_MPAL_LPN1     28  /* MPAL - mainly Brazil */
#define OS_VI_MPAL_LPF1     29
#define OS_VI_MPAL_LAN1     30
#define OS_VI_MPAL_LAF1     31
#define OS_VI_MPAL_LPN2     32
#define OS_VI_MPAL_LPF2     33
#define OS_VI_MPAL_LAN2     34
#define OS_VI_MPAL_LAF2     35
#define OS_VI_MPAL_HPN1     36
#define OS_VI_MPAL_HPF1     37
#define OS_VI_MPAL_HAN1     38
#define OS_VI_MPAL_HAF1     39
#define OS_VI_MPAL_HPN2     40
#define OS_VI_MPAL_HPF2     41

#define OS_VI_FPAL_LPN1     42  /* FPAL - Full screen PAL */
#define OS_VI_FPAL_LPF1     43
#define OS_VI_FPAL_LAN1     44
#define OS_VI_FPAL_LAF1     45
#define OS_VI_FPAL_LPN2     46
#define OS_VI_FPAL_LPF2     47
#define OS_VI_FPAL_LAN2     48
#define OS_VI_FPAL_LAF2     49
#define OS_VI_FPAL_HPN1     50
#define OS_VI_FPAL_HPF1     51
#define OS_VI_FPAL_HAN1     52
#define OS_VI_FPAL_HAF1     53
#define OS_VI_FPAL_HPN2     54
#define OS_VI_FPAL_HPF2     55

#define OS_TV_PAL           0
#define OS_TV_NTSC          1
#define OS_TV_MPAL          2
#define OS_VI_UNK28         28

#endif

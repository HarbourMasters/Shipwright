#include "PR/os.h"
#include "PR/rcp.h"
#include "PR/viint.h"

OSViMode osViModeNtscHpn1 = {
    OS_VI_NTSC_HPN1, // type
    {
        // comRegs
        VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_SERRATE_ON | VI_CTRL_ANTIALIAS_MODE_2 |
            VI_CTRL_PIXEL_ADV_3, // ctrl
        WIDTH(1280),             // width
        BURST(57, 34, 5, 62),    // burst
        VSYNC(524),              // vSync
        HSYNC(3093, 0),          // hSync
        LEAP(3093, 3093),        // leap
        HSTART(108, 748),        // hStart
        SCALE(1, 0),             // xScale
        VCURRENT(0),             // vCurrent
    },
    { // fldRegs
      {
          // [0]
          ORIGIN(1280),       // origin
          SCALE(1, 0),        // yScale
          HSTART(35, 509),    // vStart
          BURST(4, 2, 14, 0), // vBurst
          VINTR(2),           // vIntr
      },
      {
          // [1]
          ORIGIN(2560),       // origin
          SCALE(1, 0),        // yScale
          HSTART(37, 511),    // vStart
          BURST(4, 2, 14, 0), // vBurst
          VINTR(2),           // vIntr
      } },
};

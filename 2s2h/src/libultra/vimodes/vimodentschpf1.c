#include "PR/os.h"
#include "PR/rcp.h"
#include "PR/viint.h"

OSViMode osViModeNtscHpf1 = {
    OS_VI_NTSC_HPF1, // type
    {
        // comRegs
        VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_SERRATE_ON | VI_CTRL_ANTIALIAS_MODE_2 |
            VI_CTRL_PIXEL_ADV_3, // ctrl
        WIDTH(640),              // width
        BURST(57, 34, 5, 62),    // 0x3E52239, // burst
        VSYNC(524),              // vSync
        HSYNC(3093, 0),          // hSync
        LEAP(3093, 3093),        // leap
        HSTART(108, 748),        // 0x6C02EC,  // hStart
        SCALE(1, 0),             // xScale
        VCURRENT(0),             // vCurrent
    },
    { // fldRegs
      {
          // [0]
          ORIGIN(1280),       // origin
          SCALE(0.5f, 0.5f),  // 0x2000800, // yScale
          START(35, 509),     // vStart
          BURST(4, 2, 14, 0), // vBurst
          VINTR(2),           // vIntr
      },
      {
          // [1]
          ORIGIN(2560),       // origin
          SCALE(0.5f, 0.5f),  // 0x2000800, // yScale
          START(37, 511),     // vStart
          BURST(4, 2, 14, 0), // vBurst
          VINTR(2)            // vIntr
      } },
};

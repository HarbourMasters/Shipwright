// clang-format off

#include "global.h"

OSViMode osViModePalLan1 = {
    0x10, // type
    {
        // comRegs
        0x311E,       // ctrl
        SCREEN_WIDTH, // width
        0x4541E3A,    // burst
        0x271,        // vSync
        0x170C69,     // hSync
        0xC6F0C6D,    // leap
        0x800300,     // hStart
        0x200,        // xScale
        0             // vCurrent
    },
    { // fldRegs
      {
          0x280,    // origin
          0x400,    // yScale
          0x5F0239, // vStart
          0x9026B,  // vBurst
          2,        // vIntr
      },
      {
          0x280,    // origin
          0x400,    // yScale
          0x5F0239, // vStart
          0x9026B,  // vBurst
          2         // vIntr
      } },
};

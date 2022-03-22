#include "global.h"

OSViMode osViModeMpalLan1 = {
    0x1E, // type
    {
        // comRegs
        0x311E,       // ctrl
        SCREEN_WIDTH, // width
        0x4651E39,    // burst
        0x20D,        // vSync
        0x40C11,      // hSync
        0xC190C1A,    // leap
        0x6C02EC,     // hStart
        0x200,        // xScale
        0,            // vCurrent
    },
    { // fldRegs
      {
          // [0]
          0x280,    // origin
          0x400,    // yScale
          0x2501FF, // vStart
          0xE0204,  // vBurst
          2,        // vIntr
      },
      {
          // [1]
          0x280,    // origin
          0x400,    // yScale
          0x2501FF, // vStart
          0xE0204,  // vBurst
          2,        // vIntr
      } },
};

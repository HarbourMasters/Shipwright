#include "global.h"

OSViMode osViModeNtscLan1 = {
    2, // type
    {
        // comRegs
        0x311E,       // ctrl
        SCREEN_WIDTH, // width
        0x3E52239,    // burst
        0x20D,        // vSync
        0xC15,        // hSync
        0xC150C15,    // leap
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

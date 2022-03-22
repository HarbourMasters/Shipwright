#include "z64.h"

// 0x18000 bytes
u64 gGfxSPTaskOutputBuffer[0x3000];

// 0xC00 bytes
u8 gGfxSPTaskYieldBuffer[OS_YIELD_DATA_SIZE];

// 0x400 bytes
u8 gGfxSPTaskStack[0x400];

// 0x12410 bytes each; 0x24820 bytes total
GfxPool gGfxPools[2];

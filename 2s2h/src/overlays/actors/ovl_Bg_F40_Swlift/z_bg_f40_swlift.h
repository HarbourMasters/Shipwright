#ifndef Z_BG_F40_SWLIFT_H
#define Z_BG_F40_SWLIFT_H

#define BG_F40_SWLIFT_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0xFF)
#define BG_F40_SWLIFT_GET_INDEX(thisx) (((thisx)->params >> 8) & 0xFF)

#include "global.h"

struct BgF40Swlift;

typedef struct BgF40Swlift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ s32 timer;
} BgF40Swlift; // size = 0x160

#endif // Z_BG_F40_SWLIFT_H

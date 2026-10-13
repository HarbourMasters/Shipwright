#ifndef Z_BG_MARKET_STEP_H
#define Z_BG_MARKET_STEP_H

#include "global.h"

#define BG_MARKET_STEP_GET_TIME_OF_DAY(thisx) ((thisx)->params & 1)

struct BgMarketStep;

typedef struct BgMarketStep {
    /* 0x000 */ Actor actor;
} BgMarketStep; // size = 0x144

#endif // Z_BG_MARKET_STEP_H

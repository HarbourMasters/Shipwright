#ifndef Z_BG_CTOWER_GEAR_H
#define Z_BG_CTOWER_GEAR_H

#include "global.h"

struct BgCtowerGear;


#define BGCTOWERGEAR_GET_TYPE(thisx) ((thisx)->params & 3)

typedef enum {
    /* 0 */ BGCTOWERGEAR_CEILING_COG,
    /* 1 */ BGCTOWERGEAR_CENTER_COG,
    /* 2 */ BGCTOWERGEAR_WATER_WHEEL,
    /* 3 */ BGCTOWERGEAR_ORGAN
} BgCtowerGearType;

typedef struct BgCtowerGear {
    /* 0x000 */ DynaPolyActor dyna;
} BgCtowerGear; // size = 0x15C

#endif // Z_BG_CTOWER_GEAR_H

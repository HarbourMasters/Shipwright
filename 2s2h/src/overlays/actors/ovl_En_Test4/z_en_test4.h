#ifndef Z_EN_TEST4_H
#define Z_EN_TEST4_H

#include "global.h"

struct EnTest4;

#define THREEDAY_GET_SKYBOX_NUM_STARS(thisx) (((thisx)->params >> 10) * 100)
#define THREEDAY_GET_BIG_BELLS_SFX_VOLUME_INDEX(thisx) (((thisx)->params >> 5) & 0xF)

typedef void (*EnTest4ActionFunc)(struct EnTest4*, PlayState*);

typedef enum ThreeDayDaytime {
    /* 0 */ THREEDAY_DAYTIME_NIGHT,
    /* 1 */ THREEDAY_DAYTIME_DAY,
    /* 2 */ THREEDAY_DAYTIME_MAX
} ThreeDayDaytime;

typedef struct EnTest4 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s8 daytimeIndex; // See `ThreeDayDaytime`
    /* 0x145 */ u8 transitionCsTimer;
    /* 0x146 */ u16 prevTime;
    /* 0x148 */ u16 nextBellTime; // Next time the bell will sound
    /* 0x14A */ u16 prevBellTime; // Last time the bell sounded
    /* 0x14C */ u8 weather; // See `ThreeDayWeather`
    /* 0x150 */ EnTest4ActionFunc actionFunc;
} EnTest4; // size = 0x154

typedef enum ThreeDayWeather {
    /* 0 */ THREEDAY_WEATHER_CLEAR,
    /* 1 */ THREEDAY_WEATHER_RAIN
} ThreeDayWeather;

#endif // Z_EN_TEST4_H

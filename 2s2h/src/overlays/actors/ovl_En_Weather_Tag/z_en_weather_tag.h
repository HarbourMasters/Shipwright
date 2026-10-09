#ifndef Z_EN_WEATHER_TAG_H
#define Z_EN_WEATHER_TAG_H

#include "global.h"

struct EnWeatherTag;

typedef void (*EnWeatherTagActionFunc)(struct EnWeatherTag*, PlayState*);

typedef struct EnWeatherTag {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnWeatherTagActionFunc actionFunc;
    /* 0x148 */ UNK_TYPE1 pad148[0x4];
    /* 0x14C */ u8 pathCount;
    /* 0x150 */ Vec3s* pathPoints;
    // 154: the code avoids what would be a negative value if signed, but signed wont match
    /* 0x154 */ u16 unk154;
    /* 0x156 */ u16 fadeDistance;
    /* 0x158 */ u16 unk158; // increment/decrement amount for 154
} EnWeatherTag; // size = 0x15C

#define WEATHER_TAG_TYPE(thisx) ((thisx)->params & 0xF)
#define WEATHER_TAG_GET_PATH_INDEX(thisx) (((thisx)->params >> 4) & 0xF)
#define WEATHER_TAG_RANGE100(thisx) ((u8)(((thisx)->params >> 8) & 0xFF) * 100.0f)
#define WEATHER_TAG_GET_SKYBOX_NUM_STARS(thisx) ((u8)(((thisx)->params >> 8) & 0xFF) * 100)

typedef enum {
    /* 0 */ WEATHERTAG_TYPE_UNK0,
    /* 1 */ WEATHERTAG_TYPE_UNK1,
    /* 2 */ WEATHERTAG_TYPE_WINTERFOG,
    /* 3 */ WEATHERTAG_TYPE_UNK3,
    /* 4 */ WEATHERTAG_TYPE_SKYBOX_STARS,
    /* 5 */ WEATHERTAG_TYPE_UNK5,
    /* 6 */ WEATHERTAG_TYPE_WATERMURK,
    /* 7 */ WEATHERTAG_TYPE_LOCALDAY2RAIN
} WeatherTagType;

#endif // Z_EN_WEATHER_TAG_H

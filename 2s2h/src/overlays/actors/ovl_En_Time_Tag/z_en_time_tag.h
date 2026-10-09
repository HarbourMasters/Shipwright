#ifndef Z_EN_TIME_TAG_H
#define Z_EN_TIME_TAG_H

#include "global.h"

struct EnTimeTag;

typedef void (*EnTimeTagActionFunc)(struct EnTimeTag*, PlayState*);

#define TIMETAG_GET_TYPE(thisx) (((thisx)->params & 0xE000) >> 0xD)

#define TIMETAG_KICKOUT_GET_TEXT(thisx) (((thisx)->params & 0x1FE0) >> 0x5)
#define TIMETAG_KICKOUT_GET_EXIT_INDEX(thisx) ((thisx)->params & 0x1F)
#define TIMETAG_KICKOUT_HOUR(thisx) ((thisx)->home.rot.x)
#define TIMETAG_KICKOUT_MINUTE(thisx) ((thisx)->home.rot.y)

#define TIMETAG_SOARING_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

#define TIMETAG_DIARY_GET_TYPE(thisx) ((thisx)->params & 0x7F)
#define TIMETAG_DIARY_TIMER(thisx) ((thisx)->home.rot.x)
#define TIMETAG_DIARY_SONG(thisx) ((thisx)->home.rot.z)

typedef enum {
    /* 0 */ TIMETAG_KICKOUT_DOOR,
    /* 1 */ TIMETAG_SOARING_ENGRAVING,
    /* 2 */ TIMETAG_ROOFTOP_OATH,
    /* 3 */ TIMETAG_DIARY,
    /* 4 */ TIMETAG_KICKOUT_FINAL_HOURS,
    /* 5 */ TIMETAG_KICKOUT_NO_DOOR
} TimeTagType;

typedef enum {
    /* 0 */ TIMETAG_DIARY_MIKAU,
    /* 1 */ TIMETAG_DIARY_LULU
} TimeTagDiaryType;

typedef enum {
    /* 0 */ TIMETAG_DIARY_SONG_EVAN_PART1,
    /* 1 */ TIMETAG_DIARY_SONG_EVAN_PART2
} TimeTagDiarySong;

typedef enum {
    /*  0 */ TIMETAG_KICKOUT_TEXT_0, // I am closing the shop
    /*  1 */ TIMETAG_KICKOUT_TEXT_SWAMP_SHOOTING_GALLERY, // time to close shop
    /*  2 */ TIMETAG_KICKOUT_TEXT_STOCK_POT_INN, // Stock Pot Inn locking the door
    /*  3 */ TIMETAG_KICKOUT_TEXT_HONEY_AND_DARLING, // need some alone time
    /*  4 */ TIMETAG_KICKOUT_TEXT_LOTTERY, // business is finished
    /*  5 */ TIMETAG_KICKOUT_TEXT_BARN_DAY, // the lady needs a break
    /*  6 */ TIMETAG_KICKOUT_TEXT_GENERIC, // (Many scenes) place is about to close
    /*  7 */ TIMETAG_KICKOUT_TEXT_BARN_NIGHT, // they came
    /*  8 */ TIMETAG_KICKOUT_TEXT_TRADING_POST, // we have to close shop
    /*  9 */ TIMETAG_KICKOUT_TEXT_MILK_BAR, // I have to get ready for business
    /* 10 */ TIMETAG_KICKOUT_TEXT_CURIOSITY_SHOP_BACK_1, // shop is opening, you need to go
    /* 11 */ TIMETAG_KICKOUT_TEXT_CURIOSITY_SHOP_BACK_2, // shop is opening, we're in the way
    /* 12 */ TIMETAG_KICKOUT_TEXT_TOWN_SHOOTING_GALLERY_1, // it's time to close
    /* 13 */ TIMETAG_KICKOUT_TEXT_TOWN_SHOOTING_GALLERY_2, // quick, it's closing time
    /* 14 */ TIMETAG_KICKOUT_TEXT_SWORDSMANS_SCHOOL, // need to cut the moon, must leave
    /* 15 */ TIMETAG_KICKOUT_TEXT_TREASURE_CHEST_SHOP, // already time to close shop
    /* 16 */ TIMETAG_KICKOUT_TEXT_DOGGY_RACETRACK // no more for today
} TimeTagKickOutText;

typedef struct EnTimeTag {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnTimeTagActionFunc actionFunc;
} EnTimeTag; // size = 0x148

#endif // Z_EN_TIME_TAG_H

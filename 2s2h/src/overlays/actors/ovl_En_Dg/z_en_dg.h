#ifndef Z_EN_DG_H
#define Z_EN_DG_H

#include "global.h"
#include "objects/object_dog/object_dog.h"

typedef enum {
    /* 0 */ DOG_COLOR_DEFAULT, // ends up just being treated as beige
    /* 1 */ DOG_COLOR_WHITE,
    /* 2 */ DOG_COLOR_GRAY,
    /* 3 */ DOG_COLOR_BEIGE,
    /* 4 */ DOG_COLOR_BROWN,
    /* 5 */ DOG_COLOR_BLUE,
    /* 6 */ DOG_COLOR_GOLD
} DogColor;

struct EnDg;

typedef void (*EnDgActionFunc)(struct EnDg*, PlayState*);

#define ENDG_GET_INDEX(thisx) (((thisx)->params & 0x3E0) >> 5)
#define ENDG_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 10)
#define ENDG_PARAMS(path, index) ((path << 10) | (index << 5))

#define ENDG_PATH_INDEX_NONE 0x3F

typedef enum {
    /*  0 */ ENDG_INDEX_RACETRACK_0,
    /*  1 */ ENDG_INDEX_RACETRACK_1,
    /*  2 */ ENDG_INDEX_RACETRACK_2,
    /*  3 */ ENDG_INDEX_RACETRACK_3,
    /*  4 */ ENDG_INDEX_RACETRACK_4,
    /*  5 */ ENDG_INDEX_RACETRACK_5,
    /*  6 */ ENDG_INDEX_RACETRACK_6,
    /*  7 */ ENDG_INDEX_RACETRACK_7,
    /*  8 */ ENDG_INDEX_RACETRACK_8,
    /*  9 */ ENDG_INDEX_RACETRACK_9,
    /* 10 */ ENDG_INDEX_RACETRACK_10,
    /* 11 */ ENDG_INDEX_RACETRACK_11,
    /* 12 */ ENDG_INDEX_RACETRACK_12,
    /* 13 */ ENDG_INDEX_RACETRACK_13,
    /* 20 */ ENDG_INDEX_ROMANI_RANCH = 20,
    /* 21 */ ENDG_INDEX_SWAMP_SPIDER_HOUSE,
    /* 31 */ ENDG_INDEX_SOUTH_CLOCK_TOWN = 31
} EnDgIndex;

typedef struct EnDg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnDgActionFunc actionFunc;
    /* 0x148 */ UNK_TYPE1 unk_148[0x4];
    /* 0x14C */ SkelAnime skelAnime;
    /* 0x190 */ ColliderCylinder collider;
    /* 0x1DC */ Path* path;
    /* 0x1E0 */ s32 currentPoint;
    /* 0x1E4 */ Vec3s jointTable[DOG_LIMB_MAX];
    /* 0x232 */ Vec3s morphTable[DOG_LIMB_MAX];
    /* 0x280 */ u16 dogFlags;
    /* 0x282 */ s16 timer;
    /* 0x284 */ s16 swimTimer;
    /* 0x286 */ s16 index;
    /* 0x288 */ s16 selectedDogIndex;
    /* 0x28A */ s16 sitAfterThrowTimer;
    /* 0x28C */ s16 behavior;
    /* 0x28E */ s16 attackTimer;
    /* 0x290 */ s16 grabState;
    /* 0x292 */ s16 bremenBarkTimer;
    /* 0x294 */ Vec3f curRot;
} EnDg; // size = 0x2A0

#endif // Z_EN_DG_H

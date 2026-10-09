#ifndef Z_EFF_LASTDAY_H
#define Z_EFF_LASTDAY_H

#include "global.h"

#define EFFLASTDAY_GET_F(thisx) ((thisx)->params & 0xF)

struct EffLastday;

typedef void (*EffLastdayActionFunc)(struct EffLastday*, PlayState*);

typedef struct EffLastday {
    /* 0x000 */ Actor actor;
    /* 0x144 */ Gfx* dList;
    /* 0x148 */ AnimatedMaterial* matAnim;
    /* 0x14C */ s16 step;
    /* 0x14E */ s16 alpha;
    /* 0x150 */ s16 cueType;
    /* 0x154 */ EffLastdayActionFunc actionFunc;
} EffLastday; // size = 0x158

typedef enum EffLastDayParam {
    /* 0 */ EFFLASTDAY_PARAM_0,
    /* 1 */ EFFLASTDAY_PARAM_1,
    /* 2 */ EFFLASTDAY_PARAM_2,
    /* 3 */ EFFLASTDAY_PARAM_3
} EffLastDayParam;

#endif // Z_EFF_LASTDAY_H

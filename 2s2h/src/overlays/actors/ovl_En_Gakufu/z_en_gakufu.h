#ifndef Z_EN_GAKUFU_H
#define Z_EN_GAKUFU_H

#include "global.h"

struct EnGakufu;

typedef void (*EnGakufuActionFunc)(struct EnGakufu*, PlayState*);

#define GAKUFU_GET_TYPE(thisx) ((thisx)->params & 0xF)

typedef enum {
    /* 0 */ GAKUFU_TERMINA_FIELD,   
    /* 1 */ GAKUFU_MILK_BAR
} GakufuType;

typedef struct EnGakufu {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s32 songIndex;
    /* 0x148 */ u8 buttonIndex[8];
    /* 0x150 */ EnGakufuActionFunc actionFunc;
} EnGakufu; // size = 0x154

#endif // Z_EN_GAKUFU_H

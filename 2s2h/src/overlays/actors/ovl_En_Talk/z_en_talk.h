#ifndef Z_EN_TALK_H
#define Z_EN_TALK_H

#include "global.h"

#define ENTALK_GET_TEXT_ID(thisx) (((thisx)->params & 0x3F) + 0x1C00)
#define ENTALK_GET_ATTENTION_RANGE_TYPE(thisx) (((thisx)->home.rot.x) - 1)

struct EnTalk;

typedef void (*EnTalkActionFunc)(struct EnTalk*, PlayState*);

typedef struct EnTalk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnTalkActionFunc actionFunc;
} EnTalk; // size = 0x148

#endif // Z_EN_TALK_H

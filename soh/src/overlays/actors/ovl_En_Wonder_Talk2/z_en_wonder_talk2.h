#ifndef Z_EN_WONDER_TALK2_H
#define Z_EN_WONDER_TALK2_H

#include "ultra64.h"
#include "global.h"

struct EnWonderTalk2;

typedef void (*EnWonderTalk2Func)(struct EnWonderTalk2*, PlayState*);

typedef struct EnWonderTalk2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnWonderTalk2Func actionFunc;
    /* 0x0150 */ s16 baseMsgId; // specified message type
    /* 0x0152 */ s16 switchFlag;
    /* 0x0154 */ s16 talkMode;
    /* 0x0156 */ s16 unk_156;
    /* 0x0158 */ s16 unk_158;
    /* 0x015A */ u8 unk_15A;
    /* 0x015B */ u8 unk_15B;
    /* 0x015C */ f32 triggerRange;
    /* 0x0160 */ f32 height;
    /* 0x0164 */ Vec3f initPos;
} EnWonderTalk2; // size = 0x0170

#endif

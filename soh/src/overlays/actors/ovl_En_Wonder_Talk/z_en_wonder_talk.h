#ifndef Z_EN_WONDER_TALK_H
#define Z_EN_WONDER_TALK_H

#include "ultra64.h"
#include "global.h"

struct EnWonderTalk;

typedef void (*EnWonderTalkFunc)(struct EnWonderTalk*, PlayState*);

typedef struct EnWonderTalk {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnWonderTalkFunc actionFunc;
    /* 0x0150 */ s16 unk_150;
    /* 0x0152 */ s16 unk_152;
    /* 0x0154 */ s16 switchFlag;
    /* 0x0156 */ s16 unk_156;
    /* 0x0158 */ s16 unk_158;
    /* 0x0160 */ s16 unk_15A;
    /* 0x015C */ f32 unk_15C;
    /* 0x0160 */ f32 height;
    /* 0x0164 */ u8  unk_164;
} EnWonderTalk; // size = 0x0168

#endif

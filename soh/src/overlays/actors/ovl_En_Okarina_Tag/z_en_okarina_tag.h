#ifndef Z_EN_OKARINA_TAG_H
#define Z_EN_OKARINA_TAG_H

#include "ultra64.h"
#include "global.h"

struct EnOkarinaTag;

typedef void (*EnOkarinaTagActionFunc)(struct EnOkarinaTag*, PlayState*);

typedef struct EnOkarinaTag {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnOkarinaTagActionFunc actionFunc;
    /* 0x0150 */ s16 type;
    /* 0x0152 */ s16 ocarinaSong;
    /* 0x0154 */ s16 switchFlag;
    /* 0x0156 */ char unk_156[0x2];
    /* 0x0158 */ s16 unk_158;
    /* 0x015A */ s16 unk_15A;
    /* 0x015C */ f32 interactRange;
} EnOkarinaTag; // size = 0x0160

#endif

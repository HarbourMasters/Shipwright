#ifndef Z_EN_PART_H
#define Z_EN_PART_H

#include "global.h"

struct EnPart;

#define ENPART_PARAMS(type) (type)

typedef void (*EnPartActionFunc)(struct EnPart*, PlayState*);

typedef enum {
    /* 0x0 */ ENPART_TYPE_0,
    /* 0x1 */ ENPART_TYPE_1,
    /* 0x4 */ ENPART_TYPE_4 = 4,
    /* 0xF */ ENPART_TYPE_15 = 15
} EnPartParam;

typedef struct EnPart {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u8 actionFuncIndex;
    /* 0x146 */ s16 unk146;
    /* 0x148 */ f32 zRot;
    /* 0x14C */ f32 unk14C;
    /* 0x150 */ Gfx* dList;
} EnPart; // size = 0x154

#endif // Z_EN_PART_H

#ifndef Z_OBJ_PURIFY_H
#define Z_OBJ_PURIFY_H

#include "global.h"

struct ObjPurify;

typedef void (*ObjPurifyActionFunc)(struct ObjPurify*, PlayState*);

typedef struct ObjPurify {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjPurifyActionFunc actionFunc;
    /* 0x160 */ s32 gfxIndex;
    /* 0x164 */ s16 timer;
    /* 0x166 */ s16 objectSlot;
    /* 0x168 */ f32 unk168[2];
} ObjPurify; // size = 0x170

#define OBJPURIFY_GET_INFO_INDEX(thisx) ((thisx)->params & 0xF)
#define OBJPURIFY_GET_UNK_FLAG(thisx) (((thisx)->params >> 0xC) & 1)

#endif // Z_OBJ_PURIFY_H

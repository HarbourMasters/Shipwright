#ifndef Z_OBJ_MU_PICT_H
#define Z_OBJ_MU_PICT_H

#include "global.h"

struct ObjMuPict;

#define OBJMUPICT_GET_F000(thisx) (((thisx)->params & 0xF000) >> 0xC)

typedef void (*ObjMuPictActionFunc)(struct ObjMuPict*, PlayState*);

typedef struct ObjMuPict {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjMuPictActionFunc actionFunc;
    /* 0x148 */ s16 unk148;
    /* 0x14A */ s16 unk14A;
    /* 0x14C */ u16 textId;
} ObjMuPict; // size = 0x150

#endif // Z_OBJ_MU_PICT_H

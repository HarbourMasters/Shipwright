#ifndef Z_OBJ_MURE3_H
#define Z_OBJ_MURE3_H

#include "global.h"

#define OBJMURE3_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define OBJMURE3_PARAM_RUPEEINDEX(thisx) (((thisx)->params >> 13) & 7)

struct ObjMure3;

typedef void (*ObjMure3ActionFunc)(struct ObjMure3*, PlayState*);
typedef void (*ObjMure3SpawnFunc)(struct ObjMure3*, PlayState*);

typedef struct ObjMure3 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjMure3ActionFunc actionFunc;
    /* 0x148 */ EnItem00* unk148[6];
    /* 0x160 */ Actor* unk160;
    /* 0x164 */ u16 unk164;
} ObjMure3; // size = 0x168

#endif // Z_OBJ_MURE3_H

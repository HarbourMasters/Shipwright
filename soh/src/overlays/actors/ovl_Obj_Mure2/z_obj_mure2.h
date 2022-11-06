#ifndef Z_OBJ_MURE2_H
#define Z_OBJ_MURE2_H

#include "ultra64.h"
#include "global.h"

struct ObjMure2;

typedef void (*ObjMure2ActionFunc)(struct ObjMure2*, PlayState*);

typedef struct ObjMure2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ObjMure2ActionFunc actionFunc;
    /* 0x0150 */ Actor* actorSpawnPtrList[12]; // pointers to all of the actors spawned by the parent
    /* 0x0180 */ u16 currentActorNum; // used to keep track of of the index to actorSpawnPtrList
    /* 0x0184 */ f32 unk_184; // some sort of distance
} ObjMure2; // size = 0x0188

#endif

#ifndef Z_OBJ_MURE_H
#define Z_OBJ_MURE_H

#include "ultra64.h"
#include "global.h"

struct ObjMure;

typedef void (*ObjMureActionFunc)(struct ObjMure*, PlayState*);

#define OBJMURE_MAX_SPAWNS 15

typedef struct ObjMure {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ObjMureActionFunc actionFunc;
    /* 0x0150 */ s16 chNum;
    /* 0x0152 */ s16 ptn;
    /* 0x0154 */ s16 svNum;
    /* 0x0156 */ s16 type;
    /* 0x0158 */ Actor* children[OBJMURE_MAX_SPAWNS];
    /* 0x0194 */ u8 childrenStates[OBJMURE_MAX_SPAWNS];
    /* 0x01A4 */ s16 unk_1A4;
    /* 0x01A6 */ s16 unk_1A6;
    /* 0x01A8 */ s16 unk_1A8;
} ObjMure; // size = 0x01AC

#endif

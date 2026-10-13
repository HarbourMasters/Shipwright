#ifndef Z_OBJ_MURE_H
#define Z_OBJ_MURE_H

#include "global.h"

struct ObjMure;

typedef void (*ObjMureActionFunc)(struct ObjMure*, PlayState*);

#define OBJ_MURE_GET_CHNUM(thisx) (((thisx)->params >> 12) & 0xF)
#define OBJ_MURE_GET_PTN(thisx) (((thisx)->params >> 8) & 0x7)
#define OBJ_MURE_GET_SVNUM(thisx) (((thisx)->params >> 5) & 0x3)
#define OBJ_MURE_GET_TYPE(thisx) ((thisx)->params & 0x1F)

typedef enum {
    /* 0 */ OBJMURE_TYPE_GRASS,
    /* 1 */ OBJMURE_TYPE_UNDEFINED,
    /* 2 */ OBJMURE_TYPE_FISH,
    /* 3 */ OBJMURE_TYPE_BUGS,
    /* 4 */ OBJMURE_TYPE_BUTTERFLY,
    /* 5 */ OBJMURE_TYPE_MAX
} ObjMureType;

#define OBJMURE_MAX_SPAWNS 15

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x53];
    /* 0x197 */ u8 unk_197;
} ObjMureChild; // size = 0x198

typedef struct ObjMure {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjMureActionFunc actionFunc;
    /* 0x148 */ s16 chNum;
    /* 0x14A */ s16 ptn;
    /* 0x14C */ s16 svNum;
    /* 0x14E */ s16 type;
    /* 0x150 */ Actor* children[OBJMURE_MAX_SPAWNS];
    /* 0x18C */ u8 childrenStates[OBJMURE_MAX_SPAWNS];
    /* 0x19C */ s16 unk_19C;
    /* 0x19E */ s16 unk_19E;
    /* 0x1A0 */ s16 unk_1A0;
} ObjMure; // size = 0x1A4

#endif // Z_OBJ_MURE_H

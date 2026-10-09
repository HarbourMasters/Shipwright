#ifndef Z_OBJ_NOZOKI_H
#define Z_OBJ_NOZOKI_H

#include "global.h"

struct ObjNozoki;

typedef void (*ObjNozokiActionFunc)(struct ObjNozoki*, PlayState*);

#define OBJNOZOKI_GET_SWITCH_FLAG1(thisx) ((thisx)->params & 0x7F)
#define OBJNOZOKI_GET_180(thisx) (((thisx)->params >> 7) & 3)
#define OBJNOZOKI_GET_200(thisx) ((thisx)->params & 0x200)
#define OBJNOZOKI_GET_400(thisx) ((thisx)->params & 0x400)
#define OBJNOZOKI_GET_SWITCH_FLAG2(thisx) (((thisx)->params >> 9) & 0x7F)

#define OBJNOZOKI_400 0x400

typedef struct ObjNozoki {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ u8 unk_15C;
    /* 0x15D */ u8 unk_15D;
    /* 0x15E */ s8 unk_15E;
    /* 0x15F */ s8 csId;
    /* 0x160 */ ObjNozokiActionFunc actionFunc;
} ObjNozoki; // size = 0x164

#endif // Z_OBJ_NOZOKI_H

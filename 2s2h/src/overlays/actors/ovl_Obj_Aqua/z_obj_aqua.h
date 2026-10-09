#ifndef Z_OBJ_AQUA_H
#define Z_OBJ_AQUA_H

#include "global.h"

#define AQUA_GET_TYPE(thisx) ((thisx)->params & 1)

#define AQUA_PARAMS(type) ((type) & 1)

typedef enum {
    /* 0 */ AQUA_TYPE_COLD,
    /* 1 */ AQUA_TYPE_HOT
} AquaType;

struct ObjAqua;

typedef void (*ObjAquaActionFunc)(struct ObjAqua*, PlayState*);

typedef struct ObjAqua {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ ObjAquaActionFunc actionFunc;
    /* 0x194 */ s16 counter;
    /* 0x196 */ u8 alpha;
    /* 0x198 */ s16 unk_198;
} ObjAqua; // size = 0x19C

#endif // Z_OBJ_AQUA_H

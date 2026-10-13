#ifndef Z_OBJ_ROTLIFT_H
#define Z_OBJ_ROTLIFT_H

#include "global.h"

#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"

#define OBJROTLIFT_GET_TYPE(thisx) ((thisx)->params & 1)
#define OBJROTLIFT_FIRST_DEKU_FLOWER_IS_GOLD(thisx) ((thisx)->params & 0x4000)

typedef enum {
    /* 0 */ OBJROTLIFT_TYPE_PLATFORMS,
    /* 1 */ OBJROTLIFT_TYPE_SPIKES
} ObjRotliftType;

struct ObjRotlift;

typedef struct ObjRotlift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjEtcetera* dekuFlowers[2];
} ObjRotlift; // size = 0x164

#endif // Z_OBJ_ROTLIFT_H

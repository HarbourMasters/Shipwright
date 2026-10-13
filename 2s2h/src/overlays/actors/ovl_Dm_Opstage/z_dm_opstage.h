#ifndef Z_DM_OPSTAGE_H
#define Z_DM_OPSTAGE_H

#include "global.h"

#define DMOPSTAGE_GET_TYPE(thisx) ((thisx)->params & 0xFF)
#define DMOPSTAGE_GET_CUE_TYPE_OFFSET(thisx) (((thisx)->params >> 8) & 0xFF)

struct DmOpstage;

typedef void (*DmOpstageActionFunc)(struct DmOpstage*, PlayState*);

typedef enum {
    /* 0 */ DMOPSTAGE_TYPE_GROUND,
    /* 1 */ DMOPSTAGE_TYPE_ROOT_TREE,
    /* 2 */ DMOPSTAGE_TYPE_CUT_TREE,
    /* 3 */ DMOPSTAGE_TYPE_STRAIGHT_TREE
} DmOpStageTypes;

typedef struct DmOpstage {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ UNK_TYPE1 pad_15C[0x44];
    /* 0x1A0 */ DmOpstageActionFunc actionFunc;
    /* 0x1A4 */ Vec3f drawOffset;
} DmOpstage; // size = 0x1B0

#endif // Z_DM_OPSTAGE_H

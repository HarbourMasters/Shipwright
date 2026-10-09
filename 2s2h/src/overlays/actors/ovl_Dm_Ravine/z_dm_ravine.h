#ifndef Z_DM_RAVINE_H
#define Z_DM_RAVINE_H

#include "global.h"

struct DmRavine;

typedef void (*DmRavineActionFunc)(struct DmRavine*, PlayState*);

typedef enum {
    /* 0 */ DM_RAVINE_STATE_INITIALIZED,
    /* 1 */ DM_RAVINE_STATE_ACTIVE,
    /* 2 */ DM_RAVINE_STATE_PENDING_DEATH
} DmRavineState;

typedef struct DmRavine {
    /* 0x000 */ Actor actor;
    /* 0x144 */ DmRavineActionFunc actionFunc;
    /* 0x148 */ u8 state;
    /* 0x149 */ u8 isActive;
} DmRavine; // size = 0x14C

#endif // Z_DM_RAVINE_H

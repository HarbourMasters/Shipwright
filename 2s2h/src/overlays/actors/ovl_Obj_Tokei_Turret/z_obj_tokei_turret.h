#ifndef Z_OBJ_TOKEI_TURRET_H
#define Z_OBJ_TOKEI_TURRET_H

#include "global.h"

#define OBJ_TOKEI_TURRET_TIER_TYPE(thisx) ((thisx)->params & 3)

typedef enum {
    /* 0 */ TURRET_TIER_BASE,
    /* 1 */ TURRET_TIER_TOP
} TurretTierType;

struct ObjTokeiTurret;

typedef struct ObjTokeiTurret {
    /* 0x000 */ DynaPolyActor dyna;
} ObjTokeiTurret; // size = 0x15C

#endif // Z_OBJ_TOKEI_TURRET_H

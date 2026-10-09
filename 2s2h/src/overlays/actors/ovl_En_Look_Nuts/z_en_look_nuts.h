#ifndef Z_EN_LOOK_NUTS_H
#define Z_EN_LOOK_NUTS_H

#include "global.h"
#include "objects/object_dnk/object_dnk.h"

struct EnLookNuts;

typedef void (*EnLookNutsActionFunc)(struct EnLookNuts*, PlayState*);

#define LOOKNUTS_GET_SPAWN_INDEX(thisx) (((thisx)->params >> 0xC) & 0xF)
#define LOOKNUTS_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define LOOKNUTS_GET_PATH_INDEX(thisx) (((thisx)->params >> 0x7) & 0x1F)

#define LOOKNUTS_SWITCH_FLAG_NONE 0x7F
#define LOOKNUTS_PATH_INDEX_NONE 0x1F

typedef struct EnLookNuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[DEKU_PALACE_GUARD_LIMB_MAX];
    /* 0x1CA */ Vec3s morphTable[DEKU_PALACE_GUARD_LIMB_MAX];
    /* 0x20C */ EnLookNutsActionFunc actionFunc;
    /* 0x210 */ Path *path;
    /* 0x214 */ s16 waypointIndex; // Index for the point where the deku guard is in its path 
    /* 0x216 */ s16 eyeState;
    /* 0x218 */ s16 blinkTimer; 
    /* 0x21A */ s16 eventTimer; // Timer to trigger when another event within the actor will happen
    /* 0x21C */ s16 state;
    /* 0x21E */ s16 switchFlag;
    /* 0x220 */ s16 pathIndex; // Determines path that a guard will patrol
    /* 0x222 */ s16 isPlayerDetected;
    /* 0x224 */ s16 waitTimer; // Timer for how long the deku guard will take a break for
    /* 0x226 */ s16 spawnIndex;
    /* 0x228 */ UNK_TYPE1 pad228[0x4];
    /* 0x22C */ Vec3f headRot;
    /* 0x238 */ Vec3f headRotTarget; // Target value for head rotation
    /* 0x244 */ ColliderCylinder collider;
} EnLookNuts; // size = 0x290

#endif // Z_EN_LOOK_NUTS_H

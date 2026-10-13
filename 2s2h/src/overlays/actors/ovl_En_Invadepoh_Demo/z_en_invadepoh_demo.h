#ifndef Z_EN_INVADEPOH_DEMO_H
#define Z_EN_INVADEPOH_DEMO_H

#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_cow/object_cow.h"
#include "assets/objects/object_ma1/object_ma1.h"
#include "assets/objects/object_uch/object_uch.h"

typedef enum {
    /* 0 */ EN_INVADEPOH_DEMO_TYPE_ALIEN,
    /* 1 */ EN_INVADEPOH_DEMO_TYPE_ROMANI,
    /* 2 */ EN_INVADEPOH_DEMO_TYPE_COW,
    /* 3 */ EN_INVADEPOH_DEMO_TYPE_UFO,
    /* 4 */ EN_INVADEPOH_DEMO_TYPE_COW_TAIL,
    /* 5 */ EN_INVADEPOH_DEMO_TYPE_MAX
} EnInvadepohDemoType;

#define EN_INVADEPOH_DEMO_GET_TYPE(thisx) ((thisx)->params & 0xF)
#define EN_INVADEPOH_DEMO_GET_CUEID_OFFSET(thisx) (((thisx)->params >> 11) & 0x7)
#define EN_INVADEPOH_DEMO_GET_PATH_INDEX(thisx) (((thisx)->params >> 4) & 0x7F)
#define EN_INVADEPOH_DEMO_GET_SPEED(thisx) ((thisx)->shape.rot.z)

struct EnInvadepohDemo;

typedef void (*EnInvadepohDemoActionFunc)(struct EnInvadepohDemo*, PlayState*);

// This actor has multiple arrays of function pointers which are used for things like
// Init, Destroy, Draw, and something similar to an Update function. Some of these
// functions do not have any stack space for a recast temp, which means that they must
// take an EnInvadepohDemo pointer, not an Actor pointer. In other words, these
// functions are not ActorFuncs. This typedef is used for these arrays of function
// pointers to better distinguish them from "actual" action functions, even if they
// have the exact same signature.
typedef void (*EnInvadepohDemoFunc)(struct EnInvadepohDemo*, PlayState*);

typedef struct EnInvadepohDemo {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnInvadepohDemoActionFunc actionFunc;
    /* 0x148 */ s32 type; // "type"
    /* 0x14C */ f32 speed;
    /* 0x150 */ s32 cueChannel;
    /* 0x154 */ s32 cueId; // "demo_mode"
    /* 0x158 */ s32 pathIndex;
    /* 0x15C */ s32 pointIndex;
    /* 0x160 */ s32 objectSlot;
    /* 0x164 */ s32 drawFlags; // Only has one flag to control whether or not to draw
    /* 0x168 */ s32 cueIdOffset;
    /* 0x16C */ s16 ufoRotZ;
    /* 0x170 */ SkelAnime skelAnime;
    /* 0x1B4 */ Vec3s jointTable[ROMANI_LIMB_MAX];
    /* 0x23E */ Vec3s morphTable[ROMANI_LIMB_MAX];
} EnInvadepohDemo; // size = 0x2C8

#endif // Z_EN_INVADEPOH_DEMO_H

#ifndef Z_EN_LIFT_NUTS_H
#define Z_EN_LIFT_NUTS_H

#include "global.h"
#include "objects/object_dnt/object_dnt.h"

struct EnLiftNuts;

typedef void (*EnLiftNutsActionFunc)(struct EnLiftNuts*, PlayState*);

#define ENLIFTNUTS_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFF00) >> 8)

typedef struct EnLiftNuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnLiftNutsActionFunc actionFunc;
    /* 0x1D8 */ Vec3f waypointPos;
    /* 0x1E4 */ s32 eyeTexIndex;
    /* 0x1E8 */ s32 unk1E8; // Set but never used
    /* 0x1EC */ s16* minigameScore; // Pointer to shared memory location with actor EnGamelupy
    /* 0x1F0 */ Vec3s jointTable[BUSINESS_SCRUB_LIMB_MAX];
    /* 0x298 */ Vec3s morphTable[BUSINESS_SCRUB_LIMB_MAX];
    /* 0x340 */ UNK_TYPE1 unk_340[0xC];
    /* 0x34C */ s16 textId;
    /* 0x34E */ s16 autotalk;
    /* 0x350 */ UNK_TYPE1 unk_350[0x4];
    /* 0x354 */ s16 timer; // Frame counter used for various different things
    /* 0x356 */ s16 isFirstTimeHiding;
} EnLiftNuts; // size = 0x358

#endif // Z_EN_LIFT_NUTS_H

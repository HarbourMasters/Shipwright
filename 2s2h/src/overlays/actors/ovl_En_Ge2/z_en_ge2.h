#ifndef Z_EN_GE2_H
#define Z_EN_GE2_H

#include "global.h"
#include "z64snap.h"
#include "objects/object_gla/object_gla.h"

struct EnGe2;

typedef void (*EnGe2ActionFunc)(struct EnGe2*, PlayState*);

#define GERUDO_PURPLE_GET_EXIT(thisx) (((thisx)->params) & 0x1F) //!< Exit to send Player to when caught
#define GERUDO_PURPLE_GET_TYPE(thisx) (((thisx)->params & 0xE0) >> 5)
#define GERUDO_PURPLE_GET_PATH_INDEX(thisx) ((((thisx)->params) & 0xFC00) >> 10)

#define GERUDO_PURPLE_PARAMS(exit, type, pathIndex) (((exit) & 0x1F) | (((type) & 7) << 5) | (((pathIndex) & 0x3F) << 10))

#define GERUDO_PURPLE_EXIT_NONE 0x1F
#define GERUDO_PRUPLE_PATH_INDEX_NONE 0x3F

typedef enum {
    /* 0 */ GERUDO_PURPLE_TYPE_CUTSCENE,
    /* 1 */ GERUDO_PURPLE_TYPE_BOAT_SENTRY, //!< on boats
    /* 2 */ GERUDO_PURPLE_TYPE_AVEIL_GUARD,
    /* 7 */ GERUDO_PURPLE_TYPE_FORTRESS = 7 //!< In both courtyard and rooms
} GerudoPurpleType;

//! TODO: work out where to put this
typedef enum {
    /* 1 */ ENGE2_CUEID_BEEHIVE_PATROL = 1,
    /* 2 */ ENGE2_CUEID_BEEHIVE_RUN_AWAY,
    /* 3 */ ENGE2_CUEID_BEEHIVE_EXIT,
    /* 4 */ ENGE2_CUEID_GBT_ENTR_STAND_STILL,
    /* 5 */ ENGE2_CUEID_GBT_ENTR_BLOWN_AWAY
} EnGe2CsAction;

typedef struct EnGe2 {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ SkelAnime skelAnime;
    /* 0x1D8 */ Vec3s jointTable[GERUDO_PURPLE_LIMB_MAX];
    /* 0x25C */ Vec3s morphTable[GERUDO_PURPLE_LIMB_MAX];
    /* 0x2E0 */ s16 eyeIndex;
    /* 0x2E2 */ s16 blinkTimer;
    /* 0x2E4 */ Vec3s headRot;
    /* 0x2E8 */ Vec3s torsoRot; // unused, inferred from similar usage in other actors
    /* 0x2F0 */ Path* path;
    /* 0x2F4 */ s32 curPointIndex;
    /* 0x2F8 */ u16 stateFlags;
    /* 0x2FA */ s16 yawTarget; //!< used when Player detected to fix the yaw to turn to
    /* 0x2FC */ f32 verticalDetectRange; //!< vertical range to look for the player within
    /* 0x300 */ u8 timer;
    /* 0x301 */ u8 detectedStatus;
    /* 0x302 */ s16 cueId;
    /* 0x304 */ s16 unk304; // unused
    /* 0x306 */ s16 screamTimer;
    /* 0x308 */ EnGe2ActionFunc actionFunc;
} EnGe2; // size = 0x30C

#endif // Z_EN_GE2_H

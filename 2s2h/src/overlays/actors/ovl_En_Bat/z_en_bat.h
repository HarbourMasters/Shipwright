#ifndef Z_EN_BAT_H
#define Z_EN_BAT_H

#include "global.h"

#define BAD_BAT_GET_TYPE(thisx) (((thisx)->params >> 0) & 0x1F)
#define BAD_BAT_GET_PARAMFLAGS(thisx) (((thisx)->params >> 5) & 7)
#define BAD_BAT_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0xFF)
#define BAD_BAT_PARAMS(switchFlag, paramflags, type) ((type) | ((paramflags) << 5) | ((switchFlag) << 8))

//! PARAMFLAGS
//! Affects which flags are used in Actor_UpdateBgCheckInfo in certain circumstances
#define BAD_BAT_PARAMFLAG_0 (1 << 0)
//! Whether to apply a height range check to attack
#define BAD_BAT_PARAMFLAG_CHECK_HEIGHTREL (1 << 1)
//! Spawn as perching instead of flying
#define BAD_BAT_PARAMFLAG_PERCH (1 << 2)

#define BAD_BAT_GET_NUMBER_TO_SPAWN(thisx) ((thisx)->params)
#define BAD_BAT_SPAWN_COUNT(n) (n)

struct EnBat;

typedef void (*EnBatActionFunc)(struct EnBat*, PlayState*);

typedef enum BadBatBodyPart {
    /* 0 */ BAD_BAT_BODYPART_0,
    /* 1 */ BAD_BAT_BODYPART_1,
    /* 2 */ BAD_BAT_BODYPART_2,
    /* 3 */ BAD_BAT_BODYPART_MAX
} BadBatBodyPart;

typedef struct EnBat {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnBatActionFunc actionFunc;
    /* 0x148 */ u8 drawDmgEffType;
    /* 0x149 */ u8 paramFlags;
    /* 0x14A */ s16 switchFlag;
    /* 0x14C */ s16 timer;
    /* 0x14E */ s16 pitchTarget;
    /* 0x150 */ s16 yawTarget;
    /* 0x152 */ s16 animationFrame; //!< Manual animation using displaylists
    /* 0x154 */ f32 drawDmgEffScale;
    /* 0x158 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x15C */ f32 drawDmgEffAlpha;
    /* 0x160 */ Vec3f bodyPartsPos[BAD_BAT_BODYPART_MAX];
    /* 0x184 */ ColliderSphere collider;
} EnBat; // size = 0x1DC

#endif // Z_EN_BAT_H

#ifndef Z_EN_AOB_01_H
#define Z_EN_AOB_01_H

#include "global.h"
#include "objects/object_aob/object_aob.h"

struct EnAob01;

typedef void (*EnAob01ActionFunc)(struct EnAob01*, PlayState*);

// This needs to be kept in sync with the number of weekEventRegs used to store the text ID offsets for
// the race dogs. In particular, there needs to be one weekEventReg for every two dogs. This needs to be
// an even number to ensure that all text ID offsets are created (see EnAob01_InitializeDogTextOffsets).
#define RACEDOG_COUNT 14

#define ENAOB01_GET_STARTING_DOG_PATH_INDEX(thisx) (((thisx)->params & 0x7E00) >> 9)

//! @bug This should be shifted up by 1 to match how EnRacedog expects the path index to be supplied.
//! It doesn't cause any problems in the final game because every time EnAob01 is spawned, all the
//! bits here are zero.
#define ENAOB01_GET_RACEDOG_PATH_INDEX(thisx) ((thisx)->params & 0x7E00)

#define ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE (1 << 1)
#define ENAOB01_FLAG_PLAYER_TOLD_TO_PICK_A_DOG (1 << 2)
#define ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG (1 << 3)
#define ENAOB01_FLAG_CONVERSATION_OVER (1 << 4)
#define ENAOB01_FLAG_STARTED_RACE (1 << 5)
#define ENAOB01_FLAG_LAUGH (1 << 6)
#define ENAOB01_FLAG_SURPRISE (1 << 7)
#define ENAOB01_FLAG_PLAYER_CAN_TALK (1 << 8)

#define ENAOB01_PATH_INDEX_NONE 0x3F

typedef struct EnAob01 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnAob01ActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ Path* dogPaths[RACEDOG_COUNT];
    /* 0x210 */ u16 textId;
    /* 0x212 */ Vec3s jointTable[MAMAMU_YAN_LIMB_MAX];
    /* 0x272 */ Vec3s morphTable[MAMAMU_YAN_LIMB_MAX];
    /* 0x2D2 */ u16 stateFlags;
    /* 0x2D4 */ Vec3s trackTarget;
    /* 0x2DA */ Vec3s headRot;
    /* 0x2E0 */ Vec3s torsoRot;
    /* 0x2E6 */ Vec3s prevTrackTarget;
    /* 0x2EC */ Vec3s prevHeadRot;
    /* 0x2F2 */ Vec3s prevTorsoRot;
    /* 0x2F8 */ s16 fidgetTableY[MAMAMU_YAN_LIMB_MAX];
    /* 0x318 */ s16 fidgetTableZ[MAMAMU_YAN_LIMB_MAX];
    /* 0x338 */ UNK_TYPE1 unk338[0xB6];
    /* 0x3EE */ s16 eyeIndex;
    /* 0x3F0 */ s16 blinkTimer;
    /* 0x3F4 */ Actor* dogToFollow;
    /* 0x3F8 */ Actor* dogs[RACEDOG_COUNT];
    /* 0x430 */ s16 csId;
    /* 0x432 */ s16 selectedDogIndex;
    /* 0x434 */ s32 rupeesBet; // Also used for determining how many rupees to give to the player if they place 1st-5th in the race.
    /* 0x438 */ UNK_TYPE1 unk438[4];
    /* 0x43C */ s32 animIndex;
    /* 0x440 */ s16 forceRaceEndTimer;
} EnAob01; // size = 0x444

#endif // Z_EN_AOB_01_H

#ifndef Z64_DOOR_H
#define Z64_DOOR_H

#include "ultra64.h"
#include "z64actor.h"

typedef struct KnobDoorActor {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ u8 animIndex; // Value between 0 ~ 9. 0 ~ 4 are for left doors while 5 ~ 9 are for right doors. Each value maps to a PlayerTransformation
    /* 0x1A1 */ u8 requestOpen; // An actor can set this member to `true` to request this door to open. It must also set `animIndex`
    /* 0x1A2 */ s8 objectSlot;
    /* 0x1A3 */ s8 dlIndex;
} KnobDoorActor; // size = 0x1A4

typedef struct SlidingDoorActor {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ s16 unk_15C;
    /* 0x15E */ s16 unk_15E;
} SlidingDoorActor; // size = 0x160

#define TRANSITION_ACTOR_PARAMS_INDEX_SHIFT 10
#define TRANSITION_ACTOR_PARAMS(index, params) ((index) << TRANSITION_ACTOR_PARAMS_INDEX_SHIFT) + ((params) & 0x3FF)

#define DOOR_GET_TRANSITION_ID(thisx) ((u16)(thisx)->params >> TRANSITION_ACTOR_PARAMS_INDEX_SHIFT)

#endif

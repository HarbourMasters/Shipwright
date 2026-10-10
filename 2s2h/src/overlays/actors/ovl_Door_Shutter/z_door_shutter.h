#ifndef Z_DOOR_SHUTTER_H
#define Z_DOOR_SHUTTER_H

#include "global.h"
#include "z64door.h"

struct DoorShutter;

typedef void (*DoorShutterActionFunc)(struct DoorShutter*, PlayState*);

typedef enum DoorShutterType {
    /* 0 */ DOORSHUTTER_TYPE_0,
    /* 1 */ DOORSHUTTER_TYPE_1,
    /* 2 */ DOORSHUTTER_TYPE_2,
    /* 3 */ DOORSHUTTER_TYPE_3,
    /* 4 */ DOORSHUTTER_TYPE_4,
    /* 5 */ DOORSHUTTER_TYPE_BOSS_DOOR,
    /* 6 */ DOORSHUTTER_TYPE_6,
    /* 7 */ DOORSHUTTER_TYPE_7
} DoorShutterType;

#define DOORSHUTTER_GET_1F(thisx) ((thisx)->params & 0x1F)
#define DOORSHUTTER_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define DOORSHUTTER_GET_TYPE(thisx) (((thisx)->params >> 7) & 7)
#define DOORSHUTTER_PARAMS_GET_TYPE(params) (((params) >> 7) & 7)

typedef struct DoorShutter {
    /* 0x000 */ SlidingDoorActor slidingDoor;
    /* 0x160 */ s16 csId;
    /* 0x0162 */ u8 doorType;
    /* 0x0163 */ u8 unk_163;
    /* 0x0164 */ u8 unk_164;
    /* 0x0165 */ s8 objectSlot;
    /* 0x0166 */ s8 unk_166;
    /* 0x0167 */ s8 unk_167;
    /* 0x0168 */ f32 unk_168;
    /* 0x016C */ DoorShutterActionFunc actionFunc;
} DoorShutter; // size = 0x170

#endif // Z_DOOR_SHUTTER_H

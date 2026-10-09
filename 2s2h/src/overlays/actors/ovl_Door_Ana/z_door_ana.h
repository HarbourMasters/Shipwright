#ifndef Z_DOOR_ANA_H
#define Z_DOOR_ANA_H

#include "global.h"

struct DoorAna;

typedef void (*DoorAnaActionFunc)(struct DoorAna*, PlayState*);

typedef struct DoorAna {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder bombCollider;
    /* 0x190 */ DoorAnaActionFunc actionFunc;
} DoorAna; // size = 0x194

#define DOORANA_TYPE_BITMASK            0x300
#define DOORANA_TYPE_VISIBLE            0x000
#define DOORANA_TYPE_HIDDEN_STORMS      0x100 // Incomplete, leftover from OoT 
#define DOORANA_TYPE_HIDDEN_BOMB        0x200
#define DOORANA_TYPE_VISIBLE_SCENE_EXIT 0x300 // Destination dependent on scene exit list

#define DOORANA_GET_TYPE(thisx) ((thisx)->params & DOORANA_TYPE_BITMASK)
#define DOORANA_GET_ENTRANCE(thisx) ((((thisx)->params >> 12) & 0x7) - 1) 
#define DOORANA_GET_EX_ENTRANCE(thisx) ((thisx)->home.rot.z + 1)
#define DOORANA_GET_EXIT_INDEX(thisx) ((thisx)->params & 0x1F)
#define DOORANA_GET_ITEMFLAGS(thisx) ((thisx)->params & 0xFF)
#define DOORANA_SET_TYPE(thisx, t) ((thisx)->params = ((thisx)->params & ~DOORANA_TYPE_BITMASK) + (t))

#endif // Z_DOOR_ANA_H

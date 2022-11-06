#ifndef Z_OBJ_LIGHTSWITCH_H
#define Z_OBJ_LIGHTSWITCH_H

#include "ultra64.h"
#include "global.h"

struct ObjLightswitch;

typedef void (*ObjLightswitchActionFunc)(struct ObjLightswitch*, PlayState*);

typedef enum {
    /* 0 */ OBJLIGHTSWITCH_TYPE_STAY_ON, // doesn't turn off unless the switch flag is cleared some other way
    /* 1 */ OBJLIGHTSWITCH_TYPE_1,       // turns on and off
    /* 2 */ OBJLIGHTSWITCH_TYPE_2,       // turns on and off
    /* 3 */ OBJLIGHTSWITCH_TYPE_BURN     // disappears when turned on
} ObjLightswitch_Type;

typedef struct ObjLightswitch {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ObjLightswitchActionFunc actionFunc;
    /* 0x0150 */ ColliderJntSph collider;
    /* 0x0170 */ ColliderJntSphElement colliderItems[1];
    /* 0x01B0 */ s16 timer;       // collision-related threshold and controls animation/logic when turning on/off
    /* 0x01B2 */ s16 toggleDelay; // timer ticking down used for delaying tuning on/off or disappearing, can be bypassed
    /* 0x01B4 */ s16 faceTextureIndex; // texture used by the center part of the sun
    /* 0x01B6 */ s16 color[3];         // rgb, (0-255 color component) << 6
    /* 0x01BC */ s16 alpha;            // (0-255 alpha) << 6
    /* 0x01BE */ s16 flameRingRot;
    /* 0x01C0 */ s16 flameRingRotSpeed;
    /* 0x01C2 */ u8 prevFrameACflags;
} ObjLightswitch; // size = 0x01C4

#endif

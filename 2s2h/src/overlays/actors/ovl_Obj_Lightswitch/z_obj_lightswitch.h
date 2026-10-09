#ifndef Z_OBJ_LIGHTSWITCH_H
#define Z_OBJ_LIGHTSWITCH_H

#include "global.h"

struct ObjLightswitch;

typedef void (*ObjLightswitchActionFunc)(struct ObjLightswitch*, PlayState*);
typedef void (*ObjLightswitchSetupFunc)(struct ObjLightswitch*);

typedef struct ObjLightswitch {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider;
    /* 0x164 */ ColliderJntSphElement elements;
    /* 0x1A4 */ ObjLightswitchActionFunc actionFunc;
    /* 0x1A8 */ s16 colorR; // not RGBA8 as 4x s16 (not 4x s8)
    /* 0x1AA */ s16 colorG;
    /* 0x1AC */ s16 colorB;
    /* 0x1AE */ s16 colorAlpha;
    /* 0x1B0 */ s16 edgeRot;
    /* 0x1B2 */ s16 edgeRotSpeed;
    /* 0x1B4 */ s8 colorShiftTimer;
    /* 0x1B5 */ s8 hitState;
    /* 0x1B6 */ s8 faceState;
    /* 0x1B7 */ u8 previousACFlags;
    /* 0x1B8 */ ObjLightswitchSetupFunc setupFunc;
    /* 0x1BC */ s8 switchFlagSetType; // 1 or 0 , for set or unset flags
    /* 0x1BD */ s8 cutsceneTimer;

} ObjLightswitch; // size = 0x1C0

#define LIGHTSWITCH_FACE_ASLEEP 0
#define LIGHTSWITCH_FACE_WAKING 1
#define LIGHTSWITCH_FACE_FULLAWAKE 2

#define LIGHTSWITCH_TYPE_REGULAR 0
#define LIGHTSWITCH_TYPE_FLIP 1
#define LIGHTSWITCH_TYPE_UNK2 2
#define LIGHTSWITCH_TYPE_FAKE 3

#define LIGHTSWITCH_GET_TYPE(thisx) (((thisx)->params >> 4) & 0x3)
#define LIGHTSWITCH_GET_INVISIBLE(thisx) (((thisx)->params >> 3) & 1)
#define LIGHTSWITCH_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)

#endif // Z_OBJ_LIGHTSWITCH_H

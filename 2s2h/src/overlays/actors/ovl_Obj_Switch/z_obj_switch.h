#ifndef Z_OBJ_SWITCH_H
#define Z_OBJ_SWITCH_H

#include "global.h"

struct ObjSwitch;

typedef void (*ObjSwitchActionFunc)(struct ObjSwitch* this, PlayState* play);
typedef void (*ObjSwitchDrawFunc)(struct ObjSwitch* this, PlayState* play);
typedef void (*ObjSwitchSetupActionFunc)(struct ObjSwitch* this);

typedef enum {
    /* 0 */ OBJSWITCH_TYPE_FLOOR,
    /* 1 */ OBJSWITCH_TYPE_FLOOR_RUSTY, // Unused, incomplete implementation
    /* 2 */ OBJSWITCH_TYPE_EYE,
    /* 3 */ OBJSWITCH_TYPE_CRYSTAL,
    /* 4 */ OBJSWITCH_TYPE_CRYSTAL_TARGETABLE,
    /* 5 */ OBJSWITCH_TYPE_FLOOR_LARGE
} ObjSwitchType;

typedef enum {
    /* 0 */ OBJSWITCH_SUBTYPE_ONCE, // Set Switch Flag when activated, stays activated
    /* 1 */ OBJSWITCH_SUBTYPE_TOGGLE, // Toggle Switch Flag when activated
    /* 2 */ OBJSWITCH_SUBTYPE_RESET, // Floor type only, set switch flag when pressed, unset when released
    /* 3 */ OBJSWITCH_SUBTYPE_RESET_INVERTED, // Floor type only, set switch flag when not pressed, unset when pressed
    /* 4 */ OBJSWITCH_SUBTYPE_SYNC // Crystal type only, syncronizes all crystal fl
} ObjSwitchSubType;

#define OBJSWITCH_GET_33(thisx) ((thisx)->params & 0x33)
#define OBJSWITCH_GET_7F00(thisx) (((thisx)->params >> 8) & 0x7F)

#define OBJSWITCH_NORMAL_BLUE 0x20
#define OBJSWITCH_INVERSE_BLUE 0x30

typedef struct ObjSwitch {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjSwitchActionFunc actionFunc;
    /* 0x160 */ s16 floorSwitchReleaseTimer;
    /* 0x162 */ s16 disableCrystalSwitchTimer;
    /* 0x164 */ s8 eyeTexIndex;
    /* 0x165 */ s8 sfxTimer;
    /* 0x168 */ s32 crystalAnimTimer;
    /* 0x16C */ Color_RGB8 color;
    /* 0x16F */ u8 collisionFlags;
    /* 0x170 */ s8 floorSwitchPlayerSnapState;
    /* 0x171 */ s8 nextSwitchFlagState;
    /* 0x172 */ s8 isPlayingCutscene;
    /* 0x174 */ ObjSwitchSetupActionFunc setupFunc;
    union {
        struct {
    /* 0x178 */ ColliderJntSph colliderJntSph;
    /* 0x198 */ ColliderJntSphElement colliderJntSphElements[1];
        };
        struct {
    /* 0x178 */ ColliderTris colliderTris;
    /* 0x198 */ ColliderTrisElement colliderTrisElements[2];
        };
    };
    /* 0x250 */ f32 floorSwitchUpScale;
    /* 0x254 */ f32 floorSwitchDownScale;
} ObjSwitch; // size = 0x258

#define OBJ_SWITCH_GET_TYPE(thisx) ((thisx)->params&7)
#define OBJ_SWITCH_GET_SUBTYPE(thisx) ((thisx)->params >> 4 & 7)
#define OBJ_SWITCH_GET_SWITCH_FLAG(thisx) ((thisx)->params >> 8 & 0x7F)
#define OBJ_SWITCH_IS_FROZEN(thisx) ((thisx)->params >> 7 & 1)
#define OBJ_SWITCH_UNSET_FROZEN(thisx) ((thisx)->params &= ~(1 << 7))
#define OBJ_SWITCH_IS_INVISIBLE(thisx) ((thisx)->params >> 3 & 1)
#define OBJ_SWITCH_GET_COLOR_ID(thisx) ((thisx)->home.rot.z&1)

#endif // Z_OBJ_SWITCH_H

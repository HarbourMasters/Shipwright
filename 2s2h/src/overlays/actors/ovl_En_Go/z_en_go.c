/*
 * File: z_en_go.c
 * Overlay: ovl_En_Go
 * Description: Goron
 *
 * This actor handles multiple (but not all) Gorons. The Gorons included are:
 * - Athletics Goron: Gorons in the racetrack performing stretches before/after a race.
 * - Spectator Goron: Gorons scattered throughout the racetrack watching the race.
 * - Gatekeeper Goron: The Gatekeeper of the Goron Shrine.
 * - Graveyard Goron: Both Gorons outside the Goron Graveyard (one of which is initially frozen).
 * - Shrine Goron: Gorons in the Goron Shrine (specifically, one in the elders room, one outside the elder's room, and
 *     one outside the shop, not all Gorons in the Goron Shrine are covered by this actor).
 * - Medigoron: The Powder Keg making Medigoron in Goron Village.
 */
#include "z_en_go.h"
#include "z64quake.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/object_hakugin_demo/object_hakugin_demo.h"
#include "objects/object_taisou/object_taisou.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Obj_Aqua/z_obj_aqua.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

#define ENGO_STANDING_Y_OFFSET 0.0f  // Actor shape offset in use when a Goron is in any standing state.
#define ENGO_ROLLEDUP_Y_OFFSET 14.0f // Actor shape offset in use when a Goron is "rolled up".
#define ENGO_SNOWBALL_Y_OFFSET 46.0f // Actor shape offset in use when a Goron is in a snowball.

#define ENGO_NORMAL_SCALE 0.01f              // "Unit Scale" of a normal Goron.
#define ENGO_MEDIGORON_SCALE_MULTIPLIER 5.0f // Scale factor for the Medigoron in the Powder Keg Shop.

#define ENGO_POUND_INITIAL_RISE_VEL 10.0f // Velocity at which the Gatekeeper's pound surges into the air.
#define ENGO_POUND_INITIAL_FALL_VEL -6.0f // Velocity at which the Gatekeeper's pound surges to the ground.

#define ENGO_DUST_STEAM_LIFETIME 16 // Lifetime in frames of the Dust and Steam effects.

#define ENGO_FLAG_ENGAGED (1 << 3)
#define ENGO_FLAG_FACE_TARGET (1 << 4)
#define ENGO_FLAG_EYES_OPEN (1 << 5)
#define ENGO_FLAG_LOST_ATTENTION (1 << 6)
#define ENGO_FLAG_STANDING (1 << 7)
#define ENGO_FLAG_SNOWBALLED (1 << 8)
#define ENGO_FLAG_ROLLED_UP (1 << 9)
#define ENGO_FLAG_FROZEN (1 << 10)
#define ENGO_FLAG_HIT_OTHER (1 << 11)
#define ENGO_FLAG_HIT_BY_OTHER (1 << 12)
#define ENGO_FLAG_HIT_OBJ (1 << 13)
#define ENGO_FLAG_ROLLING_UP (1 << 14)
#define ENGO_FLAG_UNROLLING (1 << 15)

void EnGo_Init(Actor* thisx, PlayState* play);
void EnGo_Destroy(Actor* thisx, PlayState* play);
void EnGo_Update(Actor* thisx, PlayState* play);
void EnGo_Draw(Actor* thisx, PlayState* play);

typedef enum EnGoEyeTexture {
    /* 0 */ ENGO_EYETEX_OPEN,
    /* 1 */ ENGO_EYETEX_HALF,
    /* 2 */ ENGO_EYETEX_CLOSED,
    /* 3 */ ENGO_EYETEX_HALF2,
    /* 4 */ ENGO_EYETEX_CLOSED2
} EnGoEyeTexture;

typedef enum EnGoSleepState {
    /* -1 */ ENGO_ASLEEP_NEG = -1,
    /*  0 */ ENGO_AWAKE,
    /*  1 */ ENGO_ASLEEP_POS
} EnGoSleepState;

typedef enum EnGoEffectType {
    /* 0 */ ENGO_EFFECT_NONE,
    /* 1 */ ENGO_EFFECT_SNOW1,
    /* 2 */ ENGO_EFFECT_SNOW2,
    /* 3 */ ENGO_EFFECT_SNOW3,
    /* 4 */ ENGO_EFFECT_DUST_MIN,
    /* 4 */ ENGO_EFFECT_DUST1 = ENGO_EFFECT_DUST_MIN,
    /* 5 */ ENGO_EFFECT_DUST2,
    /* 6 */ ENGO_EFFECT_DUST3,
    /* 7 */ ENGO_EFFECT_STEAM_MIN,
    /* 7 */ ENGO_EFFECT_STEAM = ENGO_EFFECT_STEAM_MIN
} EnGoEffectType;

typedef enum EnGoAnimation {
    /* -1 */ ENGO_ANIM_NONE = -1,
    /*  0 */ ENGO_ANIM_GORON_MIN,
    /*  0 */ ENGO_ANIM_LYINGDOWNIDLE = ENGO_ANIM_GORON_MIN,
    /*  1 */ ENGO_ANIM_LYINGDOWNIDLE_IMM,
    /*  2 */ ENGO_ANIM_UNROLL,
    /*  3 */ ENGO_ANIM_UNROLL_IMM,
    /*  4 */ ENGO_ANIM_ROLL,
    /*  5 */ ENGO_ANIM_SHIVER,
    /*  6 */ ENGO_ANIM_SHIVER_IMM,
    /*  7 */ ENGO_ANIM_DROPKEG,
    /*  8 */ ENGO_ANIM_COVEREARS,
    /*  9 */ ENGO_ANIM_SHIVERINGSURPRISED,
    /* 10 */ ENGO_ANIM_ATHLETICS_MIN,
    /* 10 */ ENGO_ANIM_DOUBLE_ARM_SIDEBEND = ENGO_ANIM_ATHLETICS_MIN,
    /* 11 */ ENGO_ANIM_SQUAT_SIDE_TO_SIDE,
    /* 12 */ ENGO_ANIM_SHAKE_LIMBS,
    /* 13 */ ENGO_ANIM_SINGLE_ARM_SIDEBEND,
    /* 14 */ ENGO_ANIM_SITTING_STRETCH,
    /* 15 */ ENGO_ANIM_CHEER,
    /* 16 */ ENGO_ANIM_SHOUT,
    /* 17 */ ENGO_ANIM_HELP_SITTING_STRETCH,
    /* 18 */ ENGO_ANIM_SPRING_MIN,
    /* 18 */ ENGO_ANIM_SHOW = ENGO_ANIM_SPRING_MIN,
    /* 19 */ ENGO_ANIM_SHOW_LOOPED,
    /* 20 */ ENGO_ANIM_LOOK_AROUND,
    /* 21 */ ENGO_ANIM_LOOK_AROUND_LOOPED
} EnGoAnimation;

void EnGo_Idle(EnGo* this, PlayState* play);
void EnGo_Sleep(EnGo* this, PlayState* play);
void EnGo_Frozen(EnGo* this, PlayState* play);
void EnGo_AwaitThaw(EnGo* this, PlayState* play);
void EnGo_Thaw(EnGo* this, PlayState* play);

void EnGo_HandleSpringArrivalCutscene(EnGo* this, PlayState* play);

void EnGo_Snowball(EnGo* this, PlayState* play);

void EnGo_Talk(EnGo* this, PlayState* play);

// MsgEvent script for the Goron who made Darmani's grave in the mountain village.
static MsgScript sMsgScriptGoronGravemaker[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON, 0x0045 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x002A - 0x0008),
    /* 0x0008 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_21_02, 0x0025 - 0x000D),
    /* 0x000D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E10),
    /* 0x0010 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0011 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E11),
    /* 0x0014 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0015 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E12),
    /* 0x0018 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0019 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E13),
    /* 0x001C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E14),
    /* 0x0020 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0021 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_21_02),
    /* 0x0024 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0025 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E15),
    /* 0x0028 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0029 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x002A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_GORON_GRAVEMAKER_AS_GORON, 0x0040 - 0x002F),
    /* 0x002F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E16),
    /* 0x0032 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0033 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E17),
    /* 0x0036 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0037 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E18),
    /* 0x003A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003B 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_GORON_GRAVEMAKER_AS_GORON),
    /* 0x003E 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x003F 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0040 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E19),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0045 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_GORON_GRAVEMAKER_AS_GORON, 0x0057 - 0x004A),
    /* 0x004A 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0052 - 0x004D),
    /* 0x004D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E31),
    /* 0x0050 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0051 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0052 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E2F),
    /* 0x0055 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0056 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0057 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x005F - 0x005A),
    /* 0x005A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E2D),
    /* 0x005D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x005F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E2B),
    /* 0x0062 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0063 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for the frozen Goron aside the gravemaker in the mountain village.
static MsgScript sMsgScriptGoronFrozen[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON, 0x0083 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0055 - 0x0008),
    /* 0x0008 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_GORON_GRAVEMAKER_AS_GORON, 0x0025 - 0x000D),
    /* 0x000D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E1A),
    /* 0x0010 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0011 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E23),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E24),
    /* 0x001A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E25),
    /* 0x001E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001F 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0020 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0021 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON),
    /* 0x0024 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0025 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E1A),
    /* 0x0028 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0029 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x002A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E23),
    /* 0x002D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002E 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x002F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E24),
    /* 0x0032 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0033 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E25),
    /* 0x0036 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0037 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0038 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E26),
    /* 0x003B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003C 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x003D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E27),
    /* 0x0040 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0041 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0042 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E28),
    /* 0x0045 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0046 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0047 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E29),
    /* 0x004A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004B 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x004C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E2A),
    /* 0x004F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0050 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0051 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON),
    /* 0x0054 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0055 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E1A),
    /* 0x0058 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0059 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x005A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E1B),
    /* 0x005D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005E 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x005F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E1C),
    /* 0x0062 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0063 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E1D),
    /* 0x0066 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0067 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E1E),
    /* 0x006A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x006B 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x006C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E1F),
    /* 0x006F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0070 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0071 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E20),
    /* 0x0074 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0075 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0076 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E21),
    /* 0x0079 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x007A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E22),
    /* 0x007D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x007E 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x007F 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON),
    /* 0x0082 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0083 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_GORON_GRAVEMAKER_AS_GORON, 0x0095 - 0x0088),
    /* 0x0088 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0090 - 0x008B),
    /* 0x008B 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E32),
    /* 0x008E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x008F 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0090 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E30),
    /* 0x0093 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0094 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0095 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x009D - 0x0098),
    /* 0x0098 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E2E),
    /* 0x009B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x009C 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x009D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E2C),
    /* 0x00A0 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A1 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for the shrine Goron standing aside the store.
static MsgScript sMsgScriptGoronAsideStore[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E52),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for the shrine Goron standing aside the Elder's son.
static MsgScript sMsgScriptGoronAsideEldersSon[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON, 0x003D - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0018 - 0x0008),
    /* 0x0008 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E43),
    /* 0x000B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E44),
    /* 0x000F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0010 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E45),
    /* 0x0013 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0014 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_24_80),
    /* 0x0017 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0018 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_22_08, 0x0038 - 0x001D),
    /* 0x001D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E46),
    /* 0x0020 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0021 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E47),
    /* 0x0024 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0025 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E48),
    /* 0x0028 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0029 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E49),
    /* 0x002C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E4A),
    /* 0x0030 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0031 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_22_08),
    /* 0x0034 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_24_80),
    /* 0x0037 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0038 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E4B),
    /* 0x003B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003C 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x003D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E42),
    /* 0x0040 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0041 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for the shrine Goron standing aside the Elder's room.
static MsgScript sMsgScriptGoronAsideEldersRoom[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON, 0x0027 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0011 - 0x0008),
    /* 0x0008 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E4D),
    /* 0x000B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E4E),
    /* 0x000F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0010 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0011 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_23_01, 0x0022 - 0x0016),
    /* 0x0016 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E4F),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0E50),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_23_01),
    /* 0x0021 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0022 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E51),
    /* 0x0025 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0026 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0027 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E4C),
    /* 0x002A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002B 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for the Goron in Goron Village who sells Powder Kegs.
static MsgScript sMsgScriptGoronMedigoron[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0015 - 0x0003),
    /* 0x0003 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_18_20, 0x0010 - 0x0008),
    /* 0x0008 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C8E),
    /* 0x000B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000C 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_18_20),
    /* 0x000F 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0010 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C8F),
    /* 0x0013 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0014 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0015 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_18_40, 0x0037 - 0x001A),
    //! @bug Incorrect sfx
    //! The text associated with 0x0C80 makes use of an incorrect pop sound (`NA_SE_EN_SLIME_BREAK`)
    /* 0x001A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C80),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_18_40),
    /* 0x0021 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES, 0x009A - 0x0026),
    /* 0x0026 0x05 */ MSCRIPT_CMD_CHECK_ITEM(ITEM_POWDER_KEG, 0x009A - 0x002B),
    /* 0x002B 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_19_01, 0x0075 - 0x0030),
    /* 0x0030 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0C81),
    /* 0x0033 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0034 0x03 */ MSCRIPT_CMD_JUMP(0x004A - 0x0037),
    /* 0x0037 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES, 0x009A - 0x003C),
    /* 0x003C 0x05 */ MSCRIPT_CMD_CHECK_ITEM(ITEM_POWDER_KEG, 0x009A - 0x0041),
    /* 0x0041 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_19_01, 0x0075 - 0x0046),
    /* 0x0046 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C81),
    /* 0x0049 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0C82),
    /* 0x004D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004E 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0, 0x006F - 0x0055, 0x006F - 0x0055),
    /* 0x0055 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x0056 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C83),
    /* 0x0059 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005A 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x005B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x005E 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0061 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C84),
    /* 0x0064 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0065 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0066 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_19_01),
    /* 0x0069 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x006A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C84),
    /* 0x006D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x006E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x006F 0x01 */ MSCRIPT_CMD_PLAY_CANCEL(),
    /* 0x0070 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C85),
    /* 0x0073 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0074 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0075 0x03 */ MSCRIPT_CMD_CHECK_HAS_POWDER_KEG(0x006A - 0x0078),

    /* 0x0078 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_19_02, 0x008C - 0x007D),
    /* 0x007D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C88),
    /* 0x0080 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0081 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0C89),
    /* 0x0084 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0085 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0096 - 0x008C, 0x006F - 0x008C, 0x006F - 0x008C),

    /* 0x008C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C86),
    /* 0x008F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0090 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES),
    /* 0x0093 0x03 */ MSCRIPT_CMD_JUMP(0x00B7 - 0x0096),
    /* 0x0096 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x0097 0x03 */ MSCRIPT_CMD_JUMP(0x005A - 0x009A),

    /* 0x009A 0x03 */ MSCRIPT_CMD_CHECK_HAS_POWDER_KEG(0x00C9 - 0x009D),
    /* 0x009D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C8C),
    /* 0x00A0 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A1 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0, 0x00AD - 0x00A8, 0x0),
    /* 0x00A8 0x05 */ MSCRIPT_CMD_CHECK_RUPEES(100, 0x00B3 - 0x00AD),
    /* 0x00AD 0x01 */ MSCRIPT_CMD_PLAY_ERROR(),
    /* 0x00AE 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C8D),
    /* 0x00B1 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00B2 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00B3 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x00B4 0x03 */ MSCRIPT_CMD_CHANGE_RUPEES(-100),
    /* 0x00B7 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00B8 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_POWDER_KEG, 0x0),
    /* 0x00BD 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0034),
    /* 0x00C0 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x00C3 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C87),
    /* 0x00C6 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00C7 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x00C8 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00C9 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C8B),
    /* 0x00CC 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00CD 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for the gatekeeper Goron in Goron Village who opens the shrine gate.
static MsgScript sMsgScriptGoronGatekeeper[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE, 0x002E - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_DEKU(0x0016 - 0x0008),
    /* 0x0008 0x03 */ MSCRIPT_CMD_CHECK_ZORA(0x001E - 0x000B),
    /* 0x000B 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0026 - 0x000E),
    /* 0x000E 0x05 */
    MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_HUMAN, 0x0064 - 0x0013),
    /* 0x0013 0x03 */ MSCRIPT_CMD_JUMP(0x0034 - 0x0016),
    /* 0x0016 0x05 */
    MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_DEKU, 0x0064 - 0x001B),
    /* 0x001B 0x03 */ MSCRIPT_CMD_JUMP(0x0034 - 0x001E),
    /* 0x001E 0x05 */
    MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_ZORA, 0x0064 - 0x0023),
    /* 0x0023 0x03 */ MSCRIPT_CMD_JUMP(0x0034 - 0x0026),
    /* 0x0026 0x05 */
    MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_GORON, 0x0064 - 0x002B),
    /* 0x002B 0x03 */ MSCRIPT_CMD_JUMP(0x005D - 0x002E),
    /* 0x002E 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0D4D),
    /* 0x0031 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0032 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0033 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0034 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0D48),
    /* 0x0037 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0038 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0D49),
    /* 0x003B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0D4A),
    /* 0x003F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0040 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0, 0x0056 - 0x0047, 0x0056 - 0x0047),
    /* 0x0047 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x0048 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0D4B),
    /* 0x004B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004C 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x004D 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0050 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0D4D),
    /* 0x0053 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0054 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0055 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0056 0x01 */ MSCRIPT_CMD_PLAY_CANCEL(),
    /* 0x0057 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0D4C),
    /* 0x005A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005B 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x005C 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x005D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0D4E),
    /* 0x0060 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0061 0x03 */ MSCRIPT_CMD_JUMP(0x003C - 0x0064),

    /* 0x0064 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0D4F),
    /* 0x0067 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0068 0x03 */ MSCRIPT_CMD_JUMP(0x0040 - 0x006B),
};

// MsgEvent script for one of the Goron stretchers at the racetrack (Initially stretching side to side while squatting).
static MsgScript sMsgScriptGoronAthleticA[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0009 - 0x0003),
    /* 0x0003 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0DFE),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0009 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0DFF),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for one of the Goron stretchers at the racetrack (Initially doing sidebend stretches with one arm).
static MsgScript sMsgScriptGoronAthleticB[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0009 - 0x0003),
    /* 0x0003 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E00),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0009 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E01),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for one of the Goron stretchers at the racetrack (Initially shaking out their limbs).
static MsgScript sMsgScriptGoronAthleticC[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0009 - 0x0003),
    /* 0x0003 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E02),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0009 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E03),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for one of the Goron stretchers (Initially doing sidebend stretches with both arms).
static MsgScript sMsgScriptGoronAthleticD[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0009 - 0x0003),
    /* 0x0003 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E04),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0009 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E05),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for the pair of Gorons stretching one Goron's hamstrings.
static MsgScript sMsgScriptGoronAthleticHamstring[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0009 - 0x0003),
    /* 0x0003 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E06),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0009 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0E07),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

// MsgEvent script for any sleeping Goron.
static MsgScript sMsgScriptGoronSleeping[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x023A),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0005 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Go_Profile = {
    /**/ ACTOR_EN_GO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OF1D_MAP,
    /**/ sizeof(EnGo),
    /**/ EnGo_Init,
    /**/ EnGo_Destroy,
    /**/ EnGo_Update,
    /**/ NULL,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 0 }, 100 },
};

// Collider for the frozen solid Gorons.
static ColliderCylinderInit sCylinderInitFrozen = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x02, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

// Collider for normal Gorons.
static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* 0x0 */ ENGO_DMGEFF_NONE,
    /* 0x2 */ ENGO_DMGEFF_FIRE = 0x2,
    /* 0xF */ ENGO_DMGEFF_BREAK = 0xF
} EnGoDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, ENGO_DMGEFF_BREAK),
    /* Sword          */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, ENGO_DMGEFF_BREAK),
    /* Fire arrow     */ DMG_ENTRY(1, ENGO_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Light arrow    */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Goron spikes   */ DMG_ENTRY(1, ENGO_DMGEFF_BREAK),
    /* Deku spin      */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Normal shield  */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, ENGO_DMGEFF_NONE),
};

/**
 * Animations used in the actor.
 *
 * @see EnGoAnimation
 */
static AnimationInfoS sAnimationInfo[] = {

    /* Animations for generic Goron actions */
    { &gGoronLyingDownIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronLyingDownIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gGoronUnrollAnim, 2.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gGoronUnrollAnim, 2.0f, 0, -1, ANIMMODE_ONCE, -4 },
    { &gGoronUnrollAnim, -2.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gGoronShiverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronShiverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gGoronDropKegAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },
    { &gGoronCoverEarsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronShiveringSurprisedAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },

    /* Animations for stretching Gorons at the racetrack */
    { &gGoronAthleticsDoubleArmSideBendAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronAthleticsSquatSideToSideAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronAthleticsShakeLimbsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronAthleticsSingleArmSideBendAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronAthleticsHamstringStretchSittingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronAthleticsCheerAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronAthleticsShoutAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
    { &gGoronAthleticsHamstringStretchStandingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },

    /* Animations for the Goron during the spring arrival cutscene */
    { &gGoronSpringShowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gGoronSpringShowLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
    { &gGoronSpringLookAroundAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gGoronSpringLookAroundLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },
};

/**
 * Initialize the latter section of the effects in 'effectTable' to be of type ENGO_EFFECT_STEAM.
 *
 * @param effect Latter section of the EnGoEffect table (&effectTable[ENGO_SNOW_EFFECT_COUNT])
 * @param pos Initial pos
 * @param accel Initial acceleration
 * @param velocity Initial velocity
 * @param scale Initial scale
 * @param deltaScale How much the effect scales per frame (additive)
 * @param maxFrames Maximum number of frames the effect will last. Actual lifetime will be between 'maxFrames / 3' and
 * 'maxFrames'
 */
EnGoEffect* EnGo_InitSteam(EnGoEffect effect[ENGO_OTHER_EFFECT_COUNT], Vec3f pos, Vec3f accel, Vec3f velocity,
                           f32 scale, f32 deltaScale, s32 maxFrames) {
    s32 i;

    // Steam effects are from the end of the snow effects to the end of all effects
    for (i = ENGO_SNOW_EFFECT_COUNT; i < ENGO_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENGO_EFFECT_NONE) {
            continue;
        }

        effect->type = ENGO_EFFECT_STEAM;
        effect->alphaDenom = (Rand_ZeroOne() * (2.0f * (maxFrames / 3.0f))) + (maxFrames / 3.0f);
        effect->alphaNumer = effect->alphaDenom;
        effect->pos = pos;
        effect->accel = accel;
        effect->velocity = velocity;
        effect->scaleXY = scale;
        effect->scaleXYDelta = deltaScale;
        break;
    }
    return effect;
}

/**
 * Draw all effects of type ENGO_EFFECT_STEAM.
 *
 * @param effect The EnGoEffect table
 */
void EnGo_DrawSteam(EnGoEffect effect[ENGO_EFFECT_COUNT], PlayState* play2) {
    PlayState* play = play2;
    s32 i;
    s32 isMaterialSet = false;
    f32 alpha;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    for (i = 0; i < ENGO_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENGO_EFFECT_STEAM) {
            continue;
        }

        gDPPipeSync(POLY_XLU_DISP++);
        if (!isMaterialSet) {
            gSPDisplayList(POLY_XLU_DISP++, gGoronSteamMaterialDL);
            isMaterialSet = true;
        }

        FrameInterpolation_RecordOpenChild(effect, effect->type);
        Matrix_Push();

        alpha = (f32)effect->alphaNumer / effect->alphaDenom;
        alpha *= 255.0f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (u8)alpha);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (effect->alphaNumer + (i * 3)) * 3,
                                      (effect->alphaNumer + (i * 3)) * 15, 0x20, 0x40, 1, 0, 0, 0x20, 0x20, 3, 15, 0,
                                      0));

        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(effect->scaleXY, effect->scaleXY, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gGoronSteamModelDL);

        Matrix_Pop();
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Initialize the first available effect to be of type 'ENGO_EFFECT_DUST1'/'2'/'3'.
 *
 * @param effect Latter section of the EnGoEffect table (&effectTable[ENGO_SNOW_EFFECT_COUNT])
 * @param pos Initial pos
 * @param accel Initial acceleration
 * @param velocity Initial velocity
 * @param scale Initial scale
 * @param deltaScale How much the effect scales per frame (additive)
 * @param maxFrames Maximum number of frames the effect will last. Actual lifetime will be 1/3 * maxFrames -> maxFrames
 * @param parentEffectType Type of the parent effect, determines which of the possible dust effects correspond
 */
void EnGo_InitDust(EnGoEffect effect[ENGO_OTHER_EFFECT_COUNT], Vec3f pos, Vec3f accel, Vec3f velocity, f32 scale,
                   f32 deltaScale, s32 maxFrames, EnGoEffectType parentEffectType) {
    s32 i;

    // Dust effects are from the end of the snow effects to the end of all effects.
    for (i = ENGO_SNOW_EFFECT_COUNT; i < ENGO_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENGO_EFFECT_NONE) {
            continue;
        }

        effect->type = parentEffectType + ENGO_EFFECT_DUST_MIN;
        effect->alphaDenom = (Rand_ZeroOne() * (2.0f * (maxFrames / 3.0f))) + (maxFrames / 3.0f);
        effect->alphaNumer = effect->alphaDenom;
        effect->pos = pos;
        effect->accel = accel;
        effect->velocity = velocity;
        effect->scaleXY = scale;
        effect->scaleXYDelta = deltaScale;
        break;
    }
}

/**
 * Draw all effects of type 'ENGO_EFFECT_DUST1'/'2'/'3'.
 *
 * @param effect The EnGoEffect table
 */
void EnGo_DrawDust(EnGoEffect effect[ENGO_EFFECT_COUNT], PlayState* play2) {
    static TexturePtr sDustTextures[] = {
        gEffDust8Tex, gEffDust7Tex, gEffDust6Tex, gEffDust5Tex, gEffDust4Tex, gEffDust3Tex, gEffDust2Tex, gEffDust1Tex,
    };
    static Color_RGBA8 sEnGoDustPrimColor[] = {
        { 255, 255, 255, 0 }, // White
        { 170, 130, 90, 0 },  // Light Brown
        { 0, 0, 0, 0 },       // Black
    };
    static Color_RGBA8 sEnGoDustEnvColor[] = {
        { 255, 255, 255, 0 }, // White
        { 100, 60, 20, 0 },   // Dark Brown
        { 0, 0, 0, 0 },       // Black
    };
    PlayState* play = play2;
    s32 i;
    u8 isMaterialSet = false;
    f32 alpha;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    for (i = 0; i < ENGO_EFFECT_COUNT; i++, effect++) {
        if ((effect->type < ENGO_EFFECT_DUST_MIN) || (effect->type >= ENGO_EFFECT_STEAM_MIN)) {
            continue;
        }

        if (!isMaterialSet) {
            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_0);
            gSPDisplayList(POLY_XLU_DISP++, gGoronDustMaterialDL);
            isMaterialSet = true;
        }

        FrameInterpolation_RecordOpenChild(effect, effect->type);
        Matrix_Push();

        alpha = (f32)effect->alphaNumer / effect->alphaDenom;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, sEnGoDustPrimColor[(s32)effect->type - ENGO_EFFECT_DUST_MIN].r,
                        sEnGoDustPrimColor[(s32)effect->type - ENGO_EFFECT_DUST_MIN].g,
                        sEnGoDustPrimColor[(s32)effect->type - ENGO_EFFECT_DUST_MIN].b, (u8)(alpha * UINT8_MAX));
        gDPSetEnvColor(POLY_XLU_DISP++, sEnGoDustEnvColor[(s32)effect->type - ENGO_EFFECT_DUST_MIN].r,
                       sEnGoDustEnvColor[(s32)effect->type - ENGO_EFFECT_DUST_MIN].g,
                       sEnGoDustEnvColor[(s32)effect->type - ENGO_EFFECT_DUST_MIN].b, 0);

        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_Scale(effect->scaleXY, effect->scaleXY, 1.0f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sDustTextures[(s32)(alpha * 7.0f)]));
        gSPDisplayList(POLY_XLU_DISP++, gGoronDustModelDL);

        Matrix_Pop();
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Initialize the snowball break effects.
 *
 * Half of the elements will be 'ENGO_EFFECT_SNOW1'/'2'/'3' randomly generated around the character. The second half
 * will be dedicated 'ENGO_EFFECT_DUST1'/'2'/'3' paired with the snow.
 *
 * @param effect The EnGoEffect table
 * @param pos Position around which the effects appear
 */
void EnGo_InitSnow(EnGoEffect effect[ENGO_SNOW_EFFECT_COUNT], Vec3f pos) {
    static u8 sEffectIndexToSnowEffectTable[ENGO_SNOW_EFFECT_COUNT] = {
        ENGO_EFFECT_SNOW3, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW2,
        ENGO_EFFECT_SNOW3, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW2,
        ENGO_EFFECT_SNOW3, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW2,
        ENGO_EFFECT_SNOW3, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW1, ENGO_EFFECT_SNOW2,
    };
    EnGoEffect* dustEffects = &effect[ENGO_SNOW_EFFECT_COUNT];
    s32 i;
    Vec3f randRelativeToWorldPos;
    Vec3f randYOneToFour;
    f32 velMagnitude;

    // Snow effects consume the first part of the effects table, and are paired with dust effects in the latter part
    for (i = 0; i < ENGO_SNOW_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ENGO_EFFECT_NONE) {
            continue;
        }

        effect->pos = pos;
        effect->pos.y += 56.0f;

        // Generate a +-15 degree rotational velocity
        effect->angularVelocity.x = (Rand_ZeroOne() - 0.5f) * 0x1554;
        effect->angularVelocity.y = (Rand_ZeroOne() - 0.5f) * 0x1554;
        effect->angularVelocity.z = (Rand_ZeroOne() - 0.5f) * 0x1554;

        // Generate a radially outward velocity for each of the effects
        velMagnitude = (Rand_ZeroOne() * 4.0f) + 6.0f;
        effect->velocity.x = Math_SinS(i * (0x10000 / ENGO_SNOW_EFFECT_COUNT)) * velMagnitude;
        effect->velocity.z = Math_CosS(i * (0x10000 / ENGO_SNOW_EFFECT_COUNT)) * velMagnitude;
        effect->velocity.y = (Rand_ZeroOne() * 3.0f) + 6.0f;

        // No acceleration on the X,Z axis, negative acceleration on the Y axis
        effect->accel = gZeroVec3f;
        effect->accel.y = -0.8f;

        // Full visibility (1/1)
        effect->alphaDenom = effect->alphaNumer = 1;

        // Assign a snow effect value of 'ENGO_EFFECT_SNOW1'/'2'/'3'
        effect->type = sEffectIndexToSnowEffectTable[i];

        // Initialize the parameters for the paired element
        randRelativeToWorldPos.x = ((Rand_ZeroOne() - 0.5f) * 80.0f) + effect->pos.x;
        randRelativeToWorldPos.y = ((Rand_ZeroOne() - 0.5f) * 40.0f) + effect->pos.y;
        randRelativeToWorldPos.z = ((Rand_ZeroOne() - 0.5f) * 80.0f) + effect->pos.z;

        randYOneToFour = gZeroVec3f;
        randYOneToFour.y = (Rand_ZeroOne() * 3.0f) + 1.0f;

        // Initialize the paired element.
        EnGo_InitDust(dustEffects, randRelativeToWorldPos, gZeroVec3f, randYOneToFour, 0.6f, 0.2f,
                      ENGO_DUST_STEAM_LIFETIME, 0);
    }
}

/**
 * Update the snow effect's movement.
 *
 * When the snow drops to the ground, it is converted to dust.
 *
 * @param effect Pointer to the effect to update
 * @param dustConversionHeight  Height above the ground at which to convert the effect to dust
 */
void EnGo_UpdateSnow(EnGoEffect* effect, f32 dustConversionHeight) {
    f32 x;
    f32 z;

    effect->pos.x += effect->velocity.x;
    effect->pos.y += effect->velocity.y;
    effect->pos.z += effect->velocity.z;

    effect->velocity.y += effect->accel.y;
    effect->scaleXY += effect->scaleXYDelta;

    if (effect->pos.y < dustConversionHeight) {
        effect->pos.y = dustConversionHeight;

        effect->type = ENGO_EFFECT_DUST1;
        effect->alphaDenom = (Rand_ZeroOne() * 8.0f) + 4.0f;
        effect->alphaNumer = effect->alphaDenom;

        effect->velocity = gZeroVec3f;
        effect->velocity.y = (Rand_ZeroOne() * 3.0f) + 1.0f;

        effect->scaleXY = 0.4f;
        effect->scaleXYDelta = 0.1f;
        return;
    }

    if (effect->velocity.x != 0.0f) {
        x = effect->velocity.x / fabsf(effect->velocity.x);
        x *= (sREG(13) + 140) * 0.01f;
        Math_StepToF(&effect->velocity.x, x, (sREG(14) + 40) * 0.01f);
    }

    if (effect->velocity.z != 0.0f) {
        z = effect->velocity.z / fabsf(effect->velocity.z);
        z *= ((sREG(13) + 140) * 0.01f);
        Math_StepToF(&effect->velocity.z, z, (sREG(14) + 40) * 0.01f);
    }

    effect->rot.x += effect->angularVelocity.x;
    effect->rot.y += effect->angularVelocity.y;
    effect->rot.z += effect->angularVelocity.z;
}

/**
 * Draw all effects of type 'ENGO_EFFECT_SNOW1'/'2'/'3'.
 *
 * @param effect The EnGoEffect table
 * @param material Snow Material
 * @param model Snow Model
 * @param type Snow Effect type to draw
 */
void EnGo_DrawSnow(EnGoEffect effect[ENGO_SNOW_EFFECT_COUNT], PlayState* play, Gfx* material, Gfx* model,
                   u8 effectType) {
    s32 i;
    u8 isMaterialSet = false;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < ENGO_SNOW_EFFECT_COUNT; i++, effect++) {
        if (effect->type != effectType) {
            continue;
        }

        if (!isMaterialSet) {
            gSPDisplayList(POLY_OPA_DISP++, material);
            isMaterialSet = true;
        }

        FrameInterpolation_RecordOpenChild(effect, effect->type);
        Matrix_Push();
        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_Scale(0.08f, 0.08f, 0.08f, MTXMODE_APPLY);
        Matrix_RotateZS(effect->rot.z, MTXMODE_APPLY);
        Matrix_RotateXS(effect->rot.x, MTXMODE_APPLY);
        Matrix_RotateYS(effect->rot.y, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, model);

        Matrix_Pop();
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Update all effects, potentially fading them away to nothing.
 */
void EnGo_UpdateEffects(EnGo* this) {
    EnGoEffect* effect = this->effectTable;
    s32 i;

    for (i = 0; i < ENGO_EFFECT_COUNT; i++, effect++) {
        if (effect->type == ENGO_EFFECT_NONE) {
            continue;
        }

        if (effect->alphaNumer == 0) {
            // End usage of the effect
            effect->type = ENGO_EFFECT_NONE;
        } else if ((effect->type > ENGO_EFFECT_NONE) && (effect->type < ENGO_EFFECT_DUST_MIN)) {
            // Snow rotates, and changes to dust upon dropping below the actor position
            EnGo_UpdateSnow(effect, this->actor.world.pos.y);
        } else {
            // Dust and steam move according to their initial values
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            effect->scaleXY += effect->scaleXYDelta;
            effect->alphaNumer--;
        }
    }
}

/**
 * Draw all effects.
 */
void EnGo_DrawEffects(EnGo* this, PlayState* play) {
    EnGo_DrawSnow(this->effectTable, play, gGoronLargeSnowballFragmentMaterialDL, gGoronLargeSnowballFragmentModelDL,
                  ENGO_EFFECT_SNOW1);
    EnGo_DrawSnow(this->effectTable, play, gGoronMediumSnowballFragmentMaterialDL, gGoronMediumSnowballFragmentModelDL,
                  ENGO_EFFECT_SNOW2);
    EnGo_DrawSnow(this->effectTable, play, gGoronSmallSnowballFragmentMaterialDL, gGoronSmallSnowballFragmentModelDL,
                  ENGO_EFFECT_SNOW3);
    EnGo_DrawSteam(this->effectTable, play);
    EnGo_DrawDust(this->effectTable, play);
}

/**
 * Check if the Goron can hurt the player.
 */
s32 EnGo_CanSnowballHurtPlayer(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->transformation == PLAYER_FORM_GORON) && (player->stateFlags3 & PLAYER_STATE3_2000000)) {
        return false;
    } else {
        return true;
    }
}

/**
 * Check if the Goron is falling asleep.
 */
s32 EnGo_IsFallingAsleep(EnGo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 isFallingAsleep = false;

    if (((player->transformation == PLAYER_FORM_GORON) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
         (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY) && (this->sleepState == ENGO_AWAKE) &&
         (this->actor.xzDistToPlayer < 400.0f)) ||
        (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON) && (play->sceneId == SCENE_16GORON_HOUSE) &&
         (gSaveContext.sceneLayer == 0) && (this->sleepState == ENGO_AWAKE) && (play->csCtx.scriptIndex == 1))) {
        isFallingAsleep = true;
    }
    return isFallingAsleep;
}

/**
 * Update the actor's focus.
 */
s32 EnGo_UpdateFocus(EnGo* this) {
    static Vec3f sMedigoronPos = { 0.0f, 100.0f, 160.0f };
    s32 pad;
    f32 yAxisOffsetToFocusPoint = 58.0f;

    if (ENGO_GET_TYPE(&this->actor) == ENGO_MEDIGORON) {
        Lib_Vec3f_TranslateAndRotateY(&this->actor.world.pos, this->actor.shape.rot.y, &sMedigoronPos,
                                      &this->actor.focus.pos);
    } else {
        if ((this->actionFlags & ENGO_FLAG_ROLLED_UP) || (this->actionFlags & ENGO_FLAG_SNOWBALLED)) {
            yAxisOffsetToFocusPoint = this->actor.shape.yOffset;
        }
        Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);
        this->actor.focus.pos.y += yAxisOffsetToFocusPoint;
    }

    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
    return false;
}

/**
 * Update the collider for the snowball form of the Goron Shrine gatekeeper.
 */
void EnGo_UpdateSnowballCollider(EnGo* this, PlayState* play) {
    Vec3f worldPos;

    Math_Vec3f_Copy(&worldPos, &this->actor.world.pos);

    this->colliderSphere.dim.worldSphere.center.x = worldPos.x;
    this->colliderSphere.dim.worldSphere.center.y = worldPos.y;
    this->colliderSphere.dim.worldSphere.center.y += TRUNCF_BINANG(this->actor.shape.yOffset);
    this->colliderSphere.dim.worldSphere.center.z = worldPos.z;

    this->colliderSphere.dim.modelSphere.radius = 48;
    this->colliderSphere.dim.worldSphere.radius =
        this->colliderSphere.dim.modelSphere.radius * this->colliderSphere.dim.scale;

    if (EnGo_CanSnowballHurtPlayer(play)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderSphere.base);
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSphere.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
}

/**
 * Update the collider for the Medigoron in the Powder Keg Shop.
 */
void EnGo_UpdateMedigoronCollider(EnGo* this, PlayState* play) {
    this->colliderSphere.dim.worldSphere.radius =
        this->colliderSphere.dim.modelSphere.radius * this->colliderSphere.dim.scale;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
}

/**
 * Update the collider for the rolled up form of a Goron.
 */
void EnGo_UpdateRolledUpCollider(EnGo* this, PlayState* play) {
    Vec3f worldPos;

    Math_Vec3f_Copy(&worldPos, &this->actor.world.pos);
    this->colliderSphere.dim.worldSphere.center.x = worldPos.x;
    this->colliderSphere.dim.worldSphere.center.y = worldPos.y;
    this->colliderSphere.dim.worldSphere.center.y += TRUNCF_BINANG(this->actor.shape.yOffset);
    this->colliderSphere.dim.worldSphere.center.z = worldPos.z;
    this->colliderSphere.dim.modelSphere.radius = 20;
    this->colliderSphere.dim.worldSphere.radius =
        this->colliderSphere.dim.modelSphere.radius * this->colliderSphere.dim.scale;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
}

/**
 * Update the collider for the frozen form of a Goron.
 */
void EnGo_UpdateFrozenCollider(EnGo* this, PlayState* play) {
    s32 pad;

    Math_Vec3f_ToVec3s(&this->colliderCylinder.dim.pos, &this->actor.world.pos);
    this->colliderCylinder.dim.radius = 46;
    this->colliderCylinder.dim.height = 78;
    if (this->harmlessTimer == 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderCylinder.base);
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
}

/**
 * Update the collider for all standing forms of a Goron.
 */
void EnGo_UpdateStandingCollider(EnGo* this, PlayState* play) {
    Math_Vec3f_ToVec3s(&this->colliderCylinder.dim.pos, &this->actor.world.pos);
    this->colliderCylinder.dim.radius = 24;
    this->colliderCylinder.dim.height = 62;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
}

/**
 * Update the collider for the Goron.
 *
 * Both the rolling snowball and frozen forms can hurt Player on collision, others do not.
 */
void EnGo_UpdateCollider(EnGo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags2 & PLAYER_STATE2_4000)) {
        DECR(this->harmlessTimer);
    }
    if (ENGO_GET_TYPE(&this->actor) == ENGO_MEDIGORON) {
        EnGo_UpdateMedigoronCollider(this, play);
    } else if (this->actionFlags & ENGO_FLAG_SNOWBALLED) {
        EnGo_UpdateSnowballCollider(this, play);
    } else if (this->actionFlags & ENGO_FLAG_ROLLED_UP) {
        EnGo_UpdateRolledUpCollider(this, play);
    } else if (this->actionFlags & ENGO_FLAG_FROZEN) {
        EnGo_UpdateFrozenCollider(this, play);
    } else {
        EnGo_UpdateStandingCollider(this, play);
    }
}

/**
 * Check if the actor is talking, and if so set the action function to handle it.
 *
 * @return True if talking
 */
s32 EnGo_UpdateTalking(EnGo* this, PlayState* play) {
    if (((this->actionFlags & SUBS_OFFER_MODE_MASK) == SUBS_OFFER_MODE_NONE) ||
        !Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        return false;
    }

    if ((ENGO_GET_TYPE(&this->actor) != ENGO_MEDIGORON) && (ENGO_GET_TYPE(&this->actor) != ENGO_ATHLETIC)) {
        if (!(this->actionFlags & ENGO_FLAG_ROLLED_UP)) {
            this->actionFlags |= ENGO_FLAG_ENGAGED;
        }
    }

    if ((ENGO_GET_TYPE(&this->actor) == ENGO_ASIDE_STORE) || (ENGO_GET_TYPE(&this->actor) == ENGO_ASIDE_ELDERSROOM) ||
        (ENGO_GET_TYPE(&this->actor) == ENGO_ASIDE_ELDERSSON)) {
        this->blinkTimer = 0;
        this->eyeTexIndex = ENGO_EYETEX_OPEN;
        this->actionFlags |= ENGO_FLAG_EYES_OPEN;
    }

    SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
    this->cutsceneState = 0;
    this->gatekeeperAnimState = 0;
    this->interruptedActionFunc = this->actionFunc;
    this->actionFunc = EnGo_Talk;
    return true;
}

/**
 * Detect any collisions, and convert detections to object specific 'actionFlags'.
 */
s32 EnGo_DetectCollisions(EnGo* this, PlayState* play) {
    this->actionFlags &= ~ENGO_FLAG_HIT_OTHER;
    this->actionFlags &= ~ENGO_FLAG_HIT_BY_OTHER;
    this->actionFlags &= ~ENGO_FLAG_HIT_OBJ;

    if ((this->colliderCylinder.base.atFlags & AT_HIT) || (this->colliderSphere.base.atFlags & AT_HIT)) {
        this->colliderCylinder.base.atFlags &= ~AT_HIT;
        this->colliderSphere.base.atFlags &= ~AT_HIT;
        this->actionFlags |= ENGO_FLAG_HIT_OTHER;
        this->harmlessTimer = 40;
    }

    if ((this->colliderCylinder.base.acFlags & AC_HIT) || (this->colliderSphere.base.acFlags & AC_HIT)) {
        this->colliderCylinder.base.acFlags &= ~AC_HIT;
        this->colliderSphere.base.acFlags &= ~AC_HIT;
        this->actionFlags |= ENGO_FLAG_HIT_BY_OTHER;
    }

    if ((this->colliderCylinder.base.ocFlags1 & OC1_HIT) || (this->colliderSphere.base.ocFlags1 & OC1_HIT)) {
        this->colliderCylinder.base.ocFlags1 &= ~OC1_HIT;
        this->colliderSphere.base.ocFlags1 &= ~OC1_HIT;
        this->actionFlags |= ENGO_FLAG_HIT_OBJ;
    }

    return false;
}

/**
 * Start, or finish, the cutscene for the arrival of springtime.
 */
s32 EnGo_UpdateSpringArrivalCutscene(EnGo* this, PlayState* play) {
    if ((ENGO_GET_TYPE(&this->actor) == ENGO_GRAVEYARD) && (play->csCtx.state != CS_STATE_IDLE) &&
        (this->actor.draw != NULL) && (play->sceneId == SCENE_10YUKIYAMANOMURA2) && (gSaveContext.sceneLayer == 1) &&
        (play->csCtx.scriptIndex == 0)) {
        if (!this->springArrivalCutsceneActive) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->springArrivalCueId = 255;
            this->springArrivalCutsceneActive = true;
            this->interruptedActionFunc = this->actionFunc;
        }
        SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->actionFunc = EnGo_HandleSpringArrivalCutscene;
    } else if (this->springArrivalCutsceneActive) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->springArrivalCueId = 255;
        this->springArrivalCutsceneActive = false;
        SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->actionFunc = this->interruptedActionFunc;
    }

    return false;
}

/**
 * Update the animation to the actor's currently set value.
 *
 * @return True if non-repeating animation has finished
 */
s32 EnGo_UpdateSkelAnime(EnGo* this, PlayState* play) {
    s8 objectSlot = this->actor.objectSlot;
    s8 extraObjectSlot = OBJECT_SLOT_NONE;
    s32 isAnimFinished = false;

    if ((this->animIndex >= ENGO_ANIM_SPRING_MIN) && (this->hakuginDemoObjectSlot > OBJECT_SLOT_NONE)) {
        extraObjectSlot = this->hakuginDemoObjectSlot;
    } else if ((this->animIndex >= ENGO_ANIM_ATHLETICS_MIN) && (this->taisouObjectSlot > OBJECT_SLOT_NONE)) {
        extraObjectSlot = this->taisouObjectSlot;
    } else if (this->animIndex < ENGO_ANIM_ATHLETICS_MIN) {
        extraObjectSlot = this->actor.objectSlot;
    }

    if (extraObjectSlot > OBJECT_SLOT_NONE) {
        gSegments[0x06] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[extraObjectSlot].segment);
        this->skelAnime.playSpeed = this->animPlaySpeed;
        isAnimFinished = SkelAnime_Update(&this->skelAnime);
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
    }

    return isAnimFinished;
}

/**
 * Update sound effects based on animation.
 */
s32 EnGo_UpdateSfx(EnGo* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        if (this->animIndex == ENGO_ANIM_ROLL) {
            if (Animation_OnFrame(&this->skelAnime, 2.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_CIRCLE);
            }

            if (Animation_OnFrame(&this->skelAnime, 22.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_SIT_IMT);
            }
        } else if ((this->animIndex == ENGO_ANIM_UNROLL) || (this->animIndex == ENGO_ANIM_UNROLL_IMM)) {
            if (Animation_OnFrame(&this->skelAnime, 2.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_CIRCLE_OFF);
            }

            if (Animation_OnFrame(&this->skelAnime, 24.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_STAND_IMT);
            }
        }
    }
    return 0;
}

/**
 * Change the Goron's animation.
 *
 * Goron animations come from one of three categories:
 * - Basic Goron animations
 * - Goron athletics like stretches, cheers, etc...
 * - Spring arrival cutscene animations
 * Changing animations with this function handles the transitions between the three categories.
 *
 * @return True if animation was changed
 */
s32 EnGo_ChangeAnim(EnGo* this, PlayState* play, EnGoAnimation animIndex) {
    s8 objectSlot = this->actor.objectSlot;
    s8 extraObjectSlot = OBJECT_SLOT_NONE;
    s32 didAnimChange = false;

    if ((animIndex >= ENGO_ANIM_SPRING_MIN) && (this->hakuginDemoObjectSlot > OBJECT_SLOT_NONE)) {
        extraObjectSlot = this->hakuginDemoObjectSlot;
    } else if ((animIndex >= ENGO_ANIM_ATHLETICS_MIN) && (this->taisouObjectSlot > OBJECT_SLOT_NONE)) {
        extraObjectSlot = this->taisouObjectSlot;
    } else if (animIndex < ENGO_ANIM_ATHLETICS_MIN) {
        extraObjectSlot = this->actor.objectSlot;
    }

    if (extraObjectSlot > OBJECT_SLOT_NONE) {
        gSegments[0x06] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[extraObjectSlot].segment);
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        this->animPlaySpeed = this->skelAnime.playSpeed;
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
    }

    return didAnimChange;
}

/**
 * Update the eye textures to blink.
 */
void EnGo_UpdateEyes(EnGo* this) {
    if (this->actionFlags & ENGO_FLAG_EYES_OPEN) {
        if (DECR(this->blinkTimer) == 0) {
            this->eyeTexIndex++;
            if (this->eyeTexIndex >= ENGO_EYETEX_CLOSED2) {
                this->blinkTimer = Rand_S16Offset(30, 30);
                this->eyeTexIndex = ENGO_EYETEX_OPEN;
            }
        }
    }
}

/**
 * Play the surprise animation then return to shivering.
 */
void EnGo_UpdateShiverSurprisedAnimation(EnGo* this, PlayState* play) {
    if (this->surprisePhase == 0) {
        EnGo_ChangeAnim(this, play, ENGO_ANIM_SHIVERINGSURPRISED);
        this->surprisePhase++;
    } else if ((this->surprisePhase == 1) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnGo_ChangeAnim(this, play, ENGO_ANIM_SHIVER_IMM);
        this->surprisePhase++;
    }
}

/**
 * Change the attention target, and dialog reactions of the Gorons outside Darmani's grave.
 */
s32 EnGo_UpdateGraveyardAttentionTargetAndReactions(EnGo* this, PlayState* play) {
    u16 textId = play->msgCtx.currentTextId;
    Player* player = GET_PLAYER(play);

    if (ENGO_GET_TYPE(&this->actor) != ENGO_GRAVEYARD) {
        return false;
    }

    if (player->stateFlags1 & PLAYER_STATE1_TALKING) {
        if (this->lastTextId != textId) {
            switch (textId) {
                case 0xE1A: // Awakening from frozen form, confused, turn to other Goron
                    this->actionFlags |= ENGO_FLAG_ENGAGED;
                    this->attentionTarget = this->actor.child;
                    break;

                case 0xE1D: // Turn to player (Goron form) while stating Darmani is dead
                case 0xE25: // Turn to player (Other form) while thanking player
                    if (ENGO_GET_SUBTYPE(&this->actor) == ENGO_GRAVEYARD_FROZEN) {
                        this->attentionTarget = &GET_PLAYER(play)->actor;
                    }
                    break;

                case 0xE27: // (spoken) "What's this?..."
                    if (ENGO_GET_SUBTYPE(&this->actor) == ENGO_GRAVEYARD_FROZEN) {
                        this->attentionTarget = this->actor.child;
                    }

                case 0xE16: // Surprised, questioning if player is Darmani
                case 0xE1E: // Surprised, seeing Darmani
                    this->graveyardDialogActionFunc = EnGo_UpdateShiverSurprisedAnimation;
                    break;

                case 0xE1F: // Turn to player stating Darmani is not dead
                    if (ENGO_GET_SUBTYPE(&this->actor) == ENGO_GRAVEYARD_GRAVEMAKER) {
                        this->attentionTarget = &GET_PLAYER(play)->actor;
                    }
                    break;

                default:
                    break;
            }
        }
        this->changedText = true;
        this->lastTextId = textId;
    } else if (this->changedText) {
        // If the player isn't talking, and the text has changed, its the last text.
        // Set everything back to the "Idle" behavior.
        this->changedText = false;
        this->graveyardDialogActionFunc = NULL;
        this->lastTextId = 0;
        EnGo_ChangeAnim(this, play, ENGO_ANIM_SHIVER);
        this->actionFlags &= ~ENGO_FLAG_ENGAGED;
    }

    if (this->graveyardDialogActionFunc != NULL) {
        this->graveyardDialogActionFunc(this, play);
    }

    return false;
}

/**
 * Update the actor's rotation to point towards the target.
 */
s32 EnGo_UpdateRotationToTarget(EnGo* this, PlayState* play) {
    s32 pad;
    Vec3f targetPos;
    Vec3f thisPos;
    s16 desiredYaw;

    Math_Vec3f_Copy(&targetPos, &this->attentionTarget->world.pos);
    Math_Vec3f_Copy(&thisPos, &this->actor.world.pos);
    desiredYaw = Math_Vec3f_Yaw(&thisPos, &targetPos);

    Math_ApproachS(&this->headRotY, (desiredYaw - this->bodyRotY) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->headRotY = CLAMP(this->headRotY, -0x1FFE, 0x1FFE);

    Math_ApproachS(&this->bodyRotY, (desiredYaw - this->headRotY) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->bodyRotY = CLAMP(this->bodyRotY, -0x1C70, 0x1C70);

    Math_Vec3f_Copy(&thisPos, &this->actor.focus.pos);
    if (this->attentionTarget->id == ACTOR_PLAYER) {
        targetPos.y = ((Player*)this->attentionTarget)->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&targetPos, &this->attentionTarget->focus.pos);
    }

    Math_ApproachS(&this->headRotZ, Math_Vec3f_Pitch(&thisPos, &targetPos) - this->bodyRotZ, 4, 0x2AA8);
    this->headRotZ = CLAMP(this->headRotZ, -0x1554, 0x1554);

    Math_ApproachS(&this->bodyRotZ, Math_Vec3f_Pitch(&thisPos, &targetPos) - this->headRotZ, 4, 0x2AA8);
    this->bodyRotZ = CLAMP(this->bodyRotZ, -0xE38, 0xE38);

    return false;
}

/**
 * Handle updating of Goron's attention target, and reactions during dialog.
 */
s32 EnGo_UpdateAttentionTargetAndReactions(EnGo* this, PlayState* play) {
    if (!this->changedText) {
        this->attentionTarget = &GET_PLAYER(play)->actor;
    }

    EnGo_UpdateGraveyardAttentionTargetAndReactions(this, play);

    if (this->actionFlags & ENGO_FLAG_ENGAGED) {
        this->actionFlags &= ~ENGO_FLAG_LOST_ATTENTION;
        this->actionFlags |= ENGO_FLAG_FACE_TARGET;
        EnGo_UpdateRotationToTarget(this, play);
    } else if (this->actionFlags & ENGO_FLAG_FACE_TARGET) {
        this->actionFlags &= ~ENGO_FLAG_FACE_TARGET;
        this->headRotZ = 0;
        this->headRotY = 0;
        this->bodyRotZ = 0;
        this->bodyRotY = 0;
        this->loseAttentionTimer = 20;
    } else if (DECR(this->loseAttentionTimer) == 0) {
        this->actionFlags |= ENGO_FLAG_LOST_ATTENTION;
        this->loseAttentionTimer = 20;
    }

    return true;
}

/**
 * Idle action function for the gravemaker at the Goron Graveyard.
 */
void EnGo_GravemakerIdle(EnGo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 deltaYaw = BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y);

    if ((fabsf(this->actor.playerHeightRel) > 20.0f) || (this->actor.xzDistToPlayer > 300.0f)) {
        SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    } else if ((player->transformation != PLAYER_FORM_GORON) || (ABS_ALT(deltaYaw) >= 0x1C70) ||
               CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_GRAVEMAKER_AS_GORON) ||
               CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON)) {
        SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    } else {
        SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_AUTO, SUBS_OFFER_MODE_MASK);
    }
}

/**
 * Idle action function for the Goron frozen at the Goron Graveyard
 */
void EnGo_FrozenIdle(EnGo* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON)) {
        SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    } else {
        SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_AUTO, SUBS_OFFER_MODE_MASK);
    }
}

/**
 * Obtain a pointer to the gravemaker at the Goron Graveyard.
 */
Actor* EnGo_FindGravemaker(EnGo* this, PlayState* play) {
    Actor* actor;
    Actor* retActor = NULL;

    while (true) {
        actor = SubS_FindActor(play, retActor, ACTORCAT_NPC, ACTOR_EN_GO);
        retActor = actor;

        if ((actor != NULL) && ((EnGo*)actor != this) && (ENGO_GET_TYPE(actor) == ENGO_GRAVEYARD) &&
            (ENGO_GET_SUBTYPE(actor) == ENGO_GRAVEYARD_GRAVEMAKER)) {
            return retActor;
        }

        if (actor == NULL) {
            break;
        }

        actor = actor->next;
        if (actor == NULL) {
            break;
        }
        retActor = actor;
    }

    return NULL;
}

/**
 * The Medigoron's collider radius is greater when it needs to be able to drop the Powder Keg in front of the player.
 */
void EnGo_UpdateMedigoronColliderRadius(EnGo* this, PlayState* play, s32 isGivenPowderKeg) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES) ||
        (play->actorCtx.flags & ACTORCTX_FLAG_0) // Same check occurs in Powder Keg ammo check MsgScript Command
        || isGivenPowderKeg) {
        this->colliderSphere.dim.modelSphere.radius = 300;
    } else {
        this->colliderSphere.dim.modelSphere.radius = 380;
    }
}

/**
 * Helper to change a cutscene.
 *
 * @returns Status of cutscene change
 * @retval true Cutscene has started
 * @retval false Cutscene is queued to start
 */
s32 EnGo_ChangeCutscene(EnGo* this, s16 csId) {
    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
    } else if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_StartWithPlayerCs(csId, &this->actor);
        return true;
    }
    CutsceneManager_Queue(csId);
    return false;
}

/**
 * Handle the animation of the "Goron Pound".
 *
 * There are Several Phases to the pound:
 * - Phase 1: (Once in Rolled-Up form) the pound starts by leaping into the air.
 *   - The Goron rotates until its back faces the ground at its peak.
 *   - The Goron distorts shape until at its peak.
 *   - Transition: At the peak, all movement is halted.
 * - Phase 2: While hanging, the Goron reverts back to normal shape.
 *   - Transition: At reaching maxHangTime, ENGO_POUND_INITIAL_FALL_VEL is applied.
 * - Phase 3: The Goron falls to the ground.
 *   - Transition: At reaching the ground.
 *
 * @param initialVelocity Initial +Y velocity of the pound.
 * @param maxDistortion Maximum distortion of the Goron's shape.
 * @param maxHangtime Maximum hangtime at the peak of the pound.
 * @returns True when pound finishes.
 */
s32 EnGo_HandleGatekeeperPoundCutscene(EnGo* this, f32 initialVelocity, f32 maxDistortion, s32 maxHangtime) {
    s32 pad;
    f32 rotAndDistortFactor = 1.0f;
    f32 velocity = this->actor.velocity.y + this->actor.gravity;
    s32 isFinished;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        // Transition 3->DONE
        isFinished = true;
    } else {
        if (velocity > 0.0f) {
            // PHASE 1: Goron is rising
            rotAndDistortFactor = velocity / initialVelocity;
            this->actor.shape.rot.x += TRUNCF_BINANG(0x238C * rotAndDistortFactor);
            this->elapsedHangtime = 0;
        } else if ((s32)this->actor.velocity.y == 0) {
            if (maxHangtime >= this->elapsedHangtime) {
                // PHASE 2: Goron is hanging in the air
                rotAndDistortFactor = (f32)this->elapsedHangtime / maxHangtime;
            } else {
                // Transition 2->3: Hanging is complete
                this->actor.gravity = ENGO_POUND_INITIAL_FALL_VEL;
            }
            this->elapsedHangtime++;
        } else if (this->elapsedHangtime == 0) {
            // Transition 1->2: Goron has reached the peak
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = 0.0f;
            rotAndDistortFactor = velocity / initialVelocity;
        } else {
            // PHASE 3: Goron is falling (Just letting gravity take effect)
        }
        isFinished = false;
    }

    // Handle shape distortions
    this->scaleFactorPoundDistortion = (1.0f - rotAndDistortFactor) * maxDistortion;
    this->actor.scale.x = this->scaleFactor - this->scaleFactorPoundDistortion;
    this->actor.scale.y = this->scaleFactor + this->scaleFactorPoundDistortion;
    this->actor.scale.z = MAX(this->actor.scale.x, this->actor.scale.y);

    return isFinished;
}

void EnGo_RequestQuake(PlayState* play, s16 speed, s16 y, s16 duration) {
    s16 quakeIndex = Quake_Request(Play_GetCamera(play, CAM_ID_MAIN), QUAKE_TYPE_3);

    Quake_SetDuration(quakeIndex, duration);
    Quake_SetSpeed(quakeIndex, speed);
    Quake_SetPerturbations(quakeIndex, y, 0, 0, 0);
}

/**
 * Create the visual effects caused by the gatekeeper's pound.
 */
void EnGo_CreateGatekeeperPoundEffects(EnGo* this, PlayState* play) {
    EnGo_RequestQuake(play, 27767, 7, 20);
    play->actorCtx.unk2 = 4;
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TEST, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0, 0, 0, 0);
    EffectSsBlast_SpawnWhiteShockwave(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f);
}

/**
 * Draw the ice encasing a frozen Goron.
 */
void EnGo_DrawIceBlockWhenFrozen(EnGo* this, PlayState* play, f32 scale, f32 alpha) {
    u32 y1;
    u32 y2;

    if (this->actionFlags & ENGO_FLAG_FROZEN) {
        Matrix_Push();

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(scale * 0.7f, scale * 0.8f, scale, MTXMODE_APPLY);
        func_800B8118(&this->actor, play, 0);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        y1 = play->gameplayFrames % 256;
        y2 = (play->gameplayFrames * 2) % 256;
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, y1, 0x20, 0x10, 1, 0, y2, 0x40, 0x20, 0, 1, 0, 2));
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, (u8)alpha);
        gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);

        CLOSE_DISPS(play->state.gfxCtx);

        Matrix_Pop();
    }
}

/**
 * Initialize the steam effect used for melting ice.
 */
void EnGo_MakeSteam(EnGo* this) {
    static Vec3f sAccel = { 0.0f, 0.06f, 0.0f };
    Vec3f tempPos;
    Vec3f effectPos;
    s16 rotAngle = Rand_ZeroOne() * 360.0f * 182.0f;

    Math_Vec3f_Copy(&tempPos, &gZeroVec3f);
    tempPos.z = 28.0f;
    Lib_Vec3f_TranslateAndRotateY(&this->actor.world.pos, rotAngle, &tempPos, &effectPos);
    effectPos.y = (Rand_ZeroOne() * 10.0f) + 4.0f;
    effectPos.y += this->actor.floorHeight;
    EnGo_InitSteam(&this->effectTable[ENGO_SNOW_EFFECT_COUNT], effectPos, sAccel, gZeroVec3f, ENGO_NORMAL_SCALE,
                   (0.2f * ENGO_NORMAL_SCALE), ENGO_DUST_STEAM_LIFETIME);
}

/**
 * Transition through the cutscenes when the gatekeeper is opening the shrine.
 *
 * @return True when cutscenes end.
 */
s32 EnGo_HandleOpenShrineCutscene(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnGo* this = (EnGo*)thisx;
    s32 ret = false;

    switch (this->cutsceneState) {
        case 0:
            this->csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
            if (!EnGo_ChangeCutscene(this, this->csId)) {
                break;
            }
            this->gatekeeperAnimState = 1;
            this->cutsceneState = 1;
            // fallthrough
        case 1:
            if (CutsceneManager_GetCurrentCsId() == this->csId) {
                break;
            }
            this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
            this->cutsceneState = 2;
            // fallthrough
        case 2:
            if (!EnGo_ChangeCutscene(this, this->csId)) {
                break;
            }
            this->cutsceneState = 3;
            // fallthrough
        case 3:
            if (CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) {
                CutsceneManager_StartWithPlayerCs(CS_ID_GLOBAL_TALK, NULL);
                this->cutsceneState = 4;
            } else if (CutsceneManager_GetCurrentCsId() == this->csId) {
                CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
            }
            // fallthrough
        default:
            break;
    }

    switch (this->gatekeeperAnimState) {
        case 1:
            EnGo_ChangeAnim(this, play, ENGO_ANIM_ROLL);
            this->actionFlags |= ENGO_FLAG_ROLLING_UP;
            this->gatekeeperAnimState++;
            break;

        case 2:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                this->actionFlags &= ~ENGO_FLAG_ROLLING_UP;
                this->actionFlags &= ~ENGO_FLAG_STANDING;
                this->actionFlags |= ENGO_FLAG_ROLLED_UP;
                this->gatekeeperAnimState++;
                this->cutsceneDelayTimer = 0;
                this->actor.shape.yOffset = ENGO_ROLLEDUP_Y_OFFSET;
            }
            break;

        case 3:
            this->cutsceneDelayTimer++;
            if (this->cutsceneDelayTimer >= 10) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_JUMP);
                this->actor.velocity.y = ENGO_POUND_INITIAL_RISE_VEL;
                this->actor.gravity = -1.0f;
                this->gatekeeperAnimState++;
            }
            break;

        case 4:
            if (EnGo_HandleGatekeeperPoundCutscene(this, ENGO_POUND_INITIAL_RISE_VEL, (0.4f) * ENGO_NORMAL_SCALE, 6)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_LAND_BIG);
                EnGo_CreateGatekeeperPoundEffects(this, play);
                this->gatekeeperAnimState++;
                this->cutsceneDelayTimer = 0;
                SET_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE);
            }
            break;

        case 5:
            this->cutsceneDelayTimer++;
            if (this->cutsceneDelayTimer >= 10) {
                EnGo_ChangeAnim(this, play, ENGO_ANIM_SHIVER);
                this->actor.shape.rot.x = 0;
                this->actionFlags &= ~ENGO_FLAG_ROLLED_UP;
                this->actionFlags |= ENGO_FLAG_STANDING;
                this->gatekeeperAnimState++;
            }
            break;

        case 6:
            this->cutsceneState++;
            if (this->cutsceneState >= 65) {
                switch (player->transformation) {
                    case PLAYER_FORM_HUMAN:
                        SET_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_HUMAN);
                        break;

                    case PLAYER_FORM_GORON:
                        SET_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_GORON);
                        break;

                    case PLAYER_FORM_ZORA:
                        SET_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_ZORA);
                        break;

                    case PLAYER_FORM_DEKU:
                        SET_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_DEKU);
                        break;

                    default:
                        break;
                }
                ret = true;
            }
            break;

        default:
            break;
    }

    Actor_MoveWithGravity(&this->actor);
    return ret;
}

/**
 * Medigoron's MsgEvent callback to give the Powder Keg for the test.
 */
s32 EnGo_HandleGivePowderKegCutscene(Actor* thisx, PlayState* play) {
    static Vec3f sPowderKegSpawnOffset = { 0.0f, 200.0f, 280.0f };
    EnGo* this = (EnGo*)thisx;
    Vec3f powderKegSpawnPos;
    s32 ret = false;

    switch (this->cutsceneState) {
        case 0:
            this->csId = this->actor.csId;
            if (EnGo_ChangeCutscene(this, this->csId)) {
                this->cutsceneState++;
            }
            break;

        case 1:
            EnGo_ChangeAnim(this, play, ENGO_ANIM_DROPKEG);
            this->cutsceneState++;

        case 2:
            if (Animation_OnFrame(&this->skelAnime, 16.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_GORON_HAND_HIT);
            }

            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                EnGo_ChangeAnim(this, play, ENGO_ANIM_LYINGDOWNIDLE_IMM);
                Lib_Vec3f_TranslateAndRotateY(&this->actor.world.pos, this->actor.shape.rot.y, &sPowderKegSpawnOffset,
                                              &powderKegSpawnPos);
                gSaveContext.powderKegTimer = 2400;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, powderKegSpawnPos.x, powderKegSpawnPos.y,
                            powderKegSpawnPos.z, BOMB_EXPLOSIVE_TYPE_POWDER_KEG, 0, 0, BOMB_TYPE_BODY);
                EnGo_UpdateMedigoronColliderRadius(this, play, true);
                this->cutsceneDelayTimer = 0;
                this->cutsceneState++;
            }
            break;

        case 3:
            if (this->cutsceneDelayTimer >= 60) {
                CutsceneManager_Stop(this->csId);
                this->cutsceneDelayTimer = 0;
                this->cutsceneState = 0;
                ret = true;
            } else {
                this->cutsceneDelayTimer++;
            }
            break;

        default:
            break;
    }

    return ret;
}

/**
 * Update the stretching racer's animation and actionFlags.
 *
 * 'ENGO_ATHLETIC_A' through 'ENGO_ATHLETIC_D' exchange animations before and after the race, while the hamstring
 * stretchers have the same animation every time.
 *
 * @see EnGo_Idle
 */
void EnGo_ChangeToStretchingAnimation(EnGo* this, PlayState* play) {
    static Vec3f sStretchingGoronOffset = { 0.0f, 0.0f, 40.0f };
    static s32 sSubtypeToAnimIndex[] = {
        ENGO_ANIM_SQUAT_SIDE_TO_SIDE,  ENGO_ANIM_DOUBLE_ARM_SIDEBEND, ENGO_ANIM_SHAKE_LIMBS,
        ENGO_ANIM_SINGLE_ARM_SIDEBEND, ENGO_ANIM_SITTING_STRETCH,     ENGO_ANIM_HELP_SITTING_STRETCH,
    };
    Vec3f newSittingStretcherPos;

    // The first four Racer Gorons have different stretches depending on context
    // The last two remain the same (since they are dependent on one-another)
    s32 subtypeLookup = ENGO_GET_SUBTYPE(&this->actor) % 6;

    if (subtypeLookup < 4) {
        subtypeLookup = ((gSaveContext.eventInf[2] & 0xF) + subtypeLookup) % 4;
    }

    EnGo_ChangeAnim(this, play, sSubtypeToAnimIndex[subtypeLookup]);

    // Move the sitting Goron forward, since their spawn location is the same as their standing counterpart.
    if (this->animIndex == ENGO_ANIM_SITTING_STRETCH) {

        Lib_Vec3f_TranslateAndRotateY(&this->actor.world.pos, this->actor.shape.rot.y, &sStretchingGoronOffset,
                                      &newSittingStretcherPos);
        Math_Vec3f_Copy(&this->actor.world.pos, &newSittingStretcherPos);
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SetScale(&this->actor, this->scaleFactor);
    this->sleepState = ENGO_AWAKE;
    this->actionFlags = 0;
    this->actionFlags |= (ENGO_FLAG_LOST_ATTENTION | ENGO_FLAG_EYES_OPEN);
    this->actor.gravity = 0.0f;
}

/**
 * Update the Goron's animation and actionFlags to 'spectating'.
 *
 * Spectators engage in one of two cheering animations. Either cheering with their hands above their head, or
 * cupping their hands infront of their face and shouting.
 *
 * @see EnGo_Idle
 */
void EnGo_ChangeToSpectatingAnimation(EnGo* this, PlayState* play) {
    static s32 sSubtypeToAnimIndex[] = { ENGO_ANIM_CHEER, ENGO_ANIM_SHOUT };
    s16 animFrame;

    EnGo_ChangeAnim(this, play, sSubtypeToAnimIndex[ENGO_GET_SUBTYPE(&this->actor) % 2]);
    animFrame = Rand_ZeroOne() * this->skelAnime.endFrame;
    this->skelAnime.curFrame = animFrame;

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SetScale(&this->actor, this->scaleFactor);
    this->sleepState = ENGO_AWAKE;
    this->actionFlags = 0;
    this->actionFlags |= ENGO_FLAG_LOST_ATTENTION;
    this->actionFlags |= ENGO_FLAG_EYES_OPEN;
    this->actor.gravity = 0.0f;
}

/**
 * Update the Goron's animation and actionFlags to 'frozen'.
 *
 * The Goron will shiver upon awakening, but is otherwise encased in a block of ice which deals damage upon contact.
 *
 * @see EnGo_Frozen
 */
void EnGo_ChangeToFrozenAnimation(EnGo* this, PlayState* play) {
    Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInitFrozen);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->animIndex = ENGO_ANIM_NONE;
    EnGo_ChangeAnim(this, play, ENGO_ANIM_SHIVER);
    this->sleepState = ENGO_AWAKE;
    this->iceBlockScale = (this->scaleFactor / 0.01f) * 0.9f;
    this->eyeTexIndex = ENGO_EYETEX_CLOSED;
    this->actionFlags = 0;
    this->actionFlags |= ENGO_FLAG_LOST_ATTENTION;
    this->actionFlags |= ENGO_FLAG_FROZEN;
    this->iceBlockAlpha = 100.0f;
}

/**
 * Update the Goron Gatekeeper's animation and actionFlags to 'snowball'.
 *
 * @see EnGo_Snowball
 */
void EnGo_ChangeToSnowballAnimation(EnGo* this, PlayState* play) {
    s16 yawToPathPoint;
    Vec3f currentPos;
    Vec3f startingPathPoint;

    Math_Vec3f_Copy(&currentPos, &this->actor.world.pos);
    if (this->gatekeeperPath != NULL) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_DURING_OCARINA;
        SubS_CopyPointFromPathCheckBounds(this->gatekeeperPath, 0, &startingPathPoint);
        yawToPathPoint = Math_Vec3f_Yaw(&currentPos, &startingPathPoint);
        this->actor.shape.rot.y = yawToPathPoint;
        this->actor.world.rot.y = yawToPathPoint;
    }
    this->actionFlags = 0;
    this->actionFlags |= ENGO_FLAG_SNOWBALLED;
    this->actor.shape.yOffset = ENGO_SNOWBALL_Y_OFFSET;
    this->actor.gravity = -1.0f;
}

/**
 * Update the Goron's animation and actionFlags to 'covering ears'.
 *
 * @see EnGo_Idle
 */
void EnGo_ChangeToCoveringEarsAnimation(EnGo* this, PlayState* play) {
    EnGo_ChangeAnim(this, play, ENGO_ANIM_COVEREARS);
    Actor_SetScale(&this->actor, this->scaleFactor);
    this->actionFlags = 0;
    this->actor.gravity = -1.0f;
    SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->sleepState = ENGO_AWAKE;
    this->actionFlags |= ENGO_FLAG_LOST_ATTENTION;
    this->blinkTimer = 0;
    this->eyeTexIndex = ENGO_EYETEX_CLOSED2;
    this->iceBlockScale = 0.0f;
    this->iceBlockAlpha = 0.0f;
}

/**
 * Update the Goron's animation and actionFlags to 'shivering'.
 *
 * @see EnGo_Idle
 */
void EnGo_ChangeToShiveringAnimation(EnGo* this, PlayState* play) {
    EnGo_ChangeAnim(this, play, ENGO_ANIM_SHIVER);
    Actor_SetScale(&this->actor, this->scaleFactor);
    this->actionFlags = 0;
    this->actor.gravity = -1.0f;
    SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->sleepState = ENGO_AWAKE;
    this->actionFlags |= ENGO_FLAG_LOST_ATTENTION;
    this->actionFlags |= ENGO_FLAG_EYES_OPEN;
    this->blinkTimer = 0;
    this->eyeTexIndex = ENGO_EYETEX_OPEN;
    this->iceBlockScale = 0.0f;
    this->iceBlockAlpha = 0.0f;
}

/**
 * Setup a Goron stretching at the racetrack.
 *
 * Stretching Gorons placed in the starting area of the racetrack doing various stretches.
 */
void EnGo_SetupAthletic(EnGo* this, PlayState* play) {
    if (((gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 0)) ||
         (gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 2))) &&
        CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
        EnGo_ChangeToStretchingAnimation(this, play);
        this->actionFunc = EnGo_Idle;
    } else {
        Actor_Kill(&this->actor);
    }
}

/**
 * Setup a Goron spectating at the racetrack.
 *
 * Spectators to the Goron races cannot be engaged, they simply stand idle and cheer.
 */
void EnGo_SetupSpectator(EnGo* this, PlayState* play) {
    if ((gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 1)) ||
        (gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 0))) {
        EnGo_ChangeToSpectatingAnimation(this, play);
        this->actionFunc = EnGo_Idle;
    } else {
        Actor_Kill(&this->actor);
    }
}

/**
 * Setup the Goron Shrine's Gatekeeper.
 *
 * When spoken to, can open the Goron Shrine for the player.
 * - On Day 1: Can be found in the open, shivering.
 * - On Days 2,3: Can be found in a snowball.
 */
void EnGo_SetupGatekeeper(EnGo* this, PlayState* play) {
    if (gSaveContext.save.day >= 2) {
        this->gatekeeperPath = SubS_GetDayDependentPath(play, ENGO_GET_PATH_INDEX(&this->actor), ENGO_PATH_INDEX_NONE,
                                                        &this->indexPathPoint);
        if (this->gatekeeperPath != NULL) {
            this->indexPathPoint = 1;
        }
        EnGo_ChangeToSnowballAnimation(this, play);
        this->actionFunc = EnGo_Snowball;
        this->msgScriptCallback = EnGo_HandleOpenShrineCutscene;
    } else {
        EnGo_ChangeToShiveringAnimation(this, play);
        this->actionFunc = EnGo_Idle;
        this->msgScriptCallback = EnGo_HandleOpenShrineCutscene;
    }
}

/**
 * Setup a Goron at the graveyard
 *
 * The Gorons are together, one shivering from the cold, another frozen solid.
 */
void EnGo_SetupGraveyardGoron(EnGo* this, PlayState* play) {
    if ((ENGO_GET_SUBTYPE(&this->actor) == ENGO_GRAVEYARD_FROZEN) &&
        (((play->sceneId == SCENE_10YUKIYAMANOMURA2) && (gSaveContext.sceneLayer == 1) &&
          (play->csCtx.scriptIndex == 0)) ||
         !CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON))) {
        this->actor.child = EnGo_FindGravemaker(this, play);
        this->actor.child->child = &this->actor;
        EnGo_ChangeToFrozenAnimation(this, play);
        this->actionFunc = EnGo_Frozen;
    } else {
        EnGo_ChangeToShiveringAnimation(this, play);
        this->actionFunc = EnGo_Idle;
    }
}

/**
 * Setup a shrine Goron.
 *
 * There are three, but they all behave the same way aside from dialog.
 */
void EnGo_SetupShrineGoron(EnGo* this, PlayState* play) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
        EnGo_ChangeToCoveringEarsAnimation(this, play);
        this->actionFunc = EnGo_Idle;
    } else {
        EnGo_ChangeToShiveringAnimation(this, play);
        this->actionFunc = EnGo_Idle;
    }
}

/**
 * Setup the Medigoron in the Powder Keg Shop.
 *
 * This Goron sells Powder Kegs, and gives the player a Powder Keg as part of a challenge to gain
 * the privilege to buy them.
 */
void EnGo_SetupMedigoron(EnGo* this, PlayState* play) {
    EnGo_UpdateMedigoronColliderRadius(this, play, false);
    EnGo_ChangeAnim(this, play, ENGO_ANIM_LYINGDOWNIDLE);
    this->scaleFactor *= ENGO_MEDIGORON_SCALE_MULTIPLIER;
    Actor_SetScale(&this->actor, this->scaleFactor);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->actionFlags = 0;
    this->actor.gravity = -1.0f;
    SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->actionFlags |= ENGO_FLAG_LOST_ATTENTION;
    this->actionFlags |= ENGO_FLAG_EYES_OPEN;
    this->msgScriptCallback = EnGo_HandleGivePowderKegCutscene;
    this->actionFunc = EnGo_Idle;
}

/**
 * Default setup function as the initial action function.
 */
void EnGo_SetupInitialAction(EnGo* this, PlayState* play) {
    EffectTireMarkInit tireMarkInit = { 0, 62, { 0, 0, 15, 100 } };

    if (((this->taisouObjectSlot <= OBJECT_SLOT_NONE) || SubS_IsObjectLoaded(this->taisouObjectSlot, play)) ||
        ((this->hakuginDemoObjectSlot <= OBJECT_SLOT_NONE) || SubS_IsObjectLoaded(this->hakuginDemoObjectSlot, play))) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gGoronSkel, NULL, this->jointTable, this->morphTable,
                           GORON_LIMB_MAX);

        this->animIndex = ENGO_ANIM_NONE;
        EnGo_ChangeAnim(this, play, ENGO_ANIM_UNROLL);
        this->actor.draw = EnGo_Draw;

        Collider_InitAndSetSphere(play, &this->colliderSphere, &this->actor, &sSphereInit);
        Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
        Effect_Add(play, &this->indexEffect, EFFECT_TIRE_MARK, 0, 0, &tireMarkInit);

        this->actor.attentionRangeType = ATTENTION_RANGE_1;
        this->scaleFactor = ENGO_NORMAL_SCALE;
        this->msgScriptCallback = NULL;

        switch (ENGO_GET_TYPE(&this->actor)) {
            case ENGO_ATHLETIC:
                EnGo_SetupAthletic(this, play);
                break;

            case ENGO_SPECTATOR:
                EnGo_SetupSpectator(this, play);
                break;

            case ENGO_GATEKEEPER:
                EnGo_SetupGatekeeper(this, play);
                break;

            case ENGO_GRAVEYARD:
                EnGo_SetupGraveyardGoron(this, play);
                break;

            case ENGO_ASIDE_STORE:
            case ENGO_ASIDE_ELDERSROOM:
            case ENGO_ASIDE_ELDERSSON:
                EnGo_SetupShrineGoron(this, play);
                break;

            case ENGO_MEDIGORON:
                EnGo_SetupMedigoron(this, play);
                break;

            default:
                Actor_Kill(&this->actor);
                break;
        }
    }
}

/**
 * Default action for the Gorons. Different types have different default actions.
 *
 * The default action possibilities include:
 * - Shivering
 * - Covering ears
 * - Spectating (cheering or shouting)
 * - Stretching (side-to-side squatting, two-arm side bend, limb shake, one-arm side bend, hamstring stretching while
 * sitting, and helping the hamstring stretching Goron.)
 */
void EnGo_Idle(EnGo* this, PlayState* play) {
    s16 targetRot = this->actor.world.rot.y;

    if ((ENGO_GET_TYPE(&this->actor) == ENGO_SPECTATOR) &&
        (gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 1))) {
        // Spectators only cheer. No other interactions
        Actor_PlaySfx(&this->actor, NA_SE_EV_GORON_CHEER - SFX_FLAG);
    } else if (ENGO_GET_TYPE(&this->actor) != ENGO_MEDIGORON) {
        // All others besides the Medigoron in the Powder Keg Shop can fall asleep
        if (EnGo_IsFallingAsleep(this, play)) {
            SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
            this->sleepState = ENGO_ASLEEP_POS;
            this->actionFunc = EnGo_Sleep;
        } else if (ENGO_GET_TYPE(&this->actor) == ENGO_GRAVEYARD) {
            switch (ENGO_GET_SUBTYPE(&this->actor)) {
                case ENGO_GRAVEYARD_GRAVEMAKER:
                    EnGo_GravemakerIdle(this, play);
                    break;

                case ENGO_GRAVEYARD_FROZEN:
                    EnGo_FrozenIdle(this, play);
                    break;

                default:
                    break;
            }
        } else if (ENGO_GET_TYPE(&this->actor) == ENGO_ATHLETIC) {
            if (ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y)) < 0x3FFC) {
                SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
            } else {
                SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
            }
        }
    }
    Math_ApproachS(&this->actor.shape.rot.y, targetRot, 4, 0x2AA8);
}

/**
 * Handles falling asleep, as well as snoring while asleep.
 */
void EnGo_Sleep(EnGo* this, PlayState* play) {
    s16 targetRot = this->actor.world.rot.y;
    u16 sfxId;

    if (EnGo_IsFallingAsleep(this, play)) {
        this->sleepState = ENGO_ASLEEP_POS;
    }

    if (this->actionFlags & ENGO_FLAG_ROLLING_UP) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->actionFlags &= ~ENGO_FLAG_ROLLING_UP;
            this->actionFlags |= ENGO_FLAG_ROLLED_UP;
            if (this->sleepState != ENGO_AWAKE) {
                this->snorePhase = 0;
            }
            this->actor.shape.yOffset = ENGO_ROLLEDUP_Y_OFFSET;
        }
    } else if (this->actionFlags & ENGO_FLAG_UNROLLING) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->actionFlags |= ENGO_FLAG_STANDING;
            this->actionFlags &= ~ENGO_FLAG_UNROLLING;
        }
    } else if (this->actionFlags & ENGO_FLAG_ROLLED_UP) {
        if ((this->actor.xzDistToPlayer < 160.0f) && (this->actor.playerHeightRel < 20.0f) &&
            (this->sleepState == ENGO_AWAKE)) {
            EnGo_ChangeAnim(this, play, ENGO_ANIM_UNROLL_IMM);
            this->actionFlags &= ~ENGO_FLAG_STANDING;
            this->actionFlags &= ~ENGO_FLAG_ROLLED_UP;
            this->actionFlags |= ENGO_FLAG_UNROLLING;
            this->actor.shape.yOffset = ENGO_STANDING_Y_OFFSET;
        } else if ((this->sleepState != ENGO_AWAKE) && CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
            // While asleep, rhythmicallly snore and change shape to show breathing.
            this->actor.scale.x = this->scaleFactor - (Math_SinS(this->snorePhase) * (0.1f * ENGO_NORMAL_SCALE));
            this->actor.scale.y = this->scaleFactor + (Math_SinS(this->snorePhase) * (0.1f * ENGO_NORMAL_SCALE));
            this->actor.scale.z = this->scaleFactor + (Math_SinS(this->snorePhase) * (0.1f * ENGO_NORMAL_SCALE));

            if (this->snorePhase == 0) {
                this->sleepState = -this->sleepState;
                sfxId = (this->sleepState > ENGO_AWAKE) ? NA_SE_EN_GOLON_SNORE1 : NA_SE_EN_GOLON_SNORE2;
                Actor_PlaySfx(&this->actor, sfxId);
            }
            this->snorePhase += 0x400;
            this->actor.shape.yOffset = (this->actor.scale.y / this->scaleFactor) * ENGO_ROLLEDUP_Y_OFFSET;
            SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        }
    } else if ((this->actor.xzDistToPlayer >= 240.0f) || (this->actor.playerHeightRel >= 20.0f) ||
               (this->sleepState != ENGO_AWAKE)) {
        EnGo_ChangeAnim(this, play, ENGO_ANIM_ROLL);
        this->actionFlags &= ~ENGO_FLAG_STANDING;
        this->actionFlags &= ~ENGO_FLAG_ROLLED_UP;
        this->actionFlags |= ENGO_FLAG_ROLLING_UP;
        this->actor.shape.yOffset = ENGO_STANDING_Y_OFFSET;
    }

    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENGO_FIDGET_TABLE_LEN);
    Math_ApproachS(&this->actor.shape.rot.y, targetRot, 4, 0x2AA8);
}

/**
 * Action function for a frozen Goron.
 *
 * Transitions to the thawing cutscene when colliding with hot water or fire effect.
 */
void EnGo_Frozen(EnGo* this, PlayState* play) {
    Actor* actorCollidedWith = this->colliderCylinder.base.ac;

    if ((this->actionFlags & ENGO_FLAG_HIT_BY_OTHER) &&
        (((actorCollidedWith != NULL) && (actorCollidedWith->id == ACTOR_OBJ_AQUA) &&
          (AQUA_GET_TYPE(actorCollidedWith) != AQUA_TYPE_COLD)) ||
         (this->actor.colChkInfo.damageEffect == ENGO_DMGEFF_FIRE))) {
        this->actionFunc = EnGo_AwaitThaw;
    }
}

/**
 * Action function for transitioning to thawing.
 */
void EnGo_AwaitThaw(EnGo* this, PlayState* play) {
    if (EnGo_ChangeCutscene(this, this->actor.csId)) {
        this->actionFunc = EnGo_Thaw;
    }
}

/**
 * Action function for thawing of the Goron, melting sounds, and steam.
 */
void EnGo_Thaw(EnGo* this, PlayState* play) {
    EnGo* otherGoron = (EnGo*)this->actor.child;

    if ((s32)(this->iceBlockScale * 3.0f) != 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);
        Math_ApproachF(&this->iceBlockScale, 0.0f, 0.02f, 1.0f);
        this->iceBlockAlpha = (this->iceBlockScale / 0.9f) * 100.0f;
        EnGo_MakeSteam(this);
    } else {
        CutsceneManager_Stop(this->actor.csId);
        EnGo_ChangeToShiveringAnimation(this, play);
        if ((ENGO_GET_TYPE(&this->actor) == ENGO_GRAVEYARD) &&
            (ENGO_GET_SUBTYPE(&this->actor) == ENGO_GRAVEYARD_FROZEN)) {
            SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_AUTO, SUBS_OFFER_MODE_MASK);
            EnGo_ChangeToShiveringAnimation(otherGoron, play);
            otherGoron->actionFunc = EnGo_Idle;
        }
        this->actionFunc = EnGo_Idle;
    }
}

/**
 * Cutscene action function for the arrival of springtime.
 *
 * This cutscene consists of:
 * - The gravemaker stops shivering and looks around.
 * - The other Goron thaws and shivers briefly.
 * - The gravemaker pats the other's back and gestures broadly.
 * - They both look around.
 * - They both cheer.
 */
void EnGo_HandleSpringArrivalCutscene(EnGo* this, PlayState* play) {
    s32 animationIndices[] = {
        ENGO_ANIM_LYINGDOWNIDLE, ENGO_ANIM_UNROLL, ENGO_ANIM_SHIVER_IMM, ENGO_ANIM_LOOK_AROUND,
        ENGO_ANIM_SHOW,          ENGO_ANIM_SHIVER, ENGO_ANIM_SHIVER,     ENGO_ANIM_CHEER,
    };
    u16 cueType = 0;
    s32 cueId;
    s32 cueChannel;

    switch (ENGO_GET_SUBTYPE(&this->actor)) {
        case ENGO_GRAVEYARD_GRAVEMAKER:
            cueType = CS_CMD_ACTOR_CUE_128;
            break;

        case ENGO_GRAVEYARD_FROZEN:
            cueType = CS_CMD_ACTOR_CUE_129;
            break;

        default:
            break;
    }

    if ((cueType == CS_CMD_ACTOR_CUE_128) || (cueType == CS_CMD_ACTOR_CUE_129)) {
        if (Cutscene_IsCueInChannel(play, cueType)) {
            cueChannel = Cutscene_GetCueChannel(play, cueType);
            cueId = play->csCtx.actorCues[cueChannel]->id;

            if (this->springArrivalCueId != (u8)cueId) {
                this->springArrivalCueId = cueId;
                EnGo_ChangeAnim(this, play, animationIndices[cueId]);
                this->actionFlags = 0;
                this->actionFlags |= ENGO_FLAG_EYES_OPEN;
                this->eyeTexIndex = ENGO_EYETEX_OPEN;
                this->iceBlockScale = 0.0f;
                this->iceBlockAlpha = 0.0f;

                switch (cueId) {
                    case 1:
                        this->actionFlags |= ENGO_FLAG_STANDING;
                        this->skelAnime.curFrame = this->skelAnime.endFrame;
                        break;

                    case 5:
                    case 6:
                        EnGo_ChangeToFrozenAnimation(this, play);
                        break;

                    default:
                        break;
                }
            }

            switch (this->springArrivalCueId) {
                case 3:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) &&
                        (this->animIndex == ENGO_ANIM_LOOK_AROUND)) {
                        EnGo_ChangeAnim(this, play, ENGO_ANIM_LOOK_AROUND_LOOPED);
                    }
                    break;

                case 4:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) &&
                        (this->animIndex == ENGO_ANIM_SHOW)) {
                        EnGo_ChangeAnim(this, play, ENGO_ANIM_SHOW_LOOPED);
                    }
                    break;

                case 6:
                    if ((s32)(this->iceBlockScale * 3.0f) != 0) {
                        Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);
                        Math_ApproachF(&this->iceBlockScale, 0.0f, 0.02f, 1.0f);
                        this->iceBlockAlpha = (this->iceBlockScale / 0.9f) * 100.0f;
                        EnGo_MakeSteam(this);
                    } else if (this->actionFlags & ENGO_FLAG_FROZEN) {
                        EnGo_ChangeToShiveringAnimation(this, play);
                    }
                    break;

                default:
                    break;
            }

            if (cueType == 128) {
                switch (play->csCtx.curFrame) {
                    case 55:
                    case 100:
                    case 130:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_COLD);
                        break;

                    case 185:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_WAKE_UP);
                        break;

                    case 250:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_EYE_BIG);
                        break;

                    case 465:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_IWAIGORON_SOLO);
                        break;

                    case 490:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_VOICE_EATFULL);
                        break;

                    default:
                        break;
                }
            } else if (cueType == 129) {
                switch (play->csCtx.curFrame) {
                    case 360:
                    case 390:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_COLD);
                        break;

                    case 430:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_WAKE_UP);
                        break;

                    case 450:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_EYE_BIG);
                        break;

                    case 480:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_VOICE_EATFULL);
                        break;

                    default:
                        break;
                }
            }

            SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENGO_FIDGET_TABLE_LEN);
            Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        }
    }
}

/**
 * Action function for the Gatekeeper while they're in a snowball.
 *
 * On Day 2, they'll roll along a set path, and can be halted with certain attacks.
 * On Day 3, they're frozen solid.
 */
void EnGo_Snowball(EnGo* this, PlayState* play) {
    Vec3s* pathPoints;
    Vec3f currentPos;
    Vec3f currentPathPoint;

    if ((this->actionFlags & ENGO_FLAG_HIT_BY_OTHER) && (this->actor.colChkInfo.damageEffect == ENGO_DMGEFF_BREAK)) {
        // Stop the Gatekeeper when hit by an appropriate effect
        Actor_PlaySfx(&this->actor, NA_SE_EV_SNOWBALL_BROKEN);
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
        EnGo_InitSnow(this->effectTable, this->actor.world.pos);
        this->actor.shape.rot.x = 0;
        this->actor.speed = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_COLD);

        if (gSaveContext.save.day == 3) {
            EnGo_ChangeToFrozenAnimation(this, play);
            this->actionFunc = EnGo_Frozen;
        } else {
            EnGo_ChangeToShiveringAnimation(this, play);
            this->actionFunc = EnGo_Idle;
        }
    } else if (this->gatekeeperPath != NULL) {
        if (this->actionFlags & ENGO_FLAG_HIT_OTHER) {
            Player_PlaySfx(GET_PLAYER(play), NA_SE_PL_BODY_HIT);
            func_800B8D50(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 0);
        }

        pathPoints = Lib_SegmentedToVirtual(this->gatekeeperPath->points);
        if (SubS_HasReachedPoint(&this->actor, this->gatekeeperPath, this->indexPathPoint)) {
            if (this->indexPathPoint >= (this->gatekeeperPath->count - 1)) {
                this->indexPathPoint = 0;
            } else {
                this->indexPathPoint++;
            }
        }

        Math_Vec3s_ToVec3f(&currentPathPoint, &pathPoints[this->indexPathPoint]);
        Math_Vec3f_Copy(&currentPos, &this->actor.world.pos);
        Math_ApproachS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&currentPos, &currentPathPoint), 4, 0x38E);
        this->actor.shape.rot.y = this->actor.world.rot.y;

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
            func_800AE930(&play->colCtx, Effect_GetByIndex(this->indexEffect), &this->actor.world.pos, 18.0f,
                          this->actor.shape.rot.y, this->actor.floorPoly, this->actor.floorBgId);
        } else {
            func_800AEF44(Effect_GetByIndex(this->indexEffect));
        }

        this->actor.speed = 4.0f;
        this->actor.shape.rot.x += TRUNCF_BINANG(this->actor.speed * 546.0f);
        Actor_MoveWithGravity(&this->actor);
    }
}

/**
 * Return the MsgEvent script appropriate for the actor.
 */
MsgScript* EnGo_GetMsgScript(EnGo* this, PlayState* play) {
    static MsgScript* sMsgScriptGraveyard[] = {
        sMsgScriptGoronGravemaker,
        sMsgScriptGoronFrozen,
    };

    if (this->sleepState != ENGO_AWAKE) {
        return sMsgScriptGoronSleeping; // Lullaby hint
    }

    if (ENGO_GET_TYPE(&this->actor) == ENGO_ATHLETIC) {
        switch (ENGO_GET_SUBTYPE(&this->actor) % 6) {
            case ENGO_ATHLETIC_A:
                return sMsgScriptGoronAthleticA;

            case ENGO_ATHLETIC_B: // Unused!
                return sMsgScriptGoronAthleticB;

            case ENGO_ATHLETIC_C:
                return sMsgScriptGoronAthleticC;

            case ENGO_ATHLETIC_D:
                return sMsgScriptGoronAthleticD;

            case ENGO_ATHLETIC_HAMSTRINGSIT:
                return sMsgScriptGoronAthleticHamstring;

            case ENGO_ATHLETIC_HAMSTRINGSTAND:
                return sMsgScriptGoronAthleticHamstring;

            default:
                break;
        }
    }

    switch (ENGO_GET_TYPE(&this->actor)) {
        case ENGO_GATEKEEPER:
            return sMsgScriptGoronGatekeeper;

        case ENGO_GRAVEYARD:
            return sMsgScriptGraveyard[ENGO_GET_SUBTYPE(&this->actor)];

        case ENGO_ASIDE_STORE:
            return sMsgScriptGoronAsideStore;

        case ENGO_ASIDE_ELDERSROOM:
            return sMsgScriptGoronAsideEldersRoom;

        case ENGO_ASIDE_ELDERSSON:
            return sMsgScriptGoronAsideEldersSon;

        case ENGO_MEDIGORON:
            return sMsgScriptGoronMedigoron;

        default:
            return sMsgScriptGoronAsideStore;
    }
}

/**
 * Action function for dialog, movement due to dialog, and ending dialog
 */
void EnGo_Talk(EnGo* this, PlayState* play) {
    s32 pad;
    Vec3f targetPos;
    Vec3f thisPos;

    if (!MsgEvent_RunScript(&this->actor, play, EnGo_GetMsgScript(this, play), this->msgScriptCallback,
                            &this->msgScriptPos)) {
        if ((ENGO_GET_TYPE(&this->actor) != ENGO_ATHLETIC) && !(this->actionFlags & ENGO_FLAG_ROLLED_UP)) {
            Math_Vec3f_Copy(&targetPos, &this->attentionTarget->world.pos);
            Math_Vec3f_Copy(&thisPos, &this->actor.world.pos);
            Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&thisPos, &targetPos), 4, 0x2AA8);
        }
        SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENGO_FIDGET_TABLE_LEN);
        return;
    }

    if ((ENGO_GET_TYPE(&this->actor) == ENGO_ASIDE_STORE) || (ENGO_GET_TYPE(&this->actor) == ENGO_ASIDE_ELDERSROOM) ||
        (ENGO_GET_TYPE(&this->actor) == ENGO_ASIDE_ELDERSSON)) {
        this->blinkTimer = 0;
        this->actionFlags &= ~ENGO_FLAG_EYES_OPEN;
        this->eyeTexIndex = ENGO_EYETEX_CLOSED2;
    }

    this->actionFlags &= ~ENGO_FLAG_ENGAGED;
    SubS_SetOfferMode(&this->actionFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->msgScriptPos = 0;
    this->actionFlags |= ENGO_FLAG_LOST_ATTENTION;
    this->actionFunc = this->interruptedActionFunc;
}

void EnGo_Init(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;

    this->taisouObjectSlot = SubS_GetObjectSlot(OBJECT_TAISOU, play);
    this->hakuginDemoObjectSlot = SubS_GetObjectSlot(OBJECT_HAKUGIN_DEMO, play);
    this->actionFunc = EnGo_SetupInitialAction;
}

void EnGo_Destroy(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroySphere(play, &this->colliderSphere);
    Effect_Destroy(play, this->indexEffect);
}

void EnGo_Update(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;
    f32 xzRange;

    EnGo_DetectCollisions(this, play);

    if (!EnGo_UpdateTalking(this, play)) {
        EnGo_UpdateSpringArrivalCutscene(this, play);
    }

    this->actionFunc(this, play);

    if (!(this->actionFlags & ENGO_FLAG_FROZEN)) {
        EnGo_UpdateEyes(this);
        EnGo_UpdateSkelAnime(this, play);
        EnGo_UpdateAttentionTargetAndReactions(this, play);
        EnGo_UpdateSfx(this, play);
    }

    if (!(this->actionFlags & ENGO_FLAG_SNOWBALLED) && !(this->actionFlags & ENGO_FLAG_ROLLED_UP) &&
        !(this->actionFlags & ENGO_FLAG_FROZEN)) {
        if (ENGO_GET_TYPE(&this->actor) == ENGO_MEDIGORON) {
            xzRange = this->colliderSphere.dim.worldSphere.radius + 60;
        } else {
            xzRange = this->colliderCylinder.dim.radius + 40;
        }
        SubS_Offer(&this->actor, play, xzRange, 20.0f, PLAYER_IA_NONE, this->actionFlags & SUBS_OFFER_MODE_MASK);
    } else if ((this->actionFlags & ENGO_FLAG_ROLLED_UP) && (this->sleepState != ENGO_AWAKE)) {
        xzRange = this->colliderCylinder.dim.radius + 40;
        SubS_Offer(&this->actor, play, xzRange, 20.0f, PLAYER_IA_NONE, this->actionFlags & SUBS_OFFER_MODE_MASK);
    }

    if ((ENGO_GET_TYPE(&this->actor) != ENGO_MEDIGORON) && (ENGO_GET_TYPE(&this->actor) != ENGO_SPECTATOR) &&
        (ENGO_GET_TYPE(&this->actor) != ENGO_ATHLETIC)) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    }

    EnGo_UpdateFocus(this);
    EnGo_UpdateCollider(this, play);
    EnGo_UpdateEffects(this);
}

/**
 *  Draw function for a snowballed or rolled up Goron (no need to draw limbs).
 */
void EnGo_Draw_NoSkeleton(EnGo* this, PlayState* play) {
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + this->actor.shape.yOffset,
                     this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(0.0f, -this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);

    if (this->actionFlags & ENGO_FLAG_SNOWBALLED) {
        Matrix_Scale(this->actor.scale.x * 8.0f, this->actor.scale.y * 8.0f, this->actor.scale.z * 8.0f, MTXMODE_APPLY);
    } else {
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    }

    Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, (this->actionFlags & ENGO_FLAG_SNOWBALLED) ? gGoronSnowballDL : gGoronRolledUpDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnGo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnGo* this = (EnGo*)thisx;
    Vec3f worldPos;
    s32 fidgetIndex;

    if ((ENGO_GET_TYPE(&this->actor) == ENGO_MEDIGORON) && (limbIndex == GORON_LIMB_BODY)) {
        Matrix_MultZero(&worldPos);
        worldPos.y = this->actor.world.pos.y;
        Math_Vec3f_ToVec3s(&this->colliderSphere.dim.worldSphere.center, &worldPos);
    }

    switch (limbIndex) {
        case GORON_LIMB_BODY:
            fidgetIndex = 0;
            break;

        case GORON_LIMB_LEFT_UPPER_ARM:
            fidgetIndex = 1;
            break;

        case GORON_LIMB_RIGHT_UPPER_ARM:
            fidgetIndex = 2;
            break;

        default:
            fidgetIndex = 9;
            break;
    }

    if ((this->actionFlags & ENGO_FLAG_STANDING) && (fidgetIndex < 9)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[fidgetIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[fidgetIndex]) * 200.0f);
    }
    return false;
}

void EnGo_TransfromLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnGo* this = (EnGo*)thisx;
    s32 stepRot;
    s32 overrideRot;

    if (this->actionFlags & ENGO_FLAG_LOST_ATTENTION) {
        stepRot = false;
    } else {
        stepRot = true;
    }

    if (this->actionFlags & ENGO_FLAG_FACE_TARGET) {
        overrideRot = true;
    } else {
        overrideRot = false;
    }

    if (!stepRot) {
        overrideRot = false;
    }

    switch (limbIndex) {
        case GORON_LIMB_HEAD:
            SubS_UpdateLimb(this->headRotZ + this->bodyRotZ + 0x4000,
                            this->headRotY + this->bodyRotY + this->actor.shape.rot.y + 0x4000, &this->headPos,
                            &this->headRot, stepRot, overrideRot);
            Matrix_Pop();
            Matrix_Translate(this->headPos.x, this->headPos.y, this->headPos.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->headRot.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->headRot.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->headRot.z, MTXMODE_APPLY);
            Matrix_Translate(CVarGetFloat(CVAR_COSMETIC("Silly.GoronNeckLength"), 0.0f), 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Push();
            break;

        case GORON_LIMB_BODY:
            SubS_UpdateLimb(this->bodyRotZ + 0x4000, this->bodyRotY + this->actor.shape.rot.y + 0x4000, &this->bodyPos,
                            &this->bodyRot, stepRot, overrideRot);
            Matrix_Pop();
            Matrix_Translate(this->bodyPos.x, this->bodyPos.y, this->bodyPos.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->bodyRot.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->bodyRot.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->bodyRot.z, MTXMODE_APPLY);
            Matrix_Push();
            break;

        default:
            break;
    }
}

void EnGo_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gGoronEyeOpenTex, gGoronEyeHalfTex, gGoronEyeClosedTex, gGoronEyeHalfTex, gGoronEyeClosed2Tex,
    };
    EnGo* this = (EnGo*)thisx;

    if (!(this->actionFlags & (ENGO_FLAG_SNOWBALLED | ENGO_FLAG_ROLLED_UP))) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));

        if (this->animIndex == ENGO_ANIM_SITTING_STRETCH) {
            Matrix_Translate(0.0f, 0.0f, -4000.0f, MTXMODE_APPLY);
        }
        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnGo_OverrideLimbDraw, NULL, EnGo_TransfromLimbDraw,
                                       &this->actor);

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        EnGo_Draw_NoSkeleton(this, play);
    }
    EnGo_DrawIceBlockWhenFrozen(this, play, this->iceBlockScale, this->iceBlockAlpha);
    EnGo_DrawEffects(this, play);
}

/**
 * Whip Item Header
 *
 * Grapple whip: hooks onto beam/bar surfaces and certain actors for pendulum swing.
 * Paralyzes small enemies and pulls them to Link. Boomerang damage to others
 * with 3-second rage mode after stun. Disarms Stalfos shield / Lizalfos weapon.
 */

#ifndef ITEM_WHIP_H
#define ITEM_WHIP_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// States
// =============================================================================
#define WHIP_STATE_INACTIVE   0  // Not active
#define WHIP_STATE_EQUIP      1  // Held, coiled rope visible in hand
#define WHIP_STATE_EXTENDING  2  // Rope traveling forward
#define WHIP_STATE_HIT_ENEMY  3  // Hit enemy, processing interaction
#define WHIP_STATE_ATTACHED   4  // Attached to surface/actor, preparing swing
#define WHIP_STATE_SWINGING   5  // Pendulum swing active
#define WHIP_STATE_RETRACTING 6  // Rope returning to Link

// =============================================================================
// Range (matches longshot: 26 frames * 20.0f speed = 520 units)
// =============================================================================
#define WHIP_RANGE            520.0f
#define WHIP_EXTEND_SPEED     20.0f
#define WHIP_RETRACT_SPEED    25.0f
#define WHIP_TIMER_MAX        26

// =============================================================================
// Swing Physics
// =============================================================================
#define WHIP_GRAVITY          0.012f   // Angular acceleration from gravity
#define WHIP_DAMPING          0.995f   // Angular velocity damping per frame
#define WHIP_INPUT_FORCE      0.006f   // Stick input angular acceleration
#define WHIP_MAX_ANGLE        1.2f     // Max swing angle radians (~69 deg)
#define WHIP_RELEASE_BOOST    1.3f     // Velocity multiplier on release
#define WHIP_ROPE_LENGTH_MIN  80.0f    // Min rope length for swing
#define WHIP_ROPE_LENGTH_MAX  520.0f   // Max rope = range
#define WHIP_FIXED_ROPE_LENGTH 136.0f  // Fixed swing length = 2 adult Links (68 * 2)

// =============================================================================
// Combat
// =============================================================================
#define WHIP_DAMAGE           2        // Half heart (boomerang-level)
#define WHIP_STUN_FRAMES      60       // 1 second stun for paralyze
#define WHIP_PULL_SPEED       15.0f    // Speed to pull enemies to Link
#define WHIP_PULL_ARRIVE_DIST 50.0f    // Distance to consider pull complete
#define WHIP_RAGE_DURATION    90       // 3 seconds at 30fps
#define WHIP_RAGE_SPEED_MULT  1.5f     // Speed multiplier during rage

// =============================================================================
// Collision
// =============================================================================
#define WHIP_COL_RADIUS       12
#define WHIP_COL_HEIGHT       8
#define WHIP_ARRIVE_DIST      30.0f    // Distance to consider tip returned

// Boomerang damage flag
#ifndef DMG_BOOMERANG
#define DMG_BOOMERANG         (1 << 0x04)
#endif

// =============================================================================
// Rope Visual
// =============================================================================
#define WHIP_ROPE_SEGMENT     15.0f    // Units per segment
#define WHIP_ROPE_MAX_SEGS    40       // Max visual segments
#define WHIP_ROPE_SCALE       0.015f   // Segment model scale
#define WHIP_COIL_SEGMENTS    6        // Segments for coiled equip visual
#define WHIP_COIL_SCALE       0.008f   // Scale for coil segments
#define WHIP_ANCHOR_SCALE     0.012f   // Scale for hookshot tip at anchor

// =============================================================================
// Sound Effects
// =============================================================================
#define WHIP_SFX_THROW        NA_SE_IT_SWORD_SWING
#define WHIP_SFX_HIT_SURFACE  NA_SE_IT_HOOKSHOT_STICK_OBJ
#define WHIP_SFX_HIT_ENEMY    NA_SE_IT_SHIELD_BOUND
#define WHIP_SFX_RETRACT      NA_SE_PL_CATCH_BOOMERANG
#define WHIP_SFX_SWING        (NA_SE_IT_HOOKSHOT_CHAIN - SFX_FLAG)
#define WHIP_SFX_DISARM       NA_SE_IT_SHIELD_BOUND
#define WHIP_SFX_EQUIP        NA_SE_PL_CHANGE_ARMS
#define WHIP_SFX_UNEQUIP      NA_SE_PL_CHANGE_ARMS

// =============================================================================
// Disarm Flag (set in actor->home.rot.z to avoid clashing with params)
// =============================================================================
#define WHIP_DISARMED_FLAG    0x4000

// =============================================================================
// Tables
// =============================================================================

// Graspable actors (hook onto these for swing)
typedef struct {
    s16 actorId;
    s16 params;   // -1 = any
} WhipGrappleEntry;

static const WhipGrappleEntry sWhipGrappleTable[] = {
    { ACTOR_OBJ_SYOKUDAI, -1 },  // Torch stands (any variant)
};
#define WHIP_GRAPPLE_COUNT (sizeof(sWhipGrappleTable) / sizeof(sWhipGrappleTable[0]))

// Paralyze + pull enemies (stun and drag to Link)
typedef struct {
    s16 actorId;
    s16 params;   // -1 = any
} WhipParalyzeEntry;

static const WhipParalyzeEntry sWhipParalyzeTable[] = {
    { ACTOR_EN_FIREFLY, -1 },    // Keese (all variants)
    { ACTOR_EN_BB,      -1 },    // Bubble (all variants)
};
#define WHIP_PARALYZE_COUNT (sizeof(sWhipParalyzeTable) / sizeof(sWhipParalyzeTable[0]))

// Disarm enemies
typedef enum {
    WHIP_DISARM_SHIELD,   // Stalfos: lose shield (can't block)
    WHIP_DISARM_WEAPON    // Lizalfos: lose weapon (can't attack)
} WhipDisarmType;

typedef struct {
    s16 actorId;
    s16 params;           // -1 = any
    WhipDisarmType type;
} WhipDisarmEntry;

static const WhipDisarmEntry sWhipDisarmTable[] = {
    { ACTOR_EN_IK,       -1, WHIP_DISARM_SHIELD },  // Stalfos
    { ACTOR_EN_ZF, -1, WHIP_DISARM_WEAPON },  // Lizalfos / Dinolfos
};
#define WHIP_DISARM_COUNT (sizeof(sWhipDisarmTable) / sizeof(sWhipDisarmTable[0]))

// =============================================================================
// State Aliases (mapped to gCustomItemState fields)
// =============================================================================
#define whipActive        gCustomItemState.whipActive
#define whipState         gCustomItemState.whipState
#define whipTipPos        gCustomItemState.whipTipPos
#define whipAttachPos     gCustomItemState.whipAttachPos
#define whipAttachNormal  gCustomItemState.whipAttachNormal
#define whipTimer         gCustomItemState.whipTimer
#define whipCollider      gCustomItemState.whipCollider
#define whipSwingAngle    gCustomItemState.whipSwingAngle
#define whipSwingVel      gCustomItemState.whipSwingVel
#define whipSwingYaw      gCustomItemState.whipSwingYaw
#define whipRopeLength    gCustomItemState.whipRopeLength
#define whipAttachedBgId  gCustomItemState.whipAttachedBgId
#define whipPullTarget    gCustomItemState.whipPullTarget
#define whipRageTarget    gCustomItemState.whipRageTarget
#define whipRageTimer     gCustomItemState.whipRageTimer
#define whipRageOrigSpeed gCustomItemState.whipRageOrigSpeed
#define whipPrevInvinc    gCustomItemState.whipPrevInvinc
#define whipExtendYaw     gCustomItemState.whipExtendYaw
#define whipExtendPitch   gCustomItemState.whipExtendPitch
#define whipFirstPerson   gCustomItemState.whipFirstPersonActive

// =============================================================================
// Collider Init
// =============================================================================
static ColliderCylinderInit sWhipColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_TYPE_PLAYER, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2, { DMG_BOOMERANG, 0x00, WHIP_DAMAGE }, { 0, 0, 0 }, TOUCH_ON | TOUCH_SFX_NORMAL, BUMP_NONE, OCELEM_NONE },
    { WHIP_COL_RADIUS, WHIP_COL_HEIGHT, 0, { 0, 0, 0 } }
};

#endif // ITEM_WHIP_H

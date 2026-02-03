/**
 * Ball and Chain Item
 *
 * Heavy weapon that Link spins above his head and throws.
 * Deals Giant's Knife damage, double to Skulltulas/Freezards.
 * Can destroy Spirit Temple iron objects and red ice.
 */

#ifndef ITEM_BALLCHAIN_H
#define ITEM_BALLCHAIN_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// States
// =============================================================================
#define BALLCHAIN_STATE_INACTIVE    0   // Not equipped
#define BALLCHAIN_STATE_EQUIP       1   // Holding ball, can walk slowly
#define BALLCHAIN_STATE_SPINNING    2   // Spinning above head, charging
#define BALLCHAIN_STATE_THROWN      3   // Ball flying, Link frozen

// =============================================================================
// Physics Constants
// =============================================================================

// Throw
#define BALLCHAIN_THROW_DIST_MIN    150.0f   // Min throw range (no charge)
#define BALLCHAIN_THROW_DIST_MAX    300.0f  // Max throw range (full charge)
#define BALLCHAIN_THROW_SPEED       20.0f   // Ball travel speed
#define BALLCHAIN_RETRACT_SPEED     20.0f   // Return speed to Link

// Throw direction
#define BALLCHAIN_THROW_YAW_MAX     0x2000  // ~45 deg horizontal offset
#define BALLCHAIN_THROW_PITCH_MAX   0x1800  // ~30 deg vertical offset
#define BALLCHAIN_THROW_LEAN        3000    // Upper body forward lean

// Spin orbit
#define BALLCHAIN_SPIN_RADIUS       20.0f   // Orbit radius around Link
#define BALLCHAIN_SPIN_HEIGHT_MIN   50.0f   // Starting height (low charge)
#define BALLCHAIN_SPIN_HEIGHT_MAX   55.0f   // Final height (full charge)
#define BALLCHAIN_SPIN_SPEED_MIN    0x1000   // Starting spin angular velocity
#define BALLCHAIN_SPIN_SPEED_MAX    0x2000   // Max spin angular velocity
#define BALLCHAIN_CHARGE_MAX        60      // Frames to full charge

// Equip state
#define BALLCHAIN_EQUIP_HEIGHT      20.0f
#define BALLCHAIN_EQUIP_FORWARD     10.0f
#define BALLCHAIN_EQUIP_Y_OFFSET    5.0f
#define BALLCHAIN_EQUIP_SCALE       0.06f
#define BALLCHAIN_SPIN_SCALE        0.1f

// Movement penalties
#define BALLCHAIN_SPEED_MULT        0.4f    // Walk speed multiplier
#define BALLCHAIN_LEAN_MULT         3500.0f // Upper body lean factor
#define BALLCHAIN_LEAN_TILT         40.0f   // Orbit tilt from stick
#define BALLCHAIN_STICK_DEADZONE    5.0f

// =============================================================================
// Collision
// =============================================================================
#define BALLCHAIN_COL_RADIUS        20
#define BALLCHAIN_COL_HEIGHT        20
#define BALLCHAIN_WALL_RADIUS       20.0f
#define BALLCHAIN_WALL_HEIGHT       20.0f
#define BALLCHAIN_RETURN_DIST       65.0f   // Distance to consider "returned"
#define BALLCHAIN_DAMAGE            8       // 2 hearts

#ifndef DMG_JUMP_GIANT
#define DMG_JUMP_GIANT (1 << 0x1A)
#endif

// =============================================================================
// Sound Effects
// =============================================================================
#define BALLCHAIN_SFX_SWING         NA_SE_IT_HAMMER_SWING
#define BALLCHAIN_SFX_HIT           NA_SE_IT_HAMMER_HIT
#define BALLCHAIN_SFX_WHOOSH        (NA_SE_IT_SWORD_SWING - SFX_FLAG)
#define BALLCHAIN_SFX_RETRACT       (NA_SE_PL_WALK_GROUND - SFX_FLAG)
#define BALLCHAIN_SFX_WALL_BOUNCE   NA_SE_IT_SHIELD_BOUND
#define BALLCHAIN_SFX_VOICE_ADULT   NA_SE_VO_LI_SWORD_N
#define BALLCHAIN_SFX_VOICE_CHILD   NA_SE_VO_LI_SWORD_N_KID

// =============================================================================
// State Aliases (mapped to gCustomItemState fields)
// =============================================================================
#define bcActive        gCustomItemState.ballAndChainThrown         // u8: Item is active
#define bcState         gCustomItemState.timer2                     // s16: Current state (INACTIVE/EQUIP/SPINNING/THROWN)
#define bcCharge        gCustomItemState.timer1                     // s16: Charge frames (0 to CHARGE_MAX)
#define bcSpinAngle     gCustomItemState.somariaCooldown            // s16: Current spin angle (binary angle)
#define bcThrowDist     gCustomItemState.globalCooldownTimer        // s32: Remaining throw distance
#define bcThrowYaw      gCustomItemState.gustJarProjYaw             // s16: Throw direction yaw
#define bcThrowPitch    gCustomItemState.gustJarProjPitch           // s16: Throw direction pitch
#define bcBallPos       gCustomItemState.sharedProjectilePos        // Vec3f: Ball world position
#define bcCollider      gCustomItemState.ballAndChainCollider       // ColliderCylinder: Damage collider
#define bcFirstPerson   gCustomItemState.ballAndChainFirstPersonActive // u8: First person aim mode

#endif

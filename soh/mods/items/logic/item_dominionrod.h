/**
 * Dominion Rod Configuration Header
 * Edit this file to customize sounds, visuals, damage, and behavior
 */

#ifndef ITEM_DOMINIONROD_H
#define ITEM_DOMINIONROD_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// STATES
// =============================================================================

#define DOMROD_STATE_IDLE        0
#define DOMROD_STATE_AIMING      1
#define DOMROD_STATE_ORB_FLYING  2
#define DOMROD_STATE_ORB_RETURN  3
#define DOMROD_STATE_CONTROLLING 4
#define DOMROD_STATE_CATCHING    5

// =============================================================================
// ACTOR CONTROL TYPES
// =============================================================================

#define DOMROD_CONTROL_NONE      0
#define DOMROD_CONTROL_MOVEMENT  1  // Stick moves actor X/Z
#define DOMROD_CONTROL_ROTATION  2  // Stick rotates actor (Beamos)
#define DOMROD_CONTROL_ACTION    3  // Has special attacks only
#define DOMROD_CONTROL_FLAME     4  // Spawns controllable flame (Torch)

// =============================================================================
// ORB MOVEMENT CONSTANTS
// =============================================================================

#define DOMROD_ORB_SPEED             18.0f
#define DOMROD_ORB_SPEED_HOMING      22.0f
#define DOMROD_ORB_RETURN_SPEED      20.0f
#define DOMROD_ORB_MAX_RANGE         2500.0f   // ~25 meters (half range)
#define DOMROD_ORB_CATCH_DISTANCE    60.0f
#define DOMROD_CONTROL_MAX_DISTANCE  2500.0f   // ~25 meters (half range)
#define DOMROD_ORB_LAUNCH_OFFSET_Y   45.0f
#define DOMROD_ORB_LAUNCH_OFFSET_XZ  30.0f

// =============================================================================
// ORB VISUAL CONSTANTS
// =============================================================================

#define DOMROD_ORB_SCALE             5.5f
#define DOMROD_ORB_LIGHT_RADIUS      200
#define DOMROD_ORB_COLLIDER_RADIUS   35
#define DOMROD_ORB_COLLIDER_HEIGHT   50

// =============================================================================
// CONTROL MOVEMENT CONSTANTS
// =============================================================================

#define DOMROD_CONTROL_MAX_SPEED     8.0f
#define DOMROD_CONTROL_ACCEL         0.5f
#define DOMROD_CONTROL_DECEL         0.3f

// =============================================================================
// ACTOR-SPECIFIC CONSTANTS
// =============================================================================

// Beamos
#define DOMROD_BEAMOS_TURN_SPEED     0x600
#define DOMROD_BEAMOS_LASER_COOLDOWN 305  // Laser duration in frames

// Armos
#define DOMROD_ARMOS_HOP_VEL_Y       12.0f
#define DOMROD_ARMOS_HOP_SPEED       6.0f
#define DOMROD_ARMOS_SPIN_COOLDOWN   30

// Spike Trap
#define DOMROD_TRAP_DASH_SPEED       15.0f
#define DOMROD_TRAP_DASH_COOLDOWN    15

// Iron Knuckle
#define DOMROD_IK_WALK_SPEED         0.9f
#define DOMROD_IK_ATTACK_COOLDOWN    45

// Spike Ball
#define DOMROD_SPIKE_MAX_SPEED       2.8f
#define DOMROD_SPIKE_KAMIKAZE_SPEED  15.0f
#define DOMROD_SPIKE_REACH_DIST      50.0f
#define DOMROD_SPIKE_DAMAGE          8

// Torch Flame
#define DOMROD_FLAME_DURATION        600  // 10 seconds
#define DOMROD_FLAME_SPEED           8.0f

// Anubis
#define DOMROD_ANUBIS_SPEED          5.0f
#define DOMROD_ANUBIS_FIRE_COOLDOWN  30

// Floormaster
#define DOMROD_FLOORMASTER_SPEED     4.0f
#define DOMROD_FLOORMASTER_LUNGE_VEL 8.0f
#define DOMROD_FLOORMASTER_COOLDOWN  45

// Wallmaster
#define DOMROD_WALLMASTER_SPEED      5.0f
#define DOMROD_WALLMASTER_ASCEND     3.0f
#define DOMROD_WALLMASTER_DROP_VEL   -20.0f

// Boulder
#define DOMROD_BOULDER_BASE_SPEED    5.0f

// Pushable Block
#define DOMROD_BLOCK_SPEED           2.0f

// C-Button Hold Threshold (frames for hold vs tap)
#define DOMROD_CBUTTON_HOLD_FRAMES   40  // ~2 seconds at 20fps

// =============================================================================
// TIMING
// =============================================================================

#define DOMROD_ORB_MAX_TIME          600
#define DOMROD_CATCH_ANIM_FRAMES     12

// =============================================================================
// HOMING CONSTANTS
// =============================================================================

#define DOMROD_HOMING_TURN_SPEED     0x400

// =============================================================================
// SOUNDS
// =============================================================================

#define DOMROD_SFX_LAUNCH            NA_SE_IT_ARROW_SHOT
#define DOMROD_SFX_FLY               NA_SE_EN_FANTOM_FIRE
#define DOMROD_SFX_HIT_WALL          NA_SE_IT_SHIELD_BOUND
#define DOMROD_SFX_CATCH             NA_SE_PL_CATCH_BOOMERANG
#define DOMROD_SFX_POSSESS           NA_SE_EN_FANTOM_SPARK
#define DOMROD_SFX_RELEASE           NA_SE_EV_BOMB_DROP_WATER
#define DOMROD_SFX_ATTACK            NA_SE_EN_IRONNACK_SWING_AXE
#define DOMROD_SFX_EXPLODE           NA_SE_IT_BOMB_EXPLOSION

// =============================================================================
// COLORS - Primary (inner glow) and Environment (outer glow)
// =============================================================================

#define DOMROD_ORB_PRIM_R       255
#define DOMROD_ORB_PRIM_G       255
#define DOMROD_ORB_PRIM_B       255
#define DOMROD_ORB_PRIM_A       200

#define DOMROD_ORB_ENV_R        255
#define DOMROD_ORB_ENV_G        215
#define DOMROD_ORB_ENV_B        50
#define DOMROD_ORB_ENV_A        0

// Reticle color (first person mode) - Red
#define DOMROD_RETICLE_R        255
#define DOMROD_RETICLE_G        50
#define DOMROD_RETICLE_B        50

// =============================================================================
// STATE ALIASES
// =============================================================================

#define domRodActive             gCustomItemState.dominionRodActive
#define domRodState              gCustomItemState.dominionRodState
#define domRodFirstPerson        gCustomItemState.dominionRodFirstPersonActive
#define domRodOrbPos             gCustomItemState.dominionRodOrbPos
#define domRodOrbRot             gCustomItemState.dominionRodOrbRot
#define domRodControlledActor    gCustomItemState.dominionRodControlledActor
#define domRodTimer              gCustomItemState.dominionRodTimer
#define domRodStartPos           gCustomItemState.dominionRodStartPos
#define domRodCollider           gCustomItemState.dominionRodCollider
#define domRodLightNode          gCustomItemState.dominionRodLightNode
#define domRodLightInfo          gCustomItemState.dominionRodLightInfo
#define domRodButtonMask         gCustomItemState.dominionRodButtonMask
#define domRodControlType        gCustomItemState.dominionRodControlType
#define domRodControlVel         gCustomItemState.dominionRodControlVel
#define domRodDamagePaused       gCustomItemState.dominionRodDamagePaused
#define domRodPrevInvinc         gCustomItemState.dominionRodPrevInvincibility
#define domRodActorHomePos       gCustomItemState.dominionRodActorHomePos
#define domRodFlameTimer         gCustomItemState.dominionRodFlameTimer
#define domRodAttackCooldown     gCustomItemState.dominionRodAttackCooldown
#define domRodSpikeInvulnerable  gCustomItemState.dominionRodSpikeInvulnerable
#define domRodFlameActor         gCustomItemState.dominionRodFlameActor
#define domRodCButtonHoldTimer   gCustomItemState.dominionRodCButtonHoldTimer

// =============================================================================
// COLLIDER CONFIG
// =============================================================================

static ColliderCylinderInit sDomRodColliderInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_ON | OC1_TYPE_ALL,
      OC2_TYPE_PLAYER, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 },
      TOUCH_ON | TOUCH_NEAREST, BUMP_NONE, OCELEM_ON },
    { DOMROD_ORB_COLLIDER_RADIUS, DOMROD_ORB_COLLIDER_HEIGHT, 0, { 0, 0, 0 } }
};

// =============================================================================
// FUNCTIONS
// =============================================================================

void Handle_DominionRod(Player* player, PlayState* play);
void Player_InitDominionRodIA(PlayState* play, Player* player);
void CustomItems_DrawDominionRod(Player* player, PlayState* play);
void CustomItems_DrawDominionRodReticle(Player* player, PlayState* play);

#endif // ITEM_DOMINIONROD_H

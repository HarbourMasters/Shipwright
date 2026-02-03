/**
 * Fire Rod Configuration Header
 * Edit this file to customize sounds, visuals, damage, and behavior
 */

#ifndef ITEM_ROD_FIRE_H
#define ITEM_ROD_FIRE_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// STATES
// =============================================================================

#define FIRE_ROD_STATE_INACTIVE     0
#define FIRE_ROD_STATE_EQUIPPED     1
#define FIRE_ROD_STATE_SWINGING     2
#define FIRE_ROD_STATE_CHARGING     3
#define FIRE_ROD_STATE_AIMING       4

// =============================================================================
// CHARGE SETTINGS
// =============================================================================

#define FIRE_ROD_CHARGE_RATE        0.02f
#define FIRE_ROD_CHARGE_MIN         0.1f
#define FIRE_ROD_CHARGE_BIG         0.85f
#define FIRE_ROD_CHARGE_HOLD_FRAMES 10

// =============================================================================
// MAGIC COSTS
// =============================================================================

#define FIRE_ROD_MAGIC_SLASH        3
#define FIRE_ROD_MAGIC_STAB         3
#define FIRE_ROD_MAGIC_JUMP         6
#define FIRE_ROD_MAGIC_SPIN_SMALL   6
#define FIRE_ROD_MAGIC_SPIN_BIG     12

// =============================================================================
// BACKFIRE CHANCES (percentage when no magic)
// =============================================================================

#define FIRE_ROD_BACKFIRE_SLASH     10
#define FIRE_ROD_BACKFIRE_JUMP      20
#define FIRE_ROD_BACKFIRE_SPIN      50

// =============================================================================
// PROJECTILE SETTINGS
// =============================================================================

#define FIRE_ROD_PROJ_SPEED         15.0f
#define FIRE_ROD_PROJ_LIFETIME      30
#define FIRE_ROD_PROJ_RADIUS        10
#define FIRE_ROD_PROJ_HEIGHT        20
#define FIRE_ROD_PROJ_DAMAGE        4

// =============================================================================
// SLASH SETTINGS (3 fireballs spread)
// =============================================================================

#define FIRE_ROD_SLASH_RANGE        200.0f
#define FIRE_ROD_SLASH_SPREAD       30
#define FIRE_ROD_SLASH_COUNT        3

// =============================================================================
// FLAMETHROWER SETTINGS (jump slash)
// =============================================================================

#define FIRE_ROD_FLAME_COUNT        6
#define FIRE_ROD_FLAME_SPACING      40.0f
#define FIRE_ROD_FLAME_BASE_SCALE   50.0f
#define FIRE_ROD_FLAME_SCALE_GROW   30.0f
#define FIRE_ROD_FLAME_RADIUS       5
#define FIRE_ROD_FLAME_HEIGHT       10
#define FIRE_ROD_FLAME_DAMAGE       4

// =============================================================================
// SPIN ATTACK SETTINGS
// =============================================================================

#define FIRE_ROD_SPIN_SMALL_RADIUS  100.0f
#define FIRE_ROD_SPIN_BIG_RADIUS    500.0f
#define FIRE_ROD_SPIN_DAMAGE        8

// =============================================================================
// SOUNDS - Change these to customize audio
// =============================================================================

#define FIRE_ROD_SFX_SWING          NA_SE_IT_SWORD_SWING
#define FIRE_ROD_SFX_CHARGE         NA_SE_PL_SWORD_CHARGE
#define FIRE_ROD_SFX_FIRE_LOOP      NA_SE_EN_ANUBIS_FIRE
#define FIRE_ROD_SFX_FIRE_IGNITE    NA_SE_IT_BOMB_IGNIT
#define FIRE_ROD_SFX_FIRE_EXPLODE   NA_SE_EN_ANUBIS_FIREBOMB
#define FIRE_ROD_SFX_FIRE_CAST      NA_SE_PL_MAGIC_FIRE
#define FIRE_ROD_SFX_FLAMETHROWER   NA_SE_PL_MAGIC_FIRE
#define FIRE_ROD_SFX_BACKFIRE_HIT   NA_SE_PL_BODY_HIT
#define FIRE_ROD_SFX_NO_MAGIC       NA_SE_SY_ERROR

// =============================================================================
// COLORS - Primary (inner glow) and Environment (outer glow)
// =============================================================================

#define FIRE_ROD_PRIM_R     255
#define FIRE_ROD_PRIM_G     255
#define FIRE_ROD_PRIM_B     0
#define FIRE_ROD_PRIM_A     255

#define FIRE_ROD_ENV_R      255
#define FIRE_ROD_ENV_G      80
#define FIRE_ROD_ENV_B      0
#define FIRE_ROD_ENV_A      255

// Sword trail colors
#define FIRE_ROD_TRAIL_P1_R     255
#define FIRE_ROD_TRAIL_P1_G     200
#define FIRE_ROD_TRAIL_P1_B     50
#define FIRE_ROD_TRAIL_P1_A     255

#define FIRE_ROD_TRAIL_P2_R     255
#define FIRE_ROD_TRAIL_P2_G     80
#define FIRE_ROD_TRAIL_P2_B     0
#define FIRE_ROD_TRAIL_P2_A     128

// Reticle color (first person mode)
#define FIRE_ROD_RETICLE_R      255
#define FIRE_ROD_RETICLE_G      0
#define FIRE_ROD_RETICLE_B      0

// =============================================================================
// STATE ALIASES
// =============================================================================

#define fireRodActive       gCustomItemState.fireRodActive
#define fireRodState        gCustomItemState.fireRodState
#define fireRodProjActive   gCustomItemState.fireRodProjActive
#define fireRodProjType     gCustomItemState.fireRodProjType
#define fireRodProjPos      gCustomItemState.fireRodProjPos
#define fireRodProjYaw      gCustomItemState.fireRodProjYaw
#define fireRodProjPitch    gCustomItemState.fireRodProjPitch
#define fireRodProjTimer    gCustomItemState.fireRodProjTimer
#define fireRodCollider     gCustomItemState.fireRodCollider
#define fireRodBlureIdx     gCustomItemState.fireRodBlureIdx

#define fireRodProjTrail    gCustomItemState.fireRodProjTrail
#define fireRodProjScale    gCustomItemState.fireRodProjScale
#define fireRodProjRotZ     gCustomItemState.fireRodProjRotZ
#define fireRodProjTrailIdx gCustomItemState.fireRodProjTrailIdx

#define fireRodCharging     gCustomItemState.fireRodCharging
#define fireRodChargeLevel  gCustomItemState.fireRodChargeLevel
#define fireRodChargeReady  gCustomItemState.fireRodChargeReady
#define fireRodChargeTimer  gCustomItemState.fireRodChargeTimer

#define fireRodSpinActive      gCustomItemState.fireRodSpinActive
#define fireRodSpinIsBig       gCustomItemState.fireRodSpinIsBig
#define fireRodSpinRadius      gCustomItemState.fireRodSpinRadius
#define fireRodSpinMaxRadius   gCustomItemState.fireRodSpinMaxRadius
#define fireRodSpinCollider    gCustomItemState.fireRodSpinCollider

#define fireRodFirstPerson     gCustomItemState.fireRodFirstPerson
#define fireRodButtonMask      gCustomItemState.fireRodButtonMask

// =============================================================================
// COLLIDER CONFIGS
// =============================================================================

static ColliderCylinderInit sFireRodProjColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_ARROW_FIRE, 0x01, FIRE_ROD_PROJ_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { FIRE_ROD_PROJ_RADIUS, FIRE_ROD_PROJ_HEIGHT, 0, { 0, 0, 0 } }
};

static ColliderCylinderInit sFireRodSpinColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_MAGIC_FIRE | DMG_SLASH, 0x01, FIRE_ROD_SPIN_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { 50, 80, 0, { 0, 0, 0 } }
};

static ColliderCylinderInit sFireRodFlameColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_ARROW_FIRE, 0x01, FIRE_ROD_FLAME_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { FIRE_ROD_FLAME_RADIUS, FIRE_ROD_FLAME_HEIGHT, 0, { 0, 0, 0 } }
};

// =============================================================================
// FUNCTIONS
// =============================================================================

void Handle_FireRod(Player* player, PlayState* play);
void Player_InitFireRodIA(PlayState* play, Player* player);
void CustomItems_DrawFireRod(Player* player, PlayState* play);
void CustomItems_DrawFireRodReticle(Player* player, PlayState* play);

#endif

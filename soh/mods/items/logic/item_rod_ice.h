/**
 * Ice Rod Configuration Header
 * Edit this file to customize sounds, visuals, damage, and behavior
 */

#ifndef ITEM_ROD_ICE_H
#define ITEM_ROD_ICE_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// STATES
// =============================================================================

#define ICE_ROD_STATE_INACTIVE     0
#define ICE_ROD_STATE_EQUIPPED     1
#define ICE_ROD_STATE_SWINGING     2
#define ICE_ROD_STATE_CHARGING     3
#define ICE_ROD_STATE_AIMING       4

// =============================================================================
// CHARGE SETTINGS
// =============================================================================

#define ICE_ROD_CHARGE_RATE        0.02f
#define ICE_ROD_CHARGE_MIN         0.1f
#define ICE_ROD_CHARGE_BIG         0.85f
#define ICE_ROD_CHARGE_HOLD_FRAMES 10

// =============================================================================
// MAGIC COSTS
// =============================================================================

#define ICE_ROD_MAGIC_SLASH        3
#define ICE_ROD_MAGIC_STAB         3
#define ICE_ROD_MAGIC_JUMP         6
#define ICE_ROD_MAGIC_SPIN_SMALL   6
#define ICE_ROD_MAGIC_SPIN_BIG     12

// =============================================================================
// BACKFIRE CHANCES (percentage when no magic)
// =============================================================================

#define ICE_ROD_BACKFIRE_SLASH     10
#define ICE_ROD_BACKFIRE_JUMP      20
#define ICE_ROD_BACKFIRE_SPIN      50

// =============================================================================
// PROJECTILE SETTINGS
// =============================================================================

#define ICE_ROD_PROJ_SPEED         15.0f
#define ICE_ROD_PROJ_LIFETIME      30
#define ICE_ROD_PROJ_RADIUS        10
#define ICE_ROD_PROJ_HEIGHT        20
#define ICE_ROD_PROJ_DAMAGE        4

// =============================================================================
// SLASH SETTINGS (3 iceballs spread)
// =============================================================================

#define ICE_ROD_SLASH_RANGE        200.0f
#define ICE_ROD_SLASH_SPREAD       30
#define ICE_ROD_SLASH_COUNT        3

// =============================================================================
// ICE WAVE SETTINGS (jump slash - like flamethrower but ice)
// =============================================================================

#define ICE_ROD_WAVE_COUNT         6
#define ICE_ROD_WAVE_SPACING       40.0f
#define ICE_ROD_WAVE_BASE_SCALE    0.15f
#define ICE_ROD_WAVE_SCALE_GROW    0.05f
#define ICE_ROD_WAVE_RADIUS        5
#define ICE_ROD_WAVE_HEIGHT        10
#define ICE_ROD_WAVE_DAMAGE        4

// =============================================================================
// SPIN ATTACK SETTINGS
// =============================================================================

#define ICE_ROD_SPIN_SMALL_RADIUS  100.0f
#define ICE_ROD_SPIN_BIG_RADIUS    500.0f
#define ICE_ROD_SPIN_DAMAGE        8

// =============================================================================
// FREEZE SETTINGS
// =============================================================================

#define ICE_ROD_FREEZE_DURATION    60    // Frames enemies stay frozen

// =============================================================================
// SOUNDS - Change these to customize audio
// =============================================================================

#define ICE_ROD_SFX_SWING          NA_SE_IT_SWORD_SWING
#define ICE_ROD_SFX_CHARGE         NA_SE_PL_SWORD_CHARGE
#define ICE_ROD_SFX_ICE_LOOP       NA_SE_EV_ICE_FREEZE
#define ICE_ROD_SFX_ICE_IGNITE     NA_SE_EV_ICE_MELT
#define ICE_ROD_SFX_ICE_EXPLODE    NA_SE_EV_ICE_BROKEN
#define ICE_ROD_SFX_ICE_CAST       NA_SE_PL_FREEZE_S
#define ICE_ROD_SFX_ICEWAVE        NA_SE_EV_ICE_FREEZE
#define ICE_ROD_SFX_BACKFIRE_HIT   NA_SE_PL_FREEZE_S
#define ICE_ROD_SFX_NO_MAGIC       NA_SE_SY_ERROR

// =============================================================================
// COLORS - Primary (inner glow) and Environment (outer glow)
// =============================================================================

#define ICE_ROD_PRIM_R     200
#define ICE_ROD_PRIM_G     255
#define ICE_ROD_PRIM_B     255
#define ICE_ROD_PRIM_A     255

#define ICE_ROD_ENV_R      0
#define ICE_ROD_ENV_G      100
#define ICE_ROD_ENV_B      255
#define ICE_ROD_ENV_A      255

// Sword trail colors (icy blue to white)
#define ICE_ROD_TRAIL_P1_R     200
#define ICE_ROD_TRAIL_P1_G     255
#define ICE_ROD_TRAIL_P1_B     255
#define ICE_ROD_TRAIL_P1_A     255

#define ICE_ROD_TRAIL_P2_R     0
#define ICE_ROD_TRAIL_P2_G     100
#define ICE_ROD_TRAIL_P2_B     255
#define ICE_ROD_TRAIL_P2_A     128

// Reticle color (first person mode)
#define ICE_ROD_RETICLE_R      0
#define ICE_ROD_RETICLE_G      200
#define ICE_ROD_RETICLE_B      255

// =============================================================================
// STATE ALIASES
// =============================================================================

#define iceRodActive       gCustomItemState.iceRodActive
#define iceRodState        gCustomItemState.iceRodState
#define iceRodProjActive   gCustomItemState.iceRodProjActive
#define iceRodProjType     gCustomItemState.iceRodProjType
#define iceRodProjPos      gCustomItemState.iceRodProjPos
#define iceRodProjYaw      gCustomItemState.iceRodProjYaw
#define iceRodProjPitch    gCustomItemState.iceRodProjPitch
#define iceRodProjTimer    gCustomItemState.iceRodProjTimer
#define iceRodCollider     gCustomItemState.iceRodCollider
#define iceRodBlureIdx     gCustomItemState.iceRodBlureIdx

#define iceRodProjTrail    gCustomItemState.iceRodProjTrail
#define iceRodProjScale    gCustomItemState.iceRodProjScale
#define iceRodProjRotZ     gCustomItemState.iceRodProjRotZ
#define iceRodProjTrailIdx gCustomItemState.iceRodProjTrailIdx

#define iceRodCharging     gCustomItemState.iceRodCharging
#define iceRodChargeLevel  gCustomItemState.iceRodChargeLevel
#define iceRodChargeReady  gCustomItemState.iceRodChargeReady
#define iceRodChargeTimer  gCustomItemState.iceRodChargeTimer

#define iceRodSpinActive      gCustomItemState.iceRodSpinActive
#define iceRodSpinIsBig       gCustomItemState.iceRodSpinIsBig
#define iceRodSpinRadius      gCustomItemState.iceRodSpinRadius
#define iceRodSpinMaxRadius   gCustomItemState.iceRodSpinMaxRadius
#define iceRodSpinCollider    gCustomItemState.iceRodSpinCollider

#define iceRodFirstPerson     gCustomItemState.iceRodFirstPerson
#define iceRodButtonMask      gCustomItemState.iceRodButtonMask

// =============================================================================
// COLLIDER CONFIGS
// =============================================================================

static ColliderCylinderInit sIceRodProjColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_ARROW_ICE, 0x01, ICE_ROD_PROJ_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { ICE_ROD_PROJ_RADIUS, ICE_ROD_PROJ_HEIGHT, 0, { 0, 0, 0 } }
};

static ColliderCylinderInit sIceRodSpinColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_MAGIC_ICE | DMG_SLASH, 0x01, ICE_ROD_SPIN_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { 50, 80, 0, { 0, 0, 0 } }
};

static ColliderCylinderInit sIceRodWaveColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_ARROW_ICE, 0x01, ICE_ROD_WAVE_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { ICE_ROD_WAVE_RADIUS, ICE_ROD_WAVE_HEIGHT, 0, { 0, 0, 0 } }
};

// =============================================================================
// FUNCTIONS
// =============================================================================

void Handle_IceRod(Player* player, PlayState* play);
void Player_InitIceRodIA(PlayState* play, Player* player);
void CustomItems_DrawIceRod(Player* player, PlayState* play);
void CustomItems_DrawIceRodReticle(Player* player, PlayState* play);

#endif

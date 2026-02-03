/**
 * Light Rod Configuration Header
 * Edit this file to customize sounds, visuals, damage, and behavior
 */

#ifndef ITEM_ROD_LIGHT_H
#define ITEM_ROD_LIGHT_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// STATES
// =============================================================================

#define LIGHT_ROD_STATE_INACTIVE     0
#define LIGHT_ROD_STATE_EQUIPPED     1
#define LIGHT_ROD_STATE_SWINGING     2
#define LIGHT_ROD_STATE_CHARGING     3
#define LIGHT_ROD_STATE_AIMING       4

// =============================================================================
// CHARGE SETTINGS
// =============================================================================

#define LIGHT_ROD_CHARGE_RATE        0.02f
#define LIGHT_ROD_CHARGE_MIN         0.1f
#define LIGHT_ROD_CHARGE_BIG         0.85f
#define LIGHT_ROD_CHARGE_HOLD_FRAMES 10

// =============================================================================
// MAGIC COSTS
// =============================================================================

#define LIGHT_ROD_MAGIC_SLASH        3
#define LIGHT_ROD_MAGIC_STAB         3
#define LIGHT_ROD_MAGIC_JUMP         6
#define LIGHT_ROD_MAGIC_SPIN_SMALL   6
#define LIGHT_ROD_MAGIC_SPIN_BIG     12

// =============================================================================
// BACKFIRE CHANCES (percentage when no magic)
// =============================================================================

#define LIGHT_ROD_BACKFIRE_SLASH     10
#define LIGHT_ROD_BACKFIRE_JUMP      20
#define LIGHT_ROD_BACKFIRE_SPIN      50

// =============================================================================
// PROJECTILE SETTINGS
// =============================================================================

#define LIGHT_ROD_PROJ_SPEED         18.0f
#define LIGHT_ROD_PROJ_LIFETIME      25
#define LIGHT_ROD_PROJ_RADIUS        10
#define LIGHT_ROD_PROJ_HEIGHT        20
#define LIGHT_ROD_PROJ_DAMAGE        4

// =============================================================================
// SLASH SETTINGS (3 light balls spread)
// =============================================================================

#define LIGHT_ROD_SLASH_RANGE        200.0f
#define LIGHT_ROD_SLASH_SPREAD       30
#define LIGHT_ROD_SLASH_COUNT        3

// =============================================================================
// LIGHT BEAM SETTINGS (jump slash - increasing light balls)
// =============================================================================

#define LIGHT_ROD_BEAM_COUNT         6
#define LIGHT_ROD_BEAM_SPACING       40.0f
#define LIGHT_ROD_BEAM_BASE_SCALE    50.0f
#define LIGHT_ROD_BEAM_SCALE_GROW    30.0f
#define LIGHT_ROD_BEAM_RADIUS        5
#define LIGHT_ROD_BEAM_HEIGHT        10
#define LIGHT_ROD_BEAM_DAMAGE        4

// =============================================================================
// SPIN ATTACK SETTINGS (paralyze all enemies in radius)
// =============================================================================

#define LIGHT_ROD_SPIN_SMALL_RADIUS  100.0f
#define LIGHT_ROD_SPIN_BIG_RADIUS    500.0f
#define LIGHT_ROD_SPIN_DAMAGE        8
#define LIGHT_ROD_STUN_DURATION      60    // Frames enemies stay stunned

// =============================================================================
// SOUNDS - Change these to customize audio
// =============================================================================

#define LIGHT_ROD_SFX_SWING          NA_SE_IT_SWORD_SWING
#define LIGHT_ROD_SFX_CHARGE         NA_SE_PL_SWORD_CHARGE
#define LIGHT_ROD_SFX_LIGHT_LOOP     NA_SE_EN_FANTOM_SPARK
#define LIGHT_ROD_SFX_LIGHT_IGNITE   NA_SE_EV_TRIFORCE_FLASH
#define LIGHT_ROD_SFX_LIGHT_EXPLODE  NA_SE_IT_SWORD_REFLECT_MG
#define LIGHT_ROD_SFX_LIGHT_CAST     NA_SE_IT_LASH
#define LIGHT_ROD_SFX_LIGHTBEAM      NA_SE_EN_FANTOM_SPARK
#define LIGHT_ROD_SFX_BACKFIRE_HIT   NA_SE_EN_FANTOM_HIT_THUNDER
#define LIGHT_ROD_SFX_NO_MAGIC       NA_SE_SY_ERROR

// =============================================================================
// COLORS - Primary (inner glow) and Environment (outer glow)
// =============================================================================

#define LIGHT_ROD_PRIM_R     255
#define LIGHT_ROD_PRIM_G     255
#define LIGHT_ROD_PRIM_B     200
#define LIGHT_ROD_PRIM_A     255

#define LIGHT_ROD_ENV_R      255
#define LIGHT_ROD_ENV_G      255
#define LIGHT_ROD_ENV_B      0
#define LIGHT_ROD_ENV_A      255

// Sword trail colors (golden yellow to white)
#define LIGHT_ROD_TRAIL_P1_R     255
#define LIGHT_ROD_TRAIL_P1_G     255
#define LIGHT_ROD_TRAIL_P1_B     200
#define LIGHT_ROD_TRAIL_P1_A     255

#define LIGHT_ROD_TRAIL_P2_R     255
#define LIGHT_ROD_TRAIL_P2_G     200
#define LIGHT_ROD_TRAIL_P2_B     0
#define LIGHT_ROD_TRAIL_P2_A     128

// Reticle color (first person mode)
#define LIGHT_ROD_RETICLE_R      255
#define LIGHT_ROD_RETICLE_G      255
#define LIGHT_ROD_RETICLE_B      0

// =============================================================================
// STATE ALIASES
// =============================================================================

#define lightRodActive       gCustomItemState.lightRodActive
#define lightRodState        gCustomItemState.lightRodState
#define lightRodProjActive   gCustomItemState.lightRodProjActive
#define lightRodProjType     gCustomItemState.lightRodProjType
#define lightRodProjPos      gCustomItemState.lightRodProjPos
#define lightRodProjYaw      gCustomItemState.lightRodProjYaw
#define lightRodProjPitch    gCustomItemState.lightRodProjPitch
#define lightRodProjTimer    gCustomItemState.lightRodProjTimer
#define lightRodCollider     gCustomItemState.lightRodCollider
#define lightRodBlureIdx     gCustomItemState.lightRodBlureIdx

#define lightRodProjTrail    gCustomItemState.lightRodProjTrail
#define lightRodProjScale    gCustomItemState.lightRodProjScale
#define lightRodProjRotZ     gCustomItemState.lightRodProjRotZ
#define lightRodProjTrailIdx gCustomItemState.lightRodProjTrailIdx

#define lightRodCharging     gCustomItemState.lightRodCharging
#define lightRodChargeLevel  gCustomItemState.lightRodChargeLevel
#define lightRodChargeReady  gCustomItemState.lightRodChargeReady
#define lightRodChargeTimer  gCustomItemState.lightRodChargeTimer

#define lightRodSpinActive      gCustomItemState.lightRodSpinActive
#define lightRodSpinIsBig       gCustomItemState.lightRodSpinIsBig
#define lightRodSpinRadius      gCustomItemState.lightRodSpinRadius
#define lightRodSpinMaxRadius   gCustomItemState.lightRodSpinMaxRadius
#define lightRodSpinCollider    gCustomItemState.lightRodSpinCollider

#define lightRodFirstPerson     gCustomItemState.lightRodFirstPerson
#define lightRodButtonMask      gCustomItemState.lightRodButtonMask

// =============================================================================
// COLLIDER CONFIGS
// =============================================================================

static ColliderCylinderInit sLightRodProjColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_ARROW_LIGHT, 0x01, LIGHT_ROD_PROJ_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { LIGHT_ROD_PROJ_RADIUS, LIGHT_ROD_PROJ_HEIGHT, 0, { 0, 0, 0 } }
};

static ColliderCylinderInit sLightRodSpinColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_MAGIC_LIGHT | DMG_SLASH, 0x01, LIGHT_ROD_SPIN_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { 50, 80, 0, { 0, 0, 0 } }
};

static ColliderCylinderInit sLightRodBeamColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2,
      { DMG_ARROW_LIGHT, 0x01, LIGHT_ROD_BEAM_DAMAGE },
      { 0, 0, 0 },
      TOUCH_ON | TOUCH_SFX_NORMAL,
      BUMP_NONE,
      OCELEM_NONE },
    { LIGHT_ROD_BEAM_RADIUS, LIGHT_ROD_BEAM_HEIGHT, 0, { 0, 0, 0 } }
};

// =============================================================================
// FUNCTIONS
// =============================================================================

void Handle_LightRod(Player* player, PlayState* play);
void Player_InitLightRodIA(PlayState* play, Player* player);
void CustomItems_DrawLightRod(Player* player, PlayState* play);
void CustomItems_DrawLightRodReticle(Player* player, PlayState* play);

#endif

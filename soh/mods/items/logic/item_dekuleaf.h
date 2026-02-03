/**
 * Deku Leaf Item Header
 * Toggle item with dual functionality: gliding (air) and wind blow (ground)
 */

#ifndef ITEM_DEKULEAF_H
#define ITEM_DEKULEAF_H

#include "z64.h"
#include "../custom_items.h"

// Modes
#define DEKULEAF_MODE_INACTIVE     0
#define DEKULEAF_MODE_GLIDING      1
#define DEKULEAF_MODE_BLOWING      2

// Physics
#define DEKULEAF_FALL_VELOCITY     -1.5f

// Magic costs
#define DEKULEAF_GLIDE_MAGIC_INTERVAL  7
#define DEKULEAF_GLIDE_MAGIC_COST      1
#define DEKULEAF_BLOW_MAGIC_COST       3

// Blow effect
#define DEKULEAF_BLOW_RANGE        150.0f
#define DEKULEAF_BLOW_FORCE        20.0f
#define DEKULEAF_BLOW_DURATION     60
#define DEKULEAF_WIND_SPAWN_RATE   3

// =============================================================================
// Scale settings for Deku Leaf in hand
// =============================================================================
#define DEKULEAF_HOLD_SCALE        0.08f   // Small scale when held in hand
#define DEKULEAF_ATTACK_SCALE      0.25f   // Large scale during attack frames 10-22

// Frame ranges for scale transitions
#define DEKULEAF_ATTACK_FRAME_START  10
#define DEKULEAF_ATTACK_FRAME_END    22

// Animation timings (blow animation is now 39 frames from skeletal anim)
#define DEKULEAF_BLOW_ANIM_FRAMES  39

// Blow effect frame (when wind actually fires)
#define DEKULEAF_BLOW_EFFECT_FRAME 15

// Sound
#define DEKULEAF_SOUND_WIND        NA_SE_PL_MAGIC_WIND_NORMAL
#define DEKULEAF_SOUND_BLOW        NA_SE_EV_WIND_TRAP

// Animation reference
#define DEKULEAF_ANIM_GLIDE        gPlayerAnim_link_normal_carryB_wait

// State aliases
#define dlActive     gCustomItemState.dekuLeafActive
#define dlMode       gCustomItemState.dekuLeafMode
#define dlGliding    gCustomItemState.dekuLeafGliding
#define dlBlowing    gCustomItemState.dekuLeafBlowing
#define dlAnimTimer  gCustomItemState.dekuLeafAnimTimer
#define dlBlowTimer  gCustomItemState.dekuLeafBlowTimer

// =============================================================================
// Animation extern (defined in anim/deku_leaf/)
// =============================================================================
extern LinkAnimationHeader gDekuLeafBlowAnim;

// =============================================================================
// Functions
// =============================================================================
void Handle_DekuLeaf(Player* player, PlayState* play);
s32 Player_UpperAction_DekuLeaf(Player* player, PlayState* play);
void CustomItems_DrawDekuLeaf(Player* player, PlayState* play);

#endif // ITEM_DEKULEAF_H

/**
 * Mogma Mitts Item Header
 * Equip-based passive item: transforms all walls into climbable surfaces.
 * Drains 1 MP every other frame while active.
 */

#ifndef ITEM_MITTS_H
#define ITEM_MITTS_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// Constants
// =============================================================================
#define MITTS_MP_COST         1    // MP consumed per drain tick
#define MITTS_DRAIN_INTERVAL  10    // Drain every N frames (every other frame)

// =============================================================================
// State Aliases (mapped to gCustomItemState fields)
// =============================================================================
#define mmActive       gCustomItemState.mogmaMittsActive
#define mmDrainTick    gCustomItemState.mogmaMittsDrainTick

// =============================================================================
// Global climb flag (extern'd by z_bgcheck.c and z_player.c)
// Kept in sync with mmActive - separate global avoids coupling core engine
// files to the full custom_items.h header.
// =============================================================================
extern u8 gMogmaMittsClimbActive;

// =============================================================================
// Functions
// =============================================================================
void Handle_MogmaMitts(Player* player, PlayState* play);
s32 Player_UpperAction_MogmaMitts(Player* player, PlayState* play);
void Player_InitMogmaMittsIA(PlayState* play, Player* player);

#endif // ITEM_MITTS_H

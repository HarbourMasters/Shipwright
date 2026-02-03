/**
 * Cane of Somaria Item Header
 * Pattern based on Fire Rod
 */

#ifndef ITEM_CANE_OF_SOMARIA_H
#define ITEM_CANE_OF_SOMARIA_H

#include "z64.h"
#include "../custom_items.h"

// =============================================================================
// STATES (like Fire Rod)
// =============================================================================

#define SOMARIA_STATE_INACTIVE      0
#define SOMARIA_STATE_EQUIPPED      1
#define SOMARIA_STATE_CASTING       2

// =============================================================================
// SETTINGS
// =============================================================================

#define SOMARIA_ANIM_DURATION 30

// =============================================================================
// STATE ALIASES (like Fire Rod)
// =============================================================================

#define shSomariaActive     gCustomItemState.somariaActive
#define somariaState        gCustomItemState.somariaActionType
#define shSomariaAnimating  gCustomItemState.somariaAnimating
#define shSomariaAnimTimer  gCustomItemState.somariaAnimTimer

// Block state aliases
#define somariaBlocks       gCustomItemState.somariaBlocks
#define somariaBlockCount   gCustomItemState.somariaBlockCount
#define somariaOldestSlot   gCustomItemState.somariaOldestSlot
#define somariaButtonMask   gCustomItemState.somariaButtonMask

// =============================================================================
// DISPLAY LISTS (defined in object_cane_of_somaria.c)
// =============================================================================

extern Gfx* gCaneOfSomariaStaffDL;
extern Gfx* gCaneOfSomariaOrbDL;

// =============================================================================
// FUNCTIONS
// =============================================================================

void Handle_CaneOfSomaria(Player* player, PlayState* play);
void Player_InitCaneOfSomariaIA(PlayState* play, Player* player);
void CustomItems_DrawCaneOfSomaria(Player* player, PlayState* play);

#endif // ITEM_CANE_OF_SOMARIA_H

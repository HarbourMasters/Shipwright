/**
 * Roc's Cape Item Header
 * High jump + double jump - can use in water with reduced force
 *
 * Part of Progressive Roc's system:
 * - Base item: Roc's Feather (single jump)
 * - Upgrade: Roc's Cape (single jump + double jump)
 * - Both items share SLOT_ROCS (slot 24)
 * - Both usable by Adult and Child (AGE_REQ_NONE)
 */

#ifndef ITEM_ROCSCAPE_H
#define ITEM_ROCSCAPE_H

#include "z64.h"
#include "../custom_items.h"

// Jump velocities
#define ROCSCAPE_JUMP_VELOCITY          11.0f
#define ROCSCAPE_DOUBLE_JUMP_VELOCITY   11.0f
#define ROCSCAPE_WATER_JUMP_VELOCITY    5.5f

// Effects
#define ROCSCAPE_SHOCKWAVE_Y_OFFSET     10.0f

// Sound
#define ROCSCAPE_SOUND_JUMP    NA_SE_VO_LI_AUTO_JUMP_KID
#define ROCSCAPE_SOUND_DOUBLE  NA_SE_VO_LI_AUTO_JUMP_KID

// State alias
#define rcJumpCount  gCustomItemState.rocsJumpCount

#endif // ITEM_ROCSCAPE_H

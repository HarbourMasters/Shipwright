/**
 * item_mitts.c - Mogma Mitts from Skyward Sword
 *
 * Controls:
 *   C Button: Toggle climb mode on/off
 *
 * Features:
 *   - All walls become climbable while active
 *   - Consumes magic over time (1 MP per interval)
 *   - Auto-deactivates when magic depleted
 *   - Re-press to reactivate when magic available
 */

#include "z64.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/movement_helper.h"
#include "macros.h"
#include "functions.h"
#include "item_mitts.h"
//
// Visual: Link holds white gauntlets while this item is active.
// ============================================================================

// Global flag - accessed by z_bgcheck.c and z_player.c via extern
u8 gMogmaMittsClimbActive = 0;

// Equip state for ItemEquip_Update callbacks
static ItemEquipState sMittsEquipState = { 0 };
static s8 sMittsPrevInvinc = 0;

// ============================================================================
// Activate / Deactivate
// ============================================================================

static void Mitts_Activate(Player* p, PlayState* play) {
    mmActive = 1;
    mmDrainTick = 0;
    gMogmaMittsClimbActive = 1;
    Audio_PlayActorSound2(&p->actor, NA_SE_SY_LOCK_ON);
}

static void Mitts_Deactivate(void) {
    mmActive = 0;
    mmDrainTick = 0;
    gMogmaMittsClimbActive = 0;
}

// ============================================================================
// Equip / Unequip Callbacks (called by ItemEquip_Update)
// ============================================================================

static void Mitts_OnEquip(PlayState* play, Player* p) {
    // Activate immediately on equip if player has MP
    if (ItemMagic_HasEnough(play, MITTS_MP_COST)) {
        Mitts_Activate(p, play);
    }
    ItemEquip_PlayEquipSFX(play, p);
}

static void Mitts_OnUnequip(PlayState* play, Player* p) {
    Mitts_Deactivate();
    ItemEquip_PlayUnequipSFX(play, p);
}

// ============================================================================
// Main Handler
// ============================================================================

void Handle_MogmaMitts(Player* this, PlayState* play) {
    ItemInputState input;
    ItemInput_Update(&input, ITEM_MOGMA_MITTS, this, play);

    // Not equipped - cleanup
    if (!input.wasEquipped) {
        if (mmActive) {
            Mitts_Deactivate();
        }
        sMittsEquipState.isEquipped = 0;
        return;
    }

    // Blocking check
    if (!mmActive) {
        if (ItemInput_IsBlockedEx(this, play, 1)) return;
    } else {
        u32 criticalBlocks = (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE |
                              PLAYER_STATE1_LOADING | PLAYER_STATE1_IN_ITEM_CS |
                              PLAYER_STATE1_TALKING | PLAYER_STATE1_GETTING_ITEM |
                              PLAYER_STATE1_ON_HORSE);
        if (this->stateFlags1 & criticalBlocks) {
            return;
        }
    }

    // Damage check
    if (ItemInput_CheckDamage(this, &sMittsPrevInvinc)) {
        if (mmActive) {
            Mitts_Deactivate();
        }
        sMittsEquipState.isEquipped = 0;
        return;
    }

    // Equip state management with callbacks
    ItemEquip_Update(&sMittsEquipState, &input, Mitts_OnEquip, Mitts_OnUnequip, this, play);

    // Button press: toggle on/off
    if (input.isPressed) {
        if (mmActive) {
            // Toggle off
            Mitts_Deactivate();
            Audio_PlayActorSound2(&this->actor, NA_SE_SY_CANCEL);
            return;
        } else {
            // Try to activate
            if (ItemMagic_HasEnough(play, MITTS_MP_COST)) {
                Mitts_Activate(this, play);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_SY_ERROR);
                return;
            }
        }
    }

    if (!mmActive) return;

    // ========================================================================
    // MP Drain - 1 MP every MITTS_DRAIN_INTERVAL frames
    // ========================================================================
    mmDrainTick++;
    if (mmDrainTick >= MITTS_DRAIN_INTERVAL) {
        mmDrainTick = 0;

        if (!ItemMagic_HasEnough(play, MITTS_MP_COST)) {
            // Out of MP - deactivate climb effect
            Mitts_Deactivate();
            Audio_PlayActorSound2(&this->actor, NA_SE_SY_ERROR);
            return;
        }

        ItemMagic_Consume(play, MITTS_MP_COST);
    }

    // Keep climb flag synchronized
    gMogmaMittsClimbActive = 1;
}

// ============================================================================
// Init & Upper Action (passive item - no animation override)
// ============================================================================

void Player_InitMogmaMittsIA(PlayState* play, Player* this) {
    // No special init needed for passive equip item
}

s32 Player_UpperAction_MogmaMitts(Player* this, PlayState* play) {
    return 0;  // Passive item - no upper body animation override
}

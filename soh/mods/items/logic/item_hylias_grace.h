/**
 * Hylia's Grace Item Header
 * Fairy transformation spell - costs 24 MP, transforms Link into a fairy for 10 seconds
 *
 * Sequence: Farore's Wind cast -> blue warp enter -> fairy flight -> warp exit
 * A=ascend, B=descend, L=sprint, stick=direction
 * Ascending or sprinting drains timer 2x faster
 * 1 minute cooldown after use
 */

#ifndef ITEM_HYLIAS_GRACE_H
#define ITEM_HYLIAS_GRACE_H

#include "z64.h"
#include "../custom_items.h"

// States
#define HGRACE_STATE_IDLE       0
#define HGRACE_STATE_CASTING    1   // Farore's Wind animation (kaze1 -> kaze2 -> kaze3)
#define HGRACE_STATE_WARP_IN    2   // Blue warp transition
#define HGRACE_STATE_FAIRY      3   // Fairy flight mode
#define HGRACE_STATE_WARP_OUT   4   // Reverse warp, Link reappears

// Casting sub-phases (Farore's Wind 3-part animation)
#define HGRACE_CAST_KAZE1   0
#define HGRACE_CAST_KAZE2   1
#define HGRACE_CAST_KAZE3   2

// Magic
#define HGRACE_MAGIC_COST       24

// Fairy mode
#define HGRACE_FAIRY_DURATION   600     // 10 seconds at 20fps
#define HGRACE_FLICKER_START    90      // Last 1.5 seconds = flicker
#define HGRACE_SPEED            5.0f
#define HGRACE_SPRINT_MULT      2.0f
#define HGRACE_COOLDOWN         1200    // 1 minute at 20fps

// Warp timing
#define HGRACE_WARP_IN_DURATION  40
#define HGRACE_WARP_OUT_DURATION 30

// Collision
#define HGRACE_FAIRY_HOVER      5.0f    // Minimum hover height above floor

// DynaPoly actors that BLOCK fairy passage (doors / solid shutters).
// All other DynaPoly actors (bars, grates, bombable walls, fake walls, etc.)
// are passable — the fairy slips through them.
// Scene geometry always blocks regardless of this list.
// Note: En_Door (standard room doors) uses OC, not DynaPoly.
static const u16 sHGraceDoorActors[] = {
    ACTOR_DOOR_SHUTTER,      // 0x002E  Dungeon shutter door (solid metal)
    ACTOR_DOOR_TOKI,         // 0x0070  Door of Time
    ACTOR_DOOR_GERUDO,       // 0x0172  Gerudo cell door
    ACTOR_DOOR_KILLER,       // 0x01C1  Fake/killer door (solid, spikes)
    ACTOR_BG_HAKA_HUTA,      // 0x00BD  Shadow Temple tombstone lid
    ACTOR_BG_MIZU_SHUTTER,   // 0x01BB  Water Temple shutter
    ACTOR_BG_SPOT18_FUTA,    // 0x01C3  Goron City shop lid
    ACTOR_BG_SPOT18_SHUTTER, // 0x01C4  Goron City shutter
    ACTOR_BG_INGATE,         // 0x0140  Lon Lon Ranch entrance gate
};

// State aliases
#define hgActive    gCustomItemState.hyliasGraceActive
#define hgState     gCustomItemState.hyliasGraceState
#define hgSubPhase  gCustomItemState.hyliasGraceSubPhase
#define hgTimer     gCustomItemState.hyliasGraceTimer
#define hgCooldown  gCustomItemState.hyliasGraceCooldown
#define hgFairy     gCustomItemState.hyliasGraceFairy

#endif // ITEM_HYLIAS_GRACE_H

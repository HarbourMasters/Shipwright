#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
#include "overlays/actors/ovl_En_Syateki_Okuta/z_en_syateki_okuta.h"
void EnSyatekiMan_Town_RunGame(EnSyatekiMan* enSyatekiMan, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Minigames.RandomizeShootingGalleryOctoroks"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// This defines the configuration of a single wave of Octoroks in the Town Shooting Gallery. These are
// arranged in the same columns and rows that appear in-game; the player stands in the center column, and
// the Octorok with an index of 7 appears directly in front of them like so:
// 0     1     2
// 3     4     5
// 6     7     8
//     Player
// Note: OCTO_FLAGS is redefined here because the original macro in z_en_syateki_man.c is
// not exposed in the header.
#define OCTO_FLAGS(type0, type1, type2, type3, type4, type5, type6, type7, type8)           \
    (SG_OCTO_SET_FLAG(type0, 0) | SG_OCTO_SET_FLAG(type1, 1) | SG_OCTO_SET_FLAG(type2, 2) | \
     SG_OCTO_SET_FLAG(type3, 3) | SG_OCTO_SET_FLAG(type4, 4) | SG_OCTO_SET_FLAG(type5, 5) | \
     SG_OCTO_SET_FLAG(type6, 6) | SG_OCTO_SET_FLAG(type7, 7) | SG_OCTO_SET_FLAG(type8, 8))

/**
 * In the Town Shooting Gallery, there are fifteen waves of Octoroks.
 * For each wave, these flags are used to control which Octoroks appear.
 *
 * Note: This array is redefined here because the original sOctorokFlagsPerWave in
 * z_en_syateki_man.c is static and cannot be accessed externally.
 */
static const s32 sOctorokFlagsPerWave[] = {
    // clang-format off
    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_NONE),
    // clang-format on
};

static void ShuffleWaveOrder(s16* shuffledWaveIndices, int count) {
    for (s32 i = 0; i < count; i++) {
        shuffledWaveIndices[i] = i;
    }
    for (s32 i = count - 1; i > 0; i--) {
        s32 j = (s32)(Rand_ZeroOne() * (i + 1));
        if (j > i) {
            j = i;
        }
        s32 temp = shuffledWaveIndices[i];
        shuffledWaveIndices[i] = shuffledWaveIndices[j];
        shuffledWaveIndices[j] = temp;
    }
}

static s32 GetRandomizedWaveFlags(s32 originalFlags) {
    // Extract octorok types from vanilla flags
    s16 octorokTypes[9];
    s32 octorokCount = 0;
    for (s32 i = 0; i < 9; i++) {
        s16 type = SG_OCTO_GET_TYPE(originalFlags, i);
        if (type != SG_OCTO_TYPE_NONE) {
            octorokTypes[octorokCount++] = type;
        }
    }

    if (octorokCount == 0) {
        return originalFlags;
    }

    // Shuffle the positions 0-8
    s32 positions[9];
    for (s32 i = 0; i < 9; i++) {
        positions[i] = i;
    }
    for (s32 i = 8; i > 0; i--) {
        s32 j = (s32)(Rand_ZeroOne() * (i + 1));
        if (j > i) {
            j = i;
        }
        s32 temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }

    // Rebuild grid with new positions
    s16 grid[9];
    for (s32 i = 0; i < 9; i++) {
        grid[i] = SG_OCTO_TYPE_NONE;
    }

    for (s32 i = 0; i < octorokCount; i++) {
        grid[positions[i]] = octorokTypes[i];
    }

    // Check Front Center (7) for Blue/Red swap logic to avoid collision/unfairness
    // Grid indices:
    // 0 1 2 (Back)
    // 3 4 5 (Middle)
    // 6 7 8 (Front)
    // Center column is 1, 4, 7.
    if (grid[7] == SG_OCTO_TYPE_BLUE) {
        if (grid[4] == SG_OCTO_TYPE_RED) {
            // Swap 7 and 4
            grid[7] = SG_OCTO_TYPE_RED;
            grid[4] = SG_OCTO_TYPE_BLUE;
        } else if (grid[1] == SG_OCTO_TYPE_RED) {
            // Swap 7 and 1
            grid[7] = SG_OCTO_TYPE_RED;
            grid[1] = SG_OCTO_TYPE_BLUE;
        }
    }

    // Check Mid Center (4) - Re-check in case we just swapped a Blue here, or if it was already Blue
    if (grid[4] == SG_OCTO_TYPE_BLUE) {
        if (grid[1] == SG_OCTO_TYPE_RED) {
            // Swap 4 and 1
            grid[4] = SG_OCTO_TYPE_RED;
            grid[1] = SG_OCTO_TYPE_BLUE;
        }
    }

    s32 newFlags = 0;
    for (s32 i = 0; i < 9; i++) {
        if (grid[i] != SG_OCTO_TYPE_NONE) {
            newFlags |= SG_OCTO_SET_FLAG(grid[i], i);
        }
    }

    return newFlags;
}

void RegisterRandomizeShootingGalleryOctoroks() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_SYATEKI_MAN, CVAR, [](Actor* actor) {
        EnSyatekiMan* enSyatekiMan = (EnSyatekiMan*)actor;

        if (enSyatekiMan->actionFunc != EnSyatekiMan_Town_RunGame) {
            return;
        }

        if (enSyatekiMan->shootingGameState != SG_GAME_STATE_RUNNING) {
            return;
        }

        if (enSyatekiMan->octorokState != SG_OCTO_STATE_APPEARING) {
            return;
        }

        static s16 sShuffledWaveIndices[ARRAY_COUNT(sOctorokFlagsPerWave)];

        // Shuffle wave order at the start of the game
        if (enSyatekiMan->flagsIndex == 1) {
            ShuffleWaveOrder(sShuffledWaveIndices, ARRAY_COUNT(sOctorokFlagsPerWave));
        }

        // Apply shuffled wave
        // flagsIndex is 1-based here because it was incremented in RunGame before this hook
        if (enSyatekiMan->flagsIndex > 0 && enSyatekiMan->flagsIndex <= ARRAY_COUNT(sOctorokFlagsPerWave)) {
            s32 originalFlags = sOctorokFlagsPerWave[sShuffledWaveIndices[enSyatekiMan->flagsIndex - 1]];
            enSyatekiMan->octorokFlags = GetRandomizedWaveFlags(originalFlags);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterRandomizeShootingGalleryOctoroks, { CVAR_NAME });

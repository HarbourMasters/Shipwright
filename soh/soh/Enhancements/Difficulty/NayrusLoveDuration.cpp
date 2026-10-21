#include <algorithm>
#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

static constexpr int32_t CVAR_NAYRUS_LOVE_DURATION_DEFAULT = 60;
#define CVAR_NAYRUS_LOVE_DURATION_NAME CVAR_ENHANCEMENT("NayrusLoveDuration")
#define CVAR_NAYRUS_LOVE_DURATION_VALUE \
    CVarGetInteger(CVAR_NAYRUS_LOVE_DURATION_NAME, CVAR_NAYRUS_LOVE_DURATION_DEFAULT)
#define CVAR_NAYRUS_LOVE_DURATION_SET (CVAR_NAYRUS_LOVE_DURATION_VALUE != CVAR_NAYRUS_LOVE_DURATION_DEFAULT)
#define CVAR_INFINITE_NAYRU_NAME CVAR_CHEAT("InfiniteNayru")
#define CVAR_INFINITE_NAYRU_VALUE CVarGetInteger(CVAR_INFINITE_NAYRU_NAME, 0)

static constexpr int32_t NAYRUS_LOVE_FRAMES_PER_SECOND = 20;
static constexpr int32_t NAYRUS_LOVE_VANILLA_END_FRAMES = 1200;
static constexpr int32_t NAYRUS_LOVE_FLASH_FRAMES = 100;
static constexpr int32_t NAYRUS_LOVE_VANILLA_FLASH_START_FRAMES =
    NAYRUS_LOVE_VANILLA_END_FRAMES - NAYRUS_LOVE_FLASH_FRAMES;

// Update frames of the running effect, used to derive the virtual nayrusLoveTimer written back after each update
static int32_t sElapsedFrames = 0;

static void RegisterNayrusLoveDuration() {
    // The flicker only happens while nayrusLoveTimer is 1100-1199 and ticking by 1, and 1200+ ends the effect
    COND_ID_HOOK(OnActorUpdate, ACTOR_MAGIC_DARK, CVAR_NAYRUS_LOVE_DURATION_SET, [](void*) {
        int16_t timer = gSaveContext.nayrusLoveTimer;

        if (timer == 0) {
            sElapsedFrames = 0;
            return;
        }

        // Larger values are not a running effect, e.g. game over sets 2000 to have the actor end the effect
        if (timer > NAYRUS_LOVE_VANILLA_END_FRAMES || CVAR_INFINITE_NAYRU_VALUE) {
            return;
        }

        sElapsedFrames++;

        const int32_t endFrames = CVAR_NAYRUS_LOVE_DURATION_VALUE * NAYRUS_LOVE_FRAMES_PER_SECOND;
        const int32_t flashStartFrames = endFrames - NAYRUS_LOVE_FLASH_FRAMES;

        if (sElapsedFrames < flashStartFrames) {
            gSaveContext.nayrusLoveTimer = std::min(sElapsedFrames, NAYRUS_LOVE_VANILLA_FLASH_START_FRAMES - 1);
        } else {
            gSaveContext.nayrusLoveTimer = sElapsedFrames - flashStartFrames + NAYRUS_LOVE_VANILLA_FLASH_START_FRAMES;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterNayrusLoveDuration, { CVAR_NAYRUS_LOVE_DURATION_NAME });

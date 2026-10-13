#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Invadepoh/z_en_invadepoh.h"

void EnInvadepoh_InvasionHandler_SuccessCutscene(EnInvadepoh* enInvadepoh, PlayState* play);
void EnInvadepoh_InvasionHandler_SuccessEnd(EnInvadepoh* enInvadepoh, PlayState* play);

s32 EnInvadepoh_Alien_GetSpawnTime(s32 index);
void EnInvadepoh_Alien_SetSpawnTime(s32 index, s32 spawnTime);
}

#define CVAR_SKIP_NAME "gEnhancements.Minigames.SkipRanchInvasion"
#define CVAR_SKIP CVarGetInteger(CVAR_SKIP_NAME, 0)

static constexpr f32 CVAR_SPEED_DEFAULT = 1.0f;
#define CVAR_SPEED_NAME "gEnhancements.Minigames.AlienSpeed"
#define CVAR_SPEED CVarGetFloat(CVAR_SPEED_NAME, CVAR_SPEED_DEFAULT)

static constexpr s32 ALIEN_COUNT = 8;

// Needed to prevent debug warp from always skipping to 5:15 AM
static bool sInvasionSkipped = false;

static f32 sPrevAlienSpeed = CVAR_SPEED_DEFAULT;

// Used for lerp adjustments; see `AdjustAlienSpawnTimes`
static s32 sSpawnTimeOffsets[ALIEN_COUNT];

static constexpr s32 TIME_LIMIT = 80 * CLOCK_TIME_MINUTE;

static void EnInvadepoh_InvasionHandler_SkipToReward(Actor* actor, bool* should) {
    if ((CURRENT_DAY != 1) || (CURRENT_TIME >= CLOCK_TIME(5, 15)) || (CURRENT_TIME < CLOCK_TIME(2, 30)) ||
        (EN_INVADEPOH_GET_TYPE(actor) != EN_INVADEPOH_TYPE_INVASION_HANDLER)) {
        return;
    }

    EnInvadepoh* enInvadepoh = (EnInvadepoh*)actor;
    if (enInvadepoh->actionFunc == EnInvadepoh_InvasionHandler_SuccessEnd) {
        return;
    }

    EnInvadepoh_InvasionHandler_SuccessCutscene(enInvadepoh, gPlayState);
    SET_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS);
    sInvasionSkipped = true;
    *should = false;
}

static void AdvanceToEnd(s16 sceneId, s8 spawnNum) {
    if (sInvasionSkipped) {
        gSaveContext.save.time = CLOCK_TIME(5, 15);
        sInvasionSkipped = false;
    }
}

static void EnInvadepoh_Alien_AdjustSpawnTime(s32* spawnTime, s32 index, f32 speed) {
    s32 currentTime = CURRENT_TIME;
    *spawnTime = currentTime + (*spawnTime - sSpawnTimeOffsets[index] - currentTime) * speed;
}

// Reset each alien's spawn time offset when they are despawned
static void EnInvadepoh_Alien_ResetOffset(Actor* actor) {
    if (EN_INVADEPOH_GET_TYPE(actor) != EN_INVADEPOH_TYPE_ALIEN) {
        return;
    }

    EnInvadepoh* alien = (EnInvadepoh*)actor;
    if (alien->shouldDraw) {
        return;
    }

    s32 index = EN_INVADEPOH_GET_INDEX(actor);
    sSpawnTimeOffsets[index] = 0;
}

// If the speed setting is changed during the minigame, we want the aliens to stay where they are and not shift around
// along the path because of the affected lerp, so we have to adjust their spawn times.
static void AdjustAlienSpawnTimes() {
    s32 i, spawnTime;
    s32 currentTime = CURRENT_TIME;

    // If invasion is skipped, we don't care; if it's the wrong time or day, do nothing
    if (CVAR_SKIP || (CURRENT_DAY != 1) || (currentTime >= CLOCK_TIME(5, 15)) || (currentTime < CLOCK_TIME(2, 30))) {
        return;
    }

    // Check if speed setting has changed significantly beyond a small rounding error
    if (ABS(sPrevAlienSpeed - CVAR_SPEED) < 0.05f) {
        return;
    }

    f32 speedRatio = sPrevAlienSpeed / CVAR_SPEED;

    for (i = 0; i < ALIEN_COUNT; i++) {
        spawnTime = EnInvadepoh_Alien_GetSpawnTime(i);

        // Skip aliens that are waiting to respawn later
        if (spawnTime >= currentTime) {
            continue;
        }

        // Use lerp to compute new spawn time
        EnInvadepoh_Alien_AdjustSpawnTime(&spawnTime, i, speedRatio);
        EnInvadepoh_Alien_SetSpawnTime(i, spawnTime);

        // Since the setter clamps the time to be no earlier than 2:30 AM, store the difference to adjust
        sSpawnTimeOffsets[i] = EnInvadepoh_Alien_GetSpawnTime(i) - spawnTime;
    }
}

static void RegisterRanchInvasionSkipHooks() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_INVADEPOH, CVAR_SKIP, EnInvadepoh_InvasionHandler_SkipToReward);

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_INVADEPOH, CVAR_SKIP, EnInvadepoh_InvasionHandler_SkipToReward);

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_INVADEPOH, !CVAR_SKIP, EnInvadepoh_Alien_ResetOffset);

    COND_ID_HOOK(OnSceneInit, SCENE_F01, CVAR_SKIP, AdvanceToEnd);
}

static void RegisterRanchInvasionSpeedHooks() {
    AdjustAlienSpawnTimes();
    sPrevAlienSpeed = CVAR_SPEED;

    COND_VB_SHOULD(VB_SET_ALIEN_SPEED, !CVAR_SKIP && (CVAR_SPEED != CVAR_SPEED_DEFAULT), {
        s32* spawnTime = va_arg(args, s32*);
        s32 index = va_arg(args, s32);
        s32 currentTime = CURRENT_TIME;

        EnInvadepoh_Alien_AdjustSpawnTime(spawnTime, index, CVAR_SPEED);
    });
}

static RegisterShipInitFunc initFunc_Skip(RegisterRanchInvasionSkipHooks, { CVAR_SKIP_NAME });
static RegisterShipInitFunc initFunc_Speed(RegisterRanchInvasionSpeedHooks, { CVAR_SKIP_NAME, CVAR_SPEED_NAME });

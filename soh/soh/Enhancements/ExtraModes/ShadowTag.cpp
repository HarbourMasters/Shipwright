#include <libultraship/bridge.h>
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"

extern "C" {
extern PlayState* gPlayState;
#include "functions.h"
#include "macros.h"
}

#define CVAR_SHADOW_TAG_NAME CVAR_ENHANCEMENT("ShadowTag")
#define CVAR_SHADOW_TAG_DEFAULT 0
#define CVAR_SHADOW_TAG_VALUE CVarGetInteger(CVAR_SHADOW_TAG_NAME, CVAR_SHADOW_TAG_DEFAULT)

static bool shouldSpawn = false;
static u16 delayTimer = 60;

void OnPlayerUpdateShadowTag() {
    if (
        gPlayState->sceneNum == SCENE_FOREST_TEMPLE &&
        (
            gPlayState->roomCtx.curRoom.num == 16 || // Green Poe Room
            gPlayState->roomCtx.curRoom.num == 13 || // Blue Poe Room
            gPlayState->roomCtx.curRoom.num == 12    // Red Poe Room
        )
    ) {
        return;
    }

    if (shouldSpawn && delayTimer <= 0) {
        Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_WALLMAS, 0, 0, 0, 0, 0, 0, 3, false);
        shouldSpawn = false;
    } else {
        delayTimer--;
    }
}

void OnSceneSpawnActorsShadowTag() {
    shouldSpawn = true;
    delayTimer = 60;
}

void OnSceneInitShadowTag(s16 sceneNum) {
    shouldSpawn = true;
    delayTimer = 60;
}

void RegisterShadowTag() {
    COND_HOOK(OnPlayerUpdate, CVAR_SHADOW_TAG_VALUE, OnPlayerUpdateShadowTag);
    COND_HOOK(OnSceneSpawnActors, CVAR_SHADOW_TAG_VALUE, OnSceneSpawnActorsShadowTag);
    COND_HOOK(OnSceneInit, CVAR_SHADOW_TAG_VALUE, OnSceneInitShadowTag);
}

static RegisterShipInitFunc initFunc(RegisterShadowTag, { CVAR_SHADOW_TAG_NAME });

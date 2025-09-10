#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"

extern "C" PlayState* gPlayState;

static constexpr int32_t CVAR_SHADOW_TAG_DEFAULT = 0;
#define CVAR_SHADOW_TAG_NAME CVAR_ENHANCEMENT("ShadowTag")
#define CVAR_SHADOW_TAG_VALUE CVarGetInteger(CVAR_SHADOW_TAG_NAME, CVAR_SHADOW_TAG_DEFAULT)

static bool shouldSpawn = false;
static uint16_t delayTimer = 60;

static constexpr s8 ROOM_GREEN_POE = 16;
static constexpr s8 ROOM_BLUE_POE = 13;
static constexpr s8 ROOM_RED_POE = 12;

void OnPlayerUpdateShadowTag() {
    if (gPlayState->sceneNum == SCENE_FOREST_TEMPLE) {
        switch (gPlayState->roomCtx.curRoom.num) {
            case ROOM_GREEN_POE:
            case ROOM_BLUE_POE:
            case ROOM_RED_POE:
                return;
            default:
                break;
        }
    }

    if (shouldSpawn && (delayTimer <= 0)) {
        Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_WALLMAS, 0, 0, 0, 0, 0, 0, 3, false);
        shouldSpawn = false;
    } else {
        delayTimer--;
    }
}

void ResetShadowTagSpawnTimer() {
    shouldSpawn = true;
    delayTimer = 60;
}

void RegisterShadowTag() {
    COND_HOOK(OnPlayerUpdate, CVAR_SHADOW_TAG_VALUE, OnPlayerUpdateShadowTag);
    COND_HOOK(OnSceneSpawnActors, true, []() { ResetShadowTagSpawnTimer(); });
    COND_HOOK(OnSceneInit, true, [](int16_t) { ResetShadowTagSpawnTimer(); });
}

static RegisterShipInitFunc initFunc_ShadowTag(RegisterShadowTag, { CVAR_SHADOW_TAG_NAME });

#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"

#include <spdlog/spdlog.h>

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"

extern PlayState* gPlayState;
}

#define CVAR_DYNAMICSHADOW_NAME CVAR_ENHANCEMENT("DynamicShadow")
#define CVAR_DYNAMICSHADOW_DEFAULT 0
#define CVAR_DYNAMICSHADOW_VALUE CVarGetInteger(CVAR_DYNAMICSHADOW_NAME, CVAR_DYNAMICSHADOW_DEFAULT)

void RegisterDynamicShadow() {
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_STARTMODE_FUNC, CVAR_DYNAMICSHADOW_VALUE, {
        int32_t startMode = va_arg(args, int32_t);
        Player* player = GET_PLAYER(gPlayState);
        Actor* shadowActor;

        if ((player != nullptr) && (startMode != PLAYER_START_MODE_NOTHING)) {
            SPDLOG_DEBUG("Spawning Dynamic Shadow. sceneNum: {0:#x}", gPlayState->sceneNum);

            // Spawn as Link's child so the shadow doesn't get unloaded on room change.
            shadowActor = Actor_SpawnAsChild(&gPlayState->actorCtx, &player->actor, gPlayState, ACTOR_EN_SDA, 0, 0, 0,
                                             0, 0, 0, 0);

            // For whatever reason the dynamic shadow's category is ACTORCAT_BOSS.
            // We're changing it to ACTORCAT_ITEMACTION, the same category as Navi.
            Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, shadowActor, ACTORCAT_ITEMACTION);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDynamicShadow, { CVAR_DYNAMICSHADOW_NAME });

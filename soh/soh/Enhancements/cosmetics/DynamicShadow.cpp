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

static constexpr int32_t CVAR_DYNAMICSHADOW_DEFAULT = 0;
#define CVAR_DYNAMICSHADOW_NAME CVAR_ENHANCEMENT("DynamicShadow")
#define CVAR_DYNAMICSHADOW_VALUE CVarGetInteger(CVAR_DYNAMICSHADOW_NAME, CVAR_DYNAMICSHADOW_DEFAULT)

static void SpawnDynamicShadow() {
    if (gPlayState != nullptr) {
        Player* player = GET_PLAYER(gPlayState);

        if (player != nullptr) {
            Actor* shadowActor;

            // Spawn as Link's child so the shadow doesn't get unloaded on room change.
            shadowActor = Actor_SpawnAsChild(&gPlayState->actorCtx, &player->actor, gPlayState, ACTOR_EN_SDA,
                                             player->actor.world.pos.x, player->actor.world.pos.y,
                                             player->actor.world.pos.z, 0, 0, 0, 0);

            // For whatever reason the dynamic shadow's category is ACTORCAT_BOSS.
            // We're changing it to ACTORCAT_ITEMACTION, the same category as Navi.
            if (shadowActor != nullptr) {
                Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, shadowActor, ACTORCAT_ITEMACTION);
            }
        }
    }
}

static void UpdateDynamicShadow() {
    if (gPlayState != nullptr) {
        Player* player = GET_PLAYER(gPlayState);
        Actor* shadowActor;

        if (player != nullptr) {
            if (CVAR_DYNAMICSHADOW_VALUE) {
                SPDLOG_DEBUG("Dynamic Shadow has been toggled on. sceneNum: {0:#x}", gPlayState->sceneNum);

                shadowActor = Actor_Find(&gPlayState->actorCtx, ACTOR_EN_SDA, ACTORCAT_BOSS);
                if (shadowActor != nullptr) {
                    SPDLOG_DEBUG("Dynamic Shadow already exists in ACTORCAT_BOSS");
                } else {
                    shadowActor = Actor_Find(&gPlayState->actorCtx, ACTOR_EN_SDA, ACTORCAT_ITEMACTION);

                    if (shadowActor != nullptr) {
                        SPDLOG_DEBUG("Dynamic Shadow already exists in ACTORCAT_ITEMACTION");
                    } else {
                        SpawnDynamicShadow();
                    }
                }
            } else {
                SPDLOG_DEBUG("Dynamic Shadow has been toggled off. sceneNum: {0:#x}", gPlayState->sceneNum);

                shadowActor = Actor_Find(&gPlayState->actorCtx, ACTOR_EN_SDA, ACTORCAT_BOSS);
                if (shadowActor != nullptr) {
                    SPDLOG_DEBUG("Killing Dynamic Shadow in ACTORCAT_BOSS");
                    Actor_Kill(shadowActor);
                }

                shadowActor = Actor_Find(&gPlayState->actorCtx, ACTOR_EN_SDA, ACTORCAT_ITEMACTION);
                if (shadowActor != nullptr) {
                    SPDLOG_DEBUG("Killing Dynamic Shadow in ACTORCAT_ITEMACTION");
                    Actor_Kill(shadowActor);
                }

                // Restore player's normal shadow
                player->actor.shape.shadowAlpha = 255;
            }
        }
    }
}

static void RegisterDynamicShadow() {
    UpdateDynamicShadow();  // Handle Dynamic Shadow toggle

    COND_VB_SHOULD(VB_EXECUTE_PLAYER_STARTMODE_FUNC, CVAR_DYNAMICSHADOW_VALUE, {
        int32_t startMode = va_arg(args, int32_t);
        Player* player = GET_PLAYER(gPlayState);

        if ((player != nullptr) && (startMode != PLAYER_START_MODE_NOTHING)) {
            SPDLOG_DEBUG("Spawning Dynamic Shadow. sceneNum: {0:#x}", gPlayState->sceneNum);
            SpawnDynamicShadow();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDynamicShadow, { CVAR_DYNAMICSHADOW_NAME });

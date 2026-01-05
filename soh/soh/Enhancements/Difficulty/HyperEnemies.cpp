#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "macros.h"

extern "C" PlayState* gPlayState;

static constexpr int32_t CVAR_HYPER_ENEMIES_DEFAULT = 0;
#define CVAR_HYPER_ENEMIES_NAME CVAR_ENHANCEMENT("HyperEnemies")
#define CVAR_HYPER_ENEMIES_VALUE CVarGetInteger(CVAR_HYPER_ENEMIES_NAME, CVAR_HYPER_ENEMIES_DEFAULT)

static void MakeHyperEnemies(void* refActor) {
    // Run the update function a second time to make enemies and minibosses move and act twice as fast.

    Player* player = GET_PLAYER(gPlayState);
    Actor* actor = static_cast<Actor*>(refActor);

    // Some enemies are not in the ACTORCAT_ENEMY category, and some are that aren't really enemies.
    bool isEnemy = actor->category == ACTORCAT_ENEMY || actor->id == ACTOR_EN_TORCH2;
    bool isExcludedEnemy = actor->id == ACTOR_EN_FIRE_ROCK || actor->id == ACTOR_EN_ENCOUNT2;

    // Don't apply during cutscenes because it causes weird behaviour and/or crashes on some cutscenes.
    if (isEnemy && !isExcludedEnemy && !Player_InBlockingCsMode(gPlayState, player)) {
        GameInteractor::RawAction::UpdateActor(actor);
    }
}

static void UpdateHyperEnemiesState() {
    COND_HOOK(OnActorUpdate, CVAR_HYPER_ENEMIES_VALUE, MakeHyperEnemies);
}

static RegisterShipInitFunc initFunc(UpdateHyperEnemiesState, { CVAR_HYPER_ENEMIES_NAME });

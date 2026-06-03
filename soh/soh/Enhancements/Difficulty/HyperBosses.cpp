#include <libultraship/bridge.h>
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/boss-rush/BossRush.h"

extern "C" PlayState* gPlayState;

#define CVAR_HYPER_BOSSES_DEFAULT 0
#define CVAR_HYPER_BOSSES_NAME CVAR_ENHANCEMENT("HyperBosses")
#define CVAR_HYPER_BOSSES_VALUE CVarGetInteger(CVAR_HYPER_BOSSES_NAME, CVAR_HYPER_BOSSES_DEFAULT)

bool IsHyperBossesActive() {
    return CVAR_HYPER_BOSSES_VALUE ||
           (IS_BOSS_RUSH &&
            gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_HYPERBOSSES] == BR_CHOICE_HYPERBOSSES_YES);
}

void MakeHyperBosses(void* refActor) {
    // Run the update function a second time to make bosses move and act twice as fast.

    Player* player = GET_PLAYER(gPlayState);
    Actor* actor = static_cast<Actor*>(refActor);

    uint8_t isBossActor = actor->id == ACTOR_BOSS_GOMA ||      // Gohma
                          actor->id == ACTOR_BOSS_DODONGO ||   // King Dodongo
                          actor->id == ACTOR_EN_BDFIRE ||      // King Dodongo Fire Breath
                          actor->id == ACTOR_BOSS_VA ||        // Barinade
                          actor->id == ACTOR_BOSS_GANONDROF || // Phantom Ganon
                          actor->id == ACTOR_EN_FHG_FIRE ||    // Phantom Ganon/Ganondorf Energy Ball/Thunder
                          actor->id == ACTOR_EN_FHG ||         // Phantom Ganon's Horse
                          actor->id == ACTOR_BOSS_FD || actor->id == ACTOR_BOSS_FD2 || // Volvagia (grounded/flying)
                          actor->id == ACTOR_EN_VB_BALL ||                             // Volvagia Rocks
                          actor->id == ACTOR_BOSS_MO ||                                // Morpha
                          actor->id == ACTOR_BOSS_SST ||                               // Bongo Bongo
                          actor->id == ACTOR_BOSS_TW ||                                // Twinrova
                          actor->id == ACTOR_BOSS_GANON ||                             // Ganondorf
                          actor->id == ACTOR_BOSS_GANON2;                              // Ganon

    // Don't apply during cutscenes because it causes weird behaviour and/or crashes on some bosses.
    if (IsHyperBossesActive() && isBossActor && !Player_InBlockingCsMode(gPlayState, player)) {
        // Barinade needs to be updated in sequence to avoid unintended behaviour.
        if (actor->id == ACTOR_BOSS_VA) {
            // params -1 is BOSSVA_BODY
            if (actor->params == -1) {
                Actor* actorList = gPlayState->actorCtx.actorLists[ACTORCAT_BOSS].head;
                while (actorList != NULL) {
                    GameInteractor::RawAction::UpdateActor(actorList);
                    actorList = actorList->next;
                }
            }
        } else {
            GameInteractor::RawAction::UpdateActor(actor);
        }
    }
}

static void UpdateHyperBossesState() {
    COND_HOOK(OnActorUpdate, IsHyperBossesActive(), MakeHyperBosses);
}

static RegisterShipInitFunc initFunc(UpdateHyperBossesState, { CVAR_HYPER_BOSSES_NAME });
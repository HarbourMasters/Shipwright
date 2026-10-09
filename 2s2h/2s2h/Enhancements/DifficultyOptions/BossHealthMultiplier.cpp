#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include <unordered_map>

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.BossHealthMultiplier"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static std::unordered_map<Actor*, int> sLastBossHealth;

// Array mapping the UI combo box selection (0-4) directly to its corresponding float multiplier.
static const float sMultipliers[] = { 1.0f, 1.25f, 1.50f, 1.75f, 2.0f };

void HandleBossHealthMultiplier(Actor* actor) {
    if (gPlayState != NULL && Player_InBlockingCsMode(gPlayState, GET_PLAYER(gPlayState))) {
        return;
    }

    int currentHealth = actor->colChkInfo.health;

    if (currentHealth > 0) {

        if (sLastBossHealth.count(actor) == 0 || currentHealth > sLastBossHealth[actor]) {

            int option = CVAR;
            // Safety boundary check to prevent out-of-bounds array access if the CVAR contains an invalid value.
            if (option < 0 || option > 4)
                option = 0;
            float multiplier = sMultipliers[option];

            currentHealth = (s8)(currentHealth * multiplier);
            actor->colChkInfo.health = currentHealth;
        }

        sLastBossHealth[actor] = currentHealth;
    }
}

// Hooked to OnActorDestroy to automatically erase the boss from memory when the scene unloads.
void ClearBossHealthMultiplier(Actor* actor) {
    sLastBossHealth.erase(actor);
}

// Registers conditional, ID-specific hooks for both updating and destroying bosses.
// This optimizes performance by executing logic only for these 5 specific boss actors
// when the multiplier is active (CVAR > 0), and ensures clean memory management on unload.
void RegisterBossHealthMultiplier() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_BOSS_01, CVAR > 0, HandleBossHealthMultiplier);
    COND_ID_HOOK(OnActorUpdate, ACTOR_BOSS_HAKUGIN, CVAR > 0, HandleBossHealthMultiplier);
    COND_ID_HOOK(OnActorUpdate, ACTOR_BOSS_03, CVAR > 0, HandleBossHealthMultiplier);
    COND_ID_HOOK(OnActorUpdate, ACTOR_BOSS_02, CVAR > 0, HandleBossHealthMultiplier);
    COND_ID_HOOK(OnActorUpdate, ACTOR_BOSS_07, CVAR > 0, HandleBossHealthMultiplier);

    COND_ID_HOOK(OnActorDestroy, ACTOR_BOSS_01, true, ClearBossHealthMultiplier);
    COND_ID_HOOK(OnActorDestroy, ACTOR_BOSS_HAKUGIN, true, ClearBossHealthMultiplier);
    COND_ID_HOOK(OnActorDestroy, ACTOR_BOSS_03, true, ClearBossHealthMultiplier);
    COND_ID_HOOK(OnActorDestroy, ACTOR_BOSS_02, true, ClearBossHealthMultiplier);
    COND_ID_HOOK(OnActorDestroy, ACTOR_BOSS_07, true, ClearBossHealthMultiplier);
}

static RegisterShipInitFunc initFunc(RegisterBossHealthMultiplier, { CVAR_NAME });
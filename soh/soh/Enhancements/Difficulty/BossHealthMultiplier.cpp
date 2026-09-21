#include <libultraship/bridge/consolevariablebridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <unordered_map>

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_Boss_Dodongo/z_boss_dodongo.h"
}

#define CVAR_NAME "gEnhancements.BossHealthMultiplier"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Tracks the last recorded health to detect when a boss is initialized or resets
static std::unordered_map<Actor*, int> sLastBossHealth;

// Combobox index to multiplier value mapping (0: 1.0x, 1: 1.25x, 2: 1.50x, 3: 1.75x, 4: 2.0x)
static const float sMultipliers[] = { 1.0f, 1.25f, 1.50f, 1.75f, 2.0f };

/*
 * Retrieves the current health of a boss actor.
 * King Dodongo does not use colChkInfo.health and instead stores it in a custom struct field.
 */
static s16 GetBossHealth(Actor* actor) {
    if (actor->id == ACTOR_BOSS_DODONGO) {
        BossDodongo* dodongo = reinterpret_cast<BossDodongo*>(actor);
        return dodongo->health;
    }
    return actor->colChkInfo.health;
}

// Updates the boss health in its corresponding memory location.
static void SetBossHealth(Actor* actor, s16 newHealth) {
    if (actor->id == ACTOR_BOSS_DODONGO) {
        BossDodongo* dodongo = reinterpret_cast<BossDodongo*>(actor);
        dodongo->health = newHealth;
    } else {
        actor->colChkInfo.health = (u8)newHealth;
    }
}

void HandleBossHealthMultiplier(void* refActor) {
    // 0 corresponds to vanilla multiplier (1x)
    if (CVAR <= 0) {
        return;
    }

    Actor* actor = static_cast<Actor*>(refActor);
    s16 currentHealth = GetBossHealth(actor);

    if (currentHealth > 0) {
        // Apply multiplier on initial spawn or if health was externally restored
        if (sLastBossHealth.count(actor) == 0 || currentHealth > sLastBossHealth[actor]) {
            int option = CVAR;
            if (option < 0 || option > 4) {
                option = 0;
            }
            float multiplier = sMultipliers[option];

            currentHealth = (s16)(currentHealth * multiplier);
            SetBossHealth(actor, currentHealth);
        }

        sLastBossHealth[actor] = currentHealth;
    }
}

void ClearBossHealthMultiplier(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    sLastBossHealth.erase(actor);
}

// Handles Barinade (ACTOR_BOSS_VA) whose health is tracked via a file-static variable
// Triggered by the OnBossVaHealthInit GameInteractor hook, overwriting the value via pointer
void HandleBarinadeHealthMultiplier(s8* phase4Hp) {
    if (CVAR <= 0 || phase4Hp == nullptr || *phase4Hp <= 0) {
        return;
    }

    int option = CVAR;
    if (option < 0 || option > 4) {
        option = 0;
    }
    float multiplier = sMultipliers[option];

    *phase4Hp = (s8)(*phase4Hp * multiplier);
}

// Register conditional hooks for each boss with its own static hook ID
#define REGISTER_BOSS_HOOKS(bossId)                                            \
    COND_ID_HOOK(OnActorUpdate, bossId, CVAR > 0, HandleBossHealthMultiplier); \
    COND_ID_HOOK(OnActorDestroy, bossId, true, ClearBossHealthMultiplier);

void RegisterBossHealthMultiplier() {
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_GOMA);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_DODONGO);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_GANONDROF);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_FD);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_MO);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_SST);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_TW);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_GANON);
    REGISTER_BOSS_HOOKS(ACTOR_BOSS_GANON2);

    // Barinade hook
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnBossVaHealthInit>(HandleBarinadeHealthMultiplier);
}

#undef REGISTER_BOSS_HOOKS

static RegisterShipInitFunc initFunc(RegisterBossHealthMultiplier, { CVAR_NAME });
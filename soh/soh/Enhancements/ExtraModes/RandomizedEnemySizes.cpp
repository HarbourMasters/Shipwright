#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ObjectExtension/ActorMaximumHealth.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Fz/z_en_fz.h"
}

static constexpr int32_t CVAR_RANDO_ENEMY_SIZE_DEFAULT = 0;
#define CVAR_RANDO_ENEMY_SIZE_NAME CVAR_ENHANCEMENT("RandomizedEnemySizes")
#define CVAR_RANDO_ENEMY_SIZE_VALUE CVarGetInteger(CVAR_RANDO_ENEMY_SIZE_NAME, CVAR_RANDO_ENEMY_SIZE_DEFAULT)

static constexpr int32_t CVAR_ENEMY_SCALE_HEALTH_DEFAULT = 0;
#define CVAR_ENEMY_SCALE_HEALTH_NAME CVAR_ENHANCEMENT("EnemySizeScalesHealth")
#define CVAR_ENEMY_SCALE_HEALTH_VALUE CVarGetInteger(CVAR_ENEMY_SCALE_HEALTH_NAME, CVAR_ENEMY_SCALE_HEALTH_DEFAULT)

static void RandomizedEnemySizes(void* refActor) {
    // Randomized Enemy Sizes
    Actor* actor = static_cast<Actor*>(refActor);

    // Exclude wobbly platforms in Jabu because they need to act like platforms.
    // Exclude demo effect for Zora sapphire being re-categorized as a "boss".
    // Exclude Dead Hand hands and Bongo Bongo main body because they make the fights (near) impossible.
    bool excludedEnemy = actor->id == ACTOR_EN_BROB || actor->id == ACTOR_EN_DHA || actor->id == ACTOR_DEMO_EFFECT ||
                         (actor->id == ACTOR_BOSS_SST && actor->params == -1);

    // Only apply to enemies and bosses.
    if ((actor->category != ACTORCAT_ENEMY && actor->category != ACTORCAT_BOSS) || excludedEnemy) {
        return;
    }

    float randomNumber;
    float randomScale;

    // Dodongo, Volvagia and Dead Hand are always smaller because they're impossible when bigger.
    bool smallOnlyEnemy = actor->id == ACTOR_BOSS_DODONGO || actor->id == ACTOR_BOSS_FD ||
                          actor->id == ACTOR_BOSS_FD2 || actor->id == ACTOR_EN_DH;

    bool bigActor = !smallOnlyEnemy && (rand() % 2);

    // Big actor
    if (bigActor) {
        randomNumber = rand() % 200;
        // Between 100% and 300% size.
        randomScale = 1.0f + (randomNumber / 100);
    } else {
        // Small actor
        randomNumber = rand() % 90;
        // Between 10% and 100% size.
        randomScale = 0.1f + (randomNumber / 100);
    }

    Actor_SetScale(actor, actor->scale.z * randomScale);

    if (CVAR_ENEMY_SCALE_HEALTH_VALUE && (actor->category == ACTORCAT_ENEMY)) {
        // Scale the health based on a smaller factor than randomScale
        float healthScalingFactor = 0.8f; // Adjust this factor as needed
        float scaledHealth = actor->colChkInfo.health * (randomScale * healthScalingFactor);

        // Ensure the scaled health doesn't go below zero
        actor->colChkInfo.health = fmax(scaledHealth, 1.0f);

        // Ensure maximum health gets set
        SetActorMaximumHealth(actor, actor->colChkInfo.health);
    }
}

static void RegisterRandomizedEnemySizes() {
    COND_HOOK(OnActorInit, CVAR_RANDO_ENEMY_SIZE_VALUE, RandomizedEnemySizes);
}

static void RegisterFreezardHealthScale() {
    COND_VB_SHOULD(VB_FREEZARD_SCALE_HEALTH_WITH_SIZE, CVAR_RANDO_ENEMY_SIZE_VALUE && CVAR_ENEMY_SCALE_HEALTH_VALUE, {
        // With enemy health scaling, the Freezard's health could cause an index out of bounds for the displayLists, so
        // we need to recompute the index based on the scaled health (using the maximum health value) and clamp the
        // final result for safety.
        Actor* actor = va_arg(args, Actor*);
        s32* index = va_arg(args, s32*);

        u8 scaledHealth = (u8)(((f32)actor->colChkInfo.health / GetActorMaximumHealth(actor)) * 6);
        *index = CLAMP((6 - scaledHealth) >> 1, 0, 2);
    });
}

static RegisterShipInitFunc initFunc_EnemySizes(RegisterRandomizedEnemySizes, { CVAR_RANDO_ENEMY_SIZE_NAME });
static RegisterShipInitFunc initFunc_Freezard(RegisterFreezardHealthScale,
                                              { CVAR_RANDO_ENEMY_SIZE_NAME, CVAR_ENEMY_SCALE_HEALTH_NAME });

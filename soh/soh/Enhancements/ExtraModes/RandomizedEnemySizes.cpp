#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ObjectExtension/ActorMaximumHealth.h"
#include "soh/ShipInit.hpp"
#include "soh/ShipUtils.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/randomizer/SeedContext.h"

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

static float ComputeRandomScale(Actor* actor, bool isSmallOnly) {
    const uint8_t mode = CVAR_RANDO_ENEMY_SIZE_VALUE;
    bool isBigActor = false;

    if (mode == ENEMY_SIZE_RANDOM_SEEDED) {
        // Deterministic seed from actor spawn data + global seed, matching the pattern used by the enemy randomizer
        // (EnemyRandomizer.cpp).  The salt (0xDEAD) decorrelates the sequence from the enemy replacement RNG so that
        // the size and enemy type don't share the same initial state when both systems use the same actor ID and
        // position.
        static uint64_t randomState = 0;
        const uint32_t seed = gPlayState->sceneNum + actor->id + static_cast<int32_t>(actor->home.pos.x) +
                              static_cast<int32_t>(actor->home.pos.y) + static_cast<int32_t>(actor->home.pos.z) +
                              actor->home.rot.x + actor->home.rot.y + actor->home.rot.z + actor->params;

        ShipUtils::RandInit(
            (seed ^ 0xDEAD) + (IS_RANDO
                                   ? Rando::Context::GetInstance()->GetSeed()
                                   : gSaveContext.ship.stats.fileCreatedAt),
            &randomState);

        isBigActor = !isSmallOnly && ShipUtils::Random(0, 2, &randomState) == 1;
        if (isBigActor) {
            return 1.0f + ShipUtils::RandomDouble(&randomState) * 2.0f; // 100% to 300%
        }

        return 0.1f + ShipUtils::RandomDouble(&randomState) * 0.9f; // 10% to 100%
    }

    // Unseeded random -- different every room load.
    isBigActor = !isSmallOnly && ShipUtils::Random(0, 2, nullptr) == 1;
    if (isBigActor) {
        return 1.0f + ShipUtils::RandomDouble(nullptr) * 2.0f; // 100% to 300%
    }

    return 0.1f + ShipUtils::RandomDouble(nullptr) * 0.9f; // 10% to 100%
}

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

    // Dodongo, Volvagia and Dead Hand are always smaller because they're impossible when bigger.
    bool smallOnlyEnemy = actor->id == ACTOR_BOSS_DODONGO || actor->id == ACTOR_BOSS_FD ||
                          actor->id == ACTOR_BOSS_FD2 || actor->id == ACTOR_EN_DH;

    const float randomScale = ComputeRandomScale(actor, smallOnlyEnemy);
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

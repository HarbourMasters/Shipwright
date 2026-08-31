#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ObjectExtension/ActorMaximumHealth.h"
#include "soh/ShipInit.hpp"
#include "soh/ShipUtils.h"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Crow/z_en_crow.h"
#include "src/overlays/actors/ovl_En_Firefly/z_en_firefly.h"
extern void EnCrow_TurnAway(EnCrow*, PlayState*);
extern void EnFirefly_SetupFlyAway(EnFirefly*);
extern void EnFirefly_Fall(EnFirefly*, PlayState*);
}

static constexpr int32_t CVAR_RANDO_ENEMY_SIZE_DEFAULT = 0;
#define CVAR_RANDO_ENEMY_SIZE_NAME CVAR_ENHANCEMENT("RandomizedEnemySizes")
#define CVAR_RANDO_ENEMY_SIZE_VALUE CVarGetInteger(CVAR_RANDO_ENEMY_SIZE_NAME, CVAR_RANDO_ENEMY_SIZE_DEFAULT)

static constexpr int32_t CVAR_ENEMY_SCALE_HEALTH_DEFAULT = 0;
#define CVAR_ENEMY_SCALE_HEALTH_NAME CVAR_ENHANCEMENT("EnemySizeScalesHealth")
#define CVAR_ENEMY_SCALE_HEALTH_VALUE CVarGetInteger(CVAR_ENEMY_SCALE_HEALTH_NAME, CVAR_ENEMY_SCALE_HEALTH_DEFAULT)

// If scale > normal, start downscaling already during death fall
void EnemyRandoSize_KeeseFallScale(void* ptr) {
    EnFirefly* enFirefly = (EnFirefly*)ptr;
    if (enFirefly->actionFunc == EnFirefly_Fall && enFirefly->actor.scale.x > 0.005f) {
        Math_StepToF(&enFirefly->actor.scale.x, 0.005f, 0.00034f);
        enFirefly->actor.scale.y = enFirefly->actor.scale.z = enFirefly->actor.scale.x;
    }
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

    bool bigActor = !smallOnlyEnemy && ShipUtils::Random(0, 2) == 0;

    float randomScale;
    if (bigActor) {
        randomScale = static_cast<float>(1.0f + ShipUtils::RandomDouble() * 2.0f);
    } else {
        randomScale = static_cast<float>(0.1f + ShipUtils::RandomDouble() * 0.9f);
    }

    Actor_SetScale(actor, actor->scale.z * randomScale);

    if (CVAR_ENEMY_SCALE_HEALTH_VALUE && (actor->category == ACTORCAT_ENEMY)) {
        // Scale the health based on a smaller factor than randomScale
        float healthScalingFactor = 0.8f; // Adjust this factor as needed
        float scaledHealth = actor->colChkInfo.health * (randomScale * healthScalingFactor);

        // Ensure the scaled health doesn't go below zero
        actor->colChkInfo.health = static_cast<u8>(fmax(scaledHealth, 1.0f));

        // Ensure maximum health gets set
        SetActorMaximumHealth(actor, actor->colChkInfo.health);
    }
}

static void RegisterRandomizedEnemySizes() {
    COND_HOOK(OnActorInit, CVAR_RANDO_ENEMY_SIZE_VALUE, RandomizedEnemySizes);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_FIREFLY, CVAR_RANDO_ENEMY_SIZE_VALUE, EnemyRandoSize_KeeseFallScale);

    // Guays try to die on any damage, but EnCrow_Update doesn't let them reach ground if health != 0
    // Makeshift "damaged but not dead" action setup
    COND_VB_SHOULD(VB_GUAY_SETUP_DAMAGED, CVAR_RANDO_ENEMY_SIZE_VALUE && CVAR_ENEMY_SCALE_HEALTH_VALUE, {
        EnCrow* enCrow = va_arg(args, EnCrow*);

        if (enCrow->actor.colChkInfo.damage < enCrow->actor.colChkInfo.health) {
            *should = false;
            Actor_ApplyDamage(&enCrow->actor);
            enCrow->actor.colorFilterTimer = 40;
            Actor_SetColorFilter(&enCrow->actor, 0x4000, 255, 0, 40);
            Audio_PlayActorSound2(&enCrow->actor, NA_SE_EN_KAICHO_DEAD);
            enCrow->timer = 50;
            enCrow->actor.speedXZ = 3.5f;
            enCrow->aimRotX = -0x1000;
            enCrow->aimRotY = enCrow->actor.yawTowardsPlayer + 0x8000;
            enCrow->skelAnime.playSpeed = 2.0f;
            enCrow->actionFunc = EnCrow_TurnAway;
        }
    });

    // Clamp distance from ground due to hard to hit with Kokiri
    COND_VB_SHOULD(VB_GUAY_ALIVE_MOVE_HEIGHT_OFFSET, CVAR_RANDO_ENEMY_SIZE_VALUE && CVAR_ENEMY_SCALE_HEALTH_VALUE, {
        f32* scale = va_arg(args, f32*);

        if (*should && *scale > 2.0f) {
            *scale = 2.0f;
        }
    });

    // Keese try to fall/die on any damage which behaves oddly if health is above normal 1
    // Makeshift "damaged but not dead" action setup
    COND_VB_SHOULD(VB_KEESE_SETUP_FALL, CVAR_RANDO_ENEMY_SIZE_VALUE && CVAR_ENEMY_SCALE_HEALTH_VALUE, {
        EnFirefly* enFirefly = va_arg(args, EnFirefly*);

        if (*should && enFirefly->actor.colChkInfo.health != 0) {
            *should = false;
            Audio_PlayActorSound2(&enFirefly->actor, NA_SE_EN_FFLY_DEAD);
            Actor_SetColorFilter(&enFirefly->actor, 0x4000, 0xFF, 0, 40);
            EnFirefly_SetupFlyAway(enFirefly);
        }
    });

    // Ensure Keese just die on Ice Arrows
    COND_VB_SHOULD(VB_KEESE_SETUP_FROZENFALL, CVAR_RANDO_ENEMY_SIZE_VALUE && CVAR_ENEMY_SCALE_HEALTH_VALUE, {
        EnFirefly* enFirefly = va_arg(args, EnFirefly*);
        enFirefly->actor.colChkInfo.health = 0;
    });
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

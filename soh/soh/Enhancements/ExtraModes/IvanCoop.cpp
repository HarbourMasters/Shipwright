#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
extern s16 gEnPartnerId;
}

#define CVAR_NAME CVAR_ENHANCEMENT("IvanCoopModeEnabled")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, 0)

static void SpawnIvan() {
    if (!gPlayState)
        return;

    Player* player = GET_PLAYER(gPlayState);
    if (!player)
        return;

    if (Actor_Find(&gPlayState->actorCtx, gEnPartnerId, ACTORCAT_ITEMACTION))
        return;

    PosRot& world = player->actor.world;
    Actor_Spawn(&gPlayState->actorCtx, gPlayState, gEnPartnerId, world.pos.x,
                world.pos.y + Player_GetHeight(player) + 5.0f, world.pos.z, 0, world.rot.y, 0, 1);
}

static void KillIvan() {
    if (!gPlayState)
        return;

    Actor* ivan = Actor_Find(&gPlayState->actorCtx, gEnPartnerId, ACTORCAT_ITEMACTION);
    if (ivan)
        Actor_Kill(ivan);
}

// #region Patch distance checks (allows Ivan to break pots and crates while far from Link)

static bool ShouldPatchDist(s16 actorId) {
    switch (actorId) {
        // AC is enabled when player is nearby:
        case ACTOR_BG_BOMBWALL:
        case ACTOR_BG_SPOT08_BAKUDANKABE:
        case ACTOR_OBJ_KIBAKO2: // Note: Checks for explosions regardless of distance
            return true;
        // OC is enabled when player is nearby:
        case ACTOR_EN_ICE_HONO:
        case ACTOR_OBJ_HANA:
            return true;
        // AC/OC are enabled when player is nearby:
        case ACTOR_EN_ISHI:
        case ACTOR_EN_KUSA:
        case ACTOR_EN_WOOD02:
        case ACTOR_OBJ_BOMBIWA: // Note: Checks for explosions regardless of distance
        case ACTOR_OBJ_HAMISHI:
        case ACTOR_OBJ_KIBAKO:
        case ACTOR_OBJ_TSUBO:
            return true;
        // Checks for explosions if player is nearby:
        case ACTOR_BG_SPOT17_BAKUDANKABE:
            return true;
    }
    return false;
}

static f32 ClampDist(f32 distance, s16 actorId) {
    switch (actorId) {
        // Avoid offering bottle capture
        case ACTOR_EN_ICE_HONO:
            return fmaxf(distance, 60.0f);
        // Avoid offering carry
        case ACTOR_EN_ISHI:
            return fmaxf(distance, 90.0f);
        case ACTOR_EN_KUSA:
        case ACTOR_OBJ_KIBAKO:
        case ACTOR_OBJ_TSUBO:
            return fmaxf(distance, 100.0f);
    }
    return distance;
}

static void PatchDistIfNeeded(Actor* actor) {
    if (!ShouldPatchDist(actor->id))
        return;

    Actor* ivan = Actor_Find(&gPlayState->actorCtx, gEnPartnerId, ACTORCAT_ITEMACTION);
    if (ivan == nullptr)
        return;

    f32 ivanDist = Actor_WorldDistXZToActor(actor, ivan);
    ivanDist = ClampDist(ivanDist, actor->id);
    if (ivanDist < actor->xzDistToPlayer)
        actor->xzDistToPlayer = ivanDist;
}

// #endregion

static void RegisterIvanCoop() {
    if (CVAR_VALUE)
        SpawnIvan();
    else
        KillIvan();

    COND_ID_HOOK(OnActorSpawn, ACTOR_PLAYER, CVAR_VALUE, [](void*) { SpawnIvan(); });

    COND_HOOK(ShouldActorUpdate, CVAR_VALUE, [](void* actorRef, bool*) {
        Actor* actor = static_cast<Actor*>(actorRef);
        PatchDistIfNeeded(actor);
    });
}

static RegisterShipInitFunc initFunc(RegisterIvanCoop, { CVAR_NAME });

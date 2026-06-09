#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
extern PlayState* gPlayState;
extern s16 gEnPartnerId;
}

#define CVAR_NAME CVAR_ENHANCEMENT("IvanCoopModeEnabled")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, 0)

static void OnPlayerSpawn(void* actorRef) {
    Player* player = (Player*)actorRef;

    PosRot& world = player->actor.world;

    Actor_Spawn(&gPlayState->actorCtx, gPlayState, gEnPartnerId,
        world.pos.x, world.pos.y + Player_GetHeight(player) + 5.0f, world.pos.z,
        0, world.rot.y, 0,
        1);
}

static void RegisterIvanCoop() {
    COND_ID_HOOK(OnActorSpawn, ACTOR_PLAYER, CVAR_VALUE, OnPlayerSpawn);
}

static RegisterShipInitFunc initFunc(RegisterIvanCoop, { CVAR_NAME });

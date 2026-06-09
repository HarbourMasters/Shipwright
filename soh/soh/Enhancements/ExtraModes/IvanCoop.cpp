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

static void RegisterIvanCoop() {
    if (CVAR_VALUE)
        SpawnIvan();
    else
        KillIvan();

    COND_ID_HOOK(OnActorSpawn, ACTOR_PLAYER, CVAR_VALUE, [](void*) { SpawnIvan(); });
}

static RegisterShipInitFunc initFunc(RegisterIvanCoop, { CVAR_NAME });

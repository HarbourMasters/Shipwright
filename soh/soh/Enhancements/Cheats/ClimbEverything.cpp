#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
extern PlayState* gPlayState;
}

static void RegisterClimbEverything() {
    COND_VB_SHOULD(VB_SURFACE_IS_CLIMBABLE, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0), { *should = true; });
    COND_VB_SHOULD(VB_SURFACE_ANGLE_IS_CLIMBABLE, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0),
                   { *should = true; });

    COND_VB_SHOULD(VB_CLIMB, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), false), {
        Player* player = GET_PLAYER(gPlayState);

        // Allow try opening door while climbing
        if (Player_ActionHandler_1(player, gPlayState)) {
            *should = false;
            if (player->doorType == PLAYER_DOORTYPE_HANDLE) {
                player->actor.world.pos.y = player->doorActor->world.pos.y;
            }
        }
    });

    COND_VB_SHOULD(VB_AFTER_PROCESS_SCENE_COLLISION, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), false), {
        Player* player = GET_PLAYER(gPlayState);

        // Keep climb up animation on sloping ledges
        if (player->actionFunc == Player_Action_8084BDFC) {
            player->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
        }
    });

    COND_VB_SHOULD(VB_EN_DOOR_OFFER_OPEN, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), false), {
        Vec3f playerPosRelToDoor = *va_arg(args, Vec3f*);
        Player* player = GET_PLAYER(gPlayState);

        // Set higher y distance limit to offer open door
        if (fabsf(playerPosRelToDoor.y) < 50.0f && fabsf(playerPosRelToDoor.x) < 20.0f &&
            fabsf(playerPosRelToDoor.z) < 50.0f) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_BE_NEAR_DOOR_SHUTTER, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), false), {
        DoorShutter* doorShutter = va_arg(args, DoorShutter*);
        Vec3f relPlayerPos = *va_arg(args, Vec3f*);
        f32* maxDistSides = va_arg(args, f32*);

        // Set higher y distance limit to offer open door
        // Todo: Individualize this y depending on shutter door type.
        if (fabsf(relPlayerPos.x) < *maxDistSides && fabsf(relPlayerPos.y) < 50.0f) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterClimbEverything, { CVAR_CHEAT("ClimbEverything") });

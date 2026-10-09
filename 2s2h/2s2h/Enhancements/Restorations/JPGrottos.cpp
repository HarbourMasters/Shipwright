#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "functions.h"
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Restorations.JPGrottos"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static bool isSpawningJPGrottos = false;

void RegisterJPGrottos() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_DOOR_ANA, CVAR, [](Actor* actor, bool* should) {
        if (gPlayState->sceneId == SCENE_22DEKUCITY && !isSpawningJPGrottos) {
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorInit, ACTOR_OBJ_SYOKUDAI, CVAR, [](Actor* actor, bool* should) {
        if (gPlayState->sceneId == SCENE_22DEKUCITY && actor->world.rot.x == 0) {
            *should = false;
        }
    });

    // Move circle of rupees in Deku Palace
    COND_ID_HOOK(ShouldActorInit, ACTOR_OBJ_MURE3, CVAR, [](Actor* actor, bool* should) {
        if (gPlayState->sceneId == SCENE_22DEKUCITY && GetActorListIndex(actor) == 17) {
            actor->world.pos.x = 407.0f;
            actor->world.pos.z = 1569.0f;
        }
    });

    // Move single freestanding rupee that's on top of the new grotto location
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ITEM00, CVAR, [](Actor* actor, bool* should) {
        if (gPlayState->sceneId == SCENE_22DEKUCITY) {
            if ((IS_RANDO && actor->params == ITEM00_NOTHING &&
                 CUSTOM_ITEM_PARAM == RC_DEKU_PALACE_FREESTANDING_RUPEE_07) ||
                (!IS_RANDO && ENITEM00_GET_7F00(actor) == 0x11)) {
                actor->world.pos.x = -507.0f;
                actor->world.pos.y = 0.0f;
                actor->world.pos.z = 1334.0f;
            }
        }
    });

    // Override the entrance position of ENTRANCE(DEKU_PALACE, 9), which is where the non JP bean salesman grotto is.
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.entrance == ENTRANCE(DEKU_PALACE, 9)) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = ENTRANCE(DEKU_PALACE, 9);
            gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = 1;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.x = 449.5f;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.y = 80.0f;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.z = 765.4f;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = 1349;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D);
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
            gSaveContext.respawnFlag = -8;
        }
    });

    COND_ID_HOOK(AfterRoomSceneCommands, SCENE_22DEKUCITY, CVAR, [](s8 sceneId, s8 roomNum) {
        isSpawningJPGrottos = true;
        bool lightTorches = (CURRENT_TIME > CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0));
        u16 torchParams = lightTorches ? 10367 : 8319;
        if (roomNum == 1) {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_ANA, 0x01A7, 0x0000, 0x053C, 0x0007, 0x0011,
                        0x007F, 0x0304);
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_ANA, 0x0162, 0x00A0, 0x02C8, 0x0007, 0x000F,
                        0x007F, 0x0305);
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_ANA, 0x01C3, 0x0050, 0x02D6, 0x0007, 0x0010,
                        0x007F, 0x0306);

            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_OBJ_SYOKUDAI, 448.0f, 80.0f, 675.0f, 1, 0, 0,
                        torchParams);
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_OBJ_SYOKUDAI, 338.0f, 160.0f, 658.0f, 1, 0, 0,
                        torchParams);
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_OBJ_SYOKUDAI, 426.0f, 0.0f, 1295.0f, 1, 0, 0,
                        torchParams);
        }
        if (roomNum == 2) {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_ANA, -0x1A4, 0x0000, 0x053C, 0x0007, 0x0012,
                        0x007F, 0x0307);
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_ANA, -0x410, 0x0000, 0x02BD, 0x0007, 0x0013,
                        0x007F, 0x0308);

            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_OBJ_SYOKUDAI, -422.0f, 0.0f, 1297.0f, 1, 0, 0,
                        torchParams);
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_OBJ_SYOKUDAI, -1040.0f, 0.0f, 658.0f, 1, 0, 0,
                        torchParams);
        }
        isSpawningJPGrottos = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterJPGrottos, { CVAR_NAME });

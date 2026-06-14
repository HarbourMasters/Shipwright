#include <libultraship/libultraship.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"

extern "C" {
extern PlayState* gPlayState;
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "src/overlays/actors/ovl_Bg_Spot06_Objects/z_bg_spot06_objects.h"

extern s32 Object_Spawn(ObjectContext* objectCtx, s16 objectId);
extern void BgSpot06Objects_WaterPlaneCutsceneRise(BgSpot06Objects*, PlayState*);
extern void BgSpot06Objects_WaterPlaneCutsceneLower(BgSpot06Objects*, PlayState*);
extern void BgSpot06Objects_LockFloat(BgSpot06Objects*, PlayState*);
}

#define WATER_LEVEL_RAISED (-1313)

// Main water control switch
static Actor* sSwitchMain = nullptr;
// Alternate control switch on fishing island
static Actor* sSwitchIsland = nullptr;
static Actor* sLock = nullptr;
static u8 sPrevFlagState = 0;

static void SpawnSwitches(PlayState* play) {
    // Object containing floor switch data (and ice block data)
    Object_Spawn(&play->objectCtx, OBJECT_GAMEPLAY_DANGEON_KEEP);

    bool waterTempleCleared = Flags_GetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP);
    bool waterLowered = !Flags_GetEventChkInf(EVENTCHKINF_RAISED_LAKE_HYLIA_WATER);
    // Persist the water level across scene reloads.
    if (waterTempleCleared && waterLowered) {
        Flags_SetSwitch(play, 0x3E);
    }

    s16 switchParams;
    if (waterTempleCleared) {
        // Toggle-able floor switch
        switchParams = 0x3E10;
    } else {
        // Frozen rusty switch, same flag as above.  It's glitched and can't be pressed
        switchParams = 0x3E81;
    }

    sSwitchMain =
        // Spawn a floor switch
        Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_SWITCH, -896.0f, -1243.0f, 6953.0f, 0, 0, 0, switchParams);
    // Spawn a sign
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KANBAN, -970.0f, -1242.0f, 6954.0f, 0, 0, 0,
                0x0000 | (TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN & 0xFF));
    if (!waterTempleCleared) {
        // Spawn a Navi check spot when Water Temple isn't cleared
        Actor_Spawn(&play->actorCtx, play, ACTOR_ELF_MSG2, -896.0f, -1243.0f, 6953.0f, 0, 0, 0,
                    0x3D00 | (TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI & 0xFF));
    }

    // Second switch on the fishing pond island.  Up against the wall
    sSwitchIsland =
        // Spawn a floor switch
        Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_SWITCH, 1320.0f, -1218.7f, 4025.0f, 0, 0, 0, switchParams);
    // Spawn a sign
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KANBAN, 1320.0f, -1217.7f, 3951.0f, 0, -0x4000, 0,
                0x0000 | (TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN & 0xFF));
    if (!waterTempleCleared) {
        // Spawn a Navi check spot when Water Temple isn't cleared
        Actor_Spawn(&play->actorCtx, play, ACTOR_ELF_MSG2, 1320.0f, -1218.7f, 4025.0f, 0, 0, 0,
                    0x3D00 | (TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI & 0xFF));
    }

    sPrevFlagState = Flags_GetSwitch(play, 0x3E) != 0;
}

void RegisterLakeHyliaWaterControl() {
    COND_HOOK(OnSceneSpawnActors, IS_RANDO, []() {
        // Bail early for water control system for child, non-rando, or wrong scene
        if (LINK_IS_ADULT && gPlayState->sceneNum == SCENE_LAKE_HYLIA) {
            SpawnSwitches(gPlayState);
        }
    });

    // Strip the ice-block bit so melting it doesn't toggle flag 0x3E.
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_SWITCH, IS_RANDO, [](void* actorRef) {
        Actor* actor = static_cast<Actor*>(actorRef);
        if (actor == sSwitchMain || actor == sSwitchIsland) {
            actor->params &= ~0x80;
        }
    });

    // Keep track of the floating lock
    COND_ID_HOOK(OnActorInit, ACTOR_BG_SPOT06_OBJECTS, IS_RANDO, [](void* actorRef) {
        Actor* actor = static_cast<Actor*>(actorRef);
        if (actor->params == 1 /* LHO_WATER_TEMPLE_ENTRANCE_LOCK */) {
            sLock = actor;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_BG_SPOT06_OBJECTS, IS_RANDO, [](void* actorRef) {
        Actor* actor = static_cast<Actor*>(actorRef);
        if (actor->params != 2 /* LHO_WATER_PLANE */ || !LINK_IS_ADULT) {
            return;
        }
        BgSpot06Objects* waterPlane = reinterpret_cast<BgSpot06Objects*>(actor);

        if (sLock != nullptr) {
            BgSpot06Objects* lockObj = reinterpret_cast<BgSpot06Objects*>(sLock);
            if (lockObj->actionFunc == BgSpot06Objects_LockFloat) {
                // If we're in LockFloat, change the Y position to track the water surface
                sLock->home.pos.y = waterPlane->lakeHyliaWaterLevel + WATER_LEVEL_RAISED;
            }
        }

        u8 flagState = Flags_GetSwitch(gPlayState, 0x3E) != 0;
        if (sPrevFlagState == flagState) {
            return;
        }
        sPrevFlagState = flagState;
        waterPlane->actionFunc =
            flagState ? BgSpot06Objects_WaterPlaneCutsceneLower : BgSpot06Objects_WaterPlaneCutsceneRise;
    });

    // Synchronize pressed states of both main and island switches
    COND_HOOK(OnPlayerUpdate, IS_RANDO, []() {
        if (gPlayState->sceneNum != SCENE_LAKE_HYLIA) {
            return;
        }
        if (sSwitchMain == nullptr || sSwitchIsland == nullptr) {
            return;
        }
        DynaPolyActor* mainSwitch = reinterpret_cast<DynaPolyActor*>(sSwitchMain);
        DynaPolyActor* islandSwitch = reinterpret_cast<DynaPolyActor*>(sSwitchIsland);
        u32 merged = (mainSwitch->interactFlags | islandSwitch->interactFlags) & DYNA_INTERACT_PLAYER_ON_TOP;
        if (merged == 0) {
            return;
        }
        mainSwitch->interactFlags |= merged;
        islandSwitch->interactFlags |= merged;
    });

    COND_HOOK(OnPlayDestroy, IS_RANDO, []() {
        sSwitchMain = nullptr;
        sSwitchIsland = nullptr;
        sLock = nullptr;
        sPrevFlagState = 0;
    });
}

static RegisterShipInitFunc registerLakeHyliaWaterControl(RegisterLakeHyliaWaterControl, { "IS_RANDO" });

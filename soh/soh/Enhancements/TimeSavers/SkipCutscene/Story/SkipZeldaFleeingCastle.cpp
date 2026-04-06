#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64save.h"
#include "functions.h"
extern SaveContext gSaveContext;
}

/**
 * When this cutscene is skipped, walking up to the bridge to castle town triggers a quick fade in/out
 * which can be confusing to beginners, because they need to then fetch the Ocarina of Time from the water.
 * To make it more obvious what happened, we'll play the sound of the Ocarina dropping into the water.
 */
static int framesSinceSpawn = 0;
static ListenerID itemOcarinaUpdateHook = -1;
static ListenerID sceneInitHook = -1;

void SkipZeldaFleeingCastle_OnActorUpdate(IEvent* event) {
    OnActorUpdate* ev = reinterpret_cast<OnActorUpdate*>(event);
    Actor* actor = static_cast<Actor*>(ev->actor);

    if (actor->params != 3) {
        return;
    }

    framesSinceSpawn++;
    if (framesSinceSpawn > 20) {
        Audio_PlayActorSound2(actor, NA_SE_EV_BOMB_DROP_WATER);

        UNREGISTER_LISTENER(OnActorUpdate, itemOcarinaUpdateHook);
        UNREGISTER_LISTENER(OnSceneInit, sceneInitHook);

        itemOcarinaUpdateHook = -1;
        sceneInitHook = -1;
    }
}

void SkipZeldaFleeingCastle_OnActorInit(IEvent* event) {
    OnActorInit* ev = reinterpret_cast<OnActorInit*>(event);
    Actor* actor = static_cast<Actor*>(ev->actor);

    if (actor->params == 3) {
        framesSinceSpawn = 0;
        itemOcarinaUpdateHook = REGISTER_LISTENER(OnActorUpdate, EVENT_PRIORITY_LOW, SkipZeldaFleeingCastle_OnActorUpdate);
        sceneInitHook = REGISTER_LISTENER(OnSceneInit, EVENT_PRIORITY_LOW, [](IEvent* event) {
            UNREGISTER_LISTENER(OnActorUpdate, itemOcarinaUpdateHook);
            UNREGISTER_LISTENER(OnSceneInit, sceneInitHook);
            itemOcarinaUpdateHook = -1;
            sceneInitHook = -1;
        });
    }
}

void RegisterSkipZeldaFleeingCastle() {
    COND_ID_HOOK(OnActorInit, ACTOR_ITEM_OCARINA,
                 CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO),
                 SkipZeldaFleeingCastle_OnActorInit);
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO), {
        if (gSaveContext.entranceIndex == ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN && gSaveContext.cutsceneIndex == 0xFFF1) {
            // Normally set in the cutscene
            gSaveContext.dayTime = gSaveContext.skyboxTime = 0x4AAA;

            gSaveContext.cutsceneIndex = 0;
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipZeldaFleeingCastle,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), "IS_RANDO" });

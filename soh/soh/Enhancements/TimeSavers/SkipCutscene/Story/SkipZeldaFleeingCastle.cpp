#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/TimeSavers/SkipCutscene/CutsceneTime.h"
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
static HOOK_ID itemOcarinaUpdateHook = 0;
static HOOK_ID sceneInitHook = 0;

void SkipZeldaFleeingCastle_OnActorUpdate(void* actorPtr) {
    Actor* actor = static_cast<Actor*>(actorPtr);

    framesSinceSpawn++;
    if (framesSinceSpawn > 20) {
        Audio_PlayActorSound2(actor, NA_SE_EV_BOMB_DROP_WATER);

        GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorUpdate>(itemOcarinaUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(sceneInitHook);
        itemOcarinaUpdateHook = 0;
        sceneInitHook = 0;
    }
}

void SkipZeldaFleeingCastle_OnActorInit(void* actorPtr) {
    Actor* actor = static_cast<Actor*>(actorPtr);

    if (actor->params == 3) {
        framesSinceSpawn = 0;
        itemOcarinaUpdateHook = GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::OnActorUpdate>(
            (uintptr_t)actorPtr, SkipZeldaFleeingCastle_OnActorUpdate);
        sceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
            GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorUpdate>(itemOcarinaUpdateHook);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(sceneInitHook);
            itemOcarinaUpdateHook = 0;
            sceneInitHook = 0;
        });
    }
}

// BgSpot00Hanebasi_Update swaps the room's time speed for one that reaches dawn by the end, then stops time at dawn.
// Walking in late at night picks a speed too low to reach morning, and time can end up barely moving at all.
static void HanebasiTime(uint16_t& dayTime, uint16_t& timeSpeed) {
    if (timeSpeed == 50) {
        int32_t dawn = dayTime >= 0xD557 ? 0x1D556 : 0xD556;
        timeSpeed = (dawn - dayTime) * (1.0f / 350.0f);
    }
    if (dayTime >= 0x2AAC && dayTime < 0x3000) {
        timeSpeed = 0;
    }
}

void RegisterSkipZeldaFleeingCastle() {
    COND_ID_HOOK(OnActorInit, ACTOR_ITEM_OCARINA,
                 CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO),
                 SkipZeldaFleeingCastle_OnActorInit);
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO), {
        if (gSaveContext.entranceIndex == ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN && gSaveContext.cutsceneIndex == 0xFFF1) {
            // Normally set in the cutscene
            gSaveContext.dayTime = gSaveContext.skyboxTime =
                CutsceneTime_Simulate(SCENE_HYRULE_FIELD, 0xFFF1, gSaveContext.dayTime, 7, HanebasiTime);

            gSaveContext.cutsceneIndex = 0;
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipZeldaFleeingCastle,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), "IS_RANDO" });

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"

extern "C" {
    #include "z64save.h"
    #include "functions.h"
    extern SaveContext gSaveContext;
}

void SkipZeldaFleeingCastle_ShouldPlayTransitionCS(GIVanillaBehavior _, bool* should, va_list originalArgs) {
    if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
        if (gSaveContext.entranceIndex == ENTR_HYRULE_FIELD_0 && gSaveContext.cutsceneIndex == 0xFFF1) {
            // Normally set in the cutscene
            gSaveContext.dayTime = gSaveContext.skyboxTime = 0x4AAA;

            gSaveContext.cutsceneIndex = 0;
            *should = false;
        }
    }
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

    if (
        actor->params == 3 &&
        CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)
    ) {
        framesSinceSpawn = 0;
        itemOcarinaUpdateHook = GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::OnActorUpdate>((uintptr_t)actorPtr, SkipZeldaFleeingCastle_OnActorUpdate);
        sceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([] (int16_t sceneNum) {
            GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorUpdate>(itemOcarinaUpdateHook);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(sceneInitHook);
            itemOcarinaUpdateHook = 0;
            sceneInitHook = 0;
        });
    }
}

void SkipZeldaFleeingCastle_Register() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_ITEM_OCARINA, SkipZeldaFleeingCastle_OnActorInit);
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnVanillaBehavior>(VB_PLAY_TRANSITION_CS, SkipZeldaFleeingCastle_ShouldPlayTransitionCS);
}

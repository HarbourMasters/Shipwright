#include <libultraship/bridge/consolevariablebridge.h>

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

/**
 * Cutscene speeds time up so it ends at dawn. Walking in late at night picks a speed too
 * low to reach morning, and time can end up barely moving at all.
 */
static uint16_t ZeldaFleeingCutsceneTime(uint16_t time) {
    uint32_t dawn = time >= 0xD557 ? 0x1D556 : 0xD556;
    uint16_t speed = (dawn - time) / 350;

    // 2259 frames, minus 88 frozen by textboxes, minus 7 frozen while fading
    for (int32_t frame = 0; frame < 2164; frame++) {
        // time stops at dawn
        if (time >= 0x2AAC && time < 0x3000) {
            speed = 0;
        }
        // last 340 frames clear sky and race to morning
        if (frame >= 1824 && time < 0x4AAB) {
            time += 30;
        }
        time += time > 0xC000 || time < 0x4555 ? speed * 2 : speed; // night runs double
    }

    return time;
}

void RegisterSkipZeldaFleeingCastle() {
    COND_ID_HOOK(OnActorInit, ACTOR_ITEM_OCARINA,
                 CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO),
                 SkipZeldaFleeingCastle_OnActorInit);
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO), {
        if (gSaveContext.entranceIndex == ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN && gSaveContext.cutsceneIndex == 0xFFF1) {
            // Normally set in the cutscene
            gSaveContext.dayTime = gSaveContext.skyboxTime = ZeldaFleeingCutsceneTime(gSaveContext.dayTime);

            gSaveContext.cutsceneIndex = 0;
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipZeldaFleeingCastle,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), "IS_RANDO" });

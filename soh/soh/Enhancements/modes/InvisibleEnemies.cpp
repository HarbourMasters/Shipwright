#include "soh/OTRGlobals.h"
#include "InvisibleEnemies.h"
#include "libultraship/libultraship.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include "macros.h"
#include "z64.h"
extern PlayState* gPlayState;
}

void RegisterInvisibleEnemies() {
    static uint32_t enemyLensReactHookId = 0;
    static uint32_t roomLensModeHookId = 0;
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(enemyLensReactHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnRoomInit>(roomLensModeHookId);

    if (CVarGetInteger(CVAR_ENHANCEMENT("InvisibleEnemies"), 0)) {
        enemyLensReactHookId =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* actorx) {
                Actor* actor = (Actor*)actorx;
                if (actor->category == ACTORCAT_ENEMY) {
                    actor->flags |= ACTOR_FLAG_LENS;
                }
            });

        roomLensModeHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRoomInit>(
            [](int16_t sceneId, int8_t roomNum) { gPlayState->roomCtx.curRoom.lensMode = LENS_MODE_HIDE_ACTORS; });
    }
}

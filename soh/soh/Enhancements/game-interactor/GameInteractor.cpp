#include "GameInteractor.h"
#include <libultraship/bridge.h>

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
}

#include "overlays/actors/ovl_En_Niw/z_en_niw.h"

// MARK: - Effects

GameInteractionEffectQueryResult GameInteractor::CanApplyEffect(GameInteractionEffectBase* effect) {
    return effect->CanBeApplied();
}

void GameInteractor::ApplyEffect(GameInteractionEffectBase* effect) {
    return effect->Apply();
}

void GameInteractor::RemoveEffect(GameInteractionEffectBase* effect) {
    return effect->Remove();
}

// MARK: - Helpers

bool GameInteractor::IsSaveLoaded() {
    Player* player;
    if (gPlayState != NULL) {
        player = GET_PLAYER(gPlayState);
    }
    return (gPlayState == NULL || player == NULL || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) ? false : true;
}

bool GameInteractor::IsGameplayPaused() {
    Player* player = GET_PLAYER(gPlayState);
    return (Player_InBlockingCsMode(gPlayState, player) || gPlayState->pauseCtx.state != 0 || gPlayState->msgCtx.msgMode != 0) ? true : false;
}

bool GameInteractor::CanSpawnEnemy() {
    return GameInteractor::IsSaveLoaded() && !GameInteractor::IsGameplayPaused();
}

/* 
GameInteractor is meant to be used for interacting with the game (yup...).
It exposes functions that directly modify, add or remove game related elements.

GameInteractionEffects.cpp is used when code that needs these
functions also need a check wether a command can be run or not.

If these checks need to happen wherever GameInteractor functions are needed, the
GameInteractor functions can be called directly.
*/

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

GameInteractionEffectQueryResult GameInteractor::ApplyEffect(GameInteractionEffectBase* effect) {
    return effect->Apply();
}

GameInteractionEffectQueryResult GameInteractor::RemoveEffect(RemovableGameInteractionEffect* effect) {
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

bool GameInteractor::CanSpawnActor() {
    return GameInteractor::IsSaveLoaded() && !GameInteractor::IsGameplayPaused();
}

bool GameInteractor::CanAddOrTakeAmmo(int16_t amount, int16_t item) {
    int16_t upgradeToCheck = 0;

    switch (item) {
        case ITEM_STICK:
            upgradeToCheck = UPG_STICKS;
            break;
        case ITEM_NUT:
            upgradeToCheck = UPG_NUTS;
            break;
        case ITEM_BOW:
            upgradeToCheck = UPG_QUIVER;
            break;
        case ITEM_SLINGSHOT:
            upgradeToCheck = UPG_BULLET_BAG;
            break;
        case ITEM_BOMB:
            upgradeToCheck = UPG_BOMB_BAG;
            break;
        default:
            break;
    }

    if (amount < 0 && AMMO(item) == 0) {
        return false;
    }

    if (item != ITEM_BOMBCHU && item != ITEM_BEAN) {
        if ((CUR_CAPACITY(upgradeToCheck) == 0) || (amount > 0 && AMMO(item) == CUR_CAPACITY(upgradeToCheck))) {
            return false;
        }
        return true;
    } else {
        // Separate checks for beans and bombchus because they don't have capacity upgrades
        if (INV_CONTENT(item) != item ||
            (amount > 0 && ((item == ITEM_BOMBCHU && AMMO(item) == 50) || (item == ITEM_BEAN && AMMO(item) == 10)))) {
            return false;
        }
        return true;
    }
}

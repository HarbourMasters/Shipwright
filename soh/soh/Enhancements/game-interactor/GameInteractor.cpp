/*
GameInteractor is meant to be used for interacting with the game (yup...).
It exposes functions that directly modify, add or remove game related elements.

GameInteractionEffects.cpp is used when code that needs these
functions also need a check wether a command can be run or not.

If these checks need to happen wherever GameInteractor functions are needed, the
GameInteractor functions can be called directly.
*/

#define INIT_EVENT_IDS
#include "GameInteractor.h"
#include <libultraship/bridge.h>

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
}

// MARK: - Effects

GameInteractionEffectQueryResult GameInteractor::CanApplyEffect(GameInteractionEffectBase& effect) {
    return effect.CanBeApplied();
}

GameInteractionEffectQueryResult GameInteractor::ApplyEffect(GameInteractionEffectBase& effect) {
    return effect.Apply();
}

GameInteractionEffectQueryResult GameInteractor::RemoveEffect(RemovableGameInteractionEffect& effect) {
    return effect.Remove();
}

// MARK: - Helpers

bool GameInteractor::IsSaveLoaded(bool allowDbgSave) {
    Player* player;
    if (gPlayState != NULL) {
        player = GET_PLAYER(gPlayState);
    }

    // Checking for normal game mode prevents debug saves from reporting true on title screen
    if (gPlayState == NULL || player == NULL || gSaveContext.gameMode != GAMEMODE_NORMAL) {
        return false;
    }

    // Valid save file or debug save
    return (gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2) || (allowDbgSave && gSaveContext.fileNum == 0xFF);
}

bool GameInteractor::IsGameplayPaused() {
    Player* player = GET_PLAYER(gPlayState);
    return (Player_InBlockingCsMode(gPlayState, player) || gPlayState->pauseCtx.state != 0 ||
            gPlayState->msgCtx.msgMode != 0)
               ? true
               : false;
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

bool GameInteractor_Should(GIVanillaBehavior flag, u32 result, ...) {
    // Only the external function can use the Variadic Function syntax
    // To pass the va args to the next caller must be done using va_list and reading the args into it
    // Because there can be N subscribers registered to each template call, the subscribers will be responsible for
    // creating a copy of this va_list to avoid incrementing the original pointer between calls
    va_list args;
    va_start(args, result);

    // Because of default argument promotion, even though our incoming "result" is just a bool, it needs to be typed as
    // an int to be permitted to be used in `va_start`, otherwise it is undefined behavior.
    // Here we downcast back to a bool for our actual hook handlers
    bool boolResult = static_cast<bool>(result);

    CALL_EVENT(OnVanillaBehavior, flag, &boolResult, &args);

    va_end(args);
    return boolResult;
}

void EventSystem_Register() {
    // MARK: - Gameplay
    REGISTER_EVENT(OnZTitleInit);
    REGISTER_EVENT(OnZTitleUpdate);
    REGISTER_EVENT(OnLoadGame);
    REGISTER_EVENT(OnExitGame);
    REGISTER_EVENT(OnGameStateMainStart);
    REGISTER_EVENT(OnGameFrameUpdate);
    REGISTER_EVENT(OnCameraState);
    REGISTER_EVENT(OnItemReceive);
    REGISTER_EVENT(OnEquipmentDelete);
    REGISTER_EVENT(OnSaleEnd);
    REGISTER_EVENT(OnTransitionEnd);
    REGISTER_EVENT(OnSceneInit);
    REGISTER_EVENT(AfterSceneCommands);
    REGISTER_EVENT(OnSceneFlagSet);
    REGISTER_EVENT(OnSceneFlagUnset);
    REGISTER_EVENT(OnFlagSet);
    REGISTER_EVENT(OnFlagUnset);
    REGISTER_EVENT(OnSceneSpawnActors);
    REGISTER_EVENT(OnLinkSkeletonInit);
    REGISTER_EVENT(OnLinkEquipmentChange);
    REGISTER_EVENT(OnPlayerUpdate);
    REGISTER_EVENT(OnSetDoAction);
    REGISTER_EVENT(OnPlayerSfx);
    REGISTER_EVENT(OnOcarinaSongAction);
    REGISTER_EVENT(OnOcarinaNote);
    REGISTER_EVENT(OnCuccoOrChickenHatch);
    REGISTER_EVENT(OnShopSlotChange);
    REGISTER_EVENT(OnDungeonKeyUsed);

    // MARK: - Actors & Entities
    REGISTER_EVENT(ShouldActorInit);
    REGISTER_EVENT(OnActorInit);
    REGISTER_EVENT(OnActorSpawn);
    REGISTER_EVENT(ShouldActorUpdate);
    REGISTER_EVENT(OnActorUpdate);
    REGISTER_EVENT(OnActorKill);
    REGISTER_EVENT(OnActorDestroy);
    REGISTER_EVENT(OnEnemyDefeat);
    REGISTER_EVENT(OnBossDefeat);

    // MARK: - Player specifics
    REGISTER_EVENT(OnTimestamp);
    REGISTER_EVENT(OnPlayerBonk);
    REGISTER_EVENT(OnPlayerSetModels);
    REGISTER_EVENT(OnPlayerHealthChange);
    REGISTER_EVENT(OnPlayerBottleUpdate);
    REGISTER_EVENT(OnPlayerHoldUpShield);
    REGISTER_EVENT(OnPlayerFirstPersonControl);
    REGISTER_EVENT(OnPlayerProcessStick);
    REGISTER_EVENT(OnPlayerShieldControl);

    // MARK: - Play State
    REGISTER_EVENT(OnPlayDestroy);
    REGISTER_EVENT(OnPlayDrawBegin);
    REGISTER_EVENT(OnPlayDrawEnd);

    // MARK: - Vanilla Behavior
    REGISTER_EVENT(OnVanillaBehavior);

    // MARK: - Save Files
    REGISTER_EVENT(OnSaveFile);
    REGISTER_EVENT(OnLoadFile);
    REGISTER_EVENT(OnDeleteFile);

    // MARK: - Dialog & UI
    REGISTER_EVENT(OnDialogMessage);
    REGISTER_EVENT(OnPresentTitleCard);
    REGISTER_EVENT(OnInterfaceUpdate);
    REGISTER_EVENT(OnKaleidoscopeUpdate);
    REGISTER_EVENT(OnMinimapDrawCompassIcons);

    // MARK: - Main Menu
    REGISTER_EVENT(OnPresentFileSelect);
    REGISTER_EVENT(OnUpdateFileSelectSelection);
    REGISTER_EVENT(OnUpdateFileSelectConfirmationSelection);
    REGISTER_EVENT(OnUpdateFileCopySelection);
    REGISTER_EVENT(OnUpdateFileCopyConfirmationSelection);
    REGISTER_EVENT(OnUpdateFileEraseSelection);
    REGISTER_EVENT(OnUpdateFileEraseConfirmationSelection);
    REGISTER_EVENT(OnUpdateFileAudioSelection);
    REGISTER_EVENT(OnUpdateFileTargetSelection);
    REGISTER_EVENT(OnUpdateFileLanguageSelection);
    REGISTER_EVENT(OnUpdateFileQuestSelection);
    REGISTER_EVENT(OnUpdateFileBossRushOptionSelection);
    REGISTER_EVENT(OnUpdateFileRandomizerOptionSelection);
    REGISTER_EVENT(OnUpdateFileNameSelection);
    REGISTER_EVENT(OnFileChooseMain);
    REGISTER_EVENT(OnGenerationCompletion);

    // MARK: - Game
    REGISTER_EVENT(OnSetGameLanguage);

    // MARK: - System
    REGISTER_EVENT(OnAssetAltChange);

    // MARK: - Pause Menu
    REGISTER_EVENT(OnKaleidoUpdate);

    // MARK: - Messages
    REGISTER_EVENT(OnOpenText);

    // MARK: - Audio
    REGISTER_EVENT(OnSeqPlayerInit);

    // MARK: - Randomizer
    REGISTER_EVENT(OnRandoSetCheckStatus);
    REGISTER_EVENT(OnRandoSetIsSkipped);
    REGISTER_EVENT(OnRandoEntranceDiscovered);
}
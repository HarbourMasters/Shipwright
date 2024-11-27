#include "GameInteractor_Hooks.h"

// MARK: - Gameplay

void GameInteractor_ExecuteOnLoadGame(int32_t fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnLoadGame>(fileNum);
}

void GameInteractor_ExecuteOnExitGame(int32_t fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnExitGame>(fileNum);
}

void GameInteractor_ExecuteOnGameFrameUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameFrameUpdate>();
}

void GameInteractor_ExecuteOnItemReceiveHooks(GetItemEntry itemEntry) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnItemReceive>(itemEntry);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnItemReceive>(itemEntry);
}

void GameInteractor_ExecuteOnSaleEndHooks(GetItemEntry itemEntry) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSaleEnd>(itemEntry);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSaleEnd>(itemEntry);
}

void GameInteractor_ExecuteOnTransitionEndHooks(int16_t sceneNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnTransitionEnd>(sceneNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnTransitionEnd>(sceneNum, sceneNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnTransitionEnd>(sceneNum);
}

void GameInteractor_ExecuteOnSceneInitHooks(int16_t sceneNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(sceneNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnSceneInit>(sceneNum, sceneNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneInit>(sceneNum);
}

void GameInteractor_ExecuteOnSceneFlagSet(int16_t sceneNum, int16_t flagType, int16_t flag) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneFlagSet>(sceneNum, flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneFlagSet>(sceneNum, flagType, flag);
}

void GameInteractor_ExecuteOnSceneFlagUnset(int16_t sceneNum, int16_t flagType, int16_t flag) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneFlagUnset>(sceneNum, flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneFlagUnset>(sceneNum, flagType, flag);
}

void GameInteractor_ExecuteOnFlagSet(int16_t flagType, int16_t flag) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFlagSet>(flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnFlagSet>(flagType, flag);
}

void GameInteractor_ExecuteOnFlagUnset(int16_t flagType, int16_t flag) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFlagUnset>(flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnFlagUnset>(flagType, flag);
}

void GameInteractor_ExecuteOnSceneSpawnActors() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneSpawnActors>();
}

void GameInteractor_ExecuteOnPlayerUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerUpdate>();
}

void GameInteractor_ExecuteOnOcarinaSongAction() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnOcarinaSongAction>();
}

void GameInteractor_ExecuteOnShopSlotChangeHooks(uint8_t cursorIndex, int16_t price) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnShopSlotChange>(cursorIndex, price);
}

void GameInteractor_ExecuteOnActorInit(void* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorInit>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorInit>(((Actor*)actor)->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorInit>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorInit>(actor);
}

void GameInteractor_ExecuteOnActorUpdate(void* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorUpdate>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorUpdate>(((Actor*)actor)->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorUpdate>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorUpdate>(actor);
}

void GameInteractor_ExecuteOnActorKill(void* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorKill>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorKill>(((Actor*)actor)->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorKill>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorKill>(actor);
}

void GameInteractor_ExecuteOnEnemyDefeat(void* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnEnemyDefeat>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnEnemyDefeat>(((Actor*)actor)->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnEnemyDefeat>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnEnemyDefeat>(actor);
}

void GameInteractor_ExecuteOnBossDefeat(void* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnBossDefeat>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnBossDefeat>(((Actor*)actor)->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnBossDefeat>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnBossDefeat>(actor);
}

void GameInteractor_ExecuteOnTimestamp (u8 item) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnTimestamp>(item);
}

void GameInteractor_ExecuteOnPlayerBonk() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerBonk>();
}

void GameInteractor_ExecuteOnPlayerHealthChange(int16_t amount) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerHealthChange>(amount);
}

void GameInteractor_ExecuteOnPlayerBottleUpdate(int16_t contents) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerBottleUpdate>(contents);
}

void GameInteractor_ExecuteOnPlayDestroy() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayDestroy>();
}

void GameInteractor_ExecuteOnPlayDrawEnd() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayDrawEnd>();
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

    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnVanillaBehavior>(flag, &boolResult, args);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnVanillaBehavior>(flag, flag, &boolResult, args);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnVanillaBehavior>(flag, &boolResult, args);

    va_end(args);
    return boolResult;
}

// MARK: -  Save Files

void GameInteractor_ExecuteOnSaveFile(int32_t fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSaveFile>(fileNum);
}

void GameInteractor_ExecuteOnLoadFile(int32_t fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnLoadFile>(fileNum);
}

void GameInteractor_ExecuteOnDeleteFile(int32_t fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnDeleteFile>(fileNum);
}

// MARK: - Dialog

void GameInteractor_ExecuteOnDialogMessage() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnDialogMessage>();
}

void GameInteractor_ExecuteOnPresentTitleCard() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPresentTitleCard>();
}

void GameInteractor_ExecuteOnInterfaceUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnInterfaceUpdate>();
}

void GameInteractor_ExecuteOnKaleidoscopeUpdate(int16_t inDungeonScene) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnKaleidoscopeUpdate>(inDungeonScene);
}

// MARK: - Main Menu

void GameInteractor_ExecuteOnPresentFileSelect() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPresentFileSelect>();
}

void GameInteractor_ExecuteOnUpdateFileSelectSelection(uint16_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileSelectSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileSelectConfirmationSelection(uint16_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileSelectConfirmationSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileCopySelection(uint16_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileCopySelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileCopyConfirmationSelection(uint16_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileCopyConfirmationSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileEraseSelection(uint16_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileEraseSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileEraseConfirmationSelection(uint16_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileEraseConfirmationSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileAudioSelection(uint8_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileAudioSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileTargetSelection(uint8_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileTargetSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileLanguageSelection(uint8_t optionIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileLanguageSelection>(optionIndex);
}

void GameInteractor_ExecuteOnUpdateFileQuestSelection(uint8_t questIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileQuestSelection>(questIndex);
}

void GameInteractor_ExecuteOnUpdateFileBossRushOptionSelection(uint8_t optionIndex, uint8_t optionValue) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileBossRushOptionSelection>(optionIndex, optionValue);
}

void GameInteractor_ExecuteOnUpdateFileNameSelection(int16_t charCode) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnUpdateFileNameSelection>(charCode);
}

// MARK: - Game

void GameInteractor_ExecuteOnSetGameLanguage() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
}

// MARK: - System

void GameInteractor_RegisterOnAssetAltChange(void (*fn)(void)) {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnAssetAltChange>(fn);
}

//MARK: Pause Menu

void GameInteractor_ExecuteOnKaleidoUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnKaleidoUpdate>();
}

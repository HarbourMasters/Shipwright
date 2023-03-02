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

void GameInteractor_ExecuteOnReceiveItemHooks(uint8_t item) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnReceiveItem>(item);
}

void GameInteractor_ExecuteOnSceneInitHooks(int16_t sceneNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(sceneNum);
}

void GameInteractor_ExecuteOnPlayerUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerUpdate>();
}

void GameInteractor_ExecuteOnOcarinaSongAction() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnOcarinaSongAction>();
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

// MARK: - Game

void GameInteractor_ExecuteOnSetGameLanguage() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
}

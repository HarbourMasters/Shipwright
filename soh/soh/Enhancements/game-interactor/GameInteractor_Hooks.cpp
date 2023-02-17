#include "GameInteractor_Hooks.h"

// MARK: - Gameplay

void GameInteractor_ExecuteOnReceiveItemHooks(uint8_t item) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnReceiveItem>(item);
}

void GameInteractor_ExecuteOnSceneInitHooks(int16_t sceneNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(sceneNum);
}

// MARK: -  Save Files

void GameInteractor_ExecuteOnSaveFile(int fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSaveFile>(fileNum);
}

void GameInteractor_ExecuteOnLoadFile(int fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnLoadFile>(fileNum);
}

void GameInteractor_ExecuteOnDeleteFile(int fileNum) {
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

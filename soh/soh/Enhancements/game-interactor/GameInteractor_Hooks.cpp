#include "GameInteractor_Hooks.h"

extern "C" {
extern PlayState* gPlayState;
}

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

void GameInteractor_ExecuteOnReceiveItemHooks(u8 item) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnReceiveItem>(item);
}

void GameInteractor_ExecuteOnSceneInitHooks(s16 sceneNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(sceneNum);
}

void GameInteractor_ExecuteOnPlayerUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerUpdate>();
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

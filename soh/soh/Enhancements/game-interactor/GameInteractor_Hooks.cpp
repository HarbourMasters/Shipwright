#include "GameInteractor_Hooks.h"

extern "C" {
extern PlayState* gPlayState;
}

// MARK: - Gameplay

void GameInteractor_ExecuteOnReceiveItemHooks(u8 item) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnReceiveItem>(item);
}

void GameInteractor_ExecuteOnSceneInitHooks(s16 sceneNum) {
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

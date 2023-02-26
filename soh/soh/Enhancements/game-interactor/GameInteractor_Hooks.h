#include "GameInteractor.h"

// MARK: - Gameplay
extern "C" void GameInteractor_ExecuteOnReceiveItemHooks(u8 item);
extern "C" void GameInteractor_ExecuteOnSceneInit(s16 sceneNum);
extern "C" void GameInteractor_ExecuteOnPlayerUpdate();

// MARK: -  Save Files
extern "C" void GameInteractor_ExecuteOnSaveFile(int fileNum);
extern "C" void GameInteractor_ExecuteOnLoadFile(int fileNum);
extern "C" void GameInteractor_ExecuteOnDeleteFile(int fileNum);

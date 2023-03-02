#include "GameInteractor.h"

// MARK: - Gameplay
extern "C" void GameInteractor_ExecuteOnLoadGame(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnExitGame(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnGameFrameUpdate();
extern "C" void GameInteractor_ExecuteOnReceiveItemHooks(u8 item);
extern "C" void GameInteractor_ExecuteOnSceneInit(s16 sceneNum);
extern "C" void GameInteractor_ExecuteOnPlayerUpdate();

// MARK: -  Save Files
extern "C" void GameInteractor_ExecuteOnSaveFile(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnLoadFile(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnDeleteFile(int32_t fileNum);

#include "GameInteractor.h"

// MARK: - Gameplay
extern "C" void GameInteractor_ExecuteOnReceiveItemHooks(uint8_t item);
extern "C" void GameInteractor_ExecuteOnSceneInit(int16_t sceneNum);

// MARK: -  Save Files
extern "C" void GameInteractor_ExecuteOnSaveFile(int fileNum);
extern "C" void GameInteractor_ExecuteOnLoadFile(int fileNum);
extern "C" void GameInteractor_ExecuteOnDeleteFile(int fileNum);

// MARK: - Dialog
extern "C" void GameInteractor_ExecuteOnDialogMessage();
extern "C" void GameInteractor_ExecuteOnPresentTitleCard();
extern "C" void GameInteractor_ExecuteOnInterfaceUpdate();
extern "C" void GameInteractor_ExecuteOnKaleidoscopeUpdate(int16_t inDungeonScene);

// MARK: - Main Menu
extern "C" void GameInteractor_ExecuteOnUpdateFileSelectSelection(uint16_t optionIndex);
extern "C" void GameInteractor_ExecuteOnPresentFileSelect();

// MARK: - Game
extern "C" void GameInteractor_ExecuteOnSetGameLanguage();

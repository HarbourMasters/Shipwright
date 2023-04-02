#include "GameInteractor.h"

// MARK: - Gameplay
extern "C" void GameInteractor_ExecuteOnLoadGame(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnExitGame(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnGameFrameUpdate();
extern "C" void GameInteractor_ExecuteOnItemReceiveHooks(GetItemEntry itemEntry);
extern "C" void GameInteractor_ExecuteOnSaleEndHooks(GetItemEntry itemEntry);
extern "C" void GameInteractor_ExecuteOnTransitionEndHooks(int16_t sceneNum);
extern "C" void GameInteractor_ExecuteOnSceneInit(int16_t sceneNum);
extern "C" void GameInteractor_ExecuteOnSceneSpawnActors();
extern "C" void GameInteractor_ExecuteOnPlayerUpdate();
extern "C" void GameInteractor_ExecuteOnOcarinaSongAction();
extern "C" void GameInteractor_ExecuteOnActorUpdate(void* actor);
extern "C" void GameInteractor_ExecuteOnPlayerBonk();
extern "C" void GameInteractor_ExecuteOnOcarinaSongAction();

// MARK: -  Save Files
extern "C" void GameInteractor_ExecuteOnSaveFile(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnLoadFile(int32_t fileNum);
extern "C" void GameInteractor_ExecuteOnDeleteFile(int32_t fileNum);

// MARK: - Dialog
extern "C" void GameInteractor_ExecuteOnDialogMessage();
extern "C" void GameInteractor_ExecuteOnPresentTitleCard();
extern "C" void GameInteractor_ExecuteOnInterfaceUpdate();
extern "C" void GameInteractor_ExecuteOnKaleidoscopeUpdate(int16_t inDungeonScene);

// MARK: - Main Menu
extern "C" void GameInteractor_ExecuteOnPresentFileSelect();
extern "C" void GameInteractor_ExecuteOnUpdateFileSelectSelection(uint16_t optionIndex);
extern "C" void GameInteractor_ExecuteOnUpdateFileCopySelection(uint16_t optionIndex);
extern "C" void GameInteractor_ExecuteOnUpdateFileCopyConfirmationSelection(uint16_t optionIndex);
extern "C" void GameInteractor_ExecuteOnUpdateFileEraseSelection(uint16_t optionIndex);
extern "C" void GameInteractor_ExecuteOnUpdateFileEraseConfirmationSelection(uint16_t optionIndex);
extern "C" void GameInteractor_ExecuteOnUpdateFileAudioSelection(uint8_t optionIndex);
extern "C" void GameInteractor_ExecuteOnUpdateFileTargetSelection(uint8_t optionIndex);

// MARK: - Game
extern "C" void GameInteractor_ExecuteOnSetGameLanguage();

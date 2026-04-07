#pragma once

#include "z64.h"
#include <stdarg.h>
#include <libultraship/bridge.h>
#include "vanilla-behavior/GIVanillaBehavior.h"

DEFINE_EVENT(OnZTitleInit, void* gameState;);

DEFINE_EVENT(OnZTitleUpdate, void* gameState;);

DEFINE_EVENT(OnLoadGame, int32_t fileNum;);

DEFINE_EVENT(OnExitGame, int32_t fileNum;);

DEFINE_EVENT(OnGameStateMainStart);

DEFINE_EVENT(OnGameFrameUpdate);

DEFINE_EVENT(OnCameraState, PlayState* play;);

DEFINE_EVENT(OnItemReceive, GetItemEntry itemEntry;);

DEFINE_EVENT(OnEquipmentDelete, int16_t equipmentType; uint16_t equipValue;);

DEFINE_EVENT(OnSaleEnd, GetItemEntry itemEntry;);

DEFINE_EVENT(OnTransitionEnd, int16_t sceneNum;);

DEFINE_EVENT(OnSceneInit, int16_t sceneNum;);

DEFINE_EVENT(AfterSceneCommands, int16_t sceneNum;);

DEFINE_EVENT(OnSceneFlagSet, int16_t sceneNum; int16_t flagType; int16_t flag;);

DEFINE_EVENT(OnSceneFlagUnset, int16_t sceneNum; int16_t flagType; int16_t flag;);

DEFINE_EVENT(OnFlagSet, int16_t flagType; int16_t flag;);

DEFINE_EVENT(OnFlagUnset, int16_t flagType; int16_t flag;);

DEFINE_EVENT(OnSceneSpawnActors);

DEFINE_EVENT(OnLinkSkeletonInit);

DEFINE_EVENT(OnLinkEquipmentChange);

DEFINE_EVENT(OnPlayerUpdate);

DEFINE_EVENT(OnSetDoAction, uint16_t action;);

DEFINE_EVENT(OnPlayerSfx, u16 sfxId;);

DEFINE_EVENT(OnOcarinaSongAction);

DEFINE_EVENT(OnOcarinaNote, uint8_t note; float modulator; int8_t bend;);

DEFINE_EVENT(OnCuccoOrChickenHatch);

DEFINE_EVENT(OnShopSlotChange, uint8_t cursorIndex; int16_t price;);

DEFINE_EVENT(OnDungeonKeyUsed, uint16_t mapIndex;);

DEFINE_EVENT(ShouldActorInit, void* actor; bool* result;);

DEFINE_EVENT(OnActorInit, void* actor;);

DEFINE_EVENT(OnActorSpawn, void* actor;);

DEFINE_EVENT(ShouldActorUpdate, void* actor; bool* result;);

DEFINE_EVENT(OnActorUpdate, void* actor;);

DEFINE_EVENT(OnActorKill, void* actor;);

DEFINE_EVENT(OnActorDestroy, void* actor;);

DEFINE_EVENT(OnEnemyDefeat, void* actor;);

DEFINE_EVENT(OnBossDefeat, void* actor;);

DEFINE_EVENT(OnTimestamp, u8 item;);

DEFINE_EVENT(OnPlayerBonk);

DEFINE_EVENT(OnPlayerSetModels, Player* player; u8 modelGroup;);

DEFINE_EVENT(OnPlayerHealthChange, int16_t amount;);

DEFINE_EVENT(OnPlayerBottleUpdate, int16_t contents;);

DEFINE_EVENT(OnPlayerHoldUpShield);

DEFINE_EVENT(OnPlayerFirstPersonControl, Player* player;);

DEFINE_EVENT(OnPlayerProcessStick);

DEFINE_EVENT(OnPlayerShieldControl, float* sp50; float* sp54;);

DEFINE_EVENT(OnPlayDestroy);

DEFINE_EVENT(OnPlayDrawBegin);

DEFINE_EVENT(OnPlayDrawEnd);

DEFINE_EVENT(OnVanillaBehavior, GIVanillaBehavior flag; bool* result; va_list * originalArgs;);

DEFINE_EVENT(OnSaveFile, int32_t fileNum; int32_t sectionID;);

DEFINE_EVENT(OnLoadFile, int32_t fileNum;);

DEFINE_EVENT(OnDeleteFile, int32_t fileNum;);

DEFINE_EVENT(OnDialogMessage);

DEFINE_EVENT(OnPresentTitleCard);

DEFINE_EVENT(OnInterfaceUpdate);

DEFINE_EVENT(OnKaleidoscopeUpdate, int16_t inDungeonScene;);

DEFINE_EVENT(OnMinimapDrawCompassIcons);

DEFINE_EVENT(OnPresentFileSelect);

DEFINE_EVENT(OnUpdateFileSelectSelection, uint16_t optionIndex;);

DEFINE_EVENT(OnUpdateFileSelectConfirmationSelection, uint16_t optionIndex;);

DEFINE_EVENT(OnUpdateFileCopySelection, uint16_t optionIndex;);

DEFINE_EVENT(OnUpdateFileCopyConfirmationSelection, uint16_t optionIndex;);

DEFINE_EVENT(OnUpdateFileEraseSelection, uint16_t optionIndex;);

DEFINE_EVENT(OnUpdateFileEraseConfirmationSelection, uint16_t optionIndex;);

DEFINE_EVENT(OnUpdateFileAudioSelection, uint8_t optionIndex;);

DEFINE_EVENT(OnUpdateFileTargetSelection, uint8_t optionIndex;);

DEFINE_EVENT(OnUpdateFileLanguageSelection, uint8_t optionIndex;);

DEFINE_EVENT(OnUpdateFileQuestSelection, uint8_t questIndex;);

DEFINE_EVENT(OnUpdateFileBossRushOptionSelection, uint8_t optionIndex; uint8_t optionValue;);

DEFINE_EVENT(OnUpdateFileRandomizerOptionSelection, uint8_t optionIndex;);

DEFINE_EVENT(OnUpdateFileNameSelection, int16_t charCode;);

DEFINE_EVENT(OnFileChooseMain, void* gameState;);

DEFINE_EVENT(OnGenerationCompletion);

DEFINE_EVENT(OnSetGameLanguage);

DEFINE_EVENT(OnAssetAltChange);

DEFINE_EVENT(OnKaleidoUpdate);

// Messages
DEFINE_EVENT(OnOpenText, uint16_t* textId; bool* loadFromMessageTable;);

// Audio
DEFINE_EVENT(OnSeqPlayerInit, int32_t playerIdx; int32_t seqId;);

// Rando
DEFINE_EVENT(OnRandoSetCheckStatus, RandomizerCheck rc; RandomizerCheckStatus status;);

DEFINE_EVENT(OnRandoSetIsSkipped, RandomizerCheck rc; bool isSkipped;);

DEFINE_EVENT(OnRandoEntranceDiscovered, u16 entranceIndex; u8 isReversedEntrance;);
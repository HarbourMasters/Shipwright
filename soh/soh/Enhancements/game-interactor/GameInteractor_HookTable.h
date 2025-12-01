/**
 * Hook Table
 *
 * DEFINE_HOOK arguments:
 *    - Argument 1: Name of the hook
 *    - Argument 2: Function type that the hook uses
 */
DEFINE_HOOK(OnZTitleInit, (void* gameState));
DEFINE_HOOK(OnZTitleUpdate, (void* gameState));
DEFINE_HOOK(OnLoadGame, (int32_t fileNum));
DEFINE_HOOK(OnExitGame, (int32_t fileNum));
DEFINE_HOOK(OnGameStateMainStart, ());
DEFINE_HOOK(OnGameFrameUpdate, ());
DEFINE_HOOK(OnItemReceive, (GetItemEntry itemEntry));
DEFINE_HOOK(OnEquipmentDelete, (int16_t equipmentType, uint16_t equipValue));
DEFINE_HOOK(OnSaleEnd, (GetItemEntry itemEntry));
DEFINE_HOOK(OnTransitionEnd, (int16_t sceneNum));
DEFINE_HOOK(OnSceneInit, (int16_t sceneNum));
DEFINE_HOOK(AfterSceneCommands, (int16_t sceneNum));
DEFINE_HOOK(OnSceneFlagSet, (int16_t sceneNum, int16_t flagType, int16_t flag));
DEFINE_HOOK(OnSceneFlagUnset, (int16_t sceneNum, int16_t flagType, int16_t flag));
DEFINE_HOOK(OnFlagSet, (int16_t flagType, int16_t flag));
DEFINE_HOOK(OnFlagUnset, (int16_t flagType, int16_t flag));
DEFINE_HOOK(OnSceneSpawnActors, ());
DEFINE_HOOK(OnPlayerUpdate, ());
DEFINE_HOOK(OnSetDoAction, (uint16_t action));
DEFINE_HOOK(OnPlayerSfx, (u16 sfxId));
DEFINE_HOOK(OnOcarinaSongAction, ());
DEFINE_HOOK(OnOcarinaNote, (uint8_t note, float modulator, int8_t bend));
DEFINE_HOOK(OnCuccoOrChickenHatch, ());
DEFINE_HOOK(OnShopSlotChange, (uint8_t cursorIndex, int16_t price));
DEFINE_HOOK(OnDungeonKeyUsed, (uint16_t mapIndex));
DEFINE_HOOK(ShouldActorInit, (void* actor, bool* result));
DEFINE_HOOK(OnActorInit, (void* actor));
DEFINE_HOOK(OnActorSpawn, (void* actor));
DEFINE_HOOK(ShouldActorUpdate, (void* actor, bool* result));
DEFINE_HOOK(OnActorUpdate, (void* actor));
DEFINE_HOOK(OnActorKill, (void* actor));
DEFINE_HOOK(OnActorDestroy, (void* actor));
DEFINE_HOOK(OnEnemyDefeat, (void* actor));
DEFINE_HOOK(OnBossDefeat, (void* actor));
DEFINE_HOOK(OnTimestamp, (u8 item));
DEFINE_HOOK(OnPlayerBonk, ());
DEFINE_HOOK(OnPlayerHealthChange, (int16_t amount));
DEFINE_HOOK(OnPlayerBottleUpdate, (int16_t contents));
DEFINE_HOOK(OnPlayerHoldUpShield, ());
DEFINE_HOOK(OnPlayerFirstPersonControl, (Player * player));
DEFINE_HOOK(OnPlayerProcessStick, ());
DEFINE_HOOK(OnPlayerShieldControl, (float_t * sp50, float_t* sp54));
DEFINE_HOOK(OnPlayDestroy, ());
DEFINE_HOOK(OnPlayDrawBegin, ());
DEFINE_HOOK(OnPlayDrawEnd, ());
DEFINE_HOOK(OnVanillaBehavior, (GIVanillaBehavior flag, bool* result, va_list originalArgs));
DEFINE_HOOK(OnSaveFile, (int32_t fileNum, int32_t sectionID));
DEFINE_HOOK(OnLoadFile, (int32_t fileNum));
DEFINE_HOOK(OnDeleteFile, (int32_t fileNum));

DEFINE_HOOK(OnDialogMessage, ());
DEFINE_HOOK(OnPresentTitleCard, ());
DEFINE_HOOK(OnInterfaceUpdate, ());
DEFINE_HOOK(OnKaleidoscopeUpdate, (int16_t inDungeonScene));

DEFINE_HOOK(OnPresentFileSelect, ());
DEFINE_HOOK(OnUpdateFileSelectSelection, (uint16_t optionIndex));
DEFINE_HOOK(OnUpdateFileSelectConfirmationSelection, (uint16_t optionIndex));
DEFINE_HOOK(OnUpdateFileCopySelection, (uint16_t optionIndex));
DEFINE_HOOK(OnUpdateFileCopyConfirmationSelection, (uint16_t optionIndex));
DEFINE_HOOK(OnUpdateFileEraseSelection, (uint16_t optionIndex));
DEFINE_HOOK(OnUpdateFileEraseConfirmationSelection, (uint16_t optionIndex));
DEFINE_HOOK(OnUpdateFileAudioSelection, (uint8_t optionIndex));
DEFINE_HOOK(OnUpdateFileTargetSelection, (uint8_t optionIndex));
DEFINE_HOOK(OnUpdateFileLanguageSelection, (uint8_t optionIndex));
DEFINE_HOOK(OnUpdateFileQuestSelection, (uint8_t questIndex));
DEFINE_HOOK(OnUpdateFileBossRushOptionSelection, (uint8_t optionIndex, uint8_t optionValue));
DEFINE_HOOK(OnUpdateFileRandomizerOptionSelection, (uint8_t optionIndex));
DEFINE_HOOK(OnUpdateFileNameSelection, (int16_t charCode));
DEFINE_HOOK(OnFileChooseMain, (void* gameState));
DEFINE_HOOK(OnGenerationCompletion, ());

DEFINE_HOOK(OnSetGameLanguage, ());
DEFINE_HOOK(OnAssetAltChange, ());
DEFINE_HOOK(OnKaleidoUpdate, ());

// Audio
DEFINE_HOOK(OnSeqPlayerInit, (int32_t playerIdx, int32_t seqId));

// Rando
DEFINE_HOOK(OnRandoSetCheckStatus, (RandomizerCheck rc, RandomizerCheckStatus status));
DEFINE_HOOK(OnRandoSetIsSkipped, (RandomizerCheck rc, bool isSkipped));
DEFINE_HOOK(OnRandoEntranceDiscovered, (u16 entranceIndex, u8 isReversedEntrance));

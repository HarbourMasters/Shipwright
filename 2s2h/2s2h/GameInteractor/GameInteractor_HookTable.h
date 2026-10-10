DEFINE_HOOK(OnGameStateMainStart, ())
DEFINE_HOOK(OnGameStateMainFinish, ())
DEFINE_HOOK(OnGameStateDrawFinish, ())
DEFINE_HOOK(OnGameStateUpdate, ())
DEFINE_HOOK(OnConsoleLogoUpdate, ())
DEFINE_HOOK(OnKaleidoUpdate, (PauseContext * pauseCtx))
DEFINE_HOOK(BeforeKaleidoDrawPage, (PauseContext * pauseCtx, u16 pauseIndex))
DEFINE_HOOK(AfterKaleidoDrawPage, (PauseContext * pauseCtx, u16 pauseIndex))
DEFINE_HOOK(OnSaveInit, (s16 fileNum))
DEFINE_HOOK(OnSaveLoad, (s16 fileNum))
DEFINE_HOOK(OnFileSelectSaveLoad, (s16 fileNum, bool isOwlSave, SaveContext* saveContext))
DEFINE_HOOK(BeforeEndOfCycleSave, ())
DEFINE_HOOK(AfterEndOfCycleSave, ())
DEFINE_HOOK(BeforeMoonCrash, ())
DEFINE_HOOK(OnInterfaceDrawStart, ())
DEFINE_HOOK(AfterInterfaceClockDraw, ())
DEFINE_HOOK(BeforeInterfaceClockDraw, ())
DEFINE_HOOK(OnGameCompletion, ())

DEFINE_HOOK(OnSceneInit, (s8 sceneId, s8 spawnNum))
DEFINE_HOOK(OnRoomInit, (s8 sceneId, s8 roomNum))
DEFINE_HOOK(AfterRoomSceneCommands, (s8 sceneId, s8 roomNum))
DEFINE_HOOK(OnPlayDrawWorldStart, ())
DEFINE_HOOK(OnPlayDrawWorldEnd, ())
DEFINE_HOOK(OnPlayDestroy, ())

DEFINE_HOOK(ShouldActorInit, (Actor * actor, bool* should))
DEFINE_HOOK(OnActorInit, (Actor * actor))
DEFINE_HOOK(ShouldActorUpdate, (Actor * actor, bool* should))
DEFINE_HOOK(OnActorUpdate, (Actor * actor))
DEFINE_HOOK(ShouldActorDraw, (Actor * actor, bool* should))
DEFINE_HOOK(OnActorDraw, (Actor * actor))
DEFINE_HOOK(OnActorKill, (Actor * actor))
DEFINE_HOOK(OnActorDestroy, (Actor * actor))
DEFINE_HOOK(OnPlayerPostLimbDraw, (Player * player, s32 limbIndex))
DEFINE_HOOK(OnPlayerReleaseHeldActor, (PlayState * play, Player* player, Actor* heldActor))
DEFINE_HOOK(OnBossDefeated, (s16 actorId))

DEFINE_HOOK(OnSceneFlagSet, (s16 sceneId, FlagType flagType, u32 flag))
DEFINE_HOOK(OnSceneFlagUnset, (s16 sceneId, FlagType flagType, u32 flag))
DEFINE_HOOK(OnFlagSet, (FlagType flagType, u32 flag))
DEFINE_HOOK(OnFlagUnset, (FlagType flagType, u32 flag))

DEFINE_HOOK(AfterCameraUpdate, (Camera * camera))
DEFINE_HOOK(OnCameraChangeModeFlags, (Camera * camera))
DEFINE_HOOK(OnCameraChangeSettingsFlags, (Camera * camera))

DEFINE_HOOK(OnPassPlayerInputs, (Input * input))

DEFINE_HOOK(OnOpenText, (u16 * textId, bool* loadFromMessageTable))

DEFINE_HOOK(ShouldItemGive, (u8 item, bool* should))
DEFINE_HOOK(OnItemGive, (u8 item))

DEFINE_HOOK(OnBottleContentsUpdate, (u8 item))

DEFINE_HOOK(ShouldVanillaBehavior, (GIVanillaBehavior flag, bool* should, va_list originalArgs))

// Audio
DEFINE_HOOK(OnSeqPlayerInit, (s32 playerIdx, s32 seqId));

// Rando
DEFINE_HOOK(OnRandoSeedGeneration, ());

#include "BenPort.h"
#include "global.h"
#include <ship/resource/type/Blob.h>
#include <memory>
#include <cassert>
#include <ship/utils/StringHelper.h>
#include <fast/resource/type/DisplayList.h>
#include <libultraship/bridge/resourcebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/resource/type/Scene.h"
#include "2s2h/resource/type/CollisionHeader.h"
#include "2s2h/resource/type/Path.h"
#include "2s2h/resource/type/scenecommand/SetCameraSettings.h"
#include "2s2h/resource/type/scenecommand/SetCutscenes.h"
#include "2s2h/resource/type/scenecommand/SetStartPositionList.h"
#include "2s2h/resource/type/scenecommand/SetActorList.h"
#include "2s2h/resource/type/scenecommand/SetCollisionHeader.h"
#include "2s2h/resource/type/scenecommand/SetRoomList.h"
#include "2s2h/resource/type/scenecommand/SetEntranceList.h"
#include "2s2h/resource/type/scenecommand/SetSpecialObjects.h"
#include "2s2h/resource/type/scenecommand/SetRoomBehavior.h"
#include "2s2h/resource/type/scenecommand/SetMesh.h"
#include "2s2h/resource/type/scenecommand/SetObjectList.h"
#include "2s2h/resource/type/scenecommand/SetLightList.h"
#include "2s2h/resource/type/scenecommand/SetLightingSettings.h"
#include "2s2h/resource/type/scenecommand/SetPathways.h"
#include "2s2h/resource/type/scenecommand/SetTransitionActorList.h"
#include "2s2h/resource/type/scenecommand/SetSkyboxSettings.h"
#include "2s2h/resource/type/scenecommand/SetSkyboxModifier.h"
#include "2s2h/resource/type/scenecommand/SetTimeSettings.h"
#include "2s2h/resource/type/scenecommand/SetWindSettings.h"
#include "2s2h/resource/type/scenecommand/SetSoundSettings.h"
#include "2s2h/resource/type/scenecommand/SetEchoSettings.h"
#include "2s2h/resource/type/scenecommand/SetAlternateHeaders.h"
#include "2s2h/resource/type/scenecommand/SetActorCutsceneList.h"
#include "2s2h/resource/type/scenecommand/SetAnimatedMaterialList.h"
#include "2s2h/resource/type/scenecommand/SetMinimapList.h"
#include "2s2h/resource/type/scenecommand/SetMinimapChests.h"
#include "2s2h/resource/type/scenecommand/SetCsCamera.h"

s32 OTRScene_ExecuteCommands(PlayState* play, SOH::Scene* scene);

void Scene_CommandSpawnList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetStartPositionList* list = (SOH::SetStartPositionList*)cmd;
    ActorEntry* entries = (ActorEntry*)(list->GetRawPointer());
    s32 loadedCount;
    s16 playerObjectId;
    void* objectPtr;

    play->linkActorEntry = &entries[play->setupEntranceList[play->curSpawn].spawn];

    if ((PLAYER_GET_START_MODE(play->linkActorEntry) == PLAYER_START_MODE_TELESCOPE) ||
        ((gSaveContext.respawnFlag == 2) && (gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams ==
                                             PLAYER_PARAMS(0xFF, PLAYER_START_MODE_TELESCOPE)))) {
        // Skull Kid Object
        Object_SpawnPersistent(&play->objectCtx, OBJECT_STK);
        return;
    }

    loadedCount = Object_SpawnPersistent(&play->objectCtx, OBJECT_LINK_CHILD);
    objectPtr = play->objectCtx.slots[play->objectCtx.numEntries].segment;
    play->objectCtx.numEntries = loadedCount;
    play->objectCtx.numPersistentEntries = loadedCount;
    playerObjectId = gPlayerFormObjectIds[GET_PLAYER_FORM];
    gActorOverlayTable[0].profile->objectId = playerObjectId;
    Object_SpawnPersistent(&play->objectCtx, playerObjectId);

    play->objectCtx.slots[play->objectCtx.numEntries].segment = objectPtr;
}

void Scene_CommandActorList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetActorList* list = (SOH::SetActorList*)cmd;

    play->numSetupActors = list->numActors;
    play->actorCtx.halfDaysBit = 0;
    play->setupActorList = (ActorEntry*)list->GetRawPointer();
}

void Scene_CommandActorCutsceneCamList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetCsCamera* cams = (SOH::SetCsCamera*)cmd;

    play->actorCsCamList = (ActorCsCamInfo*)cams->GetPointer();
}

void Scene_CommandCollisionHeader(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetCollisionHeader* colHeader = (SOH::SetCollisionHeader*)cmd;
    BgCheck_Allocate(&play->colCtx, play, (CollisionHeader*)colHeader->GetRawPointer());
}

void Scene_CommandRoomList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetRoomList* list = (SOH::SetRoomList*)cmd;
    play->roomList.count = list->numRooms;
    play->roomList.romFiles = (RomFile*)list->GetPointer();
}

void Scene_CommandWindSettings(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetWindSettings* settings = (SOH::SetWindSettings*)cmd;

    play->envCtx.windDirection.x = settings->settings.windWest;
    play->envCtx.windDirection.y = settings->settings.windVertical;
    play->envCtx.windDirection.z = settings->settings.windSouth;
    play->envCtx.windSpeed = settings->settings.windSpeed;
}

void Scene_CommandEntranceList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetEntranceList* list = (SOH::SetEntranceList*)cmd;

    play->setupEntranceList = (EntranceEntry*)list->GetRawPointer();
}

void Scene_CommandSpecialFiles(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetSpecialObjects* specialObjList = (SOH::SetSpecialObjects*)cmd;
    // static RomFile naviQuestHintFiles[2] = {
    //     { SEGMENT_ROM_START(elf_message_field), SEGMENT_ROM_END(elf_message_field) },
    //     { SEGMENT_ROM_START(elf_message_ydan), SEGMENT_ROM_END(elf_message_ydan) },
    // };

    if (specialObjList->specialObjects.globalObject != 0) {
        play->objectCtx.subKeepSlot =
            Object_SpawnPersistent(&play->objectCtx, specialObjList->specialObjects.globalObject);
        // ZRET TODO: Segment number enum?
        // gSegments[0x05] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[play->objectCtx.subKeepSlot].segment);
    }

    // BENTODO: Figure out if the following section is needed for something
    // if (specialObjList->specialObjects.elfMessage != NAVI_QUEST_HINTS_NONE) {
    //     play->naviQuestHints = Play_LoadFile(play, &naviQuestHintFiles[specialObjList->specialObjects.elfMessage -
    //     1]);
    // }
}

void Scene_CommandRoomBehavior(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetRoomBehaviorMM* behavior = (SOH::SetRoomBehaviorMM*)cmd;

    play->roomCtx.curRoom.type = behavior->roomBehavior.gameplayFlags;
    play->roomCtx.curRoom.environmentType = behavior->roomBehavior.currRoomUnk2;
    play->roomCtx.curRoom.lensMode = behavior->roomBehavior.currRoomUnk5;
    play->msgCtx.unk12044 = behavior->roomBehavior.msgCtxUnk;
    play->roomCtx.curRoom.enablePosLights = behavior->roomBehavior.enablePointLights;
    play->envCtx.stormState = behavior->roomBehavior.kankyoContextUnkE2;
    // play->roomCtx.curRoom.type = behavior->roomBehavior.gameplayFlags;
    // play->roomCtx.curRoom.environmentType = behavior->roomBehavior.gameplayFlags2 & 0xFF;
    // play->roomCtx.curRoom.lensMode = (behavior->roomBehavior.gameplayFlags2 >> 8) & 1;
    // play->msgCtx.unk12044 = (behavior->roomBehavior.gameplayFlags2 >> 0xA) & 1;
    // play->roomCtx.curRoom.enablePosLights = (behavior->roomBehavior.gameplayFlags2 >> 0xB) & 1;
    // play->envCtx.stormState = (behavior->roomBehavior.gameplayFlags2 >> 0xC) & 1;
}

void Scene_Command09(PlayState* play, SOH::ISceneCommand* cmd) {
    // Empty in z_scene.c
}

void Scene_CommandMesh(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetMesh* mesh = (SOH::SetMesh*)cmd;

    play->roomCtx.curRoom.roomShape = (RoomShape*)mesh->GetRawPointer();
}

void Scene_CommandObjectList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetObjectList* objList = (SOH::SetObjectList*)cmd;
    s32 i;
    s32 j;
    s32 k;

    // #region 2S2H [Port] Cleaner version of decomps loops for nicer presentation

    // Loop until a mismatch in the object lists
    // Then clear all object ids past that in the context object list and kill actors for those objects
    for (i = play->objectCtx.numPersistentEntries, k = 0; i < play->objectCtx.numEntries; i++, k++) {
        if (k >= objList->objects.size() || play->objectCtx.slots[i].id != objList->objects[k]) {
            for (j = i; j < play->objectCtx.numEntries; j++) {
                play->objectCtx.slots[j].id = 0;
            }
            Actor_KillAllWithMissingObject(play, &play->actorCtx);
            break;
        }
    }

    // Continuing from the last index, add the remaining object ids from the command object list
    for (; k < objList->objects.size(); i++, k++) {
        play->objectCtx.slots[i].id = -objList->objects[k];
    }

    play->objectCtx.numEntries = i;

    // #endregion

    // Original Compatible Code Commented

    // s32 i;
    // s32 j;
    // s32 k;
    // ObjectEntry* firstObject;
    // ObjectEntry* entry;
    // ObjectEntry* invalidatedEntry;
    // s16* objectEntry;
    // void* nextPtr;

    // s16* objectEntry = (s16*)objList->GetRawPointer();
    // k = 0;
    // i = play->objectCtx.numPersistentEntries;
    // entry = &play->objectCtx.slots[i];
    // firstObject = &play->objectCtx.slots[0];

    // while (i < play->objectCtx.numEntries) {
    //     if (entry->id != *objectEntry) {
    //         invalidatedEntry = &play->objectCtx.slots[i];

    //         for (j = i; j < play->objectCtx.numEntries; j++) {
    //             invalidatedEntry->id = 0;
    //             invalidatedEntry++;
    //         }

    //         play->objectCtx.numEntries = i;
    //         Actor_KillAllWithMissingObject(play, &play->actorCtx);

    //         continue;
    //     }

    //     i++;
    //     k++;
    //     objectEntry++;
    //     entry++;
    // }

    // while (k < objList->objects.size()) {
    //     nextPtr = func_8012F73C(&play->objectCtx, i, *objectEntry);

    //     if (i < ARRAY_COUNT(play->objectCtx.slots) - 1) {
    //         firstObject[i + 1].segment = nextPtr;
    //     }

    //     i++;
    //     k++;
    //     objectEntry++;
    // }

    // play->objectCtx.numEntries = i;
}

void Scene_CommandLightList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetLightList* lightList = (SOH::SetLightList*)cmd;

    for (unsigned int i = 0; i < lightList->numLights; i++) {
        LightContext_InsertLight(play, &play->lightCtx, (LightInfo*)&lightList->lightList[i]);
    }
}

void Scene_CommandPathList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetPathwaysMM* paths = (SOH::SetPathwaysMM*)cmd;

    play->setupPathList = (Path*)paths->GetPointer()[0];
}

void Scene_CommandTransiActorList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetTransitionActorList* list = (SOH::SetTransitionActorList*)cmd;

    play->transitionActors.count = list->numTransitionActors;
    play->transitionActors.list = (TransitionActorEntry*)list->GetRawPointer();
    MapDisp_InitTransitionActorData(play, play->transitionActors.count, play->transitionActors.list);
}

void Scene_CommandEnvLightSettings(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetLightingSettings* lightSettings = (SOH::SetLightingSettings*)cmd;

    play->envCtx.numLightSettings = lightSettings->settings.size();
    play->envCtx.lightSettingsList = (EnvLightSettings*)lightSettings->GetRawPointer();
}

void Scene_CommandTimeSettings(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetTimeSettings* settings = (SOH::SetTimeSettings*)cmd;

    if ((settings->settings.hour != 0xFF) && (settings->settings.minute != 0xFF)) {
        gSaveContext.skyboxTime = gSaveContext.save.time =
            CLOCK_TIME_ALT2_F(settings->settings.hour, settings->settings.minute);
    }

    if (settings->settings.timeIncrement != 0xFF) {
        play->envCtx.sceneTimeSpeed = settings->settings.timeIncrement;
    } else {
        play->envCtx.sceneTimeSpeed = 0;
    }

    // Increase time speed during first cycle
    if (GameInteractor_Should(VB_FASTER_FIRST_CYCLE,
                              (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) &&
                                  (play->envCtx.sceneTimeSpeed != 0))) {
        play->envCtx.sceneTimeSpeed = 5;
    }

    if (gSaveContext.sunsSongState == SUNSSONG_INACTIVE) {
        R_TIME_SPEED = play->envCtx.sceneTimeSpeed;
    }

    play->envCtx.sunPos.x = -(Math_SinS(CURRENT_TIME - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    play->envCtx.sunPos.y = (Math_CosS(CURRENT_TIME - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    play->envCtx.sunPos.z = (Math_CosS(CURRENT_TIME - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;

    if ((play->envCtx.sceneTimeSpeed == 0) && (gSaveContext.save.cutsceneIndex < 0xFFF0)) {
        gSaveContext.skyboxTime = CURRENT_TIME;

        if ((gSaveContext.skyboxTime >= CLOCK_TIME(4, 0)) && (gSaveContext.skyboxTime < CLOCK_TIME(6, 30))) {
            gSaveContext.skyboxTime = CLOCK_TIME(5, 0);
        } else if ((gSaveContext.skyboxTime >= CLOCK_TIME(6, 30)) && (gSaveContext.skyboxTime < CLOCK_TIME(8, 0))) {
            gSaveContext.skyboxTime = CLOCK_TIME(8, 0);
        } else if ((gSaveContext.skyboxTime >= CLOCK_TIME(16, 0)) && (gSaveContext.skyboxTime < CLOCK_TIME(17, 0))) {
            gSaveContext.skyboxTime = CLOCK_TIME(17, 0);
        } else if ((gSaveContext.skyboxTime >= CLOCK_TIME(18, 0)) && (gSaveContext.skyboxTime < CLOCK_TIME(19, 0))) {
            gSaveContext.skyboxTime = CLOCK_TIME(19, 0);
        }
    }
}

void Scene_CommandSkyboxSettings(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetSkyboxSettings* settings = (SOH::SetSkyboxSettings*)cmd;

    play->skyboxId = settings->settings.skyboxId & 3;
    // BENTODO z_scene.c reads from skyboxSettings.skyboxConfig not weather
    // Settings uses names from OOT
    play->envCtx.skyboxConfig = play->envCtx.changeSkyboxNextConfig = settings->settings.weather;
    play->envCtx.lightMode = settings->settings.indoors;
    // Scene_LoadAreaTextures(play, settings->settings.)
}

void Scene_CommandSkyboxDisables(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetSkyboxModifier* mod = (SOH::SetSkyboxModifier*)cmd;

    play->envCtx.skyboxDisabled = mod->modifier.skyboxDisabled;
    play->envCtx.sunDisabled = mod->modifier.sunMoonDisabled;
}

void Scene_CommandExitList(PlayState* play, SOH::ISceneCommand* cmd) {
    play->setupExitList = (u16*)cmd->GetRawPointer();
}

void Scene_CommandSoundSettings(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetSoundSettings* settings = (SOH::SetSoundSettings*)cmd;

    play->sceneSequences.seqId = settings->settings.seqId;
    play->sceneSequences.ambienceId = settings->settings.natureAmbienceId;

    if (gSaveContext.seqId == NA_BGM_DISABLED || AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_FINAL_HOURS) {
        Audio_SetSpec(settings->settings.reverb); // BENTODO Verify if this should be reverb
    }
}

void Scene_CommandEchoSetting(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetEchoSettings* echo = (SOH::SetEchoSettings*)cmd;
    play->roomCtx.curRoom.echo = echo->settings.echo;
}

void Scene_CommandCutsceneScriptList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetCutscenesMM* cs = (SOH::SetCutscenesMM*)cmd;
    play->csCtx.scriptListCount = cs->entries.size();
    // BENTODO do this the right way with get pointer
    play->csCtx.scriptList = (CutsceneScriptEntry*)cs->entries.data();
}

static bool shouldEndSceneCommands = false;
void Scene_CommandAltHeaderList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetAlternateHeaders* headers = (SOH::SetAlternateHeaders*)cmd;

    if (gSaveContext.sceneLayer != 0) {
        SOH::Scene* desiredHeader =
            std::static_pointer_cast<SOH::Scene>(headers->headers[gSaveContext.sceneLayer - 1]).get();

        if (desiredHeader != nullptr) {
            OTRScene_ExecuteCommands(play, desiredHeader);
            // 2S2H [Port] The original source would grab the next command after the alternate header list
            // and change the command id to SCENE_CMD_ID_END. We can't modify LUS resources, so we'll just
            // set a flag to end the scene commands
            shouldEndSceneCommands = true;
        }
    }
}

void Scene_CommandSetRegionVisitedFlag(PlayState* play, SOH::ISceneCommand* cmd) {
    s16 j = 0;
    s16 i = 0;

    while (true) {
        if (gSceneIdsPerRegion[i][j] == 0xFFFF) {
            i++;
            j = 0;

            if (i == REGION_MAX) {
                break;
            }
        }

        if (play->sceneId == gSceneIdsPerRegion[i][j]) {
            break;
        }

        j++;
    }

    if (i < REGION_MAX) {
        gSaveContext.save.saveInfo.regionsVisited =
            (gBitFlags[i] | gSaveContext.save.saveInfo.regionsVisited) | gSaveContext.save.saveInfo.regionsVisited;
    }
}

void Scene_CommandAnimatedMaterials(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetAnimatedMaterialList* list = (SOH::SetAnimatedMaterialList*)cmd;
    play->sceneMaterialAnims = (AnimatedMaterial*)list->mat;
}

void Scene_CommandCutsceneList(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetActorCutsceneList* list = (SOH::SetActorCutsceneList*)cmd;

    CutsceneManager_Init(play, (ActorCutscene*)list->GetPointer(), list->entries.size());
}

void Scene_CommandMiniMap(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetMinimapList* list = (SOH::SetMinimapList*)cmd;

    MapDisp_Init(play);

    MapDisp_InitMapData(play, list->GetPointer());
}

void Scene_Command1D(PlayState* play, SOH::ISceneCommand* cmd) {
}

void Scene_CommandMiniMapCompassInfo(PlayState* play, SOH::ISceneCommand* cmd) {
    SOH::SetMinimapChests* chests = (SOH::SetMinimapChests*)cmd;

    MapDisp_InitChestData(play, chests->chests.size(), chests->GetPointer());
}

void (*sSceneCmdHandlersOTR[SCENE_CMD_MAX])(PlayState*, SOH::ISceneCommand*) = {
    Scene_CommandSpawnList,            // SCENE_CMD_ID_SPAWN_LIST
    Scene_CommandActorList,            // SCENE_CMD_ID_ACTOR_LIST
    Scene_CommandActorCutsceneCamList, // SCENE_CMD_ID_ACTOR_CUTSCENE_CAM_LIST
    Scene_CommandCollisionHeader,      // SCENE_CMD_ID_COL_HEADER
    Scene_CommandRoomList,             // SCENE_CMD_ID_ROOM_LIST
    Scene_CommandWindSettings,         // SCENE_CMD_ID_WIND_SETTINGS
    Scene_CommandEntranceList,         // SCENE_CMD_ID_ENTRANCE_LIST
    Scene_CommandSpecialFiles,         // SCENE_CMD_ID_SPECIAL_FILES
    Scene_CommandRoomBehavior,         // SCENE_CMD_ID_ROOM_BEHAVIOR
    Scene_Command09,                   // SCENE_CMD_ID_UNK_09
    Scene_CommandMesh,                 // SCENE_CMD_ID_ROOM_SHAPE
    Scene_CommandObjectList,           // SCENE_CMD_ID_OBJECT_LIST
    Scene_CommandLightList,            // SCENE_CMD_ID_LIGHT_LIST
    Scene_CommandPathList,             // SCENE_CMD_ID_PATH_LIST
    Scene_CommandTransiActorList,      // SCENE_CMD_ID_TRANSI_ACTOR_LIST
    Scene_CommandEnvLightSettings,     // SCENE_CMD_ID_ENV_LIGHT_SETTINGS
    Scene_CommandTimeSettings,         // SCENE_CMD_ID_TIME_SETTINGS
    Scene_CommandSkyboxSettings,       // SCENE_CMD_ID_SKYBOX_SETTINGS
    Scene_CommandSkyboxDisables,       // SCENE_CMD_ID_SKYBOX_DISABLES
    Scene_CommandExitList,             // SCENE_CMD_ID_EXIT_LIST
    NULL,                              // SCENE_CMD_ID_END
    Scene_CommandSoundSettings,        // SCENE_CMD_ID_SOUND_SETTINGS
    Scene_CommandEchoSetting,          // SCENE_CMD_ID_ECHO_SETTINGS
    Scene_CommandCutsceneScriptList,   // SCENE_CMD_ID_CUTSCENE_SCRIPT_LIST
    Scene_CommandAltHeaderList,        // SCENE_CMD_ID_ALTERNATE_HEADER_LIST
    Scene_CommandSetRegionVisitedFlag, // SCENE_CMD_ID_SET_REGION_VISITED
    Scene_CommandAnimatedMaterials,    // SCENE_CMD_ID_ANIMATED_MATERIAL_LIST
    Scene_CommandCutsceneList,         // SCENE_CMD_ID_ACTOR_CUTSCENE_LIST
    Scene_CommandMiniMap,              // SCENE_CMD_ID_MINIMAP_INFO
    Scene_Command1D,                   // SCENE_CMD_ID_UNUSED_1D
    Scene_CommandMiniMapCompassInfo,   // SCENE_CMD_ID_MINIMAP_COMPASS_ICON_INFO
};

s32 OTRScene_ExecuteCommands(PlayState* play, SOH::Scene* scene) {
    SOH::SceneCommandID cmdId;
    shouldEndSceneCommands = false;

    for (int i = 0; i < scene->commands.size(); i++) {
        auto sceneCmd = scene->commands[i];
        cmdId = sceneCmd->cmdId;

        // 2S2H [Port] This opcode is not in the original game. Its a special command for OTRs, for supporting multiple
        // games
        if (cmdId == SOH::SceneCommandID::SetCutscenesMM) {
            cmdId = SOH::SceneCommandID::SetCutscenes;
        }

        // 2S2H [Port] shouldEndSceneCommands is set when an alternate header list is found
        if (cmdId == SOH::SceneCommandID::EndMarker || shouldEndSceneCommands) {
            shouldEndSceneCommands = false;
            break;
        }

        if (cmdId < SOH::SceneCommandID::SetCutscenesMM) {
            sSceneCmdHandlersOTR[(int)cmdId](play, sceneCmd.get());
        }
    }
    return 0;
}

extern "C" s32 OTRfunc_8009728C(PlayState* play, RoomContext* roomCtx, s32 roomNum) {

    u32 size;

    if (roomCtx->status == 0) {
        roomCtx->prevRoom = roomCtx->curRoom;
        roomCtx->curRoom.num = roomNum;
        roomCtx->curRoom.segment = NULL;
        roomCtx->status = 1;

        // assert(roomNum < play->roomList.count);

        if (roomNum >= play->roomList.count)
            return 0; // UH OH

        size = play->roomList.romFiles[roomNum].vromEnd - play->roomList.romFiles[roomNum].vromStart;
        // roomCtx->roomRequestAddr =
        //     (void*)((uintptr_t)roomCtx->roomMemPages[roomCtx->activeBufPage] - ((size + 8) * roomCtx->activeBufPage +
        //     7));

        // DmaMgr_SendRequest2(&roomCtx->dmaRequest, roomCtx->unk_34, play->roomList.romFiles[roomNum].vromStart, size,
        // 0,
        //&roomCtx->loadQueue, NULL, __FILE__, __LINE__);
        auto roomData = std::static_pointer_cast<SOH::Scene>(ResourceLoad(play->roomList.romFiles[roomNum].fileName));
        roomCtx->status = 1;
        roomCtx->roomRequestAddr = roomData.get();

        roomCtx->activeBufPage ^= 1;

        GameInteractor_ExecuteOnRoomInit(play->sceneId, roomCtx->curRoom.num);

        return 1;
    }

    return 0;
}

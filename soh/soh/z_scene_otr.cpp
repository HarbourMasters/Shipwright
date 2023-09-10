#include "OTRGlobals.h"
#include <libultraship/libultraship.h>
#include "soh/resource/type/Scene.h"
#include <Utils/StringHelper.h>
#include "global.h"
#include "vt.h"
#include "soh/resource/type/CollisionHeader.h"
#include <DisplayList.h>
#include "soh/resource/type/Cutscene.h"
#include "soh/resource/type/Path.h"
#include "soh/resource/type/Text.h"
#include <Blob.h>
#include <memory>
#include <cassert>
#include "soh/resource/type/scenecommand/SetCameraSettings.h"
#include "soh/resource/type/scenecommand/SetCutscenes.h"
#include "soh/resource/type/scenecommand/SetStartPositionList.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
#include "soh/resource/type/scenecommand/SetCollisionHeader.h"
#include "soh/resource/type/scenecommand/SetRoomList.h"
#include "soh/resource/type/scenecommand/SetEntranceList.h"
#include "soh/resource/type/scenecommand/SetSpecialObjects.h"
#include "soh/resource/type/scenecommand/SetRoomBehavior.h"
#include "soh/resource/type/scenecommand/SetMesh.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include "soh/resource/type/scenecommand/SetLightList.h"
#include "soh/resource/type/scenecommand/SetPathways.h"
#include "soh/resource/type/scenecommand/SetTransitionActorList.h"
#include "soh/resource/type/scenecommand/SetSkyboxSettings.h"
#include "soh/resource/type/scenecommand/SetSkyboxModifier.h"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"
#include "soh/resource/type/scenecommand/SetWindSettings.h"
#include "soh/resource/type/scenecommand/SetSoundSettings.h"
#include "soh/resource/type/scenecommand/SetEchoSettings.h"
#include "soh/resource/type/scenecommand/SetAlternateHeaders.h"

extern LUS::IResource* OTRPlay_LoadFile(PlayState* play, const char* fileName);
extern "C" s32 Object_Spawn(ObjectContext* objectCtx, s16 objectId);
extern "C" RomFile sNaviMsgFiles[];
s32 OTRScene_ExecuteCommands(PlayState* play, LUS::Scene* scene);

std::shared_ptr<LUS::File> ResourceMgr_LoadFile(const char* path) {
    std::string Path = path;
    if (IsGameMasterQuest()) {
        size_t pos = 0;
        if ((pos = Path.find("/nonmq/", 0)) != std::string::npos) {
            Path.replace(pos, 7, "/mq/");
        }
    }
    return LUS::Context::GetInstance()->GetResourceManager()->LoadFile(Path.c_str());
}

// Forward Declaration of function declared in OTRGlobals.cpp
std::shared_ptr<LUS::IResource> GetResourceByNameHandlingMQ(const char* path);

bool Scene_CommandSpawnList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetStartPositionList* cmdStartPos = std::static_pointer_cast<LUS::SetStartPositionList>(cmd);
    LUS::SetStartPositionList* cmdStartPos = (LUS::SetStartPositionList*)cmd;
    ActorEntry* entries = (ActorEntry*)cmdStartPos->GetRawPointer();

    play->linkActorEntry = &entries[play->setupEntranceList[play->curSpawn].spawn];
    play->linkAgeOnLoad = ((void)0, gSaveContext.linkAge);
    s16 linkObjectId = gLinkObjectIds[((void)0, gSaveContext.linkAge)];

    Object_Spawn(&play->objectCtx, linkObjectId);

    return false;
}

bool Scene_CommandActorList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetActorList* cmdActor = std::static_pointer_cast<LUS::SetActorList>(cmd);
    LUS::SetActorList* cmdActor = (LUS::SetActorList*)cmd;

    play->numSetupActors = cmdActor->numActors;
    play->setupActorList = (ActorEntry*)cmdActor->GetRawPointer();

    return false;
}

bool Scene_CommandUnused2(PlayState* play, LUS::ISceneCommand* cmd) {
    // OTRTODO: Do we need to implement this?
    // play->unk_11DFC = SEGMENTED_TO_VIRTUAL(cmd->unused02.segment);

    return false;
}

bool Scene_CommandCollisionHeader(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetCollisionHeader* cmdCol = std::static_pointer_cast<LUS::SetCollisionHeader>(cmd);
    LUS::SetCollisionHeader* cmdCol = (LUS::SetCollisionHeader*)cmd;
    BgCheck_Allocate(&play->colCtx, play, (CollisionHeader*)cmdCol->GetRawPointer());

    return false;
}

bool Scene_CommandRoomList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetRoomList* cmdRoomList = std::static_pointer_cast<LUS::SetRoomList>(cmd);
    LUS::SetRoomList* cmdRoomList = (LUS::SetRoomList*)cmd;

    play->numRooms = cmdRoomList->numRooms;
    play->roomList = (RomFile*)cmdRoomList->GetRawPointer();

    return false;
}

bool Scene_CommandEntranceList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetEntranceList* otrEntrance = std::static_pointer_cast<LUS::SetEntranceList>(cmd);
    LUS::SetEntranceList* otrEntrance = (LUS::SetEntranceList*)cmd;
    play->setupEntranceList = (EntranceEntry*)otrEntrance->GetRawPointer();

    return false;
}

bool Scene_CommandSpecialFiles(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetSpecialObjects* specialCmd = std::static_pointer_cast<LUS::SetSpecialObjects>(cmd);
    LUS::SetSpecialObjects* specialCmd = (LUS::SetSpecialObjects*)cmd;

    if (specialCmd->specialObjects.globalObject != 0) {
        play->objectCtx.subKeepIndex = Object_Spawn(&play->objectCtx, specialCmd->specialObjects.globalObject);
    }

    if (specialCmd->specialObjects.elfMessage != 0) {
        auto res = 
            (LUS::Blob*)OTRPlay_LoadFile(play, sNaviMsgFiles[specialCmd->specialObjects.elfMessage - 1].fileName);
        play->cUpElfMsgs = (ElfMessage*)res->Data.data();
    }

    return false;
}

bool Scene_CommandRoomBehavior(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetRoomBehavior* cmdRoom = std::static_pointer_cast<LUS::SetRoomBehavior>(cmd);
    LUS::SetRoomBehavior* cmdRoom = (LUS::SetRoomBehavior*)cmd;

    play->roomCtx.curRoom.behaviorType1 = cmdRoom->roomBehavior.gameplayFlags;
    play->roomCtx.curRoom.behaviorType2 = cmdRoom->roomBehavior.gameplayFlags2 & 0xFF;
    play->roomCtx.curRoom.lensMode = (cmdRoom->roomBehavior.gameplayFlags2 >> 8) & 1;
    play->msgCtx.disableWarpSongs = (cmdRoom->roomBehavior.gameplayFlags2 >> 0xA) & 1;

    return false;
}

bool Scene_CommandMeshHeader(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetMesh* otrMesh = static_pointer_cast<LUS::SetMesh>(cmd);
    LUS::SetMesh* otrMesh = (LUS::SetMesh*)cmd;
    play->roomCtx.curRoom.meshHeader = (MeshHeader*)otrMesh->GetRawPointer();

    return false;
}

extern "C" void* func_800982FC(ObjectContext* objectCtx, s32 bankIndex, s16 objectId);

bool Scene_CommandObjectList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetObjectList* cmdObj = static_pointer_cast<LUS::SetObjectList>(cmd);
    LUS::SetObjectList* cmdObj = (LUS::SetObjectList*)cmd;

    s32 i;
    s32 j;
    s32 k;
    ObjectStatus* status;
    ObjectStatus* status2;
    ObjectStatus* firstStatus;
    // s16* objectEntry = SEGMENTED_TO_VIRTUAL(cmd->objectList.segment);
    s16* objectEntry = (s16*)cmdObj->GetRawPointer();
    void* nextPtr;

    k = 0;
    // i = play->objectCtx.unk_09;
    i = 0;
    firstStatus = &play->objectCtx.status[0];
    status = &play->objectCtx.status[i];

    for (int i = 0; i < cmdObj->objects.size(); i++) {
        bool alreadyIncluded = false;

        for (int j = 0; j < play->objectCtx.num; j++) {
            if (play->objectCtx.status[j].id == cmdObj->objects[i]) {
                alreadyIncluded = true;
                break;
            }
        }

        if (!alreadyIncluded) {
            play->objectCtx.status[play->objectCtx.num++].id = cmdObj->objects[i];
            func_80031A28(play, &play->actorCtx);
        }
    }

    /*
    while (i < play->objectCtx.num) {
        if (status->id != *objectEntry) {
            status2 = &play->objectCtx.status[i];
            for (j = i; j < play->objectCtx.num; j++) {
                status2->id = OBJECT_INVALID;
                status2++;
            }
            play->objectCtx.num = i;
            func_80031A28(play, &play->actorCtx);

            continue;
        }

        i++;
        k++;
        objectEntry++;
        status++;
    }

    play->objectCtx.num = i;
    */

    return false;
}

bool Scene_CommandLightList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetLightList* cmdLight = static_pointer_cast<LUS::SetLightList>(cmd);
    LUS::SetLightList* cmdLight = (LUS::SetLightList*)cmd;

    for (size_t i = 0; i < cmdLight->lightList.size(); i++) {
        LightContext_InsertLight(play, &play->lightCtx, (LightInfo*)&cmdLight->lightList[i]);
    }

    return false;
}

bool Scene_CommandPathList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetPathways* cmdPath = static_pointer_cast<LUS::SetPathways>(cmd);
    LUS::SetPathways* cmdPath = (LUS::SetPathways*)cmd;
    play->setupPathList = (Path*)(cmdPath->GetPointer()[0]);

    return false;
}

bool Scene_CommandTransitionActorList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetTransitionActorList* cmdActor = static_pointer_cast<LUS::SetTransitionActorList>(cmd);
    LUS::SetTransitionActorList* cmdActor = (LUS::SetTransitionActorList*)cmd;

    play->transiActorCtx.numActors = cmdActor->numTransitionActors;
    play->transiActorCtx.list = (TransitionActorEntry*)cmdActor->GetRawPointer();

    return false;
}

// void TransitionActor_InitContext(GameState* state, TransitionActorContext* transiActorCtx) {
//    transiActorCtx->numActors = 0;
//}

bool Scene_CommandLightSettingsList(PlayState* play, LUS::ISceneCommand* cmd) {
    play->envCtx.lightSettingsList = (EnvLightSettings*)cmd->GetRawPointer();

    return false;
}

// Scene Command 0x11: Skybox Settings
bool Scene_CommandSkyboxSettings(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetSkyboxSettings* cmdSky = static_pointer_cast<LUS::SetSkyboxSettings>(cmd);
    LUS::SetSkyboxSettings* cmdSky = (LUS::SetSkyboxSettings*)cmd;

    play->skyboxId = cmdSky->settings.skyboxId;
    play->envCtx.unk_17 = play->envCtx.unk_18 = cmdSky->settings.weather;
    play->envCtx.indoors = cmdSky->settings.indoors;

    return false;
}

bool Scene_CommandSkyboxDisables(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetSkyboxModifier* cmdSky = static_pointer_cast<LUS::SetSkyboxModifier>(cmd);
    LUS::SetSkyboxModifier* cmdSky = (LUS::SetSkyboxModifier*)cmd;

    play->envCtx.sunMoonDisabled = cmdSky->modifier.sunMoonDisabled;
    play->envCtx.skyboxDisabled = cmdSky->modifier.skyboxDisabled;

    return false;
}

bool Scene_CommandTimeSettings(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetTimeSettings* cmdTime = static_pointer_cast<LUS::SetTimeSettings>(cmd);
    LUS::SetTimeSettings* cmdTime = (LUS::SetTimeSettings*)cmd;

    if ((cmdTime->settings.hour != 0xFF) && (cmdTime->settings.minute != 0xFF)) {
        gSaveContext.skyboxTime = gSaveContext.dayTime =
            ((cmdTime->settings.hour + (cmdTime->settings.minute / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000);
    }

    if (cmdTime->settings.timeIncrement != 0xFF) {
        play->envCtx.timeIncrement = cmdTime->settings.timeIncrement;
    } else {
        play->envCtx.timeIncrement = 0;
    }

    if (gSaveContext.sunsSongState == SUNSSONG_INACTIVE) {
        gTimeIncrement = play->envCtx.timeIncrement;
    }

    play->envCtx.sunPos.x = -(Math_SinS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
    play->envCtx.sunPos.y = (Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
    play->envCtx.sunPos.z = (Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 20.0f) * 25.0f;

    if (((play->envCtx.timeIncrement == 0) && (gSaveContext.cutsceneIndex < 0xFFF0)) ||
        (gSaveContext.entranceIndex == 0x0604)) {
        gSaveContext.skyboxTime = ((void)0, gSaveContext.dayTime);
        if ((gSaveContext.skyboxTime >= 0x2AAC) && (gSaveContext.skyboxTime < 0x4555)) {
            gSaveContext.skyboxTime = 0x3556;
        } else if ((gSaveContext.skyboxTime >= 0x4555) && (gSaveContext.skyboxTime < 0x5556)) {
            gSaveContext.skyboxTime = 0x5556;
        } else if ((gSaveContext.skyboxTime >= 0xAAAB) && (gSaveContext.skyboxTime < 0xB556)) {
            gSaveContext.skyboxTime = 0xB556;
        } else if ((gSaveContext.skyboxTime >= 0xC001) && (gSaveContext.skyboxTime < 0xCAAC)) {
            gSaveContext.skyboxTime = 0xCAAC;
        }
    }

    return false;
}

bool Scene_CommandWindSettings(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetWind* cmdWind = std::static_pointer_cast<LUS::SetWind>(cmd);
    LUS::SetWindSettings* cmdWind = (LUS::SetWindSettings*)cmd;

    play->envCtx.windDirection.x = cmdWind->settings.windWest;
    play->envCtx.windDirection.y = cmdWind->settings.windVertical;
    play->envCtx.windDirection.z = cmdWind->settings.windSouth;

    play->envCtx.windSpeed = cmdWind->settings.windSpeed;

    return false;
}

bool Scene_CommandExitList(PlayState* play, LUS::ISceneCommand* cmd) {
    play->setupExitList = (s16*)cmd->GetRawPointer();

    return false;
}

bool Scene_CommandUndefined9(PlayState* play, LUS::ISceneCommand* cmd) {
    return false;
}

bool Scene_CommandSoundSettings(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetSoundSettings* cmdSnd = static_pointer_cast<LUS::SetSoundSettings>(cmd);
    LUS::SetSoundSettings* cmdSnd = (LUS::SetSoundSettings*)cmd;

    play->sequenceCtx.seqId = cmdSnd->settings.seqId;
    play->sequenceCtx.natureAmbienceId = cmdSnd->settings.natureAmbienceId;

    if (gSaveContext.seqId == 0xFF) {
        Audio_QueueSeqCmd(cmdSnd->settings.reverb | 0xF0000000);
    }

    return false;
}

bool Scene_CommandEchoSettings(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetEchoSettings* cmdEcho = static_pointer_cast<LUS::SetEchoSettings>(cmd);
    LUS::SetEchoSettings* cmdEcho = (LUS::SetEchoSettings*)cmd;

    play->roomCtx.curRoom.echo = cmdEcho->settings.echo;

    return false;
}

bool Scene_CommandAlternateHeaderList(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetAlternateHeaders* cmdHeaders = static_pointer_cast<LUS::SetAlternateHeaders>(cmd);
    LUS::SetAlternateHeaders* cmdHeaders = (LUS::SetAlternateHeaders*)cmd;

    // s32 pad;
    // SceneCmd* altHeader;

    // osSyncPrintf("\n[ZU]sceneset age    =[%X]", ((void)0, gSaveContext.linkAge));
    // osSyncPrintf("\n[ZU]sceneset time   =[%X]", ((void)0, gSaveContext.cutsceneIndex));
    // osSyncPrintf("\n[ZU]sceneset counter=[%X]", ((void)0, gSaveContext.sceneSetupIndex));

    if (gSaveContext.sceneSetupIndex != 0) {
        LUS::Scene* desiredHeader =
            std::static_pointer_cast<LUS::Scene>(cmdHeaders->headers[gSaveContext.sceneSetupIndex - 1]).get();

        if (desiredHeader != nullptr) {
            OTRScene_ExecuteCommands(play, desiredHeader);
            return true;
        } else {
            // "Coughh! There is no specified dataaaaa!"
            osSyncPrintf("\nげぼはっ！ 指定されたデータがないでええっす！");

            if (gSaveContext.sceneSetupIndex == 3) {
                LUS::Scene* desiredHeader =
                    std::static_pointer_cast<LUS::Scene>(cmdHeaders->headers[gSaveContext.sceneSetupIndex - 2]).get();

                // "Using adult day data there!"
                osSyncPrintf("\nそこで、大人の昼データを使用するでええっす！！");

                if (desiredHeader != nullptr) {
                    OTRScene_ExecuteCommands(play, desiredHeader);
                    return true;
                }
            }
        }
    }
    return false;
}

bool Scene_CommandCutsceneData(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetCutscenes* cmdCS = std::static_pointer_cast<LUS::SetCutscenes>(cmd);
    LUS::SetCutscenes* cmdCS = (LUS::SetCutscenes*)cmd;

    play->csCtx.segment = cmdCS->cutscene->commands.data();

    // osSyncPrintf("\ngame_play->demo_play.data=[%x]", play->csCtx.segment);
    return false;
}

// Camera & World Map Area
bool Scene_CommandMiscSettings(PlayState* play, LUS::ISceneCommand* cmd) {
    // LUS::SetCameraSettings* cmdCam = std::static_pointer_cast<LUS::SetCameraSettings>(cmd);
    LUS::SetCameraSettings* cmdCam = (LUS::SetCameraSettings*)cmd;

    YREG(15) = cmdCam->settings.cameraMovement;
    gSaveContext.worldMapArea = cmdCam->settings.worldMapArea;

    if ((play->sceneNum == SCENE_BAZAAR) || (play->sceneNum == SCENE_SHOOTING_GALLERY)) {
        if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
            gSaveContext.worldMapArea = 1;
        }
    }

    if (((play->sceneNum >= SCENE_HYRULE_FIELD) && (play->sceneNum <= SCENE_OUTSIDE_GANONS_CASTLE)) ||
        ((play->sceneNum >= SCENE_MARKET_ENTRANCE_DAY) && (play->sceneNum <= SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS))) {
        if (gSaveContext.cutsceneIndex < 0xFFF0) {
            gSaveContext.worldMapAreaData |= gBitFlags[gSaveContext.worldMapArea];
            osSyncPrintf("０００  ａｒｅａ＿ａｒｒｉｖａｌ＝%x (%d)\n", gSaveContext.worldMapAreaData,
                gSaveContext.worldMapArea);
        }
    }
    return false;
}

bool (*sceneCommands[])(PlayState*, LUS::ISceneCommand*) = {
    Scene_CommandSpawnList,           // SCENE_CMD_ID_SPAWN_LIST
    Scene_CommandActorList,           // SCENE_CMD_ID_ACTOR_LIST
    Scene_CommandUnused2,             // SCENE_CMD_ID_UNUSED_2
    Scene_CommandCollisionHeader,     // SCENE_CMD_ID_COLLISION_HEADER
    Scene_CommandRoomList,            // SCENE_CMD_ID_ROOM_LIST
    Scene_CommandWindSettings,        // SCENE_CMD_ID_WIND_SETTINGS
    Scene_CommandEntranceList,        // SCENE_CMD_ID_ENTRANCE_LIST
    Scene_CommandSpecialFiles,        // SCENE_CMD_ID_SPECIAL_FILES
    Scene_CommandRoomBehavior,        // SCENE_CMD_ID_ROOM_BEHAVIOR
    Scene_CommandUndefined9,          // SCENE_CMD_ID_UNDEFINED_9
    Scene_CommandMeshHeader,          // SCENE_CMD_ID_MESH_HEADER
    Scene_CommandObjectList,          // SCENE_CMD_ID_OBJECT_LIST
    Scene_CommandLightList,           // SCENE_CMD_ID_LIGHT_LIST
    Scene_CommandPathList,            // SCENE_CMD_ID_PATH_LIST
    Scene_CommandTransitionActorList, // SCENE_CMD_ID_TRANSITION_ACTOR_LIST
    Scene_CommandLightSettingsList,   // SCENE_CMD_ID_LIGHT_SETTINGS_LIST
    Scene_CommandTimeSettings,        // SCENE_CMD_ID_TIME_SETTINGS
    Scene_CommandSkyboxSettings,      // SCENE_CMD_ID_SKYBOX_SETTINGS
    Scene_CommandSkyboxDisables,      // SCENE_CMD_ID_SKYBOX_DISABLES
    Scene_CommandExitList,            // SCENE_CMD_ID_EXIT_LIST
    NULL,                             // SCENE_CMD_ID_END
    Scene_CommandSoundSettings,       // SCENE_CMD_ID_SOUND_SETTINGS
    Scene_CommandEchoSettings,        // SCENE_CMD_ID_ECHO_SETTINGS
    Scene_CommandCutsceneData,        // SCENE_CMD_ID_CUTSCENE_DATA
    Scene_CommandAlternateHeaderList, // SCENE_CMD_ID_ALTERNATE_HEADER_LIST
    Scene_CommandMiscSettings,        // SCENE_CMD_ID_MISC_SETTINGS
};

s32 OTRScene_ExecuteCommands(PlayState* play, LUS::Scene* scene) {
    LUS::SceneCommandID cmdCode;

    for (int i = 0; i < scene->commands.size(); i++) {
        auto sceneCmd = scene->commands[i];

        if (sceneCmd == nullptr) // UH OH
            continue;

        cmdCode = sceneCmd->cmdId;
        // osSyncPrintf("*** Scene_Word = { code=%d, data1=%02x, data2=%04x } ***\n", cmdCode, sceneCmd->base.data1, sceneCmd->base.data2);

        if ((int)cmdCode == 0x14) {
            break;
        }

        if ((int)cmdCode <= 0x19) {
            if (sceneCommands[(int)cmdCode](play, sceneCmd.get()))
                break;
        } else {
            osSyncPrintf(VT_FGCOL(RED));
            osSyncPrintf("code の値が異常です\n"); // "code variable is abnormal"
            osSyncPrintf(VT_RST);
        }

        // sceneCmd++;
    }
    return 0;
}

extern "C" s32 OTRfunc_800973FC(PlayState* play, RoomContext* roomCtx) {
    if (roomCtx->status == 1) {
        // if (!osRecvMesg(&roomCtx->loadQueue, NULL, OS_MESG_NOBLOCK)) {
        if (1) {
            roomCtx->status = 0;
            roomCtx->curRoom.segment = roomCtx->unk_34;
            gSegments[3] = VIRTUAL_TO_PHYSICAL(roomCtx->unk_34);

            OTRScene_ExecuteCommands(play, (LUS::Scene*)roomCtx->roomToLoad);
            Player_SetBootData(play, GET_PLAYER(play));
            Actor_SpawnTransitionActors(play, &play->actorCtx);

            return 1;
        }

        return 0;
    }

    return 1;
}

extern "C" s32 OTRfunc_8009728C(PlayState* play, RoomContext* roomCtx, s32 roomNum) {
    u32 size;

    if (roomCtx->status == 0) {
        roomCtx->prevRoom = roomCtx->curRoom;
        roomCtx->curRoom.num = roomNum;
        roomCtx->curRoom.segment = NULL;
        roomCtx->status = 1;

        assert(roomNum < play->numRooms);

        if (roomNum >= play->numRooms)
            return 0; // UH OH

        size = play->roomList[roomNum].vromEnd - play->roomList[roomNum].vromStart;
        roomCtx->unk_34 =
            (void*)ALIGN16((uintptr_t)roomCtx->bufPtrs[roomCtx->unk_30] - ((size + 8) * roomCtx->unk_30 + 7));

        osCreateMesgQueue(&roomCtx->loadQueue, &roomCtx->loadMsg, 1);
        // DmaMgr_SendRequest2(&roomCtx->dmaRequest, roomCtx->unk_34, play->roomList[roomNum].vromStart, size, 0,
                            //&roomCtx->loadQueue, NULL, __FILE__, __LINE__);

        auto roomData =
            std::static_pointer_cast<LUS::Scene>(GetResourceByNameHandlingMQ(play->roomList[roomNum].fileName));
        roomCtx->status = 1;
        roomCtx->roomToLoad = roomData.get();

        roomCtx->unk_30 ^= 1;

        return 1;
    }

    return 0;
}

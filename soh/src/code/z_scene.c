#include "global.h"
#include "vt.h"

RomFile sNaviMsgFiles[];

s32 Object_Spawn(ObjectContext* objectCtx, s16 objectId) {
    size_t size;

    objectCtx->status[objectCtx->num].id = objectId;
    size = gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart;

    osSyncPrintf("OBJECT[%d] SIZE %fK SEG=%x\n", objectId, size / 1024.0f, objectCtx->status[objectCtx->num].segment);

    osSyncPrintf("num=%d adrs=%x end=%x\n", objectCtx->num, (uintptr_t)objectCtx->status[objectCtx->num].segment + size,
                 objectCtx->spaceEnd);

    ASSERT(((objectCtx->num < OBJECT_EXCHANGE_BANK_MAX) &&
            (((uintptr_t)objectCtx->status[objectCtx->num].segment + size) < (uintptr_t)objectCtx->spaceEnd)),
           "this->num < OBJECT_EXCHANGE_BANK_MAX && (this->status[this->num].Segment + size) < this->endSegment",
           "../z_scene.c", 142);

    DmaMgr_SendRequest1(objectCtx->status[objectCtx->num].segment, gObjectTable[objectId].vromStart, size,
                        "../z_scene.c", 145);

    if (objectCtx->num < OBJECT_EXCHANGE_BANK_MAX - 1) {
        objectCtx->status[objectCtx->num + 1].segment =
            (void*)ALIGN16((uintptr_t)objectCtx->status[objectCtx->num].segment + size);
    }

    objectCtx->num++;
    objectCtx->unk_09 = objectCtx->num;

    return objectCtx->num - 1;
}

void Object_InitBank(GlobalContext* globalCtx, ObjectContext* objectCtx) {
    GlobalContext* globalCtx2 = globalCtx; // Needs to be a new variable to match (possibly a sub struct?)
    size_t spaceSize;
    s32 i;

    if (globalCtx2->sceneNum == SCENE_SPOT00) {
        spaceSize = 1024000;
    } else if (globalCtx2->sceneNum == SCENE_GANON_DEMO) {
        if (gSaveContext.sceneSetupIndex != 4) {
            spaceSize = 1177600;
        } else {
            spaceSize = 1024000;
        }
    } else if (globalCtx2->sceneNum == SCENE_JYASINBOSS) {
        spaceSize = 1075200;
    } else if (globalCtx2->sceneNum == SCENE_KENJYANOMA) {
        spaceSize = 1075200;
    } else if (globalCtx2->sceneNum == SCENE_GANON_BOSS) {
        spaceSize = 1075200;
    } else {
        spaceSize = 1024000;
    }

    objectCtx->num = objectCtx->unk_09 = 0;
    objectCtx->mainKeepIndex = objectCtx->subKeepIndex = 0;

    for (i = 0; i < OBJECT_EXCHANGE_BANK_MAX; i++) {
        objectCtx->status[i].id = OBJECT_INVALID;
        objectCtx->status[i].segment = NULL;
    }

    osSyncPrintf(VT_FGCOL(GREEN));
    // "Object exchange bank data %8.3fKB"
    osSyncPrintf("オブジェクト入れ替えバンク情報 %8.3fKB\n", spaceSize / 1024.0f);
    osSyncPrintf(VT_RST);

    objectCtx->spaceStart = objectCtx->status[0].segment =
        GameState_Alloc(&globalCtx->state, spaceSize, "../z_scene.c", 219);
    objectCtx->spaceEnd = (void*)((uintptr_t)objectCtx->spaceStart + spaceSize);

    objectCtx->mainKeepIndex = Object_Spawn(objectCtx, OBJECT_GAMEPLAY_KEEP);
    gSegments[4] = VIRTUAL_TO_PHYSICAL(objectCtx->status[objectCtx->mainKeepIndex].segment);
}

void Object_UpdateBank(ObjectContext* objectCtx) {
    s32 i;
    ObjectStatus* status = &objectCtx->status[0];
    RomFile* objectFile;
    size_t size;

    /*
    for (i = 0; i < objectCtx->num; i++) {
        if (status->id < 0) {
            if (status->dmaRequest.vromAddr == 0) {
                osCreateMesgQueue(&status->loadQueue, &status->loadMsg, 1);
                objectFile = &gObjectTable[-status->id];
                size = objectFile->vromEnd - objectFile->vromStart;
                osSyncPrintf("OBJECT EXCHANGE BANK-%2d SIZE %8.3fK SEG=%08x\n", i, size / 1024.0f, status->segment);
                DmaMgr_SendRequest2(&status->dmaRequest, status->segment, objectFile->vromStart, size, 0,
                                    &status->loadQueue, NULL, "../z_scene.c", 266);
            } else if (!osRecvMesg(&status->loadQueue, NULL, OS_MESG_NOBLOCK)) {
                status->id = -status->id;
            }
        }
        status++;
    }
    */
}

s32 Object_GetIndex(ObjectContext* objectCtx, s16 objectId) {
    s32 i;

    //return 0;

    for (i = 0; i < objectCtx->num; i++) {
        if (ABS(objectCtx->status[i].id) == objectId) {
            return i;
        }
    }

    return -1;
}

s32 Object_IsLoaded(ObjectContext* objectCtx, s32 bankIndex) {
    if (objectCtx->status[bankIndex].id > 0) {
        return true;
    } else {
        return false;
    }
}

void func_800981B8(ObjectContext* objectCtx) {
    s32 i;
    s32 id;
    size_t size;

    for (i = 0; i < objectCtx->num; i++) {
        id = objectCtx->status[i].id;
        size = gObjectTable[id].vromEnd - gObjectTable[id].vromStart;
        osSyncPrintf("OBJECT[%d] SIZE %fK SEG=%x\n", objectCtx->status[i].id, size / 1024.0f,
                     objectCtx->status[i].segment);
        osSyncPrintf("num=%d adrs=%x end=%x\n", objectCtx->num, (uintptr_t)objectCtx->status[i].segment + size,
                     objectCtx->spaceEnd);
        DmaMgr_SendRequest1(objectCtx->status[i].segment, gObjectTable[id].vromStart, size, "../z_scene.c", 342);
    }
}

void* func_800982FC(ObjectContext* objectCtx, s32 bankIndex, s16 objectId) {
    ObjectStatus* status = &objectCtx->status[bankIndex];
    RomFile* objectFile = &gObjectTable[objectId];
    size_t size;
    void* nextPtr;

    status->id = -objectId;
    status->dmaRequest.vromAddr = 0;

    size = objectFile->vromEnd - objectFile->vromStart;
    osSyncPrintf("OBJECT EXCHANGE NO=%2d BANK=%3d SIZE=%8.3fK\n", bankIndex, objectId, size / 1024.0f);

    nextPtr = (void*)ALIGN16((uintptr_t)status->segment + size);
    if (1) {} // Necessary to match

    ASSERT(nextPtr < objectCtx->spaceEnd, "nextptr < this->endSegment", "../z_scene.c", 381);

    // "Object exchange free size=%08x"
    osSyncPrintf("オブジェクト入れ替え空きサイズ=%08x\n", (uintptr_t)objectCtx->spaceEnd - (uintptr_t)nextPtr);

    return nextPtr;
}

s32 Scene_ExecuteCommands(GlobalContext* globalCtx, SceneCmd* sceneCmd) {
    u32 cmdCode;

    while (true) {
        cmdCode = sceneCmd->base.code;
        osSyncPrintf("*** Scene_Word = { code=%d, data1=%02x, data2=%04x } ***\n", cmdCode, sceneCmd->base.data1,
                     sceneCmd->base.data2);

        if (cmdCode == 0x14) {
            break;
        }

        if (cmdCode <= 0x19) {
            gSceneCmdHandlers[cmdCode](globalCtx, sceneCmd);
        } else {
            osSyncPrintf(VT_FGCOL(RED));
            osSyncPrintf("code の値が異常です\n"); // "code variable is abnormal"
            osSyncPrintf(VT_RST);
        }
        sceneCmd++;
    }
    return 0;
}

void func_80098508(GlobalContext* globalCtx, SceneCmd* cmd) {
    ActorEntry* linkEntry = globalCtx->linkActorEntry = (ActorEntry*)SEGMENTED_TO_VIRTUAL(cmd->spawnList.segment) +
                                                        globalCtx->setupEntranceList[globalCtx->curSpawn].spawn;
    s16 linkObjectId;

    globalCtx->linkAgeOnLoad = ((void)0, gSaveContext.linkAge);

    linkObjectId = gLinkObjectIds[((void)0, gSaveContext.linkAge)];

    gActorOverlayTable[linkEntry->id].initInfo->objectId = linkObjectId;
    Object_Spawn(&globalCtx->objectCtx, linkObjectId);
}

// Scene Command 0x01: Actor List
void func_800985DC(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->numSetupActors = cmd->actorList.num;
    globalCtx->setupActorList = SEGMENTED_TO_VIRTUAL(cmd->actorList.segment);
}

// Scene Command 0x02: Unused 02
void func_80098630(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->unk_11DFC = SEGMENTED_TO_VIRTUAL(cmd->unused02.segment);
}

// Scene Command 0x03: Collision Header
void func_80098674(GlobalContext* globalCtx, SceneCmd* cmd) {
    CollisionHeader* colHeader = SEGMENTED_TO_VIRTUAL(cmd->colHeader.segment);

    colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList);
    colHeader->polyList = SEGMENTED_TO_VIRTUAL(colHeader->polyList);
    colHeader->surfaceTypeList = SEGMENTED_TO_VIRTUAL(colHeader->surfaceTypeList);
    colHeader->cameraDataList = SEGMENTED_TO_VIRTUAL(colHeader->cameraDataList);
    colHeader->waterBoxes = SEGMENTED_TO_VIRTUAL(colHeader->waterBoxes);

    BgCheck_Allocate(&globalCtx->colCtx, globalCtx, colHeader);
}

// Scene Command 0x04: Room List
void func_800987A4(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->numRooms = cmd->roomList.num;
    globalCtx->roomList = SEGMENTED_TO_VIRTUAL(cmd->roomList.segment);
}

// Scene Command 0x06: Entrance List
void func_800987F8(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->setupEntranceList = SEGMENTED_TO_VIRTUAL(cmd->entranceList.segment);
}

// Scene Command 0x07: Special Files
void func_8009883C(GlobalContext* globalCtx, SceneCmd* cmd) {
    if (cmd->specialFiles.keepObjectId != OBJECT_INVALID) {
        globalCtx->objectCtx.subKeepIndex = Object_Spawn(&globalCtx->objectCtx, cmd->specialFiles.keepObjectId);
        gSegments[5] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[globalCtx->objectCtx.subKeepIndex].segment);
    }

    if (cmd->specialFiles.cUpElfMsgNum != 0) {
        globalCtx->cUpElfMsgs = Gameplay_LoadFile(globalCtx, &sNaviMsgFiles[cmd->specialFiles.cUpElfMsgNum - 1]);
    }
}

// Scene Command 0x08: Room Behavior
void func_80098904(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->roomCtx.curRoom.unk_03 = cmd->roomBehavior.gpFlag1;
    globalCtx->roomCtx.curRoom.unk_02 = cmd->roomBehavior.gpFlag2 & 0xFF;
    globalCtx->roomCtx.curRoom.showInvisActors = (cmd->roomBehavior.gpFlag2 >> 8) & 1;
    globalCtx->msgCtx.disableWarpSongs = (cmd->roomBehavior.gpFlag2 >> 0xA) & 1;
}

// Scene Command 0x0A: Mesh Header
void func_80098958(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->roomCtx.curRoom.mesh = SEGMENTED_TO_VIRTUAL(cmd->mesh.segment);
}

// Scene Command 0x0B: Object List
void func_8009899C(GlobalContext* globalCtx, SceneCmd* cmd) {
    s32 i;
    s32 j;
    s32 k;
    ObjectStatus* status;
    ObjectStatus* status2;
    ObjectStatus* firstStatus;
    s16* objectEntry = SEGMENTED_TO_VIRTUAL(cmd->objectList.segment);
    void* nextPtr;

    k = 0;
    i = globalCtx->objectCtx.unk_09;
    firstStatus = &globalCtx->objectCtx.status[0];
    status = &globalCtx->objectCtx.status[i];

    while (i < globalCtx->objectCtx.num) {
        if (status->id != *objectEntry) {
            status2 = &globalCtx->objectCtx.status[i];
            for (j = i; j < globalCtx->objectCtx.num; j++) {
                status2->id = OBJECT_INVALID;
                status2++;
            }
            globalCtx->objectCtx.num = i;
            func_80031A28(globalCtx, &globalCtx->actorCtx);

            continue;
        }

        i++;
        k++;
        objectEntry++;
        status++;
    }

    ASSERT(cmd->objectList.num <= OBJECT_EXCHANGE_BANK_MAX, "scene_info->object_bank.num <= OBJECT_EXCHANGE_BANK_MAX",
           "../z_scene.c", 705);

    if (1) {}

    while (k < cmd->objectList.num) {
        nextPtr = func_800982FC(&globalCtx->objectCtx, i, *objectEntry);
        if (i < OBJECT_EXCHANGE_BANK_MAX - 1) {
            firstStatus[i + 1].segment = nextPtr;
        }
        i++;
        k++;
        objectEntry++;
    }

    globalCtx->objectCtx.num = i;
}

// Scene Command 0x0C: Light List
void func_80098B74(GlobalContext* globalCtx, SceneCmd* cmd) {
    s32 i;
    LightInfo* lightInfo = SEGMENTED_TO_VIRTUAL(cmd->lightList.segment);

    for (i = 0; i < cmd->lightList.num; i++) {
        LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, lightInfo);
        lightInfo++;
    }
}

// Scene Command 0x0D: Path List
void func_80098C24(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->setupPathList = SEGMENTED_TO_VIRTUAL(cmd->pathList.segment);
}

// Scene Command 0x0E: Transition Actor List
void func_80098C68(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->transiActorCtx.numActors = cmd->transiActorList.num;
    globalCtx->transiActorCtx.list = SEGMENTED_TO_VIRTUAL(cmd->transiActorList.segment);
}

void TransitionActor_InitContext(GameState* state, TransitionActorContext* transiActorCtx) {
    transiActorCtx->numActors = 0;
}

// Scene Command 0x0F: Light Setting List
void func_80098CC8(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->envCtx.numLightSettings = cmd->lightSettingList.num;
    globalCtx->envCtx.lightSettingsList = SEGMENTED_TO_VIRTUAL(cmd->lightSettingList.segment);
}

// Scene Command 0x11: Skybox Settings
void func_80098D1C(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->skyboxId = cmd->skyboxSettings.skyboxId;
    globalCtx->envCtx.unk_17 = globalCtx->envCtx.unk_18 = cmd->skyboxSettings.unk_05;
    globalCtx->envCtx.indoors = cmd->skyboxSettings.unk_06;
}

// Scene Command 0x12: Skybox Disables
void func_80098D5C(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->envCtx.skyboxDisabled = cmd->skyboxDisables.unk_04;
    globalCtx->envCtx.sunMoonDisabled = cmd->skyboxDisables.unk_05;
}

// Scene Command 0x10: Time Settings
void func_80098D80(GlobalContext* globalCtx, SceneCmd* cmd) {
    if ((cmd->timeSettings.hour != 0xFF) && (cmd->timeSettings.min != 0xFF)) {
        gSaveContext.skyboxTime = gSaveContext.dayTime =
            ((cmd->timeSettings.hour + (cmd->timeSettings.min / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000);
    }

    if (cmd->timeSettings.unk_06 != 0xFF) {
        globalCtx->envCtx.timeIncrement = cmd->timeSettings.unk_06;
    } else {
        globalCtx->envCtx.timeIncrement = 0;
    }

    if (gSaveContext.sunsSongState == SUNSSONG_INACTIVE) {
        gTimeIncrement = globalCtx->envCtx.timeIncrement;
    }

    globalCtx->envCtx.sunPos.x = -(Math_SinS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
    globalCtx->envCtx.sunPos.y = (Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
    globalCtx->envCtx.sunPos.z = (Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 20.0f) * 25.0f;

    if (((globalCtx->envCtx.timeIncrement == 0) && (gSaveContext.cutsceneIndex < 0xFFF0)) ||
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
}

// Scene Command 0x05: Wind Settings
void func_80099090(GlobalContext* globalCtx, SceneCmd* cmd) {
    s8 x = cmd->windSettings.x;
    s8 y = cmd->windSettings.y;
    s8 z = cmd->windSettings.z;

    globalCtx->envCtx.windDirection.x = x;
    globalCtx->envCtx.windDirection.y = y;
    globalCtx->envCtx.windDirection.z = z;

    globalCtx->envCtx.windSpeed = cmd->windSettings.unk_07;
}

// Scene Command 0x13: Exit List
void func_800990F0(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->setupExitList = SEGMENTED_TO_VIRTUAL(cmd->exitList.segment);
}

// Scene Command 0x09: Undefined
void func_80099134(GlobalContext* globalCtx, SceneCmd* cmd) {
}

// Scene Command 0x15: Sound Settings
void func_80099140(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->sequenceCtx.seqId = cmd->soundSettings.seqId;
    globalCtx->sequenceCtx.natureAmbienceId = cmd->soundSettings.natureAmbienceId;

    if (gSaveContext.seqId == (u8)NA_BGM_DISABLED) {
        Audio_QueueSeqCmd(cmd->soundSettings.specId | 0xF0000000);
    }
}

// Scene Command 0x16: Echo Setting
void func_8009918C(GlobalContext* globalCtx, SceneCmd* cmd) {
    globalCtx->roomCtx.curRoom.echo = cmd->echoSettings.echo;
}

// Scene Command 0x18: Alternate Headers
void func_800991A0(GlobalContext* globalCtx, SceneCmd* cmd) {
    s32 pad;
    SceneCmd* altHeader;

    osSyncPrintf("\n[ZU]sceneset age    =[%X]", ((void)0, gSaveContext.linkAge));
    osSyncPrintf("\n[ZU]sceneset time   =[%X]", ((void)0, gSaveContext.cutsceneIndex));
    osSyncPrintf("\n[ZU]sceneset counter=[%X]", ((void)0, gSaveContext.sceneSetupIndex));

    if (gSaveContext.sceneSetupIndex != 0) {
        altHeader = ((SceneCmd**)SEGMENTED_TO_VIRTUAL(cmd->altHeaders.segment))[gSaveContext.sceneSetupIndex - 1];

        if (1) {}

        if (altHeader != NULL) {
            Scene_ExecuteCommands(globalCtx, SEGMENTED_TO_VIRTUAL(altHeader));
            (cmd + 1)->base.code = 0x14;
        } else {
            // "Coughh! There is no specified dataaaaa!"
            osSyncPrintf("\nげぼはっ！ 指定されたデータがないでええっす！");

            if (gSaveContext.sceneSetupIndex == 3) {
                altHeader =
                    ((SceneCmd**)SEGMENTED_TO_VIRTUAL(cmd->altHeaders.segment))[gSaveContext.sceneSetupIndex - 2];

                // "Using adult day data there!"
                osSyncPrintf("\nそこで、大人の昼データを使用するでええっす！！");

                if (altHeader != NULL) {
                    Scene_ExecuteCommands(globalCtx, SEGMENTED_TO_VIRTUAL(altHeader));
                    (cmd + 1)->base.code = 0x14;
                }
            }
        }
    }
}

// Scene Command 0x17: Cutscene Data
void func_8009934C(GlobalContext* globalCtx, SceneCmd* cmd) {
    osSyncPrintf("\ngame_play->demo_play.data=[%x]", globalCtx->csCtx.segment);
    globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(cmd->cutsceneData.segment);
}

// Scene Command 0x19: Misc. Settings (Camera & World Map Area)
void func_800993C0(GlobalContext* globalCtx, SceneCmd* cmd) {
    YREG(15) = cmd->miscSettings.cameraMovement;
    gSaveContext.worldMapArea = cmd->miscSettings.area;

    if ((globalCtx->sceneNum == SCENE_SHOP1) || (globalCtx->sceneNum == SCENE_SYATEKIJYOU)) {
        if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
            gSaveContext.worldMapArea = 1;
        }
    }

    if (((globalCtx->sceneNum >= SCENE_SPOT00) && (globalCtx->sceneNum <= SCENE_GANON_TOU)) ||
        ((globalCtx->sceneNum >= SCENE_ENTRA) && (globalCtx->sceneNum <= SCENE_SHRINE_R))) {
        if (gSaveContext.cutsceneIndex < 0xFFF0) {
            gSaveContext.worldMapAreaData |= gBitFlags[gSaveContext.worldMapArea];
            osSyncPrintf("０００  ａｒｅａ＿ａｒｒｉｖａｌ＝%x (%d)\n", gSaveContext.worldMapAreaData,
                         gSaveContext.worldMapArea);
        }
    }
}

void (*gSceneCmdHandlers[])(GlobalContext*, SceneCmd*) = {
    func_80098508, func_800985DC, func_80098630, func_80098674, func_800987A4, func_80099090, func_800987F8,
    func_8009883C, func_80098904, func_80099134, func_80098958, func_8009899C, func_80098B74, func_80098C24,
    func_80098C68, func_80098CC8, func_80098D80, func_80098D1C, func_80098D5C, func_800990F0, NULL,
    func_80099140, func_8009918C, func_8009934C, func_800991A0, func_800993C0,
};

RomFile sNaviMsgFiles[] = {
    ROM_FILE(text/elf_message_field/elf_message_field),
    ROM_FILE(text/elf_message_ydan/elf_message_ydan),
    ROM_FILE_UNSET,
};

s16 gLinkObjectIds[] = { OBJECT_LINK_BOY, OBJECT_LINK_CHILD };

u32 gObjectTableSize = ARRAY_COUNT(gObjectTable);

// Object linker symbol declarations (used in the table below)
#define DEFINE_OBJECT(name, _1) DECLARE_ROM_SEGMENT(name)
#define DEFINE_OBJECT_NULL(_0, _1) 
#define DEFINE_OBJECT_UNSET(_0)

//#include "tables/object_table.h"

#undef DEFINE_OBJECT
#undef DEFINE_OBJECT_NULL
#undef DEFINE_OBJECT_UNSET

// Object Table definition
#define DEFINE_OBJECT(name, _1) ROM_FILE(name),
#define DEFINE_OBJECT_NULL(name, _1) ROM_FILE(name),
#define DEFINE_OBJECT_UNSET(_0) { 0, 0, "" },

RomFile gObjectTable[] = {
#include "tables/object_table.h"
};

#undef DEFINE_OBJECT
#undef DEFINE_OBJECT_NULL
#undef DEFINE_OBJECT_UNSET

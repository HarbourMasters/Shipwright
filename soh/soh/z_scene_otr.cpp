#include "OTRGlobals.h"
#include "ResourceMgr.h"
#include "Scene.h"
#include "Utils/StringHelper.h"
#include "global.h"
#include "vt.h"
#include <CollisionHeader.h>
#include <DisplayList.h>
#include <Cutscene.h>
#include <Path.h>
#include <Text.h>
#include <Blob.h>

extern Ship::Resource* OTRGameplay_LoadFile(GlobalContext* globalCtx, const char* fileName);
extern "C" s32 Object_Spawn(ObjectContext* objectCtx, s16 objectId);
extern "C" RomFile sNaviMsgFiles[];
s32 OTRScene_ExecuteCommands(GlobalContext* globalCtx, Ship::Scene* scene);

bool func_80098508(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetStartPositionList* cmdStartPos = (Ship::SetStartPositionList*)cmd;

    ActorEntry* linkSpawnEntry = nullptr;

    if (cmdStartPos->cachedGameData != nullptr)
    {
        ActorEntry* entries = (ActorEntry*)cmdStartPos->cachedGameData;
        linkSpawnEntry = &entries[globalCtx->setupEntranceList[globalCtx->curSpawn].spawn];
    }
    else
    {
        ActorEntry* entries = (ActorEntry*)malloc(sizeof(ActorEntry) * cmdStartPos->entries.size());

        for (int i = 0; i < cmdStartPos->entries.size(); i++)
        {
            entries[i].id = cmdStartPos->entries[i].actorNum;
            entries[i].params = cmdStartPos->entries[i].initVar;
            entries[i].pos.x = cmdStartPos->entries[i].posX;
            entries[i].pos.y = cmdStartPos->entries[i].posY;
            entries[i].pos.z = cmdStartPos->entries[i].posZ;
            entries[i].rot.x = cmdStartPos->entries[i].rotX;
            entries[i].rot.y = cmdStartPos->entries[i].rotY;
            entries[i].rot.z = cmdStartPos->entries[i].rotZ;
        }

        linkSpawnEntry = &entries[globalCtx->setupEntranceList[globalCtx->curSpawn].spawn];
        cmdStartPos->cachedGameData = entries;
    }

    ActorEntry* linkEntry = globalCtx->linkActorEntry = linkSpawnEntry;

    s16 linkObjectId;

    globalCtx->linkAgeOnLoad = ((void)0, gSaveContext.linkAge);

    linkObjectId = gLinkObjectIds[((void)0, gSaveContext.linkAge)];

    //gActorOverlayTable[linkEntry->id].initInfo->objectId = linkObjectId;
    Object_Spawn(&globalCtx->objectCtx, linkObjectId);

    return false;
}

// Scene Command 0x01: Actor List
bool func_800985DC(GlobalContext* globalCtx, Ship::SceneCommand* cmd) {
    Ship::SetActorList* cmdActor = (Ship::SetActorList*)cmd;

    globalCtx->numSetupActors = cmdActor->entries.size();

    if (cmdActor->cachedGameData != nullptr)
        globalCtx->setupActorList = (ActorEntry*)cmdActor->cachedGameData;
    else
    {
        ActorEntry* entries = (ActorEntry*)malloc(cmdActor->entries.size() * sizeof(ActorEntry));

        for (int i = 0; i < cmdActor->entries.size(); i++)
        {
            entries[i].id = cmdActor->entries[i].actorNum;
            entries[i].pos.x = cmdActor->entries[i].posX;
            entries[i].pos.y = cmdActor->entries[i].posY;
            entries[i].pos.z = cmdActor->entries[i].posZ;
            entries[i].rot.x = cmdActor->entries[i].rotX;
            entries[i].rot.y = cmdActor->entries[i].rotY;
            entries[i].rot.z = cmdActor->entries[i].rotZ;
            entries[i].params = cmdActor->entries[i].initVar;
        }

        cmdActor->cachedGameData = entries;
        globalCtx->setupActorList = entries;
    }

    return false;
}

// Scene Command 0x02: Unused 02
bool func_80098630(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    // Do we need to implement this?
    //globalCtx->unk_11DFC = SEGMENTED_TO_VIRTUAL(cmd->unused02.segment);

    return false;
}

// Scene Command 0x03: Collision Header
bool func_80098674(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetCollisionHeader* cmdCol = (Ship::SetCollisionHeader*)cmd;

    auto colRes = std::static_pointer_cast<Ship::CollisionHeader>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(cmdCol->filePath));

    CollisionHeader* colHeader = nullptr;

    if (colRes->cachedGameAsset != nullptr)
        colHeader = (CollisionHeader*)colRes->cachedGameAsset;
    else
    {
        colHeader = (CollisionHeader*)malloc(sizeof(CollisionHeader));

        colHeader->minBounds.x = colRes->absMinX;
        colHeader->minBounds.y = colRes->absMinY;
        colHeader->minBounds.z = colRes->absMinZ;

        colHeader->maxBounds.x = colRes->absMaxX;
        colHeader->maxBounds.y = colRes->absMaxY;
        colHeader->maxBounds.z = colRes->absMaxZ;

        colHeader->vtxList = (Vec3s*)malloc(sizeof(Vec3s) * colRes->vertices.size());
        colHeader->numVertices = colRes->vertices.size();

        for (int i = 0; i < colRes->vertices.size(); i++)
        {
            colHeader->vtxList[i].x = colRes->vertices[i].x;
            colHeader->vtxList[i].y = colRes->vertices[i].y;
            colHeader->vtxList[i].z = colRes->vertices[i].z;
        }

        colHeader->polyList = (CollisionPoly*)malloc(sizeof(CollisionPoly) * colRes->polygons.size());
        colHeader->numPolygons = colRes->polygons.size();

        for (int i = 0; i < colRes->polygons.size(); i++)
        {
            colHeader->polyList[i].type = colRes->polygons[i].type;
            colHeader->polyList[i].flags_vIA = colRes->polygons[i].vtxA;
            colHeader->polyList[i].flags_vIB = colRes->polygons[i].vtxB;
            colHeader->polyList[i].vIC = colRes->polygons[i].vtxC;
            colHeader->polyList[i].normal.x = colRes->polygons[i].a;
            colHeader->polyList[i].normal.y = colRes->polygons[i].b;
            colHeader->polyList[i].normal.z = colRes->polygons[i].c;
            colHeader->polyList[i].dist = colRes->polygons[i].d;
        }

        colHeader->surfaceTypeList = (SurfaceType*)malloc(colRes->polygonTypes.size() * sizeof(SurfaceType));

        for (int i = 0; i < colRes->polygonTypes.size(); i++)
        {
            colHeader->surfaceTypeList[i].data[0] = colRes->polygonTypes[i] >> 32;
            colHeader->surfaceTypeList[i].data[1] = colRes->polygonTypes[i] & 0xFFFFFFFF;
        }

        colHeader->cameraDataList = (CamData*)malloc(sizeof(CamData) * colRes->camData->entries.size());

        for (int i = 0; i < colRes->camData->entries.size(); i++)
        {
            colHeader->cameraDataList[i].cameraSType = colRes->camData->entries[i]->cameraSType;
            colHeader->cameraDataList[i].numCameras = colRes->camData->entries[i]->numData;

            int idx = colRes->camData->entries[i]->cameraPosDataIdx;

            colHeader->cameraDataList[i].camPosData = (Vec3s*)malloc(sizeof(Vec3s) * colRes->camData->entries[i]->numData);

            for (int j = 0; j < colRes->camData->entries[i]->numData; j++)
            {
                if (colRes->camData->cameraPositionData.size() > 0)
                {
                    colHeader->cameraDataList[i].camPosData[j].x = colRes->camData->cameraPositionData[idx + j]->x;
                    colHeader->cameraDataList[i].camPosData[j].y = colRes->camData->cameraPositionData[idx + j]->y;
                    colHeader->cameraDataList[i].camPosData[j].z = colRes->camData->cameraPositionData[idx + j]->z;
                }
                else
                {
                    colHeader->cameraDataList[i].camPosData->x = 0;
                    colHeader->cameraDataList[i].camPosData->y = 0;
                    colHeader->cameraDataList[i].camPosData->z = 0;
                }
            }
        }

        colHeader->numWaterBoxes = colRes->waterBoxes.size();
        colHeader->waterBoxes = (WaterBox*)malloc(sizeof(WaterBox) * colHeader->numWaterBoxes);

        for (int i = 0; i < colHeader->numWaterBoxes; i++)
        {
            colHeader->waterBoxes[i].xLength = colRes->waterBoxes[i].xLength;
            colHeader->waterBoxes[i].ySurface = colRes->waterBoxes[i].ySurface;
            colHeader->waterBoxes[i].xMin = colRes->waterBoxes[i].xMin;
            colHeader->waterBoxes[i].zMin = colRes->waterBoxes[i].zMin;
            colHeader->waterBoxes[i].xLength = colRes->waterBoxes[i].xLength;
            colHeader->waterBoxes[i].zLength = colRes->waterBoxes[i].zLength;
            colHeader->waterBoxes[i].properties = colRes->waterBoxes[i].properties;
        }

        colRes->cachedGameAsset = colHeader;
    }

    BgCheck_Allocate(&globalCtx->colCtx, globalCtx, colHeader);

    return false;
}

// Scene Command 0x04: Room List
bool func_800987A4(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetRoomList* cmdRoomList = (Ship::SetRoomList*)cmd;

    globalCtx->numRooms = cmdRoomList->rooms.size();
    globalCtx->roomList = (RomFile*)malloc(globalCtx->numRooms * sizeof(RomFile));

    for (int i = 0; i < cmdRoomList->rooms.size(); i++)
    {
        globalCtx->roomList[i].fileName = (char*)cmdRoomList->rooms[i].name.c_str();
        globalCtx->roomList[i].vromStart = cmdRoomList->rooms[i].vromStart;
        globalCtx->roomList[i].vromEnd = cmdRoomList->rooms[i].vromEnd;
    }

    return false;
}

// Scene Command 0x06: Entrance List
bool func_800987F8(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetEntranceList* otrEntrance = (Ship::SetEntranceList*)cmd;

    if (otrEntrance->cachedGameData != nullptr)
        globalCtx->setupEntranceList = (EntranceEntry*)otrEntrance->cachedGameData;
    else
    {
        globalCtx->setupEntranceList = (EntranceEntry*)malloc(otrEntrance->entrances.size() * sizeof(EntranceEntry));

        for (int i = 0; i < otrEntrance->entrances.size(); i++)
        {
            globalCtx->setupEntranceList[i].room = otrEntrance->entrances[i].roomToLoad;
            globalCtx->setupEntranceList[i].spawn = otrEntrance->entrances[i].startPositionIndex;
        }

        otrEntrance->cachedGameData = globalCtx->setupEntranceList;
    }

    return false;
}

// Scene Command 0x07: Special Files
bool func_8009883C(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetSpecialObjects* otrSpecial = (Ship::SetSpecialObjects*)cmd;

    if (otrSpecial->globalObject != 0)
        globalCtx->objectCtx.subKeepIndex = Object_Spawn(&globalCtx->objectCtx, otrSpecial->globalObject);

    if (otrSpecial->elfMessage != 0)
    {
        auto res = (Ship::Blob*)OTRGameplay_LoadFile(globalCtx, sNaviMsgFiles[otrSpecial->elfMessage - 1].fileName);
        globalCtx->cUpElfMsgs = (ElfMessage*)res->data.data();
    }

    return false;
}

// Scene Command 0x08: Room Behavior
bool func_80098904(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetRoomBehavior* cmdRoom = (Ship::SetRoomBehavior*)cmd;

    globalCtx->roomCtx.curRoom.unk_03 = cmdRoom->gameplayFlags;
    globalCtx->roomCtx.curRoom.unk_02 = cmdRoom->gameplayFlags2 & 0xFF;
    globalCtx->roomCtx.curRoom.showInvisActors = (cmdRoom->gameplayFlags2 >> 8) & 1;
    globalCtx->msgCtx.disableWarpSongs = (cmdRoom->gameplayFlags2 >> 0xA) & 1;

    return false;
}

// Scene Command 0x0A: Mesh Header
bool func_80098958(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetMesh* otrMesh = (Ship::SetMesh*)cmd;

    if (otrMesh->cachedGameData != nullptr)
        globalCtx->roomCtx.curRoom.mesh = (Mesh*)otrMesh->cachedGameData;
    else
    {
        globalCtx->roomCtx.curRoom.mesh = (Mesh*)malloc(sizeof(Mesh));
        globalCtx->roomCtx.curRoom.mesh->polygon.type = otrMesh->meshHeaderType;
        globalCtx->roomCtx.curRoom.mesh->polygon.num = otrMesh->meshes.size();

        if (otrMesh->meshHeaderType == 2)
            globalCtx->roomCtx.curRoom.mesh->polygon.start = malloc(sizeof(PolygonDlist2) * globalCtx->roomCtx.curRoom.mesh->polygon.num);
        else
            globalCtx->roomCtx.curRoom.mesh->polygon.start = malloc(sizeof(PolygonDlist) * globalCtx->roomCtx.curRoom.mesh->polygon.num);

        for (int i = 0; i < globalCtx->roomCtx.curRoom.mesh->polygon.num; i++)
        {
            if (otrMesh->meshHeaderType == 2)
            {
                PolygonDlist2* arr = (PolygonDlist2*)globalCtx->roomCtx.curRoom.mesh->polygon.start;
                PolygonDlist2* dlist = &arr[i];

                if (otrMesh->meshes[i].opa != "")
                {
                    auto opaFile = std::static_pointer_cast<Ship::DisplayList>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(otrMesh->meshes[i].opa));

                    dlist->opaDL = opaFile.get();
                    dlist->opa = (Gfx*)&dlist->opaDL->instructions[0];
                }
                else
                {
                    dlist->opa = 0;
                }

                if (otrMesh->meshes[i].xlu != "")
                {
                    auto xluFile = std::static_pointer_cast<Ship::DisplayList>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(otrMesh->meshes[i].xlu));

                    dlist->xluDL = xluFile.get();
                    dlist->xlu = (Gfx*)&dlist->xluDL->instructions[0];
                }
                else
                    dlist->xlu = 0;

                dlist->pos.x = otrMesh->meshes[i].x;
                dlist->pos.y = otrMesh->meshes[i].y;
                dlist->pos.z = otrMesh->meshes[i].z;
                dlist->unk_06 = otrMesh->meshes[i].unk_06;

                //globalCtx->roomCtx.curRoom.mesh->polygon.start = dlist;
            }
            else if (otrMesh->meshHeaderType == 1)
            {
                PolygonDlist* pType = (PolygonDlist*)malloc(sizeof(PolygonDlist));

                if (otrMesh->meshes[0].imgOpa != "")
                    pType->opa = (Gfx*)&std::static_pointer_cast<Ship::DisplayList>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(otrMesh->meshes[0].imgOpa))->instructions[0];
                else
                    pType->opa = 0;

                if (otrMesh->meshes[0].imgXlu != "")
                    pType->xlu = (Gfx*)&std::static_pointer_cast<Ship::DisplayList>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(otrMesh->meshes[0].imgXlu))->instructions[0];
                else
                    pType->xlu = 0;

                globalCtx->roomCtx.curRoom.mesh->polygon1.dlist = (Gfx*)pType;

                globalCtx->roomCtx.curRoom.mesh->polygon1.format = otrMesh->meshes[0].imgFmt;

                if (otrMesh->meshes[0].imgFmt == 1)
                {
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.fmt = otrMesh->meshes[0].images[0].fmt;
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.source =
                        (void*)(OTRGlobals::Instance->context->GetResourceManager()->LoadFile(
                            otrMesh->meshes[0].images[0].sourceBackground))
                        .get()
                        ->buffer.get();
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.siz = otrMesh->meshes[0].images[0].siz;
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.width = otrMesh->meshes[0].images[0].width;
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.height = otrMesh->meshes[0].images[0].height;
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.fmt = otrMesh->meshes[0].images[0].fmt;
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.mode0 = otrMesh->meshes[0].images[0].mode0;
                    globalCtx->roomCtx.curRoom.mesh->polygon1.single.tlutCount = otrMesh->meshes[0].images[0].tlutCount;
                }
                else
                {
                    globalCtx->roomCtx.curRoom.mesh->polygon1.multi.count = otrMesh->meshes[0].images.size();
                    globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list =
                        (BgImage*)calloc(sizeof(BgImage), globalCtx->roomCtx.curRoom.mesh->polygon1.multi.count);

                    for (size_t i = 0; i < otrMesh->meshes[0].images.size(); i++)
                    {
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].fmt = otrMesh->meshes[0].images[i].fmt;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].source =
                            (void*)(OTRGlobals::Instance->context->GetResourceManager()->LoadFile(
                                otrMesh->meshes[0].images[i].sourceBackground))
                            .get()
                            ->buffer.get();
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].siz = otrMesh->meshes[0].images[i].siz;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].width = otrMesh->meshes[0].images[i].width;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].height =
                            otrMesh->meshes[0].images[i].height;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].fmt = otrMesh->meshes[0].images[i].fmt;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].mode0 = otrMesh->meshes[0].images[i].mode0;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].tlutCount =
                            otrMesh->meshes[0].images[i].tlutCount;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].unk_00 =
                            otrMesh->meshes[0].images[i].unk_00;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].unk_0C =
                            otrMesh->meshes[0].images[i].unk_0C;
                        globalCtx->roomCtx.curRoom.mesh->polygon1.multi.list[i].id = otrMesh->meshes[0].images[i].id;
                    }
                }
            }
            else
            {
                PolygonDlist* arr = (PolygonDlist*)globalCtx->roomCtx.curRoom.mesh->polygon.start;
                PolygonDlist* dlist = &arr[i];

                if (otrMesh->meshes[i].opa != "")
                {
                    auto opaFile = std::static_pointer_cast<Ship::DisplayList>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(otrMesh->meshes[i].opa));

                    dlist->opaDL = opaFile.get();
                    dlist->opa = (Gfx*)&dlist->opaDL->instructions[0];
                }
                else
                    dlist->opa = 0;

                if (otrMesh->meshes[i].xlu != "")
                {
                    auto xluFile = std::static_pointer_cast<Ship::DisplayList>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(otrMesh->meshes[i].xlu));

                    dlist->xluDL = xluFile.get();
                    dlist->xlu = (Gfx*)&dlist->xluDL->instructions[0];
                }
                else
                    dlist->xlu = 0;

                //globalCtx->roomCtx.curRoom.mesh->polygon.start = dlist;
            }
        }

        otrMesh->cachedGameData = globalCtx->roomCtx.curRoom.mesh;
    }

    return false;
}

extern "C" void* func_800982FC(ObjectContext * objectCtx, s32 bankIndex, s16 objectId);

// Scene Command 0x0B: Object List
bool func_8009899C(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetObjectList* cmdObj = (Ship::SetObjectList*)cmd;

    s32 i;
    s32 j;
    s32 k;
    ObjectStatus* status;
    ObjectStatus* status2;
    ObjectStatus* firstStatus;
    //s16* objectEntry = SEGMENTED_TO_VIRTUAL(cmd->objectList.segment);
    s16* objectEntry = (s16*)cmdObj->objects.data();
    void* nextPtr;

    k = 0;
    //i = globalCtx->objectCtx.unk_09;
    firstStatus = &globalCtx->objectCtx.status[0];
    status = &globalCtx->objectCtx.status[i];

    for (int i = 0; i < cmdObj->objects.size(); i++) {
        bool alreadyIncluded = false;

        for (int j = 0; j < globalCtx->objectCtx.num; j++) {
            if (globalCtx->objectCtx.status[j].id == cmdObj->objects[i]) {
                alreadyIncluded = true;
                break;
            }
        }

        if (!alreadyIncluded) {
            globalCtx->objectCtx.status[globalCtx->objectCtx.num++].id = cmdObj->objects[i];
            func_80031A28(globalCtx, &globalCtx->actorCtx);
        }
    }

    /*
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

    globalCtx->objectCtx.num = i;
    */

    return false;
}

// Scene Command 0x0C: Light List
bool func_80098B74(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetLightList* cmdLight = (Ship::SetLightList*)cmd;

    LightInfo* lightInfo = (LightInfo*)malloc(cmdLight->lights.size() * sizeof(LightInfo));

    for (int i = 0; i < cmdLight->lights.size(); i++)
    {
        lightInfo[i].type = cmdLight->lights[i].type;
        lightInfo[i].params.point.x = cmdLight->lights[i].x;
        lightInfo[i].params.point.y = cmdLight->lights[i].y;
        lightInfo[i].params.point.z = cmdLight->lights[i].z;
        lightInfo[i].params.point.radius = cmdLight->lights[i].radius;
        lightInfo[i].params.point.drawGlow = cmdLight->lights[i].drawGlow;
        lightInfo[i].params.point.color[0] = cmdLight->lights[i].r;
        lightInfo[i].params.point.color[1] = cmdLight->lights[i].g;
        lightInfo[i].params.point.color[2] = cmdLight->lights[i].b;

        LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &lightInfo[i]);
    }

    return false;
}

// Scene Command 0x0D: Path
bool func_80098C24(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetPathways* cmdPath = (Ship::SetPathways*)cmd;

    Ship::Path* path = (Ship::Path*)OTRGlobals::Instance->context->GetResourceManager()->LoadResource(cmdPath->paths[0]).get();
    globalCtx->setupPathList = (Path*)malloc(path->paths.size() * sizeof(Path));

    //for (int i = 0; i < cmdPath->paths.size(); i++)
    {
        for (int j = 0; j < path->paths.size(); j++)
        {
            globalCtx->setupPathList[j].count = path->paths[j].size();
            globalCtx->setupPathList[j].points = (Vec3s*)malloc(sizeof(Vec3s) * path->paths[j].size());

            for (int k = 0; k < path->paths[j].size(); k++)
            {
                globalCtx->setupPathList[j].points[k].x = path->paths[j][k].x;
                globalCtx->setupPathList[j].points[k].y = path->paths[j][k].y;
                globalCtx->setupPathList[j].points[k].z = path->paths[j][k].z;
            }
        }
    }

    return false;
}

// Scene Command 0x0E: Transition Actor List
bool func_80098C68(GlobalContext* globalCtx, Ship::SceneCommand* cmd) {
    Ship::SetTransitionActorList* cmdActor = (Ship::SetTransitionActorList*)cmd;

    globalCtx->transiActorCtx.numActors = cmdActor->entries.size();
    globalCtx->transiActorCtx.list = (TransitionActorEntry*)malloc(cmdActor->entries.size() * sizeof(TransitionActorEntry));

    for (int i = 0; i < cmdActor->entries.size(); i++)
    {
        globalCtx->transiActorCtx.list[i].sides[0].room = cmdActor->entries[i].frontObjectRoom;
        globalCtx->transiActorCtx.list[i].sides[0].effects = cmdActor->entries[i].frontTransitionReaction;
        globalCtx->transiActorCtx.list[i].sides[1].room = cmdActor->entries[i].backObjectRoom;
        globalCtx->transiActorCtx.list[i].sides[1].effects = cmdActor->entries[i].backTransitionReaction;
        globalCtx->transiActorCtx.list[i].id = cmdActor->entries[i].actorNum;
        globalCtx->transiActorCtx.list[i].pos.x = cmdActor->entries[i].posX;
        globalCtx->transiActorCtx.list[i].pos.y = cmdActor->entries[i].posY;
        globalCtx->transiActorCtx.list[i].pos.z = cmdActor->entries[i].posZ;
        globalCtx->transiActorCtx.list[i].rotY = cmdActor->entries[i].rotY;
        globalCtx->transiActorCtx.list[i].params = cmdActor->entries[i].initVar;
    }

    return false;
}

//void TransitionActor_InitContext(GameState* state, TransitionActorContext* transiActorCtx) {
//    transiActorCtx->numActors = 0;
//}

// Scene Command 0x0F: Light Setting List
bool func_80098CC8(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetLightingSettings* otrLight = (Ship::SetLightingSettings*)cmd;

    globalCtx->envCtx.numLightSettings = otrLight->settings.size();
    globalCtx->envCtx.lightSettingsList = (EnvLightSettings*)malloc(globalCtx->envCtx.numLightSettings * sizeof(EnvLightSettings));

    for (int i = 0; i < otrLight->settings.size(); i++)
    {
        globalCtx->envCtx.lightSettingsList[i].ambientColor[0] = otrLight->settings[i].ambientClrR;
        globalCtx->envCtx.lightSettingsList[i].ambientColor[1] = otrLight->settings[i].ambientClrG;
        globalCtx->envCtx.lightSettingsList[i].ambientColor[2] = otrLight->settings[i].ambientClrB;

        globalCtx->envCtx.lightSettingsList[i].light1Color[0] = otrLight->settings[i].diffuseClrA_R;
        globalCtx->envCtx.lightSettingsList[i].light1Color[1] = otrLight->settings[i].diffuseClrA_G;
        globalCtx->envCtx.lightSettingsList[i].light1Color[2] = otrLight->settings[i].diffuseClrA_B;

        globalCtx->envCtx.lightSettingsList[i].light1Dir[0] = otrLight->settings[i].diffuseDirA_X;
        globalCtx->envCtx.lightSettingsList[i].light1Dir[1] = otrLight->settings[i].diffuseDirA_Y;
        globalCtx->envCtx.lightSettingsList[i].light1Dir[2] = otrLight->settings[i].diffuseDirA_Z;

        globalCtx->envCtx.lightSettingsList[i].light2Color[0] = otrLight->settings[i].diffuseClrB_R;
        globalCtx->envCtx.lightSettingsList[i].light2Color[1] = otrLight->settings[i].diffuseClrB_G;
        globalCtx->envCtx.lightSettingsList[i].light2Color[2] = otrLight->settings[i].diffuseClrB_B;

        globalCtx->envCtx.lightSettingsList[i].light2Dir[0] = otrLight->settings[i].diffuseDirB_X;
        globalCtx->envCtx.lightSettingsList[i].light2Dir[1] = otrLight->settings[i].diffuseDirB_Y;
        globalCtx->envCtx.lightSettingsList[i].light2Dir[2] = otrLight->settings[i].diffuseDirB_Z;

        globalCtx->envCtx.lightSettingsList[i].fogColor[0] = otrLight->settings[i].fogClrR;
        globalCtx->envCtx.lightSettingsList[i].fogColor[1] = otrLight->settings[i].fogClrG;
        globalCtx->envCtx.lightSettingsList[i].fogColor[2] = otrLight->settings[i].fogClrB;

        globalCtx->envCtx.lightSettingsList[i].fogNear = otrLight->settings[i].fogNear;
        globalCtx->envCtx.lightSettingsList[i].fogFar = otrLight->settings[i].fogFar;
    }

    return false;
}

// Scene Command 0x11: Skybox Settings
bool func_80098D1C(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetSkyboxSettings* cmdSky = (Ship::SetSkyboxSettings*)cmd;

    globalCtx->skyboxId = cmdSky->skyboxNumber;
    globalCtx->envCtx.unk_17 = globalCtx->envCtx.unk_18 = cmdSky->cloudsType;
    globalCtx->envCtx.indoors = cmdSky->isIndoors;

    return false;
}

// Scene Command 0x12: Skybox Disables
bool func_80098D5C(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetSkyboxModifier* cmdSky = (Ship::SetSkyboxModifier*)cmd;

    globalCtx->envCtx.sunMoonDisabled = cmdSky->disableSunMoon;
    globalCtx->envCtx.skyboxDisabled = cmdSky->disableSky;

    return false;
}

// Scene Command 0x10: Time Settings
bool func_80098D80(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetTimeSettings* cmdTime = (Ship::SetTimeSettings*)cmd;

    if ((cmdTime->hour != 0xFF) && (cmdTime->min != 0xFF)) {
        gSaveContext.skyboxTime = gSaveContext.dayTime =
            ((cmdTime->hour + (cmdTime->min / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000);
    }

    if (cmdTime->unk != 0xFF) {
        globalCtx->envCtx.timeIncrement = cmdTime->unk;
    }
    else {
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
        }
        else if ((gSaveContext.skyboxTime >= 0x4555) && (gSaveContext.skyboxTime < 0x5556)) {
            gSaveContext.skyboxTime = 0x5556;
        }
        else if ((gSaveContext.skyboxTime >= 0xAAAB) && (gSaveContext.skyboxTime < 0xB556)) {
            gSaveContext.skyboxTime = 0xB556;
        }
        else if ((gSaveContext.skyboxTime >= 0xC001) && (gSaveContext.skyboxTime < 0xCAAC)) {
            gSaveContext.skyboxTime = 0xCAAC;
        }
    }

    return false;
}

// Scene Command 0x05: Wind Settings
bool func_80099090(GlobalContext* globalCtx, Ship::SceneCommand* cmd) {
    Ship::SetWind* cmdWind = (Ship::SetWind*)cmd;

    s8 x = cmdWind->windWest;
    s8 y = cmdWind->windVertical;
    s8 z = cmdWind->windSouth;

    globalCtx->envCtx.windDirection.x = x;
    globalCtx->envCtx.windDirection.y = y;
    globalCtx->envCtx.windDirection.z = z;

    globalCtx->envCtx.windSpeed = cmdWind->clothFlappingStrength;

    return false;
}

// Scene Command 0x13: Exit List
bool func_800990F0(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::ExitList* cmdExit = (Ship::ExitList*)cmd;

    globalCtx->setupExitList = (int16_t*)malloc(cmdExit->exits.size() * sizeof(int16_t));

    for (int i = 0; i < cmdExit->exits.size(); i++)
        globalCtx->setupExitList[i] = cmdExit->exits[i];

    return false;
}

// Scene Command 0x09: Undefined
bool func_80099134(GlobalContext* globalCtx, Ship::SceneCommand* cmd) {
    return false;
}

// Scene Command 0x15: Sound Settings
bool func_80099140(GlobalContext* globalCtx, Ship::SceneCommand* cmd) {
    Ship::SetSoundSettings* cmdSnd = (Ship::SetSoundSettings*)cmd;

    globalCtx->sequenceCtx.seqId = cmdSnd->musicSequence;
    globalCtx->sequenceCtx.natureAmbienceId = cmdSnd->nightTimeSFX;

    if (gSaveContext.seqId == 0xFF) {
        Audio_QueueSeqCmd(cmdSnd->reverb | 0xF0000000);
    }

    return false;
}

// Scene Command 0x16: Echo Setting
bool func_8009918C(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetEchoSettings* cmdEcho = (Ship::SetEchoSettings*)cmd;

    globalCtx->roomCtx.curRoom.echo = cmdEcho->echo;

    return false;
}

// Scene Command 0x18: Alternate Headers
bool func_800991A0(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetAlternateHeaders* cmdHeaders = (Ship::SetAlternateHeaders*)cmd;

    //s32 pad;
    //SceneCmd* altHeader;

    //osSyncPrintf("\n[ZU]sceneset age    =[%X]", ((void)0, gSaveContext.linkAge));
    //osSyncPrintf("\n[ZU]sceneset time   =[%X]", ((void)0, gSaveContext.cutsceneIndex));
    //osSyncPrintf("\n[ZU]sceneset counter=[%X]", ((void)0, gSaveContext.sceneSetupIndex));

    if (gSaveContext.sceneSetupIndex != 0)
    {
        std::string desiredHeader = cmdHeaders->headers[gSaveContext.sceneSetupIndex - 1];
        Ship::Scene* headerData = nullptr;
        if (desiredHeader != "") {
            headerData =
                (Ship::Scene*)OTRGlobals::Instance->context->GetResourceManager()->LoadResource(desiredHeader).get();
        }

        if (headerData != nullptr)
        {
            OTRScene_ExecuteCommands(globalCtx, headerData);
            return true;
        }
        else
        {
            // "Coughh! There is no specified dataaaaa!"
            osSyncPrintf("\nげぼはっ！ 指定されたデータがないでええっす！");

            if (gSaveContext.sceneSetupIndex == 3)
            {
                std::string desiredHeader = cmdHeaders->headers[gSaveContext.sceneSetupIndex - 2];
                Ship::Scene* headerData = nullptr;
                if (desiredHeader != "") {
                    headerData = (Ship::Scene*)OTRGlobals::Instance->context->GetResourceManager()
                                     ->LoadResource(desiredHeader)
                                     .get();
                }

                // "Using adult day data there!"
                osSyncPrintf("\nそこで、大人の昼データを使用するでええっす！！");

                if (headerData != nullptr)
                {
                    OTRScene_ExecuteCommands(globalCtx, headerData);
                    return true;
                }
            }
        }
    }
    return false;
}

// Scene Command 0x17: Cutscene Data
bool func_8009934C(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetCutscenes* cmdCS = (Ship::SetCutscenes*)cmd;

    Ship::Cutscene* csData = (Ship::Cutscene*)OTRGlobals::Instance->context->GetResourceManager()->LoadResource(cmdCS->cutscenePath).get();
    globalCtx->csCtx.segment = csData->commands.data();

    //osSyncPrintf("\ngame_play->demo_play.data=[%x]", globalCtx->csCtx.segment);
    return false;
}

// Scene Command 0x19: Misc. Settings (Camera & World Map Area)
bool func_800993C0(GlobalContext* globalCtx, Ship::SceneCommand* cmd)
{
    Ship::SetCameraSettings* cmdCam = (Ship::SetCameraSettings*)cmd;

    YREG(15) = cmdCam->cameraMovement;
    gSaveContext.worldMapArea = cmdCam->mapHighlights;

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
    return false;
}

bool (*sceneCommands[])(GlobalContext*, Ship::SceneCommand*) =
{
    func_80098508, func_800985DC, func_80098630, func_80098674, func_800987A4, func_80099090, func_800987F8,
    func_8009883C, func_80098904, func_80099134, func_80098958, func_8009899C, func_80098B74, func_80098C24,
    func_80098C68, func_80098CC8, func_80098D80, func_80098D1C, func_80098D5C, func_800990F0, 0,
    func_80099140, func_8009918C, func_8009934C, func_800991A0, func_800993C0,
};

s32 OTRScene_ExecuteCommands(GlobalContext* globalCtx, Ship::Scene* scene)
{
    Ship::SceneCommandID cmdCode;

    for (int i = 0; i < scene->commands.size(); i++)
    {
        auto sceneCmd = scene->commands[i];

        if (sceneCmd == nullptr) // UH OH
            continue;


        cmdCode = sceneCmd->cmdID;
        //osSyncPrintf("*** Scene_Word = { code=%d, data1=%02x, data2=%04x } ***\n", cmdCode, sceneCmd->base.data1, sceneCmd->base.data2);

        if ((int)cmdCode == 0x14) {
            break;
        }

        if ((int)cmdCode <= 0x19) {
            if (sceneCommands[(int)cmdCode](globalCtx, sceneCmd))
                break;
        }
        else {
            osSyncPrintf(VT_FGCOL(RED));
            osSyncPrintf("code の値が異常です\n"); // "code variable is abnormal"
            osSyncPrintf(VT_RST);
        }

        //sceneCmd++;
    }
    return 0;
}

extern "C" s32 OTRfunc_800973FC(GlobalContext* globalCtx, RoomContext* roomCtx) {
    if (roomCtx->status == 1) {
        //if (!osRecvMesg(&roomCtx->loadQueue, NULL, OS_MESG_NOBLOCK)) {
        if (1)
        {
            roomCtx->status = 0;
            roomCtx->curRoom.segment = roomCtx->unk_34;
            gSegments[3] = VIRTUAL_TO_PHYSICAL(roomCtx->unk_34);

            OTRScene_ExecuteCommands(globalCtx, roomCtx->roomToLoad);
            Player_SetBootData(globalCtx, GET_PLAYER(globalCtx));
            Actor_SpawnTransitionActors(globalCtx, &globalCtx->actorCtx);

            return 1;
        }

        return 0;
    }

    return 1;
}

extern "C" s32 OTRfunc_8009728C(GlobalContext* globalCtx, RoomContext* roomCtx, s32 roomNum) {
    u32 size;

    if (roomCtx->status == 0) {
        roomCtx->prevRoom = roomCtx->curRoom;
        roomCtx->curRoom.num = roomNum;
        roomCtx->curRoom.segment = NULL;
        roomCtx->status = 1;

        ASSERT(roomNum < globalCtx->numRooms, "read_room_ID < game_play->room_rom_address.num", "../z_room.c", 1009);

        if (roomNum >= globalCtx->numRooms)
            return 0; // UH OH

        size = globalCtx->roomList[roomNum].vromEnd - globalCtx->roomList[roomNum].vromStart;
        roomCtx->unk_34 = (void*)ALIGN16((u32)roomCtx->bufPtrs[roomCtx->unk_30] - ((size + 8) * roomCtx->unk_30 + 7));

        osCreateMesgQueue(&roomCtx->loadQueue, &roomCtx->loadMsg, 1);
        //DmaMgr_SendRequest2(&roomCtx->dmaRequest, roomCtx->unk_34, globalCtx->roomList[roomNum].vromStart, size, 0,
                            //&roomCtx->loadQueue, NULL, "../z_room.c", 1036);

        auto roomData = OTRGlobals::Instance->context->GetResourceManager()->LoadResource(globalCtx->roomList[roomNum].fileName);
        roomCtx->status = 1;
        roomCtx->roomToLoad = (Ship::Scene*)roomData.get();

        roomCtx->unk_30 ^= 1;

        return 1;
    }

    return 0;
}
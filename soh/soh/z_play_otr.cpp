#include "OTRGlobals.h"
#include <libultraship/ResourceMgr.h>
#include <libultraship/Scene.h>
#include <Utils/StringHelper.h>
#include "global.h"
#include "vt.h"
#include <libultraship/Vertex.h>

extern "C" void Gameplay_InitScene(GlobalContext * globalCtx, s32 spawn);
extern "C" void Gameplay_InitEnvironment(GlobalContext * globalCtx, s16 skyboxId);
void OTRGameplay_InitScene(GlobalContext* globalCtx, s32 spawn);
s32 OTRScene_ExecuteCommands(GlobalContext* globalCtx, Ship::Scene* sceneCmd);

//Ship::OTRResource* OTRGameplay_LoadFile(GlobalContext* globalCtx, RomFile* file) {
Ship::Resource* OTRGameplay_LoadFile(GlobalContext* globalCtx, const char* fileName)
{
    auto res = OTRGlobals::Instance->context->GetResourceManager()->LoadResource(fileName);
    return res.get();
}

extern "C" void OTRGameplay_SpawnScene(GlobalContext* globalCtx, s32 sceneNum, s32 spawn) {
    SceneTableEntry* scene = &gSceneTable[sceneNum];

    scene->unk_13 = 0;
    globalCtx->loadedScene = scene;
    globalCtx->sceneNum = sceneNum;
    globalCtx->sceneConfig = scene->config;

    //osSyncPrintf("\nSCENE SIZE %fK\n", (scene->sceneFile.vromEnd - scene->sceneFile.vromStart) / 1024.0f);

    std::string sceneVersion;
    if (IsGameMasterQuest()) {
        sceneVersion = "mq";
    } else {
        sceneVersion = "nonmq";
    }
    std::string scenePath = StringHelper::Sprintf("scenes/%s/%s/%s", sceneVersion.c_str(), scene->sceneFile.fileName, scene->sceneFile.fileName);

    globalCtx->sceneSegment = (Ship::Scene*)OTRGameplay_LoadFile(globalCtx, scenePath.c_str());

    // Failed to load scene... default to doodongs cavern
    if (globalCtx->sceneSegment == nullptr) 
    {
        lusprintf(__FILE__, __LINE__, 2, "Unable to load scene %s... Defaulting to Doodong's Cavern!\n",
                  scenePath.c_str());
        OTRGameplay_SpawnScene(globalCtx, 0x01, 0);
        return;
    }

    scene->unk_13 = 0;

    //ASSERT(globalCtx->sceneSegment != NULL);
    //gSegments[2] = VIRTUAL_TO_PHYSICAL(globalCtx->sceneSegment);

    OTRGameplay_InitScene(globalCtx, spawn);

    osSyncPrintf("ROOM SIZE=%fK\n", func_80096FE8(globalCtx, &globalCtx->roomCtx) / 1024.0f);
}

void OTRGameplay_InitScene(GlobalContext* globalCtx, s32 spawn) {
    globalCtx->curSpawn = spawn;
    globalCtx->linkActorEntry = nullptr;
    globalCtx->unk_11DFC = nullptr;
    globalCtx->setupEntranceList = nullptr;
    globalCtx->setupExitList = nullptr;
    globalCtx->cUpElfMsgs = nullptr;
    globalCtx->setupPathList = nullptr;
    globalCtx->numSetupActors = 0;
    Object_InitBank(globalCtx, &globalCtx->objectCtx);
    LightContext_Init(globalCtx, &globalCtx->lightCtx);
    TransitionActor_InitContext(&globalCtx->state, &globalCtx->transiActorCtx);
    func_80096FD4(globalCtx, &globalCtx->roomCtx.curRoom);
    YREG(15) = 0;
    gSaveContext.worldMapArea = 0;
    OTRScene_ExecuteCommands(globalCtx, globalCtx->sceneSegment);
    Gameplay_InitEnvironment(globalCtx, globalCtx->skyboxId);
    /* auto data = static_cast<Ship::Vertex*>(Ship::Window::GetInstance()
                                               ->GetResourceManager()
                                               ->LoadResource("object_link_child\\object_link_childVtx_01FE08")
                                               .get());

    auto data2 = ResourceMgr_LoadVtxByCRC(0x68d4ea06044e228f);*/

    volatile int a = 0;
}

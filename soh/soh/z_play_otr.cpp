#include "OTRGlobals.h"
#include <ResourceManager.h>
#include "soh/resource/type/Scene.h"
#include <Utils/StringHelper.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "global.h"
#include "vt.h"
#include <Vertex.h>

extern "C" void Play_InitScene(PlayState * play, s32 spawn);
extern "C" void Play_InitEnvironment(PlayState * play, s16 skyboxId);
void OTRPlay_InitScene(PlayState* play, s32 spawn);
s32 OTRScene_ExecuteCommands(PlayState* play, Ship::Scene* scene);

//Ship::OTRResource* OTRPlay_LoadFile(PlayState* play, RomFile* file) {
Ship::Resource* OTRPlay_LoadFile(PlayState* play, const char* fileName)
{
    auto res = OTRGlobals::Instance->context->GetResourceManager()->LoadResource(fileName);
    return res.get();
}

extern "C" void OTRPlay_SpawnScene(PlayState* play, s32 sceneNum, s32 spawn) {
    SceneTableEntry* scene = &gSceneTable[sceneNum];

    scene->unk_13 = 0;
    play->loadedScene = scene;
    play->sceneNum = sceneNum;
    play->sceneConfig = scene->config;

    //osSyncPrintf("\nSCENE SIZE %fK\n", (scene->sceneFile.vromEnd - scene->sceneFile.vromStart) / 1024.0f);

    std::string sceneVersion;
    if (IsGameMasterQuest()) {
        sceneVersion = "mq";
    } else {
        sceneVersion = "nonmq";
    }
    std::string scenePath = StringHelper::Sprintf("scenes/%s/%s/%s", sceneVersion.c_str(), scene->sceneFile.fileName, scene->sceneFile.fileName);

    play->sceneSegment = OTRPlay_LoadFile(play, scenePath.c_str());

    // Failed to load scene... default to doodongs cavern
    if (play->sceneSegment == nullptr) 
    {
        lusprintf(__FILE__, __LINE__, 2, "Unable to load scene %s... Defaulting to Doodong's Cavern!\n",
                  scenePath.c_str());
        OTRPlay_SpawnScene(play, 0x01, 0);
        return;
    }

    scene->unk_13 = 0;

    //ASSERT(play->sceneSegment != NULL);
    //gSegments[2] = VIRTUAL_TO_PHYSICAL(play->sceneSegment);

    OTRPlay_InitScene(play, spawn);
    auto roomSize = func_80096FE8(play, &play->roomCtx);

    osSyncPrintf("ROOM SIZE=%fK\n", roomSize / 1024.0f);
}

void OTRPlay_InitScene(PlayState* play, s32 spawn) {
    play->curSpawn = spawn;
    play->linkActorEntry = nullptr;
    play->unk_11DFC = nullptr;
    play->setupEntranceList = nullptr;
    play->setupExitList = nullptr;
    play->cUpElfMsgs = nullptr;
    play->setupPathList = nullptr;
    play->numSetupActors = 0;
    Object_InitBank(play, &play->objectCtx);
    LightContext_Init(play, &play->lightCtx);
    TransitionActor_InitContext(&play->state, &play->transiActorCtx);
    func_80096FD4(play, &play->roomCtx.curRoom);
    YREG(15) = 0;
    gSaveContext.worldMapArea = 0;
    OTRScene_ExecuteCommands(play, (Ship::Scene*)play->sceneSegment);
    Play_InitEnvironment(play, play->skyboxId);
    /* auto data = static_cast<Ship::Vertex*>(Ship::Window::GetInstance()
                                               ->GetResourceManager()
                                               ->LoadResource("object_link_child\\object_link_childVtx_01FE08")
                                               .get());

    auto data2 = ResourceMgr_LoadVtxByCRC(0x68d4ea06044e228f);*/
    
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(play->sceneNum);

    volatile int a = 0;
}
